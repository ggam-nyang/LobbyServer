//
// Created by ggam-nyang on 3/9/24.
//

#ifndef LOBBYSERVER_ROOM_HPP
#define LOBBYSERVER_ROOM_HPP

#include "Session.hpp"

class Lobby;

class Room : public std::enable_shared_from_this<Room> {
  Lobby* lobby_;
  std::list<Session::pointer> clients_;
  Session::pointer owner_;
  static int ID_COUNTER;
  static const int MAX_CLIENT_SIZE = 4;

 public:
  const int id_ = ID_COUNTER;
  std::string name_;
  explicit Room(Lobby* lobby);
  static std::shared_ptr<Room> create(Lobby* lobby, Session::pointer owner, std::string room_name);

  void Broadcast(char* packet, uint16_t copySize, Session::pointer sender, bool isExceptMe = true);
  int Enter(Session::pointer session);
  int Leave(Session::pointer session);
  bool IsOwner(Session::pointer session);
};

#endif  //LOBBYSERVER_ROOM_HPP
