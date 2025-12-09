/*
  Telemetria Veicular - Módulo do Box
  Autor: Miguel Ângelo de Lacerda Silva
  Data: 2025
  Descrição: Recebimento dos dados via rádio LoRa e envio para dashboard em python
  Hardware: Heltec Wifi LoRa 32(V3)
  
  MODIFICADO: Incluído o envio dos dados recebidos via LoRa para a Serial em formato JSON.
  REQUISITO: Instalar a biblioteca "ArduinoJson" (versão 6 ou superior).

  LICENÇA: AT+CDKEY=DAF0782EDB4E151F7E377F43FF5148F4 (Inserir no Serial Monitor)
*/

#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <ArduinoJson.h> // Biblioteca para manipulação de JSON

#define RF_FREQUENCY                                915000000 // Hz
#define TX_OUTPUT_POWER                             14        // dBm
#define LORA_BANDWIDTH                              0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Payload size

// Estrutura de dados recebida via LoRa
typedef struct __attribute__((__packed__)) {
  uint16_t rpmLoRa;
  uint16_t velocidadeLoRa;
  uint16_t bateriaLoRa;
  uint16_t combustivelLoRa;
  uint16_t freioLoRa;
  uint16_t cvtLoRa;
} TDadosLora;


char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

int16_t rssi, rxSize;
int8_t snr; // Adicionado snr para ser usado na função OnRxDone

bool lora_idle = true;

void setup() {
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    RadioEvents.RxDone = OnRxDone;
    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
    
    Serial.println("ESP32 LoRa Receiver - Pronto para receber e enviar JSON via Serial.");
}

void loop() {
    if (lora_idle) {
        lora_idle = false;
        Radio.Rx(0);
    }
    Radio.IrqProcess();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssiParam, int8_t snrParam) {
    rssi = rssiParam;
    snr = snrParam; // Armazena o SNR
    rxSize = size;

    if (size == sizeof(TDadosLora)) {
        TDadosLora dadosRecebidos;
        // Copia o payload binário para a estrutura
        memcpy(&dadosRecebidos, payload, sizeof(TDadosLora));

        // --- Conversão para JSON e Envio Serial ---
        
        // Calcula o tamanho do buffer JSON necessário.
        // 8 campos (6 da struct + rssi + snr)
        const size_t capacity = JSON_OBJECT_SIZE(8); 
        StaticJsonDocument<capacity> doc;

        // Popula o objeto JSON
        doc["rpm"] = dadosRecebidos.rpmLoRa;
        doc["velocidade"] = dadosRecebidos.velocidadeLoRa;
        doc["bateria"] = dadosRecebidos.bateriaLoRa;
        doc["combustivel"] = dadosRecebidos.combustivelLoRa;
        doc["freio"] = dadosRecebidos.freioLoRa;
        doc["cvt"] = dadosRecebidos.cvtLoRa;
        doc["rssi"] = rssi;
        doc["snr"] = snr;
        doc["timestamp"] = millis(); // Adiciona um timestamp para o dashboard

        // Serializa o objeto JSON para a porta Serial
        // O '\n' (newline) é crucial para que o script Python saiba onde a mensagem termina.
        serializeJson(doc, Serial);
        Serial.println(); // Adiciona a quebra de linha

        // Opcional: Imprime o JSON para debug no Monitor Serial
        // Serial.print("[JSON Enviado] ");
        // serializeJson(doc, Serial);
        // Serial.println();

    } else {
        // Apenas para debug: imprime o erro na serial
        Serial.println("\r\n[LoRa Recebido] Tamanho inesperado de pacote!");
    }

    Radio.Sleep();
    lora_idle = true;
}
