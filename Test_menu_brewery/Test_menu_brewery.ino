#include <LiquidCrystal_I2C.h>
#include "GyverEncoder.h"

/*
// пример с линейным меню
#define LINES 2   // количество строк дисплея
#define SETTINGS_AMOUNT 13  // количество настроек
#define SETTINGS_SETTING 12  // количество настроек
#define SETTINGS_MENU 3
*/
const uint8_t LCD_LINES       = 2;  // количество строк, которые может отобразить дисплей
const uint8_t SETTINGS_AMOUNT = 6;  // количество настроек  без кнопки "Exit" (настройка в формате: 1я строка Time, 2я строка Temp)
const uint8_t MAIN_MENU       = 3;  // все строки главного меню

/*
// пины энкодера
#define CLK 2
#define DT 3
#define SW 4
*/
const uint8_t CLK = 2;
const uint8_t DT  = 3;
const uint8_t SW  = 4;

// Описание интерфейса:
// Главное меню   (Main)     - это то, что отображается при запуске устройства.
//     В нём 3 записи: Settings   - перебрасывает в настройки режимов работы
//                     Start/Stop - запускает/останавливает пывоварение
//                     Status     - перебрасывает в окно состояния процесса пывоварения (текущая температура и чё-то там ещё)
// 
// Меню настроек  (Settings) - тут устанавливется время (Time) и температура (Temp) каждого этапа пывоварения (количество этапов задаётся в коде константой SETTINGS_AMOUNT).
// Также должна быть кнопка выхода из этого меню (Exit)
//
// Меню состояния (Status)   - какая температура сейчас и чё-то там ещё
enum class Menu {
  Main,
  Settings,
  Status
};

struct Setting {
    uint16_t time_val = 0;
    uint16_t temp_val = 0;
    String time = "Time-";
    String temp = "Temp-";
};

struct SettingsWindow {
    Setting settings[SETTINGS_AMOUNT];
    String exit = "Exit";
};


LiquidCrystal_I2C lcd(0x27, 16, LCD_LINES); // адрес 0x27 или 0x3f
Encoder enc1(CLK, DT, SW);          // для работы c кнопкой

//int vals[SETTINGS_SETTING];  // массив параметров
int8_t arrowPos = 0;
int8_t screenPos = 0; // номер "экрана"

Menu menu;
SettingsWindow settingsWindow;

// названия параметров
//String settingsValue[]  = { //Перечень окна с натройками
//  "Time-1", //Время 
//  "Temp-1", //Температура
//  "Time-2",
//  "Temp-2",
//  "Time-3",
//  "Temp-3",
//  "Time-4",
//  "Temp-4",
//  "Time-5",
//  "Temp-5",
//  "Time-6",
//  "Temp-6",
//  "Exit",
//};

String mainMenu[]  = { // Главное меню
  "Setting",
  "Window",
  "Stop",
};

String startStop[]  = { //Меню запуска програмы
  "Stop"
  "Start",
};

void setup() {
  Serial.begin(9600); //Волшебная цифра 
  enc1.setType(TYPE2); //Тип энкодера

  lcd.init();
  lcd.backlight();

  menu = Menu::Main;

  for(size_t i = 0; i < SETTINGS_AMOUNT; i++) {
      settingsWindow.settings[i].time.concat(i);
      settingsWindow.settings[i].temp.concat(i);
  }

  printMainMenu();
}

void loop() {
  enc1.tick();

  switch (menu) {
    case Menu::Main:
      if (enc1.isTurn()) {
        int increment = 0;  // локальная переменная направления
          // получаем направление   
        if (enc1.isRight()) increment = 1;
        if (enc1.isLeft()) increment = -1;
        arrowPos += increment;  // двигаем курсор  
        arrowPos = constrain(arrowPos, 0, MAIN_MENU - 1); // ограничиваем

        increment = 0;  // обнуляем инкремент

        printMainMenu();
      }
      if (enc1.isClick()){
        menu = Menu::Settings;
        printSettingsValue();
      }
      break;
    
    case Menu::Settings:
      if (enc1.isTurn()) {
        int increment = 0;  // локальная переменная направления
    
        // получаем направление   
        if (enc1.isRight()) increment = 1;
        if (enc1.isLeft()) increment = -1;
        arrowPos += increment;  // двигаем курсор  
        arrowPos = constrain(arrowPos, 0, SETTINGS_AMOUNT*2 + 1); // ограничиваем

        increment = 0;  // обнуляем инкремент
        if (arrowPos < SETTINGS_AMOUNT) {
          if (enc1.isRightH()) increment = 1;
          if (enc1.isLeftH()) increment = -1;
          if(arrowPos % 2 == 0) {
            settingsWindow.settings[arrowPos / LCD_LINES].time_val += increment;
          }
          else {
            settingsWindow.settings[arrowPos / LCD_LINES].temp_val += increment;
          }
          //vals[arrowPos] += increment;  // меняем параметры
        }

        printSettingsValue();
      }
      if (enc1.isClick() && arrowPos == SETTINGS_AMOUNT*2){
        menu = Menu::Main;
        printMainMenu();   
      }
      break;
    case Menu::Status:
      //TODO
      break;
    }
}

//_____________________
void printMainWindow(){
  //TODO

}

//_____________________
void printSettingsValue() {  //Функция для вывода на экран меню настроек
  lcd.clear();  
  screenPos = arrowPos / LCD_LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

  for (byte i = 0; i < LCD_LINES; i++) {  // для всех строк
    lcd.setCursor(0, i);              // курсор в начало

    // если курсор находится на выбранной строке
    if (arrowPos == LCD_LINES * screenPos + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел

    // если пункты меню закончились, покидаем цикл for
    if ( (LCD_LINES * screenPos + i) == (SETTINGS_AMOUNT*2 + 1) ) break;

    // выводим имя и значение пункта меню
    String out;
    if (arrowPos < SETTINGS_AMOUNT) {
        if (i%2 == 0) {
            out = settingsWindow.settings[screenPos].time + ": " + settingsWindow.settings[screenPos].time_val;
        }
        else {
            out = settingsWindow.settings[screenPos].temp + ": " + settingsWindow.settings[screenPos].temp_val;
        }
    }

    else {
        out = settingsWindow.exit;
    }

    lcd.print(out);
  }
}


//_____________________
void printMainMenu(){
  lcd.clear();  
  screenPos = arrowPos / LCD_LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

  for (byte i = 0; i < LCD_LINES; i++) {  // для всех строк
    lcd.setCursor(0, i);              // курсор в начало

    // если курсор находится на выбранной строке
    if (arrowPos == LCD_LINES * screenPos + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел

    // если пункты меню закончились, покидаем цикл for
    if (LCD_LINES * screenPos + i == MAIN_MENU) break;

    // выводим имя и значение пункта меню
    lcd.print(mainMenu[LCD_LINES * screenPos + i]);
  }

}


//_____________________
void printStartStopSettings() {
  //TODO
}

/*
void printGUI() {
  lcd.clear();  
  screenPos = arrowPos / LCD_LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

  for (byte i = 0; i < LCD_LINES; i++) {  // для всех строк
    lcd.setCursor(0, i);              // курсор в начало

    // если курсор находится на выбранной строке
    if (arrowPos == LCD_LINES * screenPos + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел

    // если пункты меню закончились, покидаем цикл for
    if (LCD_LINES * screenPos + i == SETTINGS_AMOUNT*2+1) break;

    // выводим имя и значение пункта меню
    lcd.print(settingsValue[LCD_LINES * screenPos + i]);
    lcd.print(": ");
    lcd.print(vals[LCD_LINES * screenPos + i]);
  }
}
*/
