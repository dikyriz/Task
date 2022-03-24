#include <ThingsBoard.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Sempol";
const char* password = "sodaGembirah";
const char* mqtt_server = "demo.thingsboard.io";
const int port = 1883;

String topic = "v1/devices/me/telemetry";

#define TOKEN               "DEMO_TOKEN"

WiFiClient espClient;
PubSubClient client(espClient);
ThingsBoard tb(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temperature_sens_read();
#ifdef __cplusplus
}
#endif

int sensors = 6;
String dataSensor = "";

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to Thingsboard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Arduino Uno Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
  pinMode(sensors, OUTPUT);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(mqtt_server);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(mqtt_server, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  Serial.println("Sending data...");

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    getAndSendTemperatureAndHumidityData();
    lastMsg = now;
  }
}

void getAndSendTemperatureAndHumidityData()
{

  if(sensors == 1) {
    Serial.println("ada");
    dataSensor = "ada";
  } else {
    Serial.println("tidak");
    dataSensor = "tidak";
  }
  
  Serial.println("Collecting temperature data.");
    float temp = (temperature_sens_read() - 32 / 1.8);

  Serial.print("Temperature: ");
//  Serial.print(t);
  Serial.print(" *C ");

  String temperature = String(temp);


  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
//  Serial.print( temperature ); Serial.print( "," );
//  Serial.print( humidity );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"Sensors\":"; payload += dataSensor;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish("v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
}
