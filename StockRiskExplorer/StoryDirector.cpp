// StoryDirector.cpp : ʵ���ļ�
//

#include "stdafx.h"

#include <direct.h>
//#include <sstream>

#include "StockRiskExplorer.h"
#include "StoryDirector.h"
#include "DownloadEvent.h"


std::string s_TempDir(".\\StockRiskExplorerTemp\\");
std::string s_WorkDir(".\\StockRiskExplorerWork\\");
// CStoryDirector

IMPLEMENT_DYNAMIC(CStoryDirector, CWnd)

CStoryDirector::CStoryDirector()
{
	m_bInitialized = false;
	//m_pageNum = 1;
}

CStoryDirector::~CStoryDirector()
{
	uninit();
}


BEGIN_MESSAGE_MAP(CStoryDirector, CWnd)
END_MESSAGE_MAP()



// CStoryDirector ��Ϣ�������


bool CStoryDirector::init()
{
	if (m_bInitialized)
		return false;

	::CoInitialize(NULL);

	//�½�Ŀ¼
	::_mkdir(s_TempDir.c_str());
	::_mkdir(s_WorkDir.c_str());

	//���temp��workĿ¼
	SHELLEXECUTEINFO delExeInfo;
	memset(&delExeInfo, 0, sizeof(delExeInfo));

	delExeInfo.cbSize = sizeof(delExeInfo);
	delExeInfo.hwnd = NULL;
	delExeInfo.lpVerb = _T("open");
	delExeInfo.lpFile = _T("cmd.exe");
	delExeInfo.lpParameters = _T("/c del /q .\\StockRiskExplorerTemp .\\StockRiskExplorerWork");
	delExeInfo.nShow = SW_SHOWNORMAL;
	delExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&delExeInfo);
	::WaitForSingleObject(delExeInfo.hProcess, INFINITE);


	//��IE�����
	SHELLEXECUTEINFO ieExeInfo;
	memset(&ieExeInfo, 0, sizeof(ieExeInfo));

	ieExeInfo.cbSize = sizeof(ieExeInfo);
	ieExeInfo.hwnd = NULL;
	ieExeInfo.lpVerb = _T("open");
	ieExeInfo.lpFile = _T("C:\\Program Files\\Internet Explorer\\iexplore.exe");
	ieExeInfo.nShow = SW_SHOWNORMAL;
	ieExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&ieExeInfo);
	//����ȴ���ҳ�򿪺�ſ��Ի�ȡ��
	::WaitForSingleObject(ieExeInfo.hProcess, 3000);

	CComQIPtr<IWebBrowser2> spWebBrowser2 = getTopIE();
	if (!spWebBrowser2)
		return false;

	m_IEAdapterMFC = boost::shared_ptr<CIEAdapterMFC>(new CIEAdapterMFC(spWebBrowser2));

	m_bInitialized = true;
	return true;
}


bool CStoryDirector::uninit()
{
	if (!m_bInitialized)
		return false;

	//������CoUninitialize()����֮ǰ�ͷ�COM��Դ
	m_IEAdapterMFC->close();
	m_IEAdapterMFC = NULL;

	::CoUninitialize();
	m_bInitialized = false;
	return true;
}


void CStoryDirector::getFileName(CComBSTR &url, CComBSTR &fname)
{
	const TCHAR tSlash(_T('/'));

	if (!url.m_str || 0 == url.Length())
		return;

	BSTR pCurPos = url.m_str + url.Length() - 1;

	bool bFound = false;
	for (; pCurPos >= url.m_str; pCurPos--)
	{		
		if (*pCurPos == tSlash)
		{
			bFound = true;
			break;
		}
	}

	fname.Empty();
	if (bFound)
	{
		fname.Append(pCurPos + 1);
	} else
	{
		fname = url;
	}
	return;
}


CComQIPtr<IWebBrowser2> CStoryDirector::getTopIE()
{
	CComQIPtr<IWebBrowser2> rtWebBrowser2;

	CComPtr<IShellWindows> psw;
	psw.CoCreateInstance(CLSID_ShellWindows);
	if(psw)
	{
		//array to storage IE window handles and interfaces
		CDWordArray	arHWNDShellWindows;
		arHWNDShellWindows.SetSize(0,10);	//grow by 10 
		CTypedPtrArray<CPtrArray, CComQIPtr<IWebBrowser2>*> arShellWindows;

		arShellWindows.SetSize(0,10);//grow by 10 
		//enumerate the ShellWindow collection 
		long lShellWindowCount=0;
		psw->get_Count(&lShellWindowCount);
		for(long i=0;i<lShellWindowCount;i++)
		{
			CComPtr<IDispatch> pdispShellWindow;
			psw->Item(COleVariant(i),&pdispShellWindow);
			CComQIPtr<IWebBrowser2> pIE(pdispShellWindow);
			if(pIE)//is it a Shell window?
			{
				//is it the right type?
				CString strWindowClass = GetWindowClassName(pIE);
				if(strWindowClass == _T("IEFrame"))
				{
					HWND hWndID=NULL;
					pIE->get_HWND((long*)&hWndID);
					//store its information
					arHWNDShellWindows.Add((DWORD)hWndID);
					arShellWindows.Add(new CComQIPtr<IWebBrowser2>(pIE));
				}
			}	
		}
		if(arHWNDShellWindows.GetSize()>0)//at least one shell window found
		{
			BOOL bFound=FALSE;
			//the first top-level window in zorder
			HWND hwndTest = ::GetWindow((HWND)arHWNDShellWindows[0],GW_HWNDFIRST);
			while( hwndTest&& !bFound)
			{
				for(int i = 0; i < arHWNDShellWindows.GetSize(); i++)
				{
					if(hwndTest == (HWND)arHWNDShellWindows[i])
					{
						//got it, attach to it
						rtWebBrowser2 = *arShellWindows[i];
						//AdviseSinkIE();
						
						bFound=TRUE;
						break;
					}
				}
				hwndTest = ::GetWindow(hwndTest, GW_HWNDNEXT);
			}
		}
		//cleanup
		for(int i=0;i<arShellWindows.GetSize();i++)
		{
			delete arShellWindows[i];
		}
	}
	return rtWebBrowser2;
}


CString	CStoryDirector::GetWindowClassName(IWebBrowser2* pwb)
{
	TCHAR szClassName[_MAX_PATH];
	ZeroMemory(szClassName,_MAX_PATH*sizeof(TCHAR));
	HWND hwnd=NULL;
	if(pwb)
	{
		LONG_PTR lwnd=NULL;
		pwb->get_HWND(&lwnd);
		hwnd=reinterpret_cast<HWND>(lwnd);
		::GetClassName(hwnd,szClassName,_MAX_PATH);
	}
	return szClassName;
}


void CStoryDirector::processShangHai()
{
	if (!m_IEAdapterMFC)
		return;

	UINT pageNum = 1;	
	TCHAR tcharMsg[1024] = {0};

	std::string szTagA("a");
	std::vector<std::string> attrsPdfUrl;
	std::vector<std::string> attrsNextPage;
	std::vector<CComQIPtr<IHTMLElement>> vecPdfUrl;
	std::vector<CComQIPtr<IHTMLElement>> vecNextPage;

	attrsPdfUrl.push_back(std::string("href"));
	attrsPdfUrl.push_back(std::string("title"));
	attrsPdfUrl.push_back(std::string("target"));
	
	attrsNextPage.push_back(std::string("id"));
	attrsNextPage.push_back(std::string("classname"));
	attrsNextPage.push_back(std::string("page"));
	attrsNextPage.push_back(std::string("name"));
	attrsNextPage.push_back(std::string("href"));

	while(true)
	{
		vecPdfUrl.clear();
		vecNextPage.clear();

		memset(tcharMsg, 0, sizeof(tcharMsg));
		_stprintf_s(tcharMsg, _T("�� %d ҳ\r\n"), pageNum++);
		MSG_OUTPUT(tcharMsg);

		m_IEAdapterMFC->queryItems(szTagA, attrsPdfUrl, vecPdfUrl);

		//memset(tcharMsg, 0, sizeof(tcharMsg));
		//_stprintf_s(tcharMsg, _T("һ�������� %d ��\r\n"), vecPdfUrl.size());
		//MSG_OUTPUT(tcharMsg);

		std::vector<CComQIPtr<IHTMLElement>>::iterator itor = vecPdfUrl.begin();
		for (; itor != vecPdfUrl.end(); itor++)
		{
			BSTR itStr = NULL;
			(*itor)->get_innerText(&itStr);

			UINT uLength = ::SysStringLen(itStr);
			if (6 > uLength)
			{
				continue;
			}
		
			CComVariant cvVal;
			(*itor)->getAttribute(CComBSTR("href"), 0, &cvVal);
			CComBSTR bstrUrl(cvVal.bstrVal);

			memset(tcharMsg, 0, sizeof(tcharMsg));
			_stprintf_s(tcharMsg, _T("����url %s \r\n"), bstrUrl);
			MSG_OUTPUT(tcharMsg);
		
			processPdfUrl(bstrUrl);
		}

		//�����һҳ
		m_IEAdapterMFC->queryItems(szTagA, attrsNextPage, vecNextPage);
		if (0 == vecNextPage.size())
			break;

		bool bNextPage = false;

		std::vector<CComQIPtr<IHTMLElement>>::iterator nextItor = vecNextPage.begin();
		for (; nextItor != vecNextPage.end(); nextItor++)
		{
			CComVariant cvVal;
			(*nextItor)->getAttribute(CComBSTR("classname"), 0, &cvVal);
			CComBSTR bstrCP(cvVal.bstrVal);
			if (0 == _tcscmp(bstrCP, _T("classPage")))
			{
				CComBSTR bstrHtml;
				(*nextItor)->get_outerHTML(&bstrHtml);

				if (NULL == _tcsstr(bstrHtml, _T("right")))
					continue;

				bNextPage = true;
				break;
			}
		}
		if (!bNextPage)
			break;

		(*nextItor)->click();
		m_IEAdapterMFC->waitCompleted();
	}

	memset(tcharMsg, 0, sizeof(tcharMsg));
	_stprintf_s(tcharMsg, _T("���� ת�� ���\r\n"));
	MSG_OUTPUT(tcharMsg);
	return;
}


void CStoryDirector::processShenZhen()
{
	if (!m_IEAdapterMFC)
		return;


}


void CStoryDirector::processPdfUrl(CComBSTR &url)
{
	CDownloadEvent dwnEvent;
	CComBSTR tFileName(1024);
	CComBSTR tTempPathName(s_TempDir.c_str());

	getFileName(url, tFileName);
	tTempPathName.Append(tFileName);

	CComBSTR tTempTextPathName(tTempPathName);

	tTempTextPathName.Append(_T(".txt"));

	::URLDownloadToFile(NULL, url, tTempPathName, 1024, (LPBINDSTATUSCALLBACK)&dwnEvent);
	//�ȴ��������
	dwnEvent.waitForEnd();

	//ת��pdf�ĵ���Ϊtext�ĵ�
	SHELLEXECUTEINFO exeInfo;
	memset(&exeInfo, 0, sizeof(exeInfo));

	CComBSTR tParm(tTempPathName);
	tParm.Append(_T(" "));
	tParm.Append(tTempTextPathName);

	exeInfo.cbSize = sizeof(exeInfo);
	exeInfo.hwnd = NULL;
	exeInfo.lpVerb = _T("open");
	exeInfo.lpFile = _T("E:\\ThirdLibs\\xpdfbin-win-3.04\\bin64\\pdftotext.exe");
	exeInfo.lpParameters = tParm;
	exeInfo.nShow = SW_SHOWNORMAL;
	exeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&exeInfo);
	//�ȴ�ת������
	::WaitForSingleObject(exeInfo.hProcess, INFINITE);
}


