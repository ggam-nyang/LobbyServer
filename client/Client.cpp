//
// Created by ggam-nyang on 2/16/24.
//

#include "Client.h"
#include "ClientPacketManager.hpp"


Client::Client(std::string ip_address, unsigned short port_num)
    : endpoint_(asio::ip::address::from_string(ip_address), port_num),
      packetManager_(this),
      sock(io_context_, endpoint_.protocol()),
      work_(new asio::io_service::work(io_context_)),
      input_(io_context_, STDIN_FILENO),
      timer_(io_context_) {}

void Client::Start() {
  for (int i = 0; i < 4; i++)
    thread_group_.create_thread([this]() { WorkerThread(); });

  // thread 잘 만들어질때까지 잠시 기다리는 부분
  this_thread::sleep_for(chrono::milliseconds(100));

  io_context_.post([this]() { TryConnect(); });

  thread_group_.join_all();
}

void Client::WorkerThread() {
  lock_.lock();
  cout << "[" << std::this_thread::get_id() << "]"
       << " Thread start" << endl;
  lock_.unlock();

  io_context_.run();

  lock_.lock();
  cout << "[" << std::this_thread::get_id() << "]"
       << " Thread End" << endl;
  lock_.unlock();
}

void Client::TryConnect() {
  cout << "[" << std::this_thread::get_id() << "]"
       << " TryConnect" << endl;

  sock.async_connect(endpoint_, boost::bind(&Client::OnConnect, this, _1));
}

void Client::OnConnect(const system::error_code& ec) {
  cout << "[" << std::this_thread::get_id() << "]"
       << " OnConnect" << endl;
  if (ec) {
    cout << "connect failed: " << ec.message() << endl;
    StopAll();
    return;
  }

  io_context_.post([this]() { Send(); });
  io_context_.post([this]() { Receive(); });
}

void Client::Send() {
  getline(std::cin, writeBuffer_);
  packetManager_.SendPacket(this, writeBuffer_);
}

void Client::Receive() {
  sock.async_read_some(asio::buffer(buffer_, buffer_.size()),
                       [this](const system::error_code& ec, size_t size) {
                         ReceiveHandle(ec, size);
                       });
}

void Client::SendHandle(const system::error_code& ec, const char* packet) {
  if (ec) {
    cout << "async_write_some error: " << ec.message() << endl;
    StopAll();
    return;
  }

  io_context_.post([this]() { Send(); });
}

void Client::ReceiveHandle(const system::error_code& ec, size_t size) {
  if (ec) {
    cout << "async_read_some error: " << ec.message() << endl;
    StopAll();
    return;
  }

  if (size == 0) {
    cout << "Server wants to close this session" << endl;
    StopAll();
    return;
  }

  packetManager_.ReceivePacket(this, buffer_.data(), size);
//  Receive();
  io_context_.post([this]() { Receive(); });
}

void Client::StopAll() {
  sock.close();
  work_.reset();
}

void Client::Battle() {
  if (state_ != USER_STATE::BATTLE)
    return;

  timer_.expires_from_now(std::chrono::seconds(1));
  timer_.async_wait(
      [this](const boost::system::error_code& ec) { BattleHandler(ec); });
}

void Client::BattleHandler(const system::error_code& ec) {
  if (state_ != USER_STATE::BATTLE) {
    io_context_.post([this]() { Send(); });
    return;
  }

  packetManager_.SendAttackPacket(this, 1);

  timer_.expires_from_now(std::chrono::seconds(1));
  timer_.async_wait(
      [this](const boost::system::error_code& ec) { BattleHandler(ec); });
}

void Client::ResponseSetName(SET_NAME_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "이미 사용중인 이름입니다." << endl;
  } else if (packet.result == 1) {
    cout << "이름이 설정되었습니다." << endl;
  }
}

void Client::ResponseEnterLobby(LOBBY_ENTER_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "이미 로비에 입장했습니다." << endl;
  } else if (packet.result == 1) {
    cout << "로비에 입장했습니다." << endl;
    state_ = USER_STATE::LOBBY;
  }
}

void Client::ResponseCreateRoom(ROOM_CREATE_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "이미 방에 입장한 상태입니다." << endl;
  } else if (packet.result == 1) {
    cout << "방 생성에 성공했습니다." << endl;
  } else if (packet.result == 2) {
    cout << "더 이상 방을 생성할 수 없습니다." << endl;
  }
}

void Client::ResponseRoomList(ROOM_LIST_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "로비에 입장하지 않았습니다." << endl;
  } else if (packet.result == 1) {
    cout << "방 목록을 출력합니다." << endl;

    for (int i = 0; i < packet.roomCount; ++i) {
      cout << "id: " << packet.roomList[i].id
           << " name: " << packet.roomList[i].name << endl;
    }
  }
}

void Client::ResponseEnterRoom(ROOM_ENTER_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "이미 방에 입장한 상태입니다." << endl;
  } else if (packet.result == 1) {
    cout << "방에 입장했습니다." << endl;
    state_ = USER_STATE::ROOM;
  } else if (packet.result == 2) {
    cout << "로비에 입장하지 않았습니다." << endl;
  } else if (packet.result == 3) {
    cout << "방이 꽉 찼습니다." << endl;
  } else if (packet.result == 4) {
    cout << "해당 방이 존재하지 않습니다." << endl;
  }
}

void Client::ResponseEnterRoomBroadcast(ROOM_ENTER_BROADCAST_PACKET packet) {
  cout << "방에 " << packet.username << "님이 입장했습니다." << endl;
}

void Client::ResponseLeaveRoom(ROOM_LEAVE_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "방에 입장하지 않았습니다." << endl;
  } else if (packet.result == 1) {
    cout << "방에서 나갔습니다." << endl;
    state_ = USER_STATE::LOBBY;
  } else if (packet.result == 2) {
    cout << "로비에 입장하지 않았습니다." << endl;
  }
}

void Client::ResponseLeaveRoomBroadcast(ROOM_LEAVE_BROADCAST_PACKET packet) {
  cout << "방에서 " << packet.username << "님이 나갔습니다." << endl;
}

void Client::ResponseChat(CHAT_RESPONSE_PACKET packet) {
  cout << "[" << packet.username << "] : " << packet.chat << endl;
}

void Client::ResponseReady(ROOM_READY_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "방에 입장하지 않았습니다." << endl;
    return;
  }

  if (packet.result == 1) {
    if (state_ == USER_STATE::READY) {
      cout << "준비가 해제 되었습니다." << endl;
      state_ = USER_STATE::ROOM;
    } else if (state_ == USER_STATE::ROOM) {
      cout << "준비가 완료 되었습니다." << endl;
      state_ = USER_STATE::READY;
    }
  }
}

void Client::ResponseBattleStart(BATTLE_START_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "방에 입장하지 않았습니다." << endl;
  } else if (packet.result == 1) {
    cout << "배틀이 시작되었습니다." << endl;
    state_ = USER_STATE::BATTLE;
    Battle();
  } else if (packet.result == 2) {
    cout << "방장이 아닙니다." << endl;
  } else if (packet.result == 3) {
    cout << "준비가 되지 않아 배틀을 시작할 수 없습니다." << endl;
  } else if (packet.result == 4) {
    cout << "인원이 1명인 경우에는 배틀을 시작할 수 없습니다." << endl;
  }
}

void Client::ResponseAttack(ATTACK_RESPONSE_PACKET packet) {
  if (packet.result == 0) {
    cout << "배틀 중이 아닐 땐, 공격 요청할 수 없습니다." << endl;
  } else if (packet.result == 1) {
    for (int i = 0; i < 2; ++i) {
      cout << "Player " << packet.battleInfos[i].username
           << " MaxHP: " << packet.battleInfos[i].maxHp
           << " Hp: " << packet.battleInfos[i].hp << endl;
    }
  }
}

void Client::ResponseBattleInfo(BATTLE_INFO_PACKET packet) {
  if (packet.result == 0) {
    cout << "배틀 중이 아닐 땐, 공격 요청할 수 없습니다." << endl;
    return;
  }

  for (int i = 0; i < 2; ++i) {
    cout << "Player " << packet.battleInfos[i].username
         << " MaxHP: " << packet.battleInfos[i].maxHp
         << " Hp: " << packet.battleInfos[i].hp << endl;
  }

  if (packet.result == 2 || state_ != USER_STATE::BATTLE) {
    cout << "배틀이 종료되었습니다." << endl;
    state_ = USER_STATE::ROOM;
  }
}
