#include "common.h"

#define VOICE_COUNT 24
#define SOUND_CHANNEL_COUNT 0x20

typedef struct
{
    /* 0x0 */ u32 AssignedVoiceNumber;
    /* 0x4 */ u8 Data[0x120];
} FSoundChannel;

typedef struct
{
    /* 0x000 */ u8 Header[0x108];
    /* 0x108 */ FSoundChannel Channels[SOUND_CHANNEL_COUNT];
} FSoundManager;

void SetVoiceVolume( s32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale );
void SetVoiceSampleRate( s32 in_VoiceIndex, s32 in_SampleRate );
