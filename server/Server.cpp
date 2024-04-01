//
// Created by ggam-nyang on 2/22/24.
//

#include "Server.h"
#include "Lobby.hpp"

Server::Server(const string& ip_address, unsigned short port_num)
    : work_(new asio::io_service::work(io_context_)),
      endpoint_(asio::ip::address::from_string(ip_address), port_num),
      acceptor_(io_context_, endpoint_.protocol()) {
  const std::shared_ptr<Lobby>& lobby = Lobby::create();
  lobbies_[lobby->getId()] = lobby;
}

void Server::start() {
  cout << "start Server" << endl;
  cout << "Creating Threads" << endl;
  for (int i = 0; i < THREAD_SIZE; i++)
    threadGroup_.create_thread([this] { workerThread(); });

  this_thread::sleep_for(chrono::milliseconds(100));
  cout << "Threads Created" << endl;

  io_context_.post([this] { openAcceptor(); });

  threadGroup_.join_all();
}

void Server::workerThread() {
  lock_.lock();
  cout << "[" << boost::this_thread::get_id() << "]"
       << " Thread start" << endl;
  lock_.unlock();

  io_context_.run();

  lock_.lock();
  cout << "[" << boost::this_thread::get_id() << "]"
       << " Thread End" << endl;
  lock_.unlock();
}

void Server::openAcceptor() {
  system::error_code ec;
  ec = acceptor_.bind(endpoint_, ec);
  if (ec) {
    cout << "bind failed: " << ec.message() << endl;
    return;
  }

  acceptor_.listen();
  cout << "Acceptor Opened" << endl;

  startAccept();
  cout << "[" << boost::this_thread::get_id() << "]"
       << " start Accepting" << endl;
}

void Server::startAccept() {
  Session::pointer session = Session::create(io_context_, this);

  acceptor_.async_accept(
      session->socket_,
      [this, session](const system::error_code& ec) { onAccept(ec, session); });
}

void Server::onAccept(const system::error_code& ec,
                      Session::pointer session) {
  if (ec) {
    cout << "accept failed: " << ec.message() << endl;
    return;
  }

  lock_.lock();
  sessions_.push_back(session);
  cout << "[" << boost::this_thread::get_id() << "]"
       << " Client Accepted" << endl;
  lock_.unlock();

  io_context_.post([this, session] { read(session); });
  startAccept();
}

void Server::read(Session::pointer session) {
  session->Receive();
}

bool Server::isValidName(string& name) {
  // 중복된 이름인지 체크
  for (const auto& session : sessions_) {
    if (name == session->name_) {
      return false;
    }
  }
  return true;
}

void Server::closeSession(Session::pointer session) {
  session->close();

  // FIXME: 세션 종료 메세지?

  for (int i = 0; i < sessions_.size(); i++) {
    if (&sessions_[i]->socket_ == &session->socket_) {
      lock_.lock();
      sessions_.erase(sessions_.begin() + i);
      lock_.unlock();
      break;
    }
  }
}