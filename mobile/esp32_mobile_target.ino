#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"

// Packet structure
struct MobilePacket {
  uint8_t mac[6];
  uint8_t id;
} attribute((packed));

uint8_t packetID = 0;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  Serial.begin(115200);
  Serial.println("\n📲 ESP-NOW Mobile Node Initializing...");

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); // Force channel 1

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW initialization failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(broadcastAddress)) {
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("❌ Failed to add broadcast peer");
      return;
    }
  }

  Serial.println("✅ ESP-NOW ready, sending every 1 sec...");
}

void loop() {
  MobilePacket packet;
  WiFi.macAddress(packet.mac);
  packet.id = packetID++;

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));
  if (result == ESP_OK) {
    Serial.printf("📤 Sent packet ID: %d\n", packet.id);
  } else {
    Serial.printf("❌ Failed to send packet ID %d | Error code: %d\n", packet.id, result);
  }

  delay(2000);
}
