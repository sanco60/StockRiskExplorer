#pragma once

#include <UrlMon.h>


class CDownloadEvent : public IBindStatusCallback
{
public:
	CDownloadEvent();

	virtual ~CDownloadEvent();

public:

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	virtual ULONG STDMETHODCALLTYPE AddRef( void);

	virtual ULONG STDMETHODCALLTYPE Release( void);

public:
    virtual HRESULT STDMETHODCALLTYPE OnStartBinding( 
        /* [in] */ DWORD dwReserved,
        /* [in] */ __RPC__in_opt IBinding *pib);
        
    virtual HRESULT STDMETHODCALLTYPE GetPriority( 
        /* [out] */ __RPC__out LONG *pnPriority);
        
    virtual HRESULT STDMETHODCALLTYPE OnLowResource( 
        /* [in] */ DWORD reserved);
        
    virtual HRESULT STDMETHODCALLTYPE OnProgress( 
        /* [in] */ ULONG ulProgress,
        /* [in] */ ULONG ulProgressMax,
        /* [in] */ ULONG ulStatusCode,
        /* [unique][in] */ __RPC__in_opt LPCWSTR szStatusText);
        
    virtual HRESULT STDMETHODCALLTYPE OnStopBinding( 
        /* [in] */ HRESULT hresult,
        /* [unique][in] */ __RPC__in_opt LPCWSTR szError);
        
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetBindInfo( 
        /* [out] */ DWORD *grfBINDF,
        /* [unique][out][in] */ BINDINFO *pbindinfo);
        
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE OnDataAvailable( 
        /* [in] */ DWORD grfBSCF,
        /* [in] */ DWORD dwSize,
        /* [in] */ FORMATETC *pformatetc,
        /* [in] */ STGMEDIUM *pstgmed);
        
    virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable( 
        /* [in] */ __RPC__in REFIID riid,
        /* [iid_is][in] */ __RPC__in_opt IUnknown *punk);
	

public:

	bool waitForEnd(UINT tSec);


private:

	bool m_downloadEnd;
};

