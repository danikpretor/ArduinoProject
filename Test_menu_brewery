#include <Vector.h>

// пример с линейным меню
#define LINES 2   // количество строк дисплея
#define SETTINGS_AMOUNT 13  // количество настроек
#define SETTINGS_SETTING 12  // количество настроек
#define SETTINGS_MENU 3

// пины энкодера
#define CLK 2
#define DT 3
#define SW 4

enum class Menu {
  MainMenu,
  MainWindow,
  SettingsValue,
  StartStopSettings
  
};

#include "GyverEncoder.h"
Encoder enc1(CLK, DT, SW);  // для работы c кнопкой

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // адрес 0x27 или 0x3f


int vals[SETTINGS_SETTING];  // массив параметров
int8_t arrowPos = 0;
int8_t screenPos = 0; // номер "экрана"

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

String settingsMainMenu[]  = { // Главное окно меню
  "Setting",
  "Window",
  "Stop",
};

String settingsStart[]  = { //Меню запуска програмы
  "Stop"
  "Start",
};

void setup() {
  Serial.begin(9600); //Волшебная цифра 
  enc1.setType(TYPE2); //Тип энкодера

  lcd.init();
  lcd.backlight();

  menu = Menu::MainMenu;

  printMainMenu();
}

void loop() {
  enc1.tick();

  switch (menu) {
    case Menu::MainMenu:
      if (enc1.isTurn()) {
        int increment = 0;  // локальная переменная направления
          // получаем направление   
        if (enc1.isRight()) increment = 1;
        if (enc1.isLeft()) increment = -1;
        arrowPos += increment;  // двигаем курсор  
        arrowPos = constrain(arrowPos, 0, SETTINGS_MENU - 1); // ограничиваем

        increment = 0;  // обнуляем инкремент

        printMainMenu();
      }
      if (enc1.isClick()){
        menu = Menu::SettingsValue;
        printSettingsValue();
      }
      break;
    
    case Menu::SettingsValue:
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
        vals[arrowPos] += increment;  // меняем параметры
      }
      printSettingsValue();
      }
      if (enc1.isClick() && arrowPos == SETTINGS_AMOUNT-1){
        menu = Menu::MainMenu;
        printMainMenu();   
        }
      break;
    case Menu::StartStopSettings:
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
void printMainMenu(){
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


//_____________________
void printStartStopSettings() {
  //TODO
}

void printGUI() {
  lcd.clear();  
  screenPos = arrowPos / LINES;   // ищем номер экрана (0..3 - 0, 4..7 - 1)

  for (byte i = 0; i < LINES; i++) {  // для всех строк
    lcd.setCursor(0, i);              // курсор в начало

    // если курсор находится на выбранной строке
    if (arrowPos == LINES * screenPos + i) lcd.write(126);  // рисуем стрелку
    else lcd.write(32);     // рисуем пробел

    // если пункты меню закончились, покидаем цикл for
    if (LINES * screenPos + i == SETTINGS_SETTING) break;

    // выводим имя и значение пункта меню
    lcd.print(settingsValue[LINES * screenPos + i]);
    lcd.print(": ");
    lcd.print(vals[LINES * screenPos + i]);
  }


}
