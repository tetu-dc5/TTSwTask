#pragma once

#include "resource.h"
#include "ImageList.h"
#include "WndList.h"
#include "launcher.h"
#include "EditIniFile.h"

extern HINSTANCE		g_hInst;
extern HWND				g_hWnd;
extern CImageList*		g_ImageList;
extern CWndList*		g_WndList;
extern CLauncher*		g_Launcher;
extern CEditIniFile*	g_EditIni;
extern LPCTSTR			g_IniPath;
