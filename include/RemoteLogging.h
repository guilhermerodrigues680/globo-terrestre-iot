/*
  RemoteLogging.h - Classe para fazer log remoto usando UDP
  Created by Guilherme Rodrigues, guilhermerodrigues680, August, 2020.
*/
#ifndef remoteLogging_h
#define remoteLogging_h

#include <Arduino.h>
#include <WiFiUdp.h>

namespace remoteLogging {

void sendPacket(String msg);
void sendPacket(const char *msg);
void sendBufferPacket(uint8_t *msg, size_t length);

}

#endif
