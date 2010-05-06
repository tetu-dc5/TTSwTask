#pragma once
#include "InfoList.h"
#include "LauncherInfo.h"

class CWndList;
class CLauncher : public CInfoList
{
private:
	LII_PARAMS	m_params;
	void	CreateParams(void);
	void	DestroyParams(void);
	void	FlashParams(void);
	LPTSTR	TrimLeft(LPTSTR src);
	LPTSTR	CopyString(LPCTSTR src);
public:
	CLauncher(void);
	virtual ~CLauncher(void);
public:
	CLauncherInfo*	operator[](int idx)	{	return (CLauncherInfo*)GetAt(idx);	}
	BOOL			ReadFromFile(LPCTSTR fname);
	void			AddListTo(CWndList* list);
};
