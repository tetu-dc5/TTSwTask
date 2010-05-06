#include "StdAfx.h"
#include "TTSwTask.h"
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
	TCHAR*	p = new TCHAR[TMP_BUF_SIZE * 7];
	m_params.title         = p + TMP_BUF_SIZE*0;
	m_params.module_path   = p + TMP_BUF_SIZE*1;
	m_params.arg           = p + TMP_BUF_SIZE*2;
	m_params.workdirectory = p + TMP_BUF_SIZE*3;
	m_params.checkpath     = p + TMP_BUF_SIZE*4;
	m_params.checktitle    = p + TMP_BUF_SIZE*5;
	m_params.iconpath      = p + TMP_BUF_SIZE*6;
	
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
	while(*p==_T('\t')) p++;
	return p;
}

BOOL CLauncher::ReadFromFile(LPCTSTR fname)
{
	FILE*	fp;
	TCHAR	line_buf[TMP_BUF_SIZE];
	LPTSTR	p;

#ifdef _UNICODE
	if(_tfopen_s(&fp, fname, _T("rt, ccs=UTF-8"))) return FALSE;
#else
	if(_tfopen_s(&fp, fname, _T("rt"))) return FALSE;
#endif
	CreateParams();		
	RemoveAll();
	TCHAR*	context = NULL;
	for(;;)
	{
		if(!_fgetts(line_buf, TMP_BUF_SIZE, fp)) break;
		
		p = _tcspbrk(line_buf, _T(";\r\n"));
		if(p) *p = _T('\0');
		p = TrimLeft(line_buf);

		if(p[0] == _T('@'))
		{
			FlashParams();
			_tcscpy_s(m_params.title, TMP_BUF_SIZE, p+1);
		}
		else if(p[0] == _T('P'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.module_path, TMP_BUF_SIZE, p);
		}
		else if(p[0] == _T('A'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.arg, TMP_BUF_SIZE, p);
		}
		else if(p[0] == _T('W'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.workdirectory, TMP_BUF_SIZE, p);
		}
		else if(p[0] == _T('C'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.checkpath, TMP_BUF_SIZE, p);
		}
		else if(p[0] == _T('N'))
		{
			p = TrimLeft(p+1);
			_tcscpy_s(m_params.checktitle, TMP_BUF_SIZE, p);
		}
		else if(p[0] == _T('I'))
		{
			context = NULL;
			p = TrimLeft(p+1);
			LPTSTR	file  = _tcstok_s(p, _T(","), &context);
			_tcscpy_s(m_params.iconpath, TMP_BUF_SIZE, file);
			LPTSTR	index = _tcstok_s(NULL, _T(","), &context);
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
