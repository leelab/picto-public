#include <Windows.h>
#include "WinXPSP2FireWall.h"

int main(int argc, char* argv[] )
{
	CoInitialize( NULL );

	WinXPSP2FireWall fw;
	fw.Initialize();

	wchar_t szApplication[MAX_PATH];
	GetCurrentDirectoryW( MAX_PATH, szApplication );
#ifdef _DEBUG
	
	wcscat(szApplication, L"\\Debug\\WindowsFirewall.exe");
#else
	wcscat( szApplication, L"\\Release\\WindowsFirewall.exe");
#endif

	fw.AddApplication( szApplication, L"FireWallTest");
	fw.RemoveApplication( szApplication );

	fw.AddPort( 4321, NET_FW_IP_PROTOCOL_TCP, L"FireWallPortTest" );
	fw.RemovePort( 4321, NET_FW_IP_PROTOCOL_TCP );

	fw.Uninitialize();

	CoUninitialize();
	return 0;
}