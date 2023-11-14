#ifndef _TIME_NPT_H_
#define _TIME_NPT_H_

#include <Arduino.h>

#include "gprs_config.h"
#include "config.h"
#include "log.h"

#define DEBUG
#include "debug_utils.h"

// Inicia la configuracion del servidor NTP
void time_npt_init()
{
    DEBUG_PRINT("Init server npt...");
    write_log("Init server npt...");

    modem.NTPServerSync(ntpServer, gmtOffset_sec);

    DEBUG_PRINT("Retrieving time again as a string...");
    write_log("Retrieving time again as a string...");

    String time = modem.getGSMDateTime(DATE_FULL);

    DEBUG_PRINT("Time: " + time);
    write_log("Time: " + time);
}

//***** Funciones que obtienen la hora y los minutos actuales *****//
int get_minute()
{
    if (!modem.getNetworkTime(nullptr, nullptr, nullptr, nullptr, &time_info.min, nullptr, nullptr))
    {
        DEBUG_PRINT("Failed to obtain time");
        write_log("Failed to obtain time");

        return -1;
    }
    else
    {
        DEBUG_PRINT("Minute: " + (String)time_info.min);
        write_log("Minute: " + (String)time_info.min);

        return time_info.min;
    }
}

String get_date()
{
    String date = "Time fail";

    if (!modem.getNetworkTime(&time_info.year, &time_info.month, &time_info.day, &time_info.hour, &time_info.min, &time_info.sec, nullptr))
    {
        DEBUG_PRINT("Failed to obtain time");
        write_log("Failed to obtain time");

        return "Failed";
    }
    else
    {
        String month = "--";
        String day = "--";
        String hour = "--";
        String min = "--";
        String sec = "--";

        date = (String)time_info.year + "-" +
               ((time_info.month < 10) ? ("0" + (String)time_info.month) : ((String)time_info.month)) + "-" +
               ((time_info.day < 10) ? ("0" + (String)time_info.day) : ((String)time_info.day)) + "T" +
               ((time_info.hour < 10) ? ("0" + (String)time_info.hour) : ((String)time_info.hour)) + ":" +
               ((time_info.min < 10) ? ("0" + (String)time_info.min) : ((String)time_info.min)) + ":" +
               ((time_info.sec < 10) ? ("0" + (String)time_info.sec) : ((String)time_info.sec)) + ".000Z";

        DEBUG_PRINT("Date: " + date);
        write_log("Date: " + date);

        return date;
    }
}

// int get_hour() {
//   struct tm timeinfo;
//
//   if (!getLocalTime(&timeinfo)) {
//     DEBUG_PRINT("Failed to obtain time");
//     return -1;
//   }
//   else {
//     char current_hour[3];
//     strftime(current_hour, 3, "%H", &timeinfo);
//     DEBUG_PRINT(current_hour);
//     return atoi(current_hour);
//   }
// }

#endif //_TIME_NPT_H_