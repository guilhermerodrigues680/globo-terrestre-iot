/*
  RemoteLogging.cpp - Classe para fazer log remoto usando UDP
  Created by Guilherme Rodrigues, guilhermerodrigues680, August, 2020.
*/
#include <WiFiUdp.h>
#include "remoteLogging.h"

namespace remoteLogging {

const char* _udp_server_addr = "192.168.2.104";
const int _udp_server_port = 2222;
WiFiUDP _udp;

void sendPacket(String msg) {
    // Inicializa o pacote de transmissao ao IP e PORTA.
    // Adiciona-se o valor ao pacote
    // Finaliza o pacote e envia.
    _udp.beginPacket(_udp_server_addr, _udp_server_port);
    _udp.print(msg);
    _udp.endPacket();
}

void sendPacket(const char *msg) {
    _udp.beginPacket(_udp_server_addr, _udp_server_port);
    _udp.write(msg);
    _udp.endPacket();
}

void sendBufferPacket(uint8_t *msg, size_t length) {
    _udp.beginPacket(_udp_server_addr, _udp_server_port);
    _udp.write(msg, length);
    _udp.endPacket();
}

}
