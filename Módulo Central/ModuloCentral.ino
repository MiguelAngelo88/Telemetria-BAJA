#include <CAN.h>
#include <WiFi.h>
#include <Wire.h>
#include <LoRa.h>
#include <SPI.h>

/* Definicoes para comunicação com radio LoRa */
#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18

#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequencia */

/* typedefs */
typedef struct __attribute__((__packed__)) {
  unsigned int rpmLoRa;
  unsigned int tempFreio;
} TDadosLora;

/* Variáveis globais */
unsigned int rpm = 0;
unsigned int tempFreio = 0;

void setup() {
  Serial.begin(115200);

  initializeCAN();  // Configura a comunicação CAN

  /* Tenta, até obter sucesso, comunicação com o chip LoRa */
  while (init_comunicacao_lora() == false);
}

void loop() {
  if (recebe_dados_can()) {  // Se dados válidos foram recebidos
    envia_informacoes_lora(rpm, tempFreio);
  }
}

/* Função para inicializar a comunicação CAN */
void initializeCAN() {
  CAN.setPins(15, 4);
  Serial.println("Inicializando o Transmissor CAN");
  if (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN");
  }
}

/* Função para receber e processar dados CAN */
bool recebe_dados_can() {
  int packetSize = CAN.parsePacket();

  if (packetSize) {  // Se um pacote for recebido
    Serial.print("Pacote recebido com ID 0x");
    Serial.println(CAN.packetId(), HEX);

    // Verifica se o pacote contém dados
    if (!CAN.packetRtr() && packetSize == 2) {  // Deve conter exatamente 2 bytes
      if (CAN.packetId() == 0x15) {  // ID para RPM
        int rpmHighByte = CAN.read();
        int rpmLowByte = CAN.read();
        rpm = (rpmHighByte << 8) | rpmLowByte;
        Serial.print("RPM recebido: ");
        Serial.println(rpm);
        return true;
      } else if (CAN.packetId() == 0x16) {  // ID para temperatura do freio
        int tempHighByte = CAN.read();
        int tempLowByte = CAN.read();
        tempFreio = (tempHighByte << 8) | tempLowByte;
        Serial.print("Temperatura do freio recebida: ");
        Serial.println(tempFreio);
        return true;
      }
    } else {
      Serial.println("Pacote com tamanho ou formato inválido.");
    }
  }
  return false;
}

/* Função para enviar informações via LoRa */
void envia_informacoes_lora(unsigned int rpm, unsigned int tempFreio) {
  TDadosLora dados_lora;

  dados_lora.rpmLoRa = rpm;
  dados_lora.tempFreio = tempFreio;

  LoRa.beginPacket();
  LoRa.write((uint8_t*)&dados_lora, sizeof(TDadosLora));
  LoRa.endPacket();

  Serial.print("LoRa enviou - RPM: ");
  Serial.print(rpm);
  Serial.print(", TempFreio: ");
  Serial.println(tempFreio);
}

/* Função para inicializar a comunicação LoRa */
bool init_comunicacao_lora(void) {
  bool status_init = false;
  Serial.println("[LoRa Sender] Tentando iniciar comunicacao com o radio LoRa...");
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
  LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);

  if (!LoRa.begin(BAND)) {
    Serial.println("[LoRa Sender] Comunicacao com o radio LoRa falhou. Nova tentativa em 1 segundo...");
    delay(1000);
    status_init = false;
  } else {
    /* Configura o ganho do receptor LoRa para 20dBm, o maior ganho possível (visando maior alcance possível) */
    LoRa.setTxPower(HIGH_GAIN_LORA);
    Serial.println("[LoRa Sender] Comunicacao com o radio LoRa ok");
    status_init = true;
  }

  return status_init;
}
