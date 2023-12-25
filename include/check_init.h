#ifndef _CHECK_INIT_H

#define _CHECK_INIT_H

#include <Arduino.h>

#include "sensor_functions.h"
#include "config.h"
#include "log.h"

#define DEBUG
#include "debug_utils.h"

void check_init()
{
    //***** Mide el voltaje de la bateria y si es inferior a 3.0 V, se duerme al ESP32 *****//
    if (read_voltaje() < voltaje_bat_min)
    {
        DEBUG_PRINT("Voltaje bajo, ESP32 a dormir");

        // esp_deep_sleep_start();
    }

    // Si la razon del reinicio es via software, el modulo SIM800 no se inicia correctamente. Asi se ha encontrado que tras
    // el despertear del sueño profundo si se inicia todo correctamente, de esta forma se configura para que duerma 1 segundo
    // y inicie inmediatamente despues

    reason_restart = esp_reset_reason();
    DEBUG_PRINT("Razon reinicio ESP32: " + (String)reason_restart);

    if (reason_restart == ESP_RST_SW)
    {
        esp_sleep_enable_timer_wakeup(TIME_SLEEP_RESET_GPRS_FAIL * uS_a_S_FACTOR);

        esp_deep_sleep_start();
    }
}

#endif //_CHECK_INIT_H
