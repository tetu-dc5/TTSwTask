// TTSwTask.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "TTSwTask.h"

#pragma	comment(lib, "msimg32.lib")

HINSTANCE	g_hInst       = NULL;
HWND		g_hWnd        = NULL;
CImageList*	g_ImageList   = NULL;
CWndList*	g_WndList     = NULL;
CLauncher*	g_Launcher    = NULL;
LPCTSTR		g_IniPath     = NULL;
HGDIOBJ		g_NormalFont  = NULL;
HGDIOBJ		g_LaunchFont  = NULL;
LPCTSTR		g_AppName     = _T("TTSwTask");
const int	g_ItemHeight  = 20;
const int	g_ItemIconX   = 2;
const int	g_ItemIconY   = 2;
const int	g_ItemTextX   = 20;
const int	g_ItemTextY   = 3;
const int	g_MinWidth    = 400;
const int	g_MaxWidth    = 800;
const int	g_TextMargin  = 2;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				OnCreate(HWND hwnd);
void				OnPaint(HWND hwnd, HDC hdc);
void				GetIniPath(void);
void				OnListUpdate(void);
void				GetItemRect(int idx, LPRECT lpRect);
void				FillGradient(HDC hdc, LPRECT lpRect, COLORREF col1, COLORREF col2);
void				OnDestroy(HWND hwnd);
void				InvalidateItem(int idx);
int					GetItemWidth(LPCTSTR title);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
	MSG msg;

	GetIniPath();
	g_WndList  = new CWndList();
	g_Launcher = new CLauncher();
	g_Launcher->ReadFromFile(g_IniPath);

	MyRegisterClass(hInstance);
	if (!InitInstance (hInstance, nCmdShow)) return FALSE;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	delete [] g_IniPath;
	delete g_WndList;
	delete g_Launcher;
	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize         = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TTSWTASK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_3DFACE+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_AppName;
	wcex.hIconSm		= NULL;
	return RegisterClassEx(&wcex);
}

void GetIniPath(void)
{
	TCHAR	me[1024];
	GetModuleFileName(NULL, me, 1024);
	TCHAR* pExt = _tcsrchr(me,_T('.'));
	*pExt = _T('\0');
	_tcscat_s(me,1024,_T(".ini"));
	int len = _tcslen(me);
	g_IniPath = new TCHAR[len+10];
	_tcscpy_s((LPTSTR)g_IniPath, len+10, me);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	g_hInst = hInstance;
	hWnd = CreateWindowEx(
		WS_EX_DLGMODALFRAME,
		g_AppName,
		g_AppName, 
		WS_POPUPWINDOW,
		100, 100, 
		400, 300,
		NULL, 
		NULL, 
		hInstance, 
		NULL);
   if(!hWnd) return FALSE;
   g_hWnd = hWnd;
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   
   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		if(!OnCreate(hWnd)) return -1;
		break;
	case WM_LBUTTONDOWN:
		OnListUpdate();
		break;
	case WM_KEYDOWN:
		if(IsWindowVisible(hWnd)){
			if(wParam==VK_UP)
			{
				g_WndList->MoveCursor(-1);
				InvalidateItem(g_WndList->GetSelectedIndex());
				InvalidateItem(g_WndList->GetLastSelectedIndex());
				return 0;
			}
			else if(wParam==VK_DOWN)
			{
				g_WndList->MoveCursor(1);
				InvalidateItem(g_WndList->GetSelectedIndex());
				InvalidateItem(g_WndList->GetLastSelectedIndex());
				return 0;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
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
	return 0;
}

BOOL OnCreate(HWND hwnd)
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

	OnListUpdate();
	
	return TRUE;
}

void OnDestroy(HWND hwnd)
{
	if(g_ImageList) delete g_ImageList;
	DeleteObject(g_NormalFont);
	DeleteObject(g_LaunchFont);
}

void GetItemRect(int idx, LPRECT lpRect)
{
	RECT	client;
	GetClientRect(g_hWnd, &client);
	
	lpRect->left   = 0;
	lpRect->top    = idx * g_ItemHeight;
	lpRect->right  = client.right-1;
	lpRect->bottom = (idx+1) * g_ItemHeight;
}

void InvalidateItem(int idx)
{
	RECT	rect;
	GetItemRect(idx, &rect);
	InvalidateRect(g_hWnd, &rect, TRUE);
}

void FillGradient(HDC hdc, LPRECT lpRect, COLORREF col1, COLORREF col2)
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

void OnPaint(HWND hwnd, HDC hdc)
{
	HGDIOBJ old_font = SelectObject(hdc, g_NormalFont);
	
	int count = g_WndList->GetCount();
	int		y = 0;
	int		x = 0;
	SetBkMode(hdc, TRANSPARENT);
	UINT	ImageStyle = ILD_NORMAL;
	RECT	rect;
	for(int i=0;i<count;i++)
	{
		CWndInfo*	info = (*g_WndList)[i];
		GetItemRect(i, &rect);
		if(info->IsSelected())
		{
			FillGradient(hdc, &rect, GetSysColor(COLOR_ACTIVECAPTION), GetSysColor(COLOR_GRADIENTACTIVECAPTION));
			SetTextColor(hdc, GetSysColor(COLOR_CAPTIONTEXT));
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
		if(info->IsLauncher())
		{
			SelectObject(hdc, g_LaunchFont);
			ImageStyle = ILD_BLEND25;
		}
		else{
			SelectObject(hdc, g_NormalFont);
			ImageStyle = ILD_NORMAL;
		}
		g_ImageList->Draw(i, hdc, x+g_ItemIconX, y+g_ItemIconY, ImageStyle);
		TextOut(hdc, x+g_ItemTextX, y+g_ItemTextY, info->GetTitle(), _tcslen(info->GetTitle()));
		y+=g_ItemHeight;
	}
	
	SelectObject(hdc, old_font);
}

int GetItemWidth(LPCTSTR title)
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

void OnListUpdate(void)
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
	}
	int fw = GetSystemMetrics(SM_CXFRAME) * 2;
	int fh = GetSystemMetrics(SM_CYFRAME) * 2;
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
}