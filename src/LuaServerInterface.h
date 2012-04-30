
#ifndef LUA_SERVER_INTERFACE_H
#define LUA_SERVER_INTERFACE_H

// Some STL libaries we need.
#include <functional>
#include <list>
#include <string>
#include <unordered_map>

// Forward decleration
struct ServerConnection;

// Some typedefs to make things easier
typedef std::unordered_map<std::string, std::string> MapResponseHeaders;
typedef std::unordered_map<std::string, std::string> MapCookies;
typedef std::function<void(ServerConnection* pConnection)> FnNewConnection;

// Here is what needs to be implemented.

struct ServerConnection
{
	std::string 		IP;
	std::string 		Method;
	std::string			RequestedFile;
	std::string			VersionString;
	MapCookies			Cookies;
	MapResponseHeaders 	Headers;
	unsigned int 		ErrorCode;
	unsigned char*		pData;
	unsigned int		DataLength;
};


class ILuaServerInterface
{
public:
	virtual ~ILuaServerInterface(){}
	virtual bool Init(unsigned int Port) = 0;
	virtual bool SetCallback(FnNewConnection Callback) = 0;
};


#endif // LUA_SERVER_INTERFACE_H
