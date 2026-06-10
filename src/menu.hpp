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

void openMegaHackMenu();
void addMegaHackButton(CCNode* parent);
