//
// Created by ggam-nyang on 3/9/24.
//

#include "Room.hpp"
#include "Lobby.hpp"

int Room::ID_COUNTER = 1;

Room::Room(int id, Lobby* lobby) : id_(id), lobby_(lobby) {}

std::shared_ptr<Room> Room::create(Lobby* lobby) {
    return std::make_shared<Room>(++ID_COUNTER, lobby);
}

