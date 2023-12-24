#include <Arduino.h>

#include <ArduinoJson.h>

#include "server_functions.h"
#include "sensor_functions.h"
#include "wifi_functions.h"
#include "gprs_functions.h"
#include "config.h"
#include "log.h"

#define DEBUG
#include "debug_utils.h"

void setup()
{
  //***** Inicializamos el pin del pluviometro y su interrupcion *****//
  pinMode(RAIN_SENSOR, INPUT);

  attachInterrupt(RAIN_SENSOR, sumLiters_m2, FALLING);
  delay(50);

  /*   //S Inicializamos el pin del anemometro y su interrupcion
    pinMode(ANEMOMETER_SENSOR, INPUT);

    attachInterrupt(ANEMOMETER_SENSOR, wind_velocity_interrupt, RISING); */

  //***** Establece la velocidad para el Monitor Serie *****//
  SERIAL_MON.begin(UART_BAUD);
  delay(100);

  //***** Configuramos el modo sleep del ESP32 *****//
  esp_sleep_enable_timer_wakeup(TIME_SLEEP * S_a_M_FACTOR * uS_a_S_FACTOR);

  //***** Configura el pin para despertar al esp32 cuando se reciba un valor alto en el pin conectado al pluviometro *****//
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_14, 1);

  delay(100);

  //***** Mide el voltaje de la bateria y si es inferior a 3.0 V, se duerme al ESP32 *****//
  if (read_voltaje() < voltaje_bat_min)
  {
    DEBUG_PRINT("Voltaje bajo, ESP32 a dormir");

    // esp_deep_sleep_start();
  }

  //***** Suma la cantidad de un vaso del pluviometro, si este ha despertado al ESP32 *****//
  wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0)
  {
    liters_m2 += u_pluviometro;
  }

  // Reinicia  el ESP32 cada 144 veces despierto, que coincide con aproximadamente cada 24 horas
  if (send_data_count >= 144)
  {
    ESP.restart();
  }

  //***** Se conecta al wifi *****//
  connectedWiFi();

  // Inicia red GPRS
  gprs_init();

  // Si llega ha este punto se ha iniciado correctamente y se detalla en el log
  write_log("Iniciado correctamente...");

  delay(100);
}

void loop()
{
  //***** Obtnemos el minuto de la hora, a fin de calcular si ha pasado el tiempo suficiente para realizar las medidas de nuevo *****//
  current_minute = get_minute();

  diff_minutes_last_send_data = current_minute - last_minute;

  // Corregimos a formato sesagesimal en caso de que haya cambio de hora
  if (diff_minutes_last_send_data < 0)
  {
    diff_minutes_last_send_data += 60;
  }

  DEBUG_PRINT("Diferencia desde ultimo envio: " + String(diff_minutes_last_send_data));

  if ((diff_minutes_last_send_data >= MINUTES_BETWEEN_SEND_DATA) || send_data_count == 0)
  {
    send_data = true;
  }
  else
  {
    send_data = false;
  }

  //***** Si la conexion sigue activa, y han pasado los minutos establecidos entre datos enviados, mandamos los datos al servidor *****//
  if (modem.isGprsConnected() && current_minute != -1 && send_data == true)
  {
    //***** Establecemos el ultimo tiempo en que se envio los datos *****//
    last_minute = current_minute;

    //***** Inicializamos el sensor BME280 *****//
    if (bme.begin())
    {
      DEBUG_PRINT("BME280 encontrado!");
      write_log("BME280 encontrado!");

      //***** Leemos los datos del sensor BME280*****//
      temp = bme.readTemperature();       // Almacena en variable el valor de temperatura
      presion = bme.readPressure() / 100; // Almacena en variable el valor de presion divido por 100 para covertirlo a hectopascales
      humedity = bme.readHumidity();      // Almacena en variable el valor de humedad
    }
    else
    {
      // Si falla la comunicacion con el sensor mostrar texto y detener flujo del programa
      DEBUG_PRINT("BME280 no encontrado!");
      write_log("BME280 no encontrado!");

      // while (1);
    }

    //***** Leemos la direccion del viento desde la veleta y se toma las medidas de su velocidad *****//
    // wind_direction = read_wind_direction();

    // measure_vel_wind();
    // avg_wind = avg_velocity();
    // min_wind = min_velocity();
    // max_wind = max_velocity();

    delay(100);

    // Contruye el objeto JSON a partir de los datos obtenidos
    String jsonOutput;
    const size_t CAPACITY = JSON_OBJECT_SIZE(13);
    StaticJsonDocument<CAPACITY> doc;

    JsonObject object = doc.to<JsonObject>();
    object["location"] = location;
    object["date"] = get_date();
    object["temp"] = temp;
    object["hum"] = humedity;
    object["pressure"] = presion;
    object["water"] = liters_m2;
    object["avg_wind"] = wind_avg;
    object["min_wind"] = wind_max;
    object["max_wind"] = wind_min;
    object["dir_wind"] = wind_direction;
    object["voltaje"] = voltaje_bat;

    serializeJson(doc, jsonOutput);

    DEBUG_PRINT("JSON: " + jsonOutput);

    // Envia los datos al servidor
    gprs_https_post(jsonOutput);

    delay(1000);

    //***** Imprimimos los valores de la estación en la consola *****//
    print_data_serial();

    //***** Resetea la lluvia caida *****//
    liters_m2 = 0;

    //***** Establece en false el envio de datos hasta que pase de nuevo 10 minutos *****//
    send_data = false;

    // Suma las veces que ha enviado datos
    send_data_count++;
  }
  else
  {DEBUG_PRINT("llega else...");
    //***** ESP32 a dormir si procede ******//
    if ((!keep_awake) && (diff_minutes_last_send_data > TIME_SERVER))
    {
      gprs_disconnect();

      DEBUG_PRINT("ESP32 to sleep!");

      //***** Configuramos el modo sleep del ESP32 *****//
      esp_sleep_enable_timer_wakeup((TIME_SLEEP - diff_minutes_last_send_data) * S_a_M_FACTOR * uS_a_S_FACTOR);

      esp_deep_sleep_start();
    }
  }
}