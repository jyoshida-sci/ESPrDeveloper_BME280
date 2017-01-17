#include <ESP8266WiFi.h>
#include <Milkcocoa.h>


#include <DHT.h>
#define DHTTYPE DHT22
#define DHT22_PIN 14
DHT dht(DHT22_PIN, DHTTYPE);

#define RELAY_PIN 4


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "___YOUR_SSID___"
#define WLAN_PASS       "___YOUR_PASSWORD___"


/************************* Your Milkcocoa Setup *********************************/

#define MILKCOCOA_APP_ID      "__YOUR_MILKCOCOA_APP_ID__"
#define MILKCOCOA_DATASTORE   "__YOUR_DATASTORE_NAME__"

/************* Milkcocoa Setup (you don't need to change this!) ******************/

#define MILKCOCOA_SERVERPORT  1883

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_APP_ID ".mlkcca.com";
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_APP_ID;

Milkcocoa milkcocoa = Milkcocoa(&client, MQTT_SERVER, MILKCOCOA_SERVERPORT, MILKCOCOA_APP_ID, MQTT_CLIENTID);

void onpush(DataElement *elem) {
  Serial.println("onpush");
  Serial.println(elem->getInt("temp"));
  Serial.println(elem->getInt("humi"));
};

void setupWiFi() {
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println(F("Milkcocoa "));

  setupWiFi();

  Serial.println( milkcocoa.on(MILKCOCOA_DATASTORE, "push", onpush) );
};


char ch[10];
char ct[10];

void loop() {
  milkcocoa.loop();

  float h  = dht.readHumidity() ;
  float t  = 5.0/9.0*(dht.readTemperature(true)-32.0); // fahrenheit -> celsius
  dtostrf(h, 6, 2, ch);
  dtostrf(t, 6, 2, ct);
  
  DataElement elem = DataElement();
  elem.setValue("temp", ct);
  elem.setValue("humi", ch);

  milkcocoa.push(MILKCOCOA_DATASTORE, &elem);
  delay(1000*60*5);//5min
};