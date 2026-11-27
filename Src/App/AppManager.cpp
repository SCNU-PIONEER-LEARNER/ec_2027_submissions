#include "AppManager.hpp"
#include "sdkconfig.h"

#include "Cmd.hpp"
#include "Bsp.hpp"
#include "Lazy.hpp"

#if EXTENSION_BUZZER
#include "Buzzer.hpp"
#endif

#if EXTENSION_LED
#include "LEDManager.hpp"
#endif

#if APP_USE_DAEMONS
#include "Daemons.hpp"
#endif

//---------------------------------------------------------------------------------------------------

#if APP_USE_INS
#include "INS.hpp"
Lazy<INS_SYS::INS> ins;
#endif

//---------------------------------------------------------------------------------------------------

Lazy<Cmd> cmd;

#ifdef CHASSIS_TYPE
#if !APP_USE_COMM || TARGET_SLAVE
#include CHASSIS_FILE
Lazy<CHASSIS::CHASSIS_TYPE> chassis;

#endif
#endif
#ifdef GIMBAL_TYPE
#if !APP_USE_COMM || TARGET_MASTER
#include GIMBAL_FILE
Lazy<GIMBAL::GIMBAL_TYPE> gimbal;
#endif
#endif

//---------------------------------------------------------------------------------------------------

#if APP_USE_UI
#include "UI/UIApp.hpp"
#endif

//---------------------------------------------------------------------------------------------------

void AppManager::initApp()
{
#if EXTENSION_BUZZER
    BUZZER::buzz.init(&BEEP_TIMER, BEEP_TIM_CHANNEL, BEEP_APB_FREQ);
    BUZZER::buzz->playNote(BUZZER::PinyCore);
#endif

#if APP_USE_COMM
    schedule([]() { CommManager::instance().rxTask(); });
#endif

#if APP_USE_INS
    ins.init(&IMU_SPI);
#endif

    cmd.init();

#if APP_USE_UI
    UI::APP::instance().init();
    schedule([]() { UI::APP::instance().task(); });
#endif
#if APP_USE_DAEMONS
    Daemons::instance().init();
    schedule([]() { Daemons::instance().update(); });
#endif

#if !APP_USE_COMM || TARGET_SLAVE
    //TODO:

#endif
#if !APP_USE_COMM || TARGET_MASTER
   //TODO:

#endif



#if APP_USE_COMM
    schedule([]() { CommManager::instance().txTask(); });
#endif

#if EXTENSION_LED
    // add custom's leds
#endif

    // Generate threads at the end
    this->createApp();
}

void AppManager::createApp()
{
#if APP_USE_TEST
    // Test-Module Continuous Task
    TestModule::instance();
#endif

    uint32_t freeHeap = xPortGetFreeHeapSize();
    LOG::info("App", "init complete, Free Heap: %u", freeHeap);
}

void AppManager::task()
{
    while (true) {
        for (auto &task : this->tasks) {
            task();
        }
        vTaskDelay(1);
    }
}