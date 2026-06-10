#include "menu.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
using namespace geode::prelude;

// ===== ИНФО НОУКЛИПА =====
class NoclipInfo : public CCNode {
public:
    CCLabelTTF* m_deaths;
    CCLabelTTF* m_acc;

    static NoclipInfo* create() {
        auto ret = new NoclipInfo();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCNode::init()) return false;
        auto win = CCDirector::get()->getWinSize();

        auto bg = CCLayerColor::create({0,0,0,120}, 120, 30);
        bg->setPosition({0, win.height - 58});
        this->addChild(bg);

        m_deaths = CCLabelTTF::create("Deaths: 0", "Arial", 9);
        m_deaths->setAnchorPoint({0, 0.5f});
        m_deaths->setPosition({4, win.height - 47});
        m_deaths->setColor({255, 100, 100});
        this->addChild(m_deaths);

        m_acc = CCLabelTTF::create("Acc: 100.0%", "Arial", 9);
        m_acc->setAnchorPoint({0, 0.5f});
        m_acc->setPosition({4, win.height - 58});
        m_acc->setColor({100, 255, 100});
        this->addChild(m_acc);

        this->schedule(schedule_selector(NoclipInfo::tick), 0.1f);
        return true;
    }

    void tick(float) {
        m_deaths->setString(fmt::format("Deaths: {}", g_deaths).c_str());
        m_acc->setString(fmt::format("Acc: {:.1f}%", g_accuracy).c_str());
    }
};

// ===== ХУКИ =====
class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        int totalFrames = 0;
        int deathFrames = 0;
    };

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        if (g_noclip) {
            g_deaths++;
            m_fields->deathFrames++;
            if (m_fields->totalFrames > 0) {
                g_accuracy = 100.0f * (1.0f -
                    (float)m_fields->deathFrames / m_fields->totalFrames);
            }
            return;
        }
        PlayLayer::destroyPlayer(p, o);
        if (g_farm) {
            this->scheduleOnce(
                schedule_selector(MyPlayLayer::killMe), 0.1f);
        }
    }

    void startGame() {
        PlayLayer::startGame();
        g_deaths = 0;
        g_accuracy = 100.0f;
        m_fields->totalFrames = 0;
        m_fields->deathFrames = 0;

        addMegaHackButton(this);
        this->addChild(NoclipInfo::create(), 999);

        // Instant Complete
        if (g_instantComplete) {
            this->scheduleOnce(
                schedule_selector(MyPlayLayer::doInstantComplete), 0.3f);
        }
    }

    void update(float dt) {
        PlayLayer::update(dt);
        m_fields->totalFrames++;
    }

    void killMe(float) {
        PlayLayer::destroyPlayer(this->m_player1, nullptr);
    }

    void doInstantComplete(float) {
        this->m_player1->setPositionX(99999.f);
    }
};

// ===== SPEEDHACK =====
class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt * g_speed);
    }
};

// ===== КНОПКА В ГЛАВНОМ МЕНЮ =====
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        addMegaHackButton(this);
        return true;
    }
};
