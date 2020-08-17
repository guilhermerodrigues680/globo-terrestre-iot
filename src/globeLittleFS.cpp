#include <FS.h>
#include <LittleFS.h>
#include "globeLittleFS.h"

namespace globeLittleFS {

void start() {
    LittleFS.begin(); // Start the SPI Flash Files System
}

bool exists(const String &path) {
    return LittleFS.exists(path); // If the file exists
}

fs::File open(const String &path, const char* mode) {
    return LittleFS.open(path, mode);
}

}
