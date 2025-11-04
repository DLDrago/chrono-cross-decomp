#include "common.h"
#include "types.h"
#include "psyq/libspu.h"

volatile extern s32 g_bSpuTransferring;

void ClearSpuTransferCallback();

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", ClearSpuTransferCallback);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", ClearSpuTranferringFlag);

//----------------------------------------------------------------------------------------------------------------------
void SetSpuTransferCallback()
{
    g_bSpuTransferring = 1;
    SpuSetTransferCallback( &ClearSpuTransferCallback );
}

//----------------------------------------------------------------------------------------------------------------------
void WriteSpu(s32 addr, s32 size)
{
  g_bSpuTransferring = 1;
  SpuSetTransferCallback( &ClearSpuTransferCallback );
  SpuWrite( addr, size );
}

//----------------------------------------------------------------------------------------------------------------------
void ReadSpu(s32 addr, s32 size)
{
  SetSpuTransferCallback();
  SpuRead( addr, size );
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", WaitForSpuTransfer);
