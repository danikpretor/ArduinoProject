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
bool StrStp = 0;
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

  menu = Menu::MainMenu; //Заходим сразу в главное меню
  printMainMenu(); // Отображаем на экране главное меню
  lcd.clear(); //предпусковое очищение экрана
  
  Serial.println("Start"); 
  //Ещё раз чистим экран и выводим привью
  lcd.clear(); 
  lcd.setCursor(0, 0); lcd.print("Brewery V 0.1");
  lcd.setCursor(0, 1); lcd.print("Turn the handle");
  
}

void loop() {
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
      if (enc1.isClick()) { //По клику на энкодер проваливаемся в соответсвующие меню
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
        
        vals[arrowPos] += increment;  // меняем параметры
      }
      
      printSettingsValue();
      }
      if (enc1.isClick() && arrowPos == SETTINGS_AMOUNT-1){ //По нажатию на Exet мы выходим из соответсвующего меню
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
}

//_____________________
void printMainWindow(){
  
  lcd.setCursor(0, 0); lcd.print("t:"); lcd.print("40"); lcd.print("C");
  lcd.setCursor(8, 0); lcd.print("T:"); lcd.print("00"); lcd.print(":");lcd.print("00");
  lcd.setCursor(0, 1); lcd.print("t:"); lcd.print("60"); lcd.print("C");
  lcd.setCursor(8, 1); lcd.print("T:"); lcd.print("60"); 

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
