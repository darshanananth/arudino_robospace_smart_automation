#include <WiFi.h>
#include <HTTPClient.h>
#include <Keypad.h>
#include <ArduinoJson.h>

#define ROW_NUM     4
#define COLUMN_NUM  4

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM]      = {19, 18, 5, 17}; // GPIO19, GPIO12, GPIO5, GPIO17 connect to the row pins
byte pin_column[COLUMN_NUM] = {27, 4, 0, 2};   // GPIO16, GPIO4, GPIO0, GPIO2 connect to the column pins I changed 16 to 27 for serial 2 i think
int door_pin = 15;
Keypad keypad = Keypad ( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

String input = "";
String rfidData = "";

// Define Wi-Fi credentials
const char* ssid = "Galaxy A14";
const char* password = "galaxy12345";
const char* keypad_pwd = "161203";


const String endpointURL1 = "http://192.168.91.54:5000/entry";
const String endpointURL2 = "http://192.168.91.54:5000/exit";

void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(door_pin, OUTPUT);
  digitalWrite(door_pin, HIGH);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  digitalWrite(door_pin, LOW);
  Serial.println("Connected to WiFi");
}

void loop()
{
  char key = keypad.getKey();

  if (key) {
    if (key == 'D') {
      // 'D' key (enter button) is pressed, do something with the stored input
      Serial.println("Input received: " + input);

      // Clear the input for the next entry
      String tempInput = input; // Store the input in a temporary variable
      input = ""; // Clear the input string

      if (tempInput == keypad_pwd) {
        Serial.println("Keypad valid opening door");
        digitalWrite(door_pin, HIGH);
        delay(15000);
        digitalWrite(door_pin, LOW);
      }
      else {
        HTTPClient http;
        http.begin(endpointURL1); // Specify the endpoint URL
        http.addHeader("Content-Type", "application/json"); // Specify the content type as JSON if needed

        // Example payload for the PUT request
        String payload = "{\"register_no\": \"" + tempInput + "\"}";

        int httpResponseCode = http.POST(payload);
        if (httpResponseCode == HTTP_CODE_OK) {
          Serial.println("HTTP Request Sent Successfully");

          // Read and print the response
          String response = http.getString();
          Serial.println("Response:");
          Serial.println(response);

          // Parse the JSON response
          DynamicJsonDocument jsonDoc(512);
          DeserializationError error = deserializeJson(jsonDoc, response);

          // Check if parsing succeeded
          if (error) {
            Serial.print("JSON parsing error: ");
            Serial.println(error.c_str());
          } else {
            // Access the "code" value
            int code = jsonDoc["code"];

            // Check the value of "code"
            if (code == 200) {
              digitalWrite(door_pin, HIGH);
              delay(15000);
              digitalWrite(door_pin, LOW);
            }
          }
        }
      }
    }
    else if (key == 'C')
    {
      input = "";
    }
    else {
      // Append the pressed key to the input string
      input += key;
    }
  }

  if (Serial.available()) {
    String inputData = Serial.readString();
    inputData.trim();
    rfidData = inputData;
    Serial.println(rfidData);
    
    // Send HTTP request only if there is RFID data
    if (!rfidData.isEmpty()) {
      // Send HTTP request
      input = "";
      HTTPClient http;
      http.begin(endpointURL1); // Specify the endpoint URL
      http.addHeader("Content-Type", "application/json"); // Specify the content type as JSON if needed

      // Example payload for the PUT request
      String payload = "{\"rfid\": \"" + rfidData + "\"}";

      int httpResponseCode = http.POST(payload);
      if (httpResponseCode == HTTP_CODE_OK) {
        Serial.println("HTTP Request Sent Successfully");

        // Read and print the response
        String response = http.getString();
        Serial.println("Response:");
        Serial.println(response);

        // Parse the JSON response
        DynamicJsonDocument jsonDoc(512);
        DeserializationError error = deserializeJson(jsonDoc, response);

        // Check if parsing succeeded
        if (error) {
          Serial.print("JSON parsing error: ");
          Serial.println(error.c_str());
        } else {
          // Access the "code" value
          int code = jsonDoc["code"];

          // Check the value of "code"
          if (code == 200) {
            digitalWrite(door_pin, HIGH);
            delay(15000);
            digitalWrite(door_pin, LOW);
          }
        }
      } else {
        Serial.print("HTTP Error Code: ");
        Serial.println(httpResponseCode);
        // Add code here to handle an error in the request
      }
      http.end();

      // Clear the RFID data after sending the HTTP request
      rfidData = "";
    }
  }
  //For RFID 2 with Serial2
  if (Serial2.available()) {
    String inputData = Serial2.readString(); // Read the incoming data
    inputData.trim(); // Remove any leading/trailing white spaces
    rfidData = inputData;
    Serial.println(rfidData);

    // Send HTTP request only if there is RFID data
    if (!rfidData.isEmpty()) {
      // Send HTTP request
      input = "";
      HTTPClient http;
      http.begin(endpointURL2); // Specify the endpoint URL
      http.addHeader("Content-Type", "application/json"); // Specify the content type as JSON if needed

      // Example payload for the PUT request
      String payload = "{\"rfid\": \"" + rfidData + "\"}";

      int httpResponseCode = http.POST(payload);
      if (httpResponseCode == HTTP_CODE_OK) {
        Serial.println("HTTP Request Sent Successfully");

        // Read and print the response
        String response = http.getString();
        Serial.println("Response:");
        Serial.println(response);

        // Parse the JSON response
        DynamicJsonDocument jsonDoc(512);
        DeserializationError error = deserializeJson(jsonDoc, response);

        // Check if parsing succeeded
        if (error) {
          Serial.print("JSON parsing error: ");
          Serial.println(error.c_str());
        } else {
          // Access the "code" value
          int code = jsonDoc["code"];

          // Check the value of "code"
          if (code == 200) {
            digitalWrite(door_pin, HIGH);
            delay(15000);
            digitalWrite(door_pin, LOW);
          }
        }
      } else {
        Serial.print("HTTP Error Code: ");
        Serial.println(httpResponseCode);
        // Add code here to handle an error in the request
      }
      http.end();

      // Clear the RFID data after sending the HTTP request
      rfidData = "";
    }
  }
}
