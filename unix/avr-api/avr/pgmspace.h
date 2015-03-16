#include <string.h>

#define PROGMEM
#define PGM_VOID_P void *
#define PGM_P      char *

#define memcpy_P   memcpy
#define strncpy_P  strncpy
#define strcpy_P   strcpy
#define strncmp_P  strncmp
#define strcmp_P   strcmp
#define strlen_P   strlen
#define strchr_P   strchr


#define pgm_read_byte_near(x) (*x)
