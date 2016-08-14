#pragma once
// StockRiskExplorerDlg.h : ͷ�ļ�
//

#include "StoryDirector.h"


// CStockRiskExplorerDlg �Ի���
class CStockRiskExplorerDlg : public CDialogEx
{
// ����
public:
	CStockRiskExplorerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_STOCKRISKEXPLORER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:

	CStoryDirector m_StoryDirector;

public:
	afx_msg void OnBnClickedButtonGowebsite();
	afx_msg void OnBnClickedButtonResult();
};
