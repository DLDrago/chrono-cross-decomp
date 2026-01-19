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

// Voice parameter update flags
#define VOICE_PARAM_VOLUME_L      (1 << 0)
#define VOICE_PARAM_VOLUME_R      (1 << 1)
#define VOICE_PARAM_VOLUME        (VOICE_PARAM_VOLUME_L | VOICE_PARAM_VOLUME_R)
#define VOICE_PARAM_SAMPLE_RATE   (1 << 4)
#define VOICE_PARAM_START_ADDR    (1 << 7)
#define VOICE_PARAM_ADSR_L_BIT_08 (1 << 8)
#define VOICE_PARAM_ADSR_L_BIT_11 (1 << 11)
#define VOICE_PARAM_ADSR_L_BIT_12 (1 << 12)
#define VOICE_PARAM_ADSR_L_BIT_15 (1 << 15)
#define VOICE_PARAM_ADSR_LOWER    (VOICE_PARAM_ADSR_L_BIT_08 | VOICE_PARAM_ADSR_L_BIT_11 | VOICE_PARAM_ADSR_L_BIT_12 | VOICE_PARAM_ADSR_L_BIT_15)
#define VOICE_PARAM_ADSR_U_BIT_09 (1 << 9)
#define VOICE_PARAM_ADSR_U_BIT_10 (1 << 10)
#define VOICE_PARAM_ADSR_U_BIT_13 (1 << 13)
#define VOICE_PARAM_ADSR_U_BIT_14 (1 << 14)
#define VOICE_PARAM_ADSR_UPPER    (VOICE_PARAM_ADSR_U_BIT_09 | VOICE_PARAM_ADSR_U_BIT_10 | VOICE_PARAM_ADSR_U_BIT_13 | VOICE_PARAM_ADSR_U_BIT_14)
#define VOICE_PARAM_LOOP_ADDR     (1 << 16)

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

#define SEMITONES_IN_OCTAVE (12)

// Semitone pitch multipliers (fixed-point, 0x1000 = 1.0)
const u32 g_SemitonePitchTable[SEMITONES_IN_OCTAVE] = {
    0x00001000,  // C  - base pitch (1.0)
    0x000010F3,  // C# - 2^(1/12)
    0x000011F5,  // D  - 2^(2/12)
    0x00001306,  // D# - 2^(3/12)
    0x00001428,  // E  - 2^(4/12)
    0x0000155B,  // F  - 2^(5/12)
    0x000016A0,  // F# - 2^(6/12)
    0x000017F9,  // G  - 2^(7/12)
    0x00001966,  // G# - 2^(8/12)
    0x00001AE8,  // A  - 2^(9/12)
    0x00001C82,  // A# - 2^(10/12)
    0x00001E34   // B  - 2^(11/12)
};

extern s16 D_80092A64;
extern s32 g_CdVolume;

void SetVoiceVolume( s32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale );
void SetVoiceSampleRate( s32 in_VoiceIndex, s32 in_SampleRate );
void SetVoiceParamsByFlags( u32 in_VoiceIndex, FSoundVoiceParams *in_VoiceParams );
void UpdateCdVolume();