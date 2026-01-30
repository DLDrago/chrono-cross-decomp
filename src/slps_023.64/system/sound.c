#include "common.h"
#include "hw.h"
#include "psyq/libspu.h"
#include "system/sound.h"

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
void SetVoiceSampleRate( s32 in_VoiceIndex, s32 in_SampleRate )
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
void SetVoiceAdsrLower( s32 in_VoiceIndex, u16 in_Register )
{
    VOICE_00_ADPCM_ADSR_LOWER[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE] = in_Register;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceAdsrUpper( s32 in_VoiceIndex, u16 in_Register )
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
void SetVoiceParams( s32 in_VoiceIndex, FSoundVoiceParams* in_VoiceParams, s32 in_VolumeScale )
{
    s32 left;
    s32 right;
    s16* p;

    in_VoiceParams->VoiceParamFlags = 0;
    p = (s16*)&VOICE_00_LEFT_RIGHT[in_VoiceIndex * SPU_VOICE_INDEX_STRIDE];
    if( in_VolumeScale == 0 )
    {
        left = in_VoiceParams->Volume.left;
        right = in_VoiceParams->Volume.right;
    }
    else
    {
        left = in_VoiceParams->Volume.left * in_VolumeScale;
        right = in_VoiceParams->Volume.right * in_VolumeScale;
        left >>= 7;
        right >>=  7;
    }

    // This is the dumbest shit, but I can't find any other way that compiles correctly
    *p++ = left & 0x7FFF;
    *p++ = right & 0x7FFF;
    *p++ = in_VoiceParams->SampleRate;
    *p++ = in_VoiceParams->StartAddress >> 3;
    *p++ = in_VoiceParams->AdsrLower;
    *p++ = in_VoiceParams->AdsrUpper;
    p++;
    *p = in_VoiceParams->LoopAddress >> 3;
}

//----------------------------------------------------------------------------------------------------------------------
void SetVoiceParamsByFlags( u32 in_VoiceIndex, FSoundVoiceParams* in_VoiceParams )
{
    s32 flags;

    flags = in_VoiceParams->VoiceParamFlags;
    if( flags == 0 )
    {
        return;
    }

    in_VoiceParams->VoiceParamFlags = 0;

    if( flags & VOICE_PARAM_SAMPLE_RATE )
    {
        flags &= ~VOICE_PARAM_SAMPLE_RATE;
        SetVoiceSampleRate( in_VoiceIndex, in_VoiceParams->SampleRate );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_VOLUME )
    {
        flags &= ~VOICE_PARAM_VOLUME;
        SetVoiceVolume( in_VoiceIndex, in_VoiceParams->Volume.left, in_VoiceParams->Volume.right, in_VoiceParams->VolumeScale );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_START_ADDR )
    {
        flags &= ~VOICE_PARAM_START_ADDR;
        SetVoiceStartAddr( in_VoiceIndex, in_VoiceParams->StartAddress );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_LOOP_ADDR )
    {
        flags &= ~VOICE_PARAM_LOOP_ADDR;
        SetVoiceRepeatAddr( in_VoiceIndex, in_VoiceParams->LoopAddress );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_ADSR_UPPER )
    {
        flags &= ~VOICE_PARAM_ADSR_UPPER;
        SetVoiceAdsrUpper( in_VoiceIndex, in_VoiceParams->AdsrUpper );
        if( flags == 0 ) return;
    }

    if( flags & VOICE_PARAM_ADSR_LOWER )
    {
        SetVoiceAdsrLower( in_VoiceIndex, in_VoiceParams->AdsrLower );
    }
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NON_MATCHING
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", Sound_UpdateSlidesAndDelays );
#else
void Sound_UpdateSlidesAndDelays( FSoundChannel* in_pChannel, u32 in_VoiceFlags, s32 in_Arg2 )
{
    s16* var_a0_2;
    s32 NewVolume;
    s32 temp_a3_4;
    s32 temp_a3_5;
    s32 temp_a3_6;
    s32 temp_a3_7;
    s32 Volume;
    s32 temp_v1_8;
    u16 NewBalance;
    u16 NewPan;
    u16 temp_v0_10;
    u16 VolumeBalance;
    u16 Pan;
    u32 temp_a0;
    u32 var_lo;

    if( in_pChannel->ChannelVolumeSlideLength != 0 )
    {
        Volume = in_pChannel->Volume;
        in_pChannel->ChannelVolumeSlideLength--;
        NewVolume = Volume + in_pChannel->VolumeSlideStep;
        if( (NewVolume & 0xFFE00000) != (Volume & 0xFFE00000) )
        {
            in_pChannel->VoiceParams.VoiceParamFlags |= 3;
        }
        in_pChannel->Volume = NewVolume;
    }

    if( in_Arg2 == 0 )
    {
        if( in_pChannel->VolumeBalanceSlideLength != 0 )
        {
            VolumeBalance = in_pChannel->VolumeBalance;
            in_pChannel->VolumeBalanceSlideLength--;
            NewBalance = VolumeBalance + in_pChannel->VolumeBalanceSlideStep;
            if( (NewBalance & 0x7F00) != (VolumeBalance & 0x7F00) )
            {
                in_pChannel->VoiceParams.VoiceParamFlags |= 3;
            }
            in_pChannel->VolumeBalance = NewBalance;
        }
    }

    if( in_pChannel->ChannelPanSlideLength != 0 )
    {
        Pan = in_pChannel->ChannelPan;
        in_pChannel->ChannelPanSlideLength--;
        NewPan = Pan + in_pChannel->PanSlideStep;
        if( (NewPan & 0xFF00) != (Pan & 0xFF00) )
        {
            in_pChannel->VoiceParams.VoiceParamFlags |= 3;
        }
        in_pChannel->ChannelPan = NewPan;
    }

    if( in_pChannel->VibratoDelayCurrent != 0 )
    {
        in_pChannel->VibratoDelayCurrent--;
    }

    if( in_pChannel->TremeloDelayCurrent != 0 )
    {
        in_pChannel->TremeloDelayCurrent--;
    }

    if( in_pChannel->VibratoRateSlideLength != 0 )
    {
        in_pChannel->VibratoRateSlideLength--;
        in_pChannel->VibratoRatePhase += in_pChannel->VibratoRateSlideStep;
    }

    if( in_pChannel->TremeloRateSlideLength != 0 )
    {
        in_pChannel->TremeloRateSlideLength--;
        in_pChannel->TremeloRatePhase += in_pChannel->TremeloRateSlideStep;
    }

    if( in_pChannel->AutoPanRateSlideLength != 0 )
    {
        in_pChannel->AutoPanRateSlideLength--;
        in_pChannel->AutoPanRatePhase += in_pChannel->AutoPanRateSlideStep;
    }

    if( in_pChannel->NoiseTimer != 0 )
    {
        in_pChannel->NoiseTimer--;
        if( !(in_pChannel->NoiseTimer & 0xFFFF) )
        {
            if (in_Arg2 == 0)
            {
                g_pActiveMusicConfig->NoiseChannelFlags ^= in_VoiceFlags;
            }
            else
            {
                g_Sound_VoiceSchedulerState.NoiseVoiceFlags ^= in_VoiceFlags;
            }
            g_Sound_GlobalFlags.UpdateFlags |= 0x110;
        }
    }

    if( in_pChannel->FmTimer != 0 )
    {
        in_pChannel->FmTimer--;
        if( !(in_pChannel->FmTimer & 0xFFFF) )
        {
            if (in_Arg2 == 0)
            {
                g_pActiveMusicConfig->FmChannelFlags ^= in_VoiceFlags;
            }
            else
            {
                g_Sound_VoiceSchedulerState.FmVoiceFlags ^= in_VoiceFlags;
            }
            g_Sound_GlobalFlags.UpdateFlags |= 0x100;
        }
    }

    if( in_pChannel->VibratoDepthSlideLength != 0 )
    {
        in_pChannel->VibratoDepthSlideLength--;
        temp_v0_10 = in_pChannel->VibratoDepth + in_pChannel->VibratoDepthSlideStep;
        in_pChannel->VibratoDepth = temp_v0_10;
        temp_a0 = (u32) (temp_v0_10 & 0x7F00) >> 8;
        if( temp_v0_10 & 0x8000 )
        {
            var_lo = temp_a0 * in_pChannel->PitchBase;
        }
        else
        {
            var_lo = temp_a0 * ((u32) (in_pChannel->PitchBase * 0xF) >> 8);
        }
        
        in_pChannel->VibratoBase = (u16) (var_lo >> 7);
        if( ((u16) in_pChannel->VibratoDelayCurrent == 0) && ((u16) in_pChannel->field72_0xb8 != 1) )
        {
            if( (in_pChannel->VibratoWave[0] == 0) && (in_pChannel->VibratoWave[1] == 0) )
            {
                in_pChannel->VibratoWave += in_pChannel->VibratoWave[2];
            }

            temp_a3_4 = (s32) (in_pChannel->VibratoBase * in_pChannel->VibratoWave[0]) >> 0x10;
            if( temp_a3_4 != in_pChannel->VibratoPitch)
            {
                in_pChannel->VibratoPitch = (s16) temp_a3_4;
                in_pChannel->VoiceParams.VoiceParamFlags |= 0x10;
                if( temp_a3_4 >= 0 )
                {
                    in_pChannel->VibratoPitch = temp_a3_4 * 2;
                }
            }
        }
    }

    if( in_pChannel->TremeloDepthSlideLength != 0 )
    {
        in_pChannel->TremeloDepthSlideLength--;
        in_pChannel->TremeloDepth += (u16) in_pChannel->TremeloDepthSlideStep;
        if( ((u16) in_pChannel->TremeloDelayCurrent == 0) && ((u16) in_pChannel->field81_0xca != 1) )
        {
            if( (in_pChannel->TremeloWave[0] == 0) && (in_pChannel->TremeloWave[1] == 0) )
            {
                in_pChannel->TremeloWave += in_pChannel->TremeloWave[2];
            }
            
            temp_a3_5 = (s32) (((s32) ((((s32) ( *((u8*)in_pChannel + 0x5E) * ((u16) in_pChannel->VolumeBalance >> 8)) >> 7) * ((u16) in_pChannel->TremeloDepth >> 8)) << 9) >> 0x10) * *var_a0_2) >> 0xF;
            if( temp_a3_5 != in_pChannel->TremeloVolume )
            {
                in_pChannel->TremeloVolume = (s16) temp_a3_5;
                in_pChannel->VoiceParams.VoiceParamFlags |= 3;
            }
        }
    }

    if( in_pChannel->AutoPanDepthSlideLength != 0 )
    {
        in_pChannel->AutoPanDepthSlideLength--;
        in_pChannel->AutoPanDepth += (u16) in_pChannel->AutoPanDepthSlideStep;
        if( in_pChannel->AutoPanRateCurrent != 1 )
        {
            if( (in_pChannel->AutoPanWave[0] == 0) && (in_pChannel->AutoPanWave[1] == 0) )
            {
                in_pChannel->AutoPanWave += in_pChannel->AutoPanWave[2];
            }

            temp_a3_6 = ((in_pChannel->AutoPanDepth >> 8) * in_pChannel->AutoPanWave[0]) >> 0xF;
            if( temp_a3_6 != in_pChannel->AutoPanVolume )
            {
                in_pChannel->AutoPanVolume = temp_a3_6;
                in_pChannel->VoiceParams.VoiceParamFlags |= 3;
            }
        }
    }

    if( in_pChannel->PitchSlideStepsCurrent != 0 )
    {
        temp_v1_8 = in_pChannel->PitchSlide;
        in_pChannel->PitchSlideStepsCurrent--;
        temp_a3_7 = temp_v1_8 + in_pChannel->PitchSlideStep;
        if( (temp_a3_7 & 0xFFFF0000) != (temp_v1_8 & 0xFFFF0000) )
        {
            in_pChannel->VoiceParams.VoiceParamFlags |= 0x10;
        }
        in_pChannel->PitchSlide = temp_a3_7;
    }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004C5A4 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CA1C );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CEAC );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CF5C );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004CFC4 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", UnassignVoicesFromChannels );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D294 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D374 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D3C4 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D3D4 );

//----------------------------------------------------------------------------------------------------------------------
void ChannelMaskToVoiceMaskFiltered( FSoundChannel* in_Channel, s32* io_VoiceMask, s32 in_ChannelMask, s32 in_VoiceMaskFilter )
{
    u32 bit = 1;

    do {
        if( in_ChannelMask & bit )
        {
            if( in_Channel->VoiceParams.AssignedVoiceNumber < VOICE_COUNT )
            {
                *io_VoiceMask |= 1 << in_Channel->VoiceParams.AssignedVoiceNumber;
            }
        }
        in_ChannelMask &= ~bit;
        in_Channel++;
        bit <<= 1;
    } while( in_ChannelMask != 0 );

    *io_VoiceMask &= in_VoiceMaskFilter;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004D978 );

INCLUDE_ASM( "asm/slps_023.64/nonmatchings/system/sound", func_8004DB24 );
