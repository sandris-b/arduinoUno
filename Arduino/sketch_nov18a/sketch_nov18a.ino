#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 13
#define DHTTYPE DHT22
#define RELAYPIN 10

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DHT dht = DHT(DHTPIN, DHTTYPE);

void setup() {
    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(RELAYPIN, HIGH);
    lcd.begin(16, 2);
    dht.begin();
    Serial.begin(9600);
}

void loop() {

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    lcd.setCursor(0, 0);  
    lcd.print(h);
    lcd.print(" mitrums%  ");
    lcd.setCursor(0, 1);  
    lcd.print(t);
    lcd.print(" C     ");

    if (t > 0.00 && t <= 18.00)
    {    


        digitalWrite(RELAYPIN, LOW);

        Serial.print(t); 
        Serial.println(" C -> Heater on!"); 

    }    
    else
    {

        digitalWrite(RELAYPIN, HIGH); 
        Serial.print(t); 
        Serial.println(" C -> Heater off!"); 
    }
    delay(1 5000);  
}
