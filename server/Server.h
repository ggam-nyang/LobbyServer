#ifndef LOBBYSERVER_SERVER_H
#define LOBBYSERVER_SERVER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../Protocol/Packet/PacketManager.hpp"
#include "Session.hpp"

using namespace boost;
using std::cout;
using std::endl;
using std::string;

class Lobby;

class Server {
    asio::io_service io_context_;
    shared_ptr<asio::io_service ::work> work_;
    asio::ip::tcp::endpoint endpoint_;
    asio::ip::tcp::acceptor acceptor_;
    std::vector<Session::pointer> sessions_{};
    boost::thread_group threadGroup_;
    std::mutex lock_;
    std::unordered_map<int, std::shared_ptr<Lobby>> lobbies_;
    std::vector<int> existingRooms_;
    PacketManager packetManager_;
    const int THREAD_SIZE = 4;

    friend class Session;
public:
    Server(const string& ip_address, unsigned short port_num);

    void start();
    void closeSession(Session::pointer session);

private:
    void workerThread();

    void openAcceptor();

    void startAccept(); // 비동기식 Accept

    void onAccept(const system::error_code &ec, Session::pointer session);

    // 동기식 Read (쓰레드가 각각의 세션을 1:1 담당)
    void read(Session::pointer session);

    bool isValidName(string& name);

    void writeAll(ProtocolPtr& protocolPtr, Session::pointer session, bool isExceptMe = true);

};

#endif //LOBBYSERVER_SERVER_H
