//
// Created by ggam-nyang on 3/26/24.
//

#ifndef LOBBYSERVER_CLIENTPACKETMANAGER_HPP
#define LOBBYSERVER_CLIENTPACKETMANAGER_HPP

#include <string>
#include "../Protocol/Packet/Packet.hpp"

class Client;

class ClientPacketManager {
 public:
  ClientPacketManager() = default;
  ~ClientPacketManager() = default;

  PACKET_ID ToPacketID(char* pBuf, uint16_t copySize);

  void ReceivePacket(Client* client, char* pBuf, uint16_t copySize);

  std::pair<char*, uint16_t> SendPacket(Client* client,
                                        const std::string& input);
};

#endif  //LOBBYSERVER_CLIENTPACKETMANAGER_HPP
