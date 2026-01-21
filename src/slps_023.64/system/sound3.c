#include "common.h"
#include "hw.h"
#include "system/sound.h"

extern s16 D_80092AFA;

void UpdateCdVolume()
{
    *CD_VOL_L = (s16*)D_80092AFA;
    *CD_VOL_R = (s16*)D_80092AFA;
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", memcpy32);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", memswap32);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80051F7C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052458);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800526FC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052790);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052DA4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052FB8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800531E0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053370);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800535E4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053D1C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", Sound_CopyInstrumentInfoToChannel);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053DC4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053DFC);
