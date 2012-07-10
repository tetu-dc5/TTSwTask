#include "StdAfx.h"
#include "util.h"
#include "WndInfo.h"
#include <psapi.h>
#include <shellapi.h>
#include <Tlhelp32.h>

#pragma	comment(lib, "psapi.lib")
#pragma	comment(lib, "shell32.lib")

HICON	CWndInfo::m_DefaultIcon = NULL;

void CWndInfo::SetDefaultIcon(HINSTANCE hInst, LPCWSTR id)
{
	m_DefaultIcon = LoadIcon(hInst, id);
}

CWndInfo::CWndInfo(void)
: CInfoNode(0)
, m_PID(0)
, m_hWnd(NULL)
, m_ModulePath(NULL)
, m_Title(NULL)
, m_Selected(FALSE)
{
}

CWndInfo::CWndInfo(int channel)
: CInfoNode(channel)
, m_PID(0)
, m_hWnd(NULL)
, m_ModulePath(NULL)
, m_Title(NULL)
, m_Selected(FALSE)
{
}

CWndInfo::~CWndInfo(void)
{
	if(m_ModulePath)	delete [] m_ModulePath;
	if(m_Title)			delete [] m_Title;
}

HICON CWndInfo::_GetIcon(HWND hwnd, BOOL& need_destroy)
{
	need_destroy = FALSE;
	HICON	hIcon = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, NULL);
	if(!hIcon)
	{
		hIcon = (HICON)GetClassLong(hwnd, GCL_HICONSM);
		if(!hIcon)
		{
			HWND	hParent = GetParent(hwnd);
			if(hParent)
			{
				hIcon = _GetIcon(hParent, need_destroy);
			}
		}
	}
	return hIcon;
}

HICON CWndInfo::_GetIcon(LPCTSTR path, int idx, BOOL& need_destroy)
{
	if(path && *path)
	{
		HICON	hIcon = NULL;
		int		cnt = ExtractIconEx(path, -1, NULL, NULL, 0);
		if(idx>=cnt) idx=0;
		if(ExtractIconEx(path, idx, NULL, &hIcon, 1)>=1)
		{
			need_destroy = TRUE;
			return hIcon;
		}
	}
	need_destroy = FALSE;
	return NULL;
}

HICON CWndInfo::GetIcon(BOOL& need_destroy)
{
	need_destroy = FALSE;
	HICON	hIcon = _GetIcon(m_hWnd, need_destroy);
	if(!hIcon)
	{
		hIcon = _GetIcon(m_ModulePath, 0, need_destroy);
		if(!hIcon)
		{
			hIcon = (HICON)SendMessage(m_hWnd, WM_GETICON, ICON_BIG, NULL);
			if(!hIcon)
			{
				need_destroy = FALSE;
				hIcon = m_DefaultIcon;
			}
		}
	}
	return hIcon;
}

void CWndInfo::SetActive(void)
{
	SetActiveWindow(m_hWnd);
}

void CWndInfo::Setup(HWND hwnd, HANDLE hSnap)
{
	TCHAR	buf[TMP_BUF_SIZE];

	m_hWnd = hwnd;
	m_Selected = FALSE;
	m_Title      = NULL;
	m_ModulePath = NULL;
	GetWindowThreadProcessId(hwnd, &m_PID);
	GetWindowText(hwnd, buf, TMP_BUF_SIZE);
	m_Title = CopyString(buf);

#if 1
	if(hSnap!=INVALID_HANDLE_VALUE){
		PROCESSENTRY32	me;
		me.dwSize = sizeof(me);
		BOOL bResult = Process32First(hSnap, &me);
		while(bResult){
			if(me.th32ProcessID==m_PID){
				m_ModulePath = CopyString(me.szExeFile);
				break;
			}
			bResult = Process32Next(hSnap, &me);
		}
	}
#else
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, m_PID);
	if(hProcess){
		HMODULE hModule;
		DWORD	cbModule;
		if(EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbModule))
		{
			if(GetModuleFileNameEx(hProcess, hModule, buf, TMP_BUF_SIZE))
			{
				m_ModulePath = CopyString(buf);
			}
		}
		CloseHandle(hProcess);
	}
#endif
}

void CWndInfo::SetActiveWindow(HWND hwnd)
{
	DWORD	result;
	
	if(IsIconic(hwnd)){
		//OpenIcon(hwnd);
		ShowWindowAsync(hwnd, SW_RESTORE);
	}
	
	HWND	prev_active = GetForegroundWindow();
	if(prev_active != hwnd){
		BringWindowToTop(hwnd);
		SetWindowPos(hwnd, HWND_TOP, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		if(!SetForegroundWindow(hwnd)){
			if(!prev_active){
				prev_active = FindWindow(_T("Shell_TrayWnd"), NULL);
			}
			DWORD	prev_thread = GetWindowThreadProcessId(prev_active, NULL);
			DWORD	next_thread = GetWindowThreadProcessId(hwnd, NULL);
			AttachThreadInput(prev_thread, next_thread, TRUE);
			SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &result, 0);
			SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, 0);
			if(!SetForegroundWindow(hwnd)){
				INPUT inp[4];
				ZeroMemory(&inp, sizeof(inp));
				inp[0].type = inp[1].type = inp[2].type = inp[3].type = INPUT_KEYBOARD;
				inp[0].ki.wVk = inp[1].ki.wVk = inp[2].ki.wVk = inp[3].ki.wVk = VK_MENU;
				inp[0].ki.dwFlags = inp[2].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
				inp[1].ki.dwFlags = inp[3].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
				SendInput(4, inp, sizeof(INPUT));
				SetForegroundWindow(hwnd);
			}
			SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (PVOID)result, 0);
			AttachThreadInput(prev_thread, next_thread, FALSE);
		}
	}
}
