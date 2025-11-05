#include "common.h"
#include "psyq/libspu.h"

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050C48);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050D38);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050DD4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050EF0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050F84);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80051000);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80051094);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80051110);

//----------------------------------------------------------------------------------------------------------------------
void DEBUG_80051234(void) {
}

//----------------------------------------------------------------------------------------------------------------------
void SetReverbMode( s32 in_ReverbMode )
{
    s32 currentReverbMode;

    SpuGetReverbModeType( &currentReverbMode );
    if( currentReverbMode != in_ReverbMode )
    {
        SpuSetReverb( SPU_OFF );
        SpuSetReverbModeType( in_ReverbMode | SPU_REV_MODE_CLEAR_WA );
        SpuSetReverb( SPU_ON );
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_8005128C);
