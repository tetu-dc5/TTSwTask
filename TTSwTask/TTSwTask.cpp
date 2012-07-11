// TTSwTask.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "TTSwTask.h"
#include <shellapi.h>
#include "..\KbHook\KbHook.h"
#include "util.h"

#include <Dwmapi.h>

#define	USE_GRADIENT
#define USE_AERO

#ifdef USE_GRADIENT
#pragma	comment(lib, "msimg32.lib")
#endif

#define		ENABLE_HOOK		1

#define		WM_TASKICON		(WM_USER+1)
#define		WM_SP_KEYDOWN	(WM_USER+2)
#define		WM_SP_KEYUP		(WM_USER+3)

HINSTANCE		g_hInst       = NULL;
HWND			g_hWnd        = NULL;
CImageList*		g_ImageList   = NULL;
CWndList*		g_WndList     = NULL;
CLauncher*		g_Launcher    = NULL;
CEditIniFile*	g_EditIni     = NULL;
LPCTSTR			g_IniPath     = NULL;
HGDIOBJ			g_NormalFont  = NULL;
HGDIOBJ			g_LaunchFont  = NULL;
LPCTSTR			g_AppName     = _T("TTSwTask");
HMENU			g_AppMenu     = NULL;
NOTIFYICONDATA	g_Notify;
BOOL			g_Shift       = FALSE;
#ifdef USE_AERO
BOOL			g_Aero        = TRUE;
#else
BOOL			g_Aero        = FALSE;
#endif

static const int	g_ItemHeight  = 20;
static const int	g_ItemIconX   = 2;
static const int	g_ItemIconY   = 2;
static const int	g_ItemTextX   = 22;
static const int	g_ItemTextY   = 3;
static const int	g_MinWidth    = 400;
static const int	g_MaxWidth    = 800;
static const int	g_TextMargin  = 2;

static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
static ATOM	MyRegisterClass(HINSTANCE hInstance);
static BOOL	InitInstance(HINSTANCE, int);
static void CreateNotifyIcon(HWND hwnd);
static BOOL	OnCreate(HWND hwnd);
static void	OnPaint(HWND hwnd, HDC hdc);
static void	GetIniPath(void);
static void	OnListUpdate(void);
static void	GetItemRect(int idx, LPRECT lpRect);
#ifdef USE_GRADIENT
static void	FillGradient(HDC hdc, LPRECT lpRect, COLORREF col1, COLORREF col2);
#endif
static void	OnDestroy(HWND hwnd);
static void	InvalidateItem(void);
static int	GetItemWidth(LPCTSTR title);
static void	MenuOpen(void);
static void	OnSpKeyDown(WPARAM wParam, LPARAM lParam);
static void	OnSpKeyUp(WPARAM wParam, LPARAM lParam);
static int	PointToItem(POINTS pt);
static void	HideWindow(void);
static void SetAero(HWND hWnd);


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
	MSG msg;

	GetIniPath();
	g_WndList  = new CWndList();
	g_Launcher = new CLauncher();
	g_EditIni = new CEditIniFile();
	g_Launcher->ReadFromFile(g_IniPath);

	BOOL enable = FALSE;
	DwmIsCompositionEnabled( &enable );
	if(!enable) g_Aero = FALSE;

	MyRegisterClass(hInstance);
	if (!InitInstance (hInstance, nCmdShow)) return FALSE;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	delete [] g_IniPath;
	delete g_EditIni;
	delete g_WndList;
	delete g_Launcher;
	return (int) msg.wParam;
}

static HICON GetResourceIconHandle(bool _small)
{
	int cx_index = SM_CXICON;
	int cy_index = SM_CYICON;
	if(_small){
		cx_index = SM_CXSMICON;
		cy_index = SM_CYSMICON;
	}
	return (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TTSWTASK), IMAGE_ICON, GetSystemMetrics(cx_index), GetSystemMetrics(cy_index), 0);
}

static void SetAero(HWND hWnd)
{
	if(g_Aero)
	{
		MARGINS margin = { -1 };
		DwmExtendFrameIntoClientArea(hWnd, &margin);
	}
}

static ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	
	wcex.cbSize         = sizeof(WNDCLASSEX);
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= GetResourceIconHandle(false);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	if(g_Aero)
		wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	else
		wcex.hbrBackground	= (HBRUSH)(COLOR_3DFACE+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_AppName;
	wcex.hIconSm		= GetResourceIconHandle(true);
	return RegisterClassEx(&wcex);
}

static void GetIniPath(void)
{
	TCHAR	me[TMP_BUF_SIZE];
	GetModuleFileName(NULL, me, TMP_BUF_SIZE);
	TCHAR* pExt = _tcsrchr(me, _T('.'));
	*pExt = _T('\0');
	_tcscat_s(me, TMP_BUF_SIZE, _T(".ini"));
	g_IniPath = CopyString(me);
}

static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	g_hInst = hInstance;
	CWndInfo::SetDefaultIcon(hInstance, MAKEINTRESOURCE(IDI_TTSWTASK));

	hWnd = CreateWindowEx(
		(WS_EX_DLGMODALFRAME | WS_EX_TOOLWINDOW),
		g_AppName,
		_T(""), 
		(g_Aero ? WS_OVERLAPPED : WS_POPUPWINDOW),
		100, 100, 
		400, 300,
		NULL, 
		NULL, 
		hInstance, 
		NULL);
   if(!hWnd) return FALSE;
   g_hWnd = hWnd;
   
   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);
   
   return TRUE;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static	UINT	uiTaskbarRestart = 0xFFFFFFFF;

	if(message == uiTaskbarRestart)
	{
		//	通知アイコンの再登録
		CreateNotifyIcon(hWnd);
	}
	else{
		switch (message)
		{
		case WM_TASKICON:
			if(lParam == WM_RBUTTONUP)
			{
				MenuOpen();
			}
			break;
		case WM_COMMAND:
			if(LOWORD(wParam) == ID_APP_QUIT)
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			else if(LOWORD(wParam) == ID_VIEW_LIST)
			{
				OnListUpdate();
			}
			else if(LOWORD(wParam) == ID_UPDATE_CONFIG)
			{
				HideWindow();
				g_Launcher->ReadFromFile(g_IniPath);
			}
			else if(LOWORD(wParam) == ID_EDIT_CONFIG)
			{
				HideWindow();
				g_EditIni->Start(g_IniPath, hWnd, WM_COMMAND, ID_UPDATE_CONFIG);
			}
			else
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CREATE:
			if(!OnCreate(hWnd)) return -1;
			SetAero(hWnd);
			uiTaskbarRestart = RegisterWindowMessage(_T("TaskbarCreated"));
			break;
		case WM_DWMCOMPOSITIONCHANGED:
			SetAero(hWnd);
			break;
		#if ENABLE_HOOK
		case WM_SP_KEYDOWN:
			OnSpKeyDown(wParam, lParam);
			break;
		case WM_SP_KEYUP:
			OnSpKeyUp(wParam, lParam);
			break;
		#else
		case WM_KEYDOWN:
			if(wParam==VK_DOWN)
			{
				g_WndList->MoveCursor(1);
				InvalidateItem();
			}
			else if(wParam==VK_UP)
			{
				g_WndList->MoveCursor(-1);
				InvalidateItem();
			}
			else if(wParam==VK_RETURN)
			{
				g_WndList->Activate();
				HideWindow();
			}
			else
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		#endif
		case WM_LBUTTONDOWN:
			if(g_WndList->SelectItem(PointToItem(MAKEPOINTS(lParam))))
			{
				InvalidateItem();
			}
			break;
		case WM_LBUTTONUP:
			if(g_WndList->SelectItem(PointToItem(MAKEPOINTS(lParam))))
			{
				g_WndList->Activate();
				HideWindow();
			}
			break;
		case WM_MOUSEMOVE:
			if(wParam & MK_LBUTTON){
				if(g_WndList->SelectItem(PointToItem(MAKEPOINTS(lParam))))
				{
					InvalidateItem();
				}
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			OnPaint(hWnd, hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			OnDestroy(hWnd);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}

static void CreateNotifyIcon(HWND hwnd)
{
	ZeroMemory(&g_Notify, sizeof(NOTIFYICONDATA));
	g_Notify.cbSize = sizeof(NOTIFYICONDATA);
	g_Notify.uID    = 1;
	g_Notify.hWnd   = hwnd;
	g_Notify.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	g_Notify.hIcon  = GetResourceIconHandle(true);
	g_Notify.uCallbackMessage = WM_TASKICON;
	_tcscpy_s(g_Notify.szTip, 64, g_AppName);
	Shell_NotifyIcon(NIM_ADD, &g_Notify);
}

static BOOL OnCreate(HWND hwnd)
{
	g_ImageList = new CImageList();
	g_ImageList->Create();

	LOGFONT				logfont;
	NONCLIENTMETRICS	NcMetrics;
	NcMetrics.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &NcMetrics, 0);
	logfont = NcMetrics.lfMenuFont;
	logfont.lfHeight  = 15;
	logfont.lfWeight  = FW_NORMAL;
	logfont.lfQuality = DEFAULT_QUALITY;
	g_NormalFont      = CreateFontIndirect(&logfont);
	logfont.lfItalic  = TRUE;
	g_LaunchFont      = CreateFontIndirect(&logfont);
	g_AppMenu         = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_MENU1));

	SendMessage(hwnd, WM_SETICON, (WPARAM)ICON_BIG,   (LPARAM)GetResourceIconHandle(false));
	SendMessage(hwnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)GetResourceIconHandle(true));

	CreateNotifyIcon(hwnd);

	#if ENABLE_HOOK
	Hook(hwnd, WM_SP_KEYDOWN, WM_SP_KEYUP);	
	#endif

	BOOL bResult;
	bResult = RegisterHotKey(hwnd, 0, MOD_ALT, VK_TAB);
	bResult = RegisterHotKey(hwnd, 1, MOD_ALT|MOD_SHIFT, VK_TAB);
	
	return TRUE;
}

static void OnDestroy(HWND hwnd)
{
	UnregisterHotKey(hwnd, 0);
	UnregisterHotKey(hwnd, 1);
	
#if ENABLE_HOOK
	Unhook();	
	#endif
	Shell_NotifyIcon(NIM_DELETE, &g_Notify);
	DestroyMenu(g_AppMenu);
	if(g_ImageList) delete g_ImageList;
	DeleteObject(g_NormalFont);
	DeleteObject(g_LaunchFont);
}

static void GetItemRect(int idx, LPRECT lpRect)
{
	int offset = g_Aero ? 2 : 0;
	RECT	client;
	GetClientRect(g_hWnd, &client);
	
	
	
	lpRect->left   = 0;
	lpRect->top    = idx * g_ItemHeight + offset;
	lpRect->right  = client.right-1;
	lpRect->bottom = (idx+1) * g_ItemHeight + offset;
}

static int PointToItem(POINTS pt)
{
	return pt.y / g_ItemHeight;
}

static void InvalidateItem(void)
{
	RECT	rect;
	
	if(g_WndList->GetSelectedIndex() != g_WndList->GetLastSelectedIndex())
	{
		GetItemRect(g_WndList->GetSelectedIndex(), &rect);
		InvalidateRect(g_hWnd, &rect, TRUE);
		GetItemRect(g_WndList->GetLastSelectedIndex(), &rect);
		InvalidateRect(g_hWnd, &rect, TRUE);
	}
}

#ifdef USE_GRADIENT
static void FillGradient(HDC hdc, LPRECT lpRect, COLORREF col1, COLORREF col2)
{
	TRIVERTEX		vt[2];
	GRADIENT_RECT	rc;
	
	vt[0].x     = lpRect->left;
	vt[0].y     = lpRect->top;
	vt[0].Red   = GetRValue(col1) << 8;
	vt[0].Green = GetGValue(col1) << 8;
	vt[0].Blue  = GetBValue(col1) << 8;
	vt[0].Alpha = 0xFF00;

	vt[1].x     = lpRect->right;
	vt[1].y     = lpRect->bottom;
	vt[1].Red   = GetRValue(col2) << 8;
	vt[1].Green = GetGValue(col2) << 8;
	vt[1].Blue  = GetBValue(col2) << 8;
	vt[1].Alpha = 0xFF00;

	rc.UpperLeft  = 0;
	rc.LowerRight = 1;

	GradientFill(hdc, vt, 2, &rc, 1, GRADIENT_FILL_RECT_H);
}
#endif

static void OnPaint(HWND hwnd, HDC hdc)
{
	HGDIOBJ old_font = SelectObject(hdc, g_NormalFont);
	
	int count = g_WndList->GetCount();
	SetBkMode(hdc, TRANSPARENT);
	UINT	ImageStyle = ILD_NORMAL;
	RECT	rect;
	CWndInfo*	info = (CWndInfo*)g_WndList->GetTop(0);
	GetItemRect(0, &rect);
	for(int i=0;i<count;i++)
	{
		if(info->IsSelected())
		{
#ifdef USE_GRADIENT
			FillGradient(hdc, &rect, GetSysColor(COLOR_ACTIVECAPTION), GetSysColor(COLOR_GRADIENTACTIVECAPTION));
#else
			FillRect(hdc, &rect, (HBRUSH)(COLOR_ACTIVECAPTION+1));
#endif
			SetTextColor(hdc, GetSysColor(COLOR_CAPTIONTEXT));
		}
		else{
			if(g_Aero){
				if(info->IsLauncher())
				{
					SetTextColor(hdc, GetSysColor(COLOR_3DDKSHADOW));
				}
				else{
					SetTextColor(hdc, GetSysColor(COLOR_3DSHADOW));
				}
			}
			else{
				if(info->IsLauncher())
				{
					SetTextColor(hdc, GetSysColor(COLOR_3DSHADOW));
				}
				else{
					SetTextColor(hdc, GetSysColor(COLOR_MENUTEXT));
				}
			}
		}
		if(info->IsLauncher())
		{
			SelectObject(hdc, g_LaunchFont);
			ImageStyle = ILD_BLEND25;
		}
		else{
			SelectObject(hdc, g_NormalFont);
			ImageStyle = ILD_NORMAL;
		}
		g_ImageList->Draw(i, hdc, g_ItemIconX, rect.top+g_ItemIconY, ImageStyle);
		TextOut(hdc, g_ItemTextX, rect.top+g_ItemTextY, info->GetTitle(), _tcslen(info->GetTitle()));
		info = (CWndInfo*)info->GetNext(0);
		rect.top    += g_ItemHeight;
		rect.bottom += g_ItemHeight;
	}
	
	SelectObject(hdc, old_font);
}

static int GetItemWidth(LPCTSTR title)
{
	HDC	hdc = GetDC(g_hWnd);
	HGDIOBJ	old = SelectObject(hdc, g_NormalFont);
	SIZE	size;
	BOOL	result = GetTextExtentPoint32(hdc, title, _tcslen(title), &size);
	size.cx += g_TextMargin;
	SelectObject(hdc, old);
	ReleaseDC(g_hWnd, hdc);
	return (!result) ? 400 : size.cx;
}

static void OnListUpdate(void)
{
	g_WndList->Create(g_Launcher);
	int count = g_WndList->GetCount();
	g_ImageList->SetCount(count);
	int		width = 0;
	for(int i=0;i<count;i++){
		BOOL	need_destroy;
		HICON hIcon = (*g_WndList)[i]->GetIcon(need_destroy);
		g_ImageList->ReplaceIcon(i, hIcon);
		if(need_destroy)
		{
			DestroyIcon(hIcon);
		}
		int w = GetItemWidth((*g_WndList)[i]->GetTitle());
		if(w>width) width = w;
		if(i==1 && !(*g_WndList)[i]->IsLauncher())
		{
			g_WndList->MoveCursor(1);
		}
	}
	int fw = GetSystemMetrics(SM_CXFRAME) * 2;
	int fh = GetSystemMetrics(SM_CYFRAME) * 2 + (g_Aero ? GetSystemMetrics(SM_CYCAPTION) : 0);
	int ww = width + g_ItemTextX + fw;
	if(ww<g_MinWidth) ww = g_MinWidth;
	if(ww>g_MaxWidth) ww = g_MaxWidth;
	int wh = g_ItemHeight*count + fh;
	int sw = GetSystemMetrics(SM_CXSCREEN);
	int sh = GetSystemMetrics(SM_CYSCREEN);
	int x  = (sw - ww) / 2;
	int y  = (sh - wh) / 2;
	SetWindowPos(g_hWnd, (HWND)HWND_TOPMOST, x, y, ww, wh, SWP_SHOWWINDOW|SWP_NOCOPYBITS);
	
	
	InvalidateRect(g_hWnd, NULL, TRUE);
	CWndInfo::SetActiveWindow(g_hWnd);
}

static void MenuOpen(void)
{
	POINT	pt;
	
	SetForegroundWindow(g_hWnd);
	GetCursorPos(&pt);
	HMENU hSub = GetSubMenu(g_AppMenu, 0);
	TrackPopupMenu(hSub, TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_LEFTBUTTON, pt.x, pt.y, 0, g_hWnd, NULL);
}

static void OnSpKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch(wParam){
	case	VK_TAB:
		if(!IsWindowVisible(g_hWnd)){
			OnListUpdate();
		}
		else{
			if(!g_Shift){
				g_WndList->MoveCursor(1);
			}
			else{
				g_WndList->MoveCursor(-1);
			}
			InvalidateItem();
		}
		break;
	case	VK_ESCAPE:
	case	VK_KANJI:
		HideWindow();
		break;
	case	VK_UP:
		g_WndList->MoveCursor(-1);
		InvalidateItem();
		break;
	case	VK_DOWN:
		g_WndList->MoveCursor(1);
		InvalidateItem();
		break;
	case	VK_LSHIFT:
	case	VK_RSHIFT:
		g_Shift = TRUE;
		break;
	}
}

static void OnSpKeyUp(WPARAM wParam, LPARAM lParam)
{
	switch(wParam){
	case	VK_LSHIFT:
	case	VK_RSHIFT:
		g_Shift = FALSE;
		break;
	case	VK_LMENU:
	case	VK_RMENU:
		if(IsWindowVisible(g_hWnd)){
			g_WndList->Activate();
			HideWindow();
		}
		break;
	}
}

static void HideWindow(void)
{
	ShowWindow(g_hWnd, SW_HIDE);
	g_WndList->RemoveAll();
	g_ImageList->SetCount(0);
}

