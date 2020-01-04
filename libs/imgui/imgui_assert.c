#include "imgui_assert.h"

void assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function)
{
    FILE *f = fopen("sdmc:/layoff_crash.log", "w+");
    fprintf(f, "Assertion failed: %s at:\n %s:%i\nFunction %s", __assertion, __file, __line, __function);
    fflush(f);
    fclose(f);
    fatalSimple(MAKERESULT(255, 80));
}