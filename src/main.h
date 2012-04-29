#ifndef __MAIN_H__
#define __MAIN_H__
#include <windows.h>

#include "LuaServerInterface.h"

class CMicroHTTPDInterface : public ILuaServerInterface
{
	~CMicroHTTPDInterface(){}
	bool Init(unsigned int Port);
	bool SetCallback(FnNewConnection Callback);
};

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C"
{
#endif
void* DLL_EXPORT GetInterface();
#ifdef __cplusplus
}
#endif
#endif // __MAIN_H__
