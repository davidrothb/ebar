#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <jled.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

AsyncWebServer server(80);

#define EBAR_I2C_ADDR 4

char auth[] = "AUTH_TOKEN";

char ssid[] = "WIFI_SSID";
char pass[] = "WIFI_PASS";

auto led_breathe = JLed(LED_BUILTIN).Breathe(800).Forever();

WidgetBridge robot(V127);

String cmdDesc[] = {
  "Command 0 not valid!",
  "Drink 1 request!",
  "Drink 2 request!",
  "Drink 3 request!",
  "Drink 4 request!",
  "Scale tare request",
  "Empty glass weight calibrate request",
  "Full glass weight calibrate request"
  };

void setup()
{
  Serial.begin(115200);

  //Blynk.begin(auth, ssid, pass, "10.0.0.52", 8080);
  Blynk.begin(auth, ssid, pass, "90.176.214.165", 8080);

  Wire.begin();
  
  WebSerial.begin(&server);
    /* Attach Message Callback */
    //WebSerial.msgCallback(recvMsg);
    server.begin();
}

void loop()
{
  Blynk.run();
  led_breathe.Update();
}

BLYNK_CONNECTED(){
  robot.setAuthToken("31JSKmaYbtNMYNYLgFA73_yS7oIHhd8d");
  Serial.print("robot auth token set");
}

BLYNK_WRITE(V0){     //virtual pin for byte requests
  int val = param.asInt();
  if(val > 0 && val < 128){
    Wire.beginTransmission(EBAR_I2C_ADDR);
    Wire.write(val);
    Wire.endTransmission();
    WebSerial.print(cmdDesc[val]);
    WebSerial.print("; sending byte ");
    WebSerial.println(val);
  }
  Blynk.virtualWrite(V0, 0); //reset button to show activity
  Blynk.run();
}

BLYNK_WRITE(V1){     //virtual pin robot commands
  int val = param.asInt();
  if(val > 0 && val < 128){
    sendRobotByte(val);
  }
  Blynk.virtualWrite(V1, 0); //reset button to show activity
  Blynk.run();
}


BLYNK_WRITE(V127){     //request from google
  WebSerial.println("Google assistant request; sending byte 69");
  if(param.asInt() == 1){
    Wire.beginTransmission(EBAR_I2C_ADDR);
    Wire.write(69);
    Wire.endTransmission();
    sendRobotByte(69);
  }
  Blynk.virtualWrite(V127, 0);
  Blynk.run();
}

void sendRobotByte(int value){
  robot.virtualWrite(V0, value);
  WebSerial.print("Sent byte ");
  WebSerial.print(value);
  WebSerial.println(" to robot");
}
