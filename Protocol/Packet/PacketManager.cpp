//
// Created by ggam-nyang on 3/25/24.
//

#include "PacketManager.hpp"
#include <iostream>
#include <utility>
#include "../../server/Session.hpp"
#include <string>

//void PacketManager::Init() {
//  recvFunctionDictionary_ =
//      std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();
//
//  recvFunctionDictionary_[(int)PACKET_ID::SET_NAME_REQUEST] =
//      &PacketManager::ProcessSetName;
//}

PACKET_ID PacketManager::ToPacketID(char* pBuf, uint16_t copySize) {
  auto const* pHeader = reinterpret_cast<PACKET_HEADER*>(pBuf);

  return static_cast<PACKET_ID>(pHeader->PacketId);
}

void PacketManager::ProcessRecvPacket(std::shared_ptr<Session> session,
                                      char* pBuf, uint16_t copySize) {
  auto packetId = ToPacketID(pBuf, copySize);

  if (packetId == PACKET_ID::SET_NAME_REQUEST) {
    auto setNameReqPacket = reinterpret_cast<SET_NAME_REQUEST_PACKET*>(pBuf);
    session->ReqSetName(*setNameReqPacket);
    return;
  }

  if (packetId == PACKET_ID::ROOM_ENTER_REQUEST) {
    return;
  }
}

//void PacketManager::ProcessEncodePacket(std::shared_ptr<Session> session, char* pBuf, uint16_t copySize) {
//  auto packetId = ToPacketID(pBuf, copySize);
//
//  if (packetId == PACKET_ID::SET_NAME_REQUEST) {
//    auto setNameReqPacket = reinterpret_cast<SET_NAME_REQUEST_PACKET*>(pBuf);
//    session->ReqSetName(*setNameReqPacket);
//    return;
//  }
//
//  if (packetId == PACKET_ID::SET_NAME_RESPONSE) {
//    return;
//  }
//
//  if (packetId == PACKET_ID::ROOM_ENTER_REQUEST) {
//    return;
//  }
//}

//void PacketManager::ProcessSetName(std::shared_ptr<Session> session, char* pBuf,
//                                   uint16_t copySize) {
//  if (SET_NAME_REQUEST_PACKET_SZIE != copySize) {
//    return;  // FIXME: error?
//  }
//
//  auto pSetNameReqPacket = reinterpret_cast<SET_NAME_REQUEST_PACKET*>(pBuf);
//
//  auto pUsername = pSetNameReqPacket->username;
//  std::cout << "requested user name = " << pUsername << std::endl;
//
//  std::string newName = pUsername;
//  session->setName(newName);
//
//  SET_NAME_RESPONSE_PACKET setNameResPacket{};
//  setNameResPacket.PacketId = (uint16_t)PACKET_ID::SET_NAME_RESPONSE;
//  setNameResPacket.PacketLength = sizeof(SET_NAME_RESPONSE_PACKET);
//  setNameResPacket.result = 1;
//
//  session->write(reinterpret_cast<char*>(&setNameResPacket),
//                 sizeof(SET_NAME_RESPONSE_PACKET));
//}