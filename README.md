# 🏎️ Sistema de Telemetria - Projeto BAJA

Este repositório documenta a arquitetura e implementação do sistema de telemetria embarcado no veículo BAJA, com base em microcontroladores ESP32, rede CAN, comunicação LoRa e interface gráfica em display.

---

## 📑 Sumário

- [Arquitetura do Sistema](#arquitetura-do-sistema)
  - [Arquitetura Interna](#arquitetura-interna)
  - [Arquitetura Externa](#arquitetura-externa)
- [Pinouts](#pinouts)
  - [LoRa32 V2](#pinout-lora32-v2)
  - [LoRa32 V3](#pinout-lora32-v3)
  - [ESP32 DEVKIT V1](#pinout-esp32-devkit-v1)
  - [Módulo Frontal](#módulo-frontal)
  - [Módulo Central](#módulo-central)
  - [Módulo Traseiro](#módulo-traseiro)
- [Especificações Técnicas](#especificações-técnicas)
  - [Tabela de Consumo Energético](#tabela-de-consumo-energético)
  - [Especificações das Variáveis](#especificações-das-variáveis)
- [Links Úteis](#links-úteis)
- [Ideias Futuras](#ideias-futuras)

---

## 🧠 Arquitetura do Sistema

### Arquitetura Interna  
![croqui telemetria BAJA](https://github.com/user-attachments/assets/2d39a95c-3ffb-498f-b455-7dad052f3d03)

### Arquitetura Externa  
![Arquitetura externa - telemetria BAJA (1)](https://github.com/user-attachments/assets/0eabbcf2-508f-4049-9948-e3a62e71c3a5)

---

## 🔌 Pinouts

### Pinout LoRa32 V2  
![LoRa32 V2](https://github.com/user-attachments/assets/a2757a7a-5f3b-4c13-9c52-783dc83b022d)

### Pinout ESP32 DEVKIT V1  
![ESP32 DEVKIT V1](https://github.com/user-attachments/assets/86cefb8a-a655-4f22-8672-2a8caeacce06)

### Pinout LoRa32 V3  
![LoRa32 V3](https://github.com/user-attachments/assets/965c292c-7f8e-433a-bdf4-9f70098de5db)

---

## Módulo Frontal  

| Pino do ESP32 | Componente                        |
|---------------|-----------------------------------|
| GPIO4         | CAN TX                            |
| GPIO15        | CAN RX                            |
| GPIO13        | Sensor indutivo (velocidade)      |
| GPIO12        | Sensor de tensão                  |
| GPIO19        | MAX6675 SO                        |
| GPIO23        | MAX6675 CS                        |
| GPIO5         | MAX6675 SCK                       |

---

## Módulo Central  

| Pino do ESP32 | Componente                 |
|---------------|----------------------------|
| GPIO4         | CAN TX                     |
| GPIO15        | CAN RX                     |
| GPIO5         | SCK (LoRa)                 |
| GPIO19        | MISO (LoRa)                |
| GPIO27        | MOSI (LoRa)                |
| GPIO14        | RESET (LoRa)               |
| GPIO18        | SS (LoRa)                  |
| GPIO16        | Display TX                 |
| GPIO17        | Display RX                 |

---

## Módulo Traseiro  

| Pino do ESP32 | Componente                  |
|---------------|-----------------------------|
| GPIO4         | CAN TX                      |
| GPIO15        | CAN RX                      |
| GPIO21        | SDA (MLX90614 - temperatura)|
| GPIO22        | SCL (MLX90614 - temperatura)|
| GPIO13        | Sensor indutivo (RPM)       |

---

## 🔋 Tabela de Consumo Energético  

| Componente               | Módulo   | Tensão | Corrente Máx.  |
|--------------------------|----------|--------|----------------|
| ESP32                    | Frontal  | 5V     | 500 mA         |
| Transceiver SN65HVD230   | Frontal  | 3.3V   | 17 mA          |
| Sensor Indutivo          | Frontal  | 12V    | 300 mA         |
| Sensor de Tensão         | Frontal  | 5V     | 20 mA          |
| Sensor MAX6675           | Frontal  | 5V     | 50 mA          |
| ESP32                    | Traseiro | 5V     | 500 mA         |
| Transceiver SN65HVD230   | Traseiro | 3.3V   | 17 mA          |
| Sensor MLX90614          | Traseiro | 5V     | 25 mA          |
| Sensor Indutivo          | Traseiro | 12V    | 300 mA         |
| ESP32                    | Central  | 5V     | 500 mA         |
| Transceiver SN65HVD230   | Central  | 3.3V   | 17 mA          |
| Display DMG80480C070_03W | Central  | 5V     | 610 mA         |

---

## 📊 Especificações das Variáveis  

| Variável             | ID (CAN / Display) | Faixa            | Bytes | Tipo     |
|----------------------|--------------------|------------------|--------|----------|
| Velocidade           | 0x15 / 0x61        | 0 a 70 km/h      | 1      | `uint8_t`|
| Nível da bateria     | 0x17 / 0x65        | 0 a 100 %        | 1      | `uint8_t`|
| Temperatura do freio | 0x16 / 0x63        | 0 a 500 °C       | 2      | `uint16_t`|
| Temperatura da CVT   | 0x19 / 0x64        | 0 a 300 °C       | 2      | `uint16_t`|
| Nível de combustível | 0x20 / 0x66        | 0 a 100 %        | 1      | `uint8_t`|
| RPM do motor         | 0x18 / 0x62        | 0 a 5000 rpm     | 2      | `uint16_t`|

---

## 🔗 Links Úteis

- 📅 [Cronograma de Atividades (Google Sheets)](https://docs.google.com/spreadsheets/d/1f6BjYHv89a4gd36owJ_0MwVJ5d3ZieXN16XCeML3Rqs/edit?usp=sharing)  
- 📝 [Planejamento do Projeto (Google Docs)](https://docs.google.com/document/d/10cPqrGxGNATz-zq_f9W72cQVOdOiKb-08O4iLef-5Vg/edit?usp=sharing)  
- 📁 [Pasta no Google Drive (documentos e arquivos)](https://drive.google.com/drive/folders/12pf-uwY25VX4iIIwu48RXH_IE0EbaV6g?usp=sharing)

---

## 💡 Ideias Futuras

- Suporte a múltiplas telas no display com seleção via botão ou encoder
- Mapeamento da pista com Visão Computacional e cálculo da melhor trajetória
- Comunicação por rádio entre piloto e box (capacete com microfone e alto-falante)
- Sistema de recuperação de energia
- Controle de tração: detecção de patinação e redistribuição de torque
- Sistema de alertas automáticos no display (falhas, limites, desempenho)

---
