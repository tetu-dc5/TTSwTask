// KbHook.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"
#include "KbHook.h"


// ����́A�G�N�X�|�[�g���ꂽ�ϐ��̗�ł��B
KBHOOK_API int nKbHook=0;

// ����́A�G�N�X�|�[�g���ꂽ�֐��̗�ł��B
KBHOOK_API int fnKbHook(void)
{
	return 42;
}

// ����́A�G�N�X�|�[�g���ꂽ�N���X�̃R���X�g���N�^�ł��B
// �N���X��`�Ɋւ��Ă� KbHook.h ���Q�Ƃ��Ă��������B
CKbHook::CKbHook()
{
	return;
}
