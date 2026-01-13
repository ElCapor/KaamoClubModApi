#include "memoryutils.h"
#include <Game/player.h>
#include <Game/system.h>
#include <Game/station.h>
#include <Game/mission.h>
#include <Game/asset.h>
#include <thread>
#include <chrono>

void Player::init()
{
    auto start = std::chrono::high_resolution_clock::now();
    uintptr_t base = MemoryUtils::GetModuleBase("GoF2.exe");

    while (globals_status == nullptr) {
        globals_status = *reinterpret_cast<Globals_status**>(base + 0x20AD6C); // Globals::status
        if (globals_status == nullptr) 
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    while (globals_appmanager == nullptr) {
        globals_appmanager = *reinterpret_cast<Globals_appManager**>(base + 0x20AEFC); // Globals::appManager
        if (globals_appmanager == nullptr)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int Player::getmoney()
{
    return globals_status->m_nMoney;
}

void Player::setmoney(int value)
{
    globals_status->m_nMoney = value;
}

int Player::getmaxcargo()
{
   if (globals_status->m_pShipInfo == nullptr) 
        return 0;
    return globals_status->m_pShipInfo->m_nMaxCapacity;
}

void Player::setmaxcargo(int value)
{
   if (globals_status->m_pShipInfo == nullptr)
        return;
    globals_status->m_pShipInfo->m_nMaxCapacity;
}

int Player::getcargo()
{
    if (globals_status->m_pShipInfo == nullptr) // Valkyrie crash fix, some missions might force change your ship
        return 0;
    return globals_status->m_pShipInfo->m_nCurrentCapacity;
}

void Player::setcargo(int value)
{
    if (globals_status->m_pShipInfo == nullptr)
        return;
    globals_status->m_pShipInfo->m_nCurrentCapacity = value;
}

int Player::getshiparmor()
{
   if (globals_status->m_pShipInfo == nullptr) 
        return 0;
    return globals_status->m_pShipInfo->m_nArmor;
}

void Player::setshiparmor(int value)
{
   if (globals_status->m_pShipInfo == nullptr) 
        return;
    globals_status->m_pShipInfo->m_nArmor = value;
}

bool Player::hasshiparmor()
{
    if (getshiparmor() != 0)
        return true;
    return false;
}

int Player::getmaxshiphealth()
{
   if (globals_status->m_pShipInfo == nullptr) 
        return 0;
    return globals_status->m_pShipInfo->m_nMaxHealth;
}

void Player::setmaxshiphealth(int value)
{
   if (globals_status->m_pShipInfo == nullptr) 
        return;
    globals_status->m_pShipInfo->m_nMaxHealth = value;
}

int Player::getenemieskilled()
{
    return globals_status->m_nEnemiesKilled;
}

void Player::setenemieskilled(int value)
{
    globals_status->m_nEnemiesKilled = value;
}

int Player::getlevel()
{
    return globals_status->m_nPlayerLevel;
}

void Player::setlevel(int value)
{
    globals_status->m_nPlayerLevel = value;
}

int Player::getvisitedstations()
{
    return globals_status->m_nVisitedStations;
}

void Player::setvisitedstations(int value)
{
    globals_status->m_nVisitedStations = value;
}

int Player::getcargosalvagedcount()
{
    return globals_status->m_nCargoSalvaged;
}

void Player::setcargosalvagedcount(int value)
{
    globals_status->m_nCargoSalvaged = value;
}

bool Player::isdocked(void)
{
    if (globals_appmanager->m_nCurrentModule == 5)
        return true;
    return false;
}

int Player::getasteroidsdestroyedcount()
{
    return globals_status->m_nAsteroidsDestroyed;
}

void Player::setasteroidsdestroyedcount(int value)
{
    globals_status->m_nAsteroidsDestroyed = value;
}