#pragma once
#include "stdint.h"
/* Файл фиксированных параметров */

// для работы с EEPROM (адреса положений серв, записываемых в EEPROM)
#define EEPROM_ADDR_SERV_WRIST_2_MIN    0   // идем с нуля, дальше прибавляем адреса
#define EEPROM_ADDR_SERV_WRIST_2_MAX    EEPROM_ADDR_SERV_WRIST_2_MIN + sizeof(uint16_t)

#define EEPROM_ADDR_SERV_CARPUS_3_MIN     EEPROM_ADDR_SERV_WRIST_2_MAX + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_CARPUS_3_MAX     EEPROM_ADDR_SERV_CARPUS_3_MIN + sizeof(uint16_t)

#define EEPROM_ADDR_SERV_CUBIT_4_MIN  EEPROM_ADDR_SERV_CARPUS_3_MAX + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_CUBIT_4_MAX  EEPROM_ADDR_SERV_CUBIT_4_MIN + sizeof(uint16_t)

#define EEPROM_ADDR_SERV_CUBIT_5_MIN   EEPROM_ADDR_SERV_CUBIT_4_MAX + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_CUBIT_5_MAX   EEPROM_ADDR_SERV_CUBIT_5_MIN + sizeof(uint16_t)

#define EEPROM_ADDR_SERV_SHOULDER_6_MIN   EEPROM_ADDR_SERV_CUBIT_5_MAX + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_SHOULDER_6_MAX   EEPROM_ADDR_SERV_SHOULDER_6_MIN + sizeof(uint16_t)

#define EEPROM_ADDR_SERV_WRIST_2_START    EEPROM_ADDR_SERV_SHOULDER_6_MAX + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_CARPUS_3_START    EEPROM_ADDR_SERV_WRIST_2_START + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_CUBIT_4_START       EEPROM_ADDR_SERV_CARPUS_3_START + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_CUBIT_5_START   EEPROM_ADDR_SERV_CUBIT_4_START + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_SHOULDER_6_START   EEPROM_ADDR_SERV_CUBIT_5_START + sizeof(uint16_t)

#define EEPROM_ADDR_SERV_WRIST_1_MIN   EEPROM_ADDR_SERV_SHOULDER_6_START + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_WRIST_1_MAX   EEPROM_ADDR_SERV_WRIST_1_MIN + sizeof(uint16_t)
#define EEPROM_ADDR_SERV_WRIST_1_START    EEPROM_ADDR_SERV_WRIST_1_MAX + sizeof(uint16_t)



#define SERVO_CENTRAL_POSITION  350  // центральное положение серв (1500 мкс)
#define SERVO_FREQ  60  // частота ШИМ (~57Гц)
#define SERVO_CALIBRATE_DELAY 1   // зедержка сервы при калибровке - влияет на скорость при калибровке
#define SERVO_DELAY 3 // задержка сервы при движении - влияет на скорость в рабочем режиме
#define SERVO_RETURN_DELAY 15 // задержка сервы при движении - влияет на скорость в рабочем режиме

// выводы драйвера моторов
#define MOTOR_ENABLE_A_CH     10  // пин разрешающий работу мотора A
#define MOTOR_ENABLE_B_CH     9   // пин разрешающий работу мотора B
#define MOTOR_PWM_A_CH        5  // канал ШИМа мотора А
#define MOTOR_PWM_B_CH        6  // канал ШИМа мотора B
#define MOTOR_PWM_INVERSE_A_CH   8   // пин инвертирующий ШИМ на канале мотора А
#define MOTOR_PWM_INVERSE_B_CH   4  // пин инвертирующий ШИМ на канале мотора B

// выводы джойстика
#define JOY_DAT_CH  A3
#define JOY_CMD_CH  A2
#define JOY_SEL_CH  2
#define JOY_CLK_CH  3

// вывод дисплея
#define DISPLAY_RESET_CH  4

// работа со звуком
#define BUZZER_CH 7  // вывод, подключённый к динамику

// информационный светодиод
#define LED_CH LED_BUILTIN  // вывод, подключённый к светодиоду

// работа с АЦП
#define ADC_VOLTAGE_CH  A7  // канал считывания напряжения
#define ADC_MAX_COUNT    15  // задержка преобразования АЦП
#define NUMBER_OF_ADC_MEASUREMENTS 10 //количество измерений для взятия среднего напряжения
#define MAX_SUPPLY_VOLTAGE 8.4 //Два аккумулятора формата 18650 выдют максимально 8.4 вольта
#define MIN_SUPPLY_VOLTAGE 6.0 //Мимальное рабочее напряжение для 2ух аакумуляторов 18650 2.75В * 2 = ~6В
#define ADC_MAX_VOLT_VALUE 1020 //значение АЦП при напряжении источника MAX_SUPPLY_VOLTAGE
#define ADC_MIN_VOLT_VALYE 735 //значение АЦП при напряжении источника MIN_SUPPLY_VOLTAGE

//координаты для заполнения уровня заряда на батарейки
#define X_1 27
#define Y_1 18
#define X_2 89
#define Y_2 48

// Итерируемы объекты - каналы серв и их имена для режима калибровки - нуль-терминальные строки
const unsigned char SERVO_ITERATED[7] = {SERVO_WRIST_1, SERVO_WRIST_2, SERVO_CARPUS_3, SERVO_CUBIT_4, SERVO_CUBIT_5, SERVO_SHOULDER_6, '\0'};
const char * SERVO_NAMES_ITERATED[7] = {"WRIST_1", "WRIST_2", "CARPUS_3", "CUBIT_4", "CUBIT_5", "SHOULDER_6", '\0'};
uint16_t SERVO_POSITIONS_ITERATED[7] = {SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION, '\0'};
/* uint16_t SERVO_START_POSITIONS_ITERATED[5] = {SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION, SERVO_CENTRAL_POSITION}; */

const unsigned int EEPROM_ADDR_SERV_MIN[7] = {
  EEPROM_ADDR_SERV_WRIST_1_MIN,
  EEPROM_ADDR_SERV_WRIST_2_MIN,
  EEPROM_ADDR_SERV_CARPUS_3_MIN,    // адреса максимальных позиций серв в епроме
  EEPROM_ADDR_SERV_CUBIT_4_MIN,
  EEPROM_ADDR_SERV_CUBIT_5_MIN,
  EEPROM_ADDR_SERV_SHOULDER_6_MIN,
  '\0'};

const unsigned int EEPROM_ADDR_SERV_MAX[7] = {
  EEPROM_ADDR_SERV_WRIST_1_MAX,
  EEPROM_ADDR_SERV_WRIST_2_MAX,
  EEPROM_ADDR_SERV_CARPUS_3_MAX,
  EEPROM_ADDR_SERV_CUBIT_4_MAX,
  EEPROM_ADDR_SERV_CUBIT_5_MAX,
  EEPROM_ADDR_SERV_SHOULDER_6_MAX,
  '\0'};

const unsigned int EEPROM_ADDR_SERV_START[7] = {
  EEPROM_ADDR_SERV_WRIST_1_START,
  EEPROM_ADDR_SERV_WRIST_2_START,
  EEPROM_ADDR_SERV_CARPUS_3_START,
  EEPROM_ADDR_SERV_CUBIT_4_START,
  EEPROM_ADDR_SERV_CUBIT_5_START,
  EEPROM_ADDR_SERV_SHOULDER_6_START,
  '\0'};


/* const unsigned char SERVO_ITERATED[5] = {SERVO_PLANT_CH, SERVO_PLOW_CH, SERVO_BUCKET_GRAB_CH, SERVO_BUCKET_CH, '\0'}; */
/* const char * SERVO_NAMES_ITERATED[5] = {"Plant", "Plow", "BucketGrab", "Bucket", '\0'}; */
/* const unsigned int EEPROM_ADDR_SERV_MIN[5] = {EEPROM_ADDR_SERV_PLANT_MIN, EEPROM_ADDR_SERV_PLOW_MIN,    // адреса максимальных позиций серв в епроме */
/*                                               EEPROM_ADDR_SERV_BUCKET_GRAB_MIN, EEPROM_ADDR_SERV_BUCKET_MIN, '\0'}; */
/* const unsigned int EEPROM_ADDR_SERV_MAX[5] = {EEPROM_ADDR_SERV_PLANT_MAX, EEPROM_ADDR_SERV_PLOW_MAX,    // адреса максимальных позиций серв в епроме */
/*                                               EEPROM_ADDR_SERV_BUCKET_GRAB_MAX, EEPROM_ADDR_SERV_BUCKET_MAX, '\0'}; */
