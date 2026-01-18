#include "common.h"
#include "psyq/libspu.h"

#define VOICE_COUNT 24
#define SOUND_CHANNEL_COUNT 0x20

typedef struct
{
    /* 0x000 */ u32 unk0;
    /* 0x004 */ u32 unk4;
    /* 0x008 */ u8 Data[0x100];
    /* 0x108 */ u32 AssignedVoiceNumber;
    /* 0x10C */ u32 unk10C;
    /* 0x110 */ u8 unk110[0x14];
} FSoundChannel;

typedef struct
{
    s32 AssignedVoiceNumber;
    s32 VoiceParamFlags;
    u32 StartAddress;
    u32 LoopAddress;
    u16 SampleRate;
    u16 AdsrLower;
    u16 AdsrUpper;
    u16 VolumeScale;
    SpuVolume Volume;
} FSoundVoiceParams;

extern s16 D_80092A64;
extern s32 g_CdVolume;

void SetVoiceVolume( s32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale );
void SetVoiceSampleRate( s32 in_VoiceIndex, s32 in_SampleRate );
void SetVoiceParamsByFlags( u32 in_VoiceIndex, FSoundVoiceParams *in_VoiceParams );
void UpdateCdVolume();