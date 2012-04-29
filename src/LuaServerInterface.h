
#ifndef LUA_SERVER_INTERFACE_H
#define LUA_SERVER_INTERFACE_H

// Some STL libaries we need.
#include <functional>
#include <list>
#include <string>
#include <unordered_map>

// Some typedefs to make things easier
typedef std::unordered_map<std::string, std::string> ResponseHeaders;
typedef std::unordered_map<std::string, std::string> Cookies;
typedef std::function<void(ILuaServerConnection* Connection)> FnNewConnection;

// Here is what needs to be implemented.

struct ServerConnection
{
	std::string 		IP;
	std::string 		Method;
	std::string			RequestedFile;
	std::string			VersionString;
	Cookies				Cookies;
	ResponseHeaders 	Headers;
	unsigned int 		ErrorCode;
	unsigned char*		pData;
	unsigned int		DataLength;
};


class ILuaServerInterface
{
public:
	virtual ~ILuaServerInterface(){}
	virtual Init(unsigned int Port) = 0;
	virtual SetCallback(FnNewConnection Callback) = 0;
};


#endif // LUA_SERVER_INTERFACE_H
