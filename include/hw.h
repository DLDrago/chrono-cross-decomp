#ifndef _HW_H
#define _HW_H

#define VOICE_00_ADPCM_SAMPLE_RATE   ( (u16*)0x1F801C04 )
#define VOICE_00_ADPCM_START_ADDR    ( (u16*)0x1F801C06 )
#define VOICE_00_ADPCM_REPEAT_ADDR   ( (u16*)0x1F801C0E )
#define SPU_VOICE_KEY_ON_LO          ( (u16*)0x1F801D88 )
#define SPU_VOICE_KEY_ON_HI          ( (u16*)0x1F801D8A )
#define SPU_VOICE_KEY_OFF_LO         ( (u16*)0x1F801D8C )
#define SPU_VOICE_KEY_OFF_HI         ( (u16*)0x1F801D8E )
#define SPU_VOICE_CHN_REVERB_MODE_LO ( (u16*)0x1F801D98 )
#define SPU_VOICE_CHN_REVERB_MODE_HI ( (u16*)0x1F801D9A )

#endif