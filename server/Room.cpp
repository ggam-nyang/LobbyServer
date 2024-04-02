//
// Created by ggam-nyang on 3/9/24.
//

#include "Room.hpp"
#include <iostream>
#include <utility>
#include "../BattleInfo//BattleManager.hpp"
#include "Lobby.hpp"

int Room::ID_COUNTER = 0;

Room::Room(Lobby* lobby) : lobby_(lobby), battleManager_() {}

std::shared_ptr<Room> Room::create(Lobby* lobby, Session::pointer owner,
                                   std::string room_name) {
  ++ID_COUNTER;
  const std::shared_ptr<Room>& room = std::make_shared<Room>(lobby);
  room->owner_ = std::move(owner);
  room->name_ = std::move(room_name);
  return room;
}

int Room::Enter(Session::pointer session) {
  if (clients_.size() >= MAX_CLIENT_SIZE) {
    return 3;
  }

  clients_.push_back(session);
  session->setRoom(shared_from_this());
  return 1;
}

int Room::Leave(Session::pointer session) {
//  clients_.remove(session);
  session->setRoom(nullptr);  // FIXME: Room에서 Session->room을 설정하는 것이 불편..

  return 1;
}

bool Room::IsOwner(Session::pointer session) {
  return owner_->id_ == session->id_;
}

void Room::Broadcast(char* packet, uint16_t copySize, Session::pointer sender,
                     bool isExceptMe) {
  for (const auto& client : clients_) {
    if (isExceptMe && client->id_ == sender->id_) {
      continue;
    }
    client->write(packet, copySize);
  }
}

void Room::Broadcast(char* packet, uint16_t copySize, Session::pointer sender,
                     std::function<bool(Session*)> condition, bool isExceptMe) {
  for (const auto& client : clients_) {
    if (isExceptMe && client->id_ == sender->id_)
      continue;
    if (condition(client.get()))
      continue;
    client->write(packet, copySize);
  }
}

bool Room::IsReady() {
  for (const auto& client : clients_) {
    if (!client->IsReady()) {
      return false;
    }
  }

  return true;
}

void Room::BattleStart() {}

int Room::GetClientSize() {
  return clients_.size();
}

void Room::Attack(Session::pointer session) {
  for (const auto& client : clients_) {
    if (client->id_ == session->id_) {
      continue;
    }

    battleManager_->Attack(session->battleInfo_, client->battleInfo_);
  }
}

std::vector<std::shared_ptr<Session>> Room::GetClients() {
  return clients_;
}
