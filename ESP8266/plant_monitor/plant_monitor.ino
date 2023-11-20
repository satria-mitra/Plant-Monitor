#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ezTime.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include "ssd1306.h"

#define DHTTYPE DHT22
uint8_t DHTPin = 12;
uint8_t soilPin = 0;
float Temperature;
float Humidity;
int Moisture = 1;
int sensorVCC = 13;
int blueLED = 2;

DHT dht(DHTPin, DHTTYPE);
String userInput = "";
char msg[50];
char receivedMessage[50]; // Moved to global scope

#include "arduino_secrets.h"

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;
const char* mqtt_server = "mqtt.cetools.org";

ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
Timezone GB;

// Rest of the code remains the same...

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  pinMode(sensorVCC, OUTPUT);
  digitalWrite(sensorVCC, LOW);
  pinMode(blueLED, OUTPUT);
  digitalWrite(blueLED, HIGH);

  Serial.begin(115200);
  delay(100);

  pinMode(DHTPin, INPUT);
  dht.begin();

  startWifi();
  startWebserver();
  syncDate();

  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);
  subscribeToTopic("student/CASA0014/ucfnmut/forLily");

  ssd1306_128x64_i2c_init();
  ssd1306_clearScreen();
  ssd1306_setFixedFont(comic_sans_font24x32_123);

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  if (minuteChanged()) {
    readMoisture();
    sendMQTT();
    Serial.println(GB.dateTime("H:i:s"));
  }
  client.loop();
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_clearScreen();
  ssd1306_printFixedN(0,  24, receivedMessage, STYLE_NORMAL, FONT_SIZE_2X);
  digitalWrite(16, HIGH);
  delay(1000);
  digitalWrite(16, LOW);
}

void subscribeToTopic(const char* mqttTopic) {
  if (!client.connected()) {
    reconnect();
  }

  if (client.subscribe(mqttTopic)) {
    Serial.print("Subscribed to topic: ");
    Serial.println(mqttTopic);
  } else {
    Serial.print("Failed to subscribe to topic: ");
    Serial.println(mqttTopic);
  }
}

void readMoisture() {
  digitalWrite(sensorVCC, HIGH);
  digitalWrite(blueLED, LOW);
  delay(100);
  Moisture = analogRead(soilPin);
  digitalWrite(sensorVCC, LOW);
  digitalWrite(blueLED, HIGH);
  delay(100);
  Serial.print("Moisture: ");
  Serial.println(Moisture);
}

void startWifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void syncDate() {
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());
}

void sendMQTT() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Temperature = dht.readTemperature();
  snprintf (msg, 50, "%.1f", Temperature);
  Serial.print("Publish message for t: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnmut/temperature", msg);

  Humidity = dht.readHumidity();
  snprintf (msg, 50, "%.0f", Humidity);
  Serial.print("Publish message for h: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnmut/humidity", msg);

  snprintf (msg, 50, "%.0i", Moisture);
  Serial.print("Publish message for m: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnmut/moisture", msg);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);
  } else {
    digitalWrite(BUILTIN_LED, HIGH);
  }

  snprintf(receivedMessage, sizeof(receivedMessage), "%.*s", length, (char*)payload);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      client.subscribe("student/CASA0014/plant/ucfnmut/inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void startWebserver() {
  server.on("/", HTTP_GET, handle_OnConnect);
  server.on("/sendMessage", HTTP_GET, handle_SendMessage);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void handle_OnConnect() {
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  server.send(200, "text/html", SendHTML(Temperature, Humidity, Moisture));
}

void handle_SendMessage() {
  userInput = server.arg("message");
  Serial.print("User Input: ");
  Serial.println(userInput);
  
  // Publish userInput to MQTT
  if (client.connected()) {
    client.publish("student/CASA0014/ucfnmut/forLily", userInput.c_str()); // Replace "your_MQTT_topic_here" with your desired MQTT topic
    Serial.println("Message published to MQTT");
  } else {
    Serial.println("Failed to publish message. MQTT not connected.");
  }  

  // Redirect back to the main page
  server.sendHeader("Location", String("/"), true);
  server.send(302, "text/plain", "Message sent to Serial Monitor");

}


void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat, float Humiditystat, int Moisturestat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 DHT22 Report</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Plant Monitor Lily</h1>\n";
  ptr += "<p>Temperature: ";
  ptr += (int)Temperaturestat;
  ptr += " C</p>";
  ptr += "<p>Humidity: ";
  ptr += (int)Humiditystat;
  ptr += "%</p>";
  ptr += "<p>Moisture: ";
  ptr += Moisturestat;
  ptr += "</p>";
  ptr += "<form action=\"/sendMessage\" method=\"GET\">";
  ptr += "  <label for=\"message\">Enter Message:</label><br>";
  ptr += "  <input type=\"text\" id=\"message\" name=\"message\"><br>";
  ptr += "  <button type=\"submit\">Send Message</button>";
  ptr += "</form>";
  ptr += "<p>Date and Time: ";
  ptr += GB.dateTime("l,");
  ptr += "<br>";
  ptr += GB.dateTime("d-M-y H:i:s T");
  ptr += "</p>";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
