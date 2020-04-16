#include "imconfig.h"
#include <switch.h>
#include <string>
void MyAssert(bool _EXPR, std::string file, int line, std::string err) {
        if (!_EXPR) {
                FILE* f = fopen ("sdmc:/layoff_error.txt", "w");
                fprintf(f, "Assert fail (%s:%d):\n%s", file.c_str(), line, err.c_str());
                fflush(f);
                fclose(f);
                fatalThrow(MAKERESULT(133,7000));
        }
}