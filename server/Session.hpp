//
// Created by ggam-nyang on 2/26/24.
//

#ifndef LOBBYSERVER_SESSION_H
#define LOBBYSERVER_SESSION_H

#include <boost/thread.hpp>
#include <iostream>
#include <memory>
#include "../Packet/Packet.hpp"
#include "boost/asio.hpp"

class Server;
class Lobby;
class Room;
class BattleInfo;

using std::string;

// FIXME: User 객체와 분리..?
enum class USER_STATE : uint16_t {
  NONE = 0,
  LOBBY = 1,
  ROOM = 2,
  READY = 3,
  BATTLE = 4,
  DIED = 5,
};

class Session : public std::enable_shared_from_this<Session> {
  static int ID_COUNTER;

 public:
  using pointer = std::shared_ptr<Session>;

  boost::asio::ip::tcp::socket socket_;
  Server* server_;

  const int id_ = ID_COUNTER;
  string name_;
  USER_STATE state_ = USER_STATE::NONE;

  string readBuffer;
  std::array<char, 240> buffer_;
  std::shared_ptr<BattleInfo> battleInfo_;

  static pointer create(boost::asio::io_context& io_context, Server* server);
  // FIXME: private으로 하고 싶은데, 그러면 create에서 접근이 안됨
  explicit Session(boost::asio::io_context& io_context, Server* server);
  Session() = delete;

  void Receive();
  void ReceiveHandle(const boost::system::error_code& ec, size_t size);
  void write(char* pBuf, uint16_t pSize);
  void onWrite(const boost::system::error_code& ec);
  void SetNameReq(SET_NAME_REQUEST_PACKET& packet);
  void EnterLobbyReq(LOBBY_ENTER_REQUEST_PACKET& packet);
  void CreateRoomReq(ROOM_CREATE_REQUEST_PACKET& packet);
  void ListRoomReq(ROOM_LIST_REQUEST_PACKET& packet);
  void EnterRoomReq(ROOM_ENTER_REQUEST_PACKET& packet);
  void LeaveRoomReq(ROOM_LEAVE_REQUEST_PACKET& packet);
  void ChatReq(CHAT_REQUEST_PACKET& packet);
  void ReadyReq(ROOM_READY_REQUEST_PACKET& packet);
  void BattleStartReq(BATTLE_START_REQUEST_PACKET& packet);
  void AttackReq(ATTACK_REQUEST_PACKET& packet);
  void close();
  void setRoom(std::shared_ptr<Room> room);
  bool IsInRoom() const;
  bool IsReady() const;

 private:
  std::shared_ptr<Lobby> lobby_;
  std::shared_ptr<Room> room_;
};

#endif  //LOBBYSERVER_SESSION_H
