# Heltec Wireless Tracker V1.1 - Complete Pin Mapping

This document provides the complete pin mapping for the Heltec Wireless Tracker V1.1 based on the official datasheet and header pinouts.

## Board Overview

- **Board**: Heltec Wireless Tracker V1.1
- **MCU**: ESP32-S3FN8
- **Display**: 0.96-inch 80x160 RGB TFT-LCD
- **Radio**: SX1262 LoRa
- **GPS**: UC6580 Multi-system GNSS
- **Headers**: J2 (Power + Signal), J3 (GPIO + Peripherals)

## Header J2 - Power and Signal

### Power Block (Left Side)
| Pin | Name | Type | Functions |
|-----|------|------|-----------|
| 1   | 5V   | P    | 5V Power Supply |
| 2   | GND  | P    | Ground |
| 3   | 3V3  | P    | Output 3.3V |
| 4   | GND  | P    | Ground |
| 5   | 3V3  | P    | Output 3.3V |
| 6   | GND  | P    | Ground |
| 7   | Vext | P    | Output 3.3V, Power for TFT, Power for GNSS |
| 8   | GND  | P    | Ground |

### Signal Block (Right Side)
| Pin | Name | Type | Functions |
|-----|------|------|-----------|
| 1   | RST  | P    | RST_SW |
| 2   | 0    | I/O  | GPIO0, USER_SW |
| 3   | 1    | I/O  | GPIO1, Vbat_Read, Touch1, ADC1_CH0 |
| 4   | 2    | I/O  | GPIO2, ADC Ctrl, Touch1, ADC1_CH0 |
| 5   | 3    | I/O  | GPIO3, Vext Ctrl, Touch1, ADC1_CH0 |
| 6   | 19   | I/O  | GPIO19, USB_D-, CLK_OUT2, ADC2_CH8, U1RTS |
| 7   | 20   | I    | GPIO20, USB_D+, CLK_OUT1, ADC2_CH9, U1CTS |
| 8   | 21   | I/O  | GPIO21, TFT_LED_K |
| 9   | 26   | I/O  | GPIO26, SPICS1 |
| 10  | 48   | I/O  | GPIO48, D_SEL |
| 11  | 47   | I/O  | GPIO47, Boot_Mode |
| 12  | 33   | I/O  | GPIO33, SPIIO4, FSPIHD, SUBSPIHD, GNSS_TX |
| 13  | 34   | I/O  | GPIO34, SPI05, FSPICS0, SUBSPICS0, GNSS_RX |
| 14  | 35   | I/O  | GPIO35, SPI06, FSPID, SUBSPID, GNSS_RST |
| 15  | 36   | I/O  | GPIO36, SPI07, FSPICLK, SUBSPICLK, GNSS_PPS |
| 16  | 37   | I/O  | GPIO37, SPIDQS, FSPIQ, SUBSPIQ |

## Header J3 - GPIO and Peripherals

| Pin | Name | Type | Functions |
|-----|------|------|-----------|
| 1   | 18   | I/O  | GPIO18, U1RXD, ADC2_CH7, CLK_OUT3, LED Write Ctrl |
| 2   | 17   | I/O  | GPIO17, U1TXD, ADC2_CH6 |
| 3   | 16   | I/O  | GPIO16, U0CTS, ADC2_CH5, XTAL_32K_N |
| 4   | 15   | I/O  | GPIO15, U0RTS, ADC2_CH4, XTAL_32K_P |
| 5   | 7    | I/O  | GPIO7, ADC1_CH6, TOUCH7 |
| 6   | 6    | I/O  | GPIO6, ADC1_CH5, TOUCH6 |
| 7   | 5    | I/O  | GPIO5, ADC1_CH4, TOUCH5 |
| 8   | 4    | I/O  | GPIO4, ADC1_CH3, TOUCH4 |
| 9   | 46   | I/O  | GPIO46 |
| 10  | 45   | I/O  | GPIO45 |
| 11  | 44   | I/O  | GPIO44, U0RXD |
| 12  | 43   | I/O  | GPIO43, U0TXD |
| 13  | 14   | I/O  | GPIO14, ADC2_CH3, TOUCH14, FSPIDQS, SUBSPIWP, FSPIWP, LoRa_DIO1 |
| 14  | 13   | I/O  | GPIO13, ADC2_CH2, TOUCH13, FSPIIO7, SUBSPIQ, FSPIQ, LoRa_Busy |
| 15  | 12   | I/O  | GPIO12, ADC2_CH1, TOUCH12, FSPIIO6, SUBSPICLK, FSPICLK, LoRa_RST |
| 16  | 11   | I/O  | GPIO11, ADC2_CH0, TOUCH11, FSPIIO5, SUBSPID, FSPID, LoRa_MISO |
| 17  | 10   | I/O  | GPIO10, ADC1_CH9, TOUCH10, FSPIIO4, SUBSPICS0, FSPICS0, LoRa_MOSI |
| 18  | 9    | I/O  | GPIO9, ADC1_CH8, TOUCH9, SUBSPIHD, FSPIHD, LoRa_SCK |
| 19  | 8    | I/O  | GPIO8, ADC1_CH7, TOUCH8, SUBSPICS1, LoRa_NSS |
| 20  | 42   | I/O  | GPIO42, MTMS, TFT_SDIN |
| 21  | 41   | I/O  | GPIO41, MTDI, TFT_SCLK |
| 22  | 40   | I/O  | GPIO40, MTMS, TFT_RS |
| 23  | 39   | I/O  | GPIO39, MTMS, TFT_RES |
| 24  | 38   | I/O  | GPIO38, FSPIWP, SUBSPIWP, TFT_CS |

## Functional Pin Groups

### TFT-LCD Display
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| TFT_SDIN | 42   | J3-20 | TFT Data Input |
| TFT_SCLK | 41   | J3-21 | TFT Clock |
| TFT_RS   | 40   | J3-22 | TFT Register Select |
| TFT_RES  | 39   | J3-23 | TFT Reset |
| TFT_CS   | 38   | J3-24 | TFT Chip Select |
| TFT_LED_K| 21   | J2-8  | TFT Backlight Control |

### LoRa Radio (SX1262)
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| LoRa_NSS  | 8    | J3-19 | LoRa Chip Select |
| LoRa_DIO1 | 14   | J3-13 | LoRa DIO1 Interrupt |
| LoRa_RST  | 12   | J3-15 | LoRa Reset |
| LoRa_Busy | 13   | J3-14 | LoRa Busy Signal |
| LoRa_MISO | 11   | J3-16 | LoRa SPI MISO |
| LoRa_MOSI | 10   | J3-17 | LoRa SPI MOSI |
| LoRa_SCK  | 9    | J3-18 | LoRa SPI Clock |

### GPS (UC6580 Multi-system GNSS)
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| GNSS_TX   | 33   | J2-12 | GPS UART Transmit |
| GNSS_RX   | 34   | J2-13 | GPS UART Receive |
| GNSS_RST  | 35   | J2-14 | GPS Reset |
| GNSS_PPS  | 36   | J2-15 | GPS PPS Signal |

### System Control
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| USER_SW   | 0    | J2-2  | User Button |
| Vext_Ctrl | 3    | J2-5  | Vext Power Control |
| Vbat_Read | 1    | J2-3  | Battery Voltage Reading |

### Communication Interfaces
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| U0RXD     | 44   | J3-11 | UART0 Receive |
| U0TXD     | 43   | J3-12 | UART0 Transmit |
| U1RXD     | 18   | J3-1  | UART1 Receive |
| U1TXD     | 17   | J3-2  | UART1 Transmit |

### Additional Features
| Function | GPIO | Pin | Description |
|----------|------|-----|-------------|
| Touch Pins| 1-7, 14 | J2-3,4,5, J3-5,6,7,8,13 | Capacitive Touch Inputs |
| ADC Pins  | 1-7, 14, 19 | J2-3,4,5, J3-5,6,7,8,13, J2-6 | Analog-to-Digital Conversion |
| USB Pins  | 19, 20 | J2-6,7 | USB D+/D- |

## Important Notes

### Power Management
- **VBAT Calculation**: `VBAT = Vbat_Read * 4.9`
- **Vext Control**: GPIO3 controls the Vext power rail
- **Vext Power**: Powers both TFT display and GNSS module

### SPI Configuration
- **LoRa SPI**: Uses dedicated SPI bus (GPIO8-14)
- **TFT SPI**: Uses MTMS/MTDI pins (GPIO38-42)
- **Flash SPI**: Uses FSPI pins for internal flash

### Frequency Bands
- **LF Variant**: 470-510MHz (CN470 LPWAN)
- **HF Variant**: 863-928MHz (EU868, US915, AU915, AS923, KR920)

### GNSS Support
- **Multi-system**: GPS, GLONASS, BDS, Galileo, NAVIC, QZSS
- **Dual-frequency**: L1 + L5 support
- **High precision**: 22nm technology for low power consumption
