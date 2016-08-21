
#include "stdafx.h"
#include "DownloadEvent.h"



CDownloadEvent::CDownloadEvent()
{
	m_downloadEnd = false;
}


CDownloadEvent::~CDownloadEvent()
{}


HRESULT CDownloadEvent::QueryInterface( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return E_NOTIMPL;
}


ULONG CDownloadEvent::AddRef( void)
{
	return E_NOTIMPL;
}


ULONG CDownloadEvent::Release( void)
{
	return E_NOTIMPL;
}


HRESULT CDownloadEvent::OnStartBinding( 
        /* [in] */ DWORD dwReserved,
        /* [in] */ __RPC__in_opt IBinding *pib)
{
	return E_NOTIMPL;
}
  

HRESULT CDownloadEvent::GetPriority( 
        /* [out] */ __RPC__out LONG *pnPriority)
{
	return E_NOTIMPL;
}

        
HRESULT CDownloadEvent::OnLowResource( 
        /* [in] */ DWORD reserved)
{
	return E_NOTIMPL;
}

        
HRESULT CDownloadEvent::OnProgress( 
        /* [in] */ ULONG ulProgress,
        /* [in] */ ULONG ulProgressMax,
        /* [in] */ ULONG ulStatusCode,
        /* [unique][in] */ __RPC__in_opt LPCWSTR szStatusText)
{
	if (BINDSTATUS_ENDDOWNLOADDATA == ulStatusCode)
	{
		m_downloadEnd = true;
	}
	return S_OK;
}
 

HRESULT CDownloadEvent::OnStopBinding( 
        /* [in] */ HRESULT hresult,
        /* [unique][in] */ __RPC__in_opt LPCWSTR szError)
{
	return E_NOTIMPL;
}

        
HRESULT CDownloadEvent::GetBindInfo( 
        /* [out] */ DWORD *grfBINDF,
        /* [unique][out][in] */ BINDINFO *pbindinfo)
{
	return E_NOTIMPL;
}
  

HRESULT CDownloadEvent::OnDataAvailable( 
        /* [in] */ DWORD grfBSCF,
        /* [in] */ DWORD dwSize,
        /* [in] */ FORMATETC *pformatetc,
        /* [in] */ STGMEDIUM *pstgmed)
{
	return E_NOTIMPL;
}
        

HRESULT CDownloadEvent::OnObjectAvailable( 
        /* [in] */ __RPC__in REFIID riid,
        /* [iid_is][in] */ __RPC__in_opt IUnknown *punk)
{
	return E_NOTIMPL;
}


void CDownloadEvent::waitForEnd()
{
	while(!m_downloadEnd)
	{
		::Sleep(1000);
	}
}




