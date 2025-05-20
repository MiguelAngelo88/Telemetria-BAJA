/*
  Telemetria Veicular - Módulo Central
  Autor: Miguel Ângelo de Lacerda Silva
  Data: 2025
  Descrição:  Módulo responsável por:
  - Receber dados dos sensores via barramento CAN
  - Enviar informações para display TFT
  - Transmitir dados via rádio LoRa para monitoramento remoto
  Hardware: Heltec Wifi LoRa 32(V2)
  Versão: 1.0
*/

#include <CAN.h>
#include <Wire.h>
#include <LoRa.h>
#include <SPI.h>
#include <esp_task_wdt.h>

// Define o timeout do Watchdog Timer em segundos
#define WDT_TIMEOUT_S 3  // Reinicia o ESP32 se o loop principal travar por mais de 3 segundos

/* ----------- IDs CAN --------------*/
const uint8_t CAN_ID_VELOCIDADE = 0x15;
const uint8_t CAN_ID_BATERIA = 0x17;
const uint8_t CAN_ID_FREIO = 0x16;
const uint8_t CAN_ID_CVT = 0x19;
const uint8_t CAN_ID_COMBUSTIVEL = 0x20;
const uint8_t CAN_ID_RPM = 0x18;

/* Definições para comunicação serial com o display*/
const byte rxPin = 16; //rx2
const byte txPin = 17; //tx2
HardwareSerial DisplaySerial(1);

// IDs para comunicação com o display
constexpr uint8_t DISP_ID_VELOCIDADE = 0x61;
constexpr uint8_t DISP_ID_RPM        = 0x62;
constexpr uint8_t DISP_ID_FREIO      = 0x63;
constexpr uint8_t DISP_ID_CVT        = 0x64;
constexpr uint8_t DISP_ID_BATERIA    = 0x65;
constexpr uint8_t DISP_ID_COMBUSTIVEL= 0x66;

// Buffers para pacotes de dados a serem enviados ao display
// Formato do pacote: [0x5a, 0xa5, tamanho, comando, endereço, 0x00, highByte, lowByte]
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
constexpr int TX_POWER   = 15; // Potência de transmissão em dBm
constexpr float BAND     = 915E6; // Frequência de operação do LoRa em Hz (915MHz)

/* Estrutura de dados para transmissão LoRa */
typedef struct __attribute__((__packed__)){
  uint16_t rpmLoRa;
  uint16_t velocidadeLoRa;
  uint16_t combustivelLoRa;
  uint16_t bateriaLoRa;
  uint16_t freioLoRa;
  uint16_t cvtLoRa;
} TDadosLora;

// Instâncias para armazenar dados atuais e anteriores (para comparação)
TDadosLora dados_lora_atual = {0}; // Armazena os valores atuais dos sensores
TDadosLora dados_lora_anterior = {0}; // Armazena os valores anteriores para comparação

/**
 * Configuração inicial do sistema
 * - Inicializa comunicação serial para debug
 * - Configura e inicia o Watchdog Timer
 * - Inicializa comunicação serial com o display
 * - Inicializa o controlador CAN
 * - Inicializa o módulo LoRa
 */
void setup() {
  // Inicializa comunicação serial para debug
  Serial.begin(115200);
  Serial.println("Inicializando Task Watchdog Timer...");

  // Configura o Watchdog Timer para reiniciar o ESP32 se o loop principal travar
  esp_task_wdt_init(WDT_TIMEOUT_S, true); // true para pânico (reset) em timeout
  esp_task_wdt_add(NULL); // Adiciona a tarefa atual (loop) ao WDT
  Serial.println("Task Watchdog Timer inicializado e tarefa atual adicionada.");

  // Inicializa comunicação serial com o display
  DisplaySerial.begin(115200, SERIAL_8N1, rxPin, txPin);

  // Inicializa o controlador CAN
  initializeCAN();

  // Tenta, até obter sucesso, comunicacao com o chip LoRa
  while (init_comunicacao_lora() == false);
}

/**
 * Loop principal do sistema
 * - Alimenta o Watchdog Timer para evitar reset
 * - Verifica e processa pacotes CAN recebidos
 * - Envia dados atualizados via LoRa quando houver mudanças
 */
void loop() {
  // Alimenta o WDT regularmente para evitar o reset
  esp_task_wdt_reset(); 

  // Verifica se há pacotes CAN disponíveis
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    Serial.print("Pacote recebido com ID 0x");
    Serial.println(CAN.packetId(), HEX);

    // Verifica se o pacote não é um pedido remoto (RTR)
    if (!CAN.packetRtr() && packetSize == 2) {
      // Lê os dois bytes do pacote CAN
      int highByte = CAN.read();
      int lowByte = CAN.read();
      unsigned int valor = (highByte << 8) | lowByte; // Combina os bytes

      // Processa o pacote com base no ID CAN
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
      // Trata pacotes inválidos ou de tamanho inesperado
      Serial.printf("Pacote inválido ou de tamanho inesperado. ID: 0x%X, Size: %d\n", CAN.packetId(), packetSize);
      while (CAN.available()) CAN.read(); // Limpa buffer para evitar processamento de dados corrompidos
    }
  }

  // Envia dados via LoRa se houver mudanças
  envia_dados_lora();
}

/**
 * Envia dados via rádio LoRa quando houver mudanças
 * - Compara os dados atuais com os anteriores
 * - Envia pacote apenas se houver alterações, economizando energia
 * - Atualiza os dados anteriores após o envio
 */
void envia_dados_lora() {
  // Verifica se houve mudança nos dados desde o último envio
  if (memcmp(&dados_lora_atual, &dados_lora_anterior, sizeof(TDadosLora)) == 0) {
    return; // Sem mudanças, não envia
  }

  // Inicia a transmissão do pacote LoRa
  LoRa.beginPacket();
  // Envia a estrutura de dados completa como bytes
  LoRa.write((uint8_t*)&dados_lora_atual, sizeof(TDadosLora));
  // Finaliza e transmite o pacote
  LoRa.endPacket();

  // Atualiza os dados anteriores com os valores atuais
  dados_lora_anterior = dados_lora_atual;

  // Log dos dados enviados para debug
  Serial.printf("[LoRa Enviado] vel: %u, freio: %u, bat: %u, rpm: %u, cvt: %u, comb: %u\n",
                dados_lora_atual.velocidadeLoRa,
                dados_lora_atual.freioLoRa,
                dados_lora_atual.bateriaLoRa,
                dados_lora_atual.rpmLoRa,
                dados_lora_atual.cvtLoRa,
                dados_lora_atual.combustivelLoRa);
}

/**
 * Inicializa o controlador CAN
 * - Define os pinos RX e TX para comunicação CAN
 * - Tenta inicializar o controlador com taxa de 500kbps
 * - Repete a tentativa até obter sucesso
 */
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

/**
 * Envia dados para o display TFT via comunicação serial
 * @param id_display Identificador do campo no display a ser atualizado
 * @param valor Valor a ser enviado para o display
 */
void envia_para_display(uint8_t id_display, unsigned int valor) {
  // Cria o pacote de dados no formato esperado pelo display
  unsigned char pacote[8] = { 0x5a, 0xa5, 0x05, 0x82, id_display, 0x00, highByte(valor), lowByte(valor) };
  // Envia o pacote completo para o display
  DisplaySerial.write(pacote, 8);
}

/**
 * Inicializa a comunicação com o módulo LoRa
 * @return true se a inicialização foi bem-sucedida, false caso contrário
 */
bool init_comunicacao_lora(){
  bool status_init = false;
  Serial.println("[LoRa Sender] Tentando iniciar comunicação com o rádio LoRa...");

  // Inicializa a comunicação SPI para o módulo LoRa
  SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_LORA);
  // Define os pinos de controle do módulo LoRa
  LoRa.setPins(SS_LORA, RESET_LORA, LORA_DIO0);

  // Tenta inicializar o módulo LoRa na frequência definida
  if (!LoRa.begin(BAND)) {
    Serial.println("[LoRa Sender] Comunicação com o rádio LoRa falhou. Nova tentativa em 1 segundo...");
    delay(1000);
    status_init = false;
  } else {
    // Configura a potência de transmissão do módulo LoRa
    LoRa.setTxPower(TX_POWER);
    Serial.println("[LoRa Sender] Comunicação com o rádio LoRa ok");
    status_init = true;
  }

  return status_init;
}
