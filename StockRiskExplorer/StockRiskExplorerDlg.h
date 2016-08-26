#pragma once
// StockRiskExplorerDlg.h : 头文件
//

#include "StoryDirector.h"
#include "afxwin.h"


// CStockRiskExplorerDlg 对话框
class CStockRiskExplorerDlg : public CDialogEx
{
// 构造
public:
	CStockRiskExplorerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_STOCKRISKEXPLORER_DIALOG };

	void showMsg(LPCTSTR lpszNewText);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:

	CStoryDirector m_StoryDirector;

public:
	afx_msg void OnBnClickedButtonGowebsite();
	afx_msg void OnBnClickedButtonResult();
	CButton m_ShanghaiRadio;
	CButton m_ShenzhenRadio;
	CEdit m_outmsgEdit;
	afx_msg void OnBnClickedButtonAnalyse();
	CListBox m_ListKeyword;
	afx_msg void OnBnClickedButtonCleantemp();
	afx_msg void OnBnClickedButtonCleanwork();
	afx_msg void OnBnClickedButtonClnmsg();
	afx_msg void OnBnClickedButtonConvt();
};


