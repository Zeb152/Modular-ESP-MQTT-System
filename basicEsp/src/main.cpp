#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

// Network Credentials - CHANGE ALL VARIABLES TO YOUR SPECIFICATIONS
const char *ssid = "YOUR_NETWORK_NAME";
const char *password = "YOUR_NETWORK_PASSWORD";
const char *mqtt_server = "MQTT_SERVER_IP";
const int mqtt_port = 1883;

// MQTT Init
WiFiClient espClient;
PubSubClient client(espClient);

// CHANGE THIS TO THE CORRECT TOPIC NAME
const char *topic = "led_control";

// Device state commands (4 Default, but you can add more easily)
enum DeviceState
{
  IDLE,
  TERMINATE,
  MAIN_ACTION,
  SUB_ACTION
};

// Set the current state to IDLE (wait for commands)
DeviceState currentState = IDLE;

/*
DEFINE ANY PINS HERE ---- v ---- v ---- v ----
*/

#define LED 2

/*
DEFINE ANY PINS HERE ---- ^ ---- ^ ---- ^ ----
*/

// METHODS -------------

// v v v  INIT METHODS  v v v

void connectWiFi();
void callback(char *topic, byte *payload, int length);

void setup()
{
  Serial.begin(115200);
  connectWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // v v v ADD ANY PINS HERE v v v
  pinMode(LED, OUTPUT);
}

void connectWiFi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// ^ ^ ^ INIT METHODS ^ ^ ^

// v v v ACTIONS v v v

// NOT a function you can call from another device. This is simply to be able to send a success message back or sensor readings.
void publishResponse(String action)
{
  Serial.print("Finished ");
  Serial.print(action);
  Serial.println(" , returning to IDLE");

  // Publish a message indicating completion of the MAIN ACTION
  if (client.connected())
  {
    client.publish(topic, "COMPLETED");
    Serial.println("Published COMPLETED to MQTT server");
  }
  else
  {
    Serial.println("Failed to publish: MQTT client not connected");
  }
}

// MAIN ACTION EXAMPLE
void main_action()
{
  Serial.println("Beginning MAIN ACTION");

  // v v v Enter action below v v v

  delay(1500);
  digitalWrite(LED, HIGH);
  delay(1500);
  digitalWrite(LED, LOW);
  delay(1500);
  digitalWrite(LED, HIGH);
  delay(1500);
  digitalWrite(LED, LOW);

  // ^ ^ ^ Action is located above ^ ^ ^

  Serial.println("Finished MAIN ACTION, returning to IDLE");

  // Send a success message back
  publishResponse("MAIN_ACTION");

  // After completing the action, go back to the IDLE state and wait for another command.
  currentState = IDLE;
}

// TERMINATE EXAMPLE
void terminate()
{
  Serial.println("Beginning TERMINATE");

  // v v v Enter action below v v v

  // TERMINATE ACTION EXAMPLE
  digitalWrite(LED, LOW);

  // ^ ^ ^ Action is located above ^ ^ ^

  Serial.println("Finished TERMINATE, returning to IDLE");

  // Send a success message back
  publishResponse("TERMINATE");

  // After completing the action, go back to the IDLE state and wait for another command.
  currentState = IDLE;
}

// SUB ACTION EXAMPLE
void sub_action()
{
  Serial.println("Beginning SUB_ACTION");

  // v v v Enter action below v v v

  // TERMINATE ACTION EXAMPLE
  digitalWrite(LED, HIGH);

  // ^ ^ ^ Action is located above ^ ^ ^

  Serial.println("Finished SUB_ACTION, returning to IDLE");

  // Send a success message back
  publishResponse("SUB_ACTION");

  // After completing the action, go back to the IDLE state and wait for another command.
  currentState = IDLE;
}

// ^ ^ ^ ACTIONS ^ ^ ^

// v v v LOOPING v v v

// If device lost the connection with mosquitto
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client"))
    {
      Serial.println("Connected to MQTT broker");
      // Once connected, publish an announcement...
      client.publish("outTopic", "LED Esp32 is connected");
      // ... and resubscribe
      client.subscribe(topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// recieve messages from MQTT broker address and decode payload
void callback(char *topic, byte *payload, int length)
{
  // Recieve the message and decode it
  String message;
  Serial.print("Message arrived here from ");
  Serial.print(topic);
  Serial.print(" -> ") for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Categorize the command and change the state
  if (message == "MAIN_ACTION")
  {
    currentState = MAIN_ACTION;
  }
  if (message == "TERMINATE")
  {
    currentState = TERMINATE;
  }
  if (message == "SUB_ACTION")
  {
    currentState = SUB_ACTION;
  }
}

// Main state machine watchdog
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  // Command state machine: simply change the state of the device and it will run the command.
  // Note: for adding new commands make sure you add a new case with the new function.
  switch (currentState)
  {
  case IDLE:
    // Listen for MQTT Commands
    break;

  case MAIN_ACTION:
    main_action();
    break;

  case TERMINATE:
    terminate();
    break;

  case SUB_ACTION:
    sub_action();
    break;

  // Default: Command not recognized. Continue.
  default:
    Serial.println("");
    break;
  }
}

// ^ ^ ^ LOOPING ^ ^ ^