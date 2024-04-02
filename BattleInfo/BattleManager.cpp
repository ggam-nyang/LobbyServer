//
// Created by ggam-nyang on 4/2/24.
//

#include "BattleManager.hpp"
#include "BattleInfo.hpp"

void BattleManager::Attack(std::shared_ptr<BattleInfo> attacker,
                           std::shared_ptr<BattleInfo> defender) {
    short damage = attacker->attack_ - defender->defense_;
    if (damage < 0) {
        damage = 0;
    }

    defender->hp_ -= damage;
    if (defender->hp_ < 0) {
        defender->hp_ = 0;
    }

    if (defender->hp_ == 0) {
        // FIXME: 사망 로직
    }
}
