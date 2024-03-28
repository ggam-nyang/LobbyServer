//
// Created by ggam-nyang on 3/26/24.
//

#include "ClientPacketManager.hpp"
#include "Client.h"

ClientPacketManager::ClientPacketManager(Client* client) : client_(client) {}

PACKET_ID ClientPacketManager::ToPacketID(char* pBuf, uint16_t copySize) {
  auto const* pHeader = reinterpret_cast<PACKET_HEADER*>(pBuf);

  return static_cast<PACKET_ID>(pHeader->PacketId);
}

bool ClientPacketManager::MakePacket(char* pBuf, uint16_t& copySize,
                                     std::string input) {
  if (input[0] != ':')
    return false;  // FIXME: Chat

  auto index = input.find(' ', 0);
  std::string order = input.substr(0, index);
  std::string body = input.substr(index + 1);

  if (order == ":set") {
    auto setNameReqPacket = SET_NAME_REQUEST_PACKET();
    strcpy(setNameReqPacket.username, body.c_str());

    copySize = setNameReqPacket.PacketLength;
    memcpy(pBuf, &setNameReqPacket, copySize);

    return true;
  }

  return false;
}

bool ClientPacketManager::SendPacket(Client* client, const std::string& input) {
  char packet[MAX_PACKET_SIZE]; // FIXME: vector 등으로 메모리 관리..?
  uint16_t copySize;

  if (!MakePacket(packet, copySize, input)) {}

  Send(packet, copySize);
}

void ClientPacketManager::Send(const char* packet, const uint16_t copySize) {
  client_->sock.async_write_some(
      asio::buffer(packet, copySize),
      [this, packet](const system::error_code& ec, size_t) {
        client_->SendHandle(ec, packet);
      });
}

void ClientPacketManager::SendHandle(const system::error_code& ec,
                                     const char* packet) {
  if (ec) {
    cout << "async_write_some error: " << ec.message() << endl;
    client_->StopAll();
    return;
  }
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
