// 1) libraries

#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>
#include <esp_log.h>

#include <soc/rtc_wdt.h>


// 2) ativação do watchdog

watchdogRTC();


// 3) linha para resetar timer do watchdog
 rtc_wdt_feed(); //Alimenta o RTC WDT


// 4) codigo para teste

     if (Serial.available())
     {
      while(1){
      Serial.println("preso");
      delay(1000);
      }
     }


// 5) funcao do final

void watchdogRTC()
{
    rtc_wdt_protect_off();      //Disable RTC WDT write protection
    //Set stage 0 to trigger a system reset after 1000ms
    rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_RTC);
    rtc_wdt_set_time(RTC_WDT_STAGE0, 9000);
    rtc_wdt_enable();           //Start the RTC WDT timer
    rtc_wdt_protect_on();       //Enable RTC WDT write protection
}
