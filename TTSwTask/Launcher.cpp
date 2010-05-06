#include "StdAfx.h"
#include "Launcher.h"
#include "WndList.h"
#include <stdio.h>
#include <stdlib.h>

CLauncher::CLauncher(void)
: CInfoList(1)
{
	ZeroMemory(&m_params, sizeof(LII_PARAMS));
}

CLauncher::~CLauncher(void)
{
	DestroyParams();
}

void CLauncher::CreateParams(void)
{
	TCHAR*	p = new TCHAR[1024 * 7];
	m_params.title         = p + 1024*0;
	m_params.module_path   = p + 1024*1;
	m_params.arg           = p + 1024*2;
	m_params.workdirectory = p + 1024*3;
	m_params.checkpath     = p + 1024*4;
	m_params.checktitle    = p + 1024*5;
	m_params.iconpath      = p + 1024*6;
	
	m_params.title[0]         = 0;
	m_params.module_path[0]   = 0;
	m_params.arg[0]           = 0;
	m_params.workdirectory[0] = 0;
	m_params.checkpath[0]     = 0;
	m_params.checktitle[0]    = 0;
	m_params.iconpath[0]      = 0;
	m_params.iconindex        = 0;
}

void CLauncher::DestroyParams(void)
{
	if(m_params.title) delete [] m_params.title;
	ZeroMemory(&m_params, sizeof(LII_PARAMS));
}

LPTSTR	CLauncher::CopyString(LPCTSTR src)
{
	if(!src[0]) return NULL;
	int		len = _tcslen(src)+10;
	LPTSTR	buf = new TCHAR[len];
	_tcscpy_s(buf, len, src);
	return buf;
}

void CLauncher::FlashParams(void)
{
	if(m_params.title[0] && m_params.module_path[0])
	{
		LII_PARAMS	params;

		ZeroMemory(&params, sizeof(LII_PARAMS));
		params.title         = CopyString(m_params.title);
		params.module_path   = CopyString(m_params.module_path);
		params.arg           = CopyString(m_params.arg);
		params.workdirectory = CopyString(m_params.workdirectory);
		params.checkpath     = CopyString(m_params.checkpath);
		params.checktitle    = CopyString(m_params.checktitle);
		params.iconpath      = CopyString(m_params.iconpath);
		params.iconindex     = m_params.iconindex;

		CLauncherInfo*	info = new CLauncherInfo();
		info->Setup(&params);
		this->AddNode(info);
	}
	
	m_params.title[0]         = 0;
	m_params.module_path[0]   = 0;
	m_params.arg[0]           = 0;
	m_params.workdirectory[0] = 0;
	m_params.checkpath[0]     = 0;
	m_params.checktitle[0]    = 0;
	m_params.iconpath[0]      = 0;
	m_params.iconindex        = 0;
}

LPTSTR	CLauncher::TrimLeft(LPTSTR src)
{
	LPTSTR p = src;
	while(*p==_T('\t') || *p==_T('\r') || *p==_T('\n')) p++;
	return p;
}

BOOL CLauncher::ReadFromFile(LPCTSTR fname)
{
	FILE*	fp;
	TCHAR	line_buf[1024];
	LPTSTR	p;

#ifdef _UNICODE
	if(_tfopen_s(&fp, fname, _T("rt, ccs=UTF-8"))) return FALSE;
#else
	if(_tfopen_s(&fp, fname, _T("rt"))) return FALSE;
#endif
	CreateParams();		
	RemoveAll();
	for(;;)
	{
		if(!_fgetts(line_buf, 1024, fp)) break;
		
		p = _tcschr(line_buf, _T(';'));
		if(p) *p = _T('\0');
		p = _tcschr(line_buf, _T('\r'));
		if(p) *p = _T('\0');
		p = _tcschr(line_buf, _T('\n'));
		if(p) *p = _T('\0');
		p = TrimLeft(line_buf);

		if(p[0] == _T('@'))
		{
			FlashParams();
			_tcscpy_s(m_params.title, 1024, p+1);
		}
		else if(p[0] == _T('P'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.module_path, 1024, p);
		}
		else if(p[0] == _T('A'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.arg, 1024, p);
		}
		else if(p[0] == _T('W'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.workdirectory, 1024, p);
		}
		else if(p[0] == _T('C'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.checkpath, 1024, p);
		}
		else if(p[0] == _T('N'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.checktitle, 1024, p);
		}
		else if(p[0] == _T('I'))
		{
			TCHAR*	context = NULL;
			p = TrimLeft(p+1);
			LPTSTR	file  = _tcstok_s(p, _T(","), &context);
			_tcscpy_s(m_params.iconpath, 1024, file);
			LPTSTR	index = _tcstok_s(p, _T(","), &context);
			if(index !=NULL)
			{
				int num = _tstoi(index);
				m_params.iconindex = num;
			}
		}
	}
	FlashParams();
	DestroyParams();
	fclose(fp);
	return TRUE;
}


void CLauncher::AddListTo(CWndList* list)
{
	int	list_count = list->GetCount();
	int launch_count = GetCount();

	CLauncherInfo*	info = (CLauncherInfo*)GetTop();
	while(info)
	{
		LPCTSTR	path  = (info->m_CheckModulePath) ? info->m_CheckModulePath : info->m_ModulePath;
		LPCTSTR	title = info->m_CheckTitle;
		BOOL	add = TRUE;
		CWndInfo*	wnd = (CWndInfo*)list->GetTop();
		while(wnd)
		{
			if(!_tcsicmp(path, wnd->m_ModulePath))
			{
				if(!title || _tcsstr(wnd->m_Title, title)){
					add = FALSE;
					break;
				}
			}
			wnd = (CWndInfo*)wnd->GetNext(0);
		}
		if(add)
		{
			info->SetSelect(FALSE);
			list->AddNode(info);
		}
		info = (CLauncherInfo*)info->GetNext(1);
	}

}