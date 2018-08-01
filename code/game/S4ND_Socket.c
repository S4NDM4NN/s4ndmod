#include "g_local.h"
#include "S4ND_local.h"

#ifdef WIN32
	#include <winsock.h>
	#pragma comment(lib,"wsock32.lib")
#else
	#define closesocket close
	#include <unistd.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif


u_int GetHostAddress( char* host ) {
	struct hostent *phe;
	char *p;

	phe = gethostbyname( host );

	if ( phe == NULL ) {
		return 0;
	}

	p = *phe->h_addr_list;
	return *( (u_int*)p );
}

void SendString( int sock,char* str ) {
	send( sock,str,(int)strlen( str ),0 );
}


int ValidHostChar( char ch ) {
	return( isalpha( ch ) || isdigit( ch )
			|| ch == '-' || ch == '.' || ch == ':' );
}

char *strupr2( char *string ) {
	char *s;

	if ( string ) {
		for ( s = string; *s; ++s )
			*s = toupper( *s );
	}
	return string;
}

void ParseURL( char* url,char* protocol,int lprotocol,
			   char* host,int lhost,char* request,int lrequest,int *port ) {
	char *work,*ptr,*ptr2;

	*protocol = *host = *request = 0;
	*port = 80;

	work = strdup( url );

	strupr2( work );

	ptr = strchr( work,':' );
	if ( ptr != NULL ) {
		*( ptr++ ) = 0;
		strcpy( protocol,work );
	} else
	{
		strcpy( protocol,"HTTP" );
		ptr = work;
	}

	if ( ( *ptr == '/' ) && ( *( ptr + 1 ) == '/' ) ) {
		ptr += 2;
	}

	ptr2 = ptr;
	while ( ValidHostChar( *ptr2 ) && *ptr2 )
		ptr2++;

	*ptr2 = 0;
	strcpy( host,ptr );

	strcpy( request,url + ( ptr2 - work ) );

	ptr = strchr( host,':' );
	if ( ptr != NULL ) {
		*ptr = 0;
		*port = atoi( ptr + 1 );
	}

	free( work );
}


int httpGet( char*url,char*filename ) {
#ifdef WIN32
	WSADATA WsaData;
#endif

	struct  sockaddr_in sin;
	int sock;
	char buffer[512];
	char protocol[20],host[256],request[1024];
	int l,port,chars,err;
	int done;
	FILE*        out;

	out = fopen( filename,"w+" );

	ParseURL( url,protocol,sizeof( protocol ),host,sizeof( host ),request,sizeof( request ),&port ); // Parse the URL

	if ( strcmp( protocol,"HTTP" ) ) {
		fclose( out );
		return 0;
	}

		#ifdef WIN32
	err = WSAStartup( 0x0101, &WsaData );       // Init Winsock
	if ( err != 0 ) {
		return 0;
	}
		#endif

	sock = (int)socket( AF_INET, SOCK_STREAM, 0 );

	if ( sock < 0 ) {
		return 0;
	}

	sin.sin_family = AF_INET;                                           //Connect to web sever
	sin.sin_port = htons( (unsigned short)port );
	sin.sin_addr.s_addr = GetHostAddress( host );

	if ( connect( sock,(struct sockaddr*)&sin, sizeof( /*struct sockaddr_in*/ sin ) ) ) {
		return 0;
	}

	if ( !*request ) {
		strcpy( request,"/" );
	}

	//Start Sending header
	SendString( sock,"GET " );
	SendString( sock,request );
	SendString( sock," HTTP/1.0\r\n"
					 "User-Agent: S4NDMoD/2.4.0\r\n"
					 "Host: " );
	SendString( sock,host );
	SendString( sock,"\r\n\r\n" );        // Send a blank line to signal end of HTTP headerReceive

	//receive the header from the server really not doing anything with this info...
	chars = 0;
	done = 0;
	while ( !done )
	{
		l = recv( sock,buffer,1,0 );
		if ( l < 0 ) {
			done = 1;
		}

		switch ( *buffer )
		{
		case '\r':
			break;
		case '\n':
			if ( chars == 0 ) {
				done = 1;
			}
			chars = 0;
			break;
		default:
			chars++;
			break;
		}
	}

	do
	{
		l = recv( sock,buffer,sizeof( buffer ) - 1,0 );
		if ( l < 0 ) {
			break;
		}
		*( buffer + l ) = 0;
		fputs( buffer,out );
	} while ( l > 0 );

	closesocket( sock );
	fclose( out );
	return 1;

}
//after some testing with sean, found that some people just dont have hostnames, so lets just return ip.
//This should be called from inside a thread!
char*iptohostname( char*ip ) {
#ifdef WIN32
	WSADATA wsaData;
	int iResult;
#endif
	struct hostent *remoteHost;
	struct in_addr addr;
#ifdef WIN32
	iResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if ( iResult != 0 ) {
		return ip;
	}
#endif
	addr.s_addr = inet_addr( ip );
	if ( addr.s_addr == INADDR_NONE ) {
		return ip;
	} else {
		remoteHost = gethostbyaddr( (char *) &addr, 4, AF_INET );
	}

	if ( remoteHost == NULL ) {
		return ip;
	} else {
		if ( !remoteHost->h_name ) {
			return ip;
		} else {
			return remoteHost->h_name;
		}
	}
	return ip;
}