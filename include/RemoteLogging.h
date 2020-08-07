/*
  RemoteLogging.h - Classe para fazer log remoto usando UDP
  Created by Guilherme Rodrigues, guilhermerodrigues680, August, 2020.
*/
#ifndef RemoteLogging_h
#define RemoteLogging_h

#include <WiFiUdp.h>

class RemoteLogging
{
  public:
    RemoteLogging(); // Disallow creating an instance of this object
    void sendPacket(String msg);
    void sendPacket(const char *msg);
    void sendBufferPacket(uint8_t *msg, size_t length);
  private:
    static constexpr char* _udp_server_addr = "192.168.2.104";
    static const int _udp_server_port = 2222;
    WiFiUDP _udp;
};

#endif
