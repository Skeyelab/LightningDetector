# Heltec LoRa V3 - Complete Pin Mapping

This document provides the complete pin mapping for the Heltec LoRa V3 based on the official datasheet and header pinouts.

## Board Overview

- **Board**: Heltec LoRa V3
- **MCU**: ESP32-S3
- **Display**: SSD1306 OLED (128x64)
- **Radio**: SX1262 LoRa
- **Headers**: J2 (Power + Signal), J3 (GPIO + Peripherals)

## Header J2 - Power and Signal

| Pin | Name | Type | Functions |
|-----|------|------|-----------|
| 1   | GND  | P    | Ground |
| 2   | 5V   | P    | 5V Power Supply |
| 3   | Ve   | P    | 3.3V Output, Power for external sensor |
| 4   | Ve   | P    | 3.3V Output, Power for external sensor |
| 5   | RX   | I/O  | GPIO44, U0RXD, Connected to CP2102 TXD |
| 6   | TX   | I/O  | GPIO43, U0TXD, Connected to CP2102 RXD |
| 7   | RST  | I    | CHIP_PU, Connected to RST switch |
| 8   | 0    | I/O  | GPIO0, Connected to PRG switch |
| 9   | 36   | I/O  | GPIO36, SPIIO7, FSPICLK, SUBSPICLK, Vext Ctrl |
| 10  | 35   | I/O  | GPIO35, SPIIO6, FSPID, SUBSPID, LED Write Ctrl |
| 11  | 34   | I/O  | GPIO34, SPIIO5, FSPICS0, SUBSPICS0 |
| 12  | 33   | I/O  | GPIO33, SPIIO4, FSPIHD, SUBSPIHD |
| 13  | 47   | I/O  | GPIO47, SPICLK_P_DIFF, SUBSPICLK_P_DIFF |
| 14  | 48   | I/O  | GPIO48, SPICLK_N_DIFF, SUBSPICLK_N_DIFF |
| 15  | 26   | I/O  | GPIO26, SPICS1 |
| 16  | 21   | I/O  | GPIO21, OLED_RST |
| 17  | 20   | I/O  | GPIO20, U1CTS, ADC2_CH9, CLK_OUT1, USB_D+ |
| 18  | 19   | I/O  | GPIO19, U1RTS, ADC2_CH8, CLK_OUT2, USB_D- |

## Header J3 - GPIO and Peripherals

| Pin | Name | Type | Functions |
|-----|------|------|-----------|
| 1   | GND  | P    | Ground |
| 2   | 3V3  | P    | 3.3V Power Supply |
| 3   | 3V3  | P    | 3.3V Power Supply |
| 4   | 37   | I/O  | GPIO37, SPIDQS, FSPIQ, SUBSPIQ |
| 5   | 46   | I/O  | GPIO46 |
| 6   | 45   | I/O  | GPIO45 |
| 7   | 42   | I/O  | GPIO42, MTMS |
| 8   | 41   | I/O  | GPIO41, MTDI |
| 9   | 40   | I/O  | GPIO40, MTDO |
| 10  | 39   | I/O  | GPIO39, MTCK |
| 11  | 38   | I/O  | GPIO38, FSPIWP, SUBSPIWP |
| 12  | 1    | I/O  | GPIO1, ADC1_CH0, TOUCH1, Read VBAT Voltage |
| 13  | 2    | I/O  | GPIO2, ADC1_CH1, TOUCH2 |
| 14  | 3    | I/O  | GPIO3, ADC1_CH2, TOUCH3 |
| 15  | 4    | I/O  | GPIO4, ADC1_CH3, TOUCH4 |
| 16  | 5    | I/O  | GPIO5, ADC1_CH4, TOUCH5 |
| 17  | 6    | I/O  | GPIO6, ADC1_CH5, TOUCH6 |
| 18  | 7    | I/O  | GPIO7, ADC1_CH6, TOUCH7 |

## Functional Pin Groups

### OLED Display (SSD1306)
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| OLED_SDA | 4    | J3-15 | OLED I2C Data (GPIO4) |
| OLED_SCL | 15   | J3-4  | OLED I2C Clock (GPIO15) |
| OLED_RST | 21   | J2-16 | OLED Reset |

### LoRa Radio (SX1262)
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| LoRa_NSS  | 8    | J2-8  | LoRa Chip Select |
| LoRa_DIO1 | 14   | J3-13 | LoRa DIO1 Interrupt |
| LoRa_RST  | 12   | J3-15 | LoRa Reset |
| LoRa_Busy | 13   | J3-14 | LoRa Busy Signal |
| LoRa_MISO | 11   | J3-16 | LoRa SPI MISO |
| LoRa_MOSI | 10   | J3-17 | LoRa SPI MOSI |
| LoRa_SCK  | 9    | J3-18 | LoRa SPI Clock |

### System Control
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| USER_SW   | 0    | J2-8  | User Button (PRG) |
| Vext_Ctrl | 36   | J2-9  | Vext Power Control |
| Vbat_Read | 1    | J3-12 | Battery Voltage Reading |

### Communication Interfaces
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| U0RXD     | 44   | J2-5  | UART0 Receive (CP2102 TXD) |
| U0TXD     | 43   | J2-6  | UART0 Transmit (CP2102 RXD) |
| U1RXD     | 18   | J3-1  | UART1 Receive |
| U1TXD     | 17   | J3-2  | UART1 Transmit |

### Additional Features
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| Touch Pins| 1-7  | J3-12,13,14,15,16,17,18 | Capacitive Touch Inputs |
| ADC Pins  | 1-7, 19, 20 | J3-12,13,14,15,16,17,18, J2-17,18 | Analog-to-Digital Conversion |
| USB Pins  | 19, 20 | J2-17,18 | USB D+/D- |

## Important Notes

### Power Management
- **VBAT Calculation**: `VBAT = (100 / (100+390)) * VADC_IN1`
- **Vext Control**: GPIO36 controls the Vext power rail
- **Vext Power**: Powers external sensors

### SPI Configuration
- **LoRa SPI**: Uses dedicated SPI bus (GPIO8-14)
- **Flash SPI**: Uses FSPI pins for internal flash
- **MTMS/MTDI**: Available for additional SPI devices

### Communication
- **UART0**: Connected to CP2102 USB-to-Serial converter
- **UART1**: Available for external communication
- **I2C**: Available on GPIO4/15 for OLED display

### Hardware Features
- **Touch Support**: 7 capacitive touch inputs (GPIO1-7)
- **ADC Support**: 10 ADC channels for analog input
- **USB Support**: Native USB D+/D- pins available
