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

  void Init();
  PACKET_ID ToPacketID(char* pBuf, uint16_t copySize);
  SET_NAME_REQUEST_PACKET& EncodeSetNameRequestPacket(char* pBuf);

  void ProcessRecvPacket(std::shared_ptr<Session> session, char* pBuf,
                         uint16_t copySize);
  static void ProcessEncodePacket(std::shared_ptr<Session> session, char* pBuf,
                           uint16_t copySize);

 private:
//  using PROCESS_RECV_PACKET_FUNCTION = void (PacketManager::*)(std::shared_ptr<Session> session, char*,
//                                                               uint16_t);
//  std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> recvFunctionDictionary_;
//  std::function<void(int, char*)> m_SendMQDataFunc;
};

#endif  //LOBBYSERVER_PACKETMANAGER_HPP
