//
// Created by ggam-nyang on 2/27/24.
//

#ifndef LOBBYSERVER_PROTOCOL_H
#define LOBBYSERVER_PROTOCOL_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class ProtocolType : uint32_t {
  SET_ID,
  CHAT,
  ALERT,
  ENTER_LOBBY,
  CREATE_ROOM,
  ENTER_ROOM,
  LEAVE_ROOM,
  ROOM_LIST,
  BATTLE_START,
  UNKNOWN
};

class Protocol {
 public:
  static constexpr size_t HEADER_SIZE = sizeof(ProtocolType) + sizeof(uint32_t);
  static constexpr size_t BODY_SIZE = 1024;

  struct Header {
    ProtocolType type;
    uint32_t body_size;
  };

  static std::shared_ptr<Protocol> create(ProtocolType type,
                                          const std::string& body);
  static std::shared_ptr<Protocol> create(const std::string& data);
  static std::shared_ptr<Protocol> from(const std::string& input);

  Protocol() = default;
  Protocol(const ProtocolType type, const std::string& body);

  virtual ~Protocol() = default;

  bool decode(const std::string& data);

  std::string& encode();

  const Header& getHeader() const noexcept { return header_; }

  std::string& getBody() { return body_; }

 private:
  Header header_{};

  std::string body_;

  // ���ڵ忡�� ����ϴ� std::string�� �޸� ������ Protocol�� ������ ����Ŭ�� ���� �������ֱ� ���ؼ�, ��� ������ �����ϰ� �̸� ��ȯ�ϰ� �߽��ϴ�
  std::string encoded_data_;
};

// shared_ptr�� ��ȯ�� ������ life-cycle �����Դϴ�
using ProtocolPtr = std::shared_ptr<Protocol>;

std::string toString(ProtocolType type);
ProtocolType toProtocolType(const std::string& str);
#endif  //LOBBYSERVER_PROTOCOL_H
