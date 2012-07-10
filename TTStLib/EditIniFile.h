#pragma once

class CEditIniFile
{
private:
	HANDLE	m_hThread;
	HANDLE	m_hProcess;
	HWND	m_hWnd;
	UINT	m_message;
	WPARAM	m_wParam;
	LPARAM	m_lParam;

private:
	static	unsigned int __stdcall threadFunc(void* arg);

private:
	void	_close(void);

public:
	CEditIniFile(void);
	virtual ~CEditIniFile(void);

	bool	Start(LPCTSTR ini_path, HWND hWnd=NULL, UINT message=0, WPARAM wParam=0, LPARAM lParam=NULL);
	bool	Wait(bool block=false);
};
