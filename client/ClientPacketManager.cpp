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
  if (input[0] != ':') {
    auto packet = CHAT_REQUEST_PACKET();
    if (client_->state_ == USER_STATE::LOBBY) {
      packet.chatType = CHAT_TYPE::LOBBY;
    } else if (client_->state_ == USER_STATE::NONE) {
      cout << "채팅은 로비나 방에 입장한 후에 가능합니다." << endl;
      return false;
    } else {
      packet.chatType = CHAT_TYPE::ROOM;
    }

    copySize = packet.PacketLength;
    strncpy(packet.chat, input.c_str(), input.size());
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  auto index = input.find(' ', 0);
  if (index == std::string::npos)
    index = input.size();
  std::string order = input.substr(0, index);
  std::string body =
      index == input.size() ? input.substr(index) : input.substr(index + 1);

  if (order == ":set") {
    if (body.size() > MAX_USERNAME_LEN) {
      cout << "이름은 최대 20자까지 가능합니다." << endl;
      return false;
    }
    auto packet = SET_NAME_REQUEST_PACKET();
    strcpy(packet.username, body.c_str());

    copySize = packet.PacketLength;
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  if (order == ":enter_lobby") {
    auto packet = LOBBY_ENTER_REQUEST_PACKET();

    copySize = packet.PacketLength;
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  if (order == ":create_room") {
    if (body.size() > MAX_ROOM_NAME_LEN) {
      cout << "방 이름은 최대 10자까지 가능합니다." << endl;
      return false;
    }
    auto packet = ROOM_CREATE_REQUEST_PACKET();
    strcpy(packet.roomName, body.c_str());

    copySize = packet.PacketLength;
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  if (order == ":room_list") {
    auto packet = ROOM_LIST_REQUEST_PACKET();

    copySize = packet.PacketLength;
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  if (order == ":enter_room") {
    if (body.empty()) {
      cout << "방 아아디를 입력해주세요." << endl;
      return false;
    }
    auto packet = ROOM_ENTER_REQUEST_PACKET();
    packet.roomId = stoi(body);

    copySize = packet.PacketLength;
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  if (order == ":leave_room") {
    auto packet = ROOM_LEAVE_REQUEST_PACKET();

    copySize = packet.PacketLength;
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  if (order == ":ready") {
    auto packet = ROOM_READY_REQUEST_PACKET();

    copySize = packet.PacketLength;
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  if (order == ":battle_start") {
    auto packet = BATTLE_START_REQUEST_PACKET();

    copySize = packet.PacketLength;
    memcpy(pBuf, &packet, copySize);

    return true;
  }

  cout << "유효하지 않은 명령입니다." << endl;
  return false;
}

bool ClientPacketManager::SendPacket(Client* client, const std::string& input) {
  char packet[MAX_PACKET_SIZE];  // FIXME: vector 등으로 메모리 관리..?
  uint16_t copySize{};

  if (!MakePacket(packet, copySize, input)) {}

  Send(packet, copySize);
}

bool ClientPacketManager::SendAttackPacket(Client* client,
                                          const uint16_t targetId) {
  auto packet = ATTACK_REQUEST_PACKET();
  packet.targetId = targetId;

  Send(reinterpret_cast<char*>(&packet), packet.PacketLength);
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
    auto packet = reinterpret_cast<SET_NAME_RESPONSE_PACKET*>(pBuf);
    client->ResponseSetName(*packet);
    return;
  }

  if (packetId == PACKET_ID::LOBBY_ENTER_RESPONSE) {
    auto packet = reinterpret_cast<LOBBY_ENTER_RESPONSE_PACKET*>(pBuf);
    client->ResponseEnterLobby(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_CREATE_RESPONSE) {
    auto packet = reinterpret_cast<ROOM_CREATE_RESPONSE_PACKET*>(pBuf);
    client->ResponseCreateRoom(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_LIST_RESPONSE) {
    auto packet = reinterpret_cast<ROOM_LIST_RESPONSE_PACKET*>(pBuf);
    client->ResponseRoomList(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_ENTER_RESPONSE) {
    auto packet = reinterpret_cast<ROOM_ENTER_RESPONSE_PACKET*>(pBuf);
    client->ResponseEnterRoom(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_ENTER_BROADCAST) {
    auto packet = reinterpret_cast<ROOM_ENTER_BROADCAST_PACKET*>(pBuf);
    client->ResponseEnterRoomBroadcast(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_LEAVE_RESPONSE) {
    auto packet = reinterpret_cast<ROOM_LEAVE_RESPONSE_PACKET*>(pBuf);
    client->ResponseLeaveRoom(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_LEAVE_BROADCAST) {
    auto packet = reinterpret_cast<ROOM_LEAVE_BROADCAST_PACKET*>(pBuf);
    client->ResponseLeaveRoomBroadcast(*packet);
    return;
  }

  if (packetId == PACKET_ID::CHAT_RESPONSE) {
    auto packet = reinterpret_cast<CHAT_RESPONSE_PACKET*>(pBuf);
    client->ResponseChat(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_READY_RESPONSE) {
    auto packet = reinterpret_cast<ROOM_READY_RESPONSE_PACKET*>(pBuf);
    client->ResponseReady(*packet);
    return;
  }

  if (packetId == PACKET_ID::BATTLE_START_RESPONSE) {
    auto packet = reinterpret_cast<BATTLE_START_RESPONSE_PACKET*>(pBuf);
    client->ResponseBattleStart(*packet);
    return;
  }

  if (packetId == PACKET_ID::ATTACK_RESPONSE) {
    auto packet = reinterpret_cast<ATTACK_RESPONSE_PACKET*>(pBuf);
    client->ResponseAttack(*packet);
    return;
  }

  if (packetId == PACKET_ID::BATTLE_INFO) {
    auto packet = reinterpret_cast<BATTLE_INFO_PACKET*>(pBuf);
    client->ResponseBattleInfo(*packet);
    return;
  }
}
