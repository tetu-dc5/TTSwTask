// KbHook.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "KbHook.h"

HINSTANCE	dll_hinst       = NULL;
#pragma data_seg("SHARE_DATA")
HWND		dll_hwnd        = NULL;
HHOOK		dll_hhook       = NULL;
UINT		dll_keydown_msg = WM_USER+10;
UINT		dll_keyup_msg   = WM_USER+11;
#pragma data_seg()

static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode<0)
	{
		return CallNextHookEx(dll_hhook, nCode, wParam, lParam);
	}
	LPKBDLLHOOKSTRUCT	lpKbInfo = (LPKBDLLHOOKSTRUCT)lParam;
	BOOL				fHandled = FALSE;

	switch(wParam){
	case	WM_KEYDOWN:
	case	WM_SYSKEYDOWN:
		if(lpKbInfo->flags & LLKHF_ALTDOWN){
			switch(lpKbInfo->vkCode){
			case	VK_TAB:
			case	VK_ESCAPE:
			case	VK_KANJI:
				fHandled = TRUE;
			case	VK_LSHIFT:
			case	VK_RSHIFT:
			case	VK_UP:
			case	VK_DOWN:
				PostMessage(dll_hwnd, dll_keydown_msg, (WPARAM)lpKbInfo->vkCode, (LPARAM)lpKbInfo->flags);
				break;
			}
		}
		break;
	case	WM_KEYUP:
	case	WM_SYSKEYUP:
		switch(lpKbInfo->vkCode){
		case	VK_LSHIFT:
		case	VK_RSHIFT:
		case	VK_LMENU:
		case	VK_RMENU:
			PostMessage(dll_hwnd, dll_keyup_msg, (WPARAM)lpKbInfo->vkCode, (LPARAM)lpKbInfo->flags);
			break;
		}
		break;
	}
	return (fHandled) ? TRUE : CallNextHookEx(dll_hhook, nCode, wParam, lParam);
}


KBHOOK_API HHOOK __cdecl Hook(HWND hwnd, UINT keydown_msg, UINT keyup_msg)
{
	dll_hwnd        = hwnd;
	dll_keydown_msg = keydown_msg;
	dll_keyup_msg   = keyup_msg;
	dll_hhook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, dll_hinst, 0);
	return dll_hhook;
}

KBHOOK_API BOOL __cdecl Unhook(void)
{
	dll_hwnd  = NULL;
	return UnhookWindowsHookEx(dll_hhook);
}
