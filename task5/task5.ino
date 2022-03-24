#include <Arduino_FreeRTOS.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>


SoftwareSerial gpsneo(A4, A5); // RX TX
TinyGPSPlus gps;

SoftwareSerial celuler(0, 1); 
char c;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  gpsneo.begin(9600);
  celuler.begin(9600);

  xTaskCreate(TaskAnalogRead, "AnalogRead", 128, NULL, 2, NULL);
  xTaskCreate(TaskGps, "GPS", 128, NULL, 2, NULL);
  xTaskCreate(TaskCeluler, "Celuler", 128, NULL, 2, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void TaskAnalogRead(void *pvParameters){
   int sensorValue = analogRead(A0);
   while(1){
    Serial.println(sensorValue);
    vTaskDelay(1);
   }   
}

void TaskGps(void *pvParameters){
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);
  while(1){
    if(gpsneo.available()){
        if(gps.encode(gpsneo.read())){
          Serial.println("lokasi");
          if(gps.location.isValid()){
            Serial.println(gps.location.lat(), 6);
            Serial.println("-----");
            Serial.println(gps.location.lng(), 6);
            Serial.println("-----");
          }
        } 
    }else {
      Serial.println("not found");
    
    }
    vTaskDelay(1);
  }
  
}


void TaskCeluler(void *pvParameters){
  Serial.println("starting");
  while(1){
    if(celuler.available()){
    c = celuler.read();
    Serial.write(c);
  } else {
    Serial.println("connection fail");
  }
  
  if(Serial.available()){
    c = Serial.read();
    celuler.write(c);
  } else {
    Serial.println("connection fail from Serial");
  }
    vTaskDelay(1);
  }
}
