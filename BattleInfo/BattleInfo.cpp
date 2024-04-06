//
// Created by ggam-nyang on 4/2/24.
//

#include "BattleInfo.hpp"

void BattleInfo::Reset() {
    hp_ = max_hp_;
    attack_ = ATTACK;
    defense_ = DEFENSE;
    position_x_ = POSITION_X;
}
