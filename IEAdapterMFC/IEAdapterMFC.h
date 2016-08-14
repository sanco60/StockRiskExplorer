// IEAdapterMFC.h : IEAdapterMFC DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include <vector>
#include <MsHTML.h>
#include <string>

#ifdef IEADAPTERMFC_EXPORTS
#define IEADAPTERMFC_PUBLIC __declspec(dllexport)
#else
#define IEADAPTERMFC_PUBLIC __declspec(dllimport)
#endif


// CIEAdapterMFCApp
// 有关此类实现的信息，请参阅 IEAdapterMFC.cpp
//

class CIEAdapterMFCApp : public CWinApp
{
public:
	CIEAdapterMFCApp();

	~CIEAdapterMFCApp();

// 重写
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

