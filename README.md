# mycoapbee
Testing CoAP [microcoap](https://github.com/1248/microcoap) realization with Arduino Uno and WiFi Bee v2 (via LilyPad Xbee)

![ScreenShot](https://dl.dropboxusercontent.com/u/39622126/live.jpeg)

## Arduino
Arduino Uno.
"Arduino, by default, has a UDP transmit buffer of 24 bytes. This is too small for some endpoints and will result in an error."

Memory problem, not enough for this command:
```
    ./coap-client -v 100 -m get coap://127.0.0.1/.well-known/core
```
but enough for:

```
    ./coap-client -e "1" -m put coap://127.0.0.1/light
    ./coap-client -e "0" -m put coap://127.0.0.1/light
```    

## WiFi Shield
LilyPad XBee (by SparkFun)
Supports 3.3V and 5V power supplies.

## WiFi module
Wi-Fi Bee v2
needs 3.3V power supply.
Works with arduino via rx-tx serial port (pin 2 and 3 in this project)

## Source codes
contains all additional files that you need for compilator
### WiFly Library
included unchanged current actual WiFly library https://github.com/Seeed-Studio/WiFi_Shield
### microCoAP
included [microcoap patched by cache91](https://github.com/cache91/microcoap) without non-needed for arduino main-posix.c and changed led pin out to arduino uno num.13 (embedded led)
### My sources
you should specify this lines in mycoapbee.ino project:

```
    #define SSID      "YOURSSID"
    #define KEY       "YOURPASSWORD"
    // WIFLY_AUTH_OPEN / WIFLY_AUTH_WPA1 / WIFLY_AUTH_WPA1_2 / WIFLY_AUTH_WPA2_PSK
    #define AUTH      WIFLY_AUTH_WPA2_PSK
```    
    
and ip your recievied by dhcpcd ip address:

```
    #define UDP_HOST_IP        "192.168.1.200" //"255.255.255.255"      // broadcast
    #define UDP_REMOTE_PORT    5683 //55555
    #define UDP_LOCAL_PORT     5683 //55555
```

not sure about this lines yet -- it's about memory limits:
```
#define UDP_TX_PACKET_MAX_SIZE 192
```
and about this:
```
uint8_t packetbuf[256];
static uint8_t scratch_raw[32];
```
