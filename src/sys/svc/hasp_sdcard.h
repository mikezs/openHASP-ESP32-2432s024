/* MIT License - Copyright (c) 2019-2024 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_SDCARD_H
#define HASP_SDCARD_H

#include "hasp_conf.h"

#if HASP_USE_SDCARD > 0
void sdcardSetup();
void sdcardLoop();
#endif

#endif
