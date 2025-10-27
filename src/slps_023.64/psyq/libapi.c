#include "common.h"

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", InitHeap);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", FlushCache);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", GPU_cw);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", DeliverEvent);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", OpenEvent);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", CloseEvent);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", TestEvent);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", EnableEvent);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", UnDeliverEvent);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", EnterCriticalSection);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", ExitCriticalSection);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", open);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", lseek);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", read);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", write);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", close);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", nextfile);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", Krom2RawAdd);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", ChangeClearPAD);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", SysEnqIntRP);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", SysDeqIntRP);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", ChangeClearRCnt);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", firstfile);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", func_80022B3C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", firstfile2);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", SetInitPadFlag);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", ReadInitPadFlag);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", PAD_init);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", InitPAD);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", StartPAD);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", StopPAD);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", func_80022DF4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", func_80022E6C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", func_80022EA4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", func_80022F0C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", InitPAD2);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", StartPAD2);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", StopPAD2);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", PAD_init2);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", EnablePAD);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", DisablePAD);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", _patch_pad);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/psyq/libapi", _remove_ChgclrPAD);
