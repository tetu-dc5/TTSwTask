#pragma once

#include "resource.h"
#include "ImageList.h"
#include "WndList.h"
#include "launcher.h"
#include "EditIniFile.h"

#define		TMP_BUF_SIZE	(1024)

extern HINSTANCE		g_hInst;
extern HWND				g_hWnd;
extern CImageList*		g_ImageList;
extern CWndList*		g_WndList;
extern CLauncher*		g_Launcher;
extern CEditIniFile*	g_EditIni;
extern LPCTSTR			g_IniPath;

LPTSTR CopyString(LPCTSTR src);
