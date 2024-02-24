//
// Created by ggam-nyang on 2/22/24.
//

#include "Server.h"

Server::Server(const string& ip_address, unsigned short port_num) :
        work_(new asio::io_service::work(io_service_)),
        endpoint_(asio::ip::address::from_string(ip_address), port_num),
        acceptor_(io_service_, endpoint_.protocol()) {
    existingRooms_.push_back(0);
}

void Server::Start() {
    cout << "Start Server" << endl;
    cout << "Creating Threads" << endl;
    for (int i = 0; i < THREAD_SIZE; i++)
        threadGroup_.create_thread([this] { WorkerThread(); });

    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "Threads Created" << endl;

    io_service_.post([this] { OpenAcceptor(); });

    threadGroup_.join_all();
}

void Server::WorkerThread() {
    lock_.lock();
    cout << "[" << boost::this_thread::get_id() << "]" << " Thread Start" << endl;
    lock_.unlock();

    io_service_.run();

    lock_.lock();
    cout << "[" << boost::this_thread::get_id() << "]" << " Thread End" << endl;
    lock_.unlock();
}

void Server::OpenAcceptor() {
    system::error_code ec;
    ec = acceptor_.bind(endpoint_, ec);
    if (ec) {
        cout << "bind failed: " << ec.message() << endl;
        return;
    }

    acceptor_.listen();
    cout << "Acceptor Opened" << endl;

    StartAccept();
    cout << "[" << boost::this_thread::get_id() << "]" << " Start Accepting" << endl;
}

void Server::StartAccept() {
    auto *session = new Session();
    shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(io_service_));
    session->sock = sock;
    acceptor_.async_accept(*sock, session->ep, bind(&Server::OnAccept, this, _1, session));
}

void Server::OnAccept(const system::error_code &ec, Session *session) {
    if (ec) {
        cout << "accept failed: " << ec.message() << endl;
        return;
    }

    lock_.lock();
    sessions_.push_back(session);
    cout << "[" << boost::this_thread::get_id() << "]" << " Client Accepted" << endl;
    lock_.unlock();

    io_service_.post([this, session] { Receive(session); });
    StartAccept();
}

void Server::Receive(Session *session) {
    system::error_code ec;
    size_t size;
    size = session->sock->read_some(asio::buffer(session->buf, sizeof(session->buf)), ec);

    if (ec) {
        cout << "[" << boost::this_thread::get_id() << "] read failed: " << ec.message() << endl;
        CloseSession(session);
        return;
    }

    if (size == 0) {
        cout << "[" << boost::this_thread::get_id() << "] peer wants to end " << endl;
        CloseSession(session);
        return;
    }

    session->buf[size] = '\0';
    session->rbuf = session->buf;
    PacketManager(session);
    cout << "[" << boost::this_thread::get_id() << "] " << session->rbuf << endl;

    Receive(session);
}

void Server::PacketManager(Session *session) {
    // :~ 라는 특수(?)메세지를 보내왔을 경우 처리
    if (session->buf[0] == ':') {
        Code code = TranslatePacket(session->rbuf);

        switch (code) {
            case Code::SET_ID:
                SetID(session);
                break;
            case Code::CREATE_ROOM:
                CreateRoom(session);
                break;
            case Code::SET_ROOM:
                SetRoom(session);
                break;
            case Code::WHISPER_TO:
                WhisperTo(session);
                break;
            case Code::KICK_ID:
                // 미구현
                break;
            case Code::INVALID:
                session->sbuf = "유효하지 않은 명령어 입니다";
                session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
                break;
        }
    } else  // :~ 라는 특수메세지가 아니고 그냥 채팅일 경우
    {
        if (session->id.length() != 0) // id length가 0인 경우는 id를 아직 등록하지 않은 경우
        {
            string temp = "[" + session->id + "]:" + session->rbuf;
            SendAll(session, session->room_no, temp, false);
        } else {
            session->sbuf = ":set 을 통해 아이디를 먼저 등록하세요";
            session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
        }
    }
}

Server::Code Server::TranslatePacket(string message) {
    string temp = message.substr(0, sizeof(":set ") - 1);
    // :set 일 경우
    if (temp.compare(":set ") == 0) {
        return Code::SET_ID;
    }

    temp = message.substr(0, sizeof(":createRoom ") - 1);
    if (temp.compare(":createRoom ") == 0) {
        return Code::CREATE_ROOM;
    }

    temp = message.substr(0, sizeof(":setRoom ") - 1);
    if (temp.compare(":setRoom ") == 0) {
        return Code::SET_ROOM;
    }

    temp = message.substr(0, sizeof(":to ") - 1);
    if (temp.compare(":to ") == 0) {
        return Code::WHISPER_TO;
    }

    temp = message.substr(0, sizeof(":kick ") - 1);
    if (temp.compare(":kick ") == 0) {
        return Code::KICK_ID;
    }

    return Code::INVALID;
}

void Server::SetID(Session *session) {
    string temp = session->rbuf.substr(sizeof(":set ") - 1, session->rbuf.length());
    // 중복된 아이디인지 체크
    for (int i = 0; i < sessions_.size(); i++) {
        if (temp.compare(sessions_[i]->id) == 0) {
            session->sbuf = "set falied: [" + temp + "]는 이미 사용중인 아이디 입니다";
            session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
            return;
        }
    }

    session->id = temp;
    session->sbuf = "set [" + temp + "] success!";
    session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));

    if (session->room_no == -1) {
        session->room_no = 0;
        SendAll(session, 0, "[" + session->id + "] 님이 로비에 입장하였습니다", false);
    } else {
        SendAll(session, session->room_no, "[" + session->id + "] 님이 아이디를 변경하였습니다", false);
    }
}

void Server::CreateRoom(Session *session) {
    if (session->room_no != 0) {
        session->sbuf = "creatRoom falied: 방은 로비에서만 생성할 수 있습니다";
        session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
        return;
    }

    string temp = session->rbuf.substr(sizeof(":createRoom ") - 1, session->rbuf.length());
    // 메세지의 방번호 부분이 정수가 맞는지 체크
    if (IsTheMessageInNumbers(temp) == false) {
        session->sbuf = "creatRoom falied: [" + temp + "]는 유효하지 않는 값입니다";
        session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
        return;
    }

    int num = atoi(temp.c_str());
    // 이미 존재하는 방인지 체크
    for (int i = 0; i < existingRooms_.size(); i++) {
        if (existingRooms_[i] == num) {
            session->sbuf = "creatRoom falied: [" + temp + "]번 방은 이미 존재합니다";
            session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
            return;
        }
    }

    lock_.lock();
    existingRooms_.push_back(num);
    lock_.unlock();

    session->room_no = num;
    session->sbuf = "createRoom [" + temp + "] success!";
    session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));

    SendAll(session, 0, "[" + temp + "]번 방이 생성되었습니다", false);

    session->sbuf = "[" + temp + "]번 방에 입장하였습니다";
    session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
}

void Server::SetRoom(Session *session) {
    if (session->id.length() == 0) {
        session->sbuf = ":set 을 통해 아이디를 먼저 등록하세요";
        session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
        return;
    }

    string temp = session->rbuf.substr(sizeof(":setRoom ") - 1, session->rbuf.length());
    // 메세지의 방번호 부분이 정수가 맞는지 체크
    if (IsTheMessageInNumbers(temp) == false) {
        session->sbuf = "setRoom falied: [" + temp + "]는 유효하지 않는 값입니다";
        session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
        return;
    }

    int num = atoi(temp.c_str());
    if (session->room_no == num) {
        session->sbuf = "setRoom falied: 이미 [" + temp + "]번 방에 있습니다";
        session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
        return;
    }

    // 존재하는 방인지 체크
    for (int i = 0; i < existingRooms_.size(); i++) {
        if (existingRooms_[i] == num) {
            SendAll(session, session->room_no, "[" + session->id + "] 님이 방을 나갔습니다", false);
            session->room_no = num;

            if (num == 0)
                session->sbuf = "로비로 이동하였습니다";
            else
                session->sbuf = "[" + temp + "]번 방으로 이동하였습니다";

            session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
            return;
        }
    }

    session->sbuf = "setRoom falied: [" + temp + "]번 방이 존재하지 않습니다";
    session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
}

void Server::WhisperTo(Session *session) {
    if (session->id.length() == 0) {
        session->sbuf = ":set 을 통해 아이디를 먼저 등록하세요";
        session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
        return;
    }

    string temp = session->rbuf.substr(sizeof(":to ") - 1, session->rbuf.length());
    int num = 0;
    num = temp.find_first_of(' ');
    if (num == 0) {
        session->sbuf = "아이디와 메세지 사이 띄워쓰기를 해주세요";
        session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
        return;
    }

    string temp2 = temp.substr(0, num);
    for (int i = 0; i < sessions_.size(); i++) {
        if (sessions_[i]->id.compare(temp2) == 0) {
            sessions_[i]->sbuf = "from [" + session->id + "]:" + temp.substr(num + 1, temp.length());
            sessions_[i]->sock->async_write_some(asio::buffer(sessions_[i]->sbuf),
                                                 bind(&Server::OnSend, this, _1));
            return;
        }
    }

    session->sbuf = "아이디를 찾을 수 없습니다";
    session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
}

void Server::SendAll(Session *session, int room_no, string message, bool sendToSenderAsWell) {
    // 같은 방에 있는 다른 모든 클라이언트들에게 보낸다
    for (int i = 0; i < sessions_.size(); i++) {
        if ((session->sock != sessions_[i]->sock) && (room_no == sessions_[i]->room_no)) {
            sessions_[i]->sbuf = message;
            sessions_[i]->sock->async_write_some(asio::buffer(sessions_[i]->sbuf),
                                                 bind(&Server::OnSend, this, _1));
        }
    }

    // 메세지를 보내온 클라이언트에게도 보낸다
    if (sendToSenderAsWell) {
        session->sbuf = message;
        session->sock->async_write_some(asio::buffer(session->sbuf), bind(&Server::OnSend, this, _1));
    }
}

void Server::OnSend(const system::error_code &ec) {
    if (ec) {
        cout << "[" << boost::this_thread::get_id() << "] async_write_some failed: " << ec.message() << endl;
        return;
    }
}

bool Server::IsTheMessageInNumbers(string message) {
    const char *cTemp = message.c_str();

    // 메세지 내용(방번호)이 정수가 아닐 경우
    for (int i = 0; i < message.length(); i++) {
        if (cTemp[i] < '0' || cTemp[i] > '9') {
            return false;
        }
    }

    return true;
}


void Server::CloseSession(Session *session) {
    if (session->room_no != -1) {
        SendAll(session, 0, "[" + session->id + "]" + "님이 종료하였습니다", false);
        SendAll(session, session->room_no, "[" + session->id + "]" + "님이 방을 나갔습니다", false);
    }

    // if session ends, close and erase
    for (int i = 0; i < sessions_.size(); i++) {
        if (sessions_[i]->sock == session->sock) {
            lock_.lock();
            sessions_.erase(sessions_.begin() + i);
            lock_.unlock();
            break;
        }
    }

    string temp = session->id;
    session->sock->close();
    delete session;
}