// LarrysHelpFileIndexerObj.h : Declaration of the CLarrysHelpFileIndexerObj

#pragma once
#include "resource.h"       // main symbols

#include "LarrysHelpFileIndexer.h"


// CLarrysHelpFileIndexerObj

class ATL_NO_VTABLE CLarrysHelpFileIndexerObj : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CLarrysHelpFileIndexerObj, &CLSID_LarrysHelpFileIndexerObj>,
	public ISupportErrorInfo,
	public IDispatchImpl<ILarrysHelpFileIndexerObj, &IID_ILarrysHelpFileIndexerObj, &LIBID_LarrysHelpFileIndexerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CLarrysHelpFileIndexerObj()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LARRYSHELPFILEINDEXEROBJ)


BEGIN_COM_MAP(CLarrysHelpFileIndexerObj)
	COM_INTERFACE_ENTRY(ILarrysHelpFileIndexerObj)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

public:

	STDMETHOD(HandleFile)(BSTR RawFullPath, IDispatch* IFactory);
};

OBJECT_ENTRY_AUTO(__uuidof(LarrysHelpFileIndexerObj), CLarrysHelpFileIndexerObj)
