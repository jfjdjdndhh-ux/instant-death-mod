#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
using namespace geode::prelude;

class $modify(PlayLayer) {
    void startGame() {
        PlayLayer::startGame();
        this->m_player1->playDeathEffect();
    }
};
