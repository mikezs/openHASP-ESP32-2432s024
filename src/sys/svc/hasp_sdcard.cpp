/* MIT License - Copyright (c) 2019-2024 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#include "hasp_sdcard.h"

#if HASP_USE_SDCARD > 0
#include "hasp_debug.h"
#include "SD.h"
#include "SPI.h"

static bool sdcard_mounted = false;

void sdcardSetup()
{
#ifdef SD_CS
    LOG_INFO(TAG_FILE, F("SD Card    : Initializing..."));

#if defined(SD_MISO) && defined(SD_MOSI) && defined(SD_SCLK)
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
#elif defined(TFT_MISO) && defined(TFT_MOSI) && defined(TFT_SCLK)
    SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, SD_CS);
#endif

    if(!SD.begin(SD_CS, SPI, 1000000, "/sdcard")) {
        LOG_ERROR(TAG_FILE, F("SD Card    : Mount failed! (Format?)"));
        sdcard_mounted = false;
    } else {
        uint8_t cardType = SD.cardType();
        if(cardType == CARD_NONE) {
            LOG_ERROR(TAG_FILE, F("SD Card    : No SD card attached"));
            sdcard_mounted = false;
            return;
        }

        sdcard_mounted = true;
        LOG_INFO(TAG_FILE, F("SD Card    : Type: %s"),
                 (cardType == CARD_MMC) ? "MMC" : (cardType == CARD_SD) ? "SDSC" : (cardType == CARD_SDHC) ? "SDHC" : "UNKNOWN");

        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        LOG_INFO(TAG_FILE, F("SD Card    : Size: %llu MB"), cardSize);
    }
#else
    LOG_WARNING(TAG_FILE, F("SD Card    : SD_CS not defined! (Build error?)"));
    sdcard_mounted = false;
#endif
}

void sdcardLoop()
{
#ifdef SD_CS
    static uint32_t last_check = 0;
    if(millis() - last_check > 2000) { // Check every 2 seconds
        last_check = millis();

        uint8_t cardType = SD.cardType();
        bool is_inserted = (cardType != CARD_NONE);

        if(is_inserted && !sdcard_mounted) {
            LOG_INFO(TAG_FILE, F("SD Card    : Insertion detected. Mounting..."));
            sdcardSetup();
        } else if(!is_inserted && sdcard_mounted) {
            LOG_WARNING(TAG_FILE, F("SD Card    : Removal detected. Unmounting..."));
            SD.end();
            sdcard_mounted = false;
        }
    }
#endif
}

#endif
