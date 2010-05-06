#ifdef KBHOOK_EXPORTS
#define KBHOOK_API extern "C" __declspec(dllexport)
#else
#define KBHOOK_API extern "C" __declspec(dllimport)
#endif

KBHOOK_API HHOOK __cdecl Hook(HWND hwnd, UINT keydown_msg, UINT keyup_msg);
KBHOOK_API BOOL __cdecl Unhook(void);
