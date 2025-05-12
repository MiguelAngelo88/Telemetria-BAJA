/*
  Telemetria Veicular - Módulo do Box
  Autor: Miguel Ângelo de Lacerda Silva
  Data: 2025
  Descrição: Recebimento dos dados via rádio LoRa e envio para dashboard em python
*/

#include "LoRaWan_APP.h"
#include "Arduino.h"

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

typedef struct __attribute__((__packed__)) {
  uint16_t rpmLoRa;
  uint16_t velocidadeLoRa;
  uint16_t combustivelLoRa;
  uint16_t bateriaLoRa;
  uint16_t freioLoRa;
  uint16_t cvtLoRa;
} TDadosLora;


char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

int16_t rssi, rxSize;

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
}

void loop() {
    if (lora_idle) {
        lora_idle = false;
        Serial.println("Into RX mode");
        Radio.Rx(0);
    }
    Radio.IrqProcess();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssiParam, int8_t snr) {
    rssi = rssiParam;
    rxSize = size;

    if (size == sizeof(TDadosLora)) {
        TDadosLora dadosRecebidos;
        memcpy(&dadosRecebidos, payload, sizeof(TDadosLora));

        Serial.printf("\r\n[LoRa Recebido] vel: %u | freio: %u | bat: %u | rpm: %u | cvt: %u | comb: %u | RSSI: %d | Size: %d\r\n",
                      dadosRecebidos.velocidadeLoRa,
                      dadosRecebidos.freioLoRa,
                      dadosRecebidos.bateriaLoRa,
                      dadosRecebidos.rpmLoRa,
                      dadosRecebidos.cvtLoRa,
                      dadosRecebidos.combustivelLoRa,
                      rssi, rxSize);
    } else {
        Serial.println("\r\n[LoRa Recebido] Tamanho inesperado de pacote!");
    }

    Radio.Sleep();
    lora_idle = true;
}
