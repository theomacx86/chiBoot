#ifndef E820_H
#define E820_H

#include <Uefi.h>

#define E820_RAM                1
#define E820_RESERVED           2
#define E820_ACPI               3
#define E820_NVS                4
#define E820_UNUSABLE           5

typedef struct E820
{
    UINT64 Address;
    UINT64 Size;
    UINT32 Type;
} E820_S;

#endif
