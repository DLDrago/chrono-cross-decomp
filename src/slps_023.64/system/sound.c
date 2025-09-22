#include "common.h"

// INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BC94);
void func_8004BC94( u32 in_KeyOn )
{
    *(u16*)0x1F801D88 = in_KeyOn;
    *(u16*)0x1F801D8A = (in_KeyOn >> 0x10);
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BCB0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BCCC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BCE8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BD04);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BD20);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BD68);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BD80);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BD9C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDB8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDD0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDE8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE18);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE40);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE68);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE9C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BECC);
