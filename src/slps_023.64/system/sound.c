#include "common.h"
#include "hw.h"

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceKeyOn( u32 in_KeyOn )
{
    SPU_VOICE_KEY_ON_LO = in_KeyOn;
    SPU_VOICE_KEY_ON_HI = (in_KeyOn >> 0x10);
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceKeyOff( u32 in_KeyOff )
{
    SPU_VOICE_KEY_OFF_LO = in_KeyOff;
    SPU_VOICE_KEY_OFF_HI = (in_KeyOff >> 0x10);
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceReverbMode);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceNoiseMode);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceFmMode);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceVolume);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceSampleRate);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceStartAddr);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceRepeatAddr);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDB8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDD0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BDE8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE18);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE40);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE68);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", func_8004BE9C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound", SetVoiceParams);
