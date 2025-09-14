
#include <math.h>

#include "net.h"

#undef SOCKET
#undef INVALID_SOCKET

#ifdef _WIN32
	#include <winsock2.h>
	#include <Ws2tcpip.h>
#else
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
#endif

static_assert( MAX_SOCKET_NAME >= INET6_ADDRSTRLEN );

constexpr i32 SocketDomain[] =
{
	AF_UNIX,
	AF_INET,
	AF_IPX,
	AF_APPLETALK,
	AF_INET6,
};

constexpr i32 SocketType[] =
{
	SOCK_STREAM,
	SOCK_DGRAM,
	SOCK_RAW,
};

i32 netinit()
{
	#ifdef _WIN32
		WSADATA wsaData;
		return WSAStartup( MAKEWORD( 2, 1 ), &wsaData );
	#else
		return 0;
	#endif
}

i32 netquit()
{
	#ifdef _WIN32
		return WSACleanup();
	#else
		return 0;
	#endif
}

SOCKET netsockopen( SOCKET_DOMAIN domain, SOCKET_TYPE type )
{
	return socket( SocketDomain[ domain ], SocketType[ type ], 0 );
}

i32 netsockclose( SOCKET sock )
{
	i32 status = 0;

	#ifdef _WIN32
		status = shutdown( sock, SD_BOTH );

		if ( status == 0 )
		{
			status = closesocket( sock );
		}
	#else
		status = shutdown( sock, SHUT_RDWR );

		if ( status == 0 )
		{
			status = close( sock );
		}
	#endif

	return status;
}

void netsocktimeout( SOCKET sock, i32 seconds )
{
	#ifdef _WIN32
		DWORD timeout = seconds * 1000;
	#else
		struct timeval timeout;
		timeout.tv_sec = seconds;
		timeout.tv_usec = 0;
	#endif

	setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof( timeout ) );
}

bool netsockname( SOCKET sock, char name[ MAX_SOCKET_NAME ] )
{
	struct sockaddr_storage sockName;
	socklen_t sockNameSize = sizeof( sockName );

	i32 result = getsockname( sock, (struct sockaddr*)&sockName, &sockNameSize );

	if ( result == SOCKET_ERROR )
		return false;

	if ( sockName.ss_family == AF_INET )
	{
		struct sockaddr_in *s = reinterpret_cast<struct sockaddr_in*>( &sockName );
		inet_ntop( AF_INET, &(s->sin_addr), name, MAX_SOCKET_NAME );
	}
	else if ( sockName.ss_family == AF_INET6 )
	{
		struct sockaddr_in6 *s = reinterpret_cast<struct sockaddr_in6*>( &sockName );
		inet_ntop( AF_INET6, &(s->sin6_addr), name, MAX_SOCKET_NAME );
	}
	else
	{
		return false;
	}

	return true;
}

i32 netbind( SOCKET sock, u16 port, i32 maxQueue )
{
	SOCKADDR_IN addr = {};
	addr.sin_port = htons( port );
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	bind( sock, (SOCKADDR *)&addr, sizeof( addr ) );
	listen( sock, maxQueue );
	return 0;
}

SOCKET netlisten( SOCKET sock )
{
	WSAPOLLFD fds =
	{
		.fd = sock,
		.events = POLLIN,
	};

	#ifdef _WIN32
		i32 ret = WSAPoll( &fds, 1, 1000 );
	#else
		i32 ret = poll( &fds, 1, 1000 );
	#endif

	if ( ret <= 0 )
		return INVALID_SOCKET;

	if ( !( fds.revents & POLLIN ) )
		return INVALID_SOCKET;

	return accept( sock, nullptr, nullptr );
}

i32 netconnect( SOCKET sock, const char *hostname, u16 port )
{
	SOCKADDR_IN addr = {};
	addr.sin_port = htons( port );
	addr.sin_family = PF_INET;
	InetPton( AF_INET, hostname, &addr.sin_addr.s_addr );
	return connect( sock, (SOCKADDR *)&addr, sizeof( addr ) );
}

i32 netsend( SOCKET sock, const char *data, i32 bytes )
{
	return send( sock, data, bytes, 0 );
}

i32 netrecv( SOCKET sock, char data[ MAX_PACKET_SIZE ] )
{
	return recv( sock, data, MAX_PACKET_SIZE, 0 );
}

// --------------------------------------------------------------------

Value BuiltInNode_Struct_NET_Init( Interpreter *interpreter, Value &self, Node *args )
{
	(void)interpreter;
	(void)args;
	(void)self;
	return netinit();
}

Value BuiltInNode_Struct_NET_Quit( Interpreter *interpreter, Value &self, Node *args )
{
	(void)interpreter;
	(void)args;
	(void)self;
	return netquit();
}

Value BuiltInNode_Struct_NET_SockOpen( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;

	SOCKET_DOMAIN domain;
	SOCKET_TYPE type;

	switch ( args->children.size() )
	{
	case 0:
		domain = SOCKET_DOMAIN_INET;
		type = SOCKET_TYPE_STREAM;
		break;

	case 1:
		{
			Arg arg = &args->children[ 0 ];
			domain = SOCKET_DOMAIN_INET;
			type = (SOCKET_TYPE)interpreter->run( arg ).get_as_i32( interpreter, arg );
		}
		break;

	case 2:
		{
			Arg arg = &args->children[ 0 ];
			domain = (SOCKET_DOMAIN)interpreter->run( arg ).get_as_i32( interpreter, arg.next() );
			type = (SOCKET_TYPE)interpreter->run( arg ).get_as_i32( interpreter, arg );
		}
		break;

	default:
		interpreter->fatal( RESULT_CODE_INVALID_ARGS_BUILTIN_FUNC, args, "sockopen received incorrect arguments." );
	}

	SOCKET socket = netsockopen( domain, type );

	if ( socket == INVALID_SOCKET )
		return static_cast<i32>( -1 );

	return static_cast<i32>( socket );
}

Value BuiltInNode_Struct_NET_SockValid( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "sockvalid", args, 1 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg );
	return socket != -1;
}

Value BuiltInNode_Struct_NET_SockClose( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "sockclose", args, 1 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg );
	return netsockclose( socket );
}

Value BuiltInNode_Struct_NET_SockTimeout( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "socktimeout", args, 2 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg.next() );
	i32 seconds = interpreter->run( arg ).get_as_i32( interpreter, arg );
	netsocktimeout( socket, seconds );
	return 0;
}

Value BuiltInNode_Struct_NET_SockName( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "sockname", args, 1 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg );
	char name[ MAX_SOCKET_NAME ];
	if ( netsockname( socket, name ) )
		return name;
	return "<unknown>";
}

Value BuiltInNode_Struct_NET_Bind( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "sockbind", args, 3 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg.next() );
	u16 port = (u16)interpreter->run( arg ).get_as_i32( interpreter, arg.next() );
	i32 maxQueue = interpreter->run( arg ).get_as_i32( interpreter, arg );
	return netbind( socket, port, maxQueue );
}

Value BuiltInNode_Struct_NET_Listen( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "socklisten", args, 1 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = netlisten( (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg ) );
	if ( socket == INVALID_SOCKET )
		return static_cast<i32>( -1 );
	return static_cast<i32>( socket );
}

Value BuiltInNode_Struct_NET_Connect( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "sockconnect", args, 3 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg.next() );
	std::string hostname = interpreter->run( arg ).get_as_string( interpreter, arg.next() );
	u16 port = (u16)interpreter->run( arg ).get_as_i32( interpreter, arg );
	return netconnect( socket, hostname.c_str(), port );
}

Value BuiltInNode_Struct_NET_Send( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "socksend", args, 3 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg.next() );
	std::string data = interpreter->run( arg ).get_as_string( interpreter, arg.next() );
	i32 bytes = interpreter->run( arg ).get_as_i32( interpreter, arg );
	return netsend( socket, data.c_str(), bytes );
}

Value BuiltInNode_Struct_NET_Recv( Interpreter *interpreter, Value &self, Node *args )
{
	(void)self;
	interpreter->expect_arg( "sockrecv", args, 1 );
	Arg arg = &args->children[ 0 ];
	SOCKET socket = (SOCKET)interpreter->run( arg ).get_as_i32( interpreter, arg );
	char data[ MAX_PACKET_SIZE ];
	if ( netrecv( socket, data ) != 0 )
		return data;
	return nullptr;
}