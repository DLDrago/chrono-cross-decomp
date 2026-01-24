#include "common.h"
#include "psyq/libspu.h"

#define VOICE_COUNT 24
#define SOUND_CHANNEL_COUNT 0x20

// Voice parameter update flags
#define VOICE_PARAM_VOLUME_L      (1 << 0) /* volume (left) */
#define VOICE_PARAM_VOLUME_R      (1 << 1) /* volume (right) */
#define VOICE_PARAM_VOLUME        (VOICE_PARAM_VOLUME_L | VOICE_PARAM_VOLUME_R)
#define	VOICE_PARAM_VOLMODE_L     (1 <<  2) /* volume mode (left) */
#define	VOICE_PARAM_VOLMODE_R     (1 <<  3) /* volume mode (right) */
#define VOICE_PARAM_SAMPLE_RATE   (1 <<  4) /* tone (pitch setting)  */
#define	VOICE_PARAM_NOTE          (1 <<  5) /* tone (note setting)  */
#define	VOICE_PARAM_SAMPLE_NOTE   (1 <<  6) /* waveform data sample note */
#define VOICE_PARAM_START_ADDR    (1 <<  7) /* waveform data start address */
#define	VOICE_PARAM_ADSR_AMODE    (1 <<  8) /* ADSR Attack rate mode */
#define	VOICE_PARAM_ADSR_SMODE    (1 <<  9) /* ADSR Sustain rate mode */
#define	VOICE_PARAM_ADSR_RMODE    (1 << 10) /* ADSR Release rate mode */
#define	VOICE_PARAM_ADSR_AR       (1 << 11) /* ADSR Attack rate */
#define	VOICE_PARAM_ADSR_DR       (1 << 12) /* ADSR Decay rate */
#define	VOICE_PARAM_ADSR_SR       (1 << 13) /* ADSR Sustain rate */
#define	VOICE_PARAM_ADSR_RR       (1 << 14) /* ADSR Release rate */
#define	VOICE_PARAM_ADSR_SL       (1 << 15) /* ADSR Sustain level */
#define VOICE_PARAM_ADSR_LOWER    (VOICE_PARAM_ADSR_AMODE | VOICE_PARAM_ADSR_AR | VOICE_PARAM_ADSR_DR | VOICE_PARAM_ADSR_SL)
#define VOICE_PARAM_ADSR_UPPER    (VOICE_PARAM_ADSR_SMODE | VOICE_PARAM_ADSR_RMODE | VOICE_PARAM_ADSR_SR | VOICE_PARAM_ADSR_RR)
#define VOICE_PARAM_ADSR_FULL     (VOICE_PARAM_ADSR_LOWER | VOICE_PARAM_ADSR_UPPER)
#define VOICE_PARAM_LOOP_ADDR     (1 << 16) /* start address for loop */
#define	VOICE_PARAM_ADSR_ADSR1    (1 << 17) /* ADSR adsr1 for `VagAtr'  */
#define	VOICE_PARAM_ADSR_ADSR2    (1 << 18) /* ADSR adsr2 for `VagAtr'  */

#define SOUND_CHANNEL_TYPE_MUSIC 0x0
#define SOUND_CHANNEL_TYPE_SOUND 0x1
#define SOUND_CHANNEL_TYPE_MENU  0x2

#define SOUND_CHANNEL_CONFIG_STEREO          ( 1 << 0 )
#define SOUND_CHANNEL_CONFIG_MONO            ( 1 << 1 )
#define SOUND_CHANNEL_CONFIG_STEREO_CHANNELS ( 1 << 2 )

#define SOUND_SFX_LEGATO      0x1
#define SOUND_SFX_FULL_LENGTH 0x4

#define SOUND_UPDATE_SPU_VOICE    (SPU_VOICE_VOLL       | SPU_VOICE_VOLR)
#define SOUND_UPDATE_SPU_ADSR     (SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_RMODE | \
                                   SPU_VOICE_ADSR_AR    | SPU_VOICE_ADSR_DR    | SPU_VOICE_ADSR_SR | SPU_VOICE_ADSR_RR | SPU_VOICE_ADSR_SL)
#define SOUND_UPDATE_SPU_BASE_WOR (SPU_VOICE_WDSA       | SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | \
                                   SPU_VOICE_ADSR_AR    | SPU_VOICE_ADSR_DR    | SPU_VOICE_ADSR_SR | \
                                   SPU_VOICE_ADSR_SL    | SPU_VOICE_LSAX)
#define SOUND_UPDATE_SPU_BASE     (SOUND_UPDATE_SPU_BASE_WOR | SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_RR)
#define SOUND_UPDATE_SPU_ALL      (SOUND_UPDATE_SPU_BASE     | SOUND_UPDATE_SPU_VOICE    | SPU_VOICE_PITCH)

#define SOUND_UPDATE_VIBRATO          ( 1 <<  0 )
#define SOUND_UPDATE_TREMOLO          ( 1 <<  1 )
#define SOUND_UPDATE_PAN_LFO          ( 1 <<  2 )
#define SOUND_UPDATE_DRUM_MODE        ( 1 <<  3 )
#define SOUND_UPDATE_SIDE_CHAIN_PITCH ( 1 <<  4 )
#define SOUND_UPDATE_SIDE_CHAIN_VOL   ( 1 <<  5 )
#define SOUND_UPDATE_UNKNOWN_01       ( 1 <<  6 )
#define SOUND_UPDATE_OVERLAY          ( 1 <<  8 )
#define SOUND_UPDATE_ALTERNATIVE      ( 1 <<  9 )
#define SOUND_UPDATE_UNKNOWN_12       ( 1 << 12 )
#define SOUND_UPDATE_UNKNOWN_24       ( 1 << 24 )
#define SOUND_UPDATE_UNKNOWN_27       ( 1 << 27 )
#define SOUND_UPDATE_UNKNOWN_28       ( 1 << 28 )


#define BIT_MASK(width)               ((1u << (width)) - 1u)
#define FIELD_MASK(width, shift)      (BIT_MASK(width) << (shift))

/*
 * ADSR1 (Lower)
 * 15 14 13 12 11 10 9 8 | 7 6 5 4 | 3 2 1 0
 *  ---------------------+---------+--------
 *   Attack Rate / Mode  | Decay   | Sustain Level
 */
#define SOUND_ADSR_ATTACK_MODE_MASK    (BIT_MASK(SOUND_ADSR_ATTACK_MODE_WIDTH) << SOUND_ADSR_ATTACK_MODE_SHIFT) // 0x8000  // bit 15
#define SOUND_ADSR_ATTACK_RATE_MASK    (BIT_MASK(SOUND_ADSR_ATTACK_RATE_WIDTH) << SOUND_ADSR_ATTACK_RATE_SHIFT) // 0x7F00  // bits 14–8
#define SOUND_ADSR_DECAY_RATE_MASK     (BIT_MASK(SOUND_ADSR_DECAY_RATE_WIDTH ) << SOUND_ADSR_DECAY_RATE_SHIFT ) // 0x00F0  // bits 7–4
#define SOUND_ADSR_SUS_LEVEL_MASK      (BIT_MASK(SOUND_ADSR_SUS_LEVEL_WIDTH  ) << SOUND_ADSR_SUS_LEVEL_SHIFT  ) // 0x000F  // bits 3–0

#define SOUND_ADSR_ATTACK_MODE_SHIFT   15
#define SOUND_ADSR_ATTACK_RATE_SHIFT    8
#define SOUND_ADSR_DECAY_RATE_SHIFT     4
#define SOUND_ADSR_SUS_LEVEL_SHIFT      0

#define SOUND_ADSR_ATTACK_MODE_WIDTH    1
#define SOUND_ADSR_ATTACK_RATE_WIDTH    7
#define SOUND_ADSR_DECAY_RATE_WIDTH     4
#define SOUND_ADSR_SUS_LEVEL_WIDTH      4

/*
 * ADSR2 (Upper)
 TODO(jperos) THESE GRAPHS ARE WRONG
 * 15 14 | 13 12 11 10 9 8 | 7 6 5 4 | 3 2 1 0
 * ------+-----------------+---------+---------
 * SMode | Sustain Rate    | Rel.    | RR
 */
#define SOUND_ADSR_SUS_MODE_MASK     (BIT_MASK(SOUND_ADSR_SUS_MODE_WIDTH    ) << SOUND_ADSR_SUS_MODE_SHIFT    ) //   0xC000
#define SOUND_ADSR_SUS_RATE_MASK     (BIT_MASK(SOUND_ADSR_SUS_RATE_WIDTH    ) << SOUND_ADSR_SUS_RATE_SHIFT    ) //   0x3F00
#define SOUND_ADSR_RELEASE_MODE_MASK (BIT_MASK(SOUND_ADSR_RELEASE_MODE_WIDTH) << SOUND_ADSR_RELEASE_MODE_SHIFT) //   0x00E0
#define SOUND_ADSR_RELEASE_RATE_MASK (BIT_MASK(SOUND_ADSR_RELEASE_RATE_WIDTH) << SOUND_ADSR_RELEASE_RATE_SHIFT) //   0x00E0

#define SOUND_ADSR_SUS_MODE_SHIFT      14
#define SOUND_ADSR_SUS_RATE_SHIFT       6
#define SOUND_ADSR_RELEASE_MODE_SHIFT   5
#define SOUND_ADSR_RELEASE_RATE_SHIFT   0

#define SOUND_ADSR_SUS_MODE_WIDTH       2
#define SOUND_ADSR_SUS_RATE_WIDTH       7
#define SOUND_ADSR_RELEASE_MODE_WIDTH   1
#define SOUND_ADSR_RELEASE_RATE_WIDTH   5

#define SOUND_AMODE_1                   1
#define SOUND_AMODE_5                   5
#define SOUND_RMODE_7                   7

#define VIBRATO_FLAG_ABSOLUTE         ( 1 << 15 )

#define SOUND_UPDATE_NOISE_CLOCK 0x10
#define SOUND_UPDATE_REVERB      0x80


typedef struct
{
    u32 ControlLatches;   // one-shot / transactional engine state flags
    u32 MixBehavior;      // global mixing & music-stack behavior flags
    u32 UpdateFlags;      // deferred SPU / voice-mode update flags
} FSoundGlobalFlags;

/* =========================
 * ControlLatches
 * ========================= */

/* SPU instrument upload / relocation transaction in progress.
 * - Set before streaming instrument data
 * - Cleared when transfer finishes
 * - Gates unk_Spu_8004ac2c()
 */
#define SOUND_CTL_INSTRUMENT_TRANSFER_ACTIVE     (1u << 0)

/* Last-active SFX channel fade completion should trigger VM command.
 * - Set when a global SFX fade starts
 * - Checked when C_StepsRemaining reaches 0 on final channel
 * - Causes Sound_Cmd_80050dd4()
 */
#define SOUND_CTL_SFX_FADE_END_CALLBACK_PENDING  (1u << 16)


/* =========================
 * MixBehavior
 * ========================= */

/* Force dual-mono output.
 * - Bypasses pan law table
 * - L = R using 0x440a scale
 * - Affects:
 *     - Music voices
 *     - SFX voices
 *     - CD audio mixing
 */
#define SOUND_MIX_FORCE_MONO                    (1u << 1)

/* Secondary (pushed) music is in fade / teardown phase.
 * - Enables periodic fade processing
 * - Delays cleanup until fade completes
 * - Cleared automatically when channels die
 */
#define SOUND_MIX_SECONDARY_MUSIC_FADING        (1u << 8)

/* Legacy / initialization bit.
 * - Written (set to 1) but not meaningfully read in observed paths
 * - Likely historical or reserved
 */
#define SOUND_MIX_LEGACY_ENABLE                 (1u << 0)

typedef struct
{
    /* 0x00 */ u32 StartAddr;
    /* 0x04 */ u32 LoopAddr;
    /* 0x08 */ s16 FineTune;
    /* 0x0A */ s16 SampleNote;
    /* 0x0C */ u16 AdsrLower;
    /* 0x0E */ u16 AdsrUpper;
} FSoundInstrumentInfo; /* size 0x10 */

typedef struct
{
    u32 ActiveChannelMask;
    u32 KeyOnFlags;
    u32 KeyedFlags;
    u32 KeyOffFlags;
    undefined4 unk_Flags_0x10;
    undefined4 field5_0x14;
    undefined4 TempoAccumumulator;
    undefined4 NoiseVoiceFlags;
    undefined4 ReverbVoiceFlags;
    undefined4 FmVoiceFlags;
    undefined2 NoiseClock;
    undefined field11_0x2a;
    undefined field12_0x2b;
    undefined4 field13_0x2c;
} FSoundVoiceSchedulerState;

typedef struct
{
    s32 field0_0x0;
    s32 unk_Mask_0x4;
    s32 field2_0x8;
    s32 VoicesInUseFlags;
    s32 VoiceIndex;
    s32 ChannelFlags;
    s32 field6_0x18;
    s32 field7_0x1c;
    s32 field8_0x20;
    s32 field9_0x24;
    s32 field10_0x28;
    s32 field11_0x2c;
    s32 field12_0x30;
    s32 field13_0x34;
    s32 field14_0x38;
    s32 field15_0x3c;
    s32 Volume;
    s32 field17_0x44;
    s32 field18_0x48;
    u8 field19_0x4c;
    u8 field20_0x4d;
    u8 field21_0x4e;
    u8 field22_0x4f;
    s32 field23_0x50;
    s32 field24_0x54;
    s32 VoiceSampleRate;
    s32 ControlFlags;
    struct FSoundChannelConfig *SomeSFXArray[24];
} FSound80094FA0;

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

#define SOUND_LOOP_STACK_SIZE (4)
#define SOUND_LOOP_STACK_MAX_INDEX (SOUND_LOOP_STACK_SIZE - 1)

typedef struct
{
    /* 0x000 */ u8*  ProgramCounter;
    /* 0x004 */ u8*  LoopStartPc[SOUND_LOOP_STACK_SIZE];
    /* 0x014 */ u8*  ReturnProgramCounter;
    /* 0x018 */ s16  field8_0x18;
    /* 0x01A */ s16  field9_0x1a;
    /* 0x01C */ s16* VibratoWave;
    /* 0x020 */ u32  TremeloWave;
    /* 0x024 */ u32  AutoPanWave;
    /* 0x028 */ s32  unk_Flags;
    /* 0x02C */ s32  PitchBase;
    /* 0x030 */ s32  PitchSlide;
    /* 0x034 */ s32  UpdateFlags;
    /* 0x038 */ u32  VibratoRatePhase;
    /* 0x03C */ s32  VibratoRateSlideStep;
    /* 0x040 */ u32  TremeloRatePhase;
    /* 0x044 */ s32  TremeloRateSlideStep;
    /* 0x048 */ s32  AutoPanRatePhase;
    /* 0x04C */ s32  AutoPanRateSlideStep;
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
    /* 0x078 */ u16  Type; /* Music, SFX, Menu */
    /* 0x07A */ s16  Length1;
    /* 0x07C */ s16  Length2;
    /* 0x07E */ u16  InstrumentIndex;
    /* 0x080 */ u16  field41_0x80;
    /* 0x082 */ s16  field42_0x82;
    /* 0x084 */ s16  OpcodeStepCounter;
    /* 0x086 */ u16  LoopIterationCount[SOUND_LOOP_STACK_SIZE];
    /* 0x08E */ s16  LoopStepCounterSnapshot[SOUND_LOOP_STACK_SIZE];
    /* 0x096 */ u16  VolumeBalance; /* Volume is set by "volume << 8" */
    /* 0x098 */ s16  VolumeBalanceSlideLength;
    /* 0x09A */ s16  field57_0x9a;
    /* 0x09C */ u16  ChannelVolumeSlideLength;
    /* 0x09E */ s16  KeyOnVolumeRampLength;
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
    /* 0x0B6 */ s16  VibratoRateSlideLength;
    /* 0x0B8 */ s16  field72_0xb8;
    /* 0x0BA */ u16  VibratoType;
    /* 0x0BC */ u16  VibratoBase;
    /* 0x0BE */ u16  VibratoDepth;
    /* 0x0C0 */ s16  VibratoDepthSlideLength;
    /* 0x0C2 */ u16  VibratoDepthSlideStep;
    /* 0x0C4 */ s16  TremeloDelay;
    /* 0x0C6 */ s16  TremeloDelayCurrent;
    /* 0x0C8 */ s16  TremeloRateSlideLength;
    /* 0x0CA */ s16  field81_0xca;
    /* 0x0CC */ u16  TremeloType;
    /* 0x0CE */ u16  TremeloDepth;
    /* 0x0D0 */ u16  TremeloDepthSlideLength;
    /* 0x0D2 */ s16  TremeloDepthSlideStep;
    /* 0x0D4 */ u16  AutoPanRateSlideLength;
    /* 0x0D6 */ u16  AutoPanRateCurrent;
    /* 0x0D8 */ u16  AutoPanType;
    /* 0x0DA */ u16  AutoPanDepth;
    /* 0x0DC */ u16  AutoPanDepthSlideLength;
    /* 0x0DE */ s16  AutoPanDepthSlideStep;
    /* 0x0E0 */ s16  NoiseTimer;
    /* 0x0E2 */ s16  FmTimer;
    /* 0x0E4 */ u16  LoopStackTop;
    /* 0x0E6 */ u16  RandomPitchDepth;
    /* 0x0E8 */ s16  LengthStored;
    /* 0x0EA */ u16  LengthFixed;
    /* 0x0EC */ s16  VolumeBalanceSlideStep;
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
    /* 0x104 */ s16  AutoPanVolume;
    /* 0x106 */ u8   field112_0x106;
    /* 0x107 */ u8   field113_0x107;
    /* 0x108 */ FSoundVoiceParams VoiceParams;
} FSoundChannel; /* size 0x124 */

typedef struct
{
    u8 InstrumentIndex;
    u8 Note;
    u8 AdsrAttackRate;
    u8 AdsrSustainRate;
    u8 SustainModeCode; /* values 3/5/7 map to 0x4000/0x8000/0xC000 */
    u8 ReleaseRate;
    u8 VolumeScale;
    u8 PanAndReverb; /* low 7 bits pan, high bit reverb-enable */
} FSoundKeymapEntry8;

typedef struct
{
    undefined4 unk0;
} FAkaoSequence;

typedef struct 
{
    s32 StatusFlags; /*   0x01 - Voice exhaustion (couldn't allocate even with stealing) 0x02 - Voice stealing occurred */
    u32 ActiveChannelMask;
    u32 KeyedMask; /* SPU voices currently keyed-on */
    u32 AllocatedVoiceMask; /* Channels with SPU voices allocated */
    s32 PendingKeyOnMask; /* Pending key-ons */
    s32 ActiveNoteMask; /* Currently playing notes (not rests) */
    u32 PreventRekeyOnMusicResumeMask; /* With the way music pushes and pops, if something like a one-shot was started and "paused" then it won't rekey/resume that note when the music resumes*/
    s32 PendingKeyOffMask;
    u32 LastChannelModeFlags;
    s32 Tempo;
    s32 TempoSlideStep;
    s32 TempoUpdate;
    FAkaoSequence* SequenceBase;
    s16 *SequencePatchTable;
    FSoundKeymapEntry8* KeymapTable;
    s32 SomeIndexRelatedToSpuVoiceInfo;
    u32 NoiseChannelFlags;
    u32 ReverbChannelFlags;
    u32 FmChannelFlags;
    s32 RevDepth;
    s32 ReverbDepthSlideStep;
    u32 A_Volume;
    s32 A_Step;
    s16 A_StepsRemaining;
    undefined field24_0x5e;
    undefined field25_0x5f;
    u32 B_Volume;
    s32 B_Step;
    s16 B_StepsRemaining;
    s16 ReverbDepthSlideLength;
    u16 TempoSlideLength;
    u16 MusicId;
    u16 JumpThresholdValue;
    u16 NoiseClock;
    s16 TimerUpper;
    s16 TimerUpperCurrent;
    s16 TimerLower;
    s16 TimerLowerCurrent;
    s16 TimerTopCurrent;
    undefined field39_0x7e;
    undefined field40_0x7f;
} FSoundChannelConfig;


#define SEMITONES_IN_OCTAVE (12)

// Semitone pitch multipliers (fixed-point, 0x1000 = 1.0)
// extern const s32 g_SemitonePitchTable[SEMITONES_IN_OCTAVE];
// Only referenced in SoundVM functions - maybe move there if we process the sound VM in that source file
/* {
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
}; */

typedef struct
{
    u32 unk0;
} FMusicSequence;

// Sound
void SetVoiceVolume( s32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale );
void SetVoiceSampleRate( s32 in_VoiceIndex, s32 in_SampleRate );
void SetVoiceParamsByFlags( u32 in_VoiceIndex, FSoundVoiceParams *in_VoiceParams );

// Sound 2
u16 Sound_ApplySampleBankOffsetIfNeeded( u32 in_Flags, FSoundChannel* in_Channel );
void Sound_SetMusicSequence( FMusicSequence *in_Sequence,int in_SwapWithSavedState );

// Sound 3
void UpdateCdVolume();
void memcpy32( void* in_Src, void* in_Dst, uint in_Size );
void memswap32( void* in_A, void* in_B, uint in_Size );
s32 Sound_ComputeSlideStep( u32*, u8, s16, u32 );
void Sound_CopyInstrumentInfoToChannel( FSoundChannel* in_pChannel, FSoundInstrumentInfo* in_pInstrumentInfo, u32 in_StartAddress );
void Sound_ClearVoiceFromSchedulerState( FSoundChannel* in_pChannel, u32 in_VoiceFlags );

// SoundVM
void SoundVM_A0_FinishChannel( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE00_SetTempo( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE01_SetTempoSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE02_SetMasterReverbDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE03_SetMasterReverbSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE06_JumpRelativeOffset( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE07_JumpRelativeWithThreshold( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE0E_CallRelativeOffset( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE0F_Return( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A3_ChannelMasterVolume( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE12_VolumeBalanceSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A8_ChannelVolume( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A9_ChannelVolumeSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE19_80054348( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1A_800543d8( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1B_800543ec( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AA_ChannelPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AB_ChannelPanSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A5_SetOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A6_IncreaseOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A7_DecreaseOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A1_LoadInstrument( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE0A_ClearInstrument( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE14_800545ec( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B3_ResetAdsr( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C0_ChannelTranspose_Absolute( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C1_ChannelTranspose_Relative( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A4_PitchBendSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DA_EnablePortamento( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DB_DisablePortamento( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D8_ChannelFineTune_Absolute( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D9_ChannelFineTune_Relative( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B4_Vibrato( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B5_VibratoDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DD_VibratoDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E4_VibratoRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B6_DisableVibrato( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B8_Tremelo( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B9_TremeloDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DE_TremeloDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E5_TremeloRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BA_DisableTremelo( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BC_AutoPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BD_AutoPanDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DF_AutoPanDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E6_AutoPanRateSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BE_DisableAutoPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C4_EnableNoiseVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C5_DisableNoiseVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C6_EnableFmVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C7_DisableFmVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C2_EnableReverbVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C3_DisableReverbVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CC_EnableLegato( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CD_DEBUG_80055078( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D0_EnableSustainedNote( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D1_DEBUG_8005509c( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AC_NoiseClockFrequency( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AD_AttackRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AE_DecayRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_AF_SustainLevel( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B1_SustainRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B2_ReleaseRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_B7_AttackMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BB_SustainMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BF_ReleaseMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE10_8005536c( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE11_8005538c( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C8_LoopPoint( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_C9_LoopN( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE08_80055480( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE09_800554ec( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CA_LoopInf( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_A2_OverwriteNextNoteLength( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_DC_FixNoteLength( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE04_ClearKeymapTable( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE05_MuteVoice( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE15_8005567c( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE16_800556b4( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_BO_DecayRateAndSustainLevel( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CE_EnableNoiseAndDelayToggle( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CF_ToggleNoiseOnDelay( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D2_EnableFmAndDelayToggle( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D3_ToggleFmDelay( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_CB_DisableVoiceModes( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D4_EnablePlaybackRateSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D5_DisablePlaybackRateSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D6_EnablePitchVolumeSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_D7_DisablePitchVolumeSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE0B_800558cc( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E0_80055944( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1C_IncrementProgramCounter( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1D_MarkVoicesKeyed( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE1E_ClearVoicesKeyed( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E1_SetRandomPitchDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_E2_ResetRandomPitchDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_FE13_PreventVoicesFromRekeyingOnResume( FSoundChannel* in_pChannel, u32 in_VoiceFlags );
void SoundVM_XX_Unimplemented( FSoundChannel* in_pChannel, u32 in_VoiceFlags );

extern u32 g_Sound_ProgramCounter;
extern const u32 g_SemitonePitchTable[SEMITONES_IN_OCTAVE];
extern FSoundChannel g_ActiveMusicChannels[0x20];
extern FSoundChannel* g_pSecondaryMusicChannels;
extern FSoundChannelConfig* g_pSavedMousicConfig; // What even is this used for
extern FSoundChannelConfig g_PushedMusicConfig;
extern FSoundInstrumentInfo g_InstrumentInfo[256];
extern u32 g_Music_LoopCounter;
extern bool g_bSpuTransferring;
extern FSoundChannelConfig* g_pActiveMusicConfig;
extern s16 D_80092A64;
extern FSoundVoiceSchedulerState g_Sound_VoiceSchedulerState;
extern s32 g_CdVolume;
extern FSoundChannel g_PushedMusicChannels[0x20];
extern FSoundGlobalFlags g_Sound_GlobalFlags;
extern FSound80094FA0 D_80094FA0;
