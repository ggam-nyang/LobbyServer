//
// Created by ggam-nyang on 3/26/24.
//

#include "ClientPacketManager.hpp"
#include "Client.h"

PACKET_ID ClientPacketManager::ToPacketID(char* pBuf, uint16_t copySize) {
  auto const* pHeader = reinterpret_cast<PACKET_HEADER*>(pBuf);

  return static_cast<PACKET_ID>(pHeader->PacketId);
}

void ClientPacketManager::ReceivePacket(Client* client, char* pBuf,
                                        uint16_t copySize) {
  auto packetId = ToPacketID(pBuf, copySize);

  if (packetId == PACKET_ID::SET_NAME_RESPONSE) {
    auto setNameResPacket = reinterpret_cast<SET_NAME_RESPONSE_PACKET*>(pBuf);
    client->ResponseSetName(*setNameResPacket);
    return;
  }

  if (packetId == PACKET_ID::ROOM_ENTER_REQUEST) {
    return;
  }
}

std::pair<char *, uint16_t> ClientPacketManager::SendPacket(Client* client, const std::string& input) {
  if (input[0] != ':')
    return std::make_pair(nullptr, 0); // FIXME: Chat

  auto index = input.find(' ', 0);
  std::string order = input.substr(0, index);
  std::string body = input.substr(index + 1);

  char* sendPacket;

  if (order == ":set") {
    auto setNameReqPacket = SET_NAME_REQUEST_PACKET{};
    setNameReqPacket.PacketId = static_cast<int>(PACKET_ID::SET_NAME_REQUEST);
    setNameReqPacket.PacketLength = PACKET_HEADER_SIZE + body.size() + 1;
    strcpy(setNameReqPacket.username, body.c_str());

    cout << "header size: " << PACKET_HEADER_SIZE << endl;
    cout << "body size: " << body.size() << endl;
    cout << "PACKet size: " << SET_NAME_REQUEST_PACKET_SZIE << endl;


    auto* packet = reinterpret_cast<char*>(&setNameReqPacket);

    return std::make_pair(packet, setNameReqPacket.PacketLength);
  }

  //  if (order == ":create_room")
  //    return create(ProtocolType::CREATE_ROOM, body);
  //
  //  if (order == ":room_list")
  //    return create(ProtocolType::ROOM_LIST, body);
  //
  //  if (order == ":enter_room")
  //    return create(ProtocolType::ENTER_ROOM, body);
  //
  //  if (order == ":enter_lobby")
  //    return create(ProtocolType::ENTER_LOBBY, body);
  //
  //  if (order == ":leave_room")
  //    return create(ProtocolType::LEAVE_ROOM, body);
  //
  //  if (order == ":battle_start")
  //    return create(ProtocolType::BATTLE_START, body);
  //
  //  return create(ProtocolType::UNKNOWN, "Wrong message");
}
