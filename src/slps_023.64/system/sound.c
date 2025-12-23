#include "common.h"
#include "hw.h"
#include "psyq/libspu.h"

typedef struct
{
    SpuVolume volume;
    u16 pitch;
    u16 addr;
    u16 adsr[2];
    u16 volumex;
    u16 loop_addr;
} SPU_VOICE_REG;
#define SPU_VOICE_INDEX_STRIDE  8  // u16s to skip per voice when indexing voice register arrays

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceKeyOn( u32 in_KeyOn )
{
    *SPU_VOICE_KEY_ON_LO = in_KeyOn;
    *SPU_VOICE_KEY_ON_HI = ( in_KeyOn >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceKeyOff( u32 in_KeyOff )
{
    *SPU_VOICE_KEY_OFF_LO = in_KeyOff;
    *SPU_VOICE_KEY_OFF_HI = ( in_KeyOff >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceReverbMode( u32 in_ReverbMode )
{
    *SPU_VOICE_CHN_REVERB_MODE_LO = in_ReverbMode;
    *SPU_VOICE_CHN_REVERB_MODE_HI = ( in_ReverbMode >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceNoiseMode( u32 in_NoiseMode )
{
    *SPU_VOICE_CHN_NOISE_MODE_LO = in_NoiseMode;
    *SPU_VOICE_CHN_NOISE_MODE_HI = ( in_NoiseMode >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceFmMode( u32 in_FmMode )
{
    *SPU_VOICE_CHN_FM_MODE_LO = in_FmMode;
    *SPU_VOICE_CHN_FM_MODE_HI = ( in_FmMode >> 0x10 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceVolume( s32 in_VoiceIndex, u32 in_VolL, u32 in_VolR, u32 in_VolumeScale )
{
    SpuVolume* pVolume;

    if( in_VolumeScale != 0 )
    {
        in_VolL = in_VolL * in_VolumeScale;
        in_VolR = in_VolR * in_VolumeScale;
        in_VolL = in_VolL >> 7;
        in_VolR = in_VolR >> 7;
    }

    pVolume = (SpuVolume*)&VOICE_00_LEFT_RIGHT[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    pVolume->left = in_VolL & 0x7FFF;
    pVolume->right = in_VolR & 0x7FFF;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceSampleRate( s32 in_VoiceIndex, s16 in_SampleRate )
{
    VOICE_00_ADPCM_SAMPLE_RATE[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = in_SampleRate;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceStartAddr( u32 in_VoiceIndex, u32 in_Addr )
{
    VOICE_00_ADPCM_START_ADDR[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = ( in_Addr >> 3 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceRepeatAddr( u32 in_VoiceIndex, u32 in_Addr )
{
    VOICE_00_ADPCM_REPEAT_ADDR[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = ( in_Addr >> 3 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrLower( s32 in_VoiceIndex, s16 in_Register )
{
    VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = in_Register;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrUpper( s32 in_VoiceIndex, s16 in_Register )
{
    VOICE_00_ADPCM_ADSR_UPPER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = in_Register;
}

//----------------------------------------------------------------------------------------------------------------------
// ADSR LOWER 16-bit (at 1F801C08h+N*10h)
//----------------------------------------------------------------------------------------------------------------------

// Bit positions
#define ADSR_ATTACK_MODE_BIT        15
#define ADSR_ATTACK_SHIFT_POS       10  // 5 bits (14-10)
#define ADSR_ATTACK_STEP_POS        8   // 2 bits (9-8)
#define ADSR_DECAY_SHIFT_POS        4   // 4 bits (7-4)
#define ADSR_SUSTAIN_LEVEL_POS      0   // 4 bits (3-0)

// Bit masks (for masking out fields)
#define ADSR_ATTACK_MODE_MASK       (0x1  << ADSR_ATTACK_MODE_BIT  )
#define ADSR_ATTACK_SHIFT_MASK      (0x1F << ADSR_ATTACK_SHIFT_POS )
#define ADSR_ATTACK_STEP_MASK       (0x3  << ADSR_ATTACK_STEP_POS  )
#define ADSR_DECAY_SHIFT_MASK       (0xF  << ADSR_DECAY_SHIFT_POS  )
#define ADSR_SUSTAIN_LEVEL_MASK     (0xF  << ADSR_SUSTAIN_LEVEL_POS)

// Attack Mode values
#define ADSR_ATTACK_MODE_LINEAR      0
#define ADSR_ATTACK_MODE_EXPONENTIAL 1

// Attack Step values ("+7,+6,+5,+4")
#define ADSR_ATTACK_STEP_PLUS_7     0
#define ADSR_ATTACK_STEP_PLUS_6     1
#define ADSR_ATTACK_STEP_PLUS_5     2
#define ADSR_ATTACK_STEP_PLUS_4     3

// Helper macros to build fields
#define ADSR_ATTACK_MODE(mode)      ((mode) << ADSR_ATTACK_MODE_BIT)
#define ADSR_ATTACK_SHIFT(shift)    (((shift) & 0x1F) << ADSR_ATTACK_SHIFT_POS )
#define ADSR_ATTACK_STEP(step)      (((step)  & 0x3)  << ADSR_ATTACK_STEP_POS  )
#define ADSR_DECAY_SHIFT(shift)     (((shift) & 0xF)  << ADSR_DECAY_SHIFT_POS  )
#define ADSR_SUSTAIN_LEVEL(level)   (((level) & 0xF)  << ADSR_SUSTAIN_LEVEL_POS)

//----------------------------------------------------------------------------------------------------------------------
// ADSR UPPER 16-bit (at 1F801C0Ah+N*10h)
//----------------------------------------------------------------------------------------------------------------------

// Bit positions (relative to the upper 16-bit word, bits 0-15)
#define ADSR_SUSTAIN_MODE_BIT       15
#define ADSR_SUSTAIN_DIRECTION_BIT  14
// Bit 13 unused
#define ADSR_SUSTAIN_SHIFT_POS      8   // 5 bits (12-8)
#define ADSR_SUSTAIN_STEP_POS       6   // 2 bits (7-6)
#define ADSR_RELEASE_MODE_BIT       5
#define ADSR_RELEASE_SHIFT_POS      0   // 5 bits (4-0)

// Bit masks (for masking out fields)
#define ADSR_SUSTAIN_MODE_MASK      (0x1  << ADSR_SUSTAIN_MODE_BIT     )
#define ADSR_SUSTAIN_DIRECTION_MASK (0x1  << ADSR_SUSTAIN_DIRECTION_BIT)
#define ADSR_SUSTAIN_SHIFT_MASK     (0x1F << ADSR_SUSTAIN_SHIFT_POS    )
#define ADSR_SUSTAIN_STEP_MASK      (0x3  << ADSR_SUSTAIN_STEP_POS     )
#define ADSR_RELEASE_MODE_MASK      (0x1  << ADSR_RELEASE_MODE_BIT     )
#define ADSR_RELEASE_SHIFT_MASK     (0x1F << ADSR_RELEASE_SHIFT_POS    )

// Sustain Mode values
#define ADSR_SUSTAIN_MODE_LINEAR      0
#define ADSR_SUSTAIN_MODE_EXPONENTIAL 1

// Sustain Direction values
#define ADSR_SUSTAIN_DIR_INCREASE   0
#define ADSR_SUSTAIN_DIR_DECREASE   1

// Sustain Step values
// When Increase: "+7,+6,+5,+4"
#define ADSR_SUSTAIN_STEP_INC_PLUS_7  0
#define ADSR_SUSTAIN_STEP_INC_PLUS_6  1
#define ADSR_SUSTAIN_STEP_INC_PLUS_5  2
#define ADSR_SUSTAIN_STEP_INC_PLUS_4  3

// When Decrease: "-8,-7,-6,-5"
#define ADSR_SUSTAIN_STEP_DEC_MINUS_8 0
#define ADSR_SUSTAIN_STEP_DEC_MINUS_7 1
#define ADSR_SUSTAIN_STEP_DEC_MINUS_6 2
#define ADSR_SUSTAIN_STEP_DEC_MINUS_5 3

// Release Mode values
#define ADSR_RELEASE_MODE_LINEAR      0
#define ADSR_RELEASE_MODE_EXPONENTIAL 1

// Helper macros to build fields
#define ADSR_SUSTAIN_MODE(mode)       ((mode) << ADSR_SUSTAIN_MODE_BIT)
#define ADSR_SUSTAIN_DIRECTION(dir)   ((dir)  << ADSR_SUSTAIN_DIRECTION_BIT)
#define ADSR_SUSTAIN_SHIFT(shift)     (((shift) & 0x1F) << ADSR_SUSTAIN_SHIFT_POS)
#define ADSR_SUSTAIN_STEP(step)       (((step)  & 0x3 ) << ADSR_SUSTAIN_STEP_POS)
#define ADSR_RELEASE_MODE(mode)       ((mode) << ADSR_RELEASE_MODE_BIT)
#define ADSR_RELEASE_SHIFT(shift)     (((shift) & 0x1F) << ADSR_RELEASE_SHIFT_POS)

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrAttackRateAndMode( s32 in_VoiceIndex, s32 in_AttackStep, u32 in_AttackMode )
{
    u16* AdsrLower = &VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    // Extract Attack Mode bit (bit 2 of in_AttackRate -> bit 15 of ADSR)
    u16 AttackMode = ADSR_ATTACK_MODE(in_AttackMode >> 2);
    // Position Attack Step (0-3) at bits 8-9
    u16 AttackStep = in_AttackStep << ADSR_ATTACK_STEP_POS;
    *AdsrLower = AttackMode | AttackStep | *(u8*)AdsrLower;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrDecayRate( s32 in_VoiceIndex, s32 in_DecayRate )
{
    u16* AdsrLower = &VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    u16 AttackStep = in_DecayRate * 0x10;
    u16 Masked = ( *AdsrLower & 0xFF0F );
    *AdsrLower = Masked | AttackStep;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrSustainLevel( s32 in_VoiceIndex, s32 in_SustainLevel )
{
    u16* AdsrLower = &VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    *AdsrLower = (*AdsrLower & 0xFFF0) | in_SustainLevel;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrSustainRateAndDirection( s32 in_VoiceIndex, s32 in_SustainRate, u32 in_SustainDirection )
{
    u16* AdsrUpper = &VOICE_00_ADPCM_ADSR_UPPER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    u16 SustainDirection = ADSR_SUSTAIN_DIRECTION( in_SustainDirection >> 1 );
    u16 SustainRate = in_SustainRate << ADSR_SUSTAIN_STEP_POS;
    u16 Masked = *AdsrUpper & 0x3F;
    *AdsrUpper = Masked | ( SustainDirection | SustainRate );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrReleaseRateAndMode( s32 in_VoiceIndex, s32 in_ReleaseRate, u32 in_ReleaseMode )
{
    u16* AdsrUpper = &VOICE_00_ADPCM_ADSR_UPPER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    u16 ReleaseMode = (in_ReleaseMode >> 2) << ADSR_RELEASE_MODE_BIT;
    u16 ReleaseRate = in_ReleaseRate << ADSR_RELEASE_SHIFT_POS;
    u16 Masked = *AdsrUpper & 0xFFC0;
    *AdsrUpper = Masked | ( ReleaseMode | ReleaseRate);
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", SetVoiceParams);

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004BF78 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004C094 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004C5A4 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CA1C );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CEAC );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CF5C );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CFC4 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D214 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D294 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D374 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D3C4 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D3D4 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D918 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D978 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004DB24 );
