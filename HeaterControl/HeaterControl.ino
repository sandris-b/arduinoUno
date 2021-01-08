#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial
#define DHTPIN D3
#define DHTTYPE DHT22
#define RELAYPIN D2

char auth[] = "xxx";
char ssid[] = "xxx";
char pass[] = "xxx";

DHT dht = DHT(DHTPIN, DHTTYPE);

float t = 0.0;
float h = 0.0;
bool IsHeaterOn = false;
int temp;

 BLYNK_WRITE(V8)  {
    if (param.asInt()){
      temp = param.asInt();
    }
    else {
      temp = 5;
    }
    Blynk.virtualWrite(V4, String("Temperature set to : ") + temp + String(" C"));
  }
WidgetLED OnOffLed(V7);

void setup() 
{
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, HIGH);
  dht.begin();
  Blynk.begin(auth, ssid, pass);  
  Serial.begin(9600);
  Blynk.virtualWrite(V4, "clr");//clear console
}

void loop()
{
  Blynk.run(); 
  GetTemperature();
  AdjustHeater();
  ShowResults();
  delay(1000); 
}

void GetTemperature() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(t) || isnan(h))
  {
    t = 0;
    h = 0;
    Blynk.virtualWrite(V4, "failed to read from DHT sensor!");
    if (Serial.available()) {
    Serial.println("failed to read from DHT sensor!");
    }
    IsHeaterOn = false;
  }
}

void AdjustHeater() {
    //turn off compleatly
    if (t > temp && IsHeaterOn == true)
    {
        IsHeaterOn = false;
        Blynk.virtualWrite(V4, "Turning heater OFF!");
    }

    if (t > temp) {
        IsHeaterOn = false;        
    }

    //turn on at 15 deg        
    if (IsHeaterOn == false && t < temp - 0.5)
    {     
      IsHeaterOn = true;
      Blynk.virtualWrite(V4, "Turning heater ON!");
    }

    if (IsHeaterOn == true)
    {
      digitalWrite(RELAYPIN, LOW);
      OnOffLed.on();
      if (Serial.available()) {
      Serial.print(t); 
      Serial.println(" C -> Heater on!");
      }
    }
    else
    {
      digitalWrite(RELAYPIN, HIGH);
      OnOffLed.off();
      if (Serial.available()) {
      Serial.print(t); 
      Serial.println(" C -> Heater off!");
      }
    }
}

void ShowResults() {
     if (Serial.available()) {
     Serial.println(t);
     Serial.println(h);
     }
     
     Blynk.virtualWrite(V5, t);
     Blynk.virtualWrite(V6, h);   
}
