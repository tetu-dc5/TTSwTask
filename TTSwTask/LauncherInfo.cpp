#include "StdAfx.h"
#include "LauncherInfo.h"

CLauncherInfo::CLauncherInfo(void)
: CWndInfo(1)
, m_Arg(NULL)
, m_WorkDirectory(NULL)
, m_CheckModulePath(NULL)
, m_CheckTitle(NULL)
, m_IconPath(NULL)
, m_IconIndex(0)
{
}

CLauncherInfo::~CLauncherInfo(void)
{
	if(m_Arg)				delete [] m_Arg;
	if(m_WorkDirectory)		delete [] m_WorkDirectory;
	if(m_CheckModulePath)	delete [] m_CheckModulePath;
	if(m_CheckTitle)		delete [] m_CheckTitle;
	if(m_IconPath)			delete [] m_IconPath;
}

HICON CLauncherInfo::GetIcon(BOOL& need_destroy)
{
	LPCTSTR	path;
	int		idx;
	if(m_IconPath)
	{
		path = m_IconPath;
		idx  = m_IconIndex;
	}
	else
	{
		path = m_ModulePath;
		idx  = 0;
	}
	return _GetIcon(path, idx, need_destroy);
}

void CLauncherInfo::SetActive(void)
{
	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;
	TCHAR				args[1024];
	BOOL				result;
	DWORD				err;
	LPVOID				lpMsg;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	_tcscpy_s(args, 1024, _T("\""));
	_tcscat_s(args, 1024, m_ModulePath);
	_tcscat_s(args, 1024, _T("\" "));
	if(m_Arg) _tcscat_s(args, 1024, m_Arg);
	result = CreateProcess(NULL, args, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, m_WorkDirectory, &si, &pi);
	if(result)
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		err = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,err,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsg,0,NULL);
		MessageBox(NULL, (LPCTSTR)lpMsg, _T("ERROR"), MB_OK|MB_ICONERROR);
		LocalFree(lpMsg);
	}
}

void CLauncherInfo::Setup(LPLII_PARAMS params)
{
	m_Title           = params->title;
	m_ModulePath      = params->module_path;
	m_Arg             = params->arg;
	m_WorkDirectory   = params->workdirectory;
	m_CheckModulePath = params->checkpath;
	m_CheckTitle      = params->checktitle;
	m_IconPath        = params->iconpath;
	m_IconIndex       = params->iconindex;
}
