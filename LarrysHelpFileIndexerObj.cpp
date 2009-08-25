// LarrysHelpFileIndexerObj.cpp : Implementation of CLarrysHelpFileIndexerObj

#include "stdafx.h"
#include "LarrysHelpFileIndexerObj.h"
#include ".\larryshelpfileindexerobj.h"

#include "ChmLib\chm_lib.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

const CLSID PluginClassID = {0xF5916434,0xF60F,0x4764,{0xAD,0x71,0x6B,0x1F,0x34,0x1B,0x57,0x23}};

LarGDSPlugin GDSPlugin(PluginClassID);

struct ExtractContext {
    string TotalData;
};

string HTMLToText(string HTML) {
	// The following is a _very simple_ HTML to text convertor. It will simply
	// remove all tags and replace them with new line characters.
	string PlainText;

	size_t CurPos = 0;
	size_t CloseTag = 1;
	size_t OpenTag = 1;

	CloseTag = HTML.find(">", CurPos);
	while ((CloseTag != HTML.npos) && (OpenTag != HTML.npos)) {
		OpenTag = HTML.find("<", CloseTag);
        
		if (OpenTag != HTML.npos) {
			if (OpenTag - CloseTag >= 2) {
				PlainText.append(HTML.substr(CloseTag + 1, OpenTag - CloseTag - 1));
				PlainText.append("\n");
			}

			CurPos = OpenTag;
			CloseTag = HTML.find(">", CurPos);
		}
	}

	return PlainText;
}

int CHMExtractCallback(struct chmFile *h, struct chmUnitInfo *ui, void *Context)
{ 
	string Path(ui->path);
	struct ExtractContext *CTX = (struct ExtractContext *)Context;

	// We need to get the filename. For that, we need to find the last slash
	size_t LastSlash = Path.find_last_of("/");
	if (LastSlash == Path.npos) {
		return CHM_ENUMERATOR_FAILURE;
	}

	string Filename;
	Filename = Path.substr(LastSlash);

	// Check if the filename contains htm or html -- to decide if it will be parsed or not
	if (!(Filename.find("htm") != Path.npos)) {
		return CHM_ENUMERATOR_CONTINUE;
	}
	
	// Read the data from the HTML into HTMLData
	string HTMLData;
	LONGINT64 ReadLength, DataRemaining;
    LONGUINT64 CurOffset = 0;
	unsigned char* ReadBuffer;
	ReadBuffer = (unsigned char*)malloc(32767);
	
	DataRemaining = ui->length;
	while (DataRemaining != 0) {
		ReadLength = chm_retrieve_object(h, ui, ReadBuffer, CurOffset, 32767);
        
		if (ReadLength > 0) {
			HTMLData.append(string((char *)ReadBuffer));
			ZeroMemory(ReadBuffer, 32767);	// Needed because of the multithreaded nature of GDS
			CurOffset += ReadLength;
            DataRemaining -= ReadLength;
        } else {
			free(ReadBuffer);
			return CHM_ENUMERATOR_FAILURE;
		}
    }
	free(ReadBuffer);

	// Convert the HTML to PlainText
	string Data;
	Data = HTMLToText(HTMLData);

	CTX->TotalData.append(Data);

	Context = CTX;
	
	return CHM_ENUMERATOR_CONTINUE;
}

STDMETHODIMP CLarrysHelpFileIndexerObj::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ILarrysHelpFileIndexerObj
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CLarrysHelpFileIndexerObj::HandleFile(BSTR RawFullPath, IDispatch* IFactory)
{
	string FullPath = CW2A(RawFullPath);

	// Open the CHM file using the CHM library
	struct chmFile *CHMHandle;
	CHMHandle = chm_open(FullPath.c_str()); 
    if (CHMHandle == NULL) {
        return S_FALSE;
    }

	// Extract data from the CHM
	string TotalData;
	struct ExtractContext CTX;
	CTX.TotalData = TotalData;

	if (!chm_enumerate(CHMHandle, CHM_ENUMERATE_ALL, CHMExtractCallback, (void *)&CTX)) {
		return S_FALSE;
	}
       
	// Close the CHM file
	chm_close(CHMHandle);

	// Get the modification time of the file
	HANDLE FileHandle;
	FileHandle = CreateFile(FullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	FILETIME *ModifiedTime = (FILETIME *)malloc(sizeof FILETIME);
	BOOL FileTimeResult = GetFileTime(FileHandle, 0, 0, ModifiedTime);
	CloseHandle(FileHandle);
	if (!FileTimeResult) {
        return S_FALSE; 
	}

	// Format modification date properly
	SYSTEMTIME SystemTime;
	FileTimeToSystemTime(ModifiedTime, &SystemTime);
	
	// Index file
	if (!GDSPlugin.SendTextFileEvent(CTX.TotalData.c_str(), FullPath, SystemTime)) {
		return S_FALSE;
	}

	return S_OK;
}
