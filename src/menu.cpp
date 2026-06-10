#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
using namespace geode::prelude;

// Глобальные переменные
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

        // Тёмный фон на весь экран
        auto bg = CCLayerColor::create({0, 0, 10, 210}, win.width, win.height);
        this->addChild(bg);

        // Заголовок
        auto title = CCLabelBMFont::create("MEGA HACK", "goldFont.fnt");
        title->setScale(0.8f);
        title->setPosition({win.width / 2, win.height - 20});
        this->addChild(title);

        // Основное меню кнопок
        auto menu = CCMenu::create();
        menu->setPosition({0, 0});
        this->addChild(menu);

        float colX1 = 10;
        float colX2 = win.width / 2 + 10;
        float rowY = win.height - 60;
        float rowH = 35;

        // ===== Колонка 1: Gameplay =====
        addColTitle("Gameplay", colX1, rowY, menu);
        addRow("Noclip", g_noclip, colX1, rowY - rowH, menu,
            menu_selector(MegaHackMenu::onNoclip));
        addRow("Attempt Farm", g_farm, colX1, rowY - rowH*2, menu,
            menu_selector(MegaHackMenu::onFarm));
        addRow("Safe Mode", g_safeMode, colX1, rowY - rowH*3, menu,
            menu_selector(MegaHackMenu::onSafeMode));

        // Speedhack
        addColTitle("Speedhack", colX1, rowY - rowH*4 - 10, menu);
        
        m_speedLabel = CCLabelBMFont::create(
            fmt::format("Speed: {:.1f}x", g_speed).c_str(), "chatFont.fnt");
        m_speedLabel->setScale(0.6f);
        m_speedLabel->setAnchorPoint({0, 0.5f});
        m_speedLabel->setPosition({colX1 + 5, rowY - rowH*5 - 10});
        this->addChild(m_speedLabel);

        auto minus = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
            this, menu_selector(MegaHackMenu::onSpeedDown));
        minus->setPosition({colX1 + 170, rowY - rowH*5 - 10});
        minus->setScale(0.55f);
        menu->addChild(minus);

        auto plus = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
            this, menu_selector(MegaHackMenu::onSpeedUp));
        plus->setPosition({colX1 + 195, rowY - rowH*5 - 10});
        plus->setScale(0.55f);
        menu->addChild(plus);

        // ===== Колонка 2: Display =====
        addColTitle("Display", colX2, rowY, menu);

        m_fpsLabel = CCLabelBMFont::create(
            fmt::format("FPS: {}", m_fps).c_str(), "chatFont.fnt");
        m_fpsLabel->setScale(0.6f);
        m_fpsLabel->setAnchorPoint({0, 0.5f});
        m_fpsLabel->setPosition({colX2 + 5, rowY - rowH});
        this->addChild(m_fpsLabel);

        auto fpsDown = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"),
            this, menu_selector(MegaHackMenu::onFpsDown));
        fpsDown->setPosition({colX2 + 170, rowY - rowH});
        fpsDown->setScale(0.55f);
        menu->addChild(fpsDown);

        auto fpsUp = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plus2Btn_001.png"),
            this, menu_selector(MegaHackMenu::onFpsUp));
        fpsUp->setPosition({colX2 + 195, rowY - rowH});
        fpsUp->setScale(0.55f);
        menu->addChild(fpsUp);

        // Кнопка закрыть
        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this, menu_selector(MegaHackMenu::onClose));
        closeBtn->setPosition({win.width - 15, win.height - 15});
        menu->addChild(closeBtn);

        // Перехват тапов
        auto listener = CCEventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [](CCTouch*, CCEvent*) { return true; };
        CCDirector::get()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
            listener, this);

        return true;
    }

    void addColTitle(const char* name, float x, float y, CCMenu* menu) {
        auto lbl = CCLabelBMFont::create(name, "bigFont.fnt");
        lbl->setScale(0.45f);
        lbl->setColor({100, 200, 255});
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({x + 5, y});
        this->addChild(lbl);

        auto line = CCLayerColor::create({100, 200, 255, 150}, 220, 1);
        line->setPosition({x, y - 10});
        this->addChild(line);
    }

    void addRow(const char* name, bool& var, float x, float y,
                CCMenu* menu, SEL_MenuHandler sel) {
        auto lbl = CCLabelBMFont::create(name, "chatFont.fnt");
        lbl->setScale(0.55f);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({x + 5, y});
        this->addChild(lbl);

        auto tog = CCMenuItemToggler::createWithStandardSprites(
            this, sel, 0.55f);
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

// Кнопка открытия меню
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
        spr->setScale(0.6f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MegaHackButton::onOpen));
        btn->setPosition({30, win.height - 30});
        menu->addChild(btn);

        return true;
    }

    void onOpen(CCObject*) {
        if (g_menuOpen) return;
        CCDirector::get()->getRunningScene()->addChild(
            MegaHackMenu::create(), 999);
    }
};
