//
// Created by ggam-nyang on 2/16/24.
//

#include "Client.h"
#include "../Protocol/Packet/Packet.hpp"
#include "../Protocol/Protocol.h"
#include "ClientPacketManager.hpp"

Client::Client(std::string ip_address, unsigned short port_num)
    : endpoint_(asio::ip::address::from_string(ip_address), port_num),
      packetManager_(this),
      sock(io_context_, endpoint_.protocol()),
      work_(new asio::io_service::work(io_context_)) {}

void Client::Start() {
  for (int i = 0; i < 3; i++)
    thread_group_.create_thread([this]() { WorkerThread(); });

  // thread 잘 만들어질때까지 잠시 기다리는 부분
  this_thread::sleep_for(chrono::milliseconds(100));

  // io_context_.post(bind(&Client::TryConnect, this));
  io_context_.post([this]() { TryConnect(); });

  thread_group_.join_all();
}

void Client::WorkerThread() {
  lock_.lock();
  cout << "[" << std::this_thread::get_id() << "]"
       << " Thread start" << endl;
  lock_.unlock();

  io_context_.run();

  lock_.lock();
  cout << "[" << std::this_thread::get_id() << "]"
       << " Thread End" << endl;
  lock_.unlock();
}

void Client::TryConnect() {
  cout << "[" << std::this_thread::get_id() << "]"
       << " TryConnect" << endl;

  sock.async_connect(endpoint_, boost::bind(&Client::OnConnect, this, _1));
}

void Client::OnConnect(const system::error_code& ec) {
  cout << "[" << std::this_thread::get_id() << "]"
       << " OnConnect" << endl;
  if (ec) {
    cout << "connect failed: " << ec.message() << endl;
    StopAll();
    return;
  }

  io_context_.post([this]() { Send(); });
  io_context_.post([this]() { Receive(); });
}

void Client::Send() {
  cout << "Client Send: " << endl;
  getline(std::cin, writeBuffer_);

  packetManager_.SendPacket(this, writeBuffer_);
}

void Client::Receive() {

  sock.async_read_some(asio::buffer(buffer_, buffer_.size()),
                       [this](const system::error_code& ec, size_t size) {
                         ReceiveHandle(ec, size);
                       });
}

void Client::SendHandle(const system::error_code& ec, const char* packet) {
  if (ec) {
    cout << "async_write_some error: " << ec.message() << endl;
    StopAll();
    return;
  }

  Send();
}

void Client::ReceiveHandle(const system::error_code& ec, size_t size) {
  if (ec) {
    cout << "async_read_some error: " << ec.message() << endl;
    StopAll();
    return;
  }

  if (size == 0) {
    cout << "Server wants to close this session" << endl;
    StopAll();
    return;
  }

  readBuffer_ = std::string(buffer_.data(), size);

  packetManager_.ReceivePacket(this, buffer_.data(), size);
  Receive();
}

void Client::StopAll() {
  sock.close();
  work_.reset();
}

void Client::ResponseSetName(SET_NAME_RESPONSE_PACKET packet) {
  if (packet.result) {
    cout << "Set name success" << endl;
  } else {
    cout << "Set name failed" << endl;
  }
}
