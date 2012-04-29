
#ifndef LUA_SERVER_INTERFACE_H
#define LUA_SERVER_INTERFACE_H

// Some STL libaries we need.
#include <functional>
#include <list>
#include <string>
#include <unordered_map>

// Some typedefs to make things easier
typedef std::unordered_map<std::string, std::string> ResponseHeaders;
typedef std::function<void(ILuaServerConnection* Connection)> FnNewConnection;

// Here is what needs to be implemented.

class ILuaServerConnection
{
public:
	virtual ~ILuaServerConnection(){}
	std::string& 			IP() = 0;
	std::string& 			Method() = 0;
	std::string&			RequestedFile() = 0;
	std::string&			VersionString() = 0;
	ResponseHeaders& 		Headers() = 0;
};


class ILuaServerInterface
{
public:
	virtual ~ILuaServerInterface();
	virtual SetCallback(FnNewConnection Callback) = 0;
	
};


#endif // LUA_SERVER_INTERFACE_H
