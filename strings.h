// C includes
#include <stdint.h>
#include <stdbool.h>


#define ID_STR_MACHINE_K 0
#define ID_STR_MACHINE_UNKNOWN 1
#define NUM_STRINGS 2
#define TOTAL_STRING_BYTES 606


// retrieves a string from extended memory and stashes in the first page of STORAGE_DOS_BOOT_BUFFER for the calling program to retrieve
// returns a pointer to the string (pointer will always be to STORAGE_DOS_BOOT_BUFFER)
char* General_GetString(uint8_t the_string_id);