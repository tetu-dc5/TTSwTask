#include "StdAfx.h"
#include "EditIniFile.h"
#include <process.h>
#include <shellapi.h>


CEditIniFile::CEditIniFile(void)
: m_hThread(NULL)
, m_hProcess(NULL)
, m_hWnd(NULL)
, m_message(0)
, m_wParam(0)
, m_lParam(NULL)
{
}

CEditIniFile::~CEditIniFile(void)
{
	Wait(true);
}

unsigned int __stdcall CEditIniFile::threadFunc(void* arg)
{
	CEditIniFile*	pThis = (CEditIniFile*)arg;
	
	DWORD result = WaitForSingleObject(pThis->m_hProcess, INFINITE);
	if(pThis->m_hWnd)
	{
		PostMessage(pThis->m_hWnd, pThis->m_message, pThis->m_wParam, pThis->m_lParam);
	}
	return result;
}

void CEditIniFile::_close(void)
{
	CloseHandle(m_hThread);
	m_hThread = NULL;
}

bool CEditIniFile::Start(LPCTSTR ini_path, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//	処理中なら不可
	if(!Wait(false)) return false;
	
	//	アプリケーション起動
	SHELLEXECUTEINFO	info;
	ZeroMemory(&info, sizeof(SHELLEXECUTEINFO));
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.nShow = SW_SHOW;
	info.fMask = SEE_MASK_NOCLOSEPROCESS;
	info.lpFile = ini_path;
	if(!ShellExecuteEx(&info)) return false;
	if((int)info.hInstApp < 32 || info.hProcess == NULL) return false;
	
	//	待ちスレッド起動
	this->m_hProcess = info.hProcess;
	this->m_hWnd = hWnd;
	this->m_message = message;
	this->m_wParam = wParam;
	this->m_lParam = lParam;
	int result = (unsigned int)_beginthreadex(NULL, 0x8000, threadFunc, this, 0, NULL);
	if(result == 0) return false;
	m_hThread = (HANDLE)result;
	return true;
}

bool CEditIniFile::Wait(bool block)
{
	if(!m_hThread) return true;
	DWORD	result = WaitForSingleObject(m_hThread, block ? INFINITE : 0);
	if(result==WAIT_OBJECT_0) _close();
	return (result!=WAIT_TIMEOUT);

}
