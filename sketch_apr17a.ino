#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include "config.h"
#include "web_interface.h"

AsyncWebServer server(80);
AsyncEventSource events("/events");

struct Device {
  int id;
  int ledPin;
  int btnPin;
  int lastBtnState;
  int status; // 0=OFF, 1=ON
};

Device devices[] = {
  {1, 16, 12, HIGH, 0}, // Salon
  {2, 17, 14, HIGH, 0}, // Cuisine
  {3, 18, 26, HIGH, 0}, // Chambre
  {4, 19, 27, HIGH, 0}  // Garage
};
const int numDevices = sizeof(devices) / sizeof(devices[0]);

// --- LOGIQUE DE CONTROLE ---

void sendUpdate(Device &d) {
  char buf[64];
  snprintf(buf, sizeof(buf), "{\"id\":\"btn%d\", \"status\":%d}", d.id, d.status);
  events.send(buf, "toggleState", millis());
}

void toggle(Device &d) {
  d.status = !d.status;
  digitalWrite(d.ledPin, d.status ? HIGH : LOW);
  sendUpdate(d);
  Serial.printf("Appareil %d : %s\n", d.id, d.status ? "ALLUME" : "ETEINT");
}

// --- MOTEUR DE RENDU HTML ---

String processor(const String& var) {
  // On parcourt nos appareils pour remplir le HTML
  for(int i=0; i < numDevices; i++) {
    if(var == "btn" + String(devices[i].id) + "txt") {
      return (devices[i].status == 1) ? "OFF" : "ON";
    }
    if(var == "btn" + String(devices[i].id) + "class") {
      return (devices[i].status == 1) ? "btn button2" : "btn button";
    }
  }
  return String();
}

// --- SETUP ---

void setup() {
  Serial.begin(115200);
  
  for(int i=0; i < numDevices; i++) {
    pinMode(devices[i].ledPin, OUTPUT);
    pinMode(devices[i].btnPin, INPUT_PULLUP);
    digitalWrite(devices[i].ledPin, LOW);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nConnecte ! IP: " + WiFi.localIP().toString());

  if (MDNS.begin(dns_name)) {
    Serial.printf("Acces : http://%s.local\n", dns_name);
  }

  // Routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("button_id")) {
      int id = request->getParam("button_id")->value().toInt();
      if(id > 0 && id <= numDevices) toggle(devices[id-1]);
    }
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    for(int i=0; i<numDevices; i++) {
      if(devices[i].status == 1) toggle(devices[i]);
    }
    request->send_P(200, "text/html", index_html, processor);
  });

  server.addHandler(&events);
  server.begin();
  Serial.println("Serveur pret.");
}

// --- LOOP ---

void loop() {
  for(int i=0; i < numDevices; i++) {
    int reading = digitalRead(devices[i].btnPin);
    // Detection de clic (Front descendant)
    if (devices[i].lastBtnState == HIGH && reading == LOW) {
      toggle(devices[i]);
      delay(150); // Debounce un peu plus long pour stabilite
    }
    devices[i].lastBtnState = reading;
  }
}
