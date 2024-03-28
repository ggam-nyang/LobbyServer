//
// Created by ggam-nyang on 3/25/24.
//

#ifndef LOBBYSERVER_PACKET_HPP
#define LOBBYSERVER_PACKET_HPP

#include <cstdlib>

// 100 ~ 1000 사이의 번호만 사용 가능!
enum class  PACKET_ID : uint16_t
{

  /// ChatClient용 패킷 ID
  SET_NAME_REQUEST = 201,
  SET_NAME_RESPONSE = 202,

  ROOM_ENTER_REQUEST = 206,
  ROOM_ENTER_RESPONSE = 207,

  ROOM_LEAVE_REQUEST = 209,
  ROOM_LEAVE_RESPONSE = 210,

  ROOM_CHAT_REQUEST = 214,
  ROOM_CHAT_RESPONSE = 215,
  ROOM_CHAT_NOTIFY = 216,
};



#pragma pack(push,1)
struct PACKET_HEADER
{
  uint16_t PacketLength;
  uint16_t PacketId;
  uint8_t Type; //압축여부 암호화여부 등 속성을 알아내는 값
};
const int PACKET_HEADER_SIZE = sizeof(PACKET_HEADER);

//- 로그인 요청
const int MAX_USERNAME_LEN = 20;

struct SET_NAME_REQUEST_PACKET : public PACKET_HEADER
{
  SET_NAME_REQUEST_PACKET() : PACKET_HEADER() {
    PacketId = static_cast<uint16_t>(PACKET_ID::SET_NAME_REQUEST);
    PacketLength = sizeof(SET_NAME_REQUEST_PACKET);
  }

  char username[MAX_USERNAME_LEN+1]{};
};
const size_t SET_NAME_REQUEST_PACKET_SZIE = sizeof(SET_NAME_REQUEST_PACKET);


struct SET_NAME_RESPONSE_PACKET : public PACKET_HEADER
{
  uint16_t result;
};

////- 룸에 들어가기 요청
//const int MAX_ROOM_TITLE_SIZE = 16;
//struct ROOM_ENTER_REQUEST_PACKET : public PACKET_HEADER
//{
//  uint32_t RoomNumber;
//};
//
//struct ROOM_ENTER_RESPONSE_PACKET : public PACKET_HEADER
//{
//  uint16_t Result;
//  char RivalUserID[MAX_USER_ID_LEN + 1] = {0, };
//};
//
//
////- 룸 나가기 요청
//struct ROOM_LEAVE_REQUEST_PACKET : public PACKET_HEADER
//{
//
//};
//
//struct ROOM_LEAVE_RESPONSE_PACKET : public PACKET_HEADER
//{
//  uint16_t Result;
//};
//
//
//
//// 룸 채팅
//const int MAX_CHAT_MSG_SIZE = 256;
//struct ROOM_CHAT_REQUEST_PACKET : public PACKET_HEADER
//{
//  char Message[MAX_CHAT_MSG_SIZE +1] = { 0, };
//};
//
//struct ROOM_CHAT_RESPONSE_PACKET : public PACKET_HEADER
//{
//  uint16_t Result;
//};
//
//struct ROOM_CHAT_NOTIFY_PACKET : public PACKET_HEADER
//{
//  char UserID[MAX_USER_ID_LEN + 1] = { 0, };
//  char Msg[MAX_CHAT_MSG_SIZE + 1] = { 0, };
//};

#pragma pack(pop) //위에 설정된 패킹설정이 사라짐
#endif  //LOBBYSERVER_PACKET_HPP
