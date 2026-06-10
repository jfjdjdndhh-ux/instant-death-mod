#include "menu.hpp"
#include <Geode/Geode.hpp>
using namespace geode::prelude;

bool g_noclip = false;
bool g_farm = false;
bool g_safeMode = false;
bool g_instantComplete = false;
bool g_allModesPlatformer = false;
float g_speed = 1.0f;
int g_deaths = 0;
float g_accuracy = 100.0f;
bool g_menuOpen = false;

// ===== СТРОКА ЧЕКБОКСА =====
class HackRow : public CCNode {
public:
    bool* m_var;
    CCLayerColor* m_bg;
    bool m_toggled = false;

    static HackRow* create(const char* name, bool& var,
        CCObject* target, SEL_MenuHandler sel, float width) {
        auto ret = new HackRow();
        if (ret && ret->initRow(name, var, target, sel, width)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool initRow(const char* name, bool& var,
        CCObject* target, SEL_MenuHandler sel, float width) {
        if (!CCNode::init()) return false;
        m_var = &var;

        // Фон строки
        m_bg = CCLayerColor::create({255,255,255, (GLubyte)(var ? 30 : 0)}, width, 22);
        m_bg->setPosition({0, 0});
        this->addChild(m_bg);

        // Текст
        auto lbl = CCLabelTTF::create(name, "Arial", 11);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({6, 11});
        lbl->setColor({var ? ccWHITE : ccc3(180,180,180)});
        this->addChild(lbl);

        // Чекбокс
        auto check = CCLayerColor::create(
            var ? ccc4(80,200,80,255) : ccc4(60,60,60,255), 14, 14);
        check->setPosition({width - 20, 4});
        this->addChild(check);

        auto menu = CCMenu::create();
        menu->setPosition({0,0});
        this->addChild(menu);

        // Невидимая кнопка на всю строку
        auto btn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({0,0,0,0}, width, 22),
            target, sel);
        btn->setAnchorPoint({0,0});
        btn->setPosition({0, 0});
        menu->addChild(btn);

        return true;
    }
};

// ===== СТОЛБИК =====
class HackColumn : public CCNode {
public:
    float m_width;
    float m_yOff;
    CCMenu* m_menu;

    static HackColumn* create(const char* title, float width, float height) {
        auto ret = new HackColumn();
        if (ret && ret->initCol(title, width, height)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool initCol(const char* title, float width, float height) {
        if (!CCNode::init()) return false;
        m_width = width;
        m_yOff = height;

        // Фон столбика
        auto bg = CCLayerColor::create({15, 15, 25, 240}, width, height);
        this->addChild(bg);

        // Заголовок
        auto titleBg = CCLayerColor::create({30, 30, 50, 255}, width, 20);
        titleBg->setPosition({0, height - 20});
        this->addChild(titleBg);

        auto titleLbl = CCLabelTTF::create(title, "Arial Bold", 11);
        titleLbl->setAnchorPoint({0, 0.5f});
        titleLbl->setPosition({5, height - 10});
        titleLbl->setColor({100, 200, 255});
        this->addChild(titleLbl);

        // Линия под заголовком
        auto line = CCLayerColor::create({80, 160, 255, 180}, width, 1);
        line->setPosition({0, height - 21});
        this->addChild(line);

        m_menu = CCMenu::create();
        m_menu->setPosition({0, 0});
        this->addChild(m_menu);

        m_yOff = height - 22;
        return true;
    }

    void addToggle(const char* name, bool& var,
        CCObject* target, SEL_MenuHandler sel) {
        m_yOff -= 22;

        // Фон строки при hover
        auto rowBg = CCLayerColor::create(
            var ? ccc4(255,255,255,20) : ccc4(0,0,0,0),
            m_width, 22);
        rowBg->setPosition({0, m_yOff});
        this->addChild(rowBg);

        // Текст
        auto lbl = CCLabelTTF::create(name, "Arial", 10);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({6, m_yOff + 11});
        lbl->setColor(var ? ccWHITE : ccc3(160, 160, 160));
        this->addChild(lbl);

        // Чекбокс квадратик
        auto checkBg = CCLayerColor::create(
            var ? ccc4(80,200,80,255) : ccc4(50,50,60,255),
            13, 13);
        checkBg->setPosition({m_width - 18, m_yOff + 5});
        this->addChild(checkBg);

        // Галочка если включено
        if (var) {
            auto check = CCLabelTTF::create("v", "Arial Bold", 10);
            check->setPosition({m_width - 12, m_yOff + 11});
            check->setColor({255, 255, 255});
            this->addChild(check);
        }

        // Кнопка
        auto btn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, m_width, 22),
            target, sel);
        btn->setAnchorPoint({0, 0});
        btn->setPosition({0, m_yOff});
        m_menu->addChild(btn);

        // Разделитель
        auto sep = CCLayerColor::create({40, 40, 55, 255}, m_width, 1);
        sep->setPosition({0, m_yOff});
        this->addChild(sep);
    }

    void addSlider(const char* name, CCLabelTTF*& outLabel,
        CCObject* target, SEL_MenuHandler selUp, SEL_MenuHandler selDown) {
        m_yOff -= 26;

        auto lbl = CCLabelTTF::create(name, "Arial", 10);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({6, m_yOff + 13});
        lbl->setColor({200, 200, 200});
        this->addChild(lbl);

        outLabel = CCLabelTTF::create("", "Arial Bold", 10);
        outLabel->setAnchorPoint({0.5f, 0.5f});
        outLabel->setPosition({m_width - 45, m_yOff + 13});
        outLabel->setColor({255, 220, 100});
        this->addChild(outLabel);

        // Кнопки - и +
        auto minusBg = CCLayerColor::create({80,30,30,255}, 18, 18);
        minusBg->setPosition({m_width - 36, m_yOff + 4});
        this->addChild(minusBg);

        auto minusLbl = CCLabelTTF::create("-", "Arial Bold", 13);
        minusLbl->setPosition({m_width - 27, m_yOff + 13});
        minusLbl->setColor({255,100,100});
        this->addChild(minusLbl);

        auto plusBg = CCLayerColor::create({30,80,30,255}, 18, 18);
        plusBg->setPosition({m_width - 18, m_yOff + 4});
        this->addChild(plusBg);

        auto plusLbl = CCLabelTTF::create("+", "Arial Bold", 13);
        plusLbl->setPosition({m_width - 9, m_yOff + 13});
        plusLbl->setColor({100,255,100});
        this->addChild(plusLbl);

        auto minusBtn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, 18, 22),
            target, selDown);
        minusBtn->setAnchorPoint({0,0});
        minusBtn->setPosition({m_width - 36, m_yOff + 4});
        m_menu->addChild(minusBtn);

        auto plusBtn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, 18, 22),
            target, selUp);
        plusBtn->setAnchorPoint({0,0});
        plusBtn->setPosition({m_width - 18, m_yOff + 4});
        m_menu->addChild(plusBtn);

        auto sep = CCLayerColor::create({40,40,55,255}, m_width, 1);
        sep->setPosition({0, m_yOff});
        this->addChild(sep);
    }

    float getUsedHeight() { return m_yOff; }
};

// ===== ГЛАВНОЕ МЕНЮ =====
class MegaHackMenu : public CCLayer {
public:
    CCLabelTTF* m_speedLabel = nullptr;
    CCLabelTTF* m_fpsLabel = nullptr;
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

    bool init() override {
        if (!CCLayer::init()) return false;
        g_menuOpen = true;

        auto win = CCDirector::get()->getWinSize();

        // Полупрозрачный оверлей
        auto overlay = CCLayerColor::create({0,0,0,120}, win.width, win.height);
        this->addChild(overlay);

        // Контейнер меню
        float menuW = win.width - 40;
        float menuH = win.height - 60;
        float menuX = 20;
        float menuY = 30;

        auto menuBg = CCLayerColor::create({10,10,20,245}, menuW, menuH);
        menuBg->setPosition({menuX, menuY});
        this->addChild(menuBg);

        // Шапка
        auto headerBg = CCLayerColor::create({20,20,40,255}, menuW, 28);
        headerBg->setPosition({menuX, menuY + menuH - 28});
        this->addChild(headerBg);

        auto titleLbl = CCLabelTTF::create("MEGA HACK  v1.0", "Arial Bold", 13);
        titleLbl->setAnchorPoint({0, 0.5f});
        titleLbl->setPosition({menuX + 8, menuY + menuH - 14});
        titleLbl->setColor({255, 220, 50});
        this->addChild(titleLbl);

        auto byLbl = CCLabelTTF::create("by myname", "Arial", 10);
        byLbl->setAnchorPoint({0, 0.5f});
        byLbl->setPosition({menuX + 160, menuY + menuH - 14});
        byLbl->setColor({150, 150, 150});
        this->addChild(byLbl);

        // Кнопка закрыть X
        auto closeBg = CCLayerColor::create({150,30,30,255}, 28, 28);
        closeBg->setPosition({menuX + menuW - 28, menuY + menuH - 28});
        this->addChild(closeBg);

        auto closeMenu = CCMenu::create();
        closeMenu->setPosition({0,0});
        this->addChild(closeMenu, 10);

        auto closeLbl = CCLabelTTF::create("X", "Arial Bold", 13);
        closeLbl->setPosition({menuX + menuW - 14, menuY + menuH - 14});
        closeLbl->setColor({255,255,255});
        this->addChild(closeLbl);

        auto closeBtn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, 28, 28),
            this, menu_selector(MegaHackMenu::onClose));
        closeBtn->setAnchorPoint({0,0});
        closeBtn->setPosition({menuX + menuW - 28, menuY + menuH - 28});
        closeMenu->addChild(closeBtn);

        // Высота контента
        float colH = menuH - 30;
        float colW = (menuW - 5) / 3.f;

        // ===== СТОЛБИК 1: Level =====
        auto col1 = HackColumn::create("Level", colW, colH);
        col1->setPosition({menuX, menuY});
        col1->addToggle("Noclip", g_noclip, this,
            menu_selector(MegaHackMenu::onNoclip));
        col1->addToggle("Attempt Farm", g_farm, this,
            menu_selector(MegaHackMenu::onFarm));
        col1->addToggle("Instant Complete", g_instantComplete, this,
            menu_selector(MegaHackMenu::onInstantComplete));
        col1->addToggle("All Modes Platformer", g_allModesPlatformer, this,
            menu_selector(MegaHackMenu::onPlatformer));
        this->addChild(col1);

        // ===== СТОЛБИК 2: Speedhack =====
        auto col2 = HackColumn::create("Speedhack", colW, colH);
        col2->setPosition({menuX + colW + 1, menuY});
        col2->addSlider("Speed", m_speedLabel,
            this,
            menu_selector(MegaHackMenu::onSpeedUp),
            menu_selector(MegaHackMenu::onSpeedDown));
        this->addChild(col2);
        updateSpeedLabel();

        // ===== СТОЛБИК 3: Display =====
        auto col3 = HackColumn::create("Display", colW, colH);
        col3->setPosition({menuX + colW*2 + 2, menuY});
        col3->addSlider("FPS Unlock", m_fpsLabel,
            this,
            menu_selector(MegaHackMenu::onFpsUp),
            menu_selector(MegaHackMenu::onFpsDown));
        col3->addToggle("Safe Mode", g_safeMode, this,
            menu_selector(MegaHackMenu::onSafeMode));
        this->addChild(col3);
        updateFpsLabel();

        // Перехват тапов
        this->setTouchEnabled(true);
        this->setTouchMode(kCCTouchesOneByOne);

        return true;
    }

    bool ccTouchBegan(CCTouch*, CCEvent*) override { return true; }

    void updateSpeedLabel() {
        if (m_speedLabel)
            m_speedLabel->setString(fmt::format("{:.1f}x", g_speed).c_str());
    }

    void updateFpsLabel() {
        if (m_fpsLabel)
            m_fpsLabel->setString(fmt::format("{}fps", m_fps).c_str());
    }

    void onNoclip(CCObject*) { g_noclip = !g_noclip; }
    void onFarm(CCObject*) { g_farm = !g_farm; }
    void onSafeMode(CCObject*) { g_safeMode = !g_safeMode; }
    void onInstantComplete(CCObject*) { g_instantComplete = !g_instantComplete; }
    void onPlatformer(CCObject*) { g_allModesPlatformer = !g_allModesPlatformer; }

    void onSpeedUp(CCObject*) {
        g_speed = MIN(g_speed + 0.1f, 10.0f);
        updateSpeedLabel();
    }
    void onSpeedDown(CCObject*) {
        g_speed = MAX(g_speed - 0.1f, 0.1f);
        updateSpeedLabel();
    }
    void onFpsUp(CCObject*) {
        m_fps = MIN(m_fps + 10, 360);
        CCDirector::get()->setAnimationInterval(1.0 / m_fps);
        updateFpsLabel();
    }
    void onFpsDown(CCObject*) {
        m_fps = MAX(m_fps - 10, 30);
        CCDirector::get()->setAnimationInterval(1.0 / m_fps);
        updateFpsLabel();
    }
    void onClose(CCObject*) {
        g_menuOpen = false;
        this->removeFromParent();
    }
};

// ===== КНОПКА =====
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

        // Простая кнопка без GD текстур
        auto bg = CCLayerColor::create({20, 20, 40, 200}, 40, 18);
        bg->setPosition({5, win.height - 23});
        this->addChild(bg);

        auto lbl = CCLabelTTF::create("MH", "Arial Bold", 10);
        lbl->setPosition({25, win.height - 14});
        lbl->setColor({100, 200, 255});
        this->addChild(lbl);

        auto menu = CCMenu::create();
        menu->setPosition({0,0});
        this->addChild(menu);

        auto btn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, 40, 18),
            this, menu_selector(MegaHackButton::onOpen));
        btn->setAnchorPoint({0,0});
        btn->setPosition({5, win.height - 23});
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
