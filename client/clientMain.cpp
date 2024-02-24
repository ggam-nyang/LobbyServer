#include "Client.h"


int main(int argc, char *argv[]) {
    Client chat("127.0.0.1", 8000);
    chat.Start();

    return 0;
}