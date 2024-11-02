#include "allocator.h"

#include <stdlib.h>
const allocator_t std_allocator = {
    .alloc = malloc,
    .free = free,
    .realloc = realloc,
};
