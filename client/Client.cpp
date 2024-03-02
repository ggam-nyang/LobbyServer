//
// Created by ggam-nyang on 2/16/24.
//

#include "Client.h"
#include "../server/Protocol.h"

Client::Client(std::string ip_address, unsigned short port_num)
    : ep(asio::ip::address::from_string(ip_address), port_num),
      sock(ios, ep.protocol()),
      work(new asio::io_service::work(ios)) {}

void Client::Start() {
  for (int i = 0; i < 3; i++)
    thread_group.create_thread([this]() { WorkerThread(); });

  // thread 잘 만들어질때까지 잠시 기다리는 부분
  this_thread::sleep_for(chrono::milliseconds(100));

  //    ios.post(bind(&Client::TryConnect, this));
  ios.post([this]() { TryConnect(); });

  thread_group.join_all();
}

void Client::WorkerThread() {
  lock.lock();
  cout << "[" << std::this_thread::get_id() << "]"
       << " Thread start" << endl;
  lock.unlock();

  ios.run();

   lock.lock();
  cout << "[" << std::this_thread::get_id() << "]"
       << " Thread End" << endl;
  lock.unlock();
}

void Client::TryConnect() {
  cout << "[" << std::this_thread::get_id() << "]"
       << " TryConnect" << endl;

  sock.async_connect(ep, boost::bind(&Client::OnConnect, this, _1));
}

void Client::OnConnect(const system::error_code& ec) {
  cout << "[" << std::this_thread::get_id() << "]"
       << " OnConnect" << endl;
  if (ec) {
    cout << "connect failed: " << ec.message() << endl;
    StopAll();
    return;
  }

  ios.post([this]() { Send(); });
  ios.post([this]() { Receive(); });
}

void Client::Send() {
  ProtocolPtr temp;
  getline(std::cin, writeBuffer);

  if (writeBuffer[0] == ':') {
    temp = Protocol::create(ProtocolType::SET_ID, writeBuffer);
    isSetId = true;
  } else {
    temp = Protocol::create(ProtocolType::CHAT, writeBuffer);
  }

  sock.async_write_some(
      asio::buffer(temp->encode()),
      [this](const system::error_code& ec, size_t) { SendHandle(ec); });
}

void Client::Receive() {
  sock.async_read_some(asio::buffer(buffer, buffer.size()),
                       [this](const system::error_code& ec, size_t size) {
                         ReceiveHandle(ec, size);
                       });
}

void Client::SendHandle(const system::error_code& ec) {
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

  buffer[size] = '\0';
  readBuffer = std::string(buffer.begin(), buffer.begin() + size);
  ProtocolPtr temp = Protocol::create(readBuffer);

//  std::lock_guard<std::mutex> lock_guard(mutex_);
//  cout << temp->getBody() << endl;
  lock.lock();
  cout << temp->getBody() << endl;
  lock.unlock();

  Receive();
}

void Client::StopAll() {
  sock.close();
  work.reset();
}