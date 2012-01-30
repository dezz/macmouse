// MacMouseDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once

#include "core/scroller.h"
#include "tasktray.h"

// MacMouseDlg �_�C�A���O
class MacMouseDlg : public CDialog
{
// �R���X�g���N�V����
public:
	MacMouseDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_MACMOUSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	MacMouseScroller scroller;
	std::auto_ptr<TaskTrayIconRegistry> taskTrayIcon;
	static const unsigned int MSG_TASKTRAY_CALLBACK = WM_USER + 100;

	void onTaskTrayMessage(WPARAM wparam, LPARAM lparam);
	void show();
	void hide();
	bool isHidden();

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
