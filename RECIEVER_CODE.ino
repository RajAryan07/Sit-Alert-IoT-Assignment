#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PubSubClient.h>

// Your Wi-Fi and MQTT broker settings
const char* ssid = "RAJ";
const char* password = "12345678";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP8266BridgeNode")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}


void OnDataRecv(uint8_t* mac, uint8_t *incomingData, uint8_t len) {
    char message[len + 1];
    memcpy(message, incomingData, len);
    message[len] = '\0'; 

   
    String msgStr = String(message);
    int separatorIndex = msgStr.indexOf(',');
    int espId = msgStr.substring(3, separatorIndex).toInt();
    String buttonState = msgStr.substring(separatorIndex + 1);

    // Print the button state
    Serial.print("ESP ID: ");
    Serial.print(espId);
    Serial.print(", Button State: ");
    Serial.println(buttonState);

    // Ensure MQTT connection
    if (!client.connected()) {
        reconnectMQTT();
    }

    // Create a topic based on the ID of the sensor node
    String topic = "sensor/node/" + String(espId);

    // Publish the message to the MQTT broker
    client.publish(topic.c_str(), buttonState.c_str());
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    client.setServer(mqtt_server, 1883);

    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();
}
