#include "menu.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

class NoclipInfo : public CCNode {
public:
    CCLabelBMFont* m_deaths;
    CCLabelBMFont* m_acc;

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

        m_deaths = CCLabelBMFont::create("Deaths: 0", "chatFont.fnt");
        m_deaths->setScale(0.45f);
        m_deaths->setAnchorPoint({0, 0.5f});
        m_deaths->setPosition({5, win.height - 60});
        this->addChild(m_deaths);

        m_acc = CCLabelBMFont::create("Acc: 100.0%", "chatFont.fnt");
        m_acc->setScale(0.45f);
        m_acc->setAnchorPoint({0, 0.5f});
        m_acc->setPosition({5, win.height - 73});
        this->addChild(m_acc);

        this->schedule(schedule_selector(NoclipInfo::tick), 0.1f);
        return true;
    }

    void tick(float) {
        m_deaths->setString(fmt::format("Deaths: {}", g_deaths).c_str());
        m_acc->setString(fmt::format("Acc: {:.1f}%", g_accuracy).c_str());
    }
};

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
    }

    void update(float dt) {
        PlayLayer::update(dt);
        m_fields->totalFrames++;
    }

    void killMe(float) {
        PlayLayer::destroyPlayer(this->m_player1, nullptr);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt * g_speed);
    }
};

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        addMegaHackButton(this);
        return true;
    }
};
