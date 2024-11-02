#ifndef _HTTPC_MACROS_H_
#define _HTTPC_MACROS_H_

#include <stdint.h>

#define in_range(x, a, b) ((x) >= (a) && (x) <= (b))
#define NOT_IMPLEMENTED assert(false);  // NOT IMPLEMENTED
#define UNREACHABLE assert(false);      // UNREACHABLE

typedef uint8_t byte;

#endif
