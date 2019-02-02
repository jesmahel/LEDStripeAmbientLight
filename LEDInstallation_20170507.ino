#include <FastLED.h>
#define NUM_LEDS 291
#define LED_ECKE = 155

//----  Digital out
#define LED_PIN 11

//----  Analog in
#define POTI 0

//---- Digital in
#define EINAUS 12
#define ZURUECK 13
#define VOR 14

#define DIGIN7 7
#define DIGIN8 8
#define DIGIN9 9
#define DIGIN10 10
#define DIGIN11 11
#define DIGIN12 12
#define DIGIN13 13
#define DIGIN14 14
#define DIGIN15 15
#define HELLMITTELN 20
#define MITTEL_N 16

//-----------------------------------------------------------------

//----   LED Stripe initialisieren
CRGB leds[NUM_LEDS];
int Farbe = 128;
int Farbe_alt = 128;
int hell_alt = 128;
boolean wechsel = false;
boolean lichtan = true;
int prog = 1; //Regenbogen
int offset = 0; // Start bei Durchlauf
int del = 200; // ms Verzögerung
int hue_start = 0;
int hue_counter = 0;
int hue_geschw = 2; // 1 = Regenbogen passend für alle LED, 2 = halber Regenbogen , etc...
int hellm[HELLMITTELN];
int hellw = 0;
int hellakt = 0;
int hell;

int potim[MITTEL_N];
int potiakt = 0;
int potiw = 0;
int poti;

//-----------------------------------------------------------------

void setup()
{
  //----  Digital I/O setzen
  pinMode(LED_PIN, OUTPUT); // LED_PIN als Ausgang definieren
  //pinMode(EINAUS, INPUT);   // Ein Aus Schalter als EIngang definieren
  //pinMode(ZURUECK, INPUT);  // ZURUECK-Schalter als Eingang definieren
  //pinMode(VOR, INPUT);      // VOR-Schalter als Eingang definieren
  
  pinMode(DIGIN7, INPUT);
  pinMode(DIGIN8, INPUT);
  pinMode(DIGIN9, INPUT);
  pinMode(DIGIN10, INPUT);
  pinMode(DIGIN12, INPUT);
  pinMode(DIGIN13, INPUT);
  pinMode(DIGIN14, INPUT);
  pinMode(DIGIN15, INPUT);

  //---- Ausgabe für Debug
  Serial.begin(9600);
  
  //---- LED initialisieren  
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  //---- Variable setzen
  hue_counter = 0;
  hue_start = 0;
}

void loop()
{
    // Potentiometer auslesen
    potiakt = analogRead(POTI); // Potentiometer auslesen (0..1023)
    
    // Wert speichern
    potim[potiw] = potiakt;

    // Wertspeicher shiften
    potiw++;
    if (potiw > MITTEL_N)
    {
      potiw = 0;
    }

    // Mittelwert berechnen
    poti = 0;
    for (int j=0; j<MITTEL_N; j++)
    {
      poti += potim[j];
    }
    poti /= MITTEL_N;

    int hue;
    //int einaus = digitalRead(EINAUS);   // EinAus Schalter auslesen
    //int zurueck = digitalRead(ZURUECK); // Zurück Schalter auslesen
    //int vor = digitalRead(VOR);         // Vor Schalter auslesen

/*
    int d7 = digitalRead(DIGIN7);
    int d8 = digitalRead(DIGIN8);
    int d9 = digitalRead(DIGIN9);
    int d10 = digitalRead(DIGIN10);
    int d12 = digitalRead(DIGIN12);
    int d13 = digitalRead(DIGIN13);
    int d14 = digitalRead(DIGIN14);
    int d15 = digitalRead(DIGIN15);

    Serial.print("POTI: ");
    Serial.print(pot);
    Serial.print("\n");

    Serial.print(d7);
    Serial.print(" ");
    Serial.print(d8);
    Serial.print(" ");
    Serial.print(d9);
    Serial.print(" ");
    Serial.print(d10);
    Serial.print(" ");
    Serial.print(d12);
    Serial.print(" ");
    Serial.print(d13);
    Serial.print(" ");
    Serial.print(d14);
    Serial.print(" ");
    Serial.print(d15);
    Serial.print("\n");
    
    delay(500);
*/      
  //--------  Jetzt LEDs setzen
  switch(prog)
  {
    case 0:
      if (lichtan)
      {
        FastLED.showColor(CRGB(0,0,0));
      }
      lichtan = false;
      break;
      
    case 1:

      // Poti steuert Geschwindigkeit und Helligkeit

      hue_geschw = (poti / 128) + 1;

      // Helligkeit
      hellakt = poti/8;
      hellakt = hellakt % 16;
      hellakt = 135 + hellakt * 8;
      hellm[hellw] = hellakt;

      // Wertspeicher shiften
      hellw++;
      if (hellw > HELLMITTELN)
      {
        hellw = 0;
      }

      // Mittelwert berechnen
      hell = 0;
      for (int j=0; j<HELLMITTELN; j++)
      {
        hell += hellm[j];
      }
      hell /= HELLMITTELN;
      
      Serial.print(hell);
      Serial.print("\n");
      
      if (hell > 255)
      {
        hell = 255;
      }
      else if (hell < 40)
      {
        hell = 0;
      }
      

      // LEDS setzen
      hue = hue_start; // Mit Start Hue Wert beginnen

      // Farbe für jede LED setzen
      for (int dot = 0; dot < NUM_LEDS; dot++)
      {
        // Starten mit der ersten mit dem hue_startwert
        leds[dot] = CHSV(hue,255,hell);

        // 
        hue_counter = hue_counter + (NUM_LEDS / hue_geschw);  // 
        if (hue_counter >= NUM_LEDS)
        {
          hue++;
          hue_counter = hue_counter - NUM_LEDS;
          if (hue > 255)
          {
            hue = 0;
          }
        }
/*        
        Serial.print("HUE : ");
        Serial.print(hue);
        Serial.print(", ");
        Serial.print(hue_counter);
        Serial.print("\n");
*/       
      }

      FastLED.show();
      delay(50);

      // Hue Startwert um 1 erhöhen
      hue_start++;
      if (hue_start >255)
      {
        hue_start = 0;
      }
      
      break;
        
    case 2:
      break;
  }
/*
  else
  {
    if ((Farbe_alt != Farbe || hell_alt != hell) && lichtan)
    {
      FastLED.showColor(CHSV(Farbe, 255, hell));
    }
    else if (!lichtan)
    {
      FastLED.showColor(CRGB(0,0,0));
    }
  }
*/
}

