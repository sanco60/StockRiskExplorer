// IEAdapterMFC.h : IEAdapterMFC DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include <vector>
#include <MsHTML.h>
#include <string>

#ifdef IEADAPTERMFC_EXPORTS
#define IEADAPTERMFC_PUBLIC __declspec(dllexport)
#else
#define IEADAPTERMFC_PUBLIC __declspec(dllimport)
#endif


// CIEAdapterMFCApp
// �йش���ʵ�ֵ���Ϣ������� IEAdapterMFC.cpp
//

class CIEAdapterMFCApp : public CWinApp
{
public:
	CIEAdapterMFCApp();

	~CIEAdapterMFCApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

};


class IEADAPTERMFC_PUBLIC CIEAdapterMFC
{
public:
	CIEAdapterMFC(CComQIPtr<IWebBrowser2> spWebBrowser2);

	CIEAdapterMFC();

	~CIEAdapterMFC();

	bool navigate(std::string szUrl);

	void registerEvents(IDispatch *pDisp);

	void unregisterEvents(IDispatch *pDisp);

	void refresh();

	bool searchItems(std::string szTag, std::vector<std::string> attrs, std::vector<CComQIPtr<IHTMLElement>> &elemVec);

private:
	
	CComQIPtr<IWebBrowser2> m_pWebBrowser2;

	DWORD m_dwCookie;

};

