//Functions to read and write layoff config
#pragma once

#include <switch.h>

namespace layoff::config {
    bool ConfigInit();
    
    //bool GetShowLog();
    bool GetUseIPC();
}