#ifndef _GPRS_FUNCTIONS_H

#define _GPRS_FUNCTIONS_H

#define TINY_GSM_MODEM_SIM800 // SIM800 Compatible with SIM800 AT instructions

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SERIAL_MON

#define TINY_GSM_USE_GPRS true

#include <Arduino.h>

#include <ESP_SSLClient.h>

#include "gprs_config.h"
#include "time_npt.h"
#include "config.h"
#include "log.h"

#define DEBUG
#include "debug_utils.h"

// Cliente ssl para conexiones https
ESP_SSLClient ssl_client;

void gprs_fail()
{
    DEBUG_PRINT("ESP32 a dormir tras dos minutos de espera a conexiones al servidor web propio");
    write_log("ESP32 a dormir tras dos minutos de espera a conexiones al servidor web propio");

    delay(TIME_SERVER * S_a_M_FACTOR * mS_a_S_FACTOR);

    esp_deep_sleep_start();
}

void gprs_init()
{
    // Set modem reset, enable, power pins
#ifdef MODEM_RST
    // Keep reset high
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);
#endif

    pinMode(MODEM_PWRKEY, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);

    // Turn on the Modem power first
    digitalWrite(MODEM_POWER_ON, HIGH);

    // Pull down PWRKEY for more than 1 second according to manual requirements
    digitalWrite(MODEM_PWRKEY, HIGH);
    delay(100);
    digitalWrite(MODEM_PWRKEY, LOW);
    delay(1000);
    digitalWrite(MODEM_PWRKEY, HIGH);

    delay(3000);

    SERIAL_AT.begin(UART_BAUD, SERIAL_8N1, MODEM_RX, MODEM_TX);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    DBG("Initializing modem... ");
    write_log("Initializing modem... ");
    if (!modem.init())
    {
        DBG("Failed to initializing modem");
        write_log("Failed to initializing modem");

        gprs_fail();
    }

    String name = modem.getModemName();
    DBG("Modem Name:", name);

    String modemInfo = modem.getModemInfo();
    DBG("Modem Info:", modemInfo);

#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }
#endif

    DEBUG_PRINT("Waiting for network... ");
    write_log("Waiting for network... ");

    if (!modem.waitForNetwork())
    {
        DEBUG_PRINT("fail");
        write_log("fail");

        gprs_fail();
    }
    DEBUG_PRINT("success");
    write_log("success");

    if (modem.isNetworkConnected())
    {
        DEBUG_PRINT("Network connected");
        write_log("Network connected");
    }

#if TINY_GSM_USE_GPRS
    // GPRS connection parameters are usually set after network registration
    DEBUG_PRINT("Connecting to... ");
    write_log("Connecting to... ");

    DEBUG_PRINT(apn);
    write_log(apn);

    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        DEBUG_PRINT("fail");
        write_log("fail");

        gprs_fail();
    }
    DEBUG_PRINT("success");
    write_log("success");

    if (modem.isGprsConnected())
    {
        DEBUG_PRINT("GPRS connected");
        write_log("GPRS connected");

        // Inicia la configuracion del servidor NTP
        time_npt_init();
    }
#endif

    // ignore server ssl certificate verification
    ssl_client.setInsecure();

    // Set the receive and transmit buffers size in bytes for memory allocation (512 to 16384).
    ssl_client.setBufferSizes(1024 /* rx */, 512 /* tx */);

    ssl_client.setDebugLevel(1);

    // Assign the basic client
    // Due to the basic_client pointer is assigned, to avoid dangling pointer, basic_client should be existed
    // as long as it was used by ssl_client for transportation.
    ssl_client.setClient(&basic_client);
}

void gprs_disconnect()
{
    modem.gprsDisconnect();

    DEBUG_PRINT("GPRS disconnected");
    write_log("GPRS disconnected");
}

void gprs_https_post(String payload)
{
    SERIAL_MON.println("\n---------------------------------");
    write_log("\n---------------------------------");

    SERIAL_MON.print("Connecting to server... ");
    write_log("Connecting to server... ");

    // String payload = "{\"location\":\"lucena\",\"date\":\"2023-11-15T20:24:28.000Z\",\"temp\":22,\"hum\":15}";

    if (ssl_client.connect(mimeteoweb_host.c_str(), 443))
    {
        Serial.println(" ok");
        Serial.println("Send POST request... ");
        ssl_client.print("POST " + mimeteoweb_post_data + " HTTP/1.1\n");
        ssl_client.print("Host: " + mimeteoweb_host + "\n");
        ssl_client.print("Content-Type: application/json\n");
        ssl_client.print("Content-Length: ");
        ssl_client.print(payload.length());
        ssl_client.print("\n");
        ssl_client.print("\n");
        ssl_client.print(payload);

        SERIAL_MON.print("Read response...");
        write_log("Read response...");

        String response = "";
        unsigned long ms = millis();
        while (!ssl_client.available() && millis() - ms < 3000)
        {
            delay(0);
        }
        Serial.println();
        while (ssl_client.available())
        {
            response += (char)ssl_client.read();
        }

        SERIAL_MON.println(response);
        write_log(response);
    }
    else
    {
        SERIAL_MON.println("failed\n");
        write_log("failed");
    }

    // Cierra conexion htpps
    ssl_client.stop();
}

#endif //_GPRS_FUNCTIONS_H