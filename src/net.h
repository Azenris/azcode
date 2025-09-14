
#pragma once

#ifdef _WIN32
	#ifdef _WIN64
		#define SOCKET u64
	#else
		#define SOCKET u32
	#endif
	#define INVALID_SOCKET ((SOCKET)(~0))
#else
	#define SOCKET i32
	#define INVALID_SOCKET -1
#endif

constexpr u64 MAX_SOCKET_NAME = 128;
constexpr u64 MAX_PACKET_SIZE = 8192;

enum SOCKET_DOMAIN
{
	SOCKET_DOMAIN_UNIX,
	SOCKET_DOMAIN_INET,
	SOCKET_DOMAIN_IPX,
	SOCKET_DOMAIN_APPLETALK,
	SOCKET_DOMAIN_INET6,
};

enum SOCKET_TYPE
{
	SOCKET_TYPE_STREAM,
	SOCKET_TYPE_DGRAM,
	SOCKET_TYPE_RAW,
};

i32 netinit();
i32 netquit();
SOCKET netsockopen( SOCKET_DOMAIN domain, SOCKET_TYPE type );
i32 netsockclose( SOCKET sock );
void netsocktimeout( SOCKET sock, i32 seconds );
bool netsockname( SOCKET sock, char name[ MAX_SOCKET_NAME ] );
i32 netbind( SOCKET sock, u16 port, i32 maxQueue );
SOCKET netlisten( SOCKET sock );
i32 netconnect( SOCKET sock, const char *hostname, u16 port );
i32 netsend( SOCKET sock, const char *data, i32 bytes );
i32 netrecv( SOCKET sock, char data[ MAX_PACKET_SIZE ] );

Value BuiltInNode_Struct_NET_Init( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_Quit( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_SockOpen( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_SockValid( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_SockClose( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_SockTimeout( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_SockName( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_Bind( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_Listen( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_Connect( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_Send( Interpreter *interpreter, Value &self, Node *args );
Value BuiltInNode_Struct_NET_Recv( Interpreter *interpreter, Value &self, Node *args );