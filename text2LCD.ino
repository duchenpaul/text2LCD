#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <ArduinoJson.h>

ESP8266WebServer server(80);

const char* ssid = "Wolfstein";
const char* password =  "++++----";

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);  //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection

    delay(500);
    Serial.println("Waiting to connect...");

  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP

  server.on("/api", handleBody); //Associate the handler function to the path

  server.begin(); //Start the server
  Serial.println("Server listening");

}

void loop() {

  server.handleClient(); //Handling of incoming requests

}

void handleBody() { //Handler for the body path

    if (server.hasArg("plain")== false){ //Check if body received
      server.send(200, "text/plain", "Body not received");
      return;
    }

    String message = "Body received:\n";
       message += server.arg("plain");
       message += "\n";

    server.send(200, "text/plain", "received");
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
      return;
    }

    // Fetch values.
    //
    // Most of the time, you can rely on the implicit casts.
    // In other case, you can do doc["time"].as<long>();
    const char* LINE_1_TEXT = doc["LINE_1_TEXT"];
    const char* LINE_2_TEXT = doc["LINE_2_TEXT"];
    const char* LINE_3_TEXT = doc["LINE_3_TEXT"];
    const char* LINE_4_TEXT = doc["LINE_4_TEXT"];
    bool BACKLIGHT = doc["BACKLIGHT"];

    // Print values.
    Serial.println(LINE_1_TEXT);
    Serial.println(LINE_2_TEXT);
    Serial.println(LINE_3_TEXT);
    Serial.println(LINE_4_TEXT);
    Serial.println(BACKLIGHT);
}