//Exemplo de código da comunicação.
#include <Nextion.h>

// Define os pinos para a UART 2
#define TX_PIN 17  // Pino TX do ESP32 para RX do Nextion
#define RX_PIN 16  // Pino RX do ESP32 para TX do Nextion

// Configuração da comunicação UART para o Nextion
HardwareSerial nextionSerial(2);
Nextion myNextion(nextionSerial, 9600);  // Inicialize o display Nextion na UART 2

// Defina o objeto Nextion para o componente que você quer controlar
NexText t0 = NexText(0, 1, "t0"); // Suponha que "t0" seja um campo de texto na página 0, ID do objeto 1

void setup() {
  // Inicie a comunicação serial para monitoramento e para o Nextion
  Serial.begin(115200);  // UART 0 para monitor serial
  nextionSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // UART 2 para comunicação com Nextion

  myNextion.init(); // Inicialize a biblioteca Nextion
  Serial.println("Iniciando comunicação com o display Nextion");

  // Exemplo: Envie um texto para o campo "t0"
  t0.setText("Olá, Nextion!");
}

void loop() {
  // Verifique se há eventos do Nextion
  myNextion.listen();
}
