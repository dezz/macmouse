
#include "StdAfx.h"
#include <stdexcept>
#include "scroller.h"

/**
 * マウスイベントのハンドラ
 */
class MouseEventHandler {
public:
	/**
	 * クソ言語仕様故に…
	 */
	virtual ~MouseEventHandler() {}

	/**
	 * マウスイベントが来たときにコールバックされるメソッド.
	 * 
	 * @param message メッセージ
	 * @param hookStruct 見ての通り。
	 * @return メッセージを破棄する場合はfalse、次のチェインに回す場合はtrue。
	 */
	virtual bool onMouseEvent(UINT message, MSLLHOOKSTRUCT *hookStruct) = 0;
};

/**
 * マウスイベントをフックする偉いやつ
 */
class MouseHooker {
private:
	static HHOOK hookHandle;
	static MouseHooker *instance;
	MouseEventHandler *handler;

public:
	/**
	 * フック開始！頑張れ！
	 * 
	 * 失敗したらdomain_errorとか投げる
	 * 
	 * @param handler Mr.Handler
	 */
	MouseHooker(MouseEventHandler *handler) : handler(handler) {
		if (hookHandle != NULL) {
			// aah!　英語変かも
			throw std::domain_error("a hook already have got set.");
		}

		// インスタンスをstaticなところに置いとく。いけてない実装
		instance = this;

		// フック開始
		hookHandle = ::SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)&MouseHooker::rawHookProc, ::AfxGetInstanceHandle(), 0);
		if (!hookHandle) {
			// aah!
			throw std::domain_error("failed to set a hook.");
		}
	}

	/**
	 * デストラクタ。
	 * Resource Acquisition Is Initializingだとかなんだとか
	 */
	~MouseHooker() {
		if (hookHandle) {
			::UnhookWindowsHookEx(hookHandle);
		}
	}

	/**
	 * フックプロシージャ.
	 * 
	 * ここにフックハンドルが来ないのは致命的な設計ミスｗ
	 *
	 * @param code よくわからんコード
	 * @param wparam WPARAM
	 * @param lparam LPARAM
	 * @return ここでTRUEを返すとイベントが消えるという謎仕様のはず
	 */
	static LRESULT CALLBACK rawHookProc(int code, WPARAM wparam, LPARAM lparam) {
		if (code < 0) {
			return ::CallNextHookEx(hookHandle, code, wparam, lparam);
		}

		if (instance->handler->onMouseEvent((UINT)wparam, (MSLLHOOKSTRUCT *)lparam)) {
			return ::CallNextHookEx(hookHandle, code, wparam, lparam);
		} else {
			// falseが返ってきたら、メッセージを握りつぶす
			return TRUE;
		}
	}
};

// 忘れないうちにstaticなメンバを定義しておく
// 変な位置だけど
HHOOK MouseHooker::hookHandle = NULL;
MouseHooker *MouseHooker::instance = NULL;

/**
 * どこのウィンドウにメッセージを送るか考える人
 */
class TargetWindowFinder {
public:
	TargetWindowFinder() {}
	~TargetWindowFinder() {}

	/**
	 * どこのウィンドウにメッセージを送るべきか探す。カーソルが指してる位置基準。
	 */
	HWND find(POINT pt) {
		// どうでもいいですが、Windowsのカーソル位置はLONGなので、2億ピクセル越えを意識しているようです。
		// さすがゲィつ！
		HWND wnd = ::WindowFromPoint(pt);
		
		// TODO: ChildWindowFromPointとか要るのかどうかよくわからない
		// 要らなかったらAPI一個呼び出してるだけのクソクラスになってしまう…

		return wnd;
	}
};

/**
 * マウスイベント発生器
 */
class MouseEventGenerator {
public:
	MouseEventGenerator() {}
	~MouseEventGenerator() {}

	/**
	 * ホイールイベントを発生させる。
	 * 
	 * @param target あて先
	 * @param pt マウスカーソル位置
	 * @param delta でるた！デルタというのはホイールの移動量？のようなもの
	 */
	void raiseWheelEvent(HWND target, POINT pt, short delta) {
		WORD keys = 0;
		::PostMessage(target, WM_MOUSEWHEEL, MAKEWPARAM(keys, delta), MAKELPARAM(pt.x, pt.y));
	}
};

/**
 * メインのやつの胡散臭い実装
 */
class MacMouseScroller::impl : public MouseEventHandler {
private:
	std::auto_ptr<MouseHooker> hooker;
	TargetWindowFinder finder;
	MouseEventGenerator generator;

public:
	/**
	 * やること無いコンストラクタ
	 */
	impl() {}

	/**
	 * やること無いデストラクタ
	 */
	virtual ~impl() {}

	/**
	 * 開始
	 */
	void start() {
		this->hooker.reset(new MouseHooker(this));
	}

	/**
	 * 終了
	 */
	void end() {
		this->hooker.reset();
	}

	/**
	 * マウスイベントが来た！
	 */
	virtual bool onMouseEvent(UINT message, MSLLHOOKSTRUCT *hookStruct) {
		if (message != WM_MOUSEWHEEL) {
			// ホイールイベント以外は興味無い
			return true;
		}

		HWND target = this->finder.find(hookStruct->pt);
		if (!target) {
			// NULLのWindowハンドルってデスクトップだったような…
		}

		// デバッグ
		TRACE("x: %d, y: %d, delta: %d\n", hookStruct->pt.x, hookStruct->pt.y, (short)HIWORD(hookStruct->mouseData));

		// イベントを発生させればOK
		generator.raiseWheelEvent(target, hookStruct->pt, (short)HIWORD(hookStruct->mouseData));

		// 後は適当に破棄してくれるとありがたい
		return false;
	}
};

// ------------------------------------------
// PImpl的な実装
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

