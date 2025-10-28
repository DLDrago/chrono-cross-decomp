#include "common.h"

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", ClearSpuTransferCallback);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", ClearSpuTranferringFlag);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", SetSpuTransferCallback);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", WriteSpu);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", ReadSpu);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/spu", WaitForSpuTransfer);
