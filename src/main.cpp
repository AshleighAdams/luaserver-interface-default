#include "main.h"
#include <sstream>

// Boost libaries
#include <boost/thread.hpp>

#if defined _WIN32 || defined _WIN64
#else
// Some stuff to display the IP
#include <sys/socket.h>
#include <net/route.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define LINUX
#endif

using namespace std;

boost::mutex g_Mutex;

CMicroHTTPDInterface* 	g_pInterface = 0;
ServerConnection* 		g_pSC = 0;

int SetupHeaderCrap(void *cls, MHD_ValueKind kind, const char *key, const char *value)
{
	assert(g_pSC);
	
	switch(kind)
	{
	case MHD_GET_ARGUMENT_KIND:
		g_pSC->GETParams[key] = value;
		break;
	case MHD_HEADER_KIND:
		g_pSC->Headers[key] = value;
		break;
	case MHD_COOKIE_KIND:
		g_pSC->Cookies[key] = value;
		break;
	case MHD_POSTDATA_KIND:
		g_pSC->POSTParams[key] = value;
		break;
	case MHD_RESPONSE_HEADER_KIND:
		break;
	case MHD_FOOTER_KIND:
		break;
	}
	
	return 1;
}


int Connection(void *cls, MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls)
{
	ServerConnection con;
	con.Method = method;
	con.RequestedFile = url;
	con.VersionString = version;
	con.ErrorCode = MHD_HTTP_OK;
	
	con.pData = 0;
	con.DataLength = 0;
	
	sockaddr* so = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS)->client_addr;
	con.IP = inet_ntoa( ((sockaddr_in*)so)->sin_addr );
	
	// Hurp
	MHD_KeyValueIterator itt_key = &SetupHeaderCrap;
	{
		boost::mutex::scoped_lock lock_thingy(g_Mutex);
		g_pSC = &con;
		MHD_get_connection_values(connection, MHD_HEADER_KIND, 				itt_key, 0);
		MHD_get_connection_values(connection, MHD_COOKIE_KIND, 				itt_key, 0);
		MHD_get_connection_values(connection, MHD_POSTDATA_KIND, 			itt_key, 0);
		MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, 		itt_key, 0);
		MHD_get_connection_values(connection, MHD_FOOTER_KIND, 				itt_key, 0);
		MHD_get_connection_values(connection, MHD_RESPONSE_HEADER_KIND, 	itt_key, 0);
		g_pSC = 0;
	}
	
	//Ok we can invoke luaserver about our connection!
	g_pInterface->m_Func(&con);
	MHD_ResponseMemoryMode freetype = MHD_RESPMEM_MUST_FREE;
	
	if(con.DataIsConstant)
		freetype = MHD_RESPMEM_PERSISTENT;
	
	MHD_Response* response = MHD_create_response_from_buffer(con.DataLength, (void*)con.pData, freetype);
	
	// MHD now has ownership of the data.
	//if(con.pData)
	//	delete [] con.pData;
	
	for(auto it = con.ResponseHeaders.begin(); it != con.ResponseHeaders.end(); it++)
		MHD_add_response_header(response, it->first.c_str(), it->second.c_str());
	
	stringstream ss;
	for(auto it = con.SetCookies.begin(); it != con.SetCookies.end(); it++)
		ss << it->first << "=" << it->second << ";";
	
	if(con.SetCookies.size() > 0)
		MHD_add_response_header(response, "Set-Cookie", ss.str().c_str());
		
	int ret = MHD_queue_response (connection, con.ErrorCode, response);
	MHD_destroy_response (response);

	return ret;
}


const char* CMicroHTTPDInterface::GetInterfaceName()
{
	return "libmicrohttpd 0.9";
}

bool CMicroHTTPDInterface::Init(unsigned int Port)
{
	//MHD_USE_SELECT_INTERNALLY
	MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_THREAD_PER_CONNECTION, (uint16_t)Port, 0, 0, &Connection, 0,
		MHD_OPTION_PER_IP_CONNECTION_LIMIT, 	(unsigned int)20000,
		MHD_OPTION_CONNECTION_LIMIT, 			(unsigned int)40000,
		MHD_OPTION_CONNECTION_TIMEOUT, 			(unsigned int)3,
	MHD_OPTION_END);

	if(!daemon)
		return false;
	
	return true;
}

bool CMicroHTTPDInterface::SetCallback(FnNewConnection Callback)
{
	m_Func = Callback;
	return true;
}

CMicroHTTPDInterface::~CMicroHTTPDInterface()
{
	g_pInterface = 0;
}

#ifdef LINUX

extern "C" ILuaServerInterface* GetInterface()
{
	if(!g_pInterface)
		g_pInterface = new CMicroHTTPDInterface();
	return g_pInterface;
}

#else

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
            break;
        case DLL_PROCESS_DETACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }
    return 1;
}

#endif
