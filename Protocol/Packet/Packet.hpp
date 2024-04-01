//
// Created by ggam-nyang on 3/25/24.
//

#ifndef LOBBYSERVER_PACKET_HPP
#define LOBBYSERVER_PACKET_HPP

#include <cstdlib>
#include <string>

// 100 ~ 1000 사이의 번호만 사용 가능!
enum class PACKET_ID : uint16_t {

  /// ChatClient용 패킷 ID
  SET_NAME_REQUEST = 201,
  SET_NAME_RESPONSE = 202,

  LOBBY_ENTER_REQUEST = 203,
  LOBBY_ENTER_RESPONSE = 204,

  ROOM_CREATE_REQUEST = 205,
  ROOM_CREATE_RESPONSE = 206,

  ROOM_LIST_REQUEST = 207,
  ROOM_LIST_RESPONSE = 208,

  ROOM_ENTER_REQUEST = 209,
  ROOM_ENTER_RESPONSE = 210,
  ROOM_ENTER_BROADCAST = 211,

  ROOM_LEAVE_REQUEST = 212,
  ROOM_LEAVE_RESPONSE = 213,
  ROOM_LEAVE_BROADCAST = 214,

  CHAT_REQUEST = 215,
  CHAT_RESPONSE = 216,
};

#pragma pack(push, 1)

struct PACKET_HEADER {
  uint16_t PacketLength;
  uint16_t PacketId;
  uint8_t Type;  //압축여부 암호화여부 등 속성을 알아내는 값
};

const int PACKET_HEADER_SIZE = sizeof(PACKET_HEADER);

// 이름 설정
const int MAX_USERNAME_LEN = 20;

struct SET_NAME_REQUEST_PACKET : public PACKET_HEADER {
  SET_NAME_REQUEST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::SET_NAME_REQUEST);
    PacketLength = sizeof(SET_NAME_REQUEST_PACKET);
  }

  char username[MAX_USERNAME_LEN]{};
};

const size_t SET_NAME_REQUEST_PACKET_SZIE = sizeof(SET_NAME_REQUEST_PACKET);

struct SET_NAME_RESPONSE_PACKET : public PACKET_HEADER {
  SET_NAME_RESPONSE_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::SET_NAME_RESPONSE);
    PacketLength = sizeof(SET_NAME_RESPONSE_PACKET);
  }

  uint16_t result{};
};

// 로비 입장
struct LOBBY_ENTER_REQUEST_PACKET : public PACKET_HEADER {
  LOBBY_ENTER_REQUEST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::LOBBY_ENTER_REQUEST);
    PacketLength = sizeof(LOBBY_ENTER_REQUEST_PACKET);
  }
};

struct LOBBY_ENTER_RESPONSE_PACKET : public PACKET_HEADER {
  LOBBY_ENTER_RESPONSE_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::LOBBY_ENTER_RESPONSE);
    PacketLength = sizeof(LOBBY_ENTER_RESPONSE_PACKET);
  }

  uint16_t result{};
};

// 방 생성
const int MAX_ROOM_NAME_LEN = 10;

struct ROOM_CREATE_REQUEST_PACKET : public PACKET_HEADER {
  ROOM_CREATE_REQUEST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_CREATE_REQUEST);
    PacketLength = sizeof(ROOM_CREATE_REQUEST_PACKET);
  }

  char roomName[MAX_ROOM_NAME_LEN]{};
};

struct ROOM_CREATE_RESPONSE_PACKET : public PACKET_HEADER {
  ROOM_CREATE_RESPONSE_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_CREATE_RESPONSE);
    PacketLength = sizeof(ROOM_CREATE_RESPONSE_PACKET);
  }

  uint16_t result{};
};

// 방 리스트
struct ROOM_LIST_REQUEST_PACKET : public PACKET_HEADER {
  ROOM_LIST_REQUEST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_LIST_REQUEST);
    PacketLength = sizeof(ROOM_LIST_REQUEST_PACKET);
  }
};

const int MAX_ROOM_LIST_COUNT = 10;

struct ROOM {
  std::string name{};
  uint16_t id{};
};

struct ROOM_LIST_RESPONSE_PACKET : public PACKET_HEADER {
  ROOM_LIST_RESPONSE_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_LIST_RESPONSE);
    PacketLength = sizeof(ROOM_LIST_RESPONSE_PACKET);
  }

  ROOM roomList[MAX_ROOM_LIST_COUNT]{};
  uint16_t roomCount = 0;
  uint16_t result{};
};

// 방 입장
struct ROOM_ENTER_REQUEST_PACKET : public PACKET_HEADER {
  ROOM_ENTER_REQUEST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_ENTER_REQUEST);
    PacketLength = sizeof(ROOM_ENTER_REQUEST_PACKET);
  }

  uint16_t roomId{};
};

// 0: 이미 방에 입장, 1: 성공, 2: 로비 입장 안됨 3: 방이 꽉 참, 4: 방이 없음
struct ROOM_ENTER_RESPONSE_PACKET : public PACKET_HEADER {
  ROOM_ENTER_RESPONSE_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_ENTER_RESPONSE);
    PacketLength = sizeof(ROOM_ENTER_RESPONSE_PACKET);
  }

  uint16_t result{};
};

struct ROOM_ENTER_BROADCAST_PACKET : public PACKET_HEADER {
  ROOM_ENTER_BROADCAST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_ENTER_BROADCAST);
    PacketLength = sizeof(ROOM_ENTER_BROADCAST_PACKET);
  }

  char username[MAX_USERNAME_LEN]{};  //입장한 유저 이름
};

// 방 퇴장
struct ROOM_LEAVE_REQUEST_PACKET : public PACKET_HEADER {
  ROOM_LEAVE_REQUEST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_LEAVE_REQUEST);
    PacketLength = sizeof(ROOM_LEAVE_REQUEST_PACKET);
  }
};

// 0: 방이 아님, 1: 성공, 2: 로비 입장 안됨
struct ROOM_LEAVE_RESPONSE_PACKET : public PACKET_HEADER {
  ROOM_LEAVE_RESPONSE_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_LEAVE_RESPONSE);
    PacketLength = sizeof(ROOM_LEAVE_RESPONSE_PACKET);
  }

  uint16_t result{};
};

struct ROOM_LEAVE_BROADCAST_PACKET : public PACKET_HEADER {
  ROOM_LEAVE_BROADCAST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::ROOM_LEAVE_BROADCAST);
    PacketLength = sizeof(ROOM_LEAVE_BROADCAST_PACKET);
  }

  char username[MAX_USERNAME_LEN]{};  //퇴장한 유저 이름
};

// 채팅
enum class CHAT_TYPE : uint16_t {
  LOBBY = 0,
  ROOM = 1,
};

const int MAX_CHAT_LEN = 100;
struct CHAT_REQUEST_PACKET : public PACKET_HEADER {
  CHAT_REQUEST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::CHAT_REQUEST);
    PacketLength = sizeof(CHAT_REQUEST_PACKET);
  }

  char chat[MAX_CHAT_LEN]{};
  CHAT_TYPE chatType{};
};

struct CHAT_RESPONSE_PACKET : public PACKET_HEADER {
  CHAT_RESPONSE_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::CHAT_RESPONSE);
    PacketLength = sizeof(CHAT_RESPONSE_PACKET);
  }

  char username[MAX_USERNAME_LEN]{};
  char chat[MAX_CHAT_LEN]{};
  CHAT_TYPE chatType{};
};

#pragma pack(pop)  //위에 설정된 패킹설정이 사라짐
#endif             //LOBBYSERVER_PACKET_HPP
