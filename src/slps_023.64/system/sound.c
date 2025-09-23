#include "common.h"
#include "psyq/libspu.h"
#include "hw.h"

typedef struct {
    SpuVolume volume;
    u16 pitch;
    u16 addr;
    u16 adsr[2];
    u16 volumex;
    u16 loop_addr;
} SPU_VOICE_REG;

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceKeyOn( u32 in_KeyOn )
{
    *SPU_VOICE_KEY_ON_LO = in_KeyOn;
    *SPU_VOICE_KEY_ON_HI = (in_KeyOn >> 0x10);
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceKeyOff( u32 in_KeyOff )
{
    *SPU_VOICE_KEY_OFF_LO = in_KeyOff;
    *SPU_VOICE_KEY_OFF_HI = (in_KeyOff >> 0x10);
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceReverbMode( u32 in_ReverbMode )
{
    *SPU_VOICE_CHN_REVERB_MODE_LO = in_ReverbMode;
    *SPU_VOICE_CHN_REVERB_MODE_HI = (in_ReverbMode >> 0x10);
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceNoiseMode( u32 in_NoiseMode )
{
    *SPU_VOICE_CHN_NOISE_MODE_LO = in_NoiseMode;
    *SPU_VOICE_CHN_NOISE_MODE_HI = (in_NoiseMode >> 0x10);
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceFmMode);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceVolume);

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceSampleRate( s32 in_VoiceIndex, s16 in_SampleRate )
{
    VOICE_00_ADPCM_SAMPLE_RATE[in_VoiceIndex * (sizeof(SPU_VOICE_REG) / sizeof(u16))] = in_SampleRate;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceStartAddr( u32 in_VoiceIndex, u32 in_Addr )
{
    VOICE_00_ADPCM_START_ADDR[in_VoiceIndex * (sizeof(SPU_VOICE_REG) / sizeof(u16))] = (in_Addr >> 3);
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceRepeatAddr( u32 in_VoiceIndex, u32 in_Addr )
{
    VOICE_00_ADPCM_REPEAT_ADDR[in_VoiceIndex * (sizeof(SPU_VOICE_REG) / sizeof(u16))] = (in_Addr >> 3);
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDB8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDD0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDE8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE18);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE40);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE68);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE9C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceParams);
