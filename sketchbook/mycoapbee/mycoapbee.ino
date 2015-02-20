#include <Arduino.h>
#include <SoftwareSerial.h>

#include "WiFly.h" // https://github.com/Seeed-Studio/WiFi_Shield

#include <coap.h> // https://github.com/cache91/microcoap

#define SSID      "YOURSSID"
#define KEY       "YOURPASSWORD"
// WIFLY_AUTH_OPEN / WIFLY_AUTH_WPA1 / WIFLY_AUTH_WPA1_2 / WIFLY_AUTH_WPA2_PSK
#define AUTH      WIFLY_AUTH_WPA2_PSK
#define BAUDRATE      9600

// FIXME: NOT SURE:
#define UDP_TX_PACKET_MAX_SIZE 192

#define UDP_HOST_IP        "192.168.1.200" //"255.255.255.255"      // broadcast
#define UDP_REMOTE_PORT    5683//55555
#define UDP_LOCAL_PORT     5683//55555

// Pins' connection
// Arduino       WiFly
//  2    <---->    TX
//  3    <---->    RX
SoftwareSerial uart(2, 3);
WiFly wifly(uart);

uint8_t packetbuf[256];
static uint8_t scratch_raw[32];
static coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

void setupUDP(const char *host_ip, uint16_t remote_port, uint16_t local_port)
{
    char cmd[32];
    wifly.sendCommand("set time address pool.ntp.org\r", "AOK");
    wifly.sendCommand("set time enable 1\r", "AOK");
    wifly.sendCommand("set time enable 1\r", "AOK");
    wifly.sendCommand("get time\r", "AOK");

    wifly.sendCommand("set w j 1\r", "AOK");   // enable auto join

    wifly.sendCommand("set i p 1\r", "AOK");
    snprintf(cmd, sizeof(cmd), "set i h %s\r", host_ip);
    wifly.sendCommand(cmd, "AOK");
    snprintf(cmd, sizeof(cmd), "set i r %d\r", remote_port);
    wifly.sendCommand(cmd, "AOK");
    snprintf(cmd, sizeof(cmd), "set i l %d\r", local_port);
    wifly.sendCommand(cmd, "AOK");
    wifly.sendCommand("save\r");
    wifly.sendCommand("reboot\r");
}

void setup() {
    Serial.begin(BAUDRATE);
    Serial.println("--------- WIFLY UDP --------");

    uart.begin(BAUDRATE);     // WiFly UART Baud Rate
    wifly.reset();

    while (1) {
        Serial.println("Try to join " SSID );
        if (wifly.join(SSID, KEY, AUTH)) {
            Serial.println("Succeed to join " SSID);
            wifly.clear();
            break;
        } else {
            Serial.println("Failed to join " SSID);
            Serial.println("Wait 1 second and try again...");
            delay(1000);
        }
    }

    setupUDP(UDP_HOST_IP, UDP_REMOTE_PORT, UDP_REMOTE_PORT);

    delay(1000);
    wifly.clear();

    coap_setup();
    endpoint_setup();
}

unsigned int time_point = 0;

int sz=0;
int rc=1;
coap_packet_t pkt;
int i=0;
int parseReady=0; // data ready bool

void coapServ() {
    if (parseReady==0) {
        sz=0;
        uint8_t r;
        while (wifly.receive(&r, 1, 10) == 1) {
            packetbuf[sz]=r;
            sz+=1;
        }
        if (sz>4)
        {
            Serial.println("Found udp packet:");
            for (i=0;i<sz;i++)
            {
                Serial.print(packetbuf[i], HEX);
                Serial.print(" ");
            }
            Serial.println("");
            parseReady=1;
        }

    }
    else {
        if (0 != (rc = coap_parse(&pkt, packetbuf, sz)))
        {
            Serial.print("Bad packet rc=");
            Serial.print(rc, DEC);
            Serial.println(' for now.');
            parseReady=0;
        }
        else
        {

            size_t rsplen = sz;

            Serial.print("Packet size: ");
            Serial.println(rsplen);

            coap_packet_t rsppkt;
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);

            memset(packetbuf, 0, UDP_TX_PACKET_MAX_SIZE);
            if (0 != (rc = coap_build(packetbuf, &rsplen, &rsppkt)))
            {
                Serial.print("coap_build failed rc=");
                Serial.println(rc, DEC);

                parseReady=0;
            }
            else
            {
                Serial.print("answer: ");
                for (i=0;i<rsplen;i++)
                {
                  Serial.print(packetbuf[i], HEX);
                  Serial.print(" ");
                }
                Serial.println("");
                wifly.send(packetbuf, rsplen);
                parseReady=0;
            }



        }
    }
}

void loop() {
    coapServ();
}


