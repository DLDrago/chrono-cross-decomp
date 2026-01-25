#include "common.h"
#include "system/sound.h"


// 0x20 toggles whether we use the alternate sample bank
#define SOUND_BANK_FLAG_ALT_SAMPLE_BANK      (1u << 5)   // 0x20

// the instrument index window that is eligible for bank remap
#define SOUND_BANK_REMAP_BASE_INDEX          0x20u       // first remappable instrument
#define SOUND_BANK_REMAP_COUNT               0x40u       // 64 instruments (0x20..0x5F)

// how far to shift SPU sample addresses when remapping
#define SOUND_BANK_SPU_ADDR_OFFSET           0x30000u

//----------------------------------------------------------------------------------------------------------------------
u16 Sound_ApplySampleBankOffsetIfNeeded( u32 in_Flags, FSoundChannel* in_pChannel )
{
    if( in_Flags & SOUND_BANK_FLAG_ALT_SAMPLE_BANK &&
            (in_pChannel->InstrumentIndex - SOUND_BANK_REMAP_BASE_INDEX) < SOUND_BANK_REMAP_COUNT
    )
    {
        in_pChannel->VoiceParams.StartAddress += SOUND_BANK_SPU_ADDR_OFFSET;
        in_pChannel->VoiceParams.LoopAddress  += SOUND_BANK_SPU_ADDR_OFFSET;
        in_pChannel->InstrumentIndex          += SOUND_BANK_REMAP_BASE_INDEX; // mirror into alt-bank instrument table
    }
    return in_pChannel->InstrumentIndex;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004DDA4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004DDF8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004DED8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004DF1C);


//----------------------------------------------------------------------------------------------------------------------
u32 ChannelMaskToVoiceMask( FSoundChannel* in_pChannel, u32 in_ChannelMask )
{
    u32 VoiceNumber, Mask;
    u32 i = 0;
    u32 out_VoiceMask = 0;

    while( i < SOUND_CHANNEL_COUNT )
    {
        Mask = 1 << i;
        if( in_ChannelMask & Mask )
        {
            VoiceNumber = in_pChannel->VoiceParams.AssignedVoiceNumber;
            if( VoiceNumber < VOICE_COUNT )
            {
                out_VoiceMask |= 1 << VoiceNumber;
            }
        }
        i++;
        in_pChannel++;
    };
    return out_VoiceMask;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", Sound_LoadAkaoSequence);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E3A4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E478);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E7D8);


//----------------------------------------------------------------------------------------------------------------------
void FreeVoiceChannels( FSoundChannel* in_Channel, u32 in_Voice )
{
    u32 VoiceIndex;

    if( in_Voice < VOICE_COUNT )
    {
        VoiceIndex = 0;
        while( VoiceIndex < SOUND_CHANNEL_COUNT )
        {
            if( in_Channel->VoiceParams.AssignedVoiceNumber == in_Voice )
            {
                in_Channel->VoiceParams.AssignedVoiceNumber = VOICE_COUNT;
                g_pActiveMusicConfig->ActiveNoteMask &= ~(1 << VoiceIndex);
            }
            in_Channel++;
            VoiceIndex++;
        };
    }
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E9D0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004EBC8);

//----------------------------------------------------------------------------------------------------------------------
// Unknown exactly how this functions but it is setting bits 0 and 1 to each channel in the incoming struct's flags
void unk_Sound_SetLow2BitsForChannels( FSoundChannelConfig* in_p, FSoundChannel* in_pChannel )
{
    u32 tmp;
    u32 Flags;
    u32 Mask;

    tmp = in_p->ActiveChannelMask;
    if( tmp == 0 )
    {
        return;
    }

    Flags = tmp;
    Mask = 1;

    while( Flags != 0 )
    {
        if( Flags & Mask )
        {
            in_pChannel->VoiceParams.VoiceParamFlags |= (1 << 1) | (1 << 0);
            Flags ^= Mask;
        }
        in_pChannel++;
        Mask <<= 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004EC88);

extern s32 D_80094FAC;
extern s32 D_80094FFC;

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", Sound_SetMusicSequence);

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004EF8C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F008);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F088);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F130);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F1F8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F3C4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F404);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F450);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F518);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F5C8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F6E8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F714);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F7C8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004F904);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004FA04);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004FAB8);

//----------------------------------------------------------------------------------------------------------------------
void SetCdVolume( u16* in_pVolume )
{
    D_80092A64 = 0;
    g_CdVolume = *in_pVolume << 0x10;
    UpdateCdVolume();
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004FC0C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004FC74);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004FCE4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004FDCC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004FF4C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004FFC8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050090);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050170);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_800502E8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050360);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050424);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050504);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8005068C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_800506E4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_800507B0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_800507CC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050834);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_800508A8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_800508C4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8005092C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_800509A0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_800509F0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050A58);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050B34);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050B94);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050BF4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_80050C34);
