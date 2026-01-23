#include "common.h"
#include "system/sound.h"

#define SOUND_DEFAULT_PORTAMENTO_STEPS (0x100) // 256 steps
#define SOUND_DEFAULT_DELAY_TIME       (0x101) // 256 + 1

extern u32 D_80072E60[];

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A0_FinishChannel( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicConfig->ActiveChannelMask &= ~in_VoiceFlags;
        if( g_pActiveMusicConfig->ActiveChannelMask == 0 )
        {
            g_Music_LoopCounter = 0;
            g_pActiveMusicConfig->MusicId = 0;
            g_pActiveMusicConfig->StatusFlags = 0;
        }
        g_pActiveMusicConfig->ActiveNoteMask     &= ~in_VoiceFlags;
        g_pActiveMusicConfig->KeyedMask          &= ~in_VoiceFlags;
        g_pActiveMusicConfig->AllocatedVoiceMask &= ~in_VoiceFlags;
        g_pActiveMusicConfig->NoiseChannelFlags  &= ~in_VoiceFlags;
        g_pActiveMusicConfig->ReverbChannelFlags &= ~in_VoiceFlags;
        g_pActiveMusicConfig->FmChannelFlags     &= ~in_VoiceFlags;
    }
    else
    {
        Sound_ClearVoiceFromSchedulerState( in_pChannel, in_VoiceFlags );
    }
    in_pChannel->UpdateFlags = 0;
    g_Sound_GlobalFlags.UpdateFlags |= 0x110;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE00_SetTempo( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    g_pActiveMusicConfig->Tempo =  in_pChannel->ProgramCounter[0] << 0x10;
    g_pActiveMusicConfig->Tempo |= in_pChannel->ProgramCounter[1] << 0x18;
    in_pChannel->ProgramCounter += 2;
    g_pActiveMusicConfig->TempoSlideLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE01_SetTempoSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u8* pc;
    u16 SlideLength;
    s32 Dest;
    s32 Prev;
    s32 Delta;

    SlideLength = *in_pChannel->ProgramCounter++;
    g_pActiveMusicConfig->TempoSlideLength = SlideLength;
    if( SlideLength == 0 )
    {
        g_pActiveMusicConfig->TempoSlideLength = 0x100;
    }
    pc = in_pChannel->ProgramCounter;
    Dest = pc[0] << 0x10;
    Dest |= pc[1] << 0x18;
    in_pChannel->ProgramCounter += 2;
    Prev = g_pActiveMusicConfig->Tempo & 0xFFFF0000;
    Delta = Dest - Prev;
    g_pActiveMusicConfig->TempoSlideStep = Delta / g_pActiveMusicConfig->TempoSlideLength;
    g_pActiveMusicConfig->Tempo = Prev;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE02_80054028);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE03_SetMasterReverbSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 Prev;
    s32 Dest;
    s32 Delta;
    s8* pc;

    g_pActiveMusicConfig->ReverbDepthSlideLength = *in_pChannel->ProgramCounter++;
    if( g_pActiveMusicConfig->ReverbDepthSlideLength == 0 )
    {
        g_pActiveMusicConfig->ReverbDepthSlideLength = 0x100;
    }
    pc = in_pChannel->ProgramCounter;
    Dest = pc[1] << 0x14;
    Dest |= (u8)pc[0] << 0xC;
    in_pChannel->ProgramCounter += 2;
    Prev = g_pActiveMusicConfig->RevDepth & ~0xFFF;
    g_pActiveMusicConfig->RevDepth = Prev;
    Delta = Dest - Prev;
    g_pActiveMusicConfig->ReverbDepthSlideStep = Delta / g_pActiveMusicConfig->ReverbDepthSlideLength;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE06_80054118);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE07_80054144);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE0E_8005419c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE0F_800541d4);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A3_ChannelMasterVolume( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->VolumeBalance = *in_pChannel->ProgramCounter++ << 8;
    in_pChannel->VoiceParams.VoiceParamFlags |= SOUND_UPDATE_SPU_VOICE;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE12_80054208);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A8_ChannelVolume( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->Volume = (s8) *in_pChannel->ProgramCounter++ << 0x17;
    in_pChannel->ChannelVolumeSlideLength = 0;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
    in_pChannel->KeyOnVolumeRampLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A9_ChannelVolumeSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 Prev;
    s32 Dest;
    u16 Length;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->ChannelVolumeSlideLength = Length;
    if( Length == 0 )
    {
        in_pChannel->ChannelVolumeSlideLength = 0x100;
    }
    Dest = ((s8) *in_pChannel->ProgramCounter++ << 0x17);

    Prev = in_pChannel->Volume & 0xFFFF0000;
    in_pChannel->Volume = Prev;

    Delta = Dest - Prev;
    in_pChannel->VolumeSlideStep = Delta / in_pChannel->ChannelVolumeSlideLength;

    in_pChannel->KeyOnVolumeRampLength = 0;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE19_80054348);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE1A_800543d8( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags |= SOUND_UPDATE_UNKNOWN_01;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_FE1B_800543ec( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_UNKNOWN_01;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AA_ChannelPan( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    // Convert signed pan (-64..+63) to unsigned 0..255, center at 0x40 and store as Q8.8 pan value
    in_pChannel->ChannelPan = ((*in_pChannel->ProgramCounter++ + 0x40) & 0xFF) << 8;
    in_pChannel->ChannelPanSlideLength = 0;
    in_pChannel->VoiceParams.VoiceParamFlags |= 3;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AB_ChannelPanSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Prev;
    u16 Length;
    u16 Dest;
    s32 Delta;


    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->ChannelPanSlideLength = Length;
    if( Length == 0 )
    {
        in_pChannel->ChannelPanSlideLength = 0x100;
    }
    
    Dest = *in_pChannel->ProgramCounter++;
    Dest = ((Dest + 0x40) & 0xFF) << 8; // Center it
    Prev = in_pChannel->ChannelPan & 0xFF00;
    Delta = Dest - Prev;
    in_pChannel->PanSlideStep = Delta / in_pChannel->ChannelPanSlideLength;
    in_pChannel->ChannelPan = Prev;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A5_SetOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->Octave = (u16) *in_pChannel->ProgramCounter++;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A6_IncreaseOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->Octave = (in_pChannel->Octave + 1) & 0xF;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A7_DecreaseOctave( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->Octave = (in_pChannel->Octave - 1) & 0xF;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A1_LoadInstrument( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    FSoundInstrumentInfo* InstrumentInfo;
    u16 InstrumentIndex;

    InstrumentIndex = *in_pChannel->ProgramCounter++;
    InstrumentInfo = &g_InstrumentInfo[ InstrumentIndex ];
    Sound_CopyInstrumentInfoToChannel( in_pChannel, InstrumentInfo, InstrumentInfo->StartAddr );
    in_pChannel->InstrumentIndex = InstrumentIndex;
    in_pChannel->VoiceParams.VolumeScale = 0;
    in_pChannel->UpdateFlags &= ~( 
        SOUND_UPDATE_DRUM_MODE  | 
        SOUND_UPDATE_UNKNOWN_12 | 
        SOUND_UPDATE_UNKNOWN_24 | 
        SOUND_UPDATE_UNKNOWN_27 | 
        SOUND_UPDATE_UNKNOWN_28 
    );
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE0A_80054580);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE14_800545ec);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B3_ResetAdsr( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    FSoundInstrumentInfo* InstrumentInfo = &g_InstrumentInfo[in_pChannel->InstrumentIndex];
    in_pChannel->VoiceParams.AdsrLower = InstrumentInfo->AdsrLower;
    in_pChannel->VoiceParams.AdsrUpper = InstrumentInfo->AdsrUpper;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_FULL;
    in_pChannel->UpdateFlags &= ~(SOUND_UPDATE_UNKNOWN_24 | SOUND_UPDATE_UNKNOWN_27 | SOUND_UPDATE_UNKNOWN_28);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C0_ChannelTranspose_Absolute( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s8 Transpose = *in_pChannel->ProgramCounter++;
    in_pChannel->Transpose = Transpose;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C1_ChannelTranspose_Relative( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s8 Transpose = *in_pChannel->ProgramCounter++;
    in_pChannel->Transpose += Transpose;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A4_PitchBendSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;

    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->PitchBendSlideLength = Length;
    if( Length == 0 )
    {
        in_pChannel->PitchBendSlideLength = 0x100;
    }
    
    in_pChannel->PitchBendSlideTranspose = (s8)*in_pChannel->ProgramCounter++;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DA_EnablePortamento(FSoundChannel* in_pChannel, u32 in_VoiceFlags) {
    u16 Steps;

    Steps = *in_pChannel->ProgramCounter++;
    in_pChannel->PortamentoSteps = Steps;
    if( Steps == 0 )
    {
        in_pChannel->PortamentoSteps = SOUND_DEFAULT_PORTAMENTO_STEPS;
    }
    in_pChannel->TransposeStored = 0;
    in_pChannel->KeyStored = 0;
    in_pChannel->SfxMask = 1;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DB_DisablePortamento( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->PortamentoSteps = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D8_ChannelFineTune_Absolute( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 FinePitchDelta;
    u32 ScaledFineTune;

    in_pChannel->FineTune = (s8)*in_pChannel->ProgramCounter++;
    ScaledFineTune = in_pChannel->PitchBase * (u8)in_pChannel->FineTune;
    
    if( in_pChannel->FineTune < 0 )
    {
        FinePitchDelta = (ScaledFineTune >> 8) - in_pChannel->PitchBase;
    }
    else
    {
        FinePitchDelta = ScaledFineTune >> 7;
    }

    in_pChannel->FinePitchDelta = FinePitchDelta;

    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D9_ChannelFineTune_Relative( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u32 ScaledFineTune;
    u32 FinePitchDelta;

    in_pChannel->FineTune = in_pChannel->FineTune + (s8) *in_pChannel->ProgramCounter++;
    ScaledFineTune = in_pChannel->PitchBase * (u8)in_pChannel->FineTune;
    if( in_pChannel->FineTune < 0 )
    {
        FinePitchDelta = (ScaledFineTune >> 8) - in_pChannel->PitchBase;
    }
    else
    {
        FinePitchDelta = ScaledFineTune >> 7;
    }
    in_pChannel->FinePitchDelta = FinePitchDelta;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B4_Vibrato);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B5_VibratoDepth(FSoundChannel* in_pChannel, u32 in_VoiceFlags) {
    s32 PitchBase;      // Base pitch around which the vibrato will oscillate
    u32 DepthAmount;    // This is just the magnitude part of the vibrato depth parameter, 0-127 depth scalar
    u32 VibratoProduct; // Intermediate scaled value

    PitchBase = in_pChannel->PitchBase;

    // Read vibrato depth parameter from the sequence.
    // Stored in the high byte so bit 15 can act as a mode flag.
    in_pChannel->VibratoDepth = *in_pChannel->ProgramCounter++ << 8;

    // Extract the depth magnitude (bits 8–14)
    DepthAmount = (in_pChannel->VibratoDepth & 0x7F00) >> 8;
    if( !(in_pChannel->VibratoDepth & VIBRATO_FLAG_ABSOLUTE) )
    {
        // Relative mode:
        // Vibrato depth is scaled down relative to pitch so higher notes
        // don’t produce excessively wide vibrato.
        // (PitchBase * 15) >> 8 ≈ PitchBase * 0.0586
        VibratoProduct = DepthAmount * ((PitchBase * 15) >> 8);
    }
    else
    {
        // Absolute mode:
        // Vibrato depth is directly proportional to pitch.
        VibratoProduct = DepthAmount * PitchBase;
    }

    // Final vibrato amplitude in pitch units.
    // The >> 7 normalizes the multiplication into a usable range.
    in_pChannel->VibratoBase = VibratoProduct >> 7;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DD_VibratoDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;
    u16 Prev;
    u16 Dest;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    if( Length == 0 )
    {
        Length = 0x100;
    }
    Dest = *in_pChannel->ProgramCounter++;
    Dest = Dest << 8;
    Prev = in_pChannel->VibratoDepth;
    Delta = Dest - Prev;
    in_pChannel->VibratoDepthSlideStep = Delta / Length;
    in_pChannel->VibratoDepthSlideLength = Length;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E4_80054a30);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B6_DisableVibrato( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->VibratoPitch = 0;
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_VIBRATO;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_SAMPLE_RATE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B8_Tremelo( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Delay;
    u32 Rate;
    u16 Type;

    in_pChannel->UpdateFlags |= 2;
    Delay = *in_pChannel->ProgramCounter++;

    if (in_pChannel->Type != 0)
    {
        in_pChannel->TremeloDelay = 0;
        if( Delay != 0 )
        {
            in_pChannel->TremeloDepth = (Delay & 0x7F) << 8;
        }
    }
    else
    {
        in_pChannel->TremeloDelay = (s16) Delay;
    }

    Rate = *in_pChannel->ProgramCounter++ << 0xA;
    in_pChannel->TremeloRateAccumulator = Rate;

    if( Rate == 0 )
    {
        in_pChannel->TremeloRateAccumulator = 0x40000;
    }

    Type = *in_pChannel->ProgramCounter++;
    in_pChannel->TremeloType = Type;

    in_pChannel->TremeloRate = 0;
    in_pChannel->field81_0xca = 1;
    in_pChannel->TremeloDelayCurrent = (u16)in_pChannel->TremeloDelay;
    in_pChannel->TremeloWave = D_80072E60[Type];
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B9_TremeloDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->TremeloDepth = (*in_pChannel->ProgramCounter++ & 0x7F) << 8;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DE_TremeloDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 Length;
    s32 Prev;
    u32 Dest;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    if( Length == 0 )
    {
        Length = 0x100;
    }
    Dest = (*in_pChannel->ProgramCounter++ & 0x7f) << 8;
    Prev = in_pChannel->TremeloDepth;
    Delta = Dest - Prev;
    in_pChannel->TremeloDepthSlideStep = Delta / Length;
    in_pChannel->TremeloDepthSlideLength = Length;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E5_80054c00);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BA_DisableTremelo( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->TremeloVolume = 0;
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_TREMOLO;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
}

extern u32 D_80072E60[];

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BC_ChannelPanLfo( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 Rate;

    in_pChannel->UpdateFlags |= 4;
    Rate = *in_pChannel->ProgramCounter++ << 0xA;
    in_pChannel->PanLfoRateAccumulator = Rate;
    if( Rate == 0 )
    {
        in_pChannel->PanLfoRateAccumulator = 0x40000;
    }
    in_pChannel->PanLfoType = *in_pChannel->ProgramCounter++;
    in_pChannel->PanLfoWave = D_80072E60[in_pChannel->PanLfoType];
    in_pChannel->PanLfoRateCurrent = 1;
    in_pChannel->PanLfoRate = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BD_ChannelLfoDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->PanLfoDepth = *in_pChannel->ProgramCounter++ << 7;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_DF_ChannelPanLfoDepthSlide( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;
    u16 Prev;
    u16 Dest;
    s32 Delta;

    Length = *in_pChannel->ProgramCounter++;
    if (Length == 0) {
        Length = 0x100;
    }
    Dest = *in_pChannel->ProgramCounter++;
    Dest = Dest << 7;
    Prev = in_pChannel->PanLfoDepth;

    Delta = Dest - Prev;
    in_pChannel->PanLfoDepthSlideStep = Delta / Length;
    in_pChannel->PanLfoDepthSlideLength = Length;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E6_80054d84);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BE_DisableChannelPanLfo( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->PanLfoVolume = 0;
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_PAN_LFO;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_VOLUME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C4_EnableNoiseVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicConfig->NoiseChannelFlags |= in_VoiceFlags;
    }
    else
    {
        g_Sound_VoiceSchedulerState.NoiseVoiceFlags |= in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= 0x110;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C5_DisableNoiseVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicConfig->NoiseChannelFlags &= ~in_VoiceFlags;
    }
    else
    {
        g_Sound_VoiceSchedulerState.NoiseVoiceFlags &= ~in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= 0x110;
    in_pChannel->NoiseTimer = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C6_EnableFmVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicConfig->FmChannelFlags |= in_VoiceFlags;
    }
    else if( in_pChannel->UpdateFlags & 0x10000 )
    {
        g_Sound_VoiceSchedulerState.FmVoiceFlags |= in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= 0x100;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C7_DisableFmVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicConfig->FmChannelFlags &= ~in_VoiceFlags;
    }
    else
    {
        g_Sound_VoiceSchedulerState.FmVoiceFlags &= ~in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= 0x100;
    in_pChannel->FmTimer = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C2_EnableReverbVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicConfig->ReverbChannelFlags |= in_VoiceFlags;
    }
    else
    {
        g_Sound_VoiceSchedulerState.ReverbVoiceFlags |= in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= 0x100;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C3_DisableReverbVoices( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        g_pActiveMusicConfig->ReverbChannelFlags &= ~in_VoiceFlags;
    }
    else
    {
        g_Sound_VoiceSchedulerState.ReverbVoiceFlags &= ~in_VoiceFlags;
    }
    g_Sound_GlobalFlags.UpdateFlags |= 0x100;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CC_EnableLegato( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->SfxMask = SOUND_SFX_LEGATO;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CD_DEBUG_80055078( FSoundChannel* in_pChannel, u32 in_VoiceFlags ) {}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D0_EnableSustainedNote( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type != SOUND_CHANNEL_TYPE_MUSIC )
    {
        in_pChannel->SfxMask = SOUND_SFX_FULL_LENGTH;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D1_DEBUG_8005509c( FSoundChannel* in_pChannel, u32 in_VoiceFlags ) {}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AC_NoiseClockFrequency( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Frequency;

    Frequency = *in_pChannel->ProgramCounter++;
    if( in_pChannel->Type == SOUND_CHANNEL_TYPE_MUSIC )
    {
        if( Frequency & 0xC0 )
        {
            g_pActiveMusicConfig->NoiseClock = (g_pActiveMusicConfig->NoiseClock + (Frequency & 0x3F)) & 0x3F;
        }
        else
        {
            g_pActiveMusicConfig->NoiseClock = Frequency;
        }
    }
    else
    {
        if( Frequency & 0xC0 )
        {
            g_Sound_VoiceSchedulerState.NoiseClock = (g_Sound_VoiceSchedulerState.NoiseClock + (Frequency & 0x3F)) & 0x3F;
        }
        else
        {
            g_Sound_VoiceSchedulerState.NoiseClock = Frequency;
        }
    }
    g_Sound_GlobalFlags.UpdateFlags |= SOUND_UPDATE_SIDE_CHAIN_PITCH;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AD_AttackRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 AttackRate = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrLower &= ~SOUND_ADSR_ATTACK_RATE_MASK;
    in_pChannel->VoiceParams.AdsrLower |= AttackRate << SOUND_ADSR_ATTACK_RATE_SHIFT;
    in_pChannel->VoiceParams.VoiceParamFlags |= (VOICE_PARAM_ADSR_AMODE | VOICE_PARAM_ADSR_AR);
    in_pChannel->UpdateFlags |= SOUND_UPDATE_UNKNOWN_24;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AE_DecayRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 DecayRate = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrLower &= ~SOUND_ADSR_DECAY_RATE_MASK;
    in_pChannel->VoiceParams.AdsrLower |= DecayRate << SOUND_ADSR_DECAY_RATE_SHIFT;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_DR;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_AF_SustainLevel( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 SustainLevel = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrLower &= ~SOUND_ADSR_SUS_LEVEL_MASK;
    in_pChannel->VoiceParams.AdsrLower |= SustainLevel;
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_SL;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B1_SustainRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 SustainRate = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrUpper &= ~SOUND_ADSR_SUS_RATE_MASK;
    in_pChannel->VoiceParams.AdsrUpper |= SustainRate << SOUND_ADSR_SUS_RATE_SHIFT;
    in_pChannel->VoiceParams.VoiceParamFlags |= (VOICE_PARAM_ADSR_SR | VOICE_PARAM_ADSR_SMODE);
    in_pChannel->UpdateFlags |= 0x08000000;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B2_ReleaseRate( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 ReleaseRate = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrUpper &= ~SOUND_ADSR_RELEASE_RATE_MASK;
    in_pChannel->VoiceParams.AdsrUpper |= ReleaseRate << SOUND_ADSR_RELEASE_RATE_SHIFT;
    in_pChannel->VoiceParams.VoiceParamFlags |= (VOICE_PARAM_ADSR_RR | VOICE_PARAM_ADSR_RMODE);
    in_pChannel->UpdateFlags |= 0x10000000;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B7_AttackMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Mode = *in_pChannel->ProgramCounter++;

    in_pChannel->VoiceParams.AdsrLower &= ~SOUND_ADSR_ATTACK_MODE_MASK;

    if( Mode == SOUND_AMODE_5 )
    {
        in_pChannel->VoiceParams.AdsrLower |= SOUND_ADSR_ATTACK_MODE_MASK;
    }
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_AMODE;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BB_SustainMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 mode;
    u16 adsrUpper;

    adsrUpper = in_pChannel->VoiceParams.AdsrUpper & 0x3FFF;
    mode = *in_pChannel->ProgramCounter++;
    in_pChannel->VoiceParams.AdsrUpper = adsrUpper;

    if (mode == 5)
    {
        in_pChannel->VoiceParams.AdsrUpper = adsrUpper | 0x8000;
    }
    else if (mode < 6)
    {
        if (mode == 3)
        {
            in_pChannel->VoiceParams.AdsrUpper = adsrUpper | 0x4000;
        }
    }
    else
    {
        if (mode == 7)
        {
            in_pChannel->VoiceParams.AdsrUpper = adsrUpper | 0xC000;
        }
    }

    in_pChannel->VoiceParams.VoiceParamFlags |= 0x200;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_BF_ReleaseMode( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Value = *in_pChannel->ProgramCounter++;

    in_pChannel->VoiceParams.AdsrUpper &= ~SOUND_ADSR_RELEASE_MODE_MASK;
    if( Value == SOUND_RMODE_7 )
    {
        in_pChannel->VoiceParams.AdsrUpper |= SOUND_ADSR_RELEASE_MODE_MASK;
    }
    in_pChannel->VoiceParams.VoiceParamFlags |= VOICE_PARAM_ADSR_RMODE;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE10_8005536c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE11_8005538c);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_C8_LoopPoint( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->LoopStackTop = (in_pChannel->LoopStackTop + 1) & SOUND_LOOP_STACK_MAX_INDEX;
    in_pChannel->LoopStartPc[in_pChannel->LoopStackTop] = in_pChannel->ProgramCounter;
    in_pChannel->LoopIterationCount[in_pChannel->LoopStackTop] = 0;
    in_pChannel->LoopStepCounterSnapshot[in_pChannel->LoopStackTop] = in_pChannel->OpcodeStepCounter;
}

//----------------------------------------------------------------------------------------------------------------------
// Is there a bug in this? It appears as though it loops one fewer times than specified...
void SoundVM_C9_LoopN( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 NewCount;
    u16 TopIndex;
    u16 DesiredLoopCount;

    DesiredLoopCount = *in_pChannel->ProgramCounter++;
    if( DesiredLoopCount == 0 )
    {
        DesiredLoopCount = 0x100;
    }

    TopIndex = in_pChannel->LoopStackTop;
    NewCount = in_pChannel->LoopIterationCount[TopIndex] + 1;
    in_pChannel->LoopIterationCount[TopIndex] = NewCount;

    if( NewCount != DesiredLoopCount )
    {
        in_pChannel->ProgramCounter = in_pChannel->LoopStartPc[in_pChannel->LoopStackTop];
        in_pChannel->OpcodeStepCounter = in_pChannel->LoopStepCounterSnapshot[in_pChannel->LoopStackTop];
        return;
    }

    in_pChannel->LoopStackTop = (in_pChannel->LoopStackTop - 1) & SOUND_LOOP_STACK_MAX_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE08_80055480);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE09_800554ec);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CA_LoopInf( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->LoopIterationCount[in_pChannel->LoopStackTop] += 1;
    in_pChannel->ProgramCounter = in_pChannel->LoopStartPc[in_pChannel->LoopStackTop];
    in_pChannel->OpcodeStepCounter = in_pChannel->LoopStepCounterSnapshot[in_pChannel->LoopStackTop];
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_A2_OverwriteNextNoteLength( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Length;

    Length = *in_pChannel->ProgramCounter++;
    in_pChannel->LengthFixed = 0;
    in_pChannel->Length2 = Length;
    in_pChannel->Length1 = Length;
    in_pChannel->LengthStored = Length;
}

//----------------------------------------------------------------------------------------------------------------------
// Set the duration for all the upcoming notes (same as A2 except it doesn't apply only to the next note)
void SoundVM_DC_FixNoteLength( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    s32 NoteLength;

    NoteLength = *(s8*)in_pChannel->ProgramCounter++;
    if( NoteLength != 0 )
    {
        NoteLength += in_pChannel->LengthStored;
        if( NoteLength <= 0 )
        {
            NoteLength = 1;
        }
        else if( NoteLength >= 256 )
        {
            NoteLength = 255;
        }
    }
    in_pChannel->LengthFixed = NoteLength;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE04_8005562c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE05_80055664);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE15_8005567c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE16_800556b4);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_B0_DecayRateAndSustainLevel( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    SoundVM_AE_DecayRate( in_pChannel, in_VoiceFlags );
    SoundVM_AF_SustainLevel( in_pChannel, in_VoiceFlags );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CE_EnableNoiseAndDelayToggle( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Timer;
    u16 Value;

    Value = *in_pChannel->ProgramCounter++;
    if( Value != 0 )
    {
        Timer = Value + 1;
    }
    else
    {
        Timer = SOUND_DEFAULT_DELAY_TIME;
    }

    in_pChannel->NoiseTimer = Timer;

    SoundVM_C4_EnableNoiseVoices( in_pChannel, in_VoiceFlags );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CF_ToggleNoiseOnDelay( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Delay = *in_pChannel->ProgramCounter++;
    if( Delay != 0 )
    {
        in_pChannel->NoiseTimer = Delay + 1;
        return;
    }
    in_pChannel->NoiseTimer = SOUND_DEFAULT_DELAY_TIME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D2_EnableFmAndDelayToggle( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Timer;
    u16 Value;

    Value = *in_pChannel->ProgramCounter++;
    if( Value != 0 )
    {
        Timer = Value + 1;
    }
    else
    {
        Timer = SOUND_DEFAULT_DELAY_TIME;
    }

    in_pChannel->FmTimer = Timer;
    SoundVM_C6_EnableFmVoices(in_pChannel, in_VoiceFlags);
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D3_ToggleFmDelay( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    u16 Delay = *in_pChannel->ProgramCounter++;
    if( Delay != 0 )
    {
        in_pChannel->FmTimer = Delay + 1;
        return;
    }
    in_pChannel->FmTimer = SOUND_DEFAULT_DELAY_TIME;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CB_DisableVoiceModes( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags &= ~( SOUND_UPDATE_SIDE_CHAIN_PITCH | SOUND_UPDATE_SIDE_CHAIN_VOL | SOUND_UPDATE_PAN_LFO |
        SOUND_UPDATE_TREMOLO | SOUND_UPDATE_VIBRATO );
    SoundVM_C5_DisableNoiseVoices(in_pChannel, in_VoiceFlags);
    SoundVM_C7_DisableFmVoices(in_pChannel, in_VoiceFlags);
    SoundVM_C3_DisableReverbVoices(in_pChannel, in_VoiceFlags);
    in_pChannel->SfxMask &= ~( SOUND_SFX_LEGATO | SOUND_SFX_FULL_LENGTH );
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D4_EnablePlaybackRateSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags |= SOUND_UPDATE_SIDE_CHAIN_PITCH;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D5_DisablePlaybackRateSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_SIDE_CHAIN_PITCH;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D6_EnablePitchVolumeSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags |= SOUND_UPDATE_SIDE_CHAIN_VOL;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D7_DisablePitchVolumeSidechain( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->UpdateFlags &= ~SOUND_UPDATE_SIDE_CHAIN_VOL;
}


//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE0B_800558cc);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E0_80055944);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE1C_80055958);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE1D_8005596c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE1E_8005598c);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_E1_SetRandomPitchDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->RandomPitchDepth = *in_pChannel->ProgramCounter++;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_E2_ResetRandomPitchDepth( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->RandomPitchDepth = 0;
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE13_800559d0);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_XX_Unimplemented( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    SoundVM_A0_FinishChannel( in_pChannel, in_VoiceFlags );
}
