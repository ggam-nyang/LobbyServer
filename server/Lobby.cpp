//
// Created by ggam-nyang on 3/9/24.
//

#include "Lobby.hpp"

int Lobby::ID_COUNTER = 1;

Lobby::Lobby(int id) : id_(id) {}

std::shared_ptr<Lobby> Lobby::create() {
  return std::make_shared<Lobby>(++ID_COUNTER);
}

void Lobby::Enter(Session::pointer session) {
  clients_.push_back(session);

  string message = "[ " + session->name_ + " ] " + " Enter Lobby";
  auto protocol = Protocol::create(ProtocolType::ALERT, message);

  WriteAll(protocol, session, false);
}

std::vector<int> Lobby::getRoomList(Session::pointer session) const {
  std::vector<int> room_list{};
  for (const auto& [room_id, room] : rooms_) {
    room_list.push_back(room_id);
  }

  return room_list;
}

std::shared_ptr<Room> Lobby::getRoom(int room_id) const {


  return rooms_.at(room_id);
}


void Lobby::WriteAll(ProtocolPtr& protocolPtr, Session::pointer session,
                     bool isExceptMe) {
  for (const auto& client : clients_) {
    if (client->IsInRoom()) {
      continue;
    }
    if ((isExceptMe && client == session)) {
      continue;
    }
    client->write(protocolPtr);
  }
}

void Lobby::CreateRoom(Session::pointer session) {
    if (rooms_.size() >= MAX_ROOM_SIZE) {
        string message = "Room is full";
        auto protocol = Protocol::create(ProtocolType::ALERT, message);
        session->write(protocol);
        return;
    }

    auto room = Room::create(this);
    rooms_[room->id_] = room;

    string message = "[ " + session->name_ + " ] " + " Create Room name_: " + std::to_string(room->id_);
    auto protocol = Protocol::create(ProtocolType::ALERT, message);

    WriteAll(protocol, session, false);
}

void Lobby::EnterRoom(Session::pointer session, int room_id) {
    if (!rooms_.contains(room_id)) {
        string message = "Room name_: " + std::to_string(room_id) + " is not exist";
        auto protocol = Protocol::create(ProtocolType::ALERT, message);
        return session->write(protocol);
    }

    auto room = rooms_.at(room_id);
    room->Enter(session);
}
