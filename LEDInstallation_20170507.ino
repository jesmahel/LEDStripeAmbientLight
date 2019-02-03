#include <FastLED.h>
#define NUM_LEDS 28
//#define LED_ECKE = 12

//----  Digital out
#define LED_PIN 11

//----  Analog in
#define POTI 0

//---- Digital in
#define EINAUS 12
#define ZURUECK 5
#define VOR 4

//---- Light states
#define ON true
#define OFF false

#define MINHELL 30   //Mindesthelligkeit
#define MAXPROG 3    //Höchstes Programm

//-----------------------------------------------------------------

//----   LED Stripe initialisieren
CRGB leds[NUM_LEDS];

// Variable zur Programmsteuerung
int Hell = 80;
int OldHell = 80;

boolean LichtAn = OFF;
boolean OldLichtAn = OFF;
boolean LedAn = OFF;

int prog = 0; // aus, wenn Platine startet
int OldProg = 1; // Vorheriges Programm Regenbogen

int erster_dot = 0; // Erste LED im Durchlauf
int dot_counter = 0; // 

int del = 200; // ms Verzögerung

// Berechnung der Linearen Werte wenn NUM_LEDS != 255;
int hue;
int hue_start = 0; // immer bei 0 beginnen in der Farbskala
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
  pinMode(LED_PIN, OUTPUT); // LED_PIN als Ausgang definieren
  pinMode(EINAUS, INPUT);
  pinMode(ZURUECK, INPUT);
  pinMode(VOR, INPUT);

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
    if (Vor != OldVor && Vor == HIGH && prog != 0)
    {
      // Programm eins vorschalten
      prog++;

      if (prog > MAXPROG)
      {
        // wenn am letzten Programm angekommen wieder vorne beginnen
        prog = 1;
      }

      //warten
      delay(del);
    }


    // Programm zurückschalten
    Zurueck = digitalRead(ZURUECK);
    if (Zurueck != OldZurueck && Zurueck == HIGH && prog != 0)
    {
      // Programm eins vorschalten
      prog--;

      if (prog < 1)
      {
        // wenn am letzten Programm angekommen wieder vorne beginnen
        prog = MAXPROG;
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
      //warten
      delay(del);
    }

    // In alt status übernehmen
    OldEinAus = EinAus;
    
    // Lampe ein/ausschalten
    if (OldLichtAn != LichtAn)
    {
        if (LichtAn == OFF)
        {
          OldProg = prog;
          prog = 0;
        }
        else
        {
          prog = OldProg;
        }
        OldLichtAn = LichtAn;
    }

    //int zurueck = digitalRead(ZURUECK); // Zurück Schalter auslesen
    //int vor = digitalRead(VOR);         // Vor Schalter auslesen
    
  //--------  Jetzt LEDs setzen
  switch(prog)
  {
    case 0:  // ausschalten
      if (LedAn)
      {
        FastLED.showColor(CRGB(0,0,0));
      }
      LedAn = false;

      //200 ms warten
      delay(200);
      break;
      
    case 1:  // Programm 1

      // Poti steuert Helligkeit
      
      // LEDS setzen
      hue = hue_start; // Mit Start Hue Wert beginnen
      dot = erster_dot; // Mit erster LED beginnen
      dot_count = 0;
      delta_led = NUM_LEDS; // alle Farben auf alle LED verteilen

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
      delay(300);
      break;
        
    case 2:

      // Poti steuert Helligkeit
      
      // LEDS setzen
      hue = hue_start; // Mit Start Hue Wert beginnen
      dot = erster_dot; // Mit erster LED beginnen
      dot_count = 0;
      delta_led = NUM_LEDS; // alle Farben auf alle LED verteilen

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
      erster_dot--;
      if (erster_dot < 0)
      {
        erster_dot = NUM_LEDS-1;
      }

      // LED leuchten
      LedAn = true;

      // 30ms warten
      delay(300);
      break;

    case 3:     

      // Poti steuert Helligkeit
      
      // LEDS setzen
      hue = hue_start; // Mit Start Hue Wert beginnen
      dot = erster_dot; // Mit erster LED beginnen
      dot_count = 0;
      delta_led = NUM_LEDS/2; // alle Farben auf die Hälfte der LED verteilen

      // LED-Farben setzen
      while (dot_count < NUM_LEDS/2)
      {
        // Farbwert setzen
        leds[dot] = CHSV(hue,255,Hell);
        leds[NUM_LEDS-1-dot] = CHSV(hue,255,Hell);

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
      delay(300);
      break;

    case 4:
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