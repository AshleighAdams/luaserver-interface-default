#include "main.h"

CMicroHTTPDInterface* g_pInterface = 0;

bool CMicroHTTPDInterface::Init(unsigned int Port)
{
	return false;
}

bool CMicroHTTPDInterface::SetCallback(FnNewConnection Callback)
{
	return false;
}

CMicroHTTPDInterface::~CMicroHTTPDInterface()
{
	g_pInterface = 0;
}

ILuaServerInterface* DLL_EXPORT GetInterface()
{
	if(!g_pInterface)
		g_pInterface = new CMicroHTTPDInterface();
	return g_pInterface;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
