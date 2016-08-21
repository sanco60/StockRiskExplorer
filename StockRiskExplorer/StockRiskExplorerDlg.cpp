
// StockRiskExplorerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "StockRiskExplorer.h"
#include "StockRiskExplorerDlg.h"
#include "afxdialogex.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStockRiskExplorerDlg 对话框




CStockRiskExplorerDlg::CStockRiskExplorerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStockRiskExplorerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CStockRiskExplorerDlg::showMsg(LPCTSTR lpszNewText)
{
	m_outmsgEdit.SetSel(-1);
	m_outmsgEdit.ReplaceSel(lpszNewText);
}


void CStockRiskExplorerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_SH, m_ShanghaiRadio);
	DDX_Control(pDX, IDC_RADIO_SZ, m_ShenzhenRadio);
	DDX_Control(pDX, IDC_EDIT_MSG, m_outmsgEdit);
}


BEGIN_MESSAGE_MAP(CStockRiskExplorerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GOWEBSITE, &CStockRiskExplorerDlg::OnBnClickedButtonGowebsite)
	ON_BN_CLICKED(IDC_BUTTON_RESULT, &CStockRiskExplorerDlg::OnBnClickedButtonResult)
END_MESSAGE_MAP()


// CStockRiskExplorerDlg 消息处理程序

BOOL CStockRiskExplorerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	if (!m_StoryDirector.init())
		return FALSE;

	m_ShanghaiRadio.SetCheck(TRUE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStockRiskExplorerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CStockRiskExplorerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CStockRiskExplorerDlg::OnBnClickedButtonGowebsite()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_StoryDirector.m_IEAdapterMFC)
		return;

	std::string szGotoUrl;
	int iSHChecked = m_ShanghaiRadio.GetCheck();
	if (1 == iSHChecked)
	{
		szGotoUrl = "http://www.sse.com.cn/disclosure/listedinfo/announcement/";		
	} else
	{
		szGotoUrl = "http://disclosure.szse.cn/m/drgg.htm";
	}
	m_StoryDirector.m_IEAdapterMFC->navigate(szGotoUrl);
	m_StoryDirector.m_IEAdapterMFC->waitCompleted();
}


void CStockRiskExplorerDlg::OnBnClickedButtonResult()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_StoryDirector.m_IEAdapterMFC)
		return;

	int iSHChecked = m_ShanghaiRadio.GetCheck();

	if (1 == iSHChecked)
	{
		m_StoryDirector.processShangHai();
	} else
	{
		m_StoryDirector.processShenZhen();
	}	
}

