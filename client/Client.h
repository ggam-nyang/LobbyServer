//
// Created by ggam-nyang on 2/16/24.
//

#ifndef BOOSTASIO_CLIENT_H
#define BOOSTASIO_CLIENT_H

#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"
#include <iostream>

using namespace boost;
using boost::asio::ip::tcp;
using std::cout;
using std::endl;

class Client {
    asio::ip::tcp::endpoint ep;
    asio::io_service ios;
    asio::ip::tcp::socket sock;
    shared_ptr<boost::asio::io_service::work> work;
    thread_group thread_group;
    std::string sbuf;
    std::string rbuf;
    char buf[80];
    boost::mutex lock;

public:
    Client(std::string ip_address, unsigned short port_num);

    void Start();

private:
    void WorkerThread();

    void TryConnect();

    void OnConnect(const system::error_code &ec);

    void Send();

    void Receive();

    void SendHandle(const system::error_code &ec);

    void ReceiveHandle(const system::error_code &ec, size_t size);

    void StopAll();
};


#endif //BOOSTASIO_CLIENT_H
