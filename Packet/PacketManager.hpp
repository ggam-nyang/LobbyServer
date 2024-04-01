//
// Created by ggam-nyang on 3/25/24.
//

#ifndef LOBBYSERVER_PACKETMANAGER_HPP
#define LOBBYSERVER_PACKETMANAGER_HPP

#include <functional>
#include "Packet.hpp"

class Session;

class PacketManager {
 public:
  PacketManager() = default;
  ~PacketManager() = default;

  static PACKET_ID ToPacketID(char* pBuf, uint16_t copySize);
  void ReceivePacket(std::shared_ptr<Session> session, char* pBuf,
                     uint16_t copySize);
};

#endif  //LOBBYSERVER_PACKETMANAGER_HPP
