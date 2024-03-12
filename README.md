# Lobby Server

## 프로젝트 소개
간단한 로비 서버, 클라이언트를 구현한 프로젝트입니다.  
로비 접속, 방 생성, 방 입장, 채팅 등의 기능을 제공합니다. 


## 설치 및 실행
1. Cmake 설치  
   [공식 사이트](https://cmake.org/download/)에서 최신 버전을 다운로드하여 설치합니다.
2. 프로젝트 빌드  
   CLI 실행법입니다.
```bash
$ gh repo clone ggam-nyang/LobbyServer
$ cd LobbyServer
$ cmake -S . -B build
$ cmake --build build
```

3. 실행  
   생성된 LobbyServer.exe 실행 + LobbyClient.exe 실행 (최소 2개 이상)

## 구현된 기능
#### Client
- 최초 접속 시, nickname을 입력합니다.
- :enter_lobby  메세지를 통해 로비에 입장합니다. (현재 로비는 한개입니다)
- 로비 진입
  - 로비 내부에서 기능은 다음과 같습니다.
  - :create_room => 방을 생성합니다. (방 이름은 자동으로 생성됩니다.)
  - :enter_room [방이름] => 방에 입장합니다.
  - :room_list => 현재 생성된 방의 목록을 확인합니다.
  - :를 제외하고 입력 시, 로비에 채팅
- 방 진입
  - :leave_room => 방을 나갑니다.
  - :를 제외하고 입력 시, 방에 채팅


## 시퀀스 다이어그램
![스크린샷 2024-03-12 21.04.59.png](..%2F..%2F..%2FDesktop%2F%EC%8A%A4%ED%81%AC%EB%A6%B0%EC%83%B7%202024-03-12%2021.04.59.png)
![스크린샷 2024-03-12 21.05.02.png](..%2F..%2F..%2FDesktop%2F%EC%8A%A4%ED%81%AC%EB%A6%B0%EC%83%B7%202024-03-12%2021.05.02.png)
![스크린샷 2024-03-12 21.05.08.png](..%2F..%2F..%2FDesktop%2F%EC%8A%A4%ED%81%AC%EB%A6%B0%EC%83%B7%202024-03-12%2021.05.08.png)