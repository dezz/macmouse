
#include "StdAfx.h"
#include "tasktray.h"

class TaskTrayIconRegistry::impl {
private:
	NOTIFYICONDATA nid;

public:
	impl(HICON icon, LPCTSTR text, HWND owner, UINT message) {
		NOTIFYICONDATA &nid = this->nid;
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = owner;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = message;
		nid.hIcon = icon;
		_tcscpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(TCHAR), text);
		
		if (!::Shell_NotifyIcon(NIM_ADD, &nid)) {
			// failed
			throw std::domain_error("failed to register icon on task-tray");
		}
	}

	~impl() {
		// unregister
		::Shell_NotifyIcon(NIM_DELETE, &this->nid);
	}
};

TaskTrayIconRegistry::TaskTrayIconRegistry(HICON icon, LPCTSTR text, HWND owner, UINT message)
		: pimpl(new impl(icon, text, owner, message)) {
}

TaskTrayIconRegistry::~TaskTrayIconRegistry() {
}
