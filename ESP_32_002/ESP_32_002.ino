#include <WiFi.h>
#include <PubSubClient.h>

// specify the connection info
const char* ssid = "23Gracehill";
const char* password = "scooter4ever";
const char* mqtt_server = "192.168.1.10";

WiFiClient wificlient;
PubSubClient client(wificlient);

void setup() {
   Serial.begin(9600);
   setup_wifi();
   client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  
  }
  Serial.println("WI-FI connected");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      //client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    
   }
}


  void loop() {
   
   if (!client.connected()) {
    reconnect();
  }

  if( client.publish("node1-sd", "test")){
  Serial.println("published");
  delay(1000);  
  }      
  
}
