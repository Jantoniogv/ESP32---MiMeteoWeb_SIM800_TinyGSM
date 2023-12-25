#ifndef _CONFIG_H

#define _CONFIG_H

#include <Arduino.h>

#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Set serial monitor
#define SERIAL_MON Serial

// Set serial comandos AT
#define SERIAL_AT Serial1

// Velocidad conexion SerialAT
#define UART_BAUD 115200

// Pluviometro pin
#define RAIN_SENSOR 14

// Definimos el pin 35 como pin de obtencion de datos de la veleta
#define WIND_DIRECTION_SENSOR 35

// Definimos el pin 34 como pin de obtencion de datos del anemometro
#define ANEMOMETER_SENSOR 32

// Definimos el pin 32 como medidor de voltaje
#define VOLTAJE_BATTERY 13

//***** Establecemos los factores de tiempo *****//
#define MINUTES_BETWEEN_SEND_DATA 10 /* Tiempo entre cada envio de datos */

#define uS_a_S_FACTOR 1000000 /* 1000000 microsegundos/segundo */

#define mS_a_S_FACTOR 1000 /* 1000 milisegundos/segundo */

#define S_a_M_FACTOR 60 /* 60 segundos/minuto para tener segundos */

#define TIME_SLEEP 10 /* Tiempo que duerme en minutos */

#define TIME_SLEEP_RESET_GPRS_FAIL 1 // Tiempo que duerme en segundos al reiniciar por software el ESP32,
                                     // ya de otro modo el SIM800 no inicia correctamente

#define TIME_SERVER 1 /* Tiempo que esta despierto esperando conexion el servidor web en segundos */

//***** Numero de muestras *****//
#define N_SAMPLES_WIND_DIRECTION 20 // Numero de muestras que toma para medir la direccion del viento

#define N_COMP_WIND_DIRECTION 17 // Numero de componentes posibles de direccion del viento

#define N_SAMPLES_VOL 10 // Numero de muestras que toma para medir el voltaje

bool keep_awake = false; // Estado que almacena si debe mantener despierto al ESP32 o no

//***** Constantes y objetos necesarios para realizar la conexion WiFi *****//
wifi_mode_t initWifiType = WIFI_MODE_AP;

const char *initSsidAP = "MyESP32AP_2022"; // SSID conexion WiFi AP
const char *initPassAP = "adminadmin";     // Contraseña conexion WiFi AP

IPAddress init_IP_ap = IPAddress(192, 168, 10, 1);
IPAddress subnet = IPAddress(255, 255, 255, 0);

//***** Constantes que definen los server y url a los que se conecta el ESP32 y envia los datos *****//

const String mimeteoweb_host = "mimeteoweb.up.railway.app";

const String mimeteoweb_post_data = "/api/v1/add-meteo-dates";

//***** Constantes y objetos necesarios para realizar la conexion NTP y obtener la hora *****//
const char *ntpServer = "pool.ntp.org";
const int gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

// Estructura de fecha y tiempo
struct time_info
{
    int year = -1;
    int month = -1;
    int day = -1;
    int hour = -1;
    int min = -1;
    int sec = -1;
    float timezone = -1;

} time_info;

//***** Constante del lugar donde se situa la estacion meteorologica *****//
String location = "lucena";

//***** Variables para almacenar los datos de las mediciones del BME280 *****//
float temp = 0;     // variable para almacenar valor de temperatura
float presion = 0;  // variable para almacenar valor de presion atmosferica
float humedity = 0; // variable para almacenar valor de presion atmosferica

//***** Variable en las que se almacenara los valores medidos para la lluvia *****//
RTC_DATA_ATTR float liters_m2 = 0; // Esta variable estara almacenada en la memoria del reloj RTC a fin de que se guarde su valor durante los periodos de sueño

const float u_pluviometro = 0.45; // Unidad medida del pluviometro

unsigned long last_t_liters = 0;    // Variable que almacena el ultimo tiempo en ms cuado se registro un pulso del pluviometro
unsigned long current_t_liters = 0; // Variable que almacena el tiempo actual en ms del pulso del pluviometro

int measurements_t_liters = 500; // Tiempo en ms, en los que se desprecia los pulsos registrado por el pluviometro, a fin de desprecia los rebotes

//***** Variables y constantes del anemometro *****//
volatile int rev_anemometer = 0; // Esta variable la encargada de almacenar las rpm  que se leen desde el anemometro

float wind_velocity[5]; // Este array almacena cinco medidas del anemometro a fin de realizar una media de entre ellos

float wind_avg = -1;
float wind_min = -1;
float wind_max = -1;

const int n_sample_wind = 5; // Numero de muestras que toma para calcular la velocidad media del viento

//***** Variables de la veleta *****//

// Arrays que identifican las distintas direcciones del viento, y definen el valor maximo y minimo que toma la lectura analogica del sensor
String wind_dir_comp[] = {"NORTE", "NORTE-NOROESTE", "NOROESTE", "ESTE-NOROESTE", "ESTE", "ESTE-SURESTE", "SURESTE", "SUR-SURESTE",
                          "SUR", "SUR-SUROESTE", "SUROESTE", "OESTE-SUROESTE", "OESTE", "OESTE-NOROESTE", "NOROESTE", "NORTE-NOROESTE", "UNDEFINED"};

int sensor_wind_min[] = {3030, 1460, 1690, 150, 210, 80, 550, 320, 980, 800, 2380, 2250, 3960, 3230, 3560, 2670};
int sensor_wind_max[] = {3110, 1540, 1770, 205, 240, 140, 630, 400, 1060, 880, 2440, 2330, 4040, 3310, 3640, 2750};

// Estas variables sirven para almacenar la direccion del viento, el valor leido del sensor y la media del valor despues de varias medidas
String wind_direction = "--";

// Constantes de medicion de voltaje
const int voltaje_analog = 3450; // Valor analogico esperable para un voltaje de 4,2V pasado por un divisor de tension R_1=1000 ohm y R_2=2000 omh, con un V_out=2,8V
const int voltaje_max_x_100 = 420;

// Almacena la lectura del voltaje
float voltaje_bat_min = 3.0;
float voltaje_bat = 0;

//***** Estas variales se encargan de almacenar la ultima vez que se envio datos y el minuto actual *****//
int diff_minutes_last_send_data = 0;
RTC_DATA_ATTR int last_minute = 0;
int current_minute = 0;

//***** Variable para contar las veces que el ESP32 ha enviado datos *****//
RTC_DATA_ATTR int send_data_count = 0;

//***** Esta variable se encarga de establecer cuando se envia datos 'true' y cuando no 'false' ******//
bool send_data = false;

//***** Creamos el objeto del sensor BME280 *****//
Adafruit_BME280 bme;

//***** Esta variable almacena la razon por la que el ESP32 ha despertado *****//
esp_sleep_wakeup_cause_t wakeup_reason;

//***** Esta variable almacena la razon por la que el ESP32 se ha iniciado o reiniciado *****//
esp_reset_reason_t reason_restart;

#endif //_CONFIG_H
