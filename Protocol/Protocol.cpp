//
// Created by ggam-nyang on 2/27/24.
//

#include "Protocol.h"
#include "boost/lexical_cast.hpp"

Protocol::Protocol(const ProtocolType type, const std::string& body)
    : header_{type, static_cast<uint32_t>(body.size())}, body_(body) {}


std::shared_ptr<Protocol> Protocol::create(ProtocolType type,
                                           const std::string& body) {
  return std::make_shared<Protocol>(type, body);
}

std::shared_ptr<Protocol> Protocol::create(const std::string& data) {
  auto protocol = std::make_shared<Protocol>();
  protocol->decode(data);

  return protocol;
}

std::shared_ptr<Protocol> Protocol::from(const std::string& input) {
  if (input[0] != ':')
    return create(ProtocolType::CHAT, input);

  auto index = input.find(' ', 0);
  std::string order = input.substr(0, index);
  std::string body = input.substr(index + 1);

  if (order == ":set")
    return create(ProtocolType::SET_ID, body);

  if (order == ":create_room")
    return create(ProtocolType::CREATE_ROOM, body);

  if (order == ":room_list")
    return create(ProtocolType::ROOM_LIST, body);

  if (order == ":enter_room")
    return create(ProtocolType::ENTER_ROOM, body);

  if (order == ":enter_lobby")
    return create(ProtocolType::ENTER_LOBBY, body);

  if (order == ":leave_room")
    return create(ProtocolType::LEAVE_ROOM, body);

  if (order == ":battle_start")
    return create(ProtocolType::BATTLE_START, body);

  return create(ProtocolType::UNKNOWN, "Wrong message");
}

bool Protocol::decode(const std::string& data) {
  // 데이터 길이가 헤더 크기보다 작으면 에러
  if (data.size() < HEADER_SIZE) {
    return false;
  }

  // 헤더 추출
  std::memcpy(&header_, data.data(), HEADER_SIZE);

  // 유효성 검사
  if (header_.type == ProtocolType::UNKNOWN) {
    return false;
  }

  // 바디 추출
  body_.resize(header_.body_size);
  std::memcpy(&body_[0], data.data() + HEADER_SIZE, header_.body_size);

  return true;
}

std::string& Protocol::encode() {
  // 헤더와 바디를 하나의 문자열로 결합
  encoded_data_.resize(HEADER_SIZE + body_.size());
  std::memcpy(&encoded_data_[0], &header_, HEADER_SIZE);
  std::memcpy(&encoded_data_[HEADER_SIZE], body_.data(), body_.size());

  return encoded_data_;
}

std::string toString(ProtocolType type) {
  switch (type) {
    using enum ProtocolType;
    case SET_ID:
      return "SET_ID";
    case CHAT:
      return "CHAT";
    case ALERT:
      return "ALERT";
    default:
      return "UNKNOWN";
  }
}

ProtocolType toProtocolType(const std::string& str) {
  if (str == "SET_ID") {
    return ProtocolType::SET_ID;
  } else if (str == "CHAT") {
    return ProtocolType::CHAT;
  } else if (str == "ALERT") {
    return ProtocolType::ALERT;
  }

  return ProtocolType::UNKNOWN;
}
