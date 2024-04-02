//
// Created by ggam-nyang on 4/2/24.
//

#ifndef LOBBYSERVER_BATTLEINFO_HPP
#define LOBBYSERVER_BATTLEINFO_HPP

class BattleInfo {
 public:
  short max_hp_ = 100;
  short hp_ = max_hp_;
  short attack_ = 15;
  short defense_ = 5;
  short position_x_ = 0; // FIXME: 이동 로직 구현

 public:
  BattleInfo() = default;
  ~BattleInfo() = default;




};

#endif  //LOBBYSERVER_BATTLEINFO_HPP
