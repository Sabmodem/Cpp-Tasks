#include "config.h"
#include "stdint.h"
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>

#define IMAGE_WIDTH   128
#define IMAGE_HEIGHT  64

/*
 * Прошивка для агробота прошедшая небольшое ревью, возможно некоторые моменты следовало бы запихнуть в классы,
 * но для одного cpp файла смысла от этого особо не вижу. Сделать большее количество cpp файлов данная среда разработки не представляет возможным.
 * Были вынесены некоторые дефа йны и константы в файлы config.h, fixed.h, pictures.h из-за большого их обилия и усложнения читабельности кода.
 * В файле config.h находятся вещи, которые можно/нужно менять в зависимости от версии или параметров робота.
 * В файле fixed.h находятся вещи, которые не надо трогать, они там только чтобы упростить читаемость кода.
 * В файле pictures.h находятся изображения выводимые на дисплей.
 * Также не вышло избавиться от всех глобальных переменных, но я старался.
 * Исправлена путаница с названием некоторых моторов(теперь есть только мотор А и B, а не моторы 1 и 2 и выходы A и B).
 * Дефайны, константы и названия были отрефакторены и преведены к некоторому условному стандарту.
 * Функционал был разложен по идемпотентным(относительно) функциям, т.е. просто разложил задачи на более мелкие и структуризировал.
 * Была немного пересмотрена логика опроса джойстика и управления - создан конечный автомат с состояниями в виде действий, которые
 * должны происходить, когды будет нажата кнопка или др., и лидирующим состоянием(LEAD), в которое переходят все действия, после отработки,
 * в этом состоянии как раз и производится опрос кнопок. Такая система хорошо расширяется, изменяется и читается, проверено на кадете.
 * И не надо бегать по коду искать в каком if производится опрос конкретной кнопки.
 * Такой же конечный автомат добавлен и для режима калибровки.
 * В режим калибровки добавлен вывод на дисплей текущего состояния сервы - максимального и минимального положения, а также вывод текущего значения.
 * Выбор серв в режиме калибровки закольцован.
 * Произведена оптимизация - в рабочем режиме ограничен вывод на дисплей повторных состояний, это многократно увеличивает скорость работы кода, но
 * уменьшает читаемость и увеличивает размер кода, что является необходимым
 * Сделал табы перед препроцесоорными дерективами в пределах ф-ий(хоть это и не по стандарту, но иначе не читается нормально)
 */

volatile Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); //инициализация i2c для pca с адресом 0x40
volatile Adafruit_SSD1306 display(IMAGE_WIDTH, IMAGE_HEIGHT, &Wire, DISPLAY_RESET_CH); // инициализация дисплея
volatile PS2X ps2x;  // cоздание экземпляра класса для джойстика

// глобальное зло
bool motor_state = false; //хранение состояния моторов для получения значения напряжения аккумуляторов без просадок
uint8_t valueRight = 127;
uint8_t valueLeft = 127;
uint8_t speedLeft = 0;
uint8_t speedRight = 0;

uint32_t standIdleTimer; // таймер отсчета времени бездействия

typedef enum
{
  EYE_UP,
  EYE_DOWN,
  EYE_LEFT,
  EYE_RIGHT,
  EYE_TIRED,
  EYE_DIFFICULT,
  EYE_WOW,
  BATTERY,
  CLEAR
} workDisplayState;   // перечисление состояния экрана в рабочем режиме (все ради оптимизации)


void motorSetup()   // инициализация моторов
{
  pinMode(MOTOR_ENABLE_A_CH, OUTPUT);
  pinMode(MOTOR_ENABLE_B_CH, OUTPUT);
  pinMode(MOTOR_PWM_A_CH, OUTPUT);
  pinMode(MOTOR_PWM_B_CH, OUTPUT);
  pinMode(MOTOR_PWM_INVERSE_A_CH, OUTPUT);
  pinMode(MOTOR_PWM_INVERSE_B_CH, OUTPUT);
}

void buzzerSetup()  // инициализация пищалки
{
  pinMode(BUZZER_CH, OUTPUT);
  noTone(BUZZER_CH);
}

void displaySetup() // инициализация дисплея
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Инициализация I2C для дисплея с адресом 0x3D
  display.display();
  delay(2000); //задержка для инициализации дисплея
  display.clearDisplay(); // очистка дисплея
}

void servoSetup() // инициализация серв
{
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);  // Установка частоты ШИМ
}

void servoCentering() // центрирование серв
{
  for (char servoCounter = 0; servoCounter < (strlen((const char*)SERVO_ITERATED)); servoCounter++) {
    EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], SERVO_POSITIONS_ITERATED[servoCounter]);
    pwm.setPWM(SERVO_ITERATED[servoCounter], 0, SERVO_POSITIONS_ITERATED[servoCounter]);
    /* Serial.print("name: "); */
    /* Serial.println(SERVO_NAMES_ITERATED[servoCounter]); */
    /* Serial.println("CurPos: "); */
    /* Serial.println(SERVO_POSITIONS_ITERATED[servoCounter]); */
  }
}

void returnServoInStartPosition() {
  uint16_t servoStartPos = SERVO_CENTRAL_POSITION;
  for (char servoCounter = 0; servoCounter < (strlen((const char*)SERVO_ITERATED)); servoCounter++) {
    EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], servoStartPos);
    /* EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], SERVO_POSITIONS_ITERATED[servoCounter]); */
    /* pwm.setPWM(SERVO_ITERATED[servoCounter], 0, SERVO_POSITIONS_ITERATED[servoCounter]); */
    if(SERVO_POSITIONS_ITERATED[servoCounter] >= servoStartPos) {
      while(SERVO_POSITIONS_ITERATED[servoCounter] >= servoStartPos) {
        SERVO_POSITIONS_ITERATED[servoCounter] = SERVO_POSITIONS_ITERATED[servoCounter] - SERVO_STEP;
        pwm.setPWM(SERVO_ITERATED[servoCounter], 0, SERVO_POSITIONS_ITERATED[servoCounter]);
        delay(SERVO_RETURN_DELAY); // делаем задержку
      }
    } else if(SERVO_POSITIONS_ITERATED[servoCounter] <= servoStartPos) {
      while(SERVO_POSITIONS_ITERATED[servoCounter] <= servoStartPos) {
        SERVO_POSITIONS_ITERATED[servoCounter] = SERVO_POSITIONS_ITERATED[servoCounter] + SERVO_STEP;
        pwm.setPWM(SERVO_ITERATED[servoCounter], 0, SERVO_POSITIONS_ITERATED[servoCounter]);
        delay(SERVO_RETURN_DELAY); // делаем задержку
      }
    }
  }
}

void readServoRange() // читает значения крайних положений серв из епрома и записывает их в глобальные переменные
{
  return;
}

void servoCalibrateDisplay(char* servoName, uint16_t servoPosition)   // вывод на дисплей название сервы и ее текущую позицию
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(servoName);
  display.setCursor(0, 16);
  display.println(servoPosition);
  display.display();
}

void servoInfoDisplay(char* servoName, uint16_t servoPositionMin, uint16_t servoPositionMax, uint16_t servoPositionStart)
{
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(servoName);
  display.setCursor(0, 10);
  display.println("MIN: ");
  display.setCursor(30, 8);
  display.println(servoPositionMin);
  display.setCursor(0, 20);
  display.println("MAX: ");
  display.setCursor(30, 20);
  display.println(servoPositionMax);
  display.setCursor(0, 30);
  display.println("START: ");
  display.setCursor(50, 30);
  display.println(servoPositionStart);

  display.display();
}

uint16_t rerangeSpeed(int16_t mspeed)  // проверка и корректировка скорости (параметр знаковый, чтоб переполнения не было)
{
  if (mspeed > SPEED_MAX) return SPEED_MAX;
  if (mspeed < SPEED_MIN) return SPEED_MIN;
  return mspeed;
}

//Запуск двигателей
void setSpeedRight(int16_t mspeed)  // первый двигатель - А
{
  if (mspeed > 0)   // если заданная скорость больше нуля, то задаем Прямой ШИМ без инвертирования
  {
    analogWrite(MOTOR_PWM_A_CH, 255);
    digitalWrite(MOTOR_PWM_INVERSE_A_CH, LOW);
  }
  else    // если меньше, то инвертируем направление
  {
    digitalWrite(MOTOR_PWM_INVERSE_A_CH, HIGH);
    analogWrite(MOTOR_PWM_A_CH, 0);
  }
  analogWrite(MOTOR_ENABLE_A_CH, abs(mspeed));
}

void setSpeedLeft(int16_t mspeed) // второй двигатель - B
{
  if (mspeed > 0)
  {
    digitalWrite(MOTOR_PWM_INVERSE_B_CH, HIGH);
    analogWrite(MOTOR_PWM_B_CH, 0);
  }
  else
  {
    analogWrite(MOTOR_PWM_B_CH, 255);
    digitalWrite(MOTOR_PWM_INVERSE_B_CH, LOW);
  }
  analogWrite(MOTOR_ENABLE_B_CH, abs(mspeed));
}

void stopMotors()   // остановка двигателей
{
  analogWrite(MOTOR_PWM_A_CH, 0);
  digitalWrite(MOTOR_PWM_INVERSE_A_CH, LOW);
  digitalWrite(MOTOR_PWM_INVERSE_B_CH, LOW);
  analogWrite(MOTOR_PWM_B_CH, 0);
}

void beep(uint8_t num, uint16_t tim)
{
  for (uint16_t i = 0; i < num; i++)
  {
    digitalWrite(BUZZER_CH, HIGH);
    delay(tim);
    digitalWrite(BUZZER_CH, LOW);
    delay(50);
  }
}

void adcDataCounter(float* voltage)   // вычисление значения напряжения питания, запись в параметры
{
  static float voltage_values[NUMBER_OF_ADC_MEASUREMENTS] = {0}; //массив для хранения NUMBER_OF_ADC_MEASUREMENTS измерений напряжений
  static uint8_t values_count = 0; //счетчик записи данных в массив
  static uint8_t adcCount = ADC_MAX_COUNT;  // ограничение по частоте считывания данных с АЦП
  static float m_voltage = 0;   // доп переменная для того, чтобы не ждать 14 вызовов ф-ии, если пропустили первый
  if (adcCount >= ADC_MAX_COUNT && !motor_state)    //если двигатели стоят, то каждые ADC_MAX_COUNT = 15 раз меняет значения, приходящие в параметрах
  {
    voltage_values[values_count] = ( (float)(analogRead(ADC_VOLTAGE_CH) - ADC_MIN_VOLT_VALYE) / (float)(ADC_MAX_VOLT_VALUE - ADC_MIN_VOLT_VALYE) ) * (MAX_SUPPLY_VOLTAGE - MIN_SUPPLY_VOLTAGE) + MIN_SUPPLY_VOLTAGE; //вычисление напряжения на выходе буффера
    adcCount = 0;
    values_count++;
  }
  if(values_count > NUMBER_OF_ADC_MEASUREMENTS - 1){ //если выполнено NUMBER_OF_ADC_MEASUREMENTS измерений
    values_count = 0;
    *voltage = 0;
    for(uint8_t i = 0; i < NUMBER_OF_ADC_MEASUREMENTS; i++){ //суммируем все
      *voltage += voltage_values[i];
    }
    *voltage = *voltage / NUMBER_OF_ADC_MEASUREMENTS; //и делим на кол-во измерений
  }
  adcCount++;
}

bool calibrationFSM()   // режим калибровки, нетривиальный конечный автомат, где состояния управляются от одного лидера, состояния переключаются по нажатию кнопок джойстика
{
  static int8_t servoCounter = 0;  // каретка, переключающаяся от сервы к серве (нужен знаковый) - текущая выбранная серва из массива
  static uint16_t servoCalibPos = SERVO_CENTRAL_POSITION;   // текущая позиция выбранной сервы
  static uint16_t tempInfoPositionMin = SERVO_CENTRAL_POSITION;   // доп переменные, для считывания информации из епрома о выбранной серве
  static uint16_t tempInfoPositionMax = SERVO_CENTRAL_POSITION;
  static uint16_t tempInfoPositionStart = SERVO_CENTRAL_POSITION;
  static enum
  {
    LEAD,   // главный режим, отсюда идет переход ко всем остальным состояниям
    EEPROM_CLEAR,  // тут происходит очистка EEPROM
    SERVO_NEXT, // переход к следующей серве
    SERVO_PREV, // переход к предыдущей серве
    SERVO_CENTERING,  // центровка выбранной сервы
    SERVO_MOVE_UP,    // увеличение скважности ШИМа на серве
    SERVO_MOVE_DOWN,  // уменьшение скважности ШИМа на серве
    SERVO_FIND_MAX,   // находим максимальную скважность сервы, в одном из крайних положений
    SERVO_FIND_MIN,   // находим минимальную скважность, в одном из крайних положений
    SERVO_FIND_START,
    /* STICK_INFO, */
    EXIT   // выход из конечного автомата, переход к другому режиму
  } state;

  switch(state)
  {
    case LEAD:  // состояние которое переходит в другие состояния, если были нажаты какие-то кнопки
      if (ps2x.Button(PSB_L3) && ps2x.Button(PSB_R3) && ps2x.Button(PSB_R1) && ps2x.Button(PSB_L1)){ state = EEPROM_CLEAR; }
      if (ps2x.ButtonPressed(PSB_R1)) { state = SERVO_NEXT; }
      if (ps2x.ButtonPressed(PSB_L1)) { state = SERVO_PREV; }
      if (ps2x.Button(PSB_L3) || ps2x.Button(PSB_R3)) { state = SERVO_CENTERING; }
      if (ps2x.Button(PSB_PAD_UP)) { state = SERVO_MOVE_UP; }
      if (ps2x.Button(PSB_PAD_DOWN)) { state = SERVO_MOVE_DOWN; }
      if (ps2x.ButtonPressed(PSB_TRIANGLE)) { state = SERVO_FIND_MAX; }
      if (ps2x.ButtonPressed(PSB_CROSS)) { state = SERVO_FIND_MIN; }
      if (ps2x.ButtonPressed(PSB_CIRCLE)) { state = SERVO_FIND_START; }
      /* if (ps2x.Analog(PSS_RX) || ps2x.Analog(PSS_RY) || ps2x.Analog(PSS_LX) || ps2x.Analog(PSS_LY)) { state = STICK_INFO ;} */
      if (ps2x.Button(PSB_L2) && ps2x.Button(PSB_R2)) { state = EXIT; }
      return false;
    case EEPROM_CLEAR:
      // тут будет очистка епрома
      state = LEAD;
      break;

    case SERVO_NEXT:  // делаем кольцевой массив, можем ходить от серве к серве по замкнутому кругу
      servoCounter++;
      if (servoCounter >= (strlen((const char*)SERVO_ITERATED))) servoCounter = 0;
      servoCalibPos = SERVO_CENTRAL_POSITION;   // центровка сервы при переключении
      pwm.setPWM(SERVO_ITERATED[servoCounter], 0, servoCalibPos);   // задаем ей центральное положение

      EEPROM.get(EEPROM_ADDR_SERV_MIN[servoCounter], tempInfoPositionMin);  // после выбора сервы считываем о ней информацию из епрома и выводим на дисплей
      EEPROM.get(EEPROM_ADDR_SERV_MAX[servoCounter], tempInfoPositionMax);
      EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], tempInfoPositionStart);
      servoInfoDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], tempInfoPositionMin, tempInfoPositionMax, tempInfoPositionStart);
      state = LEAD;
      break;

    case SERVO_PREV:  // делаем кольцевой массив, можем ходить от серве к серве по замкнутому кругу
      servoCounter--;
      if (servoCounter < 0) servoCounter = strlen((const char*)SERVO_ITERATED) - 1;
      servoCalibPos = SERVO_CENTRAL_POSITION;   // центровка сервы при переключении
      pwm.setPWM(SERVO_ITERATED[servoCounter], 0, servoCalibPos); // задаем ей центральное положение

      EEPROM.get(EEPROM_ADDR_SERV_MIN[servoCounter], tempInfoPositionMin);  // после выбора сервы считываем о ней информацию из епрома и выводим на дисплей
      EEPROM.get(EEPROM_ADDR_SERV_MAX[servoCounter], tempInfoPositionMax);
      EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], tempInfoPositionStart);
      servoInfoDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], tempInfoPositionMin, tempInfoPositionMax, tempInfoPositionStart);
      state = LEAD;
      break;

    case SERVO_CENTERING:   // центрирование выбранной сервы
      servoCalibPos = SERVO_CENTRAL_POSITION; // установка центрального значения для текущей сервы
      pwm.setPWM(SERVO_ITERATED[servoCounter], 0, servoCalibPos);

      EEPROM.get(EEPROM_ADDR_SERV_MIN[servoCounter], tempInfoPositionMin);    // после центрирования сервы считываем о ней информацию из епрома и выводим на дисплей
      EEPROM.get(EEPROM_ADDR_SERV_MAX[servoCounter], tempInfoPositionMax);
      EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], tempInfoPositionStart);
      servoInfoDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], tempInfoPositionMin, tempInfoPositionMax, tempInfoPositionStart);
      state = LEAD;
      break;

    case SERVO_MOVE_UP:
      servoCalibPos++;    // увеличиваем положение сервы
      pwm.setPWM(SERVO_ITERATED[servoCounter], 0, servoCalibPos);   // задаем его
      delay(SERVO_CALIBRATE_DELAY); // делаем задержку
      servoCalibrateDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], servoCalibPos);   // выводим имя сервы и текущее положение
      state = LEAD;
      break;

    case SERVO_MOVE_DOWN:
      servoCalibPos--;    // уменьшаем положение сервы
      pwm.setPWM(SERVO_ITERATED[servoCounter], 0, servoCalibPos); // задаем его
      delay(SERVO_CALIBRATE_DELAY); // делаем задержку
      servoCalibrateDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], servoCalibPos); // выводим имя сервы и текущее положение
      state = LEAD;
      break;

    case SERVO_FIND_MAX:  // если найдено максимальное положение сервы
      EEPROM.put(EEPROM_ADDR_SERV_MAX[servoCounter], servoCalibPos);  // записываем его в епром по адресу из массива и сигналим
      beep(1, 200);
      EEPROM.get(EEPROM_ADDR_SERV_MIN[servoCounter], tempInfoPositionMin);  // после записи, считываем информацию из епрома и выводим на дисплей
      EEPROM.get(EEPROM_ADDR_SERV_MAX[servoCounter], tempInfoPositionMax);
      EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], tempInfoPositionStart);
      servoInfoDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], tempInfoPositionMin, tempInfoPositionMax, tempInfoPositionStart);
      state = LEAD;
      break;

    case SERVO_FIND_MIN:  // если найдено максимальное положение сервы
      EEPROM.put(EEPROM_ADDR_SERV_MIN[servoCounter], servoCalibPos);  // записываем его в епром по адресу из массива и сигналим
      beep(1, 200);
      EEPROM.get(EEPROM_ADDR_SERV_MIN[servoCounter], tempInfoPositionMin);  // после записи, считываем информацию из епрома и выводим на дисплей
      EEPROM.get(EEPROM_ADDR_SERV_MAX[servoCounter], tempInfoPositionMax);
      EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], tempInfoPositionStart);
      servoInfoDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], tempInfoPositionMin, tempInfoPositionMax, tempInfoPositionStart);
      state = LEAD;
      break;

    case SERVO_FIND_START:  // если найдено стартовое положение сервы
      EEPROM.put(EEPROM_ADDR_SERV_START[servoCounter], servoCalibPos);  // записываем его в епром по адресу из массива и сигналим
      beep(1, 200);
      EEPROM.get(EEPROM_ADDR_SERV_MIN[servoCounter], tempInfoPositionMin);  // после записи, считываем информацию из епрома и выводим на дисплей
      EEPROM.get(EEPROM_ADDR_SERV_MAX[servoCounter], tempInfoPositionMax);
      EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], tempInfoPositionStart);
      servoInfoDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], tempInfoPositionMin, tempInfoPositionMax, tempInfoPositionStart);
      state = LEAD;
      break;

    case EXIT:    // выход из режима калибровки
      readServoRange(); // чтение границ серв из епрома и запись в глобальные переменные
      beep(1, 500);
      /* servoCentering();   // центровка серв */
      servoCounter = 0;   // сбрасываем каретку
      servoCalibPos = SERVO_CENTRAL_POSITION;   // сбрасываем положение
      display.clearDisplay();   // чистим дисплей
      display.display();
      state = LEAD;
      return true;
  }
  return false;
}

bool workFSM(float *voltage)    // рабочий режим
{
  static bool manipulatorMode = false;  // опущен ли плуг
  static uint32_t lastBeepTime = 0; // время последнего пищания
  static uint32_t lastAnimationTime = 0; //время последней смены анимации
  static uint8_t lastAnimetion = 0; //предыдущие глаза

  static int8_t servoCounter = 0;  // каретка, переключающаяся от сервы к серве (нужен знаковый) - текущая выбранная серва из массива
  static uint16_t servoPos = SERVO_CENTRAL_POSITION;
  static uint16_t servoMaxPos = SERVO_CENTRAL_POSITION;
  static uint16_t servoMinPos = SERVO_CENTRAL_POSITION;
  static uint16_t servoStartPos = SERVO_CENTRAL_POSITION;

  valueLeft = ps2x.Analog(PSS_LY);
  valueRight = ps2x.Analog(PSS_RY);

  static enum
  {
    LEAD,  // управляющий режим
    TRAFFIC,
    NOTHING, // остановка + ничего не делать
    SERVO_NEXT,
    SERVO_PREV,
    SERVO_MOVE_UP,
    SERVO_MOVE_DOWN,
    CHANGE_MANIPULATOR_MODE,
    SHOW_VOLTAGE, //вывести на экран напряжение аккумуляторов
    EXIT  // переход к другому режиму
  } state;


  switch(state)
  {
    case LEAD:
      if ((valueRight != 127) || (valueLeft != 127)) { state = TRAFFIC; }
      if ((valueRight == 127) && (valueLeft == 127)) { state = NOTHING; }
      if (ps2x.ButtonPressed(PSB_R1)) { state = SERVO_NEXT; }
      if (ps2x.ButtonPressed(PSB_L1)) { state = SERVO_PREV; }
      if (ps2x.Button(PSB_PAD_UP)) { state = SERVO_MOVE_UP; }
      if (ps2x.Button(PSB_PAD_DOWN)) { state = SERVO_MOVE_DOWN; }
      if (ps2x.Button(PSB_CROSS)) { state = CHANGE_MANIPULATOR_MODE; }
      if (ps2x.Button(PSB_L3)) { state = SHOW_VOLTAGE; }
      if (ps2x.Button(PSB_L3) && ps2x.Button(PSB_R3)) { state = EXIT; }
      if (state == TRAFFIC) { motor_state = true; }
      else { motor_state = false; }
      break;

  case TRAFFIC:
    if(manipulatorMode) {
      state = LEAD;
      /* break; */
      return false;
    }

    if(((int8_t)valueLeft) >= 0) {
      speedLeft = (255 - (((int8_t)valueLeft)*2));
      setSpeedLeft(speedLeft);
    } else if(((int8_t)valueLeft) < 0) {
      speedLeft = (-(255 - (((int8_t)valueLeft)*2)));
      setSpeedLeft(-speedLeft);
    } else {
      speedLeft = 0;
      setSpeedLeft(0);
    }

    if(((int8_t)valueRight) >= 0) {
      speedRight = (255 - (((int8_t)valueRight)*2));
      setSpeedRight(speedRight);
    } else if(((int8_t)valueRight) < 0) {
      speedRight = (-(255 - (((int8_t)valueRight)*2)));
      setSpeedRight(-speedRight);
    } else {
      speedRight = 0;
      setSpeedRight(0);
    }

    standIdleTimer = millis();  // запомнить время последнего действия
    state = LEAD;
    break;

  case CHANGE_MANIPULATOR_MODE:
    if(manipulatorMode) {
      returnServoInStartPosition();
    };
    manipulatorMode = !manipulatorMode;
    Serial.println(manipulatorMode);
    state = LEAD;
    break;
    /* return false; */


  case NOTHING:
      stopMotors();
      static workDisplayState stateAnimation;
      static uint8_t countAnimetion = 0;
      static bool beepFlag = false;
      // робот переходит в режим бездействия, rand
      // нет доп проверок на действия, т.к. NOTHING идет в конечном автомате перед действиями и, если они происходят, то сюда не зайдет
      if(millis() - standIdleTimer > TIME_STAND_IDLE) // робот напоминает о режиме бездействия
      {
        if(millis() - lastAnimationTime > TIME_ANIMATION_LEFT_RIGHT && countAnimetion < 3 && stateAnimation != EYE_TIRED)
        {
          if(stateAnimation == EYE_RIGHT) stateAnimation = EYE_LEFT;
          else stateAnimation = EYE_RIGHT;
          //setDisplayState(stateAnimation);
          countAnimetion++;
          lastAnimationTime = millis();
        }
        if(millis() - lastAnimationTime > TIME_ANIMATION_LEFT_RIGHT && countAnimetion == 3 && stateAnimation != EYE_TIRED)
        {
          stateAnimation = EYE_TIRED;
          //setDisplayState(stateAnimation);
          lastAnimationTime = millis();
        }
        if(millis() - lastBeepTime > TIME_BEEP_CYCLE)
        {
          beep(3, 100);
          stateAnimation = EYE_RIGHT;
          //setDisplayState(stateAnimation);
          countAnimetion = 0;
          lastBeepTime =  millis();
          lastAnimationTime = millis();
        }
      }
      state = LEAD;
      break;


    case SERVO_NEXT:  // делаем кольцевой массив, можем ходить от серве к серве по замкнутому кругу
      servoCounter++;
      if (servoCounter >= (strlen((const char*)SERVO_ITERATED))) servoCounter = 0;
      EEPROM.get(EEPROM_ADDR_SERV_MIN[servoCounter], servoMinPos);  // после выбора сервы считываем о ней информацию из епрома и выводим на дисплей
      EEPROM.get(EEPROM_ADDR_SERV_MAX[servoCounter], servoMaxPos);
      servoPos = SERVO_POSITIONS_ITERATED[servoCounter];
      EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], servoStartPos);
      servoInfoDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], servoMinPos, servoMaxPos, servoStartPos);
      state = LEAD;
      break;

    case SERVO_PREV:  // делаем кольцевой массив, можем ходить от серве к серве по замкнутому кругу
      servoCounter--;
      if (servoCounter < 0) servoCounter = strlen((const char*)SERVO_ITERATED) - 1;
      EEPROM.get(EEPROM_ADDR_SERV_MIN[servoCounter], servoMinPos);  // после выбора сервы считываем о ней информацию из епрома и выводим на дисплей
      EEPROM.get(EEPROM_ADDR_SERV_MAX[servoCounter], servoMaxPos);
      servoPos = SERVO_POSITIONS_ITERATED[servoCounter];
      EEPROM.get(EEPROM_ADDR_SERV_START[servoCounter], servoStartPos);
      servoInfoDisplay((char*)SERVO_NAMES_ITERATED[servoCounter], servoMinPos, servoMaxPos, servoStartPos);
      state = LEAD;
      break;

    case SERVO_MOVE_UP:
      if(!manipulatorMode) {
        state = LEAD;
        /* break; */
        return false;
      }

      SERVO_POSITIONS_ITERATED[servoCounter] = SERVO_POSITIONS_ITERATED[servoCounter] - SERVO_STEP;
      if (SERVO_POSITIONS_ITERATED[servoCounter] > servoMaxPos) SERVO_POSITIONS_ITERATED[servoCounter] = servoMaxPos;
      if (SERVO_POSITIONS_ITERATED[servoCounter] < servoMinPos) SERVO_POSITIONS_ITERATED[servoCounter] = servoMinPos;
      pwm.setPWM(SERVO_ITERATED[servoCounter], 0, SERVO_POSITIONS_ITERATED[servoCounter]);
      //setDisplayState(EYE_WOW);
      standIdleTimer = millis();
      state = LEAD;
      break;

    case SERVO_MOVE_DOWN:
      if(!manipulatorMode) {
        state = LEAD;
        /* break; */
        return false;
      }

      SERVO_POSITIONS_ITERATED[servoCounter] = SERVO_POSITIONS_ITERATED[servoCounter] + SERVO_STEP;
      if (SERVO_POSITIONS_ITERATED[servoCounter] > servoMaxPos) SERVO_POSITIONS_ITERATED[servoCounter] = servoMaxPos;
      if (SERVO_POSITIONS_ITERATED[servoCounter] < servoMinPos) SERVO_POSITIONS_ITERATED[servoCounter] = servoMinPos;
      pwm.setPWM(SERVO_ITERATED[servoCounter], 0, SERVO_POSITIONS_ITERATED[servoCounter]);
      //setDisplayState(EYE_WOW);
      standIdleTimer = millis();
      state = LEAD;
      break;

    case EXIT:
      beep(1, 500);
      standIdleTimer = millis();
      state = LEAD;
      return true;

    case SHOW_VOLTAGE:
      //setDisplayState(BATTERY);
      uint8_t numbOfline = (*voltage - MIN_SUPPLY_VOLTAGE) / (MAX_SUPPLY_VOLTAGE - MIN_SUPPLY_VOLTAGE) * (X_2 - X_1);
      for(uint8_t i = 0; i < numbOfline; i++){
        display.drawFastVLine(X_1 + i, Y_1, Y_2 - Y_1, WHITE);
      }
      display.display();
      state = LEAD;
      break;
  }
  return false;
}



void setup()
{
  Serial.begin(9600);
  Serial.println("serial started");
  displaySetup(); // инициализация дисплея
  motorSetup();   // инициализация моторов
  servoSetup();   // инициализация серв
  servoCentering();   // центрирование серв
  buzzerSetup(); // инициализация динамика

  pinMode(A4, INPUT_PULLUP);    // подтяжка линий I2C к питанию, мб и не надо
  pinMode(A5, INPUT_PULLUP);

  //установка выводов и настроек: GamePad(clock, command, attention, data, Pressures?, Rumble?) проверка ошибок
  ps2x.config_gamepad(JOY_CLK_CH, JOY_CMD_CH, JOY_SEL_CH, JOY_DAT_CH, JOY_PRESSURES, JOY_RUMBLE);

  readServoRange();   // чтение границ серв из епрома и запись в глобальные переменные

  beep(1, 500);

  analogReference(INTERNAL);  // настройка опорного напряжения для АЦП: внутренний источник 1.1В

  standIdleTimer = millis(); // запомнить время последнего действия
}




void loop()
{
  static bool m_exit = false; // доп переменная для хранения данных о выходе из некоторых конечных автоматов
  static float voltage = MAX_SUPPLY_VOLTAGE;
  static float LEDTimer = 0;
  static bool LedState = 0;
  static uint32_t lastBeepTime = 0; // время последнего пищания об отсутствии заряда
  static uint32_t lastVoltageMeasurement = 0; //время прошлого измерения напряжения

  static enum
  {
    WORK,   // рабочий режим - ездит, кривляется
    CALIBRATION,  // режим калибровки
    TIRED,  // если робот устал - низкое напряжение
  } state;

  ps2x.read_gamepad(false, 0); // считывание данных с джойстика и установка скорости вибрации

  if(millis() - lastVoltageMeasurement > TIME_VOLTAGE_MEASUREMENT){
    adcDataCounter(&voltage); // обновляем донные с АЦП
    lastVoltageMeasurement = millis();
  }

  if((millis() - LEDTimer) > TIME_LED){
    if(LedState){
      digitalWrite(LED_CH, LOW);
      LedState = false;
    }
    else{
      digitalWrite(LED_CH, HIGH);
      LedState = true;
    }
    LEDTimer = millis();
  }

  if (voltage < MIN_SUPPLY_VOLTAGE) state = TIRED;   // если напряжение маленькое
  switch(state)
  {
    case WORK:
      m_exit = workFSM(&voltage);   // крутимся в рабочем режиме, пока не придет флаг о выходе из него - вернется true
      if(m_exit)  state = CALIBRATION;
      m_exit = false;
      break;

    case CALIBRATION:
      m_exit = calibrationFSM();  // крутимся в режиме калибровки, пока не придет флаг о выходе из него - вернется true
      if(m_exit)  state = WORK;
      m_exit = false;
      standIdleTimer = millis();  // запомнить время последнего действия
      break;

    case TIRED:
      Serial.println("tired");
      if (voltage > MIN_SUPPLY_VOLTAGE)
      {
        state = WORK;  // если напряжение нормальное
      }
      else
      {
        while(1){
          stopMotors();
          //setDisplayState(BATTERY);
          if((millis() - lastBeepTime) > TIME_BEEP_CYCLE) // если прошло достаточно времени
          {
            lastBeepTime = millis();
            beep(2, 500);
          }
        }
      }
      delay(10);
      break;
  }
  delay(15);
}
