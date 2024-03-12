//
// Created by ggam-nyang on 3/9/24.
//

#include "Room.hpp"
#include <iostream>
#include "Lobby.hpp"

int Room::ID_COUNTER = 0;

Room::Room(int id, Lobby* lobby) : id_(id), lobby_(lobby) {}

std::shared_ptr<Room> Room::create(Lobby* lobby) {
  ++ID_COUNTER;
  return std::make_shared<Room>(ID_COUNTER, lobby);
}

void Room::WriteAll(ProtocolPtr& protocolPtr, Session::pointer session,
                    bool isExceptMe) {
  for (const auto& client : clients_) {
    if (isExceptMe && client == session) {
      continue;
    }
    client->write(protocolPtr);
  }
}

void Room::Enter(Session::pointer session) {
  if (clients_.size() >= MAX_CLIENT_SIZE) {
    string message = "Room is full!";
    auto protocol = Protocol::create(ProtocolType::ALERT, message);
    session->write(protocol);
    return;
  }

  clients_.push_back(session);
  session->setRoom(shared_from_this());
  string message = "[ " + session->id + " ] " + " Enter Room";
  auto protocol = Protocol::create(ProtocolType::ALERT, message);
  WriteAll(protocol, session, false);
}

void Room::Leave(Session::pointer session) {
  clients_.remove(session);
  session->setRoom(nullptr);  // FIXME: Room에서 Session->room을 설정하는 것이 불편..

  string message = "[ " + session->id + " ] " + " Leave Room";
  auto protocol = Protocol::create(ProtocolType::ALERT, message);
  WriteAll(protocol, session);

  //    if (clients_.empty()) {
  //        lobby_->RemoveRoom(shared_from_this());
  //    }
}
