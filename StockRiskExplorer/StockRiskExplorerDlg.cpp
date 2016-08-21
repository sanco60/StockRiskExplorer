
// StockRiskExplorerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StockRiskExplorer.h"
#include "StockRiskExplorerDlg.h"
#include "afxdialogex.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStockRiskExplorerDlg �Ի���




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


// CStockRiskExplorerDlg ��Ϣ�������

BOOL CStockRiskExplorerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	if (!m_StoryDirector.init())
		return FALSE;

	m_ShanghaiRadio.SetCheck(TRUE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CStockRiskExplorerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CStockRiskExplorerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CStockRiskExplorerDlg::OnBnClickedButtonGowebsite()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

