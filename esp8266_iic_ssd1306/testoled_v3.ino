#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Define the OLED display width and height
#define SCREEN_WIDTH 128  // For a 128x64 display
#define SCREEN_HEIGHT 64  // For a 128x64 display
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

WiFiClient mqttClient;
PubSubClient client(mqttClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int msgNumber = 0;
const char* ssid = "COFE_0912";
const char* password = "12345678";
//const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_server = "broker.mqtt-dashboard.com";

void setup() {
  int i = 0;
  Serial.begin(115200);

  // initialize the OLED object
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  firstScreen();

  //pinMode(BUILTIN_LED, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
 int flag = 0;
void loop() {
  // Clear the display buffer
 
  if (flag == 0) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("OLED is working!");
    display.display();
    delay(1000);  // Pause for 2 seconds
    flag++;
  }
  if (!client.connected()) reconnect();  // always executes on first loop
  client.loop();                         // returns false of discon, true of connect
  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    ++msgNumber;
    snprintf(msg, MSG_BUFFER_SIZE, "PNG #%ld", msgNumber);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("GTest1", msg);
  }
}

void firstScreen() {
  display.clearDisplay();

  // Set the text cursor position
  display.setCursor(15, 32);

  // Print text to the display buffer
  display.println("Hello, ucGrage89!!");

  // Display the content in the buffer on the OLED
  display.display();
  delay(4000);  // Pause for 2 seconds
}

void setup_wifi() {
  int i = 0;
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");  // We start by connecting to a WiFi network
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    i++;
    delay(500);
    Serial.println();
    Serial.println("WiFi Not connected");
    msgToOLED("WIFI Not Connected");
  }
  msgToOLED("WIFI Connected");
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("Time=");
  Serial.println(i / 2);
  Serial.println("IP address: ");  // 192.168.1.8
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

  char msg[30] = "Recv";

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {  // length = 1 always?
    msg[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  }
  msg[length] = '\0';
  Serial.println("Message Received :");
  Serial.print(msg);
  msgToOLED(msg);
  Serial.println();
  if ((char)payload[0] == '1') {           // on LED if 1 first character
    digitalWrite(BUILTIN_LED, LOW);        // LED on LOW is the voltage level
  } else digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off
}

void reconnect() {
  msgToOLED("MQTT Reconnect");
  while (!client.connected()) {  // Loop until we're reconnected
    Serial.print("MQTT connection...");
    String clientId = "stevensarns";
    //    clientId += String(random(0xffff), HEX);   // Create a random client ID

    if (client.connect(clientId.c_str())) {  // Attempt to connect
      Serial.println("connected");
      client.publish("GTest1", "xxx");  // Once connected, publish
      client.subscribe("GTest1");       // ... and resubscribe
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);  // Wait 5 seconds before retrying
    }
  }
}

void msgToOLED(char* message) {
  display.clearDisplay();

  // Set the text cursor position
  display.setCursor(15, 32);

  // Print text to the display buffer
  display.println(message);

  // Display the content in the buffer on the OLED
  display.display();
  delay(1000);  // Pause for 2 seconds
}
