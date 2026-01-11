#include "common.h"
#include "psyq/libspu.h"

#include "system/sound.h"

typedef struct
{
    /* 0x00 */ u8 unk0[0xC];
    /* 0x0C */ u32 bUnknown;
    /* 0x10 */ s32 VoiceIndex;
    /* 0x14 */ u8 unk14[0x2C];
    /* 0x40 */ s32 volume;
    /* 0x44 */ u8 unk44[0x14];
    /* 0x58 */ s32 SampleRate;
} FSound80094FA0;

extern FSound80094FA0 D_80094FA0;

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050C48);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050D38);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050DD4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/unk_reverb", func_80050EF0);

//----------------------------------------------------------------------------------------------------------------------
void func_80050F84()
{
    if( D_80094FA0.bUnknown != 0 )
    {
        // Looks like we have a stereo pair, one left and one right
        SetVoiceSampleRate( D_80094FA0.VoiceIndex, 0 );
        SetVoiceSampleRate( D_80094FA0.VoiceIndex + 1, 0 );
        SetVoiceVolume( D_80094FA0.VoiceIndex, 0, 0, 0 );
        SetVoiceVolume( D_80094FA0.VoiceIndex + 1, 0, 0, 0 );
    }
}

//----------------------------------------------------------------------------------------------------------------------
void func_80051000()
{
    s32 unpackedVolume;

    if( D_80094FA0.bUnknown != 0 )
    {
        // Looks like we have a stereo pair, one left and one right
        SetVoiceSampleRate( D_80094FA0.VoiceIndex, D_80094FA0.SampleRate );
        SetVoiceSampleRate( D_80094FA0.VoiceIndex + 1, D_80094FA0.SampleRate );

        // Unpack in sign extended way
        unpackedVolume = (s32) (D_80094FA0.volume << 0xF) >> 0x10;
        SetVoiceVolume( D_80094FA0.VoiceIndex, unpackedVolume, 0, 0 );
        SetVoiceVolume( D_80094FA0.VoiceIndex + 1, 0, unpackedVolume, 0 );
    }
}

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
