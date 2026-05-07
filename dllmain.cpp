#include "mewjector.h"
static MewjectorAPI mj;


typedef int* (__fastcall* fn_clip_concatenate)(int*, UINT32);
static fn_clip_concatenate g_orig_fn_clip_concatenate;

typedef UINT64(__fastcall* fn_pause_menu_house)(void*);
static fn_pause_menu_house g_orig_fn_pause_menu_house;

const int RVA_CLIP_CONCATENATE = 0x50f60;
const int RVA_PAUSE_MENU_HOUSE = 0x289bc0;

static bool doing_pause_menu_house = false;

static int* __fastcall HookClipConcatenate(int* arg1, UINT32 arg2)
{
    //if (doing_pause_menu_house) {
    //    mj.Log("CollarCrashFix", "clipconcatenate: %d", arg2);
    //}
    if (doing_pause_menu_house && (arg2 > 13)) {
        return g_orig_fn_clip_concatenate(arg1, 13);
    }
    return g_orig_fn_clip_concatenate(arg1, arg2);
}
static UINT64 __fastcall HookPauseMenuHouse(void* arg_1)
{
    doing_pause_menu_house = true;
    auto result = g_orig_fn_pause_menu_house(arg_1);
    doing_pause_menu_house = false;
    return result;
}

void Init() {
    if (!MJ_Resolve(&mj)) return;  // not present or too old


    void* tramp = NULL;
    mj.InstallHook(RVA_CLIP_CONCATENATE, 0, HookClipConcatenate, &tramp, 10, "CollarCrashFix");
    g_orig_fn_clip_concatenate = (fn_clip_concatenate)tramp;

    mj.InstallHook(RVA_PAUSE_MENU_HOUSE, 0, HookPauseMenuHouse, &tramp, 10, "CollarCrashFix");
    g_orig_fn_pause_menu_house = (fn_pause_menu_house)tramp;

}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Init();
    }
    return TRUE;
}

