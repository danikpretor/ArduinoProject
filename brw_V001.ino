// - ПАРАМЕТРЫ МЕНЮ НАЧАЛО
//#include <Vector.h>

#define LINES 2   // количество строк дисплея
#define SETTINGS_AMOUNT 13  // количество настроек
#define SETTINGS_SETTING 12  // количество настроек
#define SETTINGS_MENU 3 // количество настроек

#define FAST_STEP 5  // количество настроек
bool controlState = 0;  // клик

// пины энкодера
#define CLK 2
#define DT 3
#define SW 4

enum class Menu {  MainMenu,  MainWindow,  SettingsValue,  StartStopSettings }; //Создаём классы

#include "GyverEncoder.h"
Encoder enc1(CLK, DT, SW);  // для работы c кнопкой

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // адрес 0x27 или 0x3f

int vals[SETTINGS_SETTING];  // массив параметров
int8_t arrowPos = 0;
int8_t entVal = 0;
int8_t screenPos = 0; // номер "экрана"
int8_t dataEntry = 0; // переменная для хранения данных в диапахоне от 0 до 60 и от 0 до 100

bool chekTE1 = 0;

Menu menu;
// названия параметров
String settingsValue[]  = { //Перечень окна с натройками
  "Time-1", //Время 
  "Temp-1", //Температура
  "Time-2",
  "Temp-2",
  "Time-3",
  "Temp-3",
  "Time-4",
  "Temp-4",
  "Time-5",
  "Temp-5",
  "Time-6",
  "Temp-6",
  "Exit",
};

bool StrStp = 0; // Состояние переменной инициализирует работу программы

String settingsMainMenu[]  = { // Главное окно меню
  "Setting",
  "Window",
  "Stop",
};

String settingsStart[]  = { //Меню запуска програмы
  "Stop"
  "Start",
};

// - ПАРАМЕТРЫ ТАЙМЕРА НАЧАЛО
#include "GyverTimer.h" 

GTimer TimerBRW_1(MS);
GTimer TimerBRW_2(MS);
GTimer TimerBRW_3(MS);

uint32_t period = 1000;

bool fBrwStart = 0, fTimerStart = 1;
int k = 0, i = 0, ai = 0, ti = 0, fulTim = 0, summ_arr = 0;
uint32_t timer_brw1, timer_brw2, timer_brw3;
//Переменные для глобального таймера
uint32_t totalMills; //Милесек
int timeHours; // часы
int timeMins = 0;  // минуты
int timeSecs;  // секунды
//Переменные для цикла таймера
uint32_t totalCyclMills = 0; //Милесек
uint32_t totalCyclsec = 0; // секунды

int timeCyclHours; // часы
int timeCyclMins = 0;  // минуты
int timeCyclSecs = 0;  // секунды

// - ПАРАМЕТРЫ ПИД НАЧАЛО
#include <GyverPID.h>
//
//16:52:14.353 -> result: PI p: 7.72	PI i: 0.41	PID p: 21.56	PID i: 0.25	PID d: 64.99
GyverPID pid(7.72, 0.41, 0);
int PIDperiod = 500;

#define RELE_5 5
// - ПАРАМЕТРЫ ПИД КОНЕЦ

// - ПАРАМЕТРЫ датчика температуры
#include <microDS18B20.h>
MicroDS18B20<6> sensTE;

int temp1, TE1; // переменная для хранения температуры
int tempValue; // переменная для хранения температуры 


void setup() {
  Serial.begin(9600); //Волшебная цифра 
  enc1.setType(TYPE2); //Тип энкодера

  lcd.init();
  lcd.backlight();

  menu = Menu::MainMenu;
  
  Serial.println("Start");
  
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Brewery V 0.2");
  lcd.setCursor(0, 1); lcd.print("Turn the handle");

  TimerBRW_2.setInterval(100000); //Таймер для вывода сообщения в консоль  
  
  sensTE.requestTemp();     // запрос температуры
  delay(250);

  pinMode(RELE_5, OUTPUT);

  pid.setDirection(NORMAL); // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL - нагрев
  pid.setLimits(0, 255);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  delay(250);
}

void loop() {
  
  //f_timer();

  TE1 = f_TE1();      //переменная для хранения температуры

  enc1.tick();
  
  switch (menu) {

    case Menu::MainMenu: //Главное Меню
      if (enc1.isTurn()) {
        int increment = 0;  // локальная переменная направления
          // получаем направление   
        if (enc1.isRight()) increment = 1;
        if (enc1.isLeft()) increment = -1;
        arrowPos += increment;  // двигаем курсор  
        arrowPos = constrain(arrowPos, 0, SETTINGS_MENU - 1); // ограничиваем

        increment = 0;  // обнуляем инкремент

        printMainMenu(); //Выводим на экран соответсвующие меню
        Serial.println(arrowPos);
        
      }
      if (enc1.isClick()) {
        if (0 == arrowPos) {
          lcd.clear();
          printSettingsValue();
          menu = Menu::SettingsValue;
        }
        if (1 == arrowPos) {
          lcd.clear();
          printMainWindow();
          menu = Menu::MainWindow;
        }
        if (2 == arrowPos) {
          menu = Menu::StartStopSettings;
          lcd.clear();
          printMainMenu();

        }
      }

      break;

    case Menu::SettingsValue: //Меню настроек
      if (enc1.isTurn()) {
      int increment = 0;  // локальная переменная направления
    
      // получаем направление   
      if (enc1.isRight()) increment = 1;
      if (enc1.isLeft()) increment = -1;
      arrowPos += increment;  // двигаем курсор  
      arrowPos = constrain(arrowPos, 0, SETTINGS_AMOUNT - 1); // ограничиваем

      increment = 0;  // обнуляем инкремент
        
      if (arrowPos < SETTINGS_SETTING) {
        
        if (enc1.isRightH()) increment = 1;
        if (enc1.isLeftH()) increment = -1;

        vals[arrowPos] += increment;
        if (arrowPos % 2 == 0) {
          dataEntry = constrain(vals[arrowPos], 0, 60); // Ограничени для времени
          vals[arrowPos] = dataEntry;
        }
        else {
          dataEntry = constrain(vals[arrowPos], 0, 100); // Ограничени для температуры
          vals[arrowPos] = dataEntry;
        }

        // vals[arrowPos] += increment;  // меняем параметры
      }
      
      printSettingsValue();
      }
      if (enc1.isClick() && arrowPos == SETTINGS_AMOUNT-1){ //По нажатию на Exet мы выходим из соответсвующего меню
        arrowPos = 0;
        lcd.clear();
        printMainMenu(); 
        Serial.println("SettingsValue");
        menu = Menu::MainMenu;
        }
      break;

    case Menu::MainWindow:
      printMainWindow();
      if (enc1.isClick()){ //По нажатию проваливаемся в соответсвующее меню
        lcd.clear();
        Serial.println("MainWindow");
        printMainMenu();  
        menu = Menu::MainMenu;
      }     
      break;   

    case Menu::StartStopSettings:
      if (0 == StrStp) {
        StrStp = 1;
        settingsMainMenu[2] = "Start";
        Serial.print("StrStp ");
        Serial.println(StrStp);
      }
      else {
        StrStp = 0;
        settingsMainMenu[2] = "Stop";
        Serial.print("StrStp ");
        Serial.println(StrStp);
      }
        lcd.clear();
        printMainMenu();
        menu = Menu::MainMenu;
        break;
  }

  if (StrStp) {
  //---------------------------------------
    timeMins = vals[i]; //Время уставки 
    tempValue = vals[i+1];

    if (TE1 >= tempValue - 1) {
      chekTE1 = 1;
      //__________________
      if (fTimerStart) {
        
        for (int k = 0; k < SETTINGS_SETTING; k++) {
        Serial.print(vals[k]); Serial.print("; "); 
        summ_arr += vals[k]; 
        }
        Serial.println(" ");
        
        TimerBRW_1.setTimeout(vals[i]*1000ul*60ul);

        totalCyclMills = vals[i]*1000ul*60ul;
        totalCyclsec = totalCyclMills / 1000ul;


        Serial.print("Тайсер "); Serial.println(fTimerStart);

        Serial.print("Цикл на "); Serial.print(vals[i]); Serial.println(" мин ");
        Serial.print("vals    "); Serial.print(vals[i+1]); Serial.print(" С "); Serial.print("  TE1= "); Serial.println(TE1);
        Serial.print("i= "); Serial.println(i);
        
        fTimerStart = 0;
      }
      if (TimerBRW_1.isReady()) {
        Serial.print("Конец цикла на "); Serial.print(vals[i]); Serial.println(" Мин ");
        Serial.print("               "); Serial.print(vals[i+1]); Serial.println(" С ");
        fTimerStart = 1;
        i = i + 2;
      }
      if (i >= SETTINGS_SETTING) {
        //fBrwStart = 0;
        i = 0;
        StrStp = 0;
        fTimerStart = 0;
        settingsMainMenu[2] = "Stop";
      }
      //__________________
    }
    else {
      chekTE1 = 0;
    }

    if (TE1 <= tempValue) {
      pid.setpoint = tempValue;
      //fBrwStart = 1; 
      //fTimerStart = 1;
      f_pid();
    }
    else {
      pid.setpoint = 0;
    }

//---------------------------------------
  }
    else {
    //TODO
  }
}

//_____________________
void printMainWindow(){ //функция для вывода на экран меню текущих значений
  
  lcd.setCursor(0, 0); lcd.print("t:"); lcd.print(TE1); lcd.print("C"); //Температура текущая 
  lcd.setCursor(8, 0); lcd.print("T:"); lcd.print(timeCyclMins); lcd.print(":");lcd.print(timeCyclSecs); // Время общее. Глобальный таймер
  lcd.setCursor(0, 1); lcd.print("t:"); lcd.print(tempValue); lcd.print("C"); //Температура цыкла 
  lcd.setCursor(8, 1); lcd.print("T:"); lcd.print(timeMins); lcd.print(" min"); //Время цыкла

}

//_____________________
void printSettingsValue() {  //Функция для вывода на экран меню настроек
  lcd.clear();  
  screenPos = arrowPos / LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

  for (byte i = 0; i < LINES; i++) {  // для всех строк
    lcd.setCursor(0, i);              // курсор в начало

    // если курсор находится на выбранной строке
    if (arrowPos == LINES * screenPos + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел

    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPos + i == SETTINGS_AMOUNT) break;

    // выводим имя и значение пункта меню
    lcd.print(settingsValue[LINES * screenPos + i]);
    if (arrowPos < SETTINGS_SETTING) {
      lcd.print(": ");
      lcd.print(vals[LINES * screenPos + i]);
    }

  }
}

//_____________________
void printMainMenu(){ //Функция для вывода на экран главное меню
  lcd.clear();  
  screenPos = arrowPos / LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

  for (byte i = 0; i < LINES; i++) {  // для всех строк
    lcd.setCursor(0, i);              // курсор в начало
    // если курсор находится на выбранной строке
    if (arrowPos == LINES * screenPos + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел
    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPos + i == SETTINGS_MENU) break;
    // выводим имя и значение пункта меню
    lcd.print(settingsMainMenu[LINES * screenPos + i]);
  }
}

int f_TE1() { //Функия чтения температуры
  static uint32_t tmr2;
    if (millis() - tmr2 >= PIDperiod) {
    tmr2 = millis();    
    
    sensTE.readTemp();
    temp1 = sensTE.getTempInt(); 
    sensTE.requestTemp();

    return temp1;
  }
}
//_____________________
void f_pid(){ //Функция ПИД
  
  static uint32_t tmr3;
  if (millis() - tmr3 >= PIDperiod) {
    tmr3 = millis();
    pid.input = TE1;   // сообщаем регулятору текущую температуру
    pid.getResult();
    analogWrite(RELE_5, pid.output);    
  }

}

void f_tempValue(){

}

void f_timer(){
    
    if (chekTE1 == 1 && fTimerStart == 1) {
      static uint32_t tmr1;
      if (millis() - tmr1 >= 1000) {
      tmr1 = millis();
      totalCyclsec = totalCyclsec - 1;
      timeCyclHours = (totalCyclsec / 3600ul);        // часы
      timeCyclMins = (totalCyclsec % 3600ul) / 60ul;  // минуты
      timeCyclSecs = (totalCyclsec % 3600ul) % 60ul;  // секунды
  
    }
  }
  //Serial.print("sec "); Serial.println(sec);
}

