#include "Server.h"

int main() {
  Server server(asio::ip::address_v4::any().to_string(), 8000);

  server.start();
  return 0;
}