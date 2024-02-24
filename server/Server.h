#ifndef LOBBYSERVER_SERVER_H
#define LOBBYSERVER_SERVER_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <string>
#include <vector>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

using namespace boost;
using std::cout;
using std::endl;
using std::string;

struct Session {
    shared_ptr<asio::ip::tcp::socket> sock;
    asio::ip::tcp::endpoint ep;
    string id;
    int room_no = -1;

    string sbuf;
    string rbuf;
    char buf[80];
//    std::array<char, 80> buffer;
};

class Server {
    asio::io_service io_service_;
    shared_ptr<asio::io_service::work> work_;
    asio::ip::tcp::endpoint endpoint_;
    asio::ip::tcp::acceptor acceptor_;
    std::vector<Session*> sessions_;
    boost::thread_group threadGroup_;
    boost::mutex lock_;
    std::vector<int> existingRooms_;
    const int THREAD_SIZE = 4;

    enum Code {
        INVALID, SET_ID, CREATE_ROOM, SET_ROOM, WHISPER_TO, KICK_ID
    };

public:
    Server(const string& ip_address, unsigned short port_num);

    void Start();

private:
    void WorkerThread();

    void OpenAcceptor();

    void StartAccept(); // 비동기식 Accept

    void OnAccept(const system::error_code &ec, Session *session);

    // 동기식 Receive (쓰레드가 각각의 세션을 1:1 담당)
    void Receive(Session *session);

    void PacketManager(Session *session);

    Code TranslatePacket(string message);

    void SetID(Session *session);

    void CreateRoom(Session *session);

    void SetRoom(Session *session);

    void WhisperTo(Session *session);

    void SendAll(Session *session, int room_no, string message, bool sendToSenderAsWell);

    void OnSend(const system::error_code &ec);

    bool IsTheMessageInNumbers(string message);

    void CloseSession(Session *session);
};

#endif //LOBBYSERVER_SERVER_H
