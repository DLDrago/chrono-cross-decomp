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
