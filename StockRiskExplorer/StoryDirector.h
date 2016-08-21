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

protected:
	DECLARE_MESSAGE_MAP()

private:

	bool m_bInitialized;

	//UINT m_pageNum;

public:

	boost::shared_ptr<CIEAdapterMFC> m_IEAdapterMFC;

};


