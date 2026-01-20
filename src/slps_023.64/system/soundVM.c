#include "common.h"
#include "system/sound.h"

#define SOUND_DEFAULT_PORTAMENTO_STEPS (0x100) // 256 steps
#define SOUND_DEFAULT_DELAY_TIME       (0x101) // 256 + 1

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_A0_FinishChannel);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE00_80053F3C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE01_80053f88);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE02_80054028);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE03_80054070);

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

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_A8_ChannelVolume);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_A9_ChannelVolumeSlides);

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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_AB_ChannelPanSlide);

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

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B3_ResetAdsr);

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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_A4_PitchBendSlide);

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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_D8_ChannelFineTune_Absolute);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_D9_ChannelFineTune_Relative);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B4_Vibrato);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B5_VibratoDepth);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_DD_VibratoDepthSlide);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E4_80054a30);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B6_DisableVibrato);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B8_Tremelo);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B9_TremeloDepth);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_DE_TremeloDepthSlide);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E5_80054c00);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_BA_DisableTremelo);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_BC_ChannelPanLfo);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_BD_ChannelLfoDepth);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_DF_ChannelPanLfoDepthSlide);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E6_80054d84);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_BE_DisableChannelPanLfo);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_C4_EnableNoiseVoices);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_C5_DisableNoiseVoices);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_C6_EnableFmVoices);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_C7_DisableFmVoices);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_C2_EnableReverbVoices);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_C3_DisableReverbVoices);

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CC_EnableLegato( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    in_pChannel->SfxMask = SOUND_SFX_LEGATO;
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_CD_DEBUG_80055078( FSoundChannel* in_pChannel, u32 in_VoiceFlags ) {
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D0_EnableSustainedNote( FSoundChannel* in_pChannel, u32 in_VoiceFlags )
{
    if( in_pChannel->Type != SOUND_CHANNEL_TYPE_MUSIC )
    {
        in_pChannel->SfxMask = SOUND_SFX_FULL_LENGTH;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void SoundVM_D1_DEBUG_8005509c( FSoundChannel* in_pChannel, u32 in_VoiceFlags ) {
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_AC_NoiseClockFrequency);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_AD_AttackRate);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_AE_DecayRate);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_AF_SustainLevel);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B1_SustainRate);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B2_ReleaseRate);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_B7_AttackMode);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_BB_SustainMode);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_BF_ReleaseMode);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE10_8005536c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE11_8005538c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_C8_LoopPoint);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_C9_LoopN);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE08_80055480);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE09_800554ec);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_CA_LoopInf);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_A2_OverwriteNextNoteLength);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_DC_FixNoteLength);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE04_8005562c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE05_80055664);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE15_8005567c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE16_800556b4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_BO_DecayRateAndSustainLevel);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_CE_EnableNoiseAndDelayToggle);

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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_D2_EnableFmAndDelayToggle);

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
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_D4_EnablePlaybackRateSidechain);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_D5_DisablePlaybackRateSidechain);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_D6_EnablePitchVolumeSidechain);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_D7_DisablePitchVolumeSidechain);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE0B_800558cc);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E0_80055944);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE1C_80055958);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE1D_8005596c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE1E_8005598c);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E1_SetVibratoDepth);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_E2_ResetVibratoDepth);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_FE13_800559d0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/soundVM", SoundVM_XX_Unimplemented);
