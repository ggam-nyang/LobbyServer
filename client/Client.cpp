//
// Created by ggam-nyang on 2/16/24.
//

#include "Client.h"


Client::Client(std::string ip_address, unsigned short port_num) :
        ep(asio::ip::address::from_string(ip_address), port_num),
        sock(ios, ep.protocol()),
        work(new asio::io_service::work(ios)) {}

void Client::Start() {
    for (int i = 0; i < 3; i++)
        thread_group.create_thread(bind(&Client::WorkerThread, this));

    // thread 잘 만들어질때까지 잠시 기다리는 부분
    this_thread::sleep_for(chrono::milliseconds(100));

    ios.post(bind(&Client::TryConnect, this));

    thread_group.join_all();
}

void Client::WorkerThread() {
    lock.lock();
    cout << "[" << boost::this_thread::get_id() << "]" << " Thread Start" << endl;
    lock.unlock();

    ios.run();

    lock.lock();
    cout << "[" << boost::this_thread::get_id() << "]" << " Thread End" << endl;
    lock.unlock();
}

void Client::TryConnect() {
    cout << "[" << boost::this_thread::get_id() << "]" << " TryConnect" << endl;

    sock.async_connect(ep, boost::bind(&Client::OnConnect, this, _1));
}

void Client::OnConnect(const system::error_code &ec) {
    cout << "[" << boost::this_thread::get_id() << "]" << " OnConnect" << endl;
    if (ec) {
        cout << "connect failed: " << ec.message() << endl;
        StopAll();
        return;
    }

    ios.post(bind(&Client::Send, this));
    ios.post(bind(&Client::Receive, this));
}

void Client::Send() {
    getline(std::cin, sbuf);
    sock.async_write_some(asio::buffer(sbuf), bind(&Client::SendHandle, this, _1));
}

void Client::Receive() {
    sock.async_read_some(asio::buffer(buf), bind(&Client::ReceiveHandle, this, _1, _2));
}

void Client::SendHandle(const system::error_code &ec) {
    if (ec) {
        cout << "async_read_some error: " << ec.message() << endl;
        StopAll();
        return;
    }

    Send();
}

void Client::ReceiveHandle(const system::error_code &ec, size_t size) {
    if (ec) {
        cout << "async_write_some error: " << ec.message() << endl;
        StopAll();
        return;
    }

    if (size == 0) {
        cout << "Server wants to close this session" << endl;
        StopAll();
        return;
    }

    buf[size] = '\0';
    rbuf = buf;

    lock.lock();
    cout << rbuf << endl;
    lock.unlock();

    Receive();
}

void Client::StopAll() {
    sock.close();
    work.reset();
}