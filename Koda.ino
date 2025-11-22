#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// --- CONFIGURAÇÃO DE REDE ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// --- CONFIGURAÇÃO MQTT (Broker EMQX) ---
const char* mqtt_server = "broker.emqx.io"; 
const int   mqtt_port   = 1883;

// Tópicos
const char* topic_telemetry = "Koda/davi/station1/telemetry";
const char* topic_alerts    = "Koda/davi/station1/alerts";
const char* topic_cmd       = "koda/davi/station1/cmd";

// Hardware
#define DHTPIN     15
#define DHTTYPE    DHT22
#define LDR_PIN    34
#define BUTTON_PIN 4
#define LED_PIN    2
#define BUZZER_PIN 5

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis
bool modoEstudo = false;
unsigned long lastMsg = 0;
int ultimoEstadoBotao = HIGH;
unsigned long lastDebounce = 0;

void beep(int ms) {
  digitalWrite(BUZZER_PIN, HIGH); 
  delay(ms); 
  digitalWrite(BUZZER_PIN, LOW);
}

void setup_wifi() {
  delay(10);
  Serial.println("\n--- INICIANDO ---");
  Serial.print("Conectando Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int c = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    c++;
    if (c > 30) { 
      Serial.println("\nResetando WiFi..."); 
      WiFi.disconnect(); 
      WiFi.begin(ssid, password); 
      c = 0; 
    }
  }

  Serial.println("\nWiFi OK!");
  Serial.print("IP: "); 
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando MQTT...");
    String clientId = "Davi-TCC-Final-Unit1"; 
    
    if (client.connect(clientId.c_str())) {
      Serial.println(" CONECTADO!");
      client.subscribe(topic_cmd);
    } else {
      Serial.print(" Falha rc=");
      Serial.print(client.state());
      Serial.println(" (Tentando novamente em 3s)");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LDR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) { 
    reconnect(); 
  }
  
  client.loop();

  // --- BOTÃO ---
  int leitura = digitalRead(BUTTON_PIN);
  if (leitura != ultimoEstadoBotao && leitura == LOW) {
      modoEstudo = !modoEstudo;
      Serial.print("MODO ESTUDO: "); 
      Serial.println(modoEstudo ? "ON" : "OFF");
      beep(50);
      delay(200); // debounce simples
  }

  ultimoEstadoBotao = leitura;
  digitalWrite(LED_PIN, modoEstudo);

  // --- ENVIO DE TELEMETRIA ---
  if (millis() - lastMsg > 2000) {
    lastMsg = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int lum = analogRead(LDR_PIN);

    if (!isnan(t)) {
      char payload[200];
      snprintf(payload, sizeof(payload), 
        "{\"temp\":%.1f,\"umid\":%.1f,\"lum\":%d,\"mode\":%s}", 
        t, h, lum, modoEstudo ? "ON" : "OFF");
      
      Serial.print("Enviando: "); 
      Serial.println(payload);
      client.publish(topic_telemetry, payload);
    }
  }
}
