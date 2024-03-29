//
// Created by ggam-nyang on 3/9/24.
//

#include "Room.hpp"
#include <iostream>
#include "Lobby.hpp"

int Room::ID_COUNTER = 0;

Room::Room(Lobby* lobby) : lobby_(lobby) {}

std::shared_ptr<Room> Room::create(Lobby* lobby, Session::pointer owner) {
  ++ID_COUNTER;
  const std::shared_ptr<Room>& room = std::make_shared<Room>(lobby);
  room->owner_ = owner;
  return room;
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
  string message = "[ " + session->name_ + " ] " + " Enter Room";
  auto protocol = Protocol::create(ProtocolType::ALERT, message);
  WriteAll(protocol, session, false);
}

void Room::Leave(Session::pointer session) {
  clients_.remove(session);
  session->setRoom(nullptr);  // FIXME: Room에서 Session->room을 설정하는 것이 불편..

  string message = "[ " + session->name_ + " ] " + " Leave Room";
  auto protocol = Protocol::create(ProtocolType::ALERT, message);
  WriteAll(protocol, session);

  //    if (clients_.empty()) {
  //        lobby_->RemoveRoom(shared_from_this());
  //    }
}

bool Room::IsOwner(Session::pointer session) {
  return owner_ == session;
}
