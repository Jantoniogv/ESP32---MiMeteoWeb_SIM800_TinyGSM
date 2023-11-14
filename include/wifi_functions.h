#ifndef _WIFI_FUNCTIONS_H

#define _WIFI_FUNCTIONS_H

//***** Incluimos la librerias necesarias *****//
#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include "server_functions.h"
#include "config.h"
#include "time_npt.h"
#include "log.h"

#define DEBUG
#include "debug_utils.h"

//***** Conecta al ESP32 si a internet mediante wifi *****//
void connectedWiFi()
{
    // Configuramos el modo del wifi
    WiFi.mode(initWifiType);

    // Conexion AP WiFi
    WiFi.softAP(initSsidAP, initPassAP);

    // Asignamos la IP del punto de acceso
    WiFi.softAPConfig(init_IP_ap, init_IP_ap, subnet);

    DEBUG_PRINT("SsidAP: " + String(initSsidAP) + "PassAP: " + String(initPassAP));
    write_log("SsidAP: " + String(initSsidAP) + "PassAP: " + String(initPassAP));

    DEBUG_PRINT("IP as soft AP: " + WiFi.softAPIP().toString());
    write_log("IP as soft AP: " + WiFi.softAPIP().toString());

    // Se inicia el servidor web
    init_server();
}

#endif //_WIFI_FUNCTIONS_H