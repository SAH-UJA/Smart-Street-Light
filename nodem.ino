
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>
#include <WidgetRTC.h>
#include <TimeLib.h>
BlynkTimer timer1;
WidgetRTC rtc;
#include <ESP8266WiFi.h>
#define DHTPIN 2 
#define DHTTYPE DHT11  
String apiKey = "*************";     //  Enter your Write API key from ThingSpeak
char auth[] = "***************"; //Enter the Auth code which was send by Blink
const char *ssid =  "ASUS";     // replace with your wifi ssid and wpa2 key
const char *pass =  "**************";
const char* server = "api.thingspeak.com";
SimpleTimer timer;
DHT dht(DHTPIN, DHTTYPE);
float h=0.0;
float t=0.0;
String current_time="12:00:00";
WiFiClient client;
void clockDisplay()
{
  current_time=String(hour())+":"+minute()+":"+second();
  Serial.print("Current time : ");
  Serial.println(current_time);
}
BLYNK_CONNECTED()
{
  rtc.begin();
}
void sendSensor()
{
  // Please don't send more that 10 values per second.
   h = dht.readHumidity();
   t = dht.readTemperature(); 
  if (isnan(h) || isnan(t)) {
  Serial.println("Failed to read from DHT sensor!");
  }
  int fire=digitalRead(5);
  int motion=digitalRead(4);
  int state=digitalRead(0);
  Blynk.virtualWrite(V5, h);  //V5 is for Humidity
  Blynk.virtualWrite(V6, t);  //V6 is for Temperature
  Blynk.virtualWrite(V3, analogRead(A0));  //V3 is for Ambient light
  Blynk.virtualWrite(V4, map(motion,0,1,0,255)); //V4 is for Motion
  Blynk.virtualWrite(V1, digitalRead(0));
//  Blynk.virtualWrite(V4, map(state,0,1,0,255)); //V4 is for Motion
 //  Serial.println(state);
 Serial.println(motion);
  Blynk.virtualWrite(V2, map(fire,0,1,255,0)); //V2 is for Fire
  if(!fire)
    {Blynk.notify("Fire Alarm near Lamp 1"); //notification
  Blynk.email("sahil27ahuja1999@gmail.com","subject fire alarm","fire near Lamp : 1 need immediate action"); //email 
}
int hr=(current_time.substring(0,current_time.indexOf(':'))).toInt();
Serial.println(hr);
if((hr>=19||hr<=5)&&!digitalRead(0))
{Blynk.notify("Lamp 1 failure"); //notification
  Blynk.email("sahil27ahuja1999@gmail.com","subject lamp 1 failed","Check lamp : 1 immediately"); //email 
}
}
/*BLYNK_WRITE(V0) //Button Widget is writing to pin V1
{
  int pinData = param.asInt(); 
  Serial.println(pinData);
}*/
void setup() 
{
       Serial.begin(9600);
       pinMode(5,INPUT);
       pinMode(A0,INPUT);
       delay(10);
       Blynk.begin(auth, ssid, pass);
       dht.begin();
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 timer1.setInterval(10000L, clockDisplay);

}

void loop() 
{
  
                          if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius ");
                             Serial.print("Humidity: ");
                             Serial.print(h);
                             Serial.print(" degrees Celcius ");
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  //delay(10000);
  timer1.run();
  delay(1000);
}