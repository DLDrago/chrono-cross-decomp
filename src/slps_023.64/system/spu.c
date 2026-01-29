#include "common.h"
#include "psyq/libspu.h"
#include "system/sound.h"

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", Sound_CopyAndRelocateInstruments);

//----------------------------------------------------------------------------------------------------------------------
bool Sound_IsNotAkaoFile( FAkaoFileBlob* in_Blob )
{
    return in_Blob->Magic - AKAO_FILE_MAGIC;
}

//----------------------------------------------------------------------------------------------------------------------
void ClearSpuTransferCallback()
{
  SpuSetTransferCallback( NULL );
  g_bSpuTransferring = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SetSpuTransferCallback()
{
    g_bSpuTransferring = 1;
    SpuSetTransferCallback( &ClearSpuTransferCallback );
}

//----------------------------------------------------------------------------------------------------------------------
void WriteSpu(s32 in_Addr, s32 in_Size)
{
    g_bSpuTransferring = 1;
    SpuSetTransferCallback( &ClearSpuTransferCallback );
    SpuWrite( (u8*)in_Addr, in_Size );
}

//----------------------------------------------------------------------------------------------------------------------
void ReadSpu(s32 in_Addr, s32 in_Size)
{
    SetSpuTransferCallback();
    SpuRead( (u8*)in_Addr, in_Size );
}

//----------------------------------------------------------------------------------------------------------------------
void WaitForSpuTransfer()
{
    while (g_bSpuTransferring == 1)
    {
    }
}

//----------------------------------------------------------------------------------------------------------------------
INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", func_8004B284);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", func_8004B2D4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", Sound_Setup);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", Sound_Start);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", Sound_Stop);
