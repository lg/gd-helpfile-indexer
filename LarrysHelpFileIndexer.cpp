// LarrysHelpFileIndexer.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "LarrysHelpFileIndexer.h"

#include <vector>
#include <string>

const CLSID PluginClassID = {0xF5916434,0xF60F,0x4764,{0xAD,0x71,0x6B,0x1F,0x34,0x1B,0x57,0x23}};

class CLarrysHelpFileIndexerModule : public CAtlDllModuleT< CLarrysHelpFileIndexerModule >
{
public :
	DECLARE_LIBID(LIBID_LarrysHelpFileIndexerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_LARRYSHELPFILEINDEXER, "{9BD8B084-EEFE-4233-90C1-6F505EE7BD35}")
};

CLarrysHelpFileIndexerModule _AtlModule;


// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();

	if (SUCCEEDED(hr)) {
		LarGDSPlugin GDSPlugin(PluginClassID);

		vector<string> Extensions;
		Extensions.push_back("chm");
		Extensions.push_back("chw");

		GDSPlugin.RegisterPluginWithExtensions("Larry's Help File Indexer 1.00 by Larry Gadea", "Indexes CHM/CHW files", "", Extensions);
	}

	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();

	if (SUCCEEDED(hr)) {
		LarGDSPlugin GDSPlugin(PluginClassID);
		GDSPlugin.UnregisterPlugin();
	}
	
	return hr;
}
