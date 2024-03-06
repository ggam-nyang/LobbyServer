//
// Created by ggam-nyang on 2/27/24.
//

#ifndef LOBBYSERVER_PROTOCOL_H
#define LOBBYSERVER_PROTOCOL_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

enum class ProtocolType : uint32_t { SET_ID, CHAT, ALERT, UNKNOWN };

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

  // 인코드에서 사용하는 std::string에 메모리 영역을 Protocol의 라이프 사이클과 같이 유지해주기 위해서, 멤버 변수로 변경하고 이를 반환하게 했습니다
  std::string encoded_data_;
};

// shared_ptr로 변환한 이유는 life-cycle 때문입니다
using ProtocolPtr = std::shared_ptr<Protocol>;

std::string toString(ProtocolType type);
ProtocolType toProtocolType(const std::string& str);
#endif  //LOBBYSERVER_PROTOCOL_H
