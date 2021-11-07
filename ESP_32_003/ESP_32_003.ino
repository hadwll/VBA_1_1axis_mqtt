//integration of sensor and mqtt

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

// specify the connection info
const char* ssid = "*";
const char* password = "*";
const char* mqtt_server = "192.168.1.10";



WiFiClient wificlient;
PubSubClient client(wificlient);

void setup() {
   Serial.begin(9600);
   Initialise_sensor();
   bno.setExtCrystalUse(true);
   Wire.begin();
   setup_wifi();
   client.setServer(mqtt_server, 1883);
   delay(1000);
   
}

void Initialise_sensor()
{
  if(!bno.begin())
  {
    Serial.print("no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  else Serial.println("Sensor Initialised");
}

void setup_wifi() 
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WI-FI connected");
}


void reconnect() 
{
  // Loop until we're reconnected
   while (!client.connected()) 
   {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
      if (client.connect("ESP32Client")) 
      {
       Serial.println("connected");
      // Subscribe
      //client.subscribe("esp32/output");
      } else 
        {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
          // Wait 5 seconds before retrying
          delay(5000);
        }
    
   }
}

void collect_data(){
    sensors_event_t event; 
    bno.getEvent(&event);
    
}

void print_data()
  {
   /* Display the floating point data */
   imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  Serial.print("X: ");
  Serial.print(accel.x());
  Serial.print(" Y: ");
  Serial.print(accel.y());
  Serial.print(" Z: ");
  Serial.print(accel.z());
  Serial.println("");
  delay(100);
  }
  
  // this function changes the float value from the sensor library to a string and then into a char array
void publish_data()
{
   String  x_str;
    char x[50];
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
 
    x_str = String(accel.x()); //converting ftemp (the float variable above) to a string 
    x_str.toCharArray(x, x_str.length() + 1); //packaging up the data to publish to mqtt whoa...
  
 if (client.publish("node1-sd", x))
 Serial.println("published"); 
  delay(100);
}
  


  void loop() {
   
   if (!client.connected()) {
    reconnect();
  }
  collect_data();
  print_data();
  publish_data();
 
  }      
