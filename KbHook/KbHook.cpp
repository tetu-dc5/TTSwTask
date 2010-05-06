// KbHook.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include "KbHook.h"


// これは、エクスポートされた変数の例です。
KBHOOK_API int nKbHook=0;

// これは、エクスポートされた関数の例です。
KBHOOK_API int fnKbHook(void)
{
	return 42;
}

// これは、エクスポートされたクラスのコンストラクタです。
// クラス定義に関しては KbHook.h を参照してください。
CKbHook::CKbHook()
{
	return;
}
