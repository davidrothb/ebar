# EBAR
Ebar projekt SPSE V Uzlabine E4A 2021/22.

Upload config:
  - Board: Arduino Nano
  - Processor: Atmega 328P (new bootloader)

### Slozka "ebar"
Kod pro jednotku v baru, definice GPIO jsou v souboru "defines.h". Displej je pres I2C (A4 SCA, A5 SDL).

### Slozka "esp_ebar"
Kod pro externi mikrokontroler typu esp82xx, pres I2C inject na konektoru pro displej se posilaji 1-byte prikazy pro Arduino - pohodlnejsi kalibrace nulove vahy nebo plneho kelimku. Soucasne na lokalni siti moznost posilat prikazy viz. kod pres WebSerial.

### Slozka "docs"
Fotky a dokumentace
