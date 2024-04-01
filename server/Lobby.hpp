//
// Created by ggam-nyang on 3/9/24.
//

#ifndef LOBBYSERVER_LOBBY_HPP
#define LOBBYSERVER_LOBBY_HPP

#include <string>
#include <unordered_map>
#include "Room.hpp"
#include "Session.hpp"

class Lobby : public std::enable_shared_from_this<Lobby> {
  int id_;
  std::list<Session::pointer> clients_; // FIXME: set으로 바꾸는게 좋을듯
  std::unordered_map<int, std::shared_ptr<Room>> rooms_;

  static const int MAX_ROOM_SIZE = 5;
  static const int MAX_CLIENT_SIZE = 50;
  static int ID_COUNTER;

 public:
  static std::shared_ptr<Lobby> create();
  explicit Lobby(int id);
  int getId() const { return id_; };
  void Enter(Session::pointer session);
  void Leave(Session::pointer session);
  int CreateRoom(Session::pointer session, string room_name);
  int EnterRoom(Session::pointer session, int room_id);
  std::vector<string> GetRoomList() const;
  std::vector<std::shared_ptr<Room>> GetRooms() const;
  std::shared_ptr<Room> getRoom(int room_id) const;

  void Broadcast(char* packet, uint16_t copySize, Session::pointer sender, bool isExceptRoom = true, bool isExceptMe = true);
};

#endif  //LOBBYSERVER_LOBBY_HPP
