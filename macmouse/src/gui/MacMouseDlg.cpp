// MacMouseDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "macmouse.h"
#include "MacMouseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// MacMouseDlg �_�C�A���O




MacMouseDlg::MacMouseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MacMouseDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void MacMouseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(MacMouseDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_COMMAND(ID_TT_SHOW, onCommandShow)
	ON_COMMAND(ID_TT_EXIT, onCommandExit)
END_MESSAGE_MAP()


// MacMouseDlg ���b�Z�[�W �n���h��

BOOL MacMouseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	// �^�X�N�g���C�ɃA�C�R����ǉ�
	CString title;
	this->GetWindowText(title);

	taskTrayIcon.reset(
		new TaskTrayIconRegistry(
			AfxGetApp()->LoadIcon(IDR_MAINFRAME),
			title,
			this->m_hWnd,
			MSG_TASKTRAY_CALLBACK
		)
	);

	// scroller�������Ă݂�
	try {
		scroller.start();

	} catch (const std::exception &ex) {
		// get error message
		std::string msg;
		LPSTR msgbuf;
		if (::FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				::GetLastError(),
				0,
				(LPSTR)&msgbuf,
				0,
				NULL)) {
			msg = msgbuf;
			::LocalFree(msgbuf);
		} else {
			// i can't retrieve even an error message :-(
			msgbuf = NULL;
			msg = "Cannot retrieve erorr message from Windows.";
		}

		// 
		std::stringstream ss;
		ss << "un error occurred... " << std::endl;
		ss << std::endl;
		ss << msg << std::endl;
		ss << std::endl;
		ss << ex.what() << std::endl;

		::MessageBoxA(this->m_hWnd, ss.str().c_str(), "error", MB_OK | MB_ICONEXCLAMATION);
	}


	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void MacMouseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void MacMouseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR MacMouseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL MacMouseDlg::DestroyWindow()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	this->scroller.end();

	return CDialog::DestroyWindow();
}

LRESULT MacMouseDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B

	switch (message) {
	case MSG_TASKTRAY_CALLBACK:
		// message caused by task-tray
		onTaskTrayMessage(wParam, lParam);
		break;

	default:
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void MacMouseDlg::onTaskTrayMessage(WPARAM wparam, LPARAM lparam) {
	switch (lparam) {
		case WM_RBUTTONUP :{
			// show popup menu
			CPoint pos;
			GetCursorPos(&pos);
			
			CMenu menu;
			menu.LoadMenu(IDR_MENU_TASKTRAY);
			CMenu *subMenu = menu.GetSubMenu(0);

			subMenu->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_RIGHTALIGN, pos.x, pos.y, this);
			break;
		}

		case WM_LBUTTONDBLCLK:
			// show window
			show();
			break;

		default:
			break;
	}
}

void MacMouseDlg::show() {
	if (isHidden()) {
		ShowWindow(SW_SHOW);
		ShowWindow(SW_RESTORE);
	}
}

bool MacMouseDlg::isHidden() {
	DWORD style = GetWindowLong(this->m_hWnd, GWL_STYLE);
	return (style & WS_VISIBLE) ? false : true;
}

void MacMouseDlg::hide() {
	if (!isHidden()) {
		ShowWindow(SW_HIDE);
	}
}

void MacMouseDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B
	if (nType == SIZE_MINIMIZED) {
		hide();
	}
}

void MacMouseDlg::onCommandShow() {
	show();
}

void MacMouseDlg::onCommandExit() {
	::PostQuitMessage(0);
}