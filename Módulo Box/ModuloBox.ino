/*
  Telemetria Veicular - Módulo do Box
  Autor: Miguel Ângelo de Lacerda Silva
  Data: 2025
  Descrição: Recebimento dos dados via rádio LoRa e envio para dashboard em python
  Hardware: Wifi LoRa 32(V3)
  
  MODIFICADO: Incluído o envio dos dados recebidos via LoRa para a Serial em formato JSON.
  REQUISITO: Instalar a biblioteca "ArduinoJson" (versão 6 ou superior).

  LICENÇA: AT+CDKEY=DAF0782EDB4E151F7E377F43FF5148F4 (Inserir no Serial Monitor)
*/

#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <ArduinoJson.h> // Biblioteca para manipulação de JSON
#include <Wire.h>
#include "HT_SSD1306Wire.h"

static SSD1306Wire display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);

String ultimoPacote = "-----";
String statusLoRa = "Aguardando...";
unsigned long ultimoPacoteMillis = 0;
const unsigned long TIMEOUT_LORA = 2000;  // 2s sem receber pacotes 

#define RF_FREQUENCY                                915300000 // Hz
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
  uint16_t freioLoRa;
  uint16_t cvtLoRa;
} TDadosLora;


char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

int16_t rssi, rxSize; //Intensidade do sinal (dBm)
int8_t snr; // Qualidade do sinal (dB)

bool lora_idle = true;

void setup() {
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

    // OLED
    display.init();
    display.clear();
    display.display();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    display.drawString(0, 0, "Modulo BOX");
    display.drawString(0, 12, "Iniciando...");
    display.display();

    RadioEvents.RxDone = OnRxDone;
    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
    
    Serial.println("ESP32 LoRa Receiver - Pronto para receber e enviar JSON via Serial.");
}

void atualizaDisplay() {
    display.clear();

    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    display.drawString(0, 0, "LoRa: " + statusLoRa);

    display.drawString(0, 14,  ultimoPacote);

    display.drawString(0, 32, "RSSI: " + String(rssi) + " dBm");
    display.drawString(0, 46, "SNR:  " + String(snr)  + " dB");

    display.display();
}

void loop() {

    // Inicia modo RX quando o rádio está ocioso
    if (lora_idle) {
        lora_idle = false;
        Radio.Rx(0);
    }

    // Processa interrupções do LoRa
    Radio.IrqProcess();

    // --- DETECÇÃO DE TIMEOUT DO LoRa ---
    // Se ficar mais de 2 segundos sem receber pacote → sem sinal
    if (millis() - ultimoPacoteMillis > TIMEOUT_LORA) {

        statusLoRa = "Sem sinal";
        ultimoPacote = "-----";
        rssi = 0;
        snr = 0;

        atualizaDisplay();

        // evita chamar display muitas vezes
        ultimoPacoteMillis = millis();
    }
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssiParam, int8_t snrParam) {
    rssi = rssiParam;
    snr = snrParam;
    rxSize = size;

    statusLoRa = "Recebendo Pacote";

    const uint8_t expectedSize = 1 + sizeof(TDadosLora);

    if (size == expectedSize) {

        uint8_t teamID = payload[0];
        if (teamID != 15) {
            statusLoRa = "Ignorado";
            atualizaDisplay();
            Radio.Sleep();
            lora_idle = true;
            return;
        }

        TDadosLora dadosRecebidos;
        memcpy(&dadosRecebidos, payload + 1, sizeof(TDadosLora));

        // salva último pacote para o OLED
        ultimoPacote =
            "R" + String(dadosRecebidos.rpmLoRa) +
            " V" + String(dadosRecebidos.velocidadeLoRa) +
            " C" + String(dadosRecebidos.cvtLoRa) +
            " F" + String(dadosRecebidos.freioLoRa) +
            " B" + String(dadosRecebidos.bateriaLoRa);

        // JSON
        StaticJsonDocument<128> doc;

        doc["rpm"]        = dadosRecebidos.rpmLoRa;
        doc["velocidade"] = dadosRecebidos.velocidadeLoRa;
        doc["bateria"]    = dadosRecebidos.bateriaLoRa;
        doc["freio"]      = dadosRecebidos.freioLoRa;
        doc["cvt"]        = dadosRecebidos.cvtLoRa;
        doc["rssi"]       = rssi;
        doc["snr"]        = snr;
        doc["timestamp"]  = millis();

        serializeJson(doc, Serial);
        Serial.println();

    } else {
        statusLoRa = "Tamanho ERR";
        ultimoPacote = "-----";
    }

    ultimoPacoteMillis = millis();
    atualizaDisplay();

    Radio.Sleep();
    lora_idle = true;
}
