#include "common.h"
#include "psyq/libspu.h"

#define VOICE_COUNT 24
#define SOUND_CHANNEL_COUNT 0x20


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
    /* 0x00 */ u32 AssignedVoiceNumber;
    /* 0x04 */ u32 VoiceParamFlags;
    /* 0x08 */ u32 StartAddress;
    /* 0x0C */ u32 LoopAddress;
    /* 0x10 */ u16 SampleRate;
    /* 0x12 */ u16 AdsrLower;
    /* 0x14 */ u16 AdsrUpper;
    /* 0x16 */ u16 VolumeScale;
    /* 0x18 */ SpuVolume Volume;
} FSoundVoiceParams; /* size 0x1C */

typedef u8 undefined;

typedef struct
{
    /* 0x000 */ u8*  ProgramCounter;
    /* 0x004 */ u32  LoopPoints;
    /* 0x008 */ s32  field2_0x8;
    /* 0x00C */ s32  field3_0xc;
    /* 0x010 */ u16  field4_0x10;
    /* 0x012 */ u16  field5_0x12;
    /* 0x014 */ u16  field6_0x14;
    /* 0x016 */ u16  field7_0x16;
    /* 0x018 */ s16  field8_0x18;
    /* 0x01A */ s16  field9_0x1a;
    /* 0x01C */ s16* VibratoWave;
    /* 0x020 */ s32  TremeloWave;
    /* 0x024 */ u32  PanLfoWave;
    /* 0x028 */ s32  unk_Flags;
    /* 0x02C */ s32  PitchBase;
    /* 0x030 */ s32  PitchSlide;
    /* 0x034 */ s32  UpdateFlags;
    /* 0x038 */ u32  field17_0x38;
    /* 0x03C */ s32  field18_0x3c;
    /* 0x040 */ u32  field19_0x40;
    /* 0x044 */ s32  field20_0x44;
    /* 0x048 */ s32  field21_0x48;
    /* 0x04C */ s32  field22_0x4c;
    /* 0x050 */ s16  field23_0x50;
    /* 0x052 */ s16  field24_0x52;
    /* 0x054 */ u32  field25_0x54;
    /* 0x058 */ u8   field26_0x58;
    /* 0x059 */ u8   field27_0x59;
    /* 0x05A */ u8   field28_0x5a;
    /* 0x05B */ u8   field29_0x5b;
    /* 0x05C */ s32  Volume;
    /* 0x060 */ s32  VolumeSlideStep;
    /* 0x064 */ s32  PitchSlideStep;
    /* 0x068 */ u32  FinePitchDelta;
    /* 0x06C */ s32  field34_0x6c;
    /* 0x070 */ s32  field35_0x70;
    /* 0x074 */ s32  field36_0x74;
    /* 0x078 */ s16  Type; /* Music, SFX, Menu */
    /* 0x07A */ s16  Length1;
    /* 0x07C */ s16  Length2;
    /* 0x07E */ u16  InstrumentIndex;
    /* 0x080 */ u16  field41_0x80;
    /* 0x082 */ s16  field42_0x82;
    /* 0x084 */ s16  field43_0x84;
    /* 0x086 */ u16  LoopTimes;
    /* 0x088 */ u8   field45_0x88;
    /* 0x089 */ u8   field46_0x89;
    /* 0x08A */ u8   field47_0x8a;
    /* 0x08B */ u8   field48_0x8b;
    /* 0x08C */ u8   field49_0x8c;
    /* 0x08D */ u8   field50_0x8d;
    /* 0x08E */ s16  unkArray2;
    /* 0x090 */ s32  field52_0x90;
    /* 0x094 */ u8   field53_0x94;
    /* 0x095 */ u8   field54_0x95;
    /* 0x096 */ u16  VolumeBalance; /* Volume is set by "volume << 8" */
    /* 0x098 */ s16  VolumeBalanceSlideLength;
    /* 0x09A */ s16  field57_0x9a;
    /* 0x09C */ s16  ChannelVolumeSlideLength;
    /* 0x09E */ s16  field59_0x9e;
    /* 0x0A0 */ s16  field60_0xa0;
    /* 0x0A2 */ u16  ChannelPan;
    /* 0x0A4 */ u16  ChannelPanSlideLength;
    /* 0x0A6 */ u16  PitchSlideStepsCurrent;
    /* 0x0A8 */ u16  Octave;
    /* 0x0AA */ s16  PitchBendSlideLength;
    /* 0x0AC */ u16  KeyStored;
    /* 0x0AE */ s16  PortamentoSteps;
    /* 0x0B0 */ u16  SfxMask;
    /* 0x0B2 */ s16  VibratoDelay;
    /* 0x0B4 */ s16  VibratoDelayCurrent;
    /* 0x0B6 */ s16  VibratoRate;
    /* 0x0B8 */ s16  field72_0xb8;
    /* 0x0BA */ u16  VibratoType;
    /* 0x0BC */ u16  VibratoBase;
    /* 0x0BE */ u16  VibratoDepth;
    /* 0x0C0 */ s16  VibratoDepthSlideLength;
    /* 0x0C2 */ u16  VibratoDepthSlideStep;
    /* 0x0C4 */ s16  TremeloDelay;
    /* 0x0C6 */ s16  TremeloDelayCurrent;
    /* 0x0C8 */ s16  TremeloRate;
    /* 0x0CA */ s16  field81_0xca;
    /* 0x0CC */ u16  TremeloType;
    /* 0x0CE */ u16  TremeloDepth;
    /* 0x0D0 */ u16  TremeloDepthSlideLength;
    /* 0x0D2 */ s16  TremeloDepthSlideStep;
    /* 0x0D4 */ u16  PanLfoRate;
    /* 0x0D6 */ u16  PanLfoRateCurrent;
    /* 0x0D8 */ u16  PanLfoType;
    /* 0x0DA */ u16  PanLfoDepth;
    /* 0x0DC */ u16  PanLfoDepthSlideLength;
    /* 0x0DE */ s16  PanLfoDepthSlideStep;
    /* 0x0E0 */ s16  NoiseTimer;
    /* 0x0E2 */ s16  FmTimer;
    /* 0x0E4 */ u16  LoopIndex;
    /* 0x0E6 */ u16  field95_0xe6;
    /* 0x0E8 */ s16  LengthStored;
    /* 0x0EA */ u16  LengthFixed;
    /* 0x0EC */ s16  field98_0xec;
    /* 0x0EE */ s16  field99_0xee;
    /* 0x0F0 */ u16  field100_0xf0;
    /* 0x0F2 */ u8   field101_0xf2;
    /* 0x0F3 */ u8   field102_0xf3;
    /* 0x0F4 */ s16  PanSlideStep;
    /* 0x0F6 */ s16  Transpose;
    /* 0x0F8 */ s16  FineTune;
    /* 0x0FA */ s16  Key;
    /* 0x0FC */ s16  PitchBendSlideTranspose;
    /* 0x0FE */ s16  TransposeStored;
    /* 0x100 */ s16  VibratoPitch;
    /* 0x102 */ s16  TremeloVolume;
    /* 0x104 */ s16  PanLfoVolume;
    /* 0x106 */ u8   field112_0x106;
    /* 0x107 */ u8   field113_0x107;
    /* 0x108 */ FSoundVoiceParams VoiceParams;
} FSoundChannel; /* size 0x124 */

extern s16 D_80092A64;
extern s32 g_CdVolume;

void SetVoiceVolume( s32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale );
void SetVoiceSampleRate( s32 in_VoiceIndex, s32 in_SampleRate );
void SetVoiceParamsByFlags( u32 in_VoiceIndex, FSoundVoiceParams *in_VoiceParams );
void UpdateCdVolume();
