//
// Created by ggam-nyang on 2/26/24.
//

#include "Session.hpp"
#include <numeric>
#include <ranges>
#include "Lobby.hpp"
#include "Room.hpp"
#include "Server.h"

Session::pointer Session::create(boost::asio::io_context& io_context,
                                 Server* server) {
  return std::make_shared<Session>(io_context, server);
}

Session::Session(boost::asio::io_context& io_context, Server* server)
    : socket_(io_context), server_(server) {}

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
  std::cout << "[" << boost::this_thread::get_id() << "] "
            << protocol->getBody() << std::endl;

  read();
}

void Session::protocolManage(ProtocolPtr& protocolPtr) {
  Protocol::Header header = protocolPtr->getHeader();
  string body = protocolPtr->getBody();

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
    case ENTER_ROOM:
      break;
    case CREATE_ROOM:
      break;
    case ROOM_LIST:
      RoomList(body);
      break;

    default:
      break;
  }
}

void Session::write(ProtocolPtr& protocolPtr) {
  // 이 부분도 동일합니다. protocolPtr을 shared_ptr로 바꾸고, 이에 대한 capture를 통해 life-cycle을 확보해주셔야 합니다
  socket_.async_write_some(
      boost::asio::buffer(protocolPtr->encode()),
      [this, protocolPtr](const boost::system::error_code& ec, size_t _) {
        onWrite(ec);
      });
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
  std::string writeBuffer;
  if (server_->isValidId(body)) {
    id = body;
    writeBuffer = "set [" + id + "] success!";
  } else {
    writeBuffer = body + "는 이미 사용중인 id 입니다.";
  }

  ProtocolPtr alert = Protocol::create(ProtocolType::ALERT, writeBuffer);
  write(alert);

  std::string enterWriteBuffer = "[" + id + "] enter";
  ProtocolPtr alertAll =
      Protocol::create(ProtocolType::ALERT, enterWriteBuffer);
  writeAll(alertAll, false);
}

void Session::chat(std::string& body) {
  auto writeBuffer = "[" + id + "] : " + body;
  ProtocolPtr chat = Protocol::create(ProtocolType::CHAT, writeBuffer);
  writeAll(chat);
}

void Session::alert(std::string& body) {
  auto const& writeBuffer = body;
  ProtocolPtr alert = Protocol::create(ProtocolType::ALERT, writeBuffer);
  write(alert);
}

void Session::RoomList(string& body) {
  std::vector<int> room_list = lobby_->getRoomList(shared_from_this());
  auto const& writeBuffer = std::accumulate(
      room_list.begin() + 1, room_list.end(), std::to_string(room_list[0]),
      [](const std::string& a, int b) { return a + ", " + std::to_string(b); });

  ProtocolPtr protocol = Protocol::create(ProtocolType::ROOM_LIST, writeBuffer);
  write(protocol);
}

void Session::EnterLobby(std::shared_ptr<Lobby> lobby) {
  lobby_ = lobby;
  lobby_->Enter(shared_from_this());
}

void Session::close() {
  socket_.close();
  shared_from_this().reset();
}
