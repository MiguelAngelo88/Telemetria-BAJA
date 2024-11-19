#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>

// Configurações de Wi-Fi
const char* ssid = "Redmi 9";
const char* password = "W00ahnes00W";

// Configurações MQTT
const char* mqttServer = "200.129.71.138";
const int mqttPort = 1883;  // Porta padrão MQTT
const char* mqttUser = "leandro_leite:a0ad88";  // Apenas o usuário, sem senha
const char* mqttTopic = "leandro_leite:a0ad88/attrs";  // Substitua <device_id> pelo ID do dispositivo no Dojot

// Cria instâncias para cliente Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

int value = 0;
float temperatura = 0;

// Função para conectar ao Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Função de callback MQTT (não será usada neste exemplo, mas pode ser útil para receber mensagens)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Função para conectar ao broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT... ");
    if (client.connect("ESP32Client", mqttUser, "")) {
      Serial.println("Conectado com sucesso.");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(". Verifique IP, porta, e credenciais.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  // Configuração de parâmetros adicionais para MQTT
  client.setKeepAlive(30); // Tempo de keep-alive (30 segundos)
}

void loop() {
  // Verifica se o Wi-Fi está conectado
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi desconectado, tentando reconectar...");
    setup_wifi();
  }

  // Reconecta ao broker MQTT se necessário
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Gerando valores aleatórios
  value = rand() % 15;
  temperatura = (rand() % 22) / 7.0;

  // Formata o payload
  char payload[52];
  sprintf(payload, "{\"temp\": %.2f, \"pH\": %d}", temperatura, value);

  // Publica no tópico
  Serial.print("Enviando payload: ");
  Serial.println(payload);

  if (client.publish(mqttTopic, payload)) {
    Serial.println("Mensagem publicada com sucesso.");
  } else {
    Serial.println("Falha ao publicar a mensagem.");
  }

  delay(2000);  // Aguarda 2 segundos antes de enviar novamente
}
