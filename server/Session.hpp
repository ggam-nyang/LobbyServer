//
// Created by ggam-nyang on 2/26/24.
//

#ifndef LOBBYSERVER_SESSION_H
#define LOBBYSERVER_SESSION_H

#include <boost/thread.hpp>
#include <iostream>
#include <memory>
#include "../Protocol/Protocol.h"
#include "../Protocol/Packet/Packet.hpp"
#include "boost/asio.hpp"

class Server;
class Lobby;
class Room;

using std::string;

// FIXME: User 객체와 분리..?
enum class UserState {
  NONE,
  LOBBY,
  ROOM,
  READY,
  BATTLE,
};

class PacketManager;

class Session : public std::enable_shared_from_this<Session> {
  static int ID_COUNTER;
  UserState state_ = UserState::NONE; // 상태 관리를... 더 좋은 방법은 없을까

  friend class PacketManager;
 public:
  using pointer = std::shared_ptr<Session>;

  boost::asio::ip::tcp::socket socket_;
  Server* server_;

  const int id_ = ID_COUNTER;
  string name_;
  string readBuffer;
  std::array<char, 100> buffer;

  static pointer create(boost::asio::io_context& io_context, Server* server);
  // FIXME: private으로 하고 싶은데, 그러면 create에서 접근이 안됨
  explicit Session(boost::asio::io_context& io_context, Server* server);
  Session() = delete;

  void read();
  void write(ProtocolPtr& protocolPtr);
  void write(char * pBuf, uint16_t pSize);
  void onWrite(const boost::system::error_code& ec);
  void WriteToServer(ProtocolPtr& protocolPtr, bool isExceptMe = true);
  void WriteToLobby(ProtocolPtr& protocolPtr, std::shared_ptr<Lobby> lobby,
                    bool isExceptMe = true);
  void WriteToRoom(ProtocolPtr& protocolPtr, std::shared_ptr<Room> room,
                   bool isExceptMe = true);
  void protocolManage(ProtocolPtr& protocolPtr);
  void setName(string& body);
  void ReqSetName(SET_NAME_REQUEST_PACKET packet);
  void chat(string& body);
  void alert(string& body);
  void RoomList(string& body);
  void CreateRoom(string& body);
  void EnterRoom(string& body);
  void EnterLobby(std::shared_ptr<Lobby> lobby);
  void LeaveRoom();
  void BattleStart();
  void close();
  void setRoom(std::shared_ptr<Room> room);
  bool IsInRoom() const;

 private:
  std::shared_ptr<Lobby> lobby_;
  std::shared_ptr<Room> room_;
};

#endif  //LOBBYSERVER_SESSION_H
