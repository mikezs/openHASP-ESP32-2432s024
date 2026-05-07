/* MIT License - Copyright (c) 2019-2024 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_SDLOG_H
#define HASP_SDLOG_H

#include "hasp_conf.h"

#if HASP_USE_SDCARD > 0
#include <Arduino.h>
#include <FS.h>
#include <SD.h>

class SdLogStream : public Print {
public:
    SdLogStream();
    bool begin();
    void end();
    virtual size_t write(uint8_t c) override;
    virtual size_t write(const uint8_t* buffer, size_t size) override;
    void flush();
    bool is_enabled() { return _enabled; }
    void set_enabled(bool enabled);

private:
    File _logFile;
    bool _enabled;
    void _rotateLog();
};

extern SdLogStream sdLogStream;

#endif

#endif
