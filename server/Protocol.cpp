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
  if (data.size() < HEADER_SIZE) {
    return false;
  }

  std::memcpy(&header_, data.data(), HEADER_SIZE);

  if (header_.body_size > data.size() - HEADER_SIZE) {
    return false;
  }

  // 바디 할당 및 복사
  body_.resize(header_.body_size);
  std::memcpy(body_.data(), data.data() + HEADER_SIZE, header_.body_size);

  return true;
}

std::string Protocol::encode() const {
  std::string data;
  data.resize(HEADER_SIZE + header_.body_size);
  std::fill(data.begin(), data.end(), ' ');

  std::memcpy(data.data(), &header_, HEADER_SIZE);
  std::memcpy(data.data() + HEADER_SIZE, body_.data(), header_.body_size);

  return data;
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
