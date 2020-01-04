#include "Config.hpp"
#include <inih/INIReader.h>

namespace layoff::config {
    //bool ShouldShowLog;
    bool ShouldUseIPC;

    bool ConfigInit()
    {
        ShouldUseIPC = true;
        //ShouldShowLog = false;

        INIReader reader;
        reader = INIReader("sdmc:/layoff.ini");
        if (reader.ParseError() != 0) {
            return false;
        }
        //ShouldShowLog = reader.GetBoolean("layoff", "ShowLog", ShouldShowLog);
        ShouldUseIPC = reader.GetBoolean("layoff", "UseIPC", ShouldUseIPC);
        return true;
    }

    /*bool GetShowLog()
    {
        return ShouldShowLog;
    }*/

    bool GetUseIPC()
    {
        return ShouldUseIPC;
    }

}