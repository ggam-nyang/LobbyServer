//
// Created by ggam-nyang on 4/2/24.
//

#ifndef LOBBYSERVER_BATTLEMANAGER_HPP
#define LOBBYSERVER_BATTLEMANAGER_HPP

#include "memory"

class BattleInfo;
class Session;

class BattleManager {
    public:
    BattleManager() = default;
    ~BattleManager() = default;

    void Attack(std::shared_ptr<BattleInfo> attacker, std::shared_ptr<BattleInfo> defender);
//    void Move();
};

#endif  //LOBBYSERVER_BATTLEMANAGER_HPP
