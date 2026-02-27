#include <Arduino.h>
#include <Adafruit_GFX.h>                     //Display Bibliotheken werden importiert
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include "morsecode8.h"

int Taster = 5;

#define OLED_RESET -1
#define OLED_WIDTH 128    //x-Achse
#define OLED_HEIGHT 64    //y-Achse
//#define SSD1306_LCDHEIGHT 64
//#define SSD1306_128_64

//Adafruit_SSD1306 display(OLED_RESET);
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);     //Breite, Höhe, I2C und Reset pin für Display werden festgelegt

float r = 5.4;    //Radius für Kreise

int a = 0;

bool buttonPressed = false;   //für dot measurement
bool buttonhigh = false;
bool buttonlow = true;
long pressStart = 0;
long pauseStart = 0;
long signalTime = 0;
long pauseTime = 0;
bool printchar = false;

const unsigned char myBitmap [] PROGMEM = {
	0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xc0, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x03, 0xf0, 0x00, 0x00, 
	0x00, 0x0f, 0x80, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 
	0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x7f, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x0f, 0xe0, 
	0x0f, 0xe0, 0x00, 0x00, 0xfe, 0x00, 0x7f, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x0f, 0xc7, 0xf0, 0x00, 
	0x00, 0x0f, 0xe0, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0x08, 0x7f, 0x00, 0x0f, 0xf1, 0x00, 0x00, 
	0x00, 0x08, 0x07, 0xf0, 0xfe, 0x11, 0x00, 0x00, 0x00, 0x08, 0x00, 0x3f, 0xc0, 0x11, 0x00, 0x00, 
	0x00, 0x08, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 
	0x00, 0x08, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 
	0x00, 0x08, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 
	0x00, 0x06, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x3c, 0x00, 0x00, 
	0x00, 0x00, 0x7f, 0xe0, 0x7f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x10, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


bool learn = true;
bool timeSet = false;
long dotTime = 0;    //Zeit für Punkt
char character[6];
String eingabeTaster = "";
bool charPause = false;
// Morsecode Array

String morseCode[] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--",
"-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..",
"-----",".----","..---","...--","....-",".....","-....","--...","---..","----."};

char Zeichen[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m',
'n','o','p','q','r','s','t','u','v','w','x','y','z',
'0','1','2','3','4','5','6','7','8','9'};

int lenghtZeichen = sizeof(Zeichen);

char checkChar(String morseEingabe){      //Durchgehen des Arrays nach dem Morsecode
  Serial.print(morseEingabe + " : ");
  for (int i=0; i < lenghtZeichen; i++){
    if (morseCode[i] == morseEingabe){
      if (learn == true){
        display.clearDisplay();
        display.setCursor(5, 30);
        display.print(morseEingabe);
        display.setCursor(70, 30);
        display.print(':');
        display.setCursor(90, 30);
        display.print(Zeichen[i]);
        display.display();
      }
      else{
        display.print(Zeichen[i]);
        display.display();
      }
      return Zeichen[i];                  //Ausgabe des encodierten Buchstabens
    }
  }
  if(learn){
    display.clearDisplay();
    display.setCursor(5, 30);
    display.print(morseEingabe);
    display.setCursor(70, 30);
    display.print(':');
    display.setCursor(90, 30);
    display.print('?');
    display.display();
  }
  else{
    display.print('?');
    display.display();
  }
  return '?';
}


int measureDotTime(){   //misst die durchschnittliche Zeit für 1x kurz Drücken
  Serial.println("... kurze Punkte Zeit messen. Bitte 3x kurz drücken");
  /*display.setCursor(0, 0);
  display.setTextSize(2);
  display.clearDisplay();
  display.print("3x drucken");
  display.display(); */

  display.clearDisplay();     //Kreise zeichnen
  display.drawCircle((OLED_WIDTH/4), (OLED_HEIGHT/2), (OLED_HEIGHT/r), WHITE);
  display.drawCircle((OLED_WIDTH/4)*2, (OLED_HEIGHT/2), (OLED_HEIGHT/r), WHITE);
  display.drawCircle((OLED_WIDTH/4)*3, (OLED_HEIGHT/2), (OLED_HEIGHT/r), WHITE);
  display.drawCircle((OLED_WIDTH/4), (OLED_HEIGHT/2), (OLED_HEIGHT/r), WHITE);
  display.drawCircle((OLED_WIDTH/4)*2, (OLED_HEIGHT/2), (OLED_HEIGHT/r), WHITE);
  display.drawCircle((OLED_WIDTH/4)*3, (OLED_HEIGHT/2), (OLED_HEIGHT/r), WHITE);
  display.display();

  long pressStart = 0;
  long pressDuration[3];
  int i=0;
  while(timeSet == false){
    if(digitalRead(Taster) == LOW && i<3 && buttonPressed == false){         //Wenn Taster gedrückt wird, wir der "Timer" gestartet
      buttonPressed = true;
      pressStart = millis();
    }
    else if(digitalRead(Taster) == HIGH && buttonPressed == true && i<3){      //erste gemessene Zeit wird in Array geschrieben
      buttonPressed = false;
      pressDuration[i] = (millis()-pressStart);
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(pressDuration[i]);
      Serial.println(" ms");
      i++;
      /*display.setCursor(0, 20);
      display.setTextSize(2);
      display.clearDisplay();
      display.print(i);
      display.display();*/

      display.fillCircle((OLED_WIDTH/4)*i, (OLED_HEIGHT/2), (OLED_HEIGHT/r), WHITE);
      display.display();  
    }
    else if(i >= 3){                        //Nach drei Messungen wird der Durschschnittswerk für einen . berechnet
      for(int s = 0; s < i; s++){
        dotTime += pressDuration[s];
      }
      dotTime /= i;
      timeSet = true;
    }
    delay(10);
  }
  delay(dotTime*3);
  display.setCursor(0, 20);
  display.setTextSize(2);
  display.clearDisplay();
  display.print(". = ");
  display.print(dotTime);
  display.print(" ms");
  display.display();
  delay(1000);
  return dotTime;
}

void recLenght(long receivedSignal){                                         //Länge des Signals bestimmen
  //Serial.println("reclenght started");
  if (receivedSignal < (2*dotTime)){         // Zeit etwa 1x dotTime == .
    character[a] = '.';
    //Serial.print(a);
    //Serial.print(character[a]);
    //Serial.println(" short press");
    a++;
  }
  else if (receivedSignal >= (2*dotTime) && receivedSignal <= (7*dotTime)){                    // Zeit etwa 3x dotTime == -
    character[a] = '-';
    //Serial.print(a);
    //Serial.print(character[a]);
    //Serial.println("long press");
    a++;
  }
  else if (receivedSignal > (7*dotTime)){
    display.clearDisplay();
    learn = !learn;
    if(!learn){
      display.setCursor(0, 0);
    }
    if(learn){
      display.drawBitmap(34, 18, myBitmap, 60, 60, WHITE);
      display.display();
      delay(500);
      display.clearDisplay();
      display.setCursor(70, 30);
      display.print(':');
    }
    display.display();
  }
}

void recPause(long receivedPause){                                        //Länge der Pause zwischen zwei Signalen bestimmen
  //Serial.println("recPause started");
  if (receivedPause > 0 && receivedPause < (3*dotTime)){         // Zeit etwa 1x dotTime == Pause zwischen ., bzw. -
    //Serial.println("kurze Pause");
  }
  else if (receivedPause >= (3*dotTime) && receivedPause < (7*dotTime) && a > 0){                    // Zeit etwa 3x dotTime == Pause zwischen Zeichen
    printchar = true;
    //Serial.println("mittlere Pause");
    //return eingabeTaster;
  }
  else if (receivedPause >= (7*dotTime)){                    // Zeit etwa 7x dotTime == Pause zwischen Wörtern
    /*display.print(" ");
    display.display();*/    //zur Übung, bzw. für Einsteiger keine Leerzeichen
    //Serial.println("lange Pause");
    //Serial.print(" ");                        //Leerzeichen zwischen Wörtern
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Morse Code");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  } 
  delay(1000);
  pinMode(Taster, INPUT_PULLUP);

  display.clearDisplay();
  display.setCursor(0, (64/2));
  display.setTextSize(1);
  display.setFont(&MorseCoder_RppRE8pt7b);
  display.setTextColor(WHITE);
  display.print("morsecode");
  display.display();
  delay(1500);
  display.clearDisplay();
  display.setFont(NULL);
  display.setTextSize(2);
  display.setCursor(0, 0);

  measureDotTime();
  Serial.print(dotTime);
  Serial.println(" ms");
  delay(1000);

  memset(character, 0, sizeof(character));      //character Array wird komplett auf 0 gesetzt
  
  display.clearDisplay();
  display.drawBitmap(34, 18, myBitmap, 60, 60, WHITE);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.setFont(NULL);
  display.setTextSize(2);
  display.setCursor(70, 30);
  display.print(':');
  display.display();
}

void loop() {

  // put your main code here, to run repeatedly:
  if (digitalRead(Taster) == LOW && buttonhigh == false){     //wenn der Taster gedrückt wird, wird der "timer" für die Drückzeit gestartet
    pressStart = millis();
    pauseTime = millis() - pauseStart;
    if (pauseTime > 50){
      Serial.println(pauseTime);
      Serial.print("pausenzeit: ");
      Serial.println(pauseTime);
      recPause(pauseTime);
    }
    buttonhigh = true;
    buttonlow = false;
    Serial.println("1");
  }

  if (digitalRead(Taster) == HIGH && buttonlow == false){     //wenn der Taster losgelassen wird, wird der "timer" für die Pause gestartet
    pauseStart = millis();
    signalTime = millis() - pressStart;
    if (signalTime > 0 && a <= 5){
      Serial.print("drückzeit: ");
      Serial.println(signalTime);
      recLenght(signalTime);
    }
    
    if (a > 5){
      a = 0;
    }
    buttonlow = true;
    buttonhigh = false;
    Serial.println("0");
  }

  if (printchar == true or ((millis()-pressStart) > 5*dotTime) and a > 0){
    printchar = false;
    eingabeTaster = "";                                               //Eingegebene Werte werden zurückgesetzt
    for(int s = 0; s < a; s++){                        //Empfangener Code für Zeichen wird in Eingabe Taster geschrieben
      eingabeTaster += character[s];
    }
    eingabeTaster.trim();       //Lerzeichen werden entfernt
    printchar = false;
    a = 0;
    Serial.println(checkChar(eingabeTaster));
    memset(character, 0, sizeof(character));
  }


  if (Serial.available() > 0){
    String eingabe = Serial.readString();    //Eingabe bis zum Umruch also Enter lesen

    eingabe.trim();   //Leerzeichen vor und nach der Eingabe entfernen
    
    Serial.println(checkChar(eingabe));
  }


  delay(20);
}
