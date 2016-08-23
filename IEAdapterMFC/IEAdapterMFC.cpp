// IEAdapterMFC.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "IEAdapterMFC.h"

#include <Wininet.h>
#include <Afxctl.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CIEAdapterMFCApp

BEGIN_MESSAGE_MAP(CIEAdapterMFCApp, CWinApp)
END_MESSAGE_MAP()



// CIEAdapterMFCApp 构造

CIEAdapterMFCApp::CIEAdapterMFCApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


CIEAdapterMFCApp::~CIEAdapterMFCApp()
{
}


// 唯一的一个 CIEAdapterMFCApp 对象

CIEAdapterMFCApp theApp;


// CIEAdapterMFCApp 初始化

BOOL CIEAdapterMFCApp::InitInstance()
{
	CWinApp::InitInstance();	

	return TRUE;
}



/********************************************* class CIEAdapterMFCApp ***************************************************************************/


CIEAdapterMFC::CIEAdapterMFC(CComQIPtr<IWebBrowser2> spWebBrowser2)
{
	m_pWebBrowser2 = spWebBrowser2;
	m_dwCookie = 0;
}


CIEAdapterMFC::CIEAdapterMFC()
{
	m_dwCookie = 0;
}


CIEAdapterMFC::~CIEAdapterMFC()
{
	m_pWebBrowser2 = (LPUNKNOWN)NULL;
}


bool CIEAdapterMFC::navigate(std::string szUrl)
{
	if (!m_pWebBrowser2)
	{
		return false;
	}
	VARIANT vEmpty;
    VariantInit(&vEmpty);

	//COleVariant vaURL(url);
	/*m_pWebBrowser2->Navigate2(&vaURL, COleVariant((long) 0, VT_I4), COleVariant((LPCTSTR)NULL, VT_BSTR), 
		COleSafeArray(), COleVariant((LPCTSTR)NULL, VT_BSTR));*/
	
	CComBSTR bstrUrl(szUrl.c_str());

	m_pWebBrowser2->Navigate(bstrUrl, &vEmpty, &vEmpty, &vEmpty, &vEmpty);

	return true;
}


void CIEAdapterMFC::registerEvents(IDispatch *pDisp)
{
	if (NULL == pDisp || !m_pWebBrowser2 || 0 != m_dwCookie)
	{
		return;
	}
	LPUNKNOWN pUnkSink = pDisp;
	AfxConnectionAdvise((LPUNKNOWN)m_pWebBrowser2, DIID_DWebBrowserEvents2, 
		pUnkSink, FALSE, &m_dwCookie);
}


void CIEAdapterMFC::unregisterEvents(IDispatch *pDisp)
{
	if (NULL == pDisp || 0 == m_dwCookie || !m_pWebBrowser2)
	{
		return;
	}
	LPUNKNOWN pUnkSink = pDisp;
	AfxConnectionUnadvise((LPUNKNOWN)m_pWebBrowser2, DIID_DWebBrowserEvents2, pUnkSink, FALSE, m_dwCookie);
	m_dwCookie = 0;	
}


void CIEAdapterMFC::refresh()
{
	if (!m_pWebBrowser2)
	{
		return;
	}

	m_pWebBrowser2->Refresh();
}


bool CIEAdapterMFC::queryItems(std::string szTag, std::vector<std::string> &attrs, std::vector<CComQIPtr<IHTMLElement>> &elemVec)
{
	if (!m_pWebBrowser2)	
		return false;
	
	CComPtr<IDispatch> pDocDisp;
	m_pWebBrowser2->get_Document(&pDocDisp);
	CComQIPtr<IHTMLDocument2> pHTMLDoc(pDocDisp);

	CComQIPtr<IHTMLElementCollection> ecAll;
	if (!pHTMLDoc)	
		return false;
	
	pHTMLDoc->get_all(&ecAll);
	if (!ecAll)	
		return false;
	
	CComBSTR bstrTag(szTag.c_str());
	CComPtr<IDispatch> pTagDisp;
	ecAll->tags(CComVariant(bstrTag), &pTagDisp);
	CComQIPtr<IHTMLElementCollection> pTags(pTagDisp);

	long iFormCount = 0;
	pTags->get_length(&iFormCount);

	for (long iIndex = 0; iIndex < iFormCount; iIndex++)
	{
		CComPtr<IDispatch> pItemDisp;
		pTags->item(CComVariant(iIndex), CComVariant(0), &pItemDisp);

		CComQIPtr<IHTMLElement> eTagLine(pItemDisp);
		if (!eTagLine)
			continue;

		//是否所有属性都存在
		bool bAllExist = true;
		std::vector<std::string>::iterator aItor = attrs.begin();
		for (; aItor != attrs.end(); ++aItor)		
		{
			std::string tmpAttr = *aItor;
			CComBSTR bstrArrt(tmpAttr.c_str());
			CComVariant tmpVar;
			eTagLine->getAttribute(bstrArrt, 0, &tmpVar);

			if (0 == tmpVar.bstrVal)
			{
				bAllExist = false;
				break;
			}
			CComBSTR bstrVal(tmpVar.bstrVal);
			if (0 == bstrVal.Length())
			{
				bAllExist = false;
				break;
			}
		}
		
		if (bAllExist)
		{
			elemVec.push_back(eTagLine);
		}
	}

	return true;
}



bool CIEAdapterMFC::querySubItems(CComQIPtr<IHTMLElement> elem, std::string szTag, std::vector<std::string> &attrs, std::vector<CComQIPtr<IHTMLElement>> &elemVec)
{
	if (!elem)
		return false;

	CComPtr<IDispatch> pAllDisp;
	elem->get_all(&pAllDisp);

	CComQIPtr<IHTMLElementCollection> ecAll(pAllDisp);
	if (!ecAll)
		return false;

	CComBSTR bstrTag(szTag.c_str());
	CComPtr<IDispatch> pTagDisp;
	ecAll->tags(CComVariant(bstrTag), &pTagDisp);
	CComQIPtr<IHTMLElementCollection> pTags(pTagDisp);

	long iFormCount = 0;
	pTags->get_length(&iFormCount);

	for (long iIndex = 0; iIndex < iFormCount; iIndex++)
	{
		CComPtr<IDispatch> pItemDisp;
		pTags->item(CComVariant(iIndex), CComVariant(0), &pItemDisp);

		CComQIPtr<IHTMLElement> eTagLine(pItemDisp);
		if (!eTagLine)
			continue;

		//是否所有属性都存在
		bool bAllExist = true;
		std::vector<std::string>::iterator aItor = attrs.begin();
		for (; aItor != attrs.end(); ++aItor)		
		{
			std::string tmpAttr = *aItor;
			CComBSTR bstrArrt(tmpAttr.c_str());
			CComVariant tmpVar;
			eTagLine->getAttribute(bstrArrt, 0, &tmpVar);

			if (0 == tmpVar.bstrVal)
			{
				bAllExist = false;
				break;
			}
			CComBSTR bstrVal(tmpVar.bstrVal);
			if (0 == bstrVal.Length())
			{
				bAllExist = false;
				break;
			}
		}
		
		if (bAllExist)
		{
			elemVec.push_back(eTagLine);
		}
	}

	return true;
}



void CIEAdapterMFC::close()
{
	if (!m_pWebBrowser2)
		return;

	m_pWebBrowser2->Quit();
	m_pWebBrowser2 = (LPUNKNOWN)NULL;
	m_dwCookie = 0;
}


void CIEAdapterMFC::waitCompleted()
{
	if (!m_pWebBrowser2)
		return;

	while(true)
	{
		READYSTATE lReadyState = READYSTATE_UNINITIALIZED;

		::Sleep(1000);
		m_pWebBrowser2->get_ReadyState(&lReadyState);

		if (READYSTATE_COMPLETE == lReadyState)
		{
			break;
		}
	}

	return;
}