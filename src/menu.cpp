#include "menu.hpp"
#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool g_noclip = false;
bool g_farm = false;
bool g_safeMode = false;
float g_speed = 1.0f;
int g_deaths = 0;
float g_accuracy = 100.0f;
bool g_menuOpen = false;

class MegaHackMenu : public CCLayer {
public:
    CCLabelBMFont* m_speedLabel;
    CCLabelBMFont* m_fpsLabel;
    int m_fps = 60;

    static MegaHackMenu* create() {
        auto ret = new MegaHackMenu();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init()) return false;
        g_menuOpen = true;

        auto win = CCDirector::get()->getWinSize();

        auto bg = CCLayerColor::create({0, 0, 10, 220}, win.width, win.height);
        this->addChild(bg);

        auto title = CCLabelBMFont::create("MEGA HACK", "goldFont.fnt");
        title->setScale(0.8f);
        title->setPosition({win.width / 2, win.height - 20});
        this->addChild(title);

        auto ver = CCLabelBMFont::create("v1.0 by myname", "chatFont.fnt");
        ver->setScale(0.45f);
        ver->setColor({150, 150, 150});
        ver->setPosition({win.width / 2, win.height - 38});
        this->addChild(ver);

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        this->addChild(menu);

        float col1 = 10;
        float col2 = win.width / 2 + 10;
        float startY = win.height - 60;
        float rowH = 32;

        // Колонка 1
        addColTitle("Gameplay", col1, startY, menu);
        addRow("Noclip", g_noclip, col1, startY - rowH, menu,
            menu_selector(MegaHackMenu::onNoclip));
        addRow("Attempt Farm", g_farm, col1, startY - rowH*2, menu,
            menu_selector(MegaHackMenu::onFarm));
        addRow("Safe Mode", g_safeMode, col1, startY - rowH*3, menu,
            menu_selector(MegaHackMenu::onSafeMode));

        addColTitle("Speedhack", col1, startY - rowH*4 - 15, menu);
        m_speedLabel = CCLabelBMFont::create(
            fmt::format("Speed: {:.1f}x", g_speed).c_str(), "chatFont.fnt");
        m_speedLabel->setScale(0.55f);
        m_speedLabel->setAnchorPoint({0, 0.5f});
        m_speedLabel->setPosition({col1 + 5, startY - rowH*5 - 15});
        this->addChild(m_speedLabel);

        auto minus = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
            this, menu_selector(MegaHackMenu::onSpeedDown));
        minus->setPosition({col1 + 180, startY - rowH*5 - 15});
        minus->setScale(0.5f);
        menu->addChild(minus);

        auto plus = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
            this, menu_selector(MegaHackMenu::onSpeedUp));
        plus->setPosition({col1 + 205, startY - rowH*5 - 15});
        plus->setScale(0.5f);
        menu->addChild(plus);

        // Колонка 2
        addColTitle("Display", col2, startY, menu);
        m_fpsLabel = CCLabelBMFont::create(
            fmt::format("FPS: {}", m_fps).c_str(), "chatFont.fnt");
        m_fpsLabel->setScale(0.55f);
        m_fpsLabel->setAnchorPoint({0, 0.5f});
        m_fpsLabel->setPosition({col2 + 5, startY - rowH});
        this->addChild(m_fpsLabel);

        auto fpsDown = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
            this, menu_selector(MegaHackMenu::onFpsDown));
        fpsDown->setPosition({col2 + 180, startY - rowH});
        fpsDown->setScale(0.5f);
        menu->addChild(fpsDown);

        auto fpsUp = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
            this, menu_selector(MegaHackMenu::onFpsUp));
        fpsUp->setPosition({col2 + 205, startY - rowH});
        fpsUp->setScale(0.5f);
        menu->addChild(fpsUp);

        // Закрыть
        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this, menu_selector(MegaHackMenu::onClose));
        closeBtn->setPosition({win.width - 15, win.height - 15});
        menu->addChild(closeBtn);

        return true;
    }

    void addColTitle(const char* name, float x, float y, CCMenu*) {
        auto lbl = CCLabelBMFont::create(name, "bigFont.fnt");
        lbl->setScale(0.4f);
        lbl->setColor({100, 200, 255});
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({x + 5, y});
        this->addChild(lbl);

        auto line = CCLayerColor::create({100, 200, 255, 100}, 220, 1);
        line->setPosition({x, y - 10});
        this->addChild(line);
    }

    void addRow(const char* name, bool& var, float x, float y,
                CCMenu* menu, SEL_MenuHandler sel) {
        auto lbl = CCLabelBMFont::create(name, "chatFont.fnt");
        lbl->setScale(0.52f);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({x + 5, y});
        this->addChild(lbl);

        auto tog = CCMenuItemToggler::createWithStandardSprites(
            this, sel, 0.5f);
        tog->setPosition({x + 205, y});
        tog->toggle(var);
        menu->addChild(tog);
    }

    void onNoclip(CCObject*) { g_noclip = !g_noclip; }
    void onFarm(CCObject*) { g_farm = !g_farm; }
    void onSafeMode(CCObject*) { g_safeMode = !g_safeMode; }

    void onSpeedUp(CCObject*) {
        g_speed = MIN(g_speed + 0.1f, 10.0f);
        m_speedLabel->setString(fmt::format("Speed: {:.1f}x", g_speed).c_str());
    }
    void onSpeedDown(CCObject*) {
        g_speed = MAX(g_speed - 0.1f, 0.1f);
        m_speedLabel->setString(fmt::format("Speed: {:.1f}x", g_speed).c_str());
    }
    void onFpsUp(CCObject*) {
        m_fps = MIN(m_fps + 10, 360);
        CCDirector::get()->setAnimationInterval(1.0 / m_fps);
        m_fpsLabel->setString(fmt::format("FPS: {}", m_fps).c_str());
    }
    void onFpsDown(CCObject*) {
        m_fps = MAX(m_fps - 10, 30);
        CCDirector::get()->setAnimationInterval(1.0 / m_fps);
        m_fpsLabel->setString(fmt::format("FPS: {}", m_fps).c_str());
    }
    void onClose(CCObject*) {
        g_menuOpen = false;
        this->removeFromParent();
    }
};

class MegaHackButton : public CCNode {
public:
    static MegaHackButton* create() {
        auto ret = new MegaHackButton();
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
        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        this->addChild(menu);

        auto spr = CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
        spr->setScale(0.5f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MegaHackButton::onOpen));
        btn->setPosition({25, win.height - 25});
        menu->addChild(btn);
        return true;
    }

    void onOpen(CCObject*) { openMegaHackMenu(); }
};

void openMegaHackMenu() {
    if (g_menuOpen) return;
    CCDirector::get()->getRunningScene()->addChild(
        MegaHackMenu::create(), 999);
}

void addMegaHackButton(CCNode* parent) {
    parent->addChild(MegaHackButton::create(), 999);
}
