#ifndef globeLittleFS_h
#define globeLittleFS_h

#include <Arduino.h>
#include <FS.h>

namespace globeLittleFS {

void start();
bool exists(const String &path);
fs::File open(const String &path, const char* mode);

}

#endif
