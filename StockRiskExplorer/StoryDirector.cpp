// StoryDirector.cpp : 实现文件
//

#include "stdafx.h"
#include "StockRiskExplorer.h"
#include "StoryDirector.h"


// CStoryDirector

IMPLEMENT_DYNAMIC(CStoryDirector, CWnd)

CStoryDirector::CStoryDirector()
{
	m_bInitialized = false;
}

CStoryDirector::~CStoryDirector()
{
	uninit();
}


BEGIN_MESSAGE_MAP(CStoryDirector, CWnd)
END_MESSAGE_MAP()



// CStoryDirector 消息处理程序


bool CStoryDirector::init()
{
	if (m_bInitialized)
		return false;

	::ShellExecute(NULL, L"open", L"iexplore.exe", NULL, NULL, SW_SHOW);
	if (!getTopIE())
		return false;

	m_bInitialized = true;
	return true;
}


bool CStoryDirector::uninit()
{
	if (!m_bInitialized)
		return false;

	m_bInitialized = false;
	return true;
}


bool CStoryDirector::getTopIE()
{
	if (m_pWebBrowser2)
		return false;

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
						m_pWebBrowser2 = *arShellWindows[i];
						//AdviseSinkIE();
						//NavigateToSamplePage(bIE);
						bFound=TRUE;
						//m_bOwnIE=FALSE;
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
	return true;
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

