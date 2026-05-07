/* MIT License - Copyright (c) 2019-2024 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#include "hasp_sdlog.h"

#if HASP_USE_SDCARD > 0
#include "hasp_debug.h"

#define SD_LOG_FILE "/hasp.log"
#define SD_LOG_FILE_OLD "/hasp.log.old"
#define SD_LOG_MAX_SIZE (1024 * 1024) // 1MB

SdLogStream::SdLogStream() : _enabled(false)
{}

bool SdLogStream::begin()
{
    if(!_enabled) return false;

    // Check if card is mounted
    if(SD.cardType() == CARD_NONE) {
        if(_logFile) _logFile.close();
        return false;
    }

    if(_logFile) return true; // Already open

    if(SD.exists(SD_LOG_FILE)) {
        File f = SD.open(SD_LOG_FILE, "r");
        if(f) {
            size_t size = f.size();
            f.close();
            if(size > SD_LOG_MAX_SIZE) {
                _rotateLog();
            }
        }
    }

    _logFile = SD.open(SD_LOG_FILE, "a");
    return _logFile ? true : false;
}

void SdLogStream::end()
{
    if(_logFile) {
        _logFile.close();
        // Reset the File object to ensure it's evaluated as false
        _logFile = File();
    }
}

void SdLogStream::set_enabled(bool enabled)
{
    if(_enabled == enabled) return;
    _enabled = enabled;
    if(_enabled) {
        begin();
    } else {
        end();
    }
}

size_t SdLogStream::write(uint8_t c)
{
    if(!_enabled) return 0;
    if(!begin()) return 0;

    size_t n = _logFile.write(c);
    if(_logFile.position() > SD_LOG_MAX_SIZE) {
        _rotateLog();
    }
    return n;
}

size_t SdLogStream::write(const uint8_t* buffer, size_t size)
{
    if(!_enabled) return 0;
    if(!begin()) return 0;

    size_t n = _logFile.write(buffer, size);
    if(_logFile.position() > SD_LOG_MAX_SIZE) {
        _rotateLog();
    }
    return n;
}

void SdLogStream::flush()
{
    if(_logFile) _logFile.flush();
}

void SdLogStream::_rotateLog()
{
    if(_logFile) {
        _logFile.flush();
        _logFile.close();
    }

    if(SD.exists(SD_LOG_FILE_OLD)) {
        SD.remove(SD_LOG_FILE_OLD);
    }

    if(SD.exists(SD_LOG_FILE)) {
        if(SD.rename(SD_LOG_FILE, SD_LOG_FILE_OLD)) {
             // Successfully rotated
        }
    }

    if(_enabled) {
        _logFile = SD.open(SD_LOG_FILE, "a");
        if(_logFile) {
            _logFile.println(F("--- Log rotated ---"));
        }
    }
}

SdLogStream sdLogStream;

#endif
