#include <switch.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
#define __ASSERT_FUNCTION	__extension__ __PRETTY_FUNCTION__
void assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function);
#  define MyAssert(expr)							\
     (static_cast <bool> (expr)						\
      ? void (0)							\
      : assert_fail (#expr, __FILE__, __LINE__, __ASSERT_FUNCTION))
#ifdef __cplusplus
}
#endif