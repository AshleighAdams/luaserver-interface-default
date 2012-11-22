#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef _WIN32
#define WINDOWS
#endif
#ifdef _WIN64
#define WINDOWS
#endif

#include "microhttpd.h"

#ifdef WINDOWS
#include <windows.h>
#endif

#include "LuaServerInterface.h"

class CMicroHTTPDInterface : public ILuaServerInterface
{
public:
	FnNewConnection m_Func;
	~CMicroHTTPDInterface();
	bool Init(unsigned int Port);
	bool SetCallback(FnNewConnection Callback);
	const char* GetInterfaceName();
};

#ifdef WINDOWS

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
ILuaServerInterface* DLL_EXPORT GetInterface();
#ifdef __cplusplus
}
#endif

#endif // WINDOWS

#endif // __MAIN_H__
