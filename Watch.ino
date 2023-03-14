/*
  Arduino Watch

  By Jia Rong Hu, Apr 14 2020

  Parts required:
  - one SSD1306 i2c OLED
  - one DS3231 Breakout Board
  - three Pushbuttons
  - three 1 kilohm resistors
  - sixteen male to male wires
  - arduino stuff (arduino, breadboard, others)
*/

//OLED Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//RTC Libraries
#include <DS3231.h>
#define optokapu 7
//Defining the buttons
const int midButton = 8;
const int upButton  = 9;
const int dwnButton = 10;

//States for the buttons
int midbuttonState;
int upbuttonState;
int dwnbuttonState;

//Last states for the buttons
int midlastButtonState = 0;
int uplastButtonState  = 0;
int dwnlastButtonState = 0;

//Still the last states for the buttons
unsigned long midlastDebounceTime = 0;
unsigned long uplastDebounceTime  = 0;
unsigned long dwnlastDebounceTime = 0;

//Used to slow down the refresh
unsigned long previousrefresh = 0;

//Menu values for the menu
int menu = 0;
int menu1 = 0;
int menu2 = 0;
int menu3 = 0;

//Interval Value
const long interval = 1000;

//Stopwatch Values
int shr = 0;
int smin = 0;
int ssec = 0;

int startstop = 0;

unsigned long StoppreviousMillis = 0;

//Timer Values
int timerdone = 0;

int timermin = 0;
int timersec = 0;

int timermode = 0;
int timerstartstop = 0;

unsigned long TimpreviousMillis = 0;

//Set Time Values
int settimemode = 0;
int enterhour = 0;
int entermin = 0;
int entersec = 0;

bool h12Flag;
bool pmFlag;

//OLED Dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//RTC Pins
DS3231 Clock;

//Time Function
//Time t;

void setup() {
  //Debugging?
  Serial.begin(115200);

  //Defining the Buttons as INPUTs
  pinMode(midButton, INPUT);
  pinMode(upButton, INPUT);
  pinMode(dwnButton, INPUT);
  pinMode(optokapu, OUTPUT);
  digitalWrite(optokapu, LOW);

  //OLED Begin
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  //RTC Begin
  Wire.begin();
}

void loop() {

  //DateTime Clock = DS3231();
  //Button Debounce Stuff
  int reading = digitalRead(midButton);
  int reading2 = digitalRead(upButton);
  int reading3 = digitalRead(dwnButton);

  if (reading != midlastButtonState) {
    midlastDebounceTime = millis();
  }

  if (reading2 != uplastButtonState) {
    uplastDebounceTime = millis();
  }

  if (reading3 != dwnlastButtonState) {
    dwnlastDebounceTime = millis();
  }

  /////////////////Middle Button/////////////////

  if ((millis() - midlastDebounceTime) > 50) {
    if (reading != midbuttonState) {
      midbuttonState = reading;

      if (midbuttonState == HIGH) {
        if (timerdone == 1) {
          display.invertDisplay(false);
          timerdone = 0;
        } else if (menu == 0) {
          menu = menu + 1;
        } else if (menu == 1) {
          menu1 = !menu1;
        } else if (menu == 2) {
          menu2 = !menu2;
        } else if (menu == 3 && menu3 == 0) {
          menu3 = !menu3;
          enterhour = enterhour;
          entermin  = entermin;
          entersec  = entersec;
        } else if (menu == 3 && menu3 == 1) {
          settimemode = settimemode + 1;
        }
      }
    }
  }

  /////////////////Top Button/////////////////

  if ((millis() - uplastDebounceTime) > 50) {
    if (reading2 != upbuttonState) {
      upbuttonState = reading2;

      if (upbuttonState == HIGH && timerdone == 1) {
        display.invertDisplay(false);
        timerdone = 0;
      } else if (upbuttonState == HIGH && menu != 0 && menu1 == 0 && menu2 == 0 && menu3 == 0) {
        menu = menu - 1;
      } else if (upbuttonState == HIGH && menu == 0) {
        menu = menu + 1;
      } else if (upbuttonState == HIGH && menu1 == 1 && menu2 == 0 && menu3 == 0) {
        startstop = !startstop;
      } else if (upbuttonState == HIGH && menu1 == 0 && menu2 == 1 && menu3 == 0) {
        timerstartstop = !timerstartstop;
        timermin = 0;
        timersec = 0;
      } else if (upbuttonState == HIGH && menu1 == 0 && menu2 == 0 && menu3 == 1 && settimemode == 0) {
        enterhour = enterhour + 1;
      } else if (upbuttonState == HIGH && menu1 == 0 && menu2 == 0 && menu3 == 1 && settimemode == 1) {
        entermin = entermin + 1;
      } else if (upbuttonState == HIGH && menu1 == 0 && menu2 == 0 && menu3 == 1 && settimemode == 2) {
        entersec = entersec + 1;
      }
    }

  }

  /////////////////Bottom Button/////////////////

  if ((millis() - dwnlastDebounceTime) > 50) {
    if (reading3 != dwnbuttonState) {
      dwnbuttonState = reading3;

      if (dwnbuttonState == HIGH && timerdone == 1) {
        display.invertDisplay(false);
        timerdone = 0;
      } else if (dwnbuttonState == HIGH && menu1 == 0 && menu2 == 0 && menu3 == 0) {
        menu = menu + 1;
      } else if (dwnbuttonState == HIGH && menu1 == 1 && menu2 == 0 && menu3 == 0) {
        ssec = 0;
        smin = 0;
        shr  = 0;
      } else if (dwnbuttonState == HIGH && menu1 == 0 && menu2 == 1 && menu3 == 0 && timerstartstop == 0) {
        timermode = timermode + 1;
      } else if (dwnbuttonState == HIGH && menu1 == 0 && menu2 == 0 && menu3 == 1 && settimemode == 0) {
        enterhour = enterhour - 1;
      } else if (dwnbuttonState == HIGH && menu1 == 0 && menu2 == 0 && menu3 == 1 && settimemode == 1) {
        entermin = entermin - 1;
      } else if (dwnbuttonState == HIGH && menu1 == 0 && menu2 == 0 && menu3 == 1 && settimemode == 2) {
        entersec = entersec - 1;
      }
    }
  }

  //Button Debounce Stuff
  midlastButtonState = reading;
  uplastButtonState  = reading2;
  dwnlastButtonState = reading3;

  /////////////////Clock Interface/////////////////
                          
  
  if (menu == 0 && menu1 == 0 && menu2 == 0 && menu3 == 0) {
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(15, 22);
    if (enterhour < 10) {
 display.setCursor(5, 26);
 display.print("0"); 
 display.setCursor(17, 26); 
 display.print(Clock.getHour(h12Flag, pmFlag), DEC);
 } else {
 display.setCursor(5, 26); 
 display.print(Clock.getHour(h12Flag, pmFlag), DEC);
 } 
 display.setTextColor(SSD1306_WHITE); 
 display.setCursor(35, 26); 
 display.println(":");
 if (Clock.getMinute() < 10) {
 display.setCursor(48, 26); 
 display.print("0"); 
 display.setCursor(55, 26); 
 display.print(Clock.getMinute());
 } else {
 display.setCursor(48, 26); 
 display.print(Clock.getMinute());
 } 
    if (Clock.getSecond() < 10) {
      display.setTextSize(1);
      display.setCursor(85, 36);
      display.print("0");
      display.setCursor(91, 36);
      display.print(Clock.getSecond());
    } else {
      display.setTextSize(1);
      display.setCursor(85, 36);
      display.print(Clock.getSecond());
    }
    display.setCursor(0, 0);
    display.print(Clock.getTemperature());
    display.println(" C");

    if (timerstartstop == 1 || startstop == 1) {
      if (timerstartstop == 1) {
        display.setTextSize(1);
        display.setCursor(80, 9);
        display.println("Timer On");
      }
      if (startstop == 1) {
        display.setTextSize(1);
        display.setCursor(56, 0);
        display.println("Stopwatch On");
      }
    }
  }

  /////////////////Menu for Stopwatch/////////////////

  if (menu == 1 && menu1 == 0) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(10, 10);
    display.print("Stopwatch");
    display.setCursor(10, 25);
    display.setTextColor(SSD1306_WHITE);
    display.print("Timer");
    display.setCursor(10, 40);
    display.setTextColor(SSD1306_WHITE);
    display.print("Set Time");
  }

  /////////////////Menu for Timer/////////////////

  if (menu == 2 && menu2 == 0) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.print("Stopwatch");
    display.setCursor(10, 25);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.print("Timer");
    display.setCursor(10, 40);
    display.setTextColor(SSD1306_WHITE);
    display.print("Set Time");
  }

  /////////////////Menu for Time setting/////////////////

  if (menu == 3 && menu3 == 0) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.print("Stopwatch");
    display.setCursor(10, 25);
    display.setTextColor(SSD1306_WHITE);
    display.print("Timer");
    display.setCursor(10, 40);
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.print("Set Time");
  }

  //Overflow Control
  if (menu == 4) {
    menu = 0;
  }

  if (menu == -1) {
    menu = 3;
  }

  /////////////////Stopwatch/////////////////

  if (menu == 1 && menu1 == 1) {
    if (startstop == 0) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      display.setCursor(95, 0);
      display.print("Start");
      display.setCursor(95, 29);
      display.print("Back");
      display.setCursor(95, 56);
      display.print("Reset");

      display.setTextSize(2);

      if (shr < 10) {
        display.setCursor(5, 26);
        display.print("0");
        display.setCursor(17, 26);
        display.print(shr);
      } else {
        display.setCursor(5, 26);
        display.print(shr);
      }

      display.setCursor(25, 26);
      display.println(":");

      if (smin < 10) {
        display.setCursor(33, 26);
        display.print("0");
        display.setCursor(45, 26);
        display.print(smin);
      } else {
        display.setCursor(33, 26);
        display.print(smin);
      }

      display.setCursor(53, 26);
      display.println(":");

      if (ssec < 10) {
        display.setCursor(61, 26);
        display.print("0");
        display.setCursor(73, 26);
        display.print(ssec);
      } else {
        display.setCursor(61, 26);
        display.print(ssec);
      }
    }
    if (startstop == 1) {
      unsigned long StopcurrentMillis = millis();

      if (StopcurrentMillis - StoppreviousMillis >= interval) {
        StoppreviousMillis = StopcurrentMillis;
        ssec = ssec + 1;
      }
      if (ssec > 59) {
        ssec = 0;
        smin = smin + 1;
      }
      if (smin > 59) {
        smin = 0;
        shr = shr + 1;
      }

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(95, 0);
      display.print("Stop");
      display.setCursor(95, 29);
      display.print("Back");
      display.setCursor(95, 56);
      display.print("Reset");
      display.setTextSize(2);
      if (shr < 10) {
        display.setCursor(5, 26);
        display.print("0");
        display.setCursor(17, 26);
        display.print(shr);
      } else {
        display.setCursor(5, 26);
        display.print(shr);
      }
      display.setCursor(25, 26);
      display.println(":");
      if (smin < 10) {
        display.setCursor(33, 26);
        display.print("0");
        display.setCursor(45, 26);
        display.print(smin);
      } else {
        display.setCursor(33, 26);
        display.print(smin);
      }
      display.setCursor(53, 26);
      display.println(":");
      if (ssec < 10) {
        display.setCursor(61, 26);
        display.print("0");
        display.setCursor(73, 26);
        display.print(ssec);
      } else {
        display.setCursor(61, 26);
        display.print(ssec);
      }
    }
  }

  if (startstop == 1) {
    unsigned long StopcurrentMillis = millis();

    if (StopcurrentMillis - StoppreviousMillis >= interval) {
      StoppreviousMillis = StopcurrentMillis;
      ssec = ssec + 1;
    }
    if (ssec > 59) {
      ssec = 0;
      smin = smin + 1;
    }
    if (smin > 59) {
      smin = 0;
      shr = shr + 1;
    }
  }
  /////////////////Timer/////////////////

  if (menu == 2 && menu2 == 1) {
    if (timerstartstop == 0) {
      digitalWrite(optokapu, LOW);
// Egy kimeneti lábat beállitani kikapcsolt álapotban.    
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(95, 0);
      display.print("Start");
      display.setCursor(95, 29);
      display.print("Back");
      display.setCursor(95, 56);
      display.print("Mode");

      display.setTextSize(2);

      if (timermode == 0) {
        display.setCursor(5, 26);
        display.print("30 Sec");
      }
      if (timermode == 1) {
        display.setCursor(5, 26);
        display.print("1 Min");
      }
      if (timermode == 2) {
        display.setCursor(5, 26);
        display.print("5 Min");
      }
      if (timermode == 3) {
        display.setCursor(5, 26);
        display.print("10 Min");
      }
      if (timermode == 4) {
        display.setCursor(5, 26);
        display.print("20 Min");
      }
      if (timermode == 5) {
        timermode = 0;
        display.setCursor(5, 26);
        display.print("30 Sec");
      }
    }
    if (timerstartstop == 1) {
      digitalWrite(optokapu, HIGH);
// A kimeneti láb bekapcsolása.
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(95, 0);
      display.print("Stop");
      display.setCursor(95, 29);
      display.print("Back");

      display.setTextSize(2);

      unsigned long TimcurrentMillis = millis();

      if (timermode == 0 && timermin == 0 && timersec == 0) {
        timermin = 0;
        timersec = 30;
      } else if (timermode == 1 && timermin == 0 && timersec == 0) {
        timermin = 1;
        timersec = 0;
      } else if (timermode == 2 && timermin == 0 && timersec == 0) {
        timermin = 5;
        timersec = 0;
      } else if (timermode == 3 && timermin == 0 && timersec == 0) {
        timermin = 10;
        timersec = 0;
      } else if (timermode == 4 && timermin == 0 && timersec == 0) {
        timermin = 20;
        timersec = 0;
      }

      if (timermin > 0 || timersec > 0) {
        if (TimcurrentMillis - TimpreviousMillis >= interval) {
          TimpreviousMillis = TimcurrentMillis;
          timersec = timersec - 1;
        }
        if (timersec <= 0) {
          if (timermin == 0) {
            timermin == 0;
            timersec == 0;
            display.invertDisplay(true);
            timerstartstop = !timerstartstop;
            timerdone = 1;
          }
          timersec = 59;
          timermin = timermin - 1;
        }
      }

      if (timermin < 10) {
        display.setCursor(5, 26);
        display.print("0");
        display.setCursor(17, 26);
        display.print(timermin);
      } else {
        display.setCursor(5, 26);
        display.print(timermin);
      }
      display.setCursor(25, 26);
      display.println(":");
      if (timersec < 10) {
        display.setCursor(33, 26);
        display.print("0");
        display.setCursor(45, 26);
        display.print(timersec);
      } else {
        display.setCursor(33, 26);
        display.print(timersec);
      }
    }
  }

  if (timermin > 0 || timersec > 0 && timerstartstop == 1) {
    unsigned long TimcurrentMillis = millis();
    if (TimcurrentMillis - TimpreviousMillis >= interval) {
      TimpreviousMillis = TimcurrentMillis;
      timersec = timersec - 1;
    }
    if (timersec <= 0) {
      if (timermin == 0) {
        timermin == 0;
        timersec == 0;
        display.invertDisplay(true);
        digitalWrite(optokapu, LOW);
 // Ha 00 ra ér akkor a kimeneti lábat ismét kikapcsolja.
        timerstartstop = !timerstartstop;
        timerdone = 1;
      }
      timersec = 59;
      timermin = timermin - 1;
    }
  }
  /////////////////Setting Time Function/////////////////

  if (menu == 3 && menu3 == 1) {
    display.clearDisplay();
    /*enterhour = now.hour();
    entermin  = now.minute();
    entersec  = now.second();*/
    if (settimemode == 0) {
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      display.setCursor(95, 0);
      display.print("(+)");
      display.setCursor(95, 29);
      display.print("Next");
      display.setCursor(95, 56);
      display.print("(-)");

      display.setTextSize(2);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);

      if (enterhour < 10) {
        display.setCursor(5, 26);
        display.print("0");
        display.setCursor(17, 26);
        display.print(enterhour);
      } else {
        display.setCursor(5, 26);
        display.print(enterhour);
      }
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(25, 26);
      display.println(":");
      if (entermin < 10) {
        display.setCursor(33, 26);
        display.print("0");
        display.setCursor(45, 26);
        display.print(entermin);
      } else {
        display.setCursor(33, 26);
        display.print(entermin);
      }
      display.setCursor(53, 26);
      display.println(":");
      if (entersec < 10) {
        display.setCursor(61, 26);
        display.print("0");
        display.setCursor(73, 26);
        display.print(entersec);
      } else {
        display.setCursor(61, 26);
        display.print(entersec);
      }
    }

    if (settimemode == 1) {
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      display.setCursor(95, 0);
      display.print("(+)");
      display.setCursor(95, 29);
      display.print("Next");
      display.setCursor(95, 56);
      display.print("(-)");

      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);

      if (enterhour < 10) {
        display.setCursor(5, 26);
        display.print("0");
        display.setCursor(17, 26);
        display.print(enterhour);
      } else {
        display.setCursor(5, 26);
        display.print(enterhour);
      }
      display.setCursor(25, 26);
      display.println(":");
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      if (entermin < 10) {
        display.setCursor(33, 26);
        display.print("0");
        display.setCursor(45, 26);
        display.print(entermin);
      } else {
        display.setCursor(33, 26);
        display.print(entermin);
      }
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(53, 26);
      display.println(":");
      if (entersec < 10) {
        display.setCursor(61, 26);
        display.print("0");
        display.setCursor(73, 26);
        display.print(entersec);
      } else {
        display.setCursor(61, 26);
        display.print(entersec);
      }
    }

    if (settimemode == 2) {
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      display.setCursor(95, 0);
      display.print("(+)");
      display.setCursor(95, 29);
      display.print("Next");
      display.setCursor(95, 56);
      display.print("(-)");

      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);

      if (enterhour < 10) {
        display.setCursor(5, 26);
        display.print("0");
        display.setCursor(17, 26);
        display.print(enterhour);
      } else {
        display.setCursor(5, 26);
        display.print(enterhour);
      }
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(25, 26);
      display.println(":");
      if (entermin < 10) {
        display.setCursor(33, 26);
        display.print("0");
        display.setCursor(45, 26);
        display.print(entermin);
      } else {
        display.setCursor(33, 26);
        display.print(entermin);
      }

      display.setCursor(53, 26);
      display.println(":");
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      if (entersec < 10) {
        display.setCursor(61, 26);
        display.print("0");
        display.setCursor(73, 26);
        display.print(entersec);
      } else {
        display.setCursor(61, 26);
        display.print(entersec);
      }
    }

    if (enterhour == 24) {
      enterhour = 0;
    }
    if (entermin == 60) {
      entermin = 0;
    }
    if (entersec == 60) {
      entersec = 0;
    }

    if (enterhour == -1) {
      enterhour = 24;
    }
    if (entermin == -1) {
      entermin = 60;
    }
    if (entersec == -1) {
      entersec = 60;
    }

    if (settimemode == 3) {
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(5, 26);
      display.print("Time Set!");
      display.display();
      //now.setTime(enterhour, entermin, entersec);
     Clock.setHour(enterhour);
		   Clock.setMinute(entermin);
		   Clock.setSecond(entersec);
      delay(3000);
      settimemode = 0;
      menu3 = 0;
    }
  }

  /////////////////Refresh Controller/////////////////

  if (millis() - previousrefresh >= 33) {
    //Update the time function with the current time
    //t = rtc.getTime();
    
    display.display();
    previousrefresh = millis();
  }
}
