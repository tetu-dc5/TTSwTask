#include "stdafx.h"
#include "util.h"


LPTSTR CopyString(LPCTSTR src)
{
	if(!src[0]) return NULL;
	int		len = _tcslen(src)+10;
	LPTSTR	buf = new TCHAR[len];
	_tcscpy_s(buf, len, src);
	return buf;
}
