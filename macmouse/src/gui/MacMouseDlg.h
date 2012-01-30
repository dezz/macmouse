// MacMouseDlg.h : ヘッダー ファイル
//

#pragma once

#include "core/scroller.h"
#include "tasktray.h"

// MacMouseDlg ダイアログ
class MacMouseDlg : public CDialog
{
// コンストラクション
public:
	MacMouseDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_MACMOUSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	MacMouseScroller scroller;
	std::auto_ptr<TaskTrayIconRegistry> taskTrayIcon;
	static const unsigned int MSG_TASKTRAY_CALLBACK = WM_USER + 100;

	void onTaskTrayMessage(WPARAM wparam, LPARAM lparam);
	void show();
	void hide();
	bool isHidden();

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void onCommandShow();
	void onCommandExit();
};
