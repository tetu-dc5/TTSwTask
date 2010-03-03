#pragma once
#include "wndinfo.h"

typedef struct LII_PARAMS
{
	LPTSTR		title;
	LPTSTR		module_path;
	LPTSTR		arg;
	LPTSTR		workdirectory;
	LPTSTR		checkpath;
	LPTSTR		checktitle;
	LPTSTR		iconpath;
	UINT		iconindex;
} LII_PARAMS;
typedef LII_PARAMS*	LPLII_PARAMS;

class CLauncher;
class CLauncherInfo : public CWndInfo
{
	friend	class CLauncher;
public:
	CLauncherInfo(void);
	virtual ~CLauncherInfo(void);
protected:
	LPCTSTR		m_Arg;
	LPCTSTR		m_WorkDirectory;
	LPCTSTR		m_CheckModulePath;
	LPCTSTR		m_CheckTitle;
	LPCTSTR		m_IconPath;
	UINT		m_IconIndex;
public:
	virtual	BOOL	IsLauncher(void)	{	return TRUE;					}
	virtual	HICON	GetIcon(BOOL& need_destroy);
	virtual	void	SetActive(void);
			void	Setup(LPLII_PARAMS params);
};
