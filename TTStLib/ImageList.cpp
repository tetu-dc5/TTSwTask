#include "StdAfx.h"
#include "ImageList.h"

#pragma	comment(lib, "comctl32.lib")

CImageList::CImageList(void)
: m_hImageList(NULL)
{
}

CImageList::~CImageList(void)
{
	Destroy();
}

BOOL CImageList::Create(void)
{
	m_hImageList = ImageList_Create(16, 16, ILC_COLOR32|ILC_MASK, 0, 30);
	if(m_hImageList) return TRUE;
	return FALSE;
}

BOOL CImageList::Destroy(void)
{
	if(m_hImageList)
	{
		return ImageList_Destroy(m_hImageList);
	}
	return FALSE;
}

BOOL CImageList::Clear(void)
{
	return ImageList_RemoveAll(m_hImageList);
}

void CImageList::Draw(int idx, HDC hdc, int x, int y, UINT fStyle, COLORREF color)
{
	ImageList_DrawEx(m_hImageList, idx, hdc, x, y, 16, 16, CLR_NONE, color, fStyle);
}

int	 CImageList::AddIcon(HICON hIcon)
{
	return ImageList_AddIcon(m_hImageList, hIcon);
}

int	 CImageList::ReplaceIcon(int idx, HICON hIcon)
{
	return ImageList_ReplaceIcon(m_hImageList, idx, hIcon);
}

BOOL CImageList::SetCount(UINT newCount)
{
	return ImageList_SetImageCount(m_hImageList, newCount);
}

int CImageList::GetCount(void)
{
	return ImageList_GetImageCount(m_hImageList);
}
