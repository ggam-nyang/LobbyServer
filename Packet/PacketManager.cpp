//
// Created by ggam-nyang on 3/25/24.
//

#include "PacketManager.hpp"
#include <iostream>
#include <string>
#include <utility>
#include "../server/Session.hpp"

PACKET_ID PacketManager::ToPacketID(char* pBuf, uint16_t copySize) {
  auto const* pHeader = reinterpret_cast<PACKET_HEADER*>(pBuf);

  return static_cast<PACKET_ID>(pHeader->PacketId);
}

void PacketManager::ReceivePacket(std::shared_ptr<Session> session, char* pBuf,
                                  uint16_t copySize) {
  auto packetId = ToPacketID(pBuf, copySize);

  if (packetId == PACKET_ID::SET_NAME_REQUEST) {
    auto packet = reinterpret_cast<SET_NAME_REQUEST_PACKET*>(pBuf);
    session->SetNameReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::LOBBY_ENTER_REQUEST) {
    auto packet = reinterpret_cast<LOBBY_ENTER_REQUEST_PACKET*>(pBuf);
    session->EnterLobbyReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_CREATE_REQUEST) {
    auto packet = reinterpret_cast<ROOM_CREATE_REQUEST_PACKET*>(pBuf);
    session->CreateRoomReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_LIST_REQUEST) {
    auto packet = reinterpret_cast<ROOM_LIST_REQUEST_PACKET*>(pBuf);
    session->ListRoomReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_ENTER_REQUEST) {
    auto packet = reinterpret_cast<ROOM_ENTER_REQUEST_PACKET*>(pBuf);
    session->EnterRoomReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_LEAVE_REQUEST) {
    auto packet = reinterpret_cast<ROOM_LEAVE_REQUEST_PACKET*>(pBuf);
    session->LeaveRoomReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::CHAT_REQUEST) {
    auto packet = reinterpret_cast<CHAT_REQUEST_PACKET*>(pBuf);
    session->ChatReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::ROOM_READY_REQUEST) {
    auto packet = reinterpret_cast<ROOM_READY_REQUEST_PACKET*>(pBuf);
    session->ReadyReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::BATTLE_START_REQUEST) {
    auto packet = reinterpret_cast<BATTLE_START_REQUEST_PACKET*>(pBuf);
    session->BattleStartReq(*packet);
    return;
  }

  if (packetId == PACKET_ID::ATTACK_REQUEST) {
    auto packet = reinterpret_cast<ATTACK_REQUEST_PACKET*>(pBuf);
    session->AttackReq(*packet);
    return;
  }
}