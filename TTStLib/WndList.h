#pragma once
#include "InfoList.h"
#include "Launcher.h"

class CWndList : public CInfoList
{
public:
	CWndList(void);
	virtual ~CWndList(void);
private:
	int			m_cursor;
	int			m_last_cursor;
	HANDLE		m_hSnap;
private:
	static	BOOL			IsListup(HWND hwnd);
	static	BOOL CALLBACK	EnumWindowProc(HWND hwnd, LPARAM lParam);
public:
	CWndInfo*	operator[](int idx)			{	return (CWndInfo*)GetAt(idx);	}
	void		Create(CLauncher* launcher);
//	HICON		GetIcon(int idx, BOOL& need_destroy);
	BOOL		MoveCursor(int dir);
	BOOL		SelectItem(int idx);
	int			GetSelectedIndex(void)		{	return m_cursor;				}
	int			GetLastSelectedIndex(void)	{	return m_last_cursor;			}
	void		Activate(void);
};
