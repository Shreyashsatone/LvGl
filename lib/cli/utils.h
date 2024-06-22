#ifndef DISABLE_CLI
#include "cli.hpp"
#include "storage.hpp"
#include "tft.hpp"

#define SCREENSHOT_TEMP_FILE "/screenshot.raw"

// Capture the screenshot and save it to the SD card
void captureScreenshot(const char* filename, Stream *response) {
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    response->println("Failed to open file for writing");
    return;
  }

  // Allocate memory to store the screen data (1 byte per segment)
  uint8_t* buffer = (uint8_t*)malloc(tft.width() * tft.height() * 2); // 2 bytes per pixel
  if (!buffer) {
    response->println("Failed to allocate memory for buffer");
    file.close();
    return;
  }

  waitScreenRefresh = true;

  // Read the screen data into the buffer using readRect
  tft.readRect(0, 0, tft.width(), tft.height(), (uint16_t*)buffer);

  // Write the buffer data to the file
  for (int y = 0; y < tft.height(); y++) {
    for (int x = 0; x < tft.width(); x++) {
      // Combine the two 8-bit segments into a 16-bit value
      uint8_t highByte = buffer[(y * tft.width() + x) * 2];
      uint8_t lowByte = buffer[(y * tft.width() + x) * 2 + 1];
      uint16_t color = (highByte << 8) | lowByte;
      file.write((const uint8_t*)&color, sizeof(color));
    }
  }

  // Clean up
  free(buffer);
  file.close();
  response->println("Screenshot saved");
  waitScreenRefresh = false;
}

// WiFi client
WiFiClient client;

void captureScreenshot(const char* filename, const char* pc_ip, uint16_t pc_port) {
  if (!client.connect(pc_ip, pc_port)) {
    Serial.println("Connection to server failed");
    return;
  }

  Serial.println("Connected to server");

  File file = SD.open(filename, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    client.stop();
    return;
  }

  // Send the file data to the PC
  while (file.available()) {
    size_t size = 0;
    uint8_t buffer[512];
    size = file.read(buffer, sizeof(buffer));
    if (size > 0) {
      client.write(buffer, size);
    }
  }

  file.close();
  client.stop();
  Serial.println("Screenshot sent over WiFi");
}
#endif
