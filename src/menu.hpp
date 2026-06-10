#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

extern bool g_noclip;
extern bool g_farm;
extern bool g_safeMode;
extern float g_speed;
extern int g_deaths;
extern float g_accuracy;
extern bool g_menuOpen;

class MegaHackMenu : public CCLayer {
public:
    CCLabelBMFont* m_speedLabel;
    CCLabelBMFont* m_fpsLabel;
    int m_fps = 60;

    static MegaHackMenu* create();
    bool init();
    void addColTitle(const char* name, float x, float y, CCMenu* menu);
    void addRow(const char* name, bool& var, float x, float y, CCMenu* menu, SEL_MenuHandler sel);
    void onNoclip(CCObject*);
    void onFarm(CCObject*);
    void onSafeMode(CCObject*);
    void onSpeedUp(CCObject*);
    void onSpeedDown(CCObject*);
    void onFpsUp(CCObject*);
    void onFpsDown(CCObject*);
    void onClose(CCObject*);
};

class MegaHackButton : public CCNode {
public:
    static MegaHackButton* create();
    bool init();
    void onOpen(CCObject*);
};
