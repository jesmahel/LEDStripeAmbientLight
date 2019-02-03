#include <FastLED.h>
#define NUM_LEDS 28
//#define LED_ECKE = 12

//----  Digital out
#define LED_PIN 11

//----  Analog in
#define POTI 0

//---- Digital in
#define EINAUS 12
#define ZURUECK 13
#define VOR 14

//---- Freie Digital in
#define DIGIN7 7
#define DIGIN8 8
#define DIGIN9 9
#define DIGIN10 10
#define DIGIN15 15

//---- Light states
#define ON true
#define OFF false

#define MINHELL 30

//-----------------------------------------------------------------

//----   LED Stripe initialisieren
CRGB leds[NUM_LEDS];

// Steuerungsinitialisierung
int Farbe = 128;
int Farbe_alt = 128;

int Hell = 80;
int OldHell = 80;

boolean LichtAn = ON;
boolean OldLichtAn = ON;

// Variable zur Programmsteuerung
int prog = 1; //Regenbogen
int erster_dot = 0; // Erste LED im Durchlauf
int dot_counter = 0; // 
int del = 200; // ms Verzögerung
int LedAn = true;

int hue;
int hue_start = 0; // immer bei 0 beginnen in der Farbskala
int hue_counter = 0;
int hue_geschw = 1;

// Berechnung der Linearen Werte wenn NUM_LEDS != 255;
int delta_hue = 256; // hue_ende - hue_start+1;
int hue_count = 0;

int delta_led = NUM_LEDS;
int led_count = 0;

int dot = 0;
int dot_count = 0;

// Variable für Einänge und Ausgänge
int EinAus = ON;
int OldEinAus = ON;

int Potiakt = 0;

int Zurueck = OFF;
int OldZurueck = OFF;

int Vor = OFF;
int OldVor = OFF;

//-----------------------------------------------------------------

void setup()
{
  // Digital I/O setzen
  pinMode(DIGIN7, INPUT);
  pinMode(DIGIN8, INPUT);
  pinMode(DIGIN9, INPUT);
  pinMode(DIGIN10, INPUT);
  pinMode(LED_PIN, OUTPUT); // LED_PIN als Ausgang definieren
  pinMode(EINAUS, INPUT);
  pinMode(ZURUECK, INPUT);
  pinMode(VOR, INPUT);
  pinMode(DIGIN15, INPUT);

  //---- Ausgabe für Debug
  //Serial.begin(9600);
  
  //---- LED initialisieren  
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  
  // ausschalten
  FastLED.showColor(CRGB(0,0,0));

}

void loop()
{

    // Potentiometer auslesen
    Potiakt = analogRead(POTI); // Potentiometer auslesen (0..1023)
    Hell = MINHELL + Potiakt * (256 - MINHELL) / 1024;

    // Programm vorschalten
    Vor = digitalRead(VOR);
    if (vor != OldVor && Vor == HIGH)
    {
      // Programm eins vorschalten
      prog++;

      if (prog > 4)
      {
        prog = 4;
      }

      //warten
      delay(del);
    }

    // Einaus-Schalter auslesen
    EinAus = digitalRead(EINAUS);   // EinAus Schalter auslesen
    if (EinAus != OldEinAus && EinAus == HIGH)
    {
      if (LichtAn == OFF)
      {
        LichtAn = ON;
      }
      else
      {
        LichtAn = OFF;
      }
    }

    // In alt status übernehmen
    OldEinAus = EinAus;
    
    // Lampe ein/ausschalten
    if (OldLichtAn != LichtAn)
    {
        if (LichtAn == ON)
        {
          OldProg = prog;
          prog = 0;
        }
        else
        {
          prog = OldProg;
        }
        OldLichtAn = LichtAn;

        // warten
        delay(del);
    }

    //int zurueck = digitalRead(ZURUECK); // Zurück Schalter auslesen
    //int vor = digitalRead(VOR);         // Vor Schalter auslesen
    
  //--------  Jetzt LEDs setzen
  switch(prog)
  {
    case 0:
      if (LedAn)
      {
        FastLED.showColor(CRGB(0,0,0));
      }
      LedAn = false;

      //200 ms warten
      delay(200);
      break;
      
    case 1:

      // Poti steuert Helligkeit
      
      // LEDS setzen
      hue = hue_start; // Mit Start Hue Wert beginnen
      dot = erster_dot; // Mit erster LED beginnen
      dot_count = 0;

      // LED-Farben setzen
      while (dot_count < NUM_LEDS)
      {
        // Farbwert setzen
        leds[dot] = CHSV(hue,255,Hell);

        // Nächste LED
        dot++;

        // Am Ende des LED-Stripes angekommen?
        if (dot >= NUM_LEDS)
        {
          dot = 0;
        }

        // nächsten Farbwert berechnen
        if (delta_led > delta_hue) // Gibt es mehr LEDs als Farbwerte?
        { // Mehr LEDs als Farbwerte
          // Steigungsrechnung mit Hilfsvariable
          led_count += delta_hue;

          // Hue-Wert ggf. anpassen
          if (led_count >= delta_led)
          {
            // Nächster Farbwert
            hue++;

            // Steigungsberechnung
            led_count -= delta_led;
          }
        }
        else // Es gibt weniger LEDS als Farbwerte
        {
          while (hue_count <= delta_hue)
          {
            // Nächster Farbwert
            hue++;

            // Steigerungsberechnung
            hue_count += delta_led;
          }

          // Steigungsrechnung
          hue_count -= delta_hue;
        }

        // LED Zähler inkrementieren
        dot_count++;
      }
      // LEDS anzeigen
      FastLED.show();

      // Verschieben des Regenbogens
      erster_dot++;
      if (erster_dot >= NUM_LEDS)
      {
        erster_dot = 0;
      }

      // LED leuchten
      LedAn = true;

      // 30ms warten
      delay(30);
      break;
        
    case 2:
      break;
  }
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