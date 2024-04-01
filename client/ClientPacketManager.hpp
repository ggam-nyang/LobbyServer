//
// Created by ggam-nyang on 3/26/24.
//

#ifndef LOBBYSERVER_CLIENTPACKETMANAGER_HPP
#define LOBBYSERVER_CLIENTPACKETMANAGER_HPP

#include <string>
#include "../Protocol/Packet/Packet.hpp"
#include "boost/asio.hpp"

class Client;

class ClientPacketManager {
  Client* client_;
  constexpr static int MAX_PACKET_SIZE = 200;

 public:
  ClientPacketManager() = default;
  ClientPacketManager(Client* client);
  ~ClientPacketManager() = default;

  PACKET_ID ToPacketID(char* pBuf, uint16_t copySize);
  bool MakePacket(char* pBuf, uint16_t& copySize, std::string input);

  void ReceivePacket(Client* client, char* pBuf, uint16_t copySize);

  void Send(const char* packet, const uint16_t copySize);
  void SendHandle(const boost::system::error_code& ec, const char* packet);

  bool SendPacket(Client* client, const std::string& input);

//  std::vector<char []> buffer_;
};

#endif  //LOBBYSERVER_CLIENTPACKETMANAGER_HPP
