//
// Created by ggam-nyang on 2/26/24.
//

#include "Session.hpp"
#include "Server.h"

Session::pointer Session::create(boost::asio::io_context& io_context, Server* server) {
  return std::make_shared<Session>(io_context, server);
}

Session::Session(boost::asio::io_context& io_context, Server* server) : socket_(io_context), server_(server) {}

void Session::read() {
  boost::system::error_code ec;
  size_t size;
  size = socket_.read_some(boost::asio::buffer(buffer, buffer.size()), ec);

  if (ec) {
    std::cout << "[" << boost::this_thread::get_id()
              << "] read failed: " << ec.message() << std::endl;
    server_->closeSession(shared_from_this());
    return;
  }

  if (size == 0) {
    std::cout << "[" << boost::this_thread::get_id() << "] user wants to end "
              << std::endl;
    close();
    return;
  }

  buffer[size] = '\0';
  readBuffer = string(buffer.begin(), buffer.begin() + size);
  ProtocolPtr protocol = Protocol::create(readBuffer);

  protocolManage(protocol);
  std::cout << "[" << boost::this_thread::get_id() << "] " << readBuffer
            << std::endl;

  read();
}

void Session::protocolManage(ProtocolPtr& protocolPtr) {
  Protocol::Header header = protocolPtr->getHeader();
  string body = protocolPtr->getBody().data();

  switch (header.type) {
    using enum ProtocolType;

    case SET_ID:
      setId(body);
      break;
    case CHAT:
      chat(body);
      break;
    case ALERT:
//      alert(body);
      break;
    default:

      break;
  }
}

void Session::write(ProtocolPtr& protocolPtr) {
  socket_.async_write_some(
      boost::asio::buffer(protocolPtr->encode()),
      [this](const boost::system::error_code& ec, size_t _) { onWrite(ec); });
}

void Session::onWrite(const boost::system::error_code& ec) {
  if (ec) {
    std::cout << "[" << boost::this_thread::get_id()
              << "] async_write_some failed: " << ec.message() << std::endl;
    return;
  }
}

void Session::writeAll(ProtocolPtr& protocolPtr, bool isExceptMe) {
  server_->writeAll(protocolPtr, shared_from_this(), isExceptMe);
}

void Session::setId(std::string& body) {
  if (server_->isValidId(body)) {
    id = body;
    writeBuffer = "set [" + id + "] success!";
  } else {
    writeBuffer = body + "는 이미 사용중인 id 입니다.";
  }

  ProtocolPtr alert = Protocol::create(ProtocolType::ALERT, writeBuffer);
  write(alert);

  writeBuffer = "[" + id + "] 님이 로비에 입장하였습니다";
  ProtocolPtr alertAll = Protocol::create(ProtocolType::ALERT, writeBuffer);
  writeAll(alertAll, false);
}

void Session::chat(std::string& body) {
    writeBuffer = "[" + id + "] : " + body;
    ProtocolPtr chat = Protocol::create(ProtocolType::CHAT, writeBuffer);
    writeAll(chat);
}

void Session::alert(std::string& body) {
    writeBuffer = body;
    ProtocolPtr alert = Protocol::create(ProtocolType::ALERT, writeBuffer);
    write(alert);
}

void Session::close() {
  socket_.close();
  shared_from_this().reset();
}
