/*
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for ESP32

  Copyright (c) 2017 Hristo Gochkov. All rights reserved.
  This file is part of the ESP32 WiFi library for Arduino environment.

  Modified by Timo Sandmann for use with ct-Bot software framework,
  ported to C++14.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "WiFi.h"

#include <algorithm>
#include <cstdint>
#include <cstddef>

#include "config.h" // if config.h is missing, copy config.h.in to config.h and adjust for your wifi!

static WiFiServer server { 23 };
static WiFiClient serverClients[MAX_SRV_CLIENTS];
static uint32_t connection_time[MAX_SRV_CLIENTS];

static HardwareSerial Serial0 { 0 };
static HardwareSerial Serial2 { 2 };
static uint8_t serial_rx_buf[512];
static uint8_t serial_tx_buf[512];

void setup() {
    Serial0.begin(115200);
    Serial0.println("ESP32 starting WiFi setup...");

    const auto wifi_status { WiFi.begin(ssid, password) };

    Serial0.print("WiFi status=");
    Serial0.println(wifi_status, 10);

    for (uint8_t i { 0 }; WiFi.status() != WL_CONNECTED && i < 30; ++i) {
        Serial0.print("WiFi not connected... (");
        Serial0.print(i, 10);
        Serial0.print(") status=");
        Serial0.println(WiFi.status(), 10);

        if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial0.println("WiFi connect failed, rebooting...");
            Serial0.flush();

            ::esp_sleep_enable_timer_wakeup(500000ULL);
            ::esp_light_sleep_start();
            esp_restart();
        }
        ::delay(500);
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial0.print("WiFi connect failed. status=");
        Serial0.println(WiFi.status(), 10);

        while (true) {
            ::esp_sleep_enable_timer_wakeup(1000000ULL);
            ::esp_light_sleep_start();
            esp_restart();
        }
    }
    // WiFi.setSleep(false);
    Serial0.println("WiFi connected.");
    Serial0.flush();
    Serial0.end();

    /* start UART and server */
    Serial2.begin(UART_BAUDRATE);
    Serial2.setRxBufferSize(BUFFER_SIZE);

    server.begin();
    server.setNoDelay(true);
}

void loop() {
    /* check if there are any new clients */
    if (server.hasClient()) {
        for (uint8_t i { 0 }; i < MAX_SRV_CLIENTS; ++i) {
            /* find free/disconnected spot */
            if (!serverClients[i].connected()) {
                serverClients[i].stop();

                serverClients[i] = server.available();
                connection_time[i] = ::millis();

                /* suppress local echo */
                serverClients[i].write("\xff\xfb\x01", 3);
                serverClients[i].flush();
                serverClients[i].write("\xff\xfe\x01", 3);
                serverClients[i].flush();
                serverClients[i].write("\xff\xfe\x22", 3);
                serverClients[i].flush();

                serverClients[i].write("ESP32 telnet-2-serial ready.\r\n");
                serverClients[i].flush();
                return;
            }
        }
        /* no free/disconnected spot so reject */
        WiFiClient serverClient = server.available();
        serverClient.stop();
    }

    /* check clients for data */
    for (uint8_t i { 0 }; i < MAX_SRV_CLIENTS; ++i) {
        if (serverClients[i].connected() && serverClients[i].available()) {
            /* get data from the telnet client and push it to the UART */
            const uint32_t dt { ::millis() - connection_time[i] };
            if (dt < TELNET_DISCARD_TIME) {
                const uint8_t tmp { static_cast<uint8_t>(serverClients[i].read()) };
                if (tmp == 0xff) {
                    /* discard telnet protocl traffic during the first second */
                    while (serverClients[i].available() < 2) {
                        ::delay(0);
                    }
                    serverClients[i].read();
                    serverClients[i].read();
                } else {
                    Serial2.write(tmp);
                }
            } else {
                const auto len { std::min(static_cast<size_t>(serverClients[i].available()), 512U) };
                serverClients[i].read(serial_tx_buf, len);
                Serial2.write(serial_tx_buf, len);
            }
        }
        ::delay(0);
    }

    /* check UART for data */
    if (Serial2.available()) {
        const size_t len { std::min(static_cast<size_t>(Serial2.available()), 512U) };
        Serial2.readBytes(serial_rx_buf, len);
        /* push UART data to all connected telnet clients */
        for (uint8_t i { 0 }; i < MAX_SRV_CLIENTS; ++i) {
            if (serverClients[i].connected()) {
                serverClients[i].write(serial_rx_buf, len);
                ::delay(0);
            }
        }
    }
}
