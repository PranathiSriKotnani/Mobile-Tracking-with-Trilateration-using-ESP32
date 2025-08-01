#include <esp_now.h>
#include <WiFi.h>
#include "esp_wifi.h"

#define TOWER_ID 1   

// Coordinates of towers (meters)
float towerCoords[3][2] = {
  {0.0, 0.0},
  {5.0, 0.0},
  {2.5, 4.33}
};

struct MobilePacket {
  uint8_t mac[6];
  uint8_t id;
} attribute((packed));

#define NUM_SAMPLES 3
float distanceSamples[NUM_SAMPLES];
int sampleCount = 0;

float distances[3] = {0};
bool receivedFrom[3] = {false};

float rssiToDistance(float rssi) {
  const float txPower = -70.0;       // Calibrated RSSI at 1 meter
  const float pathLossExp = 2.8;     // Adjusted for indoor environments

  if (rssi == 0 || rssi < -100 || rssi > -30) return -1;

  float ratio = (txPower - rssi) / (10.0 * pathLossExp);
  float distance = pow(10.0, ratio);

  return distance;
}

void onReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int len) {
  if (len < sizeof(MobilePacket)) return;

  MobilePacket packet;
  memcpy(&packet, data, sizeof(packet));

  int rssi = recvInfo->rx_ctrl->rssi;
  float distance = rssiToDistance((float)rssi);

  if (distance > 0 && distance < 10) {
    distanceSamples[sampleCount++] = distance;
    Serial.printf("\n📶 RSSI %d dBm = %.2f m", rssi, distance);
  } else {
    Serial.printf("\n⚠ Ignored bad RSSI: %d dBm (Distance %.2f)", rssi, distance);
    return;
  }

  if (sampleCount >= NUM_SAMPLES) {
    float sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) sum += distanceSamples[i];
    float avgDistance = sum / NUM_SAMPLES;

    distances[TOWER_ID - 1] = avgDistance;
    receivedFrom[TOWER_ID - 1] = true;

    Serial.printf("\n✅ Tower %d Avg Distance: %.2f m\n", TOWER_ID, avgDistance);

    sampleCount = 0;

    bool all = true;
    for (int i = 0; i < 3; i++) if (!receivedFrom[i]) all = false;

    if (all) {
      float x1 = towerCoords[0][0], y1 = towerCoords[0][1], r1 = distances[0];
      float x2 = towerCoords[1][0], y2 = towerCoords[1][1], r2 = distances[1];
      float x3 = towerCoords[2][0], y3 = towerCoords[2][1], r3 = distances[2];

      float A = 2 * (x2 - x1);
      float B = 2 * (y2 - y1);
      float C = r1 * r1 - r2 * r2 - x1 * x1 + x2 * x2 - y1 * y1 + y2 * y2;
      float D = 2 * (x3 - x2);
      float E = 2 * (y3 - y2);
      float F = r2 * r2 - r3 * r3 - x2 * x2 + x3 * x3 - y2 * y2 + y3 * y3;

      float denom = (A * E - B * D);
      if (abs(denom) > 1e-6) {
        float x = (C * E - F * B) / denom;
        float y = (A * F - C * D) / denom;
        Serial.printf("\n📍 Estimated Position: (%.2f, %.2f) meters\n", x, y);
      } else {
        Serial.println("⚠ Trilateration failed.");
      }

      for (int i = 0; i < 3; i++) receivedFrom[i] = false;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.printf("\n🚀 Tower %d Booting Up...\n", TOWER_ID);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed!");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  float x = towerCoords[TOWER_ID - 1][0];
  float y = towerCoords[TOWER_ID - 1][1];
  Serial.printf("✅ Tower %d Ready @ (%.2f, %.2f)\n", TOWER_ID, x, y);
  Serial.println("====================================");
}

void loop() {
  delay(10);
}
