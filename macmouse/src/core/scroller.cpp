
#include "StdAfx.h"
#include <stdexcept>
#include "scroller.h"

/**
 * �}�E�X�C�x���g�̃n���h��
 */
class MouseEventHandler {
public:
	/**
	 * �N�\����d�l�̂Ɂc
	 */
	virtual ~MouseEventHandler() {}

	/**
	 * �}�E�X�C�x���g�������Ƃ��ɃR�[���o�b�N����郁�\�b�h.
	 * 
	 * @param message ���b�Z�[�W
	 * @param hookStruct ���Ă̒ʂ�B
	 * @return ���b�Z�[�W��j������ꍇ��false�A���̃`�F�C���ɉ񂷏ꍇ��true�B
	 */
	virtual bool onMouseEvent(UINT message, MSLLHOOKSTRUCT *hookStruct) = 0;
};

/**
 * �}�E�X�C�x���g���t�b�N����̂����
 */
class MouseHooker {
private:
	static HHOOK hookHandle;
	static MouseHooker *instance;
	MouseEventHandler *handler;

public:
	/**
	 * �t�b�N�J�n�I�撣��I
	 * 
	 * ���s������domain_error�Ƃ�������
	 * 
	 * @param handler Mr.Handler
	 */
	MouseHooker(MouseEventHandler *handler) : handler(handler) {
		if (hookHandle != NULL) {
			// aah!�@�p��ς���
			throw std::domain_error("a hook already have got set.");
		}

		// �C���X�^���X��static�ȂƂ���ɒu���Ƃ��B�����ĂȂ�����
		instance = this;

		// �t�b�N�J�n
		hookHandle = ::SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)&MouseHooker::rawHookProc, ::AfxGetInstanceHandle(), 0);
		if (!hookHandle) {
			// aah!
			throw std::domain_error("failed to set a hook.");
		}
	}

	/**
	 * �f�X�g���N�^�B
	 * Resource Acquisition Is Initializing���Ƃ��Ȃ񂾂Ƃ�
	 */
	~MouseHooker() {
		if (hookHandle) {
			::UnhookWindowsHookEx(hookHandle);
		}
	}

	/**
	 * �t�b�N�v���V�[�W��.
	 * 
	 * �����Ƀt�b�N�n���h�������Ȃ��̂͒v���I�Ȑ݌v�~�X��
	 *
	 * @param code �悭�킩���R�[�h
	 * @param wparam WPARAM
	 * @param lparam LPARAM
	 * @return ������TRUE��Ԃ��ƃC�x���g��������Ƃ�����d�l�̂͂�
	 */
	static LRESULT CALLBACK rawHookProc(int code, WPARAM wparam, LPARAM lparam) {
		if (code < 0) {
			return ::CallNextHookEx(hookHandle, code, wparam, lparam);
		}

		if (instance->handler->onMouseEvent((UINT)wparam, (MSLLHOOKSTRUCT *)lparam)) {
			return ::CallNextHookEx(hookHandle, code, wparam, lparam);
		} else {
			// false���Ԃ��Ă�����A���b�Z�[�W������Ԃ�
			return TRUE;
		}
	}
};

// �Y��Ȃ�������static�ȃ����o���`���Ă���
// �ςȈʒu������
HHOOK MouseHooker::hookHandle = NULL;
MouseHooker *MouseHooker::instance = NULL;

/**
 * �ǂ��̃E�B���h�E�Ƀ��b�Z�[�W�𑗂邩�l����l
 */
class TargetWindowFinder {
public:
	TargetWindowFinder() {}
	~TargetWindowFinder() {}

	/**
	 * �ǂ��̃E�B���h�E�Ƀ��b�Z�[�W�𑗂�ׂ����T���B�J�[�\�����w���Ă�ʒu��B
	 */
	HWND find(POINT pt) {
		// �ǂ��ł������ł����AWindows�̃J�[�\���ʒu��LONG�Ȃ̂ŁA2���s�N�Z���z�����ӎ����Ă���悤�ł��B
		// �������Q�B�I
		HWND wnd = ::WindowFromPoint(pt);
		
		// TODO: ChildWindowFromPoint�Ƃ��v��̂��ǂ����悭�킩��Ȃ�
		// �v��Ȃ�������API��Ăяo���Ă邾���̃N�\�N���X�ɂȂ��Ă��܂��c

		return wnd;
	}
};

/**
 * �}�E�X�C�x���g������
 */
class MouseEventGenerator {
public:
	MouseEventGenerator() {}
	~MouseEventGenerator() {}

	/**
	 * �z�C�[���C�x���g�𔭐�������B
	 * 
	 * @param target ���Đ�
	 * @param pt �}�E�X�J�[�\���ʒu
	 * @param delta �ł邽�I�f���^�Ƃ����̂̓z�C�[���̈ړ��ʁH�̂悤�Ȃ���
	 */
	void raiseWheelEvent(HWND target, POINT pt, short delta) {
		WORD keys = 0;
		::PostMessage(target, WM_MOUSEWHEEL, MAKEWPARAM(keys, delta), MAKELPARAM(pt.x, pt.y));
	}
};

/**
 * ���C���̂�̌ӎU�L������
 */
class MacMouseScroller::impl : public MouseEventHandler {
private:
	std::auto_ptr<MouseHooker> hooker;
	TargetWindowFinder finder;
	MouseEventGenerator generator;

public:
	/**
	 * ��邱�Ɩ����R���X�g���N�^
	 */
	impl() {}

	/**
	 * ��邱�Ɩ����f�X�g���N�^
	 */
	virtual ~impl() {}

	/**
	 * �J�n
	 */
	void start() {
		this->hooker.reset(new MouseHooker(this));
	}

	/**
	 * �I��
	 */
	void end() {
		this->hooker.reset();
	}

	/**
	 * �}�E�X�C�x���g�������I
	 */
	virtual bool onMouseEvent(UINT message, MSLLHOOKSTRUCT *hookStruct) {
		if (message != WM_MOUSEWHEEL) {
			// �z�C�[���C�x���g�ȊO�͋�������
			return true;
		}

		HWND target = this->finder.find(hookStruct->pt);
		if (!target) {
			// NULL��Window�n���h�����ăf�X�N�g�b�v�������悤�ȁc
		}

		// �f�o�b�O
		TRACE("x: %d, y: %d, delta: %d\n", hookStruct->pt.x, hookStruct->pt.y, (short)HIWORD(hookStruct->mouseData));

		// �C�x���g�𔭐��������OK
		generator.raiseWheelEvent(target, hookStruct->pt, (short)HIWORD(hookStruct->mouseData));

		// ��͓K���ɔj�����Ă����Ƃ��肪����
		return false;
	}
};

// ------------------------------------------
// PImpl�I�Ȏ���
// ------------------------------------------
MacMouseScroller::MacMouseScroller() : pimpl(new impl()) {
}

MacMouseScroller::~MacMouseScroller() {
}

void MacMouseScroller::start() {
	this->pimpl->start();
}

void MacMouseScroller::end() {
	this->pimpl->end();
}

