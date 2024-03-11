//
// Created by ggam-nyang on 3/9/24.
//

#ifndef LOBBYSERVER_ROOM_HPP
#define LOBBYSERVER_ROOM_HPP

#include "Session.hpp"

class Lobby;

class Room {
  int id_;
  Lobby* lobby_;
  static int ID_COUNTER;

 public:
  Room(int id, Lobby* lobby);
  static std::shared_ptr<Room> create(Lobby* lobby);
};

#endif  //LOBBYSERVER_ROOM_HPP
