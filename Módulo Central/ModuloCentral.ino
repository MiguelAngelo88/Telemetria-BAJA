/*
  Telemetria Veicular - Módulo Central
  Autor: Miguel Ângelo de Lacerda Silva
  Data: 2025
  Descrição: Leitura de dados via CAN, envio para display TFT e transmissão via rádio LoRa.
  Hardware: Heltec Wifi LoRa 32(V2)
*/

#include <CAN.h>
#include <WiFi.h>
#include <Wire.h>
#include <LoRa.h>
#include <SPI.h>
#include <SoftwareSerial.h>

/* ----------- IDs CAN --------------*/
const uint8_t CAN_ID_VELOCIDADE = 0x15;
const uint8_t CAN_ID_BATERIA = 0x17;
const uint8_t CAN_ID_FREIO = 0x16;
const uint8_t CAN_ID_CVT = 0x19;
const uint8_t CAN_ID_COMBUSTIVEL = 0x20;
const uint8_t CAN_ID_RPM = 0x18;

/* Definicoes para comunicação com o display */
const byte rxPin = 16;  //rx2
const byte txPin = 17;  //tx2
SoftwareSerial DisplaySerial(rxPin, txPin);

constexpr uint8_t DISP_ID_VELOCIDADE = 0x61;
constexpr uint8_t DISP_ID_RPM        = 0x62;
constexpr uint8_t DISP_ID_FREIO      = 0x63;
constexpr uint8_t DISP_ID_CVT        = 0x64;
constexpr uint8_t DISP_ID_BATERIA    = 0x65;
constexpr uint8_t DISP_ID_COMBUSTIVEL= 0x66;

unsigned char vel[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_VELOCIDADE, 0x00, 0x00, 0x00 };
unsigned char rpm[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_RPM, 0x00, 0x00, 0x00 };
unsigned char freio[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_FREIO, 0x00, 0x00, 0x00 };
unsigned char cvt[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_CVT, 0x00, 0x00, 0x00 };
unsigned char bat[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_BATERIA, 0x00, 0x00, 0x00 };
unsigned char comb[8] = { 0x5a, 0xa5, 0x05, 0x82, DISP_ID_COMBUSTIVEL, 0x00, 0x00, 0x00 };

/* Definicoes para comunicação com radio LoRa */
constexpr int SCK_LORA   = 5;
constexpr int MISO_LORA  = 19;
constexpr int MOSI_LORA  = 27;
constexpr int RESET_LORA = 14;
constexpr int SS_LORA    = 18;
constexpr int LORA_DIO0  = 26;
constexpr int TX_POWER   = 15; // dBm
constexpr float BAND     = 915E6;

/* Struct de dados LoRa */
typedef struct __attribute__((__packed__)){
  uint16_t rpmLoRa;
  uint16_t velocidadeLoRa;
  uint16_t combustivelLoRa;
  uint16_t bateriaLoRa;
  uint16_t freioLoRa;
  uint16_t cvtLoRa;
} TDadosLora;

TDadosLora dados_lora_atual = {0};
TDadosLora dados_lora_anterior = {0};

void setup() {
  
  Serial.begin(115200);
  DisplaySerial.begin(115200);

  initializeCAN();  // Configura a comunicação CAN

  /* Tenta, até obter sucesso, comunicacao com o chip LoRa */
  while (init_comunicacao_lora() == false);
}

void loop() {
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    Serial.print("Pacote recebido com ID 0x");
    Serial.println(CAN.packetId(), HEX);

    if (!CAN.packetRtr() && packetSize == 2) {
      int highByte = CAN.read();
      int lowByte = CAN.read();
      unsigned int valor = (highByte << 8) | lowByte;

      switch (CAN.packetId()) {
        case CAN_ID_VELOCIDADE:
          Serial.print("Velocidade recebido: ");
          Serial.println(valor);
          envia_para_display(DISP_ID_VELOCIDADE, valor);
          dados_lora_atual.velocidadeLoRa = valor;
          break;

        case CAN_ID_FREIO:
          Serial.print("TempFreio recebido: ");
          Serial.println(valor);
          envia_para_display(DISP_ID_FREIO, valor);
          dados_lora_atual.freioLoRa = valor;
          break;

        case CAN_ID_BATERIA:
          Serial.print("NivelBateria recebido: ");
          Serial.println(valor);
          envia_para_display(DISP_ID_BATERIA, valor);
          dados_lora_atual.bateriaLoRa = valor;
          break;

        case CAN_ID_RPM:
          Serial.print("RPM recebido: ");
          Serial.println(valor);
          envia_para_display(DISP_ID_RPM, valor);
          dados_lora_atual.rpmLoRa = valor;
          break;

        case CAN_ID_CVT:
          Serial.print("tempCVT recebido: ");
          Serial.println(valor);
          envia_para_display(DISP_ID_CVT, valor);
          dados_lora_atual.cvtLoRa = valor;
          break;

        case CAN_ID_COMBUSTIVEL:
          Serial.print("NivelCombustivel recebido: ");
          Serial.println(valor);
          envia_para_display(DISP_ID_COMBUSTIVEL, valor);
          dados_lora_atual.combustivelLoRa = valor;
          break;

        default:
          Serial.println("ID CAN não reconhecido.");
          break;
      }

    } else {
      Serial.printf("Pacote inválido ou de tamanho inesperado. ID: 0x%X, Size: %d\n", CAN.packetId(), packetSize);
      while (CAN.available()) CAN.read(); // Limpa buffer
    }
  }

  envia_dados_lora();
}

void envia_dados_lora() {
  if (memcmp(&dados_lora_atual, &dados_lora_anterior, sizeof(TDadosLora)) == 0) {
    return; // Sem mudanças, não envia
  }

  LoRa.beginPacket();
  LoRa.write((uint8_t*)&dados_lora_atual, sizeof(TDadosLora));
  LoRa.endPacket();

  dados_lora_anterior = dados_lora_atual;

  Serial.printf("[LoRa Enviado] vel: %u, freio: %u, bat: %u, rpm: %u, cvt: %u, comb: %u\n",
                dados_lora_atual.velocidadeLoRa,
                dados_lora_atual.freioLoRa,
                dados_lora_atual.bateriaLoRa,
                dados_lora_atual.rpmLoRa,
                dados_lora_atual.cvtLoRa,
                dados_lora_atual.combustivelLoRa);
}

void initializeCAN() {
  CAN.setPins(15, 4); // Define os pinos para RX e TX do CAN
  Serial.println("Tentando inicializar o controlador CAN...");
  
  // Tenta inicializar o CAN repetidamente até obter sucesso
  while (!CAN.begin(500E3)) {
    Serial.println("Falha ao iniciar o controlador CAN. Tentando novamente em 1 segundo...");
    delay(1000); // Aguarda 1 segundo antes de tentar novamente
  }
  
  Serial.println("Controlador CAN inicializado com sucesso!");
}

void envia_para_display(uint8_t id_display, unsigned int valor) {
  unsigned char pacote[8] = { 0x5a, 0xa5, 0x05, 0x82, id_display, 0x00, highByte(valor), lowByte(valor) };
  DisplaySerial.write(pacote, 8);
}

bool init_comunicacao_lora(){
  bool status_init = false;
  Serial.println("[LoRa Sender] Tentando iniciar comunicação com o rádio LoRa...");

  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_LORA);
  LoRa.setPins(SS_LORA, RESET_LORA, LORA_DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("[LoRa Sender] Comunicação com o rádio LoRa falhou. Nova tentativa em 1 segundo...");
    delay(1000);
    status_init = false;
  } else {
    /* Configura o ganho do transmissor LoRa para 20dBm */
    LoRa.setTxPower(TX_POWER);
    Serial.println("[LoRa Sender] Comunicação com o rádio LoRa ok");
    status_init = true;
  }

  return status_init;
}
