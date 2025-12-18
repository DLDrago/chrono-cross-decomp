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

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", SetVoiceVolume );

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceSampleRate( s32 in_VoiceIndex, s16 in_SampleRate )
{
    VOICE_00_ADPCM_SAMPLE_RATE[in_VoiceIndex * ( sizeof( SPU_VOICE_REG ) / sizeof( u16 ) )] = in_SampleRate;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceStartAddr( u32 in_VoiceIndex, u32 in_Addr )
{
    VOICE_00_ADPCM_START_ADDR[in_VoiceIndex * ( sizeof( SPU_VOICE_REG ) / sizeof( u16 ) )] = ( in_Addr >> 3 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceRepeatAddr( u32 in_VoiceIndex, u32 in_Addr )
{
    VOICE_00_ADPCM_REPEAT_ADDR[in_VoiceIndex * ( sizeof( SPU_VOICE_REG ) / sizeof( u16 ) )] = ( in_Addr >> 3 );
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrLower( s32 in_VoiceIndex, s16 in_Register )
{
    VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * ( sizeof( SPU_VOICE_REG ) / sizeof( u16 ) )] = in_Register;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrUpper( s32 in_VoiceIndex, s16 in_Register )
{
    VOICE_00_ADPCM_ADSR_UPPER[in_VoiceIndex * ( sizeof( SPU_VOICE_REG ) / sizeof( u16 ) )] = in_Register;
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
    u16* AdsrLower = &VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * 8];
    // Extract Attack Mode bit (bit 2 of in_AttackRate -> bit 15 of ADSR)
    u16 AttackMode = (in_AttackMode >> 2) << ADSR_ATTACK_MODE_BIT;
    // Position Attack Step (0-3) at bits 8-9
    u16 AttackStep = in_AttackStep << ADSR_ATTACK_STEP_POS;
    *AdsrLower = AttackMode | AttackStep | *(u8*)AdsrLower;
}

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004BE18 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004BE40 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004BE68 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004BE9C );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", SetVoiceParams );

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
