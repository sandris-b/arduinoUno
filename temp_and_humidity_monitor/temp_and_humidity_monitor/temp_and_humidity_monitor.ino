#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <WiFi.h>

#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 8
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define LCD_CONTRAST 115
#define LCD_CONTRAST_PIN 6
#define LCD_BACKLIGHT_PIN 10
#define BUTTON_PIN 7
#define buzzer 9

//lcd stuff
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int time_passed_since_last_lcd_wake_up = 0;

//button stuff
int buttonState = 0;
int buttonLastState = 0;

//light sensor
int light_sensor = 0;

int menu_state = 0;

int battVolts;

DHT dht(DHTPIN, DHTTYPE);
float last_temp;
float last_hum;
int time_passed_since_last_hum_warning = 0;

void setup() 
{
  Serial.begin(9600);

  //lcd setup
  analogWrite(LCD_CONTRAST_PIN,LCD_CONTRAST);
  lcd.begin(16, 2);
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
  wake_up_lcd_backlight();

  pinMode(BUTTON_PIN, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(buzzer, OUTPUT);

  dht.begin();
}

void loop() 
{  
  check_temp();
  check_light_sensor();
  check_button_press();

  increment_lcd_up_time();

  show_lcd_text();
  delay(300);
}

void check_temp()
{
  time_passed_since_last_hum_warning++;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  last_temp = t;
  last_hum = h;

  if (last_hum > 70 && time_passed_since_last_hum_warning > 200)
  {
    time_passed_since_last_hum_warning = 0;
    lcd.setCursor(0, 0);
    lcd.print("Mitrums                  ");
    lcd.setCursor(0, 1);
    lcd.print("Pa daudz!!!              "); 
    tone(buzzer, 3000, 1000);
    delay(200);
  }
}

void show_lcd_text()
{
  switch(menu_state)
  {   
    case 0:    
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");   
        lcd.print(last_temp);
        lcd.print(" C    ");     
      
        lcd.setCursor(0, 1);
        lcd.print("Hum: ");     
        lcd.print(last_hum);  
        lcd.print(" %      ");     
    break;
    case 1:
        lcd.setCursor(0, 0);
        lcd.print("Light Sensr:");
        lcd.print(light_sensor);
        lcd.print("        ");
      
        lcd.setCursor(0, 1);
        lcd.print("                       ");
    break;   
        case 2:
        battVolts = get_battery_volts();
        lcd.setCursor(0, 0);
        lcd.print("Voltage:");
        
        lcd.print((float)(battVolts) / 100);
        lcd.print("V        ");
      
        lcd.setCursor(0, 1);
        lcd.print("time:");
        lcd.print(time_passed_since_last_lcd_wake_up);
        lcd.print("        ");
    break;   
  }
}

//light sensor
void check_light_sensor()
{
  light_sensor = analogRead(0);
  if (light_sensor < 500) wake_up_lcd_backlight();
}

//check button
void check_button_press()
{
   buttonState = digitalRead(BUTTON_PIN);
   if (buttonState == HIGH && buttonState != buttonLastState) 
   {
      wake_up_lcd_backlight();
      if (digitalRead(LCD_BACKLIGHT_PIN) == HIGH)
      {
        menu_state++;
        if (menu_state > 2) menu_state = 0;
        Serial.println(menu_state);
      }
   }
   buttonLastState = buttonState;
}

//lcd stuff
void wake_up_lcd_backlight()
{
  digitalWrite(LCD_BACKLIGHT_PIN, HIGH);
  time_passed_since_last_lcd_wake_up = 0;
}

void increment_lcd_up_time()
{
  time_passed_since_last_lcd_wake_up++;
  if (time_passed_since_last_lcd_wake_up > 100)
  {
    digitalWrite(LCD_BACKLIGHT_PIN, LOW);  
    menu_state = 0;//reset menu
  }
}

int get_battery_volts(void) // Returns actual value of Vcc (x 100)
    {
        
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
     // For mega boards
     const long InternalReferenceVoltage = 1115L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
        // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR)| (0<<MUX5) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  
#else
     // For 168/328 boards
     const long InternalReferenceVoltage = 1056L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
        // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
      
#endif
     delay(50);  // Let mux settle a little to get a more stable A/D conversion
        // Start a conversion  
     ADCSRA |= _BV( ADSC );
        // Wait for it to complete
     while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
        // Scale the value
     int results = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L; // calculates for straight line value
     return results;
 
    }
