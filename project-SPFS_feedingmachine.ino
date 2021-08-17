
// Lib import
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <BlynkSimpleEsp8266.h>

// wifi connection
const char* ssid="...";
const char* password = "...";

// Adafruit 
#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "...."
#define MQTT_PASS "...."

//NTPClient 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

// subcribe Smart Pet Feeder
Adafruit_MQTT_Subscribe SmartPetFeeder = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/spfs");
boolean feed = true; // condition for alarm


// Blynk
#define BLYNK_PRINT Serial
char auth[] = ".....";



// LCD
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Servo
Servo servo;
int SERVO_PIN = 2; //D4   
int CLOSE_ANGLE = 180;  
int OPEN_ANGLE = 90; // The opening angle of the servo motor arm

// time feeding
int  h, hh, mm, ss;
int feed_hour;
int feed_minute;
int feeding_sec;

// week days
String weekDays[7]={"Sun", "Mon", "Tue", "Wed", "Thurs", "Fri", "Sat"};

// feeding count
int feedCount = 0;


void setup()
{

  // Blynk
  Serial.begin(9600);
  Blynk.begin(auth, ssid, password);
 
  
  // LCD setup
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();

  // Servo setup
  servo.attach(SERVO_PIN); //D4
  servo.write(CLOSE_ANGLE);

  //connect wifi
  WiFi.begin(ssid,password);

  //test network connect
  lcd.print("Connecting");

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      lcd.print(".");        
  }
  lcd.clear();
  lcd.print("Connected!!");
  
  //subscribe mqtt
  mqtt.subscribe(&SmartPetFeeder);

  // vietnam time zone
  timeClient.begin();
  timeClient.setTimeOffset(25200);
  

}

void loop()
{
  Blynk.run();


   // time Client
   MQTT_connect();
   timeClient.update();
  
   hh = timeClient.getHours(); // lcd display
   mm = timeClient.getMinutes();
   ss = timeClient.getSeconds();
   h = timeClient.getHours(); // logical hour
   String weekDay = weekDays[timeClient.getDay()];


    
    // reset feeding count
    if( h == 0 && mm == 0){
      feedCount = 0;
      }
   
    // Time  
   lcd.setCursor(0,0);
    if(hh>12){
    hh=hh-12;
    lcd.print(weekDay);
    lcd.print(", ");
    lcd.print(hh);
    lcd.print(":");
    lcd.print(mm);
    lcd.println(" PM  ");
  }
  
  else {
    lcd.print(weekDay);
    lcd.print(", ");
    lcd.print(hh);
    lcd.print(":");
    lcd.print(mm);
    lcd.print(" AM  ");   
  }

 

   if( h == feed_hour && mm == feed_minute ) //Comparing the current time with the Feed time

    { 
      open_feeding();
      delay(1000);
      close_feeding();
      lcd.setCursor(0,1);
     lcd.print("                                 ");

     delay(60000);
    }
  
   
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
     if (subscription == &SmartPetFeeder)
    {
     
    if (!strcmp((char*) SmartPetFeeder.lastread, "1"))
      {
        
        open_feeding();
        delay(1000);
        close_feeding();
        
       }
      if (!strcmp((char*) SmartPetFeeder.lastread, "morning"))
      {
        feed_hour = 9;
        feed_minute = 30;
        feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
           
      }
      if (!strcmp((char*) SmartPetFeeder.lastread, "afternoon"))
      {
        feed_hour = 14;
        feed_minute = 30;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("2");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");   
      }
      if (!strcmp((char*) SmartPetFeeder.lastread, "evening"))
      {
      feed_hour = 17;

          feed_minute = 21;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("4");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM"); 
      }
      // time number
      if (!strcmp((char*) SmartPetFeeder.lastread, "at 12 a . m ."))
      {
        feed_hour = 0;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("12");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
      
      if (!strcmp((char*) SmartPetFeeder.lastread, "at 1 a . m ."))
      {
        feed_hour = 1;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
      }

            if (!strcmp((char*) SmartPetFeeder.lastread, "at 2 a . m ."))
      {
        feed_hour = 2;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 3 a . m ."))
      {
        feed_hour = 3;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 4 a . m ."))
      {
        feed_hour = 4;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 5 a . m ."))
      {
        feed_hour = 5;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 6 a . m ."))
      {
        feed_hour = 6;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 7 a . m ."))
      {
        feed_hour = 7;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 8 a . m ."))
      {
        feed_hour = 8;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 9 a . m ."))
      {
        feed_hour = 9;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 10 a . m ."))
      {
        feed_hour = 10;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 11 a . m ."))
      {
        feed_hour = 11;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" AM");
 
      }
            if (!strcmp((char*) SmartPetFeeder.lastread, "at 12 p . m ."))
      {
        feed_hour = 12;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print(feed_hour);
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
         
      }
                 if (!strcmp((char*) SmartPetFeeder.lastread, "at 13 p . m ."))
      {
        feed_hour = 13;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("1");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }
                   if (!strcmp((char*) SmartPetFeeder.lastread, "at 2 p . m ."))
      {
        feed_hour = 14;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("2");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 3 p . m ."))
      {
        feed_hour = 15;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("3");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 4 p . m ."))
      {
        feed_hour = 16;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("4");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 5 p . m ."))
      {
        feed_hour = 17;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("5");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 6 p . m ."))
      {
        feed_hour = 18;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("6");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 7 p . m ."))
      {
        feed_hour = 19;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("7");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 8 p . m ."))
      {
        feed_hour = 20;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("8");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 9 p . m ."))
      {
        feed_hour = 21;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("9");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 10 p . m ."))
      {
        feed_hour = 22;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("10");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }             if (!strcmp((char*) SmartPetFeeder.lastread, "at 11 p . m ."))
      {
        feed_hour = 23;
        feed_minute = 0;
         feeding_sec = 0;
        lcd.setCursor(0,1);
        lcd.print("Feeding: ");
        lcd.print("11");
        lcd.print(':');
        lcd.print(feed_minute  );
        lcd.print(" PM");
        
      }

     }
  

  }

  
}


// Test Servo
//servo.write(OPEN_ANGLE);
//delay(2000);
//servo.write(CLOSE_ANGLE);
//delay(2000);


void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
}
//
BLYNK_WRITE(V1){ 

  open_feeding();   // Send the command to the servo motor to open the trap door
  delay(1000);
  close_feeding();

  }
  
BLYNK_WRITE(V2){ servo.write(param.asInt());}


void open_feeding(){
  Blynk.notify("Feeding...");
  servo.write(OPEN_ANGLE);   // Send the command to the servo motor to open the trap door
  feedCount++;
   // limit feeding time
  if (feedCount == 3)
  {
     
   
      String body = String("Automatic Message From SPFS: Feeding three time today (Last feeding at ") + hh + ":" + mm  + " o'clock)";
      Blynk.email("SPFS Message", body);

      

 }
  else if (feedCount > 3)
    {
      char num[3];
      itoa(feedCount, num, 10);
      String body = String("Automatic Message From SPFS: Overfeeding today at ") + hh + ":" + mm  + "o'clock (" + num + " times of feeding.)";
      Blynk.email("SPFS Message", body);
      
                  return;
    }
  
}

void close_feeding(){servo.write(CLOSE_ANGLE); }  // Send te command to the servo motor to close the trap door}
