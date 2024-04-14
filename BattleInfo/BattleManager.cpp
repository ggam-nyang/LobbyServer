//
// Created by ggam-nyang on 4/2/24.
//

#include "BattleManager.hpp"
#include "BattleInfo.hpp"
#include "../server/Session.hpp"

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

    }
}

void BattleManager::Attack(std::shared_ptr<Session> attacker,
                           std::shared_ptr<Session> defender) {
    short damage = attacker->battleInfo_->attack_ - defender->battleInfo_->defense_;
    if (damage < 0) {
        damage = 0;
    }

    defender->battleInfo_->hp_ -= damage;
    if (defender->battleInfo_->hp_ < 0) {
        defender->battleInfo_->hp_ = 0;
    }

    if (defender->battleInfo_->hp_ == 0) {
      defender->state_ = USER_STATE::DIED;
    }
}
