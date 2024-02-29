//
// Created by ggam-nyang on 2/27/24.
//

#include "Protocol.h"
#include "boost/lexical_cast.hpp"

Protocol::Protocol(const ProtocolType type, const std::string& body)
    : header_{type, static_cast<uint32_t>(body.size())}, body_(body) {}

std::unique_ptr<Protocol> Protocol::create(ProtocolType type,
                                           const std::string& body) {
  return std::make_unique<Protocol>(type, body);
}

std::unique_ptr<Protocol> Protocol::create(const std::string& data) {
  auto protocol = std::make_unique<Protocol>();
  protocol->decode(data);

  return protocol;
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

std::string Protocol::encode() const {
  // 헤더와 바디를 하나의 문자열로 결합
  std::string encoded_data;
  encoded_data.resize(HEADER_SIZE + body_.size());
  std::memcpy(&encoded_data[0], &header_, HEADER_SIZE);
  std::memcpy(&encoded_data[HEADER_SIZE], body_.data(), body_.size());

  return encoded_data;
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
