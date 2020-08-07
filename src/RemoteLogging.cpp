/*
  RemoteLogging.cpp - Classe para fazer log remoto usando UDP
  Created by Guilherme Rodrigues, guilhermerodrigues680, August, 2020.
*/
#include <WiFiUdp.h>
#include "RemoteLogging.h"

RemoteLogging::RemoteLogging()
    : _udp() // initializer list
{
    //Cria um objeto da classe UDP.
    // WiFiUDP _udp;)
    // _udp = WiFiUDP();
}

void RemoteLogging::sendPacket(String msg) {
    // Inicializa o pacote de transmissao ao IP e PORTA.
    // Adiciona-se o valor ao pacote
    // Finaliza o pacote e envia.
    _udp.beginPacket(_udp_server_addr, _udp_server_port);
    _udp.print(msg);
    _udp.endPacket();
}

void RemoteLogging::sendPacket(const char *msg) {
    _udp.beginPacket(_udp_server_addr, _udp_server_port);
    _udp.write(msg);
    _udp.endPacket();
}

void RemoteLogging::sendBufferPacket(uint8_t *msg, size_t length) {
    _udp.beginPacket(_udp_server_addr, _udp_server_port);
    _udp.write(msg, length);
    _udp.endPacket();
}
