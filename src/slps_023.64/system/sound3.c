#include "common.h"
#include "hw.h"
#include "system/sound.h"

extern s16 D_80092AFA;

void UpdateCdVolume()
{
    *CD_VOL_L = (s16*)D_80092AFA;
    *CD_VOL_R = (s16*)D_80092AFA;
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80051E84);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80051F04);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80051F7C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052458);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800526FC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052790);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052DA4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80052FB8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800531E0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053370);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800535E4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053D1C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053D90);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053DC4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053DFC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053E68);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053F3C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80053F88);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054028);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054070);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054118);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054144);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005419C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800541D4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800541E0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054208);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005428C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800542BC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054348);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800543D8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800543EC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054400);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054434);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800544C0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800544DC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800544F4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005450C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054580);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800545EC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054680);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800546C8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800546EC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054718);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054760);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054798);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800547A0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054800);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054864);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054964);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800549C4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054A30);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054A90);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054AB4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054B6C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054B90);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054C00);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054C60);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054C84);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054CF8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054D18);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054D84);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054DE4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054E08);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054E68);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054ED0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054F44);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80054FAC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005500C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005506C);

void func_80055078(void) {
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055080);

void func_8005509C(void) {
}

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800550A4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055140);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055184);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800551B8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800551E8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005522C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005526C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800552B0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055328);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005536C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005538C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005539C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800553EC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055480);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800554EC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055568);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800555BC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800555E4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005562C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055664);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005567C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800556B4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800556F0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005572C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055770);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800557A0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800557E4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055814);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005587C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055890);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800558A4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800558B8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800558CC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055944);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055958);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005596C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005598C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800559AC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800559C8);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800559D0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800559F0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055A10);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055AB0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055B40);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055CFC);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055D1C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055D8C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055E0C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80055E60);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005600C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800560D4);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80056144);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800562A0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800562D0);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_80056308);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_8005637C);

INCLUDE_ASM("asm/slps_023.64/nonmatchings/system/sound3", func_800563BC);
