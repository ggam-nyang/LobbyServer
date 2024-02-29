#include "Server.h"

int main() {
//  ProtocolPtr temp = Protocol::create(ProtocolType::SET_ID, "ID123");
//  string test = temp->encode();
//  ProtocolPtr decode = Protocol::create(test);

  Server serv(asio::ip::address_v4::any().to_string(), 8000);

  serv.start();
  return 0;
}