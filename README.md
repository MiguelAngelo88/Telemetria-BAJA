# Arquitetura Interna
![croqui telemetria BAJA](https://github.com/user-attachments/assets/2d39a95c-3ffb-498f-b455-7dad052f3d03)

# Arquitetura Externa
![Arquitetura externa - telemetria BAJA (1)](https://github.com/user-attachments/assets/0eabbcf2-508f-4049-9948-e3a62e71c3a5)

# Pinout Lora32 V2
![image](https://github.com/user-attachments/assets/a2757a7a-5f3b-4c13-9c52-783dc83b022d)

# Pinout ESP32 DEVKIT V1
![image](https://github.com/user-attachments/assets/86cefb8a-a655-4f22-8672-2a8caeacce06)

# Pinout Lora32 V3
![image](https://github.com/user-attachments/assets/965c292c-7f8e-433a-bdf4-9f70098de5db)

# Cronograma de Atividades
https://docs.google.com/spreadsheets/d/1f6BjYHv89a4gd36owJ_0MwVJ5d3ZieXN16XCeML3Rqs/edit?usp=sharing

# Link do Planejamento
https://docs.google.com/document/d/10cPqrGxGNATz-zq_f9W72cQVOdOiKb-08O4iLef-5Vg/edit?usp=sharing

# Pasta do Google Drive
https://drive.google.com/drive/folders/12pf-uwY25VX4iIIwu48RXH_IE0EbaV6g?usp=sharing
  
# Módulo frontal pinout  
| Pino do ESP32  | Componente |
| ------------- | ------------- |
| GPIO4  | CAN TX  |
| GPIO15  | CAN RX  |
| GPIO13  | Sensor Indutivo da velocidade |
| GPIO12  | Sensor de tensão  |
| GPIO19  | MAX6675 SO |
| GPIO23  | MAX6675 CS  |
| GPIO5  | MAX6675 SCK  |

# Módulo central pinout  
| Pino do ESP32  | Componente |
| ------------- | ------------- |
| GPIO4  | CAN TX  |
| GPIO15  | CAN RX  |
| GPIO5  | SCK_LORA  |
| GPIO19  | MISO_LORA  |
| GPIO27  | MOSI_LORA  |
| GPIO14  | RESET_PIN_LORA  |
| GPIO18  | SS_PIN_LORA  |
| GPIO16  | DISPLAY TX  |
| GPIO17  | DISPLAY RX  |

# Módulo traseiro pinout  
| Pino do ESP32  | Componente |
| ------------- | ------------- |
| GPIO4  | CAN TX  |
| GPIO15  | CAN RX  |
| GPIO21  | MLX90614 SDA|
| GPIO22  |  MLX90614 SCL |
| GPIO13  | Sensor Indutivo do RPM |

# Tabela de consumo energético 
| Componente | Módulo | Tensão de operação | Corrente máxima |
| ------------- | ------------- | ------------- | ------------- |
| Esp32 | Frontal | 5V | 500mA|
| Transceiver sn65hvd230 | Frontal | 3.3V | 17mA|
| Sensor Indutivo | Frontal | 12V | 300mA |
| Sensor de tensão | Frontal | 5V | 20mA |
| Sensor MAX6675 | Frontal | 5V | 50mA |
| Esp32 | Traseiro | 5V | 500mA|
| Transceiver sn65hvd230 | Traseiro | 3.3V | 17mA|
| Sensor MLX90614 | Traseiro | 5V | 25mA |
| Sensor Indutivo | Traseiro | 12V | 300mA |
| Esp32 | Central | 5V | 500mA|
| Transceiver sn65hvd230 | Central | 3.3V | 17mA|
| Display DMG80480C070_03W | Central | 5V | 610mA|

# Especificações das variáveis 
| Variável | ID | Faixa | 	Bytes Necessários | Tipo |
| ------------- | ------------- | ------------- | ------------- | ------------- |
| Velocidade | 0x15(CAN) e 0x61(DISPLAY) | 	0 a 70 km/h | 1 byte | uint8_t |
| Nível da bateria | 0x17(CAN) e 0x65(DISPLAY) | 	0 a 100% | 1 byte | uint8_t |
| Temperatura do freio | 0x16(CAN) e 0x63(DISPLAY) | 	0 a 500°C | 2 bytes | uint16_t |
| Temperatura da CVT | 0x19(CAN) e 0x64(DISPLAY) | 	0 a 300°C | 2 bytes | uint16_t |
| Nível de combustível | 0x20(CAN) e 0x66(DISPLAY) | 	0 a 100% | 1 byte | uint8_t |
| RPM do motor | 0x18(CAN) e 0x62(DISPLAY) | 	0 a 5000 rpm | 2 bytes | uint16_t |

# Ideias futuras
- Múltiplas cenas no Display, controladas via botão ou encoder
- Mapeamento da pista e cálculo da melhor trajetória utilizando Visão Computacional
- Comunicação via radio frequência entre o piloto e o box (capacete com mic e speaker)
- Sistema de Recuperação de Energia para otimizar o consumo
- Implementar um Controle de Tração para detectar patinação e ajustar a distribuição de potência nas rodas
- Sistema de alertas automáticos no display
