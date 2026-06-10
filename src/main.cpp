#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

bool noclipEnabled = false;
float speedMultiplier = 1.0f;
bool farmEnabled = false;
bool menuOpen = false;

// ===== КОЛОНКА =====
class HackColumn : public CCNode {
public:
    std::string title;
    CCLayerColor* bg;
    CCMenu* menu;
    float yOffset = 0;

    static HackColumn* create(std::string title, float x, float y) {
        auto ret = new HackColumn();
        if (ret && ret->initWithTitle(title, x, y)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool initWithTitle(std::string t, float x, float y) {
        if (!CCNode::init()) return false;
        title = t;
        this->setPosition({x, y});

        bg = CCLayerColor::create({20, 20, 40, 220}, 160, 300);
        bg->setPosition({0, 0});
        this->addChild(bg);

        auto titleLbl = CCLabelBMFont::create(t.c_str(), "bigFont.fnt");
        titleLbl->setScale(0.4f);
        titleLbl->setColor({100, 200, 255});
        titleLbl->setPosition({80, 285});
        this->addChild(titleLbl);

        menu = CCMenu::create();
        menu->setPosition({0, 0});
        this->addChild(menu);

        yOffset = 265;
        return true;
    }

    void addToggle(std::string name, bool& var, CCObject* target, SEL_MenuHandler callback) {
        auto lbl = CCLabelBMFont::create(name.c_str(), "chatFont.fnt");
        lbl->setScale(0.55f);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({5, yOffset});
        this->addChild(lbl);

        auto toggle = CCMenuItemToggler::createWithStandardSprites(
            target, callback, 0.55f
        );
        toggle->setPosition({145, yOffset});
        toggle->toggle(var);
        menu->addChild(toggle);

        yOffset -= 28;
    }
};

// ===== ГЛАВНОЕ МЕНЮ =====
class MegahackMenu : public CCLayer {
public:
    static MegahackMenu* create() {
        auto ret = new MegahackMenu();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init()) return false;
        menuOpen = true;

        auto winSize = CCDirector::get()->getWinSize();

        // Тёмный фон
        auto overlay = CCLayerColor::create({0, 0, 0, 150},
            winSize.width, winSize.height);
        this->addChild(overlay);

        // Заголовок
        auto title = CCLabelBMFont::create("MEGA HACK", "goldFont.fnt");
        title->setScale(0.9f);
        title->setPosition({winSize.width/2, winSize.height - 25});
        this->addChild(title);

        auto ver = CCLabelBMFont::create("v1.0 by myname", "chatFont.fnt");
        ver->setScale(0.5f);
        ver->setColor({150, 150, 150});
        ver->setPosition({winSize.width/2, winSize.height - 45});
        this->addChild(ver);

        float startX = 20;
        float startY = winSize.height - 65;

        // Колонка 1 - Gameplay
        auto col1 = HackColumn::create("Gameplay", startX, startY - 300);
        col1->addToggle("Noclip", noclipEnabled, this,
            menu_selector(MegahackMenu::onNoclip));
        col1->addToggle("Attempt Farm", farmEnabled, this,
            menu_selector(MegahackMenu::onFarm));
        this->addChild(col1);

        // Колонка 2 - Speed
        auto col2 = HackColumn::create("Speedhack", startX + 170, startY - 300);
        
        auto speedLbl = CCLabelBMFont::create("Speed:", "chatFont.fnt");
        speedLbl->setScale(0.55f);
        speedLbl->setAnchorPoint({0, 0.5f});
        speedLbl->setPosition({startX + 175, startY - 80});
        this->addChild(speedLbl);

        m_speedLabel = CCLabelBMFont::create("1.0x", "chatFont.fnt");
        m_speedLabel->setScale(0.55f);
        m_speedLabel->setPosition({startX + 280, startY - 80});
        this->addChild(m_speedLabel);

        auto colMenu = CCMenu::create();
        colMenu->setPosition({0, 0});
        this->addChild(colMenu);

        auto minusBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
            this, menu_selector(MegahackMenu::onSpeedDown)
        );
        minusBtn->setPosition({startX + 305, startY - 80});
        minusBtn->setScale(0.5f);
        colMenu->addChild(minusBtn);

        auto plusBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
            this, menu_selector(MegahackMenu::onSpeedUp)
        );
        plusBtn->setPosition({startX + 325, startY - 80});
        plusBtn->setScale(0.5f);
        colMenu->addChild(plusBtn);

        this->addChild(col2);

        // Кнопка закрыть
        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this, menu_selector(MegahackMenu::onClose)
        );
        auto closeMenu = CCMenu::create();
        closeMenu->setPosition({0, 0});
        closeBtn->setPosition({winSize.width - 20, winSize.height - 20});
        closeMenu->addChild(closeBtn);
        this->addChild(closeMenu);

        this->setTouchEnabled(true);
        return true;
    }

    CCLabelBMFont* m_speedLabel;

    void onNoclip(CCObject*) { noclipEnabled = !noclipEnabled; }
    void onFarm(CCObject*) { farmEnabled = !farmEnabled; }

    void onSpeedUp(CCObject*) {
        speedMultiplier = MIN(speedMultiplier + 0.1f, 5.0f);
        m_speedLabel->setString(fmt::format("{:.1f}x", speedMultiplier).c_str());
    }

    void onSpeedDown(CCObject*) {
        speedMultiplier = MAX(speedMultiplier - 0.1f, 0.1f);
        m_speedLabel->setString(fmt::format("{:.1f}x", speedMultiplier).c_str());
    }

    void onClose(CCObject*) {
        menuOpen = false;
        this->removeFromParent();
    }
};

// ===== КНОПКА =====
class MegahackButton : public CCLayer {
public:
    static MegahackButton* create() {
        auto ret = new MegahackButton();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init()) return false;
        auto winSize = CCDirector::get()->getWinSize();
        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        this->addChild(menu);

        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png"),
            this, menu_selector(MegahackButton::onOpen)
        );
        btn->setPosition({40, winSize.height - 40});
        btn->setScale(0.7f);
        menu->addChild(btn);
        return true;
    }

    void onOpen(CCObject*) {
        if (menuOpen) return;
        CCDirector::get()->getRunningScene()->addChild(
            MegahackMenu::create(), 999
        );
    }
};

// ===== ХУКИ =====
class $modify(MyPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* p, GameObject* o) {
        if (!noclipEnabled)
            PlayLayer::destroyPlayer(p, o);
    }

    void startGame() {
        PlayLayer::startGame();
        this->addChild(MegahackButton::create(), 999);
        if (farmEnabled)
            this->scheduleOnce(schedule_selector(MyPlayLayer::killMe), 0.1f);
    }

    void killMe(float) {
        PlayLayer::destroyPlayer(this->m_player1, nullptr);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt * speedMultiplier);
    }
};
