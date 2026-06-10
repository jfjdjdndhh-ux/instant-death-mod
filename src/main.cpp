#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
using namespace geode::prelude;

bool noclipEnabled = false;
float speedMultiplier = 1.0f;
bool farmEnabled = false;

class MegahackMenu : public CCLayer {
public:
    CCLabelBMFont* speedLabel;

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

        auto winSize = CCDirector::get()->getWinSize();

        auto bg = CCLayerColor::create({0, 0, 0, 200}, 300, 400);
        bg->setPosition({winSize.width/2 - 150, winSize.height/2 - 200});
        this->addChild(bg);

        auto title = CCLabelBMFont::create("MEGAHACK", "goldFont.fnt");
        title->setScale(0.8f);
        title->setPosition({winSize.width/2, winSize.height/2 + 170});
        this->addChild(title);

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        this->addChild(menu);

        // Noclip
        auto noclipLabel = CCLabelBMFont::create("Noclip", "bigFont.fnt");
        noclipLabel->setScale(0.5f);
        noclipLabel->setPosition({winSize.width/2 - 50, winSize.height/2 + 100});
        this->addChild(noclipLabel);

        auto noclipBtn = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MegahackMenu::onNoclip), 0.7f
        );
        noclipBtn->setPosition({winSize.width/2 + 100, winSize.height/2 + 100});
        noclipBtn->toggle(noclipEnabled);
        menu->addChild(noclipBtn);

        // Farm
        auto farmLabel = CCLabelBMFont::create("Attempt Farm", "bigFont.fnt");
        farmLabel->setScale(0.5f);
        farmLabel->setPosition({winSize.width/2 - 50, winSize.height/2 + 50});
        this->addChild(farmLabel);

        auto farmBtn = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(MegahackMenu::onFarm), 0.7f
        );
        farmBtn->setPosition({winSize.width/2 + 100, winSize.height/2 + 50});
        farmBtn->toggle(farmEnabled);
        menu->addChild(farmBtn);

        // Speed
        auto speedLabel2 = CCLabelBMFont::create("Speed", "bigFont.fnt");
        speedLabel2->setScale(0.5f);
        speedLabel2->setPosition({winSize.width/2 - 50, winSize.height/2});
        this->addChild(speedLabel2);

        speedLabel = CCLabelBMFont::create("1.0x", "bigFont.fnt");
        speedLabel->setScale(0.5f);
        speedLabel->setPosition({winSize.width/2 + 60, winSize.height/2});
        this->addChild(speedLabel);

        auto minusBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
            this, menu_selector(MegahackMenu::onSpeedDown)
        );
        minusBtn->setPosition({winSize.width/2 + 100, winSize.height/2});
        minusBtn->setScale(0.6f);
        menu->addChild(minusBtn);

        auto plusBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
            this, menu_selector(MegahackMenu::onSpeedUp)
        );
        plusBtn->setPosition({winSize.width/2 + 130, winSize.height/2});
        plusBtn->setScale(0.6f);
        menu->addChild(plusBtn);

        // Закрыть
        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this, menu_selector(MegahackMenu::onClose)
        );
        closeBtn->setPosition({winSize.width/2 + 130, winSize.height/2 + 190});
        menu->addChild(closeBtn);

        return true;
    }

    void onNoclip(CCObject*) { noclipEnabled = !noclipEnabled; }
    void onFarm(CCObject*) { farmEnabled = !farmEnabled; }

    void onSpeedUp(CCObject*) {
        speedMultiplier = MIN(speedMultiplier + 0.1f, 5.0f);
        speedLabel->setString(fmt::format("{:.1f}x", speedMultiplier).c_str());
    }

    void onSpeedDown(CCObject*) {
        speedMultiplier = MAX(speedMultiplier - 0.1f, 0.1f);
        speedLabel->setString(fmt::format("{:.1f}x", speedMultiplier).c_str());
    }

    void onClose(CCObject*) { this->removeFromParent(); }
};

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
            this, menu_selector(MegahackButton::onOpenMenu)
        );
        btn->setPosition({50, winSize.height - 50});
        btn->setScale(0.8f);
        menu->addChild(btn);

        return true;
    }

    void onOpenMenu(CCObject*) {
        auto scene = CCDirector::get()->getRunningScene();
        scene->addChild(MegahackMenu::create(), 999);
    }
};

class $modify(MyPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* p, GameObject* o) {
        if (!noclipEnabled) {
            PlayLayer::destroyPlayer(p, o);
        }
    }

    void startGame() {
        PlayLayer::startGame();
        this->addChild(MegahackButton::create(), 999);
        if (farmEnabled) {
            this->scheduleOnce(schedule_selector(MyPlayLayer::killMe), 0.1f);
        }
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
