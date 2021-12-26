# IceNav-v3
ES32 GPS Navigator 
Programa receptor y navegador GPS con ESP32 + GPS + ILI9341 + HCM5883L


       Pinout:
       HCM5883L      ILI9341        MICRO SD       VBAT        GPS
       --------------------------------------------------------------------
       VCC 3,3v      VCC  3,3v      VCC  3,3v      GPIO034     VCC  3,3v
       GND GND       GND  GND       GND  GND                   GND  GND
       SDA GPIO21    LED  GPIO33    CS   GPIO04                RX   GPIO17
       SCL GPIO22    MISO GPIO27    MISO GPIO27                TX   GPIO16
                     SCK  GPIO14    SCK  GPIO14
                     MOSI GPIO13    MOSI GPIO13
                     DC   GPIO15
                     RST  GPIO32
                     CS   GPIO2

       Librerías:
       ILI9341 :  https://github.com/Bodmer/TFT_eSPI
       GPS:       https://github.com/mikalhart/TinyGPSPlus
       PCF8574:   https://github.com/RobTillaart/PCF8574
