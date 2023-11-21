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

const uint8_t TIME_MAX = 8;  // в попугаях

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
  "Settings",
  "Status",
  "Process: ",
};

// Если 0, то процесс пывоварения не запущен
// и в главном меню отображается "Stop"
// Если 1, то наоборот: процесс запущен, отображается "Start"
bool start_process;
//Меню запуска програмы
String startStop[]  = {
  "Stop",
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

  start_process = false;
  printMainMenu();

  Serial.println("Start");
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
        Serial.println(arrowPos);
      }
      if (enc1.isClick()) {
        switch (arrowPos) {
          case 0:
            Serial.println("to settings");
            menu = Menu::Settings;
            printSettingsMenu();
            break;
          
          case 1:
            Serial.println("to status");
            menu = Menu::Status;
            printStatusMenu();
            break;
          
          case 2:
            start_process != start_process;
            Serial.println("switch to " + startStop[start_process]);
            printMainMenu();
            break;
          
          default:
            Serial.print("WTF?? arrowPos too big. This menu item does not exist!");
            break;
        }
      }
      break;
    
    case Menu::Settings:
      if (enc1.isTurn()) {
        int increment = 0;  // локальная переменная направления
    
        // получаем направление   
        if (enc1.isRight()) increment = 1;
        if (enc1.isLeft()) increment = -1;
        arrowPos += increment;  // двигаем курсор  
        arrowPos = constrain(arrowPos, 0, SETTINGS_AMOUNT*2); // ограничиваем

        increment = 0;  // обнуляем инкремент
        if (arrowPos < SETTINGS_AMOUNT*2) {
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

        printSettingsMenu();
        Serial.println(arrowPos);
      }

      if (enc1.isClick()) {
        // Exit
        if (arrowPos == SETTINGS_AMOUNT*2) {
          lcd.clear();
          arrowPos = 0;
          printMainMenu();
          Serial.println("MainWindow");
          menu = Menu::Main;
        }
        else {
          changeSetting(arrowPos);
        }
      }
      break;

    case Menu::Status:
      //TODO
      break;
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
    else {
      String out_str = mainMenu[LCD_LINES * screenPos + i];

        // если курсор на строке "Process" выводим, запущен ли процесс (Start или Stop)
        if (LCD_LINES * screenPos + i == MAIN_MENU - 1) {
          out_str.concat(startStop[start_process]);
        }

      lcd.print(out_str);
    }
  }

}


//_____________________
void printSettingsMenu() {  //Функция для вывода на экран меню настроек
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
    if (arrowPos < SETTINGS_AMOUNT*2) {
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
void printStatusMenu() {
  //TODO
}


//_____________________
void changeSetting(uint8_t setting) {
  while (!enc1.isClick()) {
    int increment = 0;
    int value = 0;

    if (enc1.isTurn()) {
      Serial.println("turning");
      // получаем направление   
      if (enc1.isRight()) increment = 1;
      if (enc1.isLeft()) increment = -1;

      switch (setting) {
        // Time-0
        case 0:
          value = settingsWindow.settings[setting % 2].time_val;
          value += increment;
          value = constrain(value, 0, TIME_MAX - 1); // ограничиваем
          settingsWindow.settings[setting % 2].time_val = value;
          Serial.println(value);
          break;
        // Temp-0
        case 1:
          break;

        // Time-1
        case 2:
          value = settingsWindow.settings[setting % 2].time_val;
          value += increment;
          value = constrain(value, 0, TIME_MAX - 1); // ограничиваем
          settingsWindow.settings[setting % 2].time_val = value;
          break;
        // Temp-1
        case 3:
          break;
        
        // Time-2
        case 4:
          value = settingsWindow.settings[setting % 2].time_val;
          value += increment;
          value = constrain(value, 0, TIME_MAX - 1); // ограничиваем
          settingsWindow.settings[setting % 2].time_val = value;
          break;
        // Temp-2
        case 5:
          break;
        
        // Time-3
        case 6:
          value = settingsWindow.settings[setting % 2].time_val;
          value += increment;
          value = constrain(value, 0, TIME_MAX - 1); // ограничиваем
          settingsWindow.settings[setting % 2].time_val = value;
          break;
        // Temp-3
        case 7:
          break;
        
        // Time-4
        case 8:
          value = settingsWindow.settings[setting % 2].time_val;
          value += increment;
          value = constrain(value, 0, TIME_MAX - 1); // ограничиваем
          settingsWindow.settings[setting % 2].time_val = value;
          break;
        // Temp-4
        case 9:
          break;
        
        // Time-5
        case 10:
          value = settingsWindow.settings[setting % 2].time_val;
          value += increment;
          value = constrain(value, 0, TIME_MAX - 1); // ограничиваем
          settingsWindow.settings[setting % 2].time_val = value;
          break;
        // Temp-5
        case 11:
          break;
      }
      printSettingsMenu();
    }
  }
}
