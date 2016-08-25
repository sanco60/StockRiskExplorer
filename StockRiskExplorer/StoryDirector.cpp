// StoryDirector.cpp : 实现文件
//

#include "stdafx.h"

#include <direct.h>
#include <fstream>
#include <codecvt>

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

	::CoInitialize(NULL);

	//新建目录
	::_mkdir(s_TempDir.c_str());
	::_mkdir(s_WorkDir.c_str());

	//打开IE浏览器
	SHELLEXECUTEINFO ieExeInfo;
	memset(&ieExeInfo, 0, sizeof(ieExeInfo));

	ieExeInfo.cbSize = sizeof(ieExeInfo);
	ieExeInfo.hwnd = NULL;
	ieExeInfo.lpVerb = _T("open");
	ieExeInfo.lpFile = _T("iexplore.exe");
	ieExeInfo.nShow = SW_SHOWNORMAL;
	ieExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&ieExeInfo);
	//必须等待网页打开后才可以获取它
	::WaitForSingleObject(ieExeInfo.hProcess, 3000);

	CComQIPtr<IWebBrowser2> spWebBrowser2 = getTopIE();
	if (!spWebBrowser2)
		return false;

	m_IEAdapterMFC = boost::shared_ptr<CIEAdapterMFC>(new CIEAdapterMFC(spWebBrowser2));

	//遍历当前目录下所有关键字文件
	TCHAR szFind[MAX_PATH] = _T("KW*.txt");
	WIN32_FIND_DATA findFileData;

	CStockRiskExplorerDlg* pDlg = (CStockRiskExplorerDlg*)theApp.m_pMainWnd;
	HANDLE hFind = ::FindFirstFile(szFind, &findFileData);

    if (INVALID_HANDLE_VALUE != hFind)
    {		
		while(TRUE)
		{			
			pDlg->m_ListKeyword.AddString(findFileData.cFileName);

			BOOL bRetNext = FALSE;
			bRetNext = ::FindNextFile(hFind, &findFileData);
			if (!bRetNext)
				break;
		}
    }

	m_bInitialized = true;
	return true;
}


bool CStoryDirector::uninit()
{
	if (!m_bInitialized)
		return false;

	//必须在CoUninitialize()方法之前释放COM资源
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
		_stprintf_s(tcharMsg, _T("第 %d 页\r\n"), pageNum++);
		MSG_OUTPUT(tcharMsg);

		m_IEAdapterMFC->queryItems(szTagA, attrsPdfUrl, vecPdfUrl);

		//memset(tcharMsg, 0, sizeof(tcharMsg));
		//_stprintf_s(tcharMsg, _T("一共搜索到 %d 项\r\n"), vecPdfUrl.size());
		//MSG_OUTPUT(tcharMsg);

		std::vector<CComQIPtr<IHTMLElement>>::iterator itor = vecPdfUrl.begin();
		for (; itor != vecPdfUrl.end(); itor++)
		{
			CComVariant cvVal;
			(*itor)->getAttribute(CComBSTR("href"), 0, &cvVal);
			CComBSTR bstrUrl(cvVal.bstrVal);

			//判断该链接是否为PDF链接
			CString csUrl(bstrUrl);
			CString csPdfSuffix = csUrl.Right(4);
			csPdfSuffix.MakeLower();
			if (0 != csPdfSuffix.Compare(_T(".pdf")))
				continue;

			//同样的pdf链接有两个，过滤其中没有说明的一个
			BSTR itBstr = NULL;
			(*itor)->get_innerText(&itBstr);
			CString csInnerText(itBstr);			
			if (6 > csInnerText.GetLength())
				continue;

			memset(tcharMsg, 0, sizeof(tcharMsg));
			_stprintf_s(tcharMsg, _T("下载 %s \r\n"), bstrUrl);
			MSG_OUTPUT(tcharMsg);
		
			csInnerText.Remove(_T(':'));
			csInnerText.Remove(_T('*'));
			processPdfUrl(bstrUrl, csInnerText.Left(10));
		}

		//点击下一页
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
	_stprintf_s(tcharMsg, _T("下载 转换 完毕\r\n"));
	MSG_OUTPUT(tcharMsg);
	return;
}


void CStoryDirector::processShenZhen()
{
	if (!m_IEAdapterMFC)
		return;

	UINT pageNum = 1;
	TCHAR tcharMsg[1024] = {0};

	std::string szTagA("a");
	std::string szTagTD("td");
	std::string szTagSpan("span");
	std::vector<std::string> attrsPdfUrl;
	std::vector<std::string> attrsNextPage;
	std::vector<CComQIPtr<IHTMLElement>> vecPdfUrl;
	std::vector<CComQIPtr<IHTMLElement>> vecTd;
	std::vector<CComQIPtr<IHTMLElement>> vecSpan;
	std::vector<CComQIPtr<IHTMLElement>> vecNextPage;

	attrsPdfUrl.push_back(std::string("href"));
	attrsPdfUrl.push_back(std::string("target"));

	attrsNextPage.push_back(std::string("href"));

	while(true)
	{
		vecPdfUrl.clear();
		vecTd.clear();
		vecSpan.clear();
		vecNextPage.clear();

		memset(tcharMsg, 0, sizeof(tcharMsg));
		_stprintf_s(tcharMsg, _T("第 %d 页\r\n"), pageNum++);
		MSG_OUTPUT(tcharMsg);

		//查找当前页所有的pdf文档并下载
		m_IEAdapterMFC->queryItems(szTagA, attrsPdfUrl, vecPdfUrl);

		std::vector<CComQIPtr<IHTMLElement>>::iterator itor = vecPdfUrl.begin();
		for (; itor != vecPdfUrl.end(); itor++)
		{
			CComVariant cvVal;
			(*itor)->getAttribute(CComBSTR("href"), 0, &cvVal);
			CComBSTR bstrUrl(cvVal.bstrVal);

			//判断该链接是否为PDF链接
			CString csUrl(bstrUrl);
			CString csPdfSuffix = csUrl.Right(4);
			csPdfSuffix.MakeLower();
			if (0 != csPdfSuffix.Compare(_T(".pdf")))
				continue;

			BSTR itBstr = NULL;
			(*itor)->get_innerText(&itBstr);
			CString csInnerText(itBstr);			
			csInnerText.Remove(_T(':'));
			csInnerText.Remove(_T('*'));

			memset(tcharMsg, 0, sizeof(tcharMsg));
			_stprintf_s(tcharMsg, _T("下载 %s \r\n"), bstrUrl);
			MSG_OUTPUT(tcharMsg);

			processPdfUrl(bstrUrl, csInnerText.Left(4));
		}

		//先判断当前页是否为最后一页
		bool lastPage = true;
		m_IEAdapterMFC->queryItems(szTagTD, std::vector<std::string>(), vecTd);

		std::vector<CComQIPtr<IHTMLElement>>::iterator itTd = vecTd.begin();
		for (; itTd != vecTd.end(); itTd++)
		{
			CComBSTR bstrTD = NULL;
			(*itTd)->get_innerText(&bstrTD);
			if (NULL == bstrTD || bstrTD.m_str != _tcsstr(bstrTD, _T("当前第")))
				continue;

			m_IEAdapterMFC->querySubItems(*itTd, szTagSpan, std::vector<std::string>(), vecSpan);
			if (2 == vecSpan.size())
			{
				CComBSTR bstrCurPage = NULL;
				CComBSTR bstrTotalPage = NULL;
				vecSpan[0]->get_innerText(&bstrCurPage);
				vecSpan[1]->get_innerText(&bstrTotalPage);
				if (0 != _tcscmp(bstrCurPage, bstrTotalPage))
					lastPage = false;
			}
			break;
		}
		if (lastPage)
			break;

		//不是最后一页 点击下一页
		m_IEAdapterMFC->queryItems(szTagA, attrsNextPage, vecNextPage);
		if (0 == vecNextPage.size())
			break;

		bool bNextPage = false;
		std::vector<CComQIPtr<IHTMLElement>>::iterator nextItor = vecNextPage.begin();
		for (; nextItor != vecNextPage.end(); nextItor++)
		{
			CComBSTR bstrHtml;
			(*nextItor)->get_outerHTML(&bstrHtml);

			if (NULL == _tcsstr(bstrHtml, _T("next.gif")))
				continue;

			bNextPage = true;
			break;
		}
		if (!bNextPage)
			break;

		(*nextItor)->click();
		m_IEAdapterMFC->waitCompleted();
	}

	memset(tcharMsg, 0, sizeof(tcharMsg));
	_stprintf_s(tcharMsg, _T("下载 转换 完毕\r\n"));
	MSG_OUTPUT(tcharMsg);
	return;
}


void CStoryDirector::processPdfUrl(CComBSTR &url, CString csNamePrefix)
{
	TCHAR tcharMsg[1024] = {0};
	//CDownloadEvent dwnEvent;
	CComBSTR tFileName(1024);
	CComBSTR tTempPathName(s_TempDir.c_str());

	getFileName(url, tFileName);
	tTempPathName.Append(csNamePrefix.GetBuffer());
	tTempPathName.Append(tFileName);

	HRESULT hr;	

	hr = ::URLDownloadToFile(NULL, url, tTempPathName, 1024, NULL);
	//等待下载完成
	if (!SUCCEEDED(hr))
	{
		memset(tcharMsg, 0, sizeof(tcharMsg));
		_stprintf_s(tcharMsg, _T("下载失败 %s \r\n"), url);
		MSG_OUTPUT(tcharMsg);
		return;
	}

	//转换pdf文档成为text文档
	SHELLEXECUTEINFO exeInfo;
	memset(&exeInfo, 0, sizeof(exeInfo));

	CComBSTR tTempTextPathName(tTempPathName);
	tTempTextPathName.Append(_T(".txt"));

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
	//等待转换结束
	::WaitForSingleObject(exeInfo.hProcess, INFINITE);

	return;
}


void CStoryDirector::cleanTempDir()
{
	TCHAR tcharMsg[1024] = {0};
	SHELLEXECUTEINFO delExeInfo;
	memset(&delExeInfo, 0, sizeof(delExeInfo));

	CComBSTR bstrParams(_T("/c del /q "));
	bstrParams.Append(s_TempDir.c_str());

	delExeInfo.cbSize = sizeof(delExeInfo);
	delExeInfo.hwnd = NULL;
	delExeInfo.lpVerb = _T("open");
	delExeInfo.lpFile = _T("cmd.exe");
	delExeInfo.lpParameters = bstrParams;
	delExeInfo.nShow = SW_SHOWNORMAL;
	delExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&delExeInfo);
	::WaitForSingleObject(delExeInfo.hProcess, INFINITE);

	memset(tcharMsg, 0, sizeof(tcharMsg));
	_stprintf_s(tcharMsg, _T("清空目录完成 \r\n"));
	MSG_OUTPUT(tcharMsg);

	return;
}


void CStoryDirector::cleanWorkDir()
{
	TCHAR tcharMsg[1024] = {0};
	SHELLEXECUTEINFO delExeInfo;
	memset(&delExeInfo, 0, sizeof(delExeInfo));

	CComBSTR bstrParams(_T("/c del /q "));
	bstrParams.Append(s_WorkDir.c_str());

	delExeInfo.cbSize = sizeof(delExeInfo);
	delExeInfo.hwnd = NULL;
	delExeInfo.lpVerb = _T("open");
	delExeInfo.lpFile = _T("cmd.exe");
	delExeInfo.lpParameters = bstrParams;
	delExeInfo.nShow = SW_SHOWNORMAL;
	delExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&delExeInfo);
	::WaitForSingleObject(delExeInfo.hProcess, INFINITE);

	memset(tcharMsg, 0, sizeof(tcharMsg));
	_stprintf_s(tcharMsg, _T("清空目录完成 \r\n"));
	MSG_OUTPUT(tcharMsg);

	return;
}


void CStoryDirector::analysing(CString & fNameKeyword)
{
	if (fNameKeyword.IsEmpty())
		return;

	TCHAR tcharMsg[1024] = {0};
	std::vector<CString> kwVec;

	//读取中文必须设置语言
	TCHAR* old_locale = _tcsdup( _tsetlocale(LC_CTYPE,NULL) );
	_tsetlocale( LC_CTYPE, _T("chs"));

	//读取关键字文件获取所有关键词
	CStdioFile sf;
	CString strTemp;

	if (sf.Open(fNameKeyword, CFile::modeRead))
	{
		while (sf.ReadString(strTemp))
		{
			if (_T('#') == strTemp.GetAt(0))
			{
				continue;
			}
			strTemp.Trim();
			if (!strTemp.IsEmpty())
			{
				kwVec.push_back(strTemp);
			}
		}
		sf.Close();
	}

	//文件读取完毕还原语言
	_tsetlocale( LC_CTYPE, old_locale );
	free( old_locale );

	//遍历临时目录下所有txt文本
	CComBSTR bstrFind(s_TempDir.c_str());
	bstrFind.Append(_T("*.txt"));

	WIN32_FIND_DATA findFileData;

	HANDLE hFind = ::FindFirstFile(bstrFind, &findFileData);

    if (INVALID_HANDLE_VALUE != hFind)
    {		
		while(TRUE)
		{
			analyseSingle(findFileData.cFileName, kwVec);
			BOOL bRetNext = FALSE;
			bRetNext = ::FindNextFile(hFind, &findFileData);
			if (!bRetNext)
				break;
		}
    }

	memset(tcharMsg, 0, sizeof(tcharMsg));
	_stprintf_s(tcharMsg, _T("分析完成 \r\n"));
	MSG_OUTPUT(tcharMsg);
	return;
}




void CStoryDirector::analyseSingle(LPCTSTR fName, std::vector<CString>& kwVec)
{
	if (NULL == fName || 4 > _tcslen(fName))
		return;

	TCHAR tcharMsg[1024] = {0};
	bool bMatchedKeyword = false;
	CComBSTR bstrPath(s_TempDir.c_str());

	bstrPath.Append(fName);

	//读取UTF-8文件使用std::wifstream
	std::wifstream wifs;
	std::wstring wsLine;

	wifs.open(bstrPath);

	if (!wifs.is_open())
		return;

	wifs.imbue(std::locale(wifs.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>()));

	UINT sizeVec = kwVec.size();
	std::vector<UINT> indexVec(sizeVec, 0);

	while(std::getline(wifs, wsLine))
	{
		if (wsLine.empty())
			continue;

		std::wstring::iterator wsIt = wsLine.begin();

		for (; wsIt != wsLine.end(); wsIt++)
		{
			wchar_t wCurChar = *wsIt;
			if (_T(' ') == wCurChar || _T('\t') == wCurChar)
				continue;

			for (int iVec = 0; iVec < sizeVec; iVec++)
			{
				TCHAR kChar = kwVec[iVec].GetAt(indexVec[iVec]);
				if (kChar != wCurChar)
				{
					indexVec[iVec] = 0;
					continue;
				}					
				indexVec[iVec]++;
				if (indexVec[iVec] == kwVec[iVec].GetLength())
				{
					bMatchedKeyword = true;

					memset(tcharMsg, 0, sizeof(tcharMsg));
					_stprintf_s(tcharMsg, _T("文件%s 匹配到 %s\r\n"), fName, kwVec[iVec].GetBuffer());
					MSG_OUTPUT(tcharMsg);
					break;
				}
			}
			if (bMatchedKeyword)
				break;
		}
		if (bMatchedKeyword)
			break;
	}
	wifs.close();

	if (!bMatchedKeyword)
		return;

	//如果文件包含关键词则将文件的pdf版本拷贝到工作目录
	CString csPath(bstrPath);
	CString csPdfPath = csPath.Left(csPath.GetLength() - 4);
	
	SHELLEXECUTEINFO cpExeInfo;
	memset(&cpExeInfo, 0, sizeof(cpExeInfo));

	CComBSTR bstrParams(_T("/c copy /Y "));
	bstrParams.Append(csPdfPath.GetBuffer());
	bstrParams.Append(_T(' '));
	bstrParams.Append(s_WorkDir.c_str());

	cpExeInfo.cbSize = sizeof(cpExeInfo);
	cpExeInfo.hwnd = NULL;
	cpExeInfo.lpVerb = _T("open");
	cpExeInfo.lpFile = _T("cmd.exe");
	cpExeInfo.lpParameters = bstrParams;
	cpExeInfo.nShow = SW_SHOWNORMAL;
	cpExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	::ShellExecuteEx(&cpExeInfo);
	::WaitForSingleObject(cpExeInfo.hProcess, INFINITE);

	return;
}


