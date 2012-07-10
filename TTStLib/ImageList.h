#pragma once
#include <commctrl.h>

class CImageList
{
private:
	HIMAGELIST	m_hImageList;
public:
	CImageList(void);
	virtual ~CImageList(void);
public:
	BOOL	Create(void);
	BOOL	Destroy(void);
	BOOL	Clear(void);
	void	Draw(int idx, HDC hdc, int x, int y, UINT fStyle=ILD_NORMAL, COLORREF color=CLR_NONE);
	int		AddIcon(HICON hIcon);
	int		ReplaceIcon(int idx, HICON hIcon);
	BOOL	SetCount(UINT newCount);
	int		GetCount(void);
};
