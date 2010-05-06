#pragma once
#include "InfoNode.h"

class CLauncher;
class CWndInfo : public CInfoNode
{
	friend	class CLauncher;
public:
	CWndInfo(void);
	CWndInfo(int channel);
	virtual ~CWndInfo(void);
protected:
	DWORD	m_PID;
	HWND	m_hWnd;
	LPCTSTR	m_ModulePath;
	LPCTSTR	m_Title;
	BOOL	m_Selected;
protected:
	static	HICON	_GetIcon(HWND hwnd, BOOL& need_destroy);
	static	HICON	_GetIcon(LPCTSTR path, int idx, BOOL& need_destroy);
public:
	virtual	BOOL	IsLauncher(void)	{	return FALSE;					}
			LPCTSTR	GetTitle(void)		{	return m_Title;					}
	virtual	HICON	GetIcon(BOOL& need_destroy);
	virtual	void	SetActive(void);
			void	Setup(HWND hwnd);
			BOOL	Enable(void)		{	return m_Title ? (m_Title[0] ? TRUE : FALSE) : FALSE;	}
			void	SetSelect(BOOL sw)	{	m_Selected = sw;				}
			BOOL	IsSelected(void)	{	return m_Selected;				}
};
