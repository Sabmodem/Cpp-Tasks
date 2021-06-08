// Управление, потенциометры - алгоритм действия(Сделано)
// Отключение лишних функций(Сделано)
// EEPROM - что это такое и как оно используется(Сделано)
// Скрины, что поменяно(Вместо них кодx)
// Подключение сервоприводов, алгоритм работы серв, где прописана логика работы
// Прицнип складывания манипулятора(Сделано)

#pragma once
/* Файл переменной конфигурации
 * файл должен быть подключен в начале скетча
 */

// определение всех нужных параметров

// регулирование скорости
#define SPEED_MIN   90  // наименьшее допустимое значение 90
#define SPEED_MAX   255 // наибольшее допустимое значение 255
#define SPEED_STEP  30  // размер приращения шага

// временные настройки
#define TIME_LED            500 // время моргания светодиода
#define TIME_STAND_IDLE     20000 // время бездействия, после которого робот перейдет в режим бездействия
#define TIME_BEEP_CYCLE   10000 // время через которое повторяется оповещение
#define TIME_ANIMATION_LEFT_RIGHT    1000 //время смены глаз
#define TIME_VOLTAGE_MEASUREMENT    20  //время между измерениями напряжения. Общее время между обновлением значения нарпяжения равно (TIME_VOLTAGE_MEASUREMENT + 15) * ADC_MAX_COUNT * NUMBER_OF_ADC_MEASUREMENTS

// режимы работы джойстика
#define JOY_PRESSURES true  // аналоговое считывание кнопок
#define JOY_RUMBLE    false  // вибромотор

// подключение серв (выводы/каналы pca)
/* #define SERVO_BUCKET_CH   6 // канал сервы ковша */
/* #define SERVO_BUCKET_GRAB_CH 7  // канал сервы схвата ковша   //+ */
/* #define SERVO_PLOW_CH     5 // канал сервы плуга */
/* #define SERVO_PLANT_CH    4 // канал сервы диспенсора */

#define SERVO_WRIST_1    7
#define SERVO_WRIST_2    2
#define SERVO_CARPUS_3    3
#define SERVO_CUBIT_4    4
#define SERVO_CUBIT_5    5
#define SERVO_SHOULDER_6    6


#define SERVO_STEP  2   // шаг изменения положения сервы при движении в рабочем режиме, влияет на скорость движения сервы

/* #define PLANT_ACTIVE_DELAY  500 // сколько времени диспенсер будет в активном положении */


/* проверка параметров - при настройке не трогаем */
#if !(defined(SPEED_MIN) && defined(SPEED_MAX) && defined(SPEED_STEP))  // если не продефайнены значения скоростей
# error "Speed parameters are not defined"
#endif

#if !defined(TIME_STAND_IDLE)   // если не продефайнены значения временных задержек
# error "Time parameters are not defined"
#endif

#if !(defined(JOY_PRESSURES) && defined(JOY_RUMBLE))  // если не продефайнены параметры джойстика
# error "Joystick modes not defined"
#endif

/* #if !(defined(SERVO_BUCKET_CH) && defined(SERVO_BUCKET_GRAB_CH) && defined(SERVO_PLOW_CH) && defined(SERVO_PLANT_CH)) // если не продефайнены каналы серв */
/* # error "Servo channels not defined" */
/* #endif */

// подключаем фиксированные параметры, проверку не делаем, т.к. не меняем их
#include "fixed.h"
