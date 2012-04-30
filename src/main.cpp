#include "main.h"
#include <sstream>

// Boost libaries
#include <boost/thread.hpp>

using namespace std;

boost::mutex g_Mutex;

CMicroHTTPDInterface* 	g_pInterface = 0;
ServerConnection* 		g_pSC = 0;

int SetupHeaderCrap(void *cls, enum MHD_ValueKind kind, const char *key, const char *value)
{
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
	
	//sockaddr *so;
	//so = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS)->client_addr;
	con.IP = "0.0.0.0"; //inet_ntoa( ((sockaddr_in*)so)->sin_addr );
	
	// Hurp
	MHD_KeyValueIterator itt_key = &SetupHeaderCrap;
	{
		boost::mutex::scoped_lock lock_thingy(g_Mutex);
		MHD_get_connection_values(connection, MHD_HEADER_KIND, 				itt_key, 0);
		MHD_get_connection_values(connection, MHD_COOKIE_KIND, 				itt_key, 0);
		MHD_get_connection_values(connection, MHD_POSTDATA_KIND, 			itt_key, 0);
		MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, 		itt_key, 0);
		MHD_get_connection_values(connection, MHD_FOOTER_KIND, 				itt_key, 0);
		MHD_get_connection_values(connection, MHD_RESPONSE_HEADER_KIND, 	itt_key, 0);
	}
	
	//Ok we can invoke luaserver about our connection!
	g_pInterface->m_Func(&con);

	MHD_Response* response = MHD_create_response_from_buffer(con.DataLength, (void*)con.pData, MHD_RESPMEM_MUST_COPY);
	
	if(con.pData)
		delete [] con.pData;
	
	for(auto it = con.ResponseHeaders.begin(); it != con.ResponseHeaders.end(); it++)
		MHD_add_response_header(response, it->first.c_str(), it->second.c_str());
	
	stringstream ss;
	for(auto it = con.Cookies.begin(); it != con.Cookies.end(); it++)
		ss << it->first << "=" << it->second << ";";
	
	if(con.Cookies.size() > 0)
		MHD_add_response_header(response, "Set-Cookie", ss.str().c_str());
		
	int ret = MHD_queue_response (connection, con.ErrorCode, response);
	MHD_destroy_response (response);

	return ret;
}


const char* CMicroHTTPDInterface::GetInterfaceName()
{
	return "libmicrohttpd";
}

bool CMicroHTTPDInterface::Init(unsigned int Port)
{
	//MHD_USE_SELECT_INTERNALLY
	MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_THREAD_PER_CONNECTION, Port, 0, 0, &Connection, 0,
		MHD_OPTION_PER_IP_CONNECTION_LIMIT, 	(unsigned int)4,
		MHD_OPTION_CONNECTION_LIMIT, 			(unsigned int)40,
		MHD_OPTION_CONNECTION_TIMEOUT, 			(unsigned int)10,
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
