// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための 
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された KBHOOK_EXPORTS
// シンボルでコンパイルされます。このシンボルは、この DLL を使うプロジェクトで定義することはできません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、 
// KBHOOK_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef KBHOOK_EXPORTS
#define KBHOOK_API __declspec(dllexport)
#else
#define KBHOOK_API __declspec(dllimport)
#endif

// このクラスは KbHook.dll からエクスポートされました。
class KBHOOK_API CKbHook {
public:
	CKbHook(void);
	// TODO: メソッドをここに追加してください。
};

extern KBHOOK_API int nKbHook;

KBHOOK_API int fnKbHook(void);
