#include "common.h"
#include "psyq/libspu.h"

volatile extern s32 g_bSpuTransferring;

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