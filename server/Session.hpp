//
// Created by ggam-nyang on 2/26/24.
//

#ifndef LOBBYSERVER_SESSION_H
#define LOBBYSERVER_SESSION_H

#include <boost/thread.hpp>
#include <iostream>
#include <memory>
#include "Protocol.h"
#include "boost/asio.hpp"

class Server;
using std::string;

class Session : public std::enable_shared_from_this<Session> {
 public:
  using pointer = std::shared_ptr<Session>;

  boost::asio::ip::tcp::socket socket_;
  Server* server_;
  string id;
  string name;
  string writeBuffer;
  string readBuffer;
  std::array<char, 80> buffer;
  int room_no = -1;  // FIXME: room class로 변경

  static pointer create(boost::asio::io_context& io_context, Server* server);

  // FIXME: private으로 하고 싶은데, 그러면 create에서 접근이 안됨
  explicit Session(boost::asio::io_context& io_context, Server* server);

  void read();
  void write(ProtocolPtr& protocolPtr);
  void onWrite(const boost::system::error_code& ec);
  void writeAll(ProtocolPtr& protocolPtr, bool isExceptMe = true);

  void protocolManage(ProtocolPtr& protocolPtr);
  void setId(string& body);
  void chat(string& body);
  void alert(string& body);

  void close();
};

#endif  //LOBBYSERVER_SESSION_H
