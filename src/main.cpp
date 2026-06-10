#include <Geode/Geode.hpp>
#include "menu.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

// Из menu.cpp
extern bool g_noclip;
extern bool g_farm;
extern bool g_safeMode;
extern float g_speed;
extern int g_deaths;
extern float g_accuracy;
extern bool g_menuOpen;
class MegaHackButton;

// Инфо слева сверху
class NoclipInfo : public CCNode {
public:
    CCLabelBMFont* m_deathsLabel;
    CCLabelBMFont* m_accLabel;

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

        m_deathsLabel = CCLabelBMFont::create("Deaths: 0", "chatFont.fnt");
        m_deathsLabel->setScale(0.5f);
        m_deathsLabel->setAnchorPoint({0, 0.5f});
        m_deathsLabel->setPosition({5, win.height - 60});
        this->addChild(m_deathsLabel);

        m_accLabel = CCLabelBMFont::create("Acc: 100%", "chatFont.fnt");
        m_accLabel->setScale(0.5f);
        m_accLabel->setAnchorPoint({0, 0.5f});
        m_accLabel->setPosition({5, win.height - 75});
        this->addChild(m_accLabel);

        this->schedule(schedule_selector(NoclipInfo::update2), 0.1f);
        return true;
    }

    void update2(float) {
        m_deathsLabel->setString(
            fmt::format("Deaths: {}", g_deaths).c_str());
        m_accLabel->setString(
            fmt::format("Acc: {:.1f}%", g_accuracy).c_str());
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        int totalFrames = 0;
        int deathFrames = 0;
        NoclipInfo* infoNode = nullptr;
        bool started = false;
    };

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        if (g_noclip) {
            g_deaths++;
            // Пересчёт точности
            m_fields->deathFrames++;
            if (m_fields->totalFrames > 0) {
                g_accuracy = 100.0f * (1.0f - 
                    (float)m_fields->deathFrames / m_fields->totalFrames);
            }
            return;
        }
        if (g_farm) {
            PlayLayer::destroyPlayer(p, o);
            return;
        }
        PlayLayer::destroyPlayer(p, o);
    }

    void startGame() {
        PlayLayer::startGame();
        g_deaths = 0;
        g_accuracy = 100.0f;
        m_fields->totalFrames = 0;
        m_fields->deathFrames = 0;

        // Кнопка меню
        auto btn = MegaHackButton::create();
        this->addChild(btn, 999);

        // Инфо ноуклипа
        auto info = NoclipInfo::create();
        m_fields->infoNode = info;
        this->addChild(info, 999);

        // Фарм попыток
        if (g_farm && !g_noclip) {
            this->scheduleOnce(
                schedule_selector(MyPlayLayer::killMe), 0.1f);
        }
    }

    void update(float dt) {
        PlayLayer::update(dt);
        m_fields->totalFrames++;
    }

    void killMe(float) {
        PlayLayer::destroyPlayer(this->m_player1, nullptr);
    }
};

// Speedhack
class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt * g_speed);
    }
};

// Кнопка в главном меню
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto win = CCDirector::get()->getWinSize();
        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto spr = CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
        spr->setScale(0.6f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MenuLayer::onMegahack));
        btn->setPosition({30, win.height - 30});
        menu->addChild(btn);

        this->addChild(menu, 999);
        return true;
    }

    void onMegahack(CCObject*) {
        if (g_menuOpen) return;
        CCDirector::get()->getRunningScene()->addChild(
            MegaHackMenu::create(), 999);
    }
};
