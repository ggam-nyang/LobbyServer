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
  explicit Room(Lobby* lobby);
  static std::shared_ptr<Room> create(Lobby* lobby, Session::pointer owner);

  void WriteAll(ProtocolPtr& protocolPtr, Session::pointer session,
                bool isExceptMe = true);
  void Enter(Session::pointer session);
  void Leave(Session::pointer session);
  bool IsOwner(Session::pointer session);
};

#endif  //LOBBYSERVER_ROOM_HPP
