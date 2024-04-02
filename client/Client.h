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
  std::string readBuffer_;
  std::array<char, 1000> buffer_;
  std::mutex lock_;

 public:
  bool in_lobby_ = false;
  bool in_room_ = false;
  bool isReady_ = false;

 public:
  Client(std::string ip_address, unsigned short port_num);

  void Start();

 private:
  void WorkerThread();

  void TryConnect();

  void OnConnect(const system::error_code& ec);

  void Send();

  void Receive();

  void SendHandle(const system::error_code& ec, const char* packet);

  void ReceiveHandle(const system::error_code& ec, size_t size);

  void StopAll();

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
};

#endif  //BOOSTASIO_CLIENT_H
