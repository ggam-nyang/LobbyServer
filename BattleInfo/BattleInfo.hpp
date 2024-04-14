//
// Created by ggam-nyang on 4/2/24.
//

#ifndef LOBBYSERVER_BATTLEINFO_HPP
#define LOBBYSERVER_BATTLEINFO_HPP

class BattleInfo {
  static const short MAX_HP = 100;
  static const short ATTACK = 15;
  static const short DEFENSE = 5;
  static const short POSITION_X = 0;

 public:
  short max_hp_ = MAX_HP;
  short hp_ = max_hp_;
  short attack_ = ATTACK;
  short defense_ = DEFENSE;
  short position_x_ = POSITION_X; // FIXME: 이동 기능

 public:
  BattleInfo() = default;
  ~BattleInfo() = default;
  void Reset();
};

#endif  //LOBBYSERVER_BATTLEINFO_HPP
