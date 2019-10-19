#include <ESP8266WiFi.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal

#include <ESP8266WebServer.h>

#include <ArduinoJson.h>

#include "LiquidCrystal_I2C.h" // Added library*
 //Set the pins on the I2C chip used for LCD connections
//ADDR,EN,R/W,RS,D4,D5,D6,D7
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7); // 0x27 is the default I2C bus address of the backpack-see article

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define HOSTNAME "ESP8266-OTA-"

ESP8266WebServer server(80);

int RESTART_CYCLE = 1000;
static int restartCount = 0;

// const char* ssid = "Wolfstein";
// const char* password =  "++++----";
void configModeCallback(WiFiManager * myWiFiManager);

void configModeCallback(WiFiManager * myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());

    lcd.setBacklight(HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Entered config mode");
    lcd.setCursor(0, 1);
    lcd.print(myWiFiManager->getConfigPortalSSID());
    lcd.setCursor(0, 2);
    lcd.print(WiFi.softAPIP().toString());

    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());

}

void setup() {

    Serial.begin(115200);

    // Set off LCD module
    lcd.begin(20, 4); // 16 x 2 LCD module
    lcd.setBacklightPin(3, POSITIVE); // BL, BL_POL

    //#################### WIFI manager ############################
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    // Uncomment for testing wifi manager
    // wifiManager.resetSettings();
    wifiManager.setAPCallback(configModeCallback);

    //or use this for auto generated name ESP + ChipID
    wifiManager.autoConnect();

    //Manual Wifi
    //WiFi.begin(WIFI_SSID, WIFI_PWD);
    String hostname(HOSTNAME);
    hostname += String(ESP.getChipId(), HEX);
    // WiFi.hostname(hostname);
    WiFi.hostname("2004LCD");

    // Connecting to WiFi network
    Serial.println();
    Serial.print("Connecting to ");

    // WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("WiFi not connected");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Printing the ESP IP address
    Serial.println(WiFi.localIP());
    //############################################################

    server.on("/api", handleBody); //Associate the handler function to the path

    server.begin(); //Start the server
    Serial.println("Server listening");

    Serial.println(WiFi.localIP().toString() += "/api");
    lcd.setBacklight(HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi connected to:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.SSID());
    lcd.setCursor(0, 2);
    lcd.print("API Address:");
    lcd.setCursor(0, 3);
    lcd.print(WiFi.localIP().toString() += "/api");

}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        server.handleClient(); //Handling of incoming requests
    } else {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("WiFi not connected");
    }
}

void handleBody() { //Handler for the body path

    if (server.hasArg("plain") == false) { //Check if body received
        server.send(200, "text/plain", "Body not received");
        return;
    }

    String message = "Body received:\n";
    message += server.arg("plain");
    message += "\n";

    Serial.println(message);

    // char json[] = {server.arg("plain")};
    String json = server.arg("plain");
    // Deserialize the JSON document
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        server.send(400, "text/plain", "Deserialize json failed");
        return;
    }

    // Fetch values.
    //
    // Most of the time, you can rely on the implicit casts.
    // In other case, you can do doc["time"].as<long>();
    const char * LINE_1_TEXT = doc["LINE_1_TEXT"];
    const char * LINE_2_TEXT = doc["LINE_2_TEXT"];
    const char * LINE_3_TEXT = doc["LINE_3_TEXT"];
    const char * LINE_4_TEXT = doc["LINE_4_TEXT"];
    bool BACKLIGHT = doc["BACKLIGHT"];

    // Print values.
    Serial.println(LINE_1_TEXT);
    Serial.println(LINE_2_TEXT);
    Serial.println(LINE_3_TEXT);
    Serial.println(LINE_4_TEXT);
    Serial.println(BACKLIGHT);

    lcd.setBacklight(BACKLIGHT);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(LINE_1_TEXT);
    lcd.setCursor(0, 1);
    lcd.print(LINE_2_TEXT);
    lcd.setCursor(0, 2);
    lcd.print(LINE_3_TEXT);
    lcd.setCursor(0, 3);
    lcd.print(LINE_4_TEXT);
    server.send(200, "text/plain", "success");

    restartCount += 1;
    Serial.println(RESTART_CYCLE);
    Serial.println(restartCount);
    if (restartCount > RESTART_CYCLE){
            Serial.println("restart");
            ESP.restart();
        }

}