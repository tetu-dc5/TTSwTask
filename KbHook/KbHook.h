// �ȉ��� ifdef �u���b�N�� DLL ����̃G�N�X�|�[�g��e�Ղɂ���}�N�����쐬���邽�߂� 
// ��ʓI�ȕ��@�ł��B���� DLL ���̂��ׂẴt�@�C���́A�R�}���h ���C���Œ�`���ꂽ KBHOOK_EXPORTS
// �V���{���ŃR���p�C������܂��B���̃V���{���́A���� DLL ���g���v���W�F�N�g�Œ�`���邱�Ƃ͂ł��܂���B
// �\�[�X�t�@�C�������̃t�@�C�����܂�ł��鑼�̃v���W�F�N�g�́A 
// KBHOOK_API �֐��� DLL ����C���|�[�g���ꂽ�ƌ��Ȃ��̂ɑ΂��A���� DLL �́A���̃}�N���Œ�`���ꂽ
// �V���{�����G�N�X�|�[�g���ꂽ�ƌ��Ȃ��܂��B
#ifdef KBHOOK_EXPORTS
#define KBHOOK_API __declspec(dllexport)
#else
#define KBHOOK_API __declspec(dllimport)
#endif

// ���̃N���X�� KbHook.dll ����G�N�X�|�[�g����܂����B
class KBHOOK_API CKbHook {
public:
	CKbHook(void);
	// TODO: ���\�b�h�������ɒǉ����Ă��������B
};

extern KBHOOK_API int nKbHook;

KBHOOK_API int fnKbHook(void);
