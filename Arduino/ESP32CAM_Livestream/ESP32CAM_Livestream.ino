#include "esp_camera.h"
#include "base64.h"
#include <HTTPClient.h>
#include <WiFi.h>
// Replace with your camera model and pins
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"


// Replace with your network credentials
const char* ssid = "yourwifiname";
const char* password = "yourwifipassword";
const String yourKey = "khoemach";
const String yourServerUrl = "<your server address>/api/upload";


void setup() {
  delay(500);
  //Serial.setDebugOutput(true);
  Serial.begin(115200);

  showHardwareInfor();
  psramInit();
  Serial.println((String)"Memory available in PSRAM : " +ESP.getFreePsram());

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config); 
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void loop() {
  // Capture photo
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Convert to base64
  String base64_image = base64::encode((uint8_t *)fb->buf, fb->len);

  // Create JSON body
  String body = "{\"apiKey\":\"" +yourKey+"\",\"image\":\"" + base64_image + "\"}";

  // Send POST request
  HTTPClient http;
  http.begin(yourServerUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(body);

  // If the POST request was successful, httpResponseCode will be 200
  if(httpResponseCode>0){
    String response = http.getString();   //Get the response to the request
    Serial.println(httpResponseCode);   //Print return code
    //Serial.println(response);           //Print request answer
  }else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end();  //Free resources

  // Return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);

  //delay(10);
}

void showHardwareInfor(){
    Serial.setDebugOutput(true);
    // Serial.begin(115200);

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    log_i("This is ESP32 chip with %d CPU cores, WiFi%s%s",
          chip_info.cores,
          (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
          (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    log_i("embedded PSRAM: %s", (chip_info.features & CHIP_FEATURE_EMB_PSRAM) ? "yes" : "no");

    log_i("%.2f iMB psram", ESP.getPsramSize() / (1024.0 * 1024.0));

    log_i("silicon revision %d", ESP.getChipRevision());

    log_i("%dMB %s flash", spi_flash_get_chip_size() / (1024 * 1024),
          (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}
