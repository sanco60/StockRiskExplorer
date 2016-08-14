#pragma once


// CStoryDirector

class CStoryDirector : public CWnd
{
	DECLARE_DYNAMIC(CStoryDirector)

public:
	CStoryDirector();

	virtual ~CStoryDirector();

	bool init();

	bool uninit();

	bool getTopIE();

	CString	GetWindowClassName(IWebBrowser2* pwb);

protected:
	DECLARE_MESSAGE_MAP()

private:

	bool m_bInitialized;

	CComQIPtr<IWebBrowser2> m_pWebBrowser2;
};


