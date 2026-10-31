#include "usercode.h"

#define USERCODE_MAGIC 0xA5C30001u

static uint32_t s_magic = 0u;

void UserCode_Init(void)
{
    s_magic = USERCODE_MAGIC;
}

uint32_t UserCode_GetMagic(void)
{
    return s_magic;
}
