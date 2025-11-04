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
void WriteSpu(s32 in_Addr, s32 in_Size)
{
    g_bSpuTransferring = 1;
    SpuSetTransferCallback( &ClearSpuTransferCallback );
    SpuWrite( in_Addr, in_Size );
}

//----------------------------------------------------------------------------------------------------------------------
void ReadSpu(s32 in_Addr, s32 in_Size)
{
    SetSpuTransferCallback();
    SpuRead( in_Addr, in_Size );
}

//----------------------------------------------------------------------------------------------------------------------
void WaitForSpuTransfer()
{
    while (g_bSpuTransferring == 1)
    {
    }
}