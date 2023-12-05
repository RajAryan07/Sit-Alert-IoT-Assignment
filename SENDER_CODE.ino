#include <ESP8266WiFi.h>
#include <espnow.h>

// Replace with the MAC address of your receiver ESP
uint8_t targetAddress[] = {0x34, 0x94, 0x54, 0x8C, 0xFB, 0xC6};

// Set the ID for each ESP
const int espId = 1; // Change to 2 for the second ESP

// Callback when data is sent
void onDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Data sent to: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print(", Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Success");
  } else {
    Serial.println("Failure");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  pinMode(D2, INPUT_PULLUP);
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(onDataSent);
}

void loop() {
  bool buttonState = digitalRead(D2) == LOW;
  const char *buttonMessage = buttonState ? "Button Pressed" : "Button Released";

  // Include the ESP ID in the message
  String fullMessage = "ID:" + String(espId) + "," + buttonMessage;
  esp_now_send(targetAddress, (uint8_t *)fullMessage.c_str(), fullMessage.length() + 1);

  delay(2000); // Send every 2 seconds
}
