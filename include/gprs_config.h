#ifndef _GPRS_CONFIG_H

#define _GPRS_CONFIG_H

#define TINY_GSM_MODEM_SIM800 // SIM800 Compatible with SIM800 AT instructions

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SERIAL_MON

#define TINY_GSM_USE_GPRS true

// Librerias incluidas
#include <Arduino.h>

#include <TinyGsmClient.h>

#include "config.h"
#include "log.h"

#define DEBUG
#include "debug_utils.h"

// TTGO SIM800C pins
#define MODEM_RST 5
#define MODEM_PWRKEY 4
#define MODEM_POWER_ON 25 // 25 en 800C y 23 en 800L
#define MODEM_TX 27
#define MODEM_RX 26

// set GSM PIN
#define GSM_PIN "6090"

// Credenciales GPRS
const char apn[] = "orangeworld";
const char gprsUser[] = "orange";
const char gprsPass[] = "orange";

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SERIAL_AT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SERIAL_AT);
#endif

// Cliente de TinyGSM para la conexion GPRS
TinyGsmClient basic_client(modem);

#endif //_TIME_NPT_H_