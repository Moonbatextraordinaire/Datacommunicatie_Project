#include <ESP8266WiFi.h>
#include <espnow.h>

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x40, 0xF5, 0x20, 0x33, 0x8F, 0x30};

uint8_t unlock = D2;
uint8_t lock = D6;

uint8_t GREEN = D4;
uint8_t RED = D5;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
   char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 500;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(D2, INPUT);
  pinMode(D1, INPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {
  if ((millis() - lastTime) > timerDelay && digitalRead(D2) == HIGH) {
    digitalWrite(D4, HIGH);
    // Set values to send
    strcpy(myData.a, "unlocked");
    myData.e = true;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    lastTime = millis();
    delay(400);
    digitalWrite(D4, LOW);
  }

  if ((millis() - lastTime) > timerDelay && digitalRead(D1) == HIGH) {
    digitalWrite(D5, HIGH);
    // Set values to send
    strcpy(myData.a, "locked");
    myData.e = false;

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    lastTime = millis();
    delay(400);
    digitalWrite(D5, LOW);
    
  }

}