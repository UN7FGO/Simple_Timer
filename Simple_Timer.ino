// http://blog.gengen.ru/

#include <LiquidCrystal.h>
#include <LCDKeypad.h>

LCDKeypad lcd;

int controlPin=12;          // Номер контакта для управляющего сигнала на реле 

int seconds[4];             // Массив для установки времени поразрядно
int n=3;                    // Позиционируем курсор на 4-й разряд
unsigned long timer;
int buttonPressed;
byte c_ya[8] = {
  B01111,
  B10001,
  B10001,
  B10001,
  B01111,
  B01001,
  B10001,
  B00000,
};
byte c_u[8] = {
  B10001,
  B10001,
  B10001,
  B10001,
  B01111,
  B00001,
  B01110,
  B00000,
};
byte c_p[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B00000,
};
byte c_ch[8] = {
  B10001,
  B10001,
  B10001,
  B10001,
  B01111,
  B00001,
  B00001,
  B00000,
};

void setup() {                                      // Создаем "буквы", инициализируем экран, пишем слово "ВРЕМЯ"
  lcd.createChar(1,c_ya);
  lcd.createChar(2,c_u);
  lcd.createChar(3,c_p);
  lcd.createChar(4,c_ch);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BPEM");
  lcd.write(1);
  pinMode(controlPin, OUTPUT);
  digitalWrite(controlPin, LOW);
}

void loop() {
  PrintWSetting();                                  // Пишем слово "УСТАНОВКА"
  PrintTime();                                      // Отображение чисел таймера (в сек.)
  lcd.blink();
  while((buttonPressed=lcd.button())==KEYPAD_NONE)  //Ждем нажатия кнопок
  {
  }
  lcd.noBlink();
  waitReleaseButton();                              // Ждем отпускания кнопки

  switch (buttonPressed) {
    case KEYPAD_RIGHT:                                         // Нажата кнопка "R"
      if (n<3) n=n+1;
      break;
    case KEYPAD_LEFT:                                         // Нажата кнопка "L"
      if (n>0) n=n-1;
      break;
    case KEYPAD_UP:                                         // Нажата кнопка "UP" - увеличиваем только цифру под курсором, с переходом с 9 на 0
      if (seconds[n]<9) {
        seconds[n]=seconds[n]+1;
      }
      else if (seconds[n]>8) {
        seconds[n]=0;
      }
      break;
    case KEYPAD_DOWN:                                         // Нажата кнопка "DOWN" - уменьшаем только цифру под курсором, с переходом с 0 на 9
      if (seconds[n]>0) {
        seconds[n]=seconds[n]-1;
      }
      else if (seconds[n]<1) {
        seconds[n]=9;
      }
      break;
    case KEYPAD_SELECT:                                         // Нажата кнопка "Start/Pause"
        unsigned long startTime = millis();
        unsigned long stopTime = millis()+timer*1000;
        PrintWCountdown();                          // Пишем слово "ОТСЧЕТ"
        digitalWrite(controlPin, HIGH);             // Замыкаем реле
        while(millis()<stopTime) {                  // Запускаем таймер
          int t = int((stopTime-millis())/1000)+1;
          if (lcd.button()==KEYPAD_SELECT) {                    // Перехват нажатия кнопки "Start/Pause"
            unsigned long startPause = millis();    // Запоминаем время нажатия на паузу
            PrintWPause();                          // Пишем слово "ПАУЗА"
            digitalWrite(controlPin, LOW);          // Размыкаем реле
            while(lcd.button()!=KEYPAD_NONE)        // Ждем отпускания кнопки
            {
            }
            lcd.setCursor(0,1);
            while(lcd.button()!=KEYPAD_SELECT)      // Ждем повторного нажатия кнопки "Start/Pause" (здесь надо помигать словом "ПАУЗА")
            {
            }
            waitReleaseButton();
            unsigned long stopPause = millis();
            stopTime=stopTime+stopPause-startPause; // Увеличиваем время остановки таймера на время паузы
            PrintWCountdown();                      // Пишем слово "ОТСЧЕТ"
            digitalWrite(controlPin, HIGH);         // Замыкаем реле
          }

          lcd.setCursor(12,0);
          if (int(t) <= 9) {
            lcd.print("   ");                       // Установка для 0-9
          }
          else if (t <= 99) {
            lcd.print("  ");                        // Установка для 10-99
          }
          else if (t <= 999) {
            lcd.print(" ");                         // Установка для 100-999
          }
          lcd.print(t);
        } 
        digitalWrite(controlPin, LOW);              // Размыкаем реле
      break;
  }

}


//                    //
// Подпрограммы здесь //
//                    //

void PrintWSetting() {
  lcd.setCursor(0,1);
  lcd.write(2);
  lcd.print("CTAHOBKA ");
}

void PrintWCountdown() {
  lcd.setCursor(0,1);
  lcd.print("OTC");
  lcd.write(4);
  lcd.print("ET    ");
}

void PrintTime() {
  lcd.setCursor(12,0);
  for (int i=0; i<4; i++){
    lcd.print(seconds[i]);
  }
  lcd.setCursor(n+12,0);
}

void PrintWPause() {
  lcd.setCursor(0,1);
  lcd.write(3);
  lcd.print("A");
  lcd.write(2);
  lcd.print("3A     ");
}

void waitReleaseButton() {
  if (lcd.button()==KEYPAD_SELECT){
    lcd.setCursor(0,1);
    lcd.print("          ");
    timer=seconds[0]*1000+seconds[1]*100+seconds[2]*10+seconds[3];
  }
  delay(50);
  while(lcd.button()!=KEYPAD_NONE)
  {
  }
}

