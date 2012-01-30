// MacMouseDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "macmouse.h"
#include "MacMouseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
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


// MacMouseDlg ダイアログ




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


// MacMouseDlg メッセージ ハンドラ

BOOL MacMouseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
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

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	// タスクトレイにアイコンを追加
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

	// scroller動かしてみる
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


	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void MacMouseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR MacMouseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL MacMouseDlg::DestroyWindow()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	this->scroller.end();

	return CDialog::DestroyWindow();
}

LRESULT MacMouseDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

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

	// TODO: ここにメッセージ ハンドラ コードを追加します。
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