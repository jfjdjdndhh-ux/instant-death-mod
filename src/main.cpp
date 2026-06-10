#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameManager.hpp>
using namespace geode::prelude;

// ===== ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ =====
bool g_noclip = false;
bool g_farm = false;
bool g_safeMode = false;
bool g_instantComplete = false;
bool g_allModesPlatformer = false;
bool g_noRespawnFlash = false;
bool g_noDeathEffect = false;
bool g_showHitboxes = false;
bool g_practiceBugFix = false;
bool g_confirmRestart = false;
bool g_noMirror = false;
bool g_noParticles = false;
bool g_noGlow = false;
bool g_noShake = false;
bool g_solidWaveTrail = false;
bool g_autoDeafen = false;
float g_speed = 1.0f;
int g_fps = 60;
int g_deaths = 0;
float g_accuracy = 100.0f;
bool g_menuOpen = false;

// ===== СТОЛБИК =====
class MHColumn : public CCNode {
public:
    float m_w;
    float m_h;
    float m_y;
    CCMenu* m_menu;

    static MHColumn* create(const char* title, float w, float h) {
        auto ret = new MHColumn();
        if (ret && ret->initCol(title, w, h)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool initCol(const char* title, float w, float h) {
        if (!CCNode::init()) return false;
        m_w = w; m_h = h;

        // Фон
        auto bg = CCLayerColor::create({18, 18, 28, 255}, w, h);
        this->addChild(bg);

        // Заголовок
        auto hdrBg = CCLayerColor::create({35, 35, 55, 255}, w, 22);
        hdrBg->setPosition({0, h - 22});
        this->addChild(hdrBg);

        auto hdrLbl = CCLabelTTF::create(title, "Arial Bold", 11);
        hdrLbl->setAnchorPoint({0, 0.5f});
        hdrLbl->setPosition({6, h - 11});
        hdrLbl->setColor({120, 180, 255});
        this->addChild(hdrLbl);

        auto line = CCLayerColor::create({80, 140, 255, 200}, w, 1);
        line->setPosition({0, h - 23});
        this->addChild(line);

        m_menu = CCMenu::create();
        m_menu->setPosition({0, 0});
        this->addChild(m_menu);

        m_y = h - 23;
        return true;
    }

    void addToggle(const char* name, bool val,
        CCObject* target, SEL_MenuHandler sel) {
        m_y -= 22;

        // Подсветка включённого
        if (val) {
            auto hl = CCLayerColor::create({50, 120, 50, 60}, m_w, 22);
            hl->setPosition({0, m_y});
            this->addChild(hl);
        }

        auto lbl = CCLabelTTF::create(name, "Arial", 10);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({6, m_y + 11});
        lbl->setColor(val ? ccc3(200, 255, 200) : ccc3(180, 180, 180));
        this->addChild(lbl);

        // Чекбокс
        auto cbBg = CCLayerColor::create(
            val ? ccc4(70,190,70,255) : ccc4(45,45,60,255), 13, 13);
        cbBg->setPosition({m_w - 18, m_y + 5});
        this->addChild(cbBg);

        auto btn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, m_w, 22),
            target, sel);
        btn->setAnchorPoint({0, 0});
        btn->setPosition({0, m_y});
        m_menu->addChild(btn);

        auto sep = CCLayerColor::create({35, 35, 50, 255}, m_w, 1);
        sep->setPosition({0, m_y});
        this->addChild(sep);
    }

    void addSlider(const char* name, std::string valStr,
        CCObject* target, SEL_MenuHandler selUp, SEL_MenuHandler selDown,
        CCLabelTTF** outLbl) {
        m_y -= 24;

        auto lbl = CCLabelTTF::create(name, "Arial", 10);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({6, m_y + 12});
        lbl->setColor({180, 180, 180});
        this->addChild(lbl);

        auto valLbl = CCLabelTTF::create(valStr.c_str(), "Arial Bold", 10);
        valLbl->setAnchorPoint({1, 0.5f});
        valLbl->setPosition({m_w - 42, m_y + 12});
        valLbl->setColor({255, 220, 80});
        this->addChild(valLbl);
        if (outLbl) *outLbl = valLbl;

        auto minBg = CCLayerColor::create({100, 40, 40, 255}, 18, 18);
        minBg->setPosition({m_w - 38, m_y + 3});
        this->addChild(minBg);
        auto minLbl = CCLabelTTF::create("-", "Arial Bold", 12);
        minLbl->setPosition({m_w - 29, m_y + 12});
        minLbl->setColor({255, 120, 120});
        this->addChild(minLbl);

        auto plsBg = CCLayerColor::create({40, 100, 40, 255}, 18, 18);
        plsBg->setPosition({m_w - 20, m_y + 3});
        this->addChild(plsBg);
        auto plsLbl = CCLabelTTF::create("+", "Arial Bold", 12);
        plsLbl->setPosition({m_w - 11, m_y + 12});
        plsLbl->setColor({120, 255, 120});
        this->addChild(plsLbl);

        auto minBtn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, 18, 20),
            target, selDown);
        minBtn->setAnchorPoint({0,0});
        minBtn->setPosition({m_w - 38, m_y + 2});
        m_menu->addChild(minBtn);

        auto plsBtn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, 18, 20),
            target, selUp);
        plsBtn->setAnchorPoint({0,0});
        plsBtn->setPosition({m_w - 20, m_y + 2});
        m_menu->addChild(plsBtn);

        auto sep = CCLayerColor::create({35, 35, 50, 255}, m_w, 1);
        sep->setPosition({0, m_y});
        this->addChild(sep);
    }
};

// ===== МЕНЮ =====
class MegaHackMenu : public CCLayer {
public:
    CCLabelTTF* m_speedLbl = nullptr;
    CCLabelTTF* m_fpsLbl = nullptr;

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

        // Оверлей
        auto ov = CCLayerColor::create({0,0,0,130}, win.width, win.height);
        this->addChild(ov);

        float mw = win.width - 20;
        float mh = win.height - 50;
        float mx = 10, my = 25;

        // Контейнер
        auto cont = CCLayerColor::create({12,12,22,250}, mw, mh);
        cont->setPosition({mx, my});
        this->addChild(cont);

        // Шапка
        auto hdr = CCLayerColor::create({25,25,45,255}, mw, 26);
        hdr->setPosition({mx, my + mh - 26});
        this->addChild(hdr);

        auto title = CCLabelTTF::create("MEGA HACK  v1.0  by myname", "Arial Bold", 12);
        title->setAnchorPoint({0, 0.5f});
        title->setPosition({mx + 8, my + mh - 13});
        title->setColor({255, 215, 0});
        this->addChild(title);

        // Кнопка X
        auto xBg = CCLayerColor::create({160, 30, 30, 255}, 26, 26);
        xBg->setPosition({mx + mw - 26, my + mh - 26});
        this->addChild(xBg);

        auto xMenu = CCMenu::create();
        xMenu->setPosition({0,0});
        this->addChild(xMenu, 5);

        auto xLbl = CCLabelTTF::create("X", "Arial Bold", 13);
        xLbl->setPosition({mx + mw - 13, my + mh - 13});
        xLbl->setColor({255,255,255});
        this->addChild(xLbl);

        auto xBtn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, 26, 26),
            this, menu_selector(MegaHackMenu::onClose));
        xBtn->setAnchorPoint({0,0});
        xBtn->setPosition({mx + mw - 26, my + mh - 26});
        xMenu->addChild(xBtn);

        // Столбики
        float colH = mh - 27;
        float colW = (mw - 3) / 3.f;

        // ===== СТОЛБИК 1: Level =====
        auto col1 = MHColumn::create("Level", colW, colH);
        col1->setPosition({mx, my});
        col1->addToggle("Noclip", g_noclip, this,
            menu_selector(MegaHackMenu::onNoclip));
        col1->addToggle("Attempt Farm", g_farm, this,
            menu_selector(MegaHackMenu::onFarm));
        col1->addToggle("Instant Complete", g_instantComplete, this,
            menu_selector(MegaHackMenu::onInstantComplete));
        col1->addToggle("All Modes Platformer", g_allModesPlatformer, this,
            menu_selector(MegaHackMenu::onPlatformer));
        col1->addToggle("No Respawn Flash", g_noRespawnFlash, this,
            menu_selector(MegaHackMenu::onNoRespawnFlash));
        col1->addToggle("No Death Effect", g_noDeathEffect, this,
            menu_selector(MegaHackMenu::onNoDeathEffect));
        col1->addToggle("Practice Bug Fix", g_practiceBugFix, this,
            menu_selector(MegaHackMenu::onPracticeBugFix));
        col1->addToggle("No Particles", g_noParticles, this,
            menu_selector(MegaHackMenu::onNoParticles));
        col1->addToggle("No Mirror", g_noMirror, this,
            menu_selector(MegaHackMenu::onNoMirror));
        col1->addToggle("Solid Wave Trail", g_solidWaveTrail, this,
            menu_selector(MegaHackMenu::onSolidWaveTrail));
        this->addChild(col1);

        // ===== СТОЛБИК 2: Speedhack =====
        auto col2 = MHColumn::create("Speedhack", colW, colH);
        col2->setPosition({mx + colW + 1, my});
        col2->addSlider("Speed", fmt::format("{:.1f}x", g_speed),
            this,
            menu_selector(MegaHackMenu::onSpeedUp),
            menu_selector(MegaHackMenu::onSpeedDown),
            &m_speedLbl);
        this->addChild(col2);

        // ===== СТОЛБИК 3: Display =====
        auto col3 = MHColumn::create("Display", colW, colH);
        col3->setPosition({mx + colW*2 + 2, my});
        col3->addSlider("FPS Unlock", fmt::format("{}fps", g_fps),
            this,
            menu_selector(MegaHackMenu::onFpsUp),
            menu_selector(MegaHackMenu::onFpsDown),
            &m_fpsLbl);
        col3->addToggle("Safe Mode", g_safeMode, this,
            menu_selector(MegaHackMenu::onSafeMode));
        col3->addToggle("No Glow", g_noGlow, this,
            menu_selector(MegaHackMenu::onNoGlow));
        col3->addToggle("No Shake", g_noShake, this,
            menu_selector(MegaHackMenu::onNoShake));
        col3->addToggle("Auto Deafen", g_autoDeafen, this,
            menu_selector(MegaHackMenu::onAutoDeafen));
        this->addChild(col3);

        this->setTouchEnabled(true);
        this->setTouchMode(kCCTouchesOneByOne);

        return true;
    }

    bool ccTouchBegan(CCTouch*, CCEvent*) override { return true; }

    void refresh() {
        g_menuOpen = false;
        this->removeFromParent();
        openMenu();
    }

    static void openMenu() {
        if (g_menuOpen) return;
        CCDirector::get()->getRunningScene()->addChild(
            MegaHackMenu::create(), 999);
    }

    void onNoclip(CCObject*) { g_noclip = !g_noclip; refresh(); }
    void onFarm(CCObject*) { g_farm = !g_farm; refresh(); }
    void onSafeMode(CCObject*) { g_safeMode = !g_safeMode; refresh(); }
    void onInstantComplete(CCObject*) { g_instantComplete = !g_instantComplete; refresh(); }
    void onPlatformer(CCObject*) { g_allModesPlatformer = !g_allModesPlatformer; refresh(); }
    void onNoRespawnFlash(CCObject*) { g_noRespawnFlash = !g_noRespawnFlash; refresh(); }
    void onNoDeathEffect(CCObject*) { g_noDeathEffect = !g_noDeathEffect; refresh(); }
    void onPracticeBugFix(CCObject*) { g_practiceBugFix = !g_practiceBugFix; refresh(); }
    void onNoParticles(CCObject*) { g_noParticles = !g_noParticles; refresh(); }
    void onNoMirror(CCObject*) { g_noMirror = !g_noMirror; refresh(); }
    void onSolidWaveTrail(CCObject*) { g_solidWaveTrail = !g_solidWaveTrail; refresh(); }
    void onNoGlow(CCObject*) { g_noGlow = !g_noGlow; refresh(); }
    void onNoShake(CCObject*) { g_noShake = !g_noShake; refresh(); }
    void onAutoDeafen(CCObject*) { g_autoDeafen = !g_autoDeafen; refresh(); }

    void onSpeedUp(CCObject*) {
        g_speed = MIN(g_speed + 0.1f, 10.0f);
        if (m_speedLbl)
            m_speedLbl->setString(fmt::format("{:.1f}x", g_speed).c_str());
    }
    void onSpeedDown(CCObject*) {
        g_speed = MAX(g_speed - 0.1f, 0.1f);
        if (m_speedLbl)
            m_speedLbl->setString(fmt::format("{:.1f}x", g_speed).c_str());
    }
    void onFpsUp(CCObject*) {
        g_fps = MIN(g_fps + 10, 360);
        CCDirector::get()->setAnimationInterval(1.0 / g_fps);
        if (m_fpsLbl)
            m_fpsLbl->setString(fmt::format("{}fps", g_fps).c_str());
    }
    void onFpsDown(CCObject*) {
        g_fps = MAX(g_fps - 10, 30);
        CCDirector::get()->setAnimationInterval(1.0 / g_fps);
        if (m_fpsLbl)
            m_fpsLbl->setString(fmt::format("{}fps", g_fps).c_str());
    }
    void onClose(CCObject*) {
        g_menuOpen = false;
        this->removeFromParent();
    }
};

// ===== КНОПКА MH =====
class MHButton : public CCNode {
public:
    static MHButton* create() {
        auto ret = new MHButton();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() override {
        if (!CCNode::init()) return false;
        auto win = CCDirector::get()->getWinSize();

        auto bg = CCLayerColor::create({20,20,40,210}, 36, 16);
        bg->setPosition({4, win.height - 20});
        this->addChild(bg);

        auto lbl = CCLabelTTF::create("MH", "Arial Bold", 9);
        lbl->setPosition({22, win.height - 12});
        lbl->setColor({120, 200, 255});
        this->addChild(lbl);

        auto menu = CCMenu::create();
        menu->setPosition({0,0});
        this->addChild(menu);

        auto btn = CCMenuItemSpriteExtra::create(
            CCLayerColor::create({255,255,255,0}, 36, 16),
            this, menu_selector(MHButton::onOpen));
        btn->setAnchorPoint({0,0});
        btn->setPosition({4, win.height - 20});
        menu->addChild(btn);

        return true;
    }

    void onOpen(CCObject*) {
        MegaHackMenu::openMenu();
    }
};

// ===== ИНФО НОУКЛИПА =====
class NoclipInfo : public CCNode {
public:
    CCLabelTTF* m_d;
    CCLabelTTF* m_a;
    int m_totalFrames = 0;
    int m_deathFrames = 0;

    static NoclipInfo* create() {
        auto ret = new NoclipInfo();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init() override {
        if (!CCNode::init()) return false;
        auto win = CCDirector::get()->getWinSize();

        auto bg = CCLayerColor::create({0,0,0,100}, 105, 26);
        bg->setPosition({4, win.height - 50});
        this->addChild(bg);

        m_d = CCLabelTTF::create("Deaths: 0", "Arial", 9);
        m_d->setAnchorPoint({0,0.5f});
        m_d->setPosition({8, win.height - 39});
        m_d->setColor({255,100,100});
        this->addChild(m_d);

        m_a = CCLabelTTF::create("Acc: 100.0%", "Arial", 9);
        m_a->setAnchorPoint({0,0.5f});
        m_a->setPosition({8, win.height - 50});
        m_a->setColor({100,255,100});
        this->addChild(m_a);

        this->schedule(schedule_selector(NoclipInfo::tick), 0.05f);
        return true;
    }

    void tick(float) {
        m_d->setString(fmt::format("Deaths: {}", g_deaths).c_str());
        m_a->setString(fmt::format("Acc: {:.1f}%", g_accuracy).c_str());
    }

    void addDeath() {
        g_deaths++;
        m_deathFrames++;
    }

    void addFrame() {
        m_totalFrames++;
        if (m_totalFrames > 0) {
            g_accuracy = 100.0f * (1.0f -
                (float)m_deathFrames / m_totalFrames);
        }
    }
};

// ===== ХУКИ =====
class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        NoclipInfo* info = nullptr;
    };

    void destroyPlayer(PlayerObject* p, GameObject* o) {
        if (g_noclip) {
            if (m_fields->info) m_fields->info->addDeath();
            return;
        }
        PlayLayer::destroyPlayer(p, o);
        if (g_farm) {
            this->scheduleOnce(
                schedule_selector(MyPlayLayer::killMe), 0.05f);
        }
    }

    void startGame() {
        PlayLayer::startGame();
        g_deaths = 0;
        g_accuracy = 100.0f;

        // Платформер для всех режимов
        if (g_allModesPlatformer) {
            this->m_isPlatformer = true;
        }

        auto btn = MHButton::create();
        this->addChild(btn, 999);

        auto info = NoclipInfo::create();
        m_fields->info = info;
        this->addChild(info, 999);

        if (g_instantComplete) {
            this->scheduleOnce(
                schedule_selector(MyPlayLayer::doComplete), 0.3f);
        }
    }

    void update(float dt) {
        PlayLayer::update(dt);
        if (m_fields->info) m_fields->info->addFrame();
    }

    void killMe(float) {
        PlayLayer::destroyPlayer(this->m_player1, nullptr);
    }

    void doComplete(float) {
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
        this->addChild(MHButton::create(), 999);
        return true;
    }
};
