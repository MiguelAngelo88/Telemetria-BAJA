#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LoRa.h>

#define OLED_ADDR 0x3C
#define OLED_LINE1 0
#define OLED_LINE2 10
#define OLED_LINE3 20
#define OLED_LINE4 30
#define OLED_LINE5 40
#define OLED_LINE6 50

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Instância do display OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 16);

/* Definições para comunicação com rádio LoRa */
#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18

#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequência */

/* Definições gerais */
#define DEBUG_SERIAL_BAUDRATE    9600

bool init_comunicacao_lora(void);

/* typedefs */
typedef struct __attribute__((__packed__))  
{
  unsigned int contador;
} TDadosLora;

// Configurações de Wi-Fi
const char* ssid = "Redmi 9";
const char* password = "W00ahnes00W";

// Configurações MQTT
const char* mqttServer = "200.129.71.138";
const int mqttPort = 1883;
const char* mqttUser = "leandro_leite:a0ad88";
const char* mqttTopic = "leandro_leite:a0ad88/attrs";

// Cria instâncias para cliente Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis para dados a serem publicados
int value = 0;
float temperatura = 0;

bool init_comunicacao_lora(void)
{
    bool status_init = false;
    Serial.println("[LoRa Receiver] Tentando iniciar comunicacao com o radio LoRa...");
    SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
    LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);
    
    if (!LoRa.begin(BAND)) 
    {
        Serial.println("[LoRa Receiver] Comunicacao com o radio LoRa falhou. Nova tentativa em 1 segundo...");        
        delay(1000);
        status_init = false;
    }
    else
    {
        /* Configura o ganho do receptor LoRa para 20dBm, o maior ganho possível (visando maior alcance possível) */ 
        LoRa.setTxPower(HIGH_GAIN_LORA); 
        Serial.println("[LoRa Receiver] Comunicacao com o radio LoRa ok");
        status_init = true;
    }

    return status_init;
}

// Função para inicializar o display
void setupDisplay() {
  Wire.begin(4, 15); // Inicializa o I2C nos pinos SDA = 4 e SCL = 15

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Display OLED: falha ao iniciar");
    while (1);
  } else {
    Serial.println("Display OLED: inicialização ok");
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

// Função para conectar ao Wi-Fi
void setupWiFi() {
  WiFi.begin(ssid, password);
  display.clearDisplay();
  display.setCursor(0, OLED_LINE1);
  display.println("Conectando ao Wi-Fi...");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, OLED_LINE1);
  display.println("Wi-Fi conectado!");
  display.setCursor(0, OLED_LINE2);
  display.print("IP: ");
  display.println(WiFi.localIP().toString());
  display.display();
}

// Função para conectar ao broker MQTT
void reconnectMQTT() {
  while (!client.connected()) {
    display.clearDisplay();
    display.setCursor(0, OLED_LINE1);
    display.println("Conectando ao MQTT...");
    display.display();

    if (client.connect("ESP32Client", mqttUser, "")) {
      Serial.println("Conectado ao broker MQTT!");
      display.setCursor(0, OLED_LINE2);
      display.println("MQTT conectado!");
      display.display();
    } else {
      Serial.print("Falha na conexão MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setupDisplay(); // Inicializa o display OLED
  setupWiFi();    // Conecta ao Wi-Fi

  client.setServer(mqttServer, mqttPort); // Configura o broker MQTT

  /* Tenta, até obter sucesso, comunicação com o chip LoRa */
  while(init_comunicacao_lora() == false);    
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }

  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  int packet_size = LoRa.parsePacket();
  if (packet_size == sizeof(TDadosLora)) {
    TDadosLora dados_lora;
    Serial.println("[LoRa Receiver] Há dados a serem lidos");

    // Lendo os dados do LoRa
    LoRa.readBytes((char *)&dados_lora, sizeof(TDadosLora));
    Serial.printf("Dados recebidos: contador = %.2d\n", dados_lora.contador);

    // Criando payload para publicação
    char payload[52];
    if (!isnan(dados_lora.contador)) {
        sprintf(payload, "{\"temp\": %.2d}", dados_lora.contador);
    } else {
        sprintf(payload, "{\"temp\": null}");
        Serial.println("Erro: dado do LoRa é inválido (NaN).");
    }

    // Exibindo no display e publicando no broker
    display.clearDisplay();
    display.setCursor(0, OLED_LINE1);
    display.println("Publicando MQTT...");
    display.setCursor(0, OLED_LINE3);
    display.println(payload);
    display.display();

    if (client.publish(mqttTopic, payload)) {
        Serial.println("Mensagem publicada com sucesso");
        display.setCursor(0, OLED_LINE6);
        display.println("Envio OK!");
    } else {
        Serial.println("Falha ao publicar a mensagem");
        display.setCursor(0, OLED_LINE6);
        display.println("Erro no envio!");
    }
    display.display();

    delay(2000);
  }
}