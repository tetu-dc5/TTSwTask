#include "StdAfx.h"
#include "WndList.h"
#include <Tlhelp32.h>


CWndList::CWndList(void)
: CInfoList(0)
, m_cursor(0)
, m_last_cursor(0)
{
}

CWndList::~CWndList(void)
{
}

BOOL CWndList::IsListup(HWND hwnd)
{
	RECT	rect;
	
	if(!IsWindowVisible(hwnd)) return FALSE;
	if(IsHungAppWindow(hwnd)) return FALSE;
	if(GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return FALSE;
	if(GetWindowLong(hwnd, GWL_STYLE) & WS_DISABLED) return FALSE;
	if(!GetWindowRect(hwnd, &rect)) return FALSE;
	if(rect.top >= rect.bottom) return FALSE;
	if(rect.left >= rect.right) return FALSE;
	return TRUE;
}

BOOL CALLBACK CWndList::EnumWindowProc(HWND hwnd, LPARAM lParam)
{
	CWndList*	pThis = (CWndList*)lParam;
	if(IsListup(hwnd))
	{
		CWndInfo*	info = new CWndInfo();
		info->Setup(hwnd, pThis->m_hSnap);
		if(info->Enable())
		{
			pThis->AddNode(info);
		}
		else
		{
			delete info;
		}
	}
	return TRUE;
}

void CWndList::Create(CLauncher* launcher)
{
	RemoveAll();
	m_hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	EnumChildWindows(NULL, EnumWindowProc, (LPARAM)this);
	if(m_hSnap != INVALID_HANDLE_VALUE){
		CloseHandle(m_hSnap);
	}
	if(launcher)
	{
		launcher->AddListTo(this);
	}
	if(GetCount()>0){
		m_cursor = 0;
		(*this)[m_cursor]->SetSelect(TRUE);
	}
}

/*
HICON CWndList::GetIcon(int idx, BOOL& need_destroy)
{
	CWndInfo*	info = (*this)[idx];
	if(!info){
		need_destroy = FALSE;
		return NULL;
	}
	else
	{
		return info->GetIcon(need_destroy);
	}
}
*/

BOOL CWndList::MoveCursor(int dir)
{
	int		count = GetCount();
	if(count){
		m_last_cursor = m_cursor;
		m_cursor += dir;
		while(m_cursor < 0) m_cursor+=count;
		m_cursor %= count;
		(*this)[m_last_cursor]->SetSelect(FALSE);
		(*this)[m_cursor]->SetSelect(TRUE);
		return TRUE;
	}
	return FALSE;
}

BOOL CWndList::SelectItem(int idx)
{
	int		count = GetCount();
	if((idx < count) && (idx >= 0)){
		m_last_cursor = m_cursor;
		m_cursor = idx;
		(*this)[m_last_cursor]->SetSelect(FALSE);
		(*this)[m_cursor]->SetSelect(TRUE);
		return TRUE;
	}
	return FALSE;
}

void CWndList::Activate(void)
{
	(*this)[m_cursor]->SetActive();
}
