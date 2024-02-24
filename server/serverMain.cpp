#include "Server.h"

int main() {
    Server serv(asio::ip::address_v4::any().to_string(), 8000);
    serv.Start();
    return 0;
}