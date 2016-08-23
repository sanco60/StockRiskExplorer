#pragma once


#include "boost\shared_ptr.hpp"
#include "IEAdapterMFC.h"

// CStoryDirector

class CStoryDirector : public CWnd
{
	DECLARE_DYNAMIC(CStoryDirector)

public:
	CStoryDirector();

	virtual ~CStoryDirector();

	bool init();

	bool uninit();

	CComQIPtr<IWebBrowser2> getTopIE();

	CString	GetWindowClassName(IWebBrowser2* pwb);

	void processShangHai();

	void processShenZhen();

	void processPdfUrl(CComBSTR &url);

	void getFileName(CComBSTR &url, CComBSTR &fname);

	void cleanTempDir();

	void cleanWorkDir();

	void analysing(CString & fNameKeyword);

	void analyseSingle(LPCTSTR fName, std::vector<CString>& kwVec);

protected:
	DECLARE_MESSAGE_MAP()

private:

	bool m_bInitialized;

public:

	boost::shared_ptr<CIEAdapterMFC> m_IEAdapterMFC;

};


