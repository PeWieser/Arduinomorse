#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//für AP2 Board
const int LDR = A1;
const int POTI = A2;
const int BUTTON_1 = 2;
const int BUTTON_2 = 3;
const int LED = 13;

//nicht für AP2 board
//int Taster = 5;
String morseInput = "...";

int a = 0;
int lcddotpos = 0;
int lcdcharpos = 0;
int cntpstn = 0;      //damit cursor in die zweite Zeile schreibt, wenn Ende erreicht.
int line = 0;         //Zeilenangabe

bool buttonPressed = false;   //für dot measurement
bool buttonhigh = false;
bool buttonlow = true;
long pressStart = 0;
long pauseStart = 0;
long signalTime = 0;
long pauseTime = 0;
bool printchar = false;

bool learn = true;    //true für lernen (code + übersetzung), false für schreiben (übersetzung aneinandergereit)
bool button_1pressed = false;

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



char checkChar(String morseEingabe){      //Durchgehen des Arrays nach dem Morsecodes 
  Serial.print(morseEingabe + " : ");
  for (int i=0; i < lenghtZeichen; i++){
    if (morseCode[i] == morseEingabe){
      if (learn == true){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(morseEingabe);
        lcd.setCursor(6, 0);
        lcd.print(':');
        lcd.setCursor(8, 0);
        lcd.print(Zeichen[i]);
      }
      else{
        if (cntpstn >= 16 && line == 0){              //für LCD Zeilensteuerung
                lcd.setCursor(0, 1);
                cntpstn = 0;
                line = 1;
              }
        else if(cntpstn >= 16 && line == 1){
          lcd.clear();
          delay(100);
          lcd.setCursor(0, 0);
          cntpstn = 0;
          line = 0;
        }
        lcd.print(Zeichen[i]);
        cntpstn ++;
      }
      return Zeichen[i];                  //Ausgabe des encodierten Buchstabens
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(morseEingabe);
  lcd.setCursor(6, 0);
  lcd.print(':');
  lcd.setCursor(8, 0);
  lcd.print('?');
  return '?';
}

int measureDotTime(){
  Serial.println("... kurze Punkte Zeit messen. Bitte 3x kurz drücken");
  lcd.print("3x kurz drücken.");
  long pressStart = 0;
  long pressDuration[3];
  int i=0;
  while(timeSet == false){
    if(digitalRead(BUTTON_2) == LOW && i<3 && buttonPressed == false){         //Wenn Taster gedrückt wird, wir der "Timer" gestartet
      buttonPressed = true;
      pressStart = millis();
      digitalWrite(LED, HIGH);
    }
    else if(digitalRead(BUTTON_2) == HIGH && buttonPressed == true && i<3){      //erste gemessene Zeit wird in Array geschrieben
      digitalWrite(LED, LOW);
      buttonPressed = false;
      pressDuration[i] = (millis()-pressStart);
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(pressDuration[i]);
      Serial.println(" ms");
      lcd.clear();
      delay(100);
      lcd.print(pressDuration[i]);
      lcd.print(" ");
      lcd.print("ms");
      i++;
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
  lcd.clear();
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
  else if (receivedSignal >= (2*dotTime)){                    // Zeit etwa 3x dotTime == -
    character[a] = '-';
    //Serial.print(a);
    //Serial.print(character[a]);
    //Serial.println("long press");
    a++;
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
    /*if (learn == false){
      lcd.print(" ");
      //Serial.println("mao");
    }*/
    //Serial.println("lange Pause");
    //Serial.print(" ");                        //Leerzeichen zwischen Wörtern
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Morse Code");

  // LCD initialisieren
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Morse Code");
  lcd.setCursor(7, 1);
  //lcd.print("by merlo");
  delay(1000);
  lcd.clear();
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  //pinMode(Taster, INPUT_PULLUP);

  Wire.begin();   // I2C starten
  measureDotTime();
  delay(100);
  Serial.print(dotTime);
  Serial.println(" ms");
  lcd.print("dotTime: ");
  lcd.print(dotTime);
  lcd.print(" ");
  lcd.print("ms");
  memset(character, 0, sizeof(character));      //character Array wird komplett auf 0 gesetzt
  delay(1000);
  lcd.clear();
  delay(100);
  lcd.setCursor(5, 0);
  lcd.print("ready!");
  delay(1000);
  lcd.clear();
  delay(100);
  lcd.setCursor(6, 0);
  lcd.print(':');
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(BUTTON_2) == LOW && buttonhigh == false){     //wenn der Taster gedrückt wird, wird der "timer" für die Drückzeit gestartet
    digitalWrite(LED, HIGH);
    pressStart = millis();
    pauseTime = millis() - pauseStart;
    if (pauseTime > 50){
      //Serial.print("pausenzeit: ");
      //Serial.println(pauseTime);
      recPause(pauseTime);
    }
    buttonhigh = true;
    buttonlow = false;
    //Serial.println("1");
  }

  if (digitalRead(BUTTON_2) == HIGH && buttonlow == false){     //wenn der Taster losgelassen wird, wird der "timer" für die Pause gestartet
    digitalWrite(LED, LOW);
    pauseStart = millis();
    signalTime = millis() - pressStart;
    if (signalTime > 0 && a <= 5){
      //Serial.print("drückzeit: ");
      //Serial.println(signalTime);
      recLenght(signalTime);
    }
    if (a > 5){
      a = 0;
    }
    buttonlow = true;
    buttonhigh = false;
    //Serial.println("0");
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


  if (Serial.available() > 0 ){
    String eingabe = Serial.readString();    //Eingabe bis zum Umruch also Enter lesen

    eingabe.trim();   //Leerzeichen vor und nach der Eingabe entfernen
    
    Serial.println(checkChar(eingabe));
  }

  if (digitalRead(BUTTON_1) == LOW && button_1pressed != true){
    button_1pressed = true;
    if (learn == true){
      learn = false;
      pauseStart = millis();    //ansonsten wird die bereits verstrichene Zeit zur Pause angerechnet, was zu einem Leerzeichen am Start führen kann.
      lcd.clear();
      lcd.setCursor(0, 0);
      cntpstn = 0;
      line = 0;
    }
    else {
      learn = true;
      lcd.clear();
      delay(100);
      lcd.setCursor(0, 0);
      lcd.setCursor(6, 0);
      lcd.print(':');
    }
  }

  if (digitalRead(BUTTON_1) == HIGH && button_1pressed == true){
    button_1pressed = false;
  }

  delay(20);
}