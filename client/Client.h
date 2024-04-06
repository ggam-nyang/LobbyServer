//
// Created by ggam-nyang on 2/16/24.
//

#ifndef BOOSTASIO_CLIENT_H
#define BOOSTASIO_CLIENT_H

#include <iostream>
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"

#include "../Packet/Packet.hpp"
#include "ClientPacketManager.hpp"

using namespace boost;
using boost::asio::ip::tcp;
using std::cout;
using std::endl;

enum class USER_STATE : uint16_t {
  NONE = 0,
  LOBBY = 1,
  ROOM = 2,
  READY = 3,
  BATTLE = 4,
};

class ClientPacketManager;

class Client {
  friend class ClientPacketManager;
  asio::ip::tcp::endpoint endpoint_;
  asio::io_context io_context_;
  ClientPacketManager packetManager_;
  asio::ip::tcp::socket sock;
  shared_ptr<boost::asio::io_service::work> work_;
  thread_group thread_group_;
  std::string writeBuffer_;
  std::array<char, 1000> buffer_;
  boost::asio::posix::stream_descriptor input_;
  std::mutex lock_;
  boost::asio::steady_timer timer_;
  std::atomic<bool> is_get_input_ = true;

 public:
  USER_STATE state_ = USER_STATE::NONE;

 public:
  Client(std::string ip_address, unsigned short port_num);

  void Start();

 private:
  void WorkerThread();

  void TryConnect();

  void OnConnect(const system::error_code& ec);

  void Send();

  void Receive();

  void InputHandler(const system::error_code& error, std::size_t bytes_transferred);

  void SendHandle(const system::error_code& ec, const char* packet);

  void ReceiveHandle(const system::error_code& ec, size_t size);

  void StopAll();

  void Battle();

  void BattleHandler(const system::error_code& ec);

  void ResponseSetName(SET_NAME_RESPONSE_PACKET packet);

  void ResponseEnterLobby(LOBBY_ENTER_RESPONSE_PACKET packet);

  void ResponseCreateRoom(ROOM_CREATE_RESPONSE_PACKET packet);

  void ResponseRoomList(ROOM_LIST_RESPONSE_PACKET packet);

  void ResponseEnterRoom(ROOM_ENTER_RESPONSE_PACKET packet);

  void ResponseEnterRoomBroadcast(ROOM_ENTER_BROADCAST_PACKET packet);

  void ResponseLeaveRoom(ROOM_LEAVE_RESPONSE_PACKET packet);

  void ResponseLeaveRoomBroadcast(ROOM_LEAVE_BROADCAST_PACKET packet);

  void ResponseChat(CHAT_RESPONSE_PACKET packet);

  void ResponseReady(ROOM_READY_RESPONSE_PACKET packet);

  void ResponseBattleStart(BATTLE_START_RESPONSE_PACKET packet);

  void ResponseAttack(ATTACK_RESPONSE_PACKET packet);

  void ResponseBattleInfo(BATTLE_INFO_PACKET packet);
};

#endif  //BOOSTASIO_CLIENT_H
