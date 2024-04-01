//
// Created by ggam-nyang on 3/9/24.
//

#include "Lobby.hpp"

int Lobby::ID_COUNTER = 0;

Lobby::Lobby(int id) : id_(id) {}

std::shared_ptr<Lobby> Lobby::create() {
  return std::make_shared<Lobby>(++ID_COUNTER);
}

void Lobby::Enter(Session::pointer session) {
  clients_.push_back(session);
}

std::vector<string> Lobby::GetRoomList() const {
  std::vector<string> room_list{};
  room_list.reserve(rooms_.size());
  for (const auto& [room_id, room] : rooms_) {
    room_list.push_back(room->name_);
  }

  return room_list;
}

std::vector<std::shared_ptr<Room>> Lobby::GetRooms() const {
  std::vector<std::shared_ptr<Room>> roomList{};
  roomList.reserve(rooms_.size());
  for (const auto& [room_id, room] : rooms_) {
    roomList.push_back(room);
  }

  return roomList;
}

std::shared_ptr<Room> Lobby::getRoom(int room_id) const {

  return rooms_.at(room_id);
}

int Lobby::CreateRoom(Session::pointer session, string room_name) {
  if (rooms_.size() >= MAX_ROOM_SIZE) {
    return 2;
  }

  auto room = Room::create(this, session, room_name);
  rooms_[room->id_] = room;
  return 1;
}

int Lobby::EnterRoom(Session::pointer session, int room_id) {
  if (!rooms_.contains(room_id)) {
    return 4;
  }

  auto room = rooms_.at(room_id);
  room->Enter(session);

  return 1;
}

void Lobby::Broadcast(char* packet, uint16_t copySize, Session::pointer sender,
                      bool isExceptRoom, bool isExceptMe) {
  for (const auto& client : clients_) {
    if (isExceptRoom && client->IsInRoom()) continue;
    if (isExceptMe && client->id_ == sender->id_) continue;

    client->write(packet, copySize);
  }
}