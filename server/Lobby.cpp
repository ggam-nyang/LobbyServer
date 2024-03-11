//
// Created by ggam-nyang on 3/9/24.
//

#include "Lobby.hpp"

int Lobby::ID_COUNTER = 1;

Lobby::Lobby(int id) : id_(id) {
  auto room = Room::create(this);
  rooms_.insert(std::make_pair(1, room));
}

std::shared_ptr<Lobby> Lobby::create() {
  return std::make_shared<Lobby>(++ID_COUNTER);
}

void Lobby::Enter(Session::pointer session) {
  clients_.push_back(session);

  string message = session->id + " Enter lobby";
  auto protocol = Protocol::create(ProtocolType::ALERT, message);
  for (const auto& client : clients_) {
    client->write(protocol);
  }
}

std::vector<int> Lobby::getRoomList(Session::pointer session) const {
  std::vector<int> room_list{};
  for (const auto& [room_id, room] : rooms_) {
    room_list.push_back(room_id);
  }

  return room_list;
}
