#include "common.h"
#include "system/sound.h"

//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
    /* 0x00 */ u8 unk0[0x4];
    /* 0x04 */ u32 unk_Flags_0x4;
    /* 0x08 */ u8 unk8[0xC];
    /* 0x14 */ u32 ChannelFlags; // 32-bit: one bit per channel
    // ...
} FSound80092A48;
extern FSound80092A48* D_80092A48;
extern FSound80092A48 D_80091940; // This is the same type as 80092A48 as shown by memcpy in FUN_8004F130
extern FSound80092A48* D_800917F0; // This seems to always either be null or a pointer to D_80091940

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", unk_Sound_8004DD50);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004DDA4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004DDF8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004DED8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004DF1C);


//----------------------------------------------------------------------------------------------------------------------
u32 ChannelMaskToVoiceMask( FSoundManager* in_pManager, s32 in_ChannelMask )
{
    u32 VoiceNumber, Mask;
    u32 i = 0;
    u32 out_VoiceMask = 0;

    while( i < SOUND_CHANNEL_COUNT )
    {
        Mask = 1 << i;
        if( in_ChannelMask & Mask )
        {
            VoiceNumber = in_pManager->Channels[i].AssignedVoiceNumber;
            if( VoiceNumber < VOICE_COUNT )
            {
                out_VoiceMask |= 1 << VoiceNumber;
            }
        }
        i++;
    };
    return out_VoiceMask;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E010);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E3A4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E478);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E7D8);


//----------------------------------------------------------------------------------------------------------------------
void FreeVoiceChannels( FSoundManager* in_Manager, u32 in_Voice )
{
    FSoundChannel* channel;
    u32 channelIndex;
    
    if (in_Voice >= VOICE_COUNT)
    {
        return;
    }
    
    channel = in_Manager->Channels;
    for (channelIndex = 0; channelIndex < SOUND_CHANNEL_COUNT; channelIndex++)
    {
        if (channel[channelIndex].AssignedVoiceNumber == in_Voice)
        {
            channel[channelIndex].AssignedVoiceNumber = VOICE_COUNT;
            D_80092A48->ChannelFlags &= ~(1 << channelIndex);
        }
        
    }
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004E9D0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004EBC8);

//----------------------------------------------------------------------------------------------------------------------
// Unknown exactly how this functions but it is setting bits 0 and 1 to each channel in the incoming struct's flags
void unk_Sound_SetLow2BitsForChannels( FSound80092A48* in_p, FSoundManager* in_Manager )
{
    u32 tmp;
    u32 Flags;
    u32 Mask;
    FSoundChannel* pChannel;

    tmp = in_p->unk_Flags_0x4;
    if( tmp == 0 )
    {
        return;
    }

    Flags = tmp;
    Mask = 1;
    pChannel = &in_Manager->Channels[0];

    while( Flags != 0 )
    {
        if( Flags & Mask )
        {
            pChannel->unk4 |= (1 << 1) | (1 << 0);
            Flags ^= Mask;
        }
        pChannel++;
        Mask <<= 1;
    }
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004EC88);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound2", func_8004ECDC);

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
