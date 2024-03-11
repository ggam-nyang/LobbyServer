//
// Created by ggam-nyang on 2/16/24.
//

#ifndef BOOSTASIO_CLIENT_H
#define BOOSTASIO_CLIENT_H

#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"
#include <iostream>

#include "../Protocol/Protocol.h"

using namespace boost;
using boost::asio::ip::tcp;
using std::cout;
using std::endl;

class Client {
    asio::ip::tcp::endpoint endpoint_;
    asio::io_context io_context_;
    asio::ip::tcp::socket sock;
    shared_ptr<boost::asio::io_service::work> work_;
    thread_group thread_group_;
    std::string writeBuffer_;
    std::string readBuffer_;
    std::array<char, 80> buffer_;
    std::mutex lock_;
    bool isSetId = false;
    int id;

public:
    Client(std::string ip_address, unsigned short port_num);

    void Start();

private:
    void WorkerThread();

    void TryConnect();

    void OnConnect(const system::error_code &ec);

    void Send();

    void Receive();

    void SendHandle(const system::error_code& ec, const ProtocolPtr& protocol);

    void ReceiveHandle(const system::error_code &ec, size_t size);

    void StopAll();
};


#endif //BOOSTASIO_CLIENT_H
