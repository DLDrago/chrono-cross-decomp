#include "common.h"

#define VOICE_COUNT 24
#define SOUND_CHANNEL_COUNT 0x20

typedef struct
{
    /* 0x0 */ u32 AssignedVoiceNumber;
    /* 0x4 */ u32 unk4;
    /* 0x8 */ u8 Data[0x11C];
} FSoundChannel;

typedef struct
{
    /* 0x000 */ u32 unk0;
    /* 0x004 */ u32 unk4;
    /* 0x008 */ u8 Header[0x100];
    /* 0x108 */ FSoundChannel Channels[SOUND_CHANNEL_COUNT];
} FSoundManager;

extern s16 D_80092A64;
extern s32 g_CdVolume;

void SetVoiceVolume( s32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale );
void SetVoiceSampleRate( s32 in_VoiceIndex, s32 in_SampleRate );
void UpdateCdVolume();