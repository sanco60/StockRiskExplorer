
// StockRiskExplorer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CStockRiskExplorerApp:
// �йش����ʵ�֣������ StockRiskExplorer.cpp
//

class CStockRiskExplorerApp : public CWinApp
{
public:
	CStockRiskExplorerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

private:


};

extern CStockRiskExplorerApp theApp;