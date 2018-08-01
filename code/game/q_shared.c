// Copyright (C) 1999-2000 Id Software, Inc.
//
// q_shared.c -- stateless support routines that are included in each code dll
#include "q_shared.h"

// os x game bundles have no standard library links, and the defines are not always defined!

#ifdef MACOS_X
int qmax( int x, int y ) {
	return ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) );
}

int qmin( int x, int y ) {
	return ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) );
}
#endif

float Com_Clamp( float min, float max, float value ) {
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}


/*
============
COM_SkipPath
============
*/
char *COM_SkipPath( char *pathname ) {
	char    *last;

	last = pathname;
	while ( *pathname )
	{
		if ( *pathname == '/' ) {
			last = pathname + 1;
		}
		pathname++;
	}
	return last;
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension( const char *in, char *out ) {
	while ( *in && *in != '.' ) {
		*out++ = *in++;
	}
	*out = 0;
}

void COM_StripFilename( char *in, char *out ) {
	char *end;
	Q_strncpyz( out, in, strlen( in ) );
	end = COM_SkipPath( out );
	*end = 0;
}


/*
==================
COM_DefaultExtension
==================
*/
void COM_DefaultExtension( char *path, int maxSize, const char *extension ) {
	char oldPath[MAX_QPATH];
	char    *src;

//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path + strlen( path ) - 1;

	while ( *src != '/' && src != path ) {
		if ( *src == '.' ) {
			return;                 // it has an extension
		}
		src--;
	}

	Q_strncpyz( oldPath, path, sizeof( oldPath ) );
	Com_sprintf( path, maxSize, "%s%s", oldPath, extension );
}

//============================================================================
/*
==================
COM_BitCheck

  Allows bit-wise checks on arrays with more than one item (> 32 bits)
==================
*/
qboolean COM_BitCheck( const int array[], int bitNum ) {
	int i;

	i = 0;
	while ( bitNum > 31 ) {
		i++;
		bitNum -= 32;
	}

	return ( ( array[i] & ( 1 << bitNum ) ) != 0 );  // (SA) heh, whoops. :)
}

/*
==================
COM_BitSet

  Allows bit-wise SETS on arrays with more than one item (> 32 bits)
==================
*/
void COM_BitSet( int array[], int bitNum ) {
	int i;

	i = 0;
	while ( bitNum > 31 ) {
		i++;
		bitNum -= 32;
	}

	array[i] |= ( 1 << bitNum );
}

/*
==================
COM_BitClear

  Allows bit-wise CLEAR on arrays with more than one item (> 32 bits)
==================
*/
void COM_BitClear( int array[], int bitNum ) {
	int i;

	i = 0;
	while ( bitNum > 31 ) {
		i++;
		bitNum -= 32;
	}

	array[i] &= ~( 1 << bitNum );
}
//============================================================================

/*
============================================================================

                    BYTE ORDER FUNCTIONS

============================================================================
*/

// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places
static short ( *_BigShort )( short l );
static short ( *_LittleShort )( short l );
static int ( *_BigLong )( int l );
static int ( *_LittleLong )( int l );
static qint64 ( *_BigLong64 )( qint64 l );
static qint64 ( *_LittleLong64 )( qint64 l );
static float ( *_BigFloat )( float l );
static float ( *_LittleFloat )( float l );

short   BigShort( short l ) {return _BigShort( l ); }
short   LittleShort( short l ) {return _LittleShort( l ); }
int     BigLong( int l ) {return _BigLong( l ); }
int     LittleLong( int l ) {return _LittleLong( l ); }
qint64  BigLong64( qint64 l ) {return _BigLong64( l ); }
qint64  LittleLong64( qint64 l ) {return _LittleLong64( l ); }
float   BigFloat( float l ) {return _BigFloat( l ); }
float   LittleFloat( float l ) {return _LittleFloat( l ); }

short   ShortSwap( short l ) {
	byte b1,b2;

	b1 = l & 255;
	b2 = ( l >> 8 ) & 255;

	return ( b1 << 8 ) + b2;
}

short   ShortNoSwap( short l ) {
	return l;
}

int    LongSwap( int l ) {
	byte b1,b2,b3,b4;

	b1 = l & 255;
	b2 = ( l >> 8 ) & 255;
	b3 = ( l >> 16 ) & 255;
	b4 = ( l >> 24 ) & 255;

	return ( (int)b1 << 24 ) + ( (int)b2 << 16 ) + ( (int)b3 << 8 ) + b4;
}

int LongNoSwap( int l ) {
	return l;
}

qint64 Long64Swap( qint64 ll ) {
	qint64 result;

	result.b0 = ll.b7;
	result.b1 = ll.b6;
	result.b2 = ll.b5;
	result.b3 = ll.b4;
	result.b4 = ll.b3;
	result.b5 = ll.b2;
	result.b6 = ll.b1;
	result.b7 = ll.b0;

	return result;
}

qint64 Long64NoSwap( qint64 ll ) {
	return ll;
}

float FloatSwap( float f ) {
	union
	{
		float f;
		byte b[4];
	} dat1, dat2;


	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

float FloatNoSwap( float f ) {
	return f;
}

/*
================
Swap_Init
================
*/
void Swap_Init( void ) {
	byte swaptest[2] = {1,0};

// set the byte swapping variables in a portable manner
	if ( *(short *)swaptest == 1 ) {
		_BigShort = ShortSwap;
		_LittleShort = ShortNoSwap;
		_BigLong = LongSwap;
		_LittleLong = LongNoSwap;
		_BigLong64 = Long64Swap;
		_LittleLong64 = Long64NoSwap;
		_BigFloat = FloatSwap;
		_LittleFloat = FloatNoSwap;
	} else
	{
		_BigShort = ShortNoSwap;
		_LittleShort = ShortSwap;
		_BigLong = LongNoSwap;
		_LittleLong = LongSwap;
		_BigLong64 = Long64NoSwap;
		_LittleLong64 = Long64Swap;
		_BigFloat = FloatNoSwap;
		_LittleFloat = FloatSwap;
	}

}


/*
============================================================================

PARSING

============================================================================
*/

static char com_token[MAX_TOKEN_CHARS];
static char com_parsename[MAX_TOKEN_CHARS];
static int com_lines;

static int backup_lines;
static char    *backup_text;

void COM_BeginParseSession( const char *name ) {
	com_lines = 0;
	Com_sprintf( com_parsename, sizeof( com_parsename ), "%s", name );
}

void COM_BackupParseSession( char **data_p ) {
	backup_lines = com_lines;
	backup_text = *data_p;
}

void COM_RestoreParseSession( char **data_p ) {
	com_lines = backup_lines;
	*data_p = backup_text;
}

void COM_SetCurrentParseLine( int line ) {
	com_lines = line;
}

int COM_GetCurrentParseLine( void ) {
	return com_lines;
}

char *COM_Parse( char **data_p ) {
	return COM_ParseExt( data_p, qtrue );
}

void COM_ParseError( char *format, ... ) {
	va_list argptr;
	static char string[4096];

	va_start( argptr, format );
	vsprintf( string, format, argptr );
	va_end( argptr );

	Com_Printf( "ERROR: %s, line %d: %s\n", com_parsename, com_lines, string );
}

void COM_ParseWarning( char *format, ... ) {
	va_list argptr;
	static char string[4096];

	va_start( argptr, format );
	vsprintf( string, format, argptr );
	va_end( argptr );

	Com_Printf( "WARNING: %s, line %d: %s\n", com_parsename, com_lines, string );
}

/*
==============
COM_Parse

Parse a token out of a string
Will never return NULL, just empty strings

If "allowLineBreaks" is qtrue then an empty
string will be returned if the next token is
a newline.
==============
*/
static char *SkipWhitespace( char *data, qboolean *hasNewLines ) {
	int c;

	while ( ( c = *data ) <= ' ' ) {
		if ( !c ) {
			return NULL;
		}
		if ( c == '\n' ) {
			com_lines++;
			*hasNewLines = qtrue;
		}
		data++;
	}

	return data;
}

int COM_Compress( char *data_p ) {
	char *datai, *datao;
	int c, pc, size;
	qboolean ws = qfalse;

	size = 0;
	pc = 0;
	datai = datao = data_p;
	if ( datai ) {
		while ( ( c = *datai ) != 0 ) {
			if ( c == 13 || c == 10 ) {
				*datao = c;
				datao++;
				ws = qfalse;
				pc = c;
				datai++;
				size++;
				// skip double slash comments
			} else if ( c == '/' && datai[1] == '/' ) {
				while ( *datai && *datai != '\n' ) {
					datai++;
				}
				ws = qfalse;
				// skip /* */ comments
			} else if ( c == '/' && datai[1] == '*' ) {
				while ( *datai && ( *datai != '*' || datai[1] != '/' ) )
				{
					datai++;
				}
				if ( *datai ) {
					datai += 2;
				}
				ws = qfalse;
			} else {
				if ( ws ) {
					*datao = ' ';
					datao++;
				}
				*datao = c;
				datao++;
				datai++;
				ws = qfalse;
				pc = c;
				size++;
			}
		}
	}
	*datao = 0;
	return size;
}

char *COM_ParseExt( char **data_p, qboolean allowLineBreaks ) {
	int c = 0, len;
	qboolean hasNewLines = qfalse;
	char *data;

	data = *data_p;
	len = 0;
	com_token[0] = 0;

	// make sure incoming data is valid
	if ( !data ) {
		*data_p = NULL;
		return com_token;
	}

	// RF, backup the session data so we can unget easily
	COM_BackupParseSession( data_p );

	while ( 1 )
	{
		// skip whitespace
		data = SkipWhitespace( data, &hasNewLines );
		if ( !data ) {
			*data_p = NULL;
			return com_token;
		}
		if ( hasNewLines && !allowLineBreaks ) {
			*data_p = data;
			return com_token;
		}

		c = *data;

		// skip double slash comments
		if ( c == '/' && data[1] == '/' ) {
			data += 2;
			while ( *data && *data != '\n' ) {
				data++;
			}
		}
		// skip /* */ comments
		else if ( c == '/' && data[1] == '*' ) {
			data += 2;
			while ( *data && ( *data != '*' || data[1] != '/' ) )
			{
				data++;
			}
			if ( *data ) {
				data += 2;
			}
		} else
		{
			break;
		}
	}

	// handle quoted strings
	if ( c == '\"' ) {
		data++;
		while ( 1 )
		{
			c = *data++;
			if ( c == '\"' || !c ) {
				com_token[len] = 0;
				*data_p = ( char * ) data;
				return com_token;
			}
			if ( len < MAX_TOKEN_CHARS ) {
				com_token[len] = c;
				len++;
			}
		}
	}

	// parse a regular word
	do
	{
		if ( len < MAX_TOKEN_CHARS ) {
			com_token[len] = c;
			len++;
		}
		data++;
		c = *data;
		if ( c == '\n' ) {
			com_lines++;
		}
	} while ( c > 32 );

	if ( len == MAX_TOKEN_CHARS ) {
//		Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
		len = 0;
	}
	com_token[len] = 0;

	*data_p = ( char * ) data;
	return com_token;
}




/*
==================
COM_MatchToken
==================
*/
void COM_MatchToken( char **buf_p, char *match ) {
	char    *token;

	token = COM_Parse( buf_p );
	if ( strcmp( token, match ) ) {
		Com_Error( ERR_DROP, "MatchToken: %s != %s", token, match );
	}
}


/*
=================
SkipBracedSection

The next token should be an open brace.
Skips until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
void SkipBracedSection( char **program ) {
	char            *token;
	int depth;

	depth = 0;
	do {
		token = COM_ParseExt( program, qtrue );
		if ( token[1] == 0 ) {
			if ( token[0] == '{' ) {
				depth++;
			} else if ( token[0] == '}' )    {
				depth--;
			}
		}
	} while ( depth && *program );
}

/*
=================
SkipRestOfLine
=================
*/
void SkipRestOfLine( char **data ) {
	char    *p;
	int c;

	p = *data;
	while ( ( c = *p++ ) != 0 ) {
		if ( c == '\n' ) {
			com_lines++;
			break;
		}
	}

	*data = p;
}


void Parse1DMatrix( char **buf_p, int x, float *m ) {
	char    *token;
	int i;

	COM_MatchToken( buf_p, "(" );

	for ( i = 0 ; i < x ; i++ ) {
		token = COM_Parse( buf_p );
		m[i] = atof( token );
	}

	COM_MatchToken( buf_p, ")" );
}

void Parse2DMatrix( char **buf_p, int y, int x, float *m ) {
	int i;

	COM_MatchToken( buf_p, "(" );

	for ( i = 0 ; i < y ; i++ ) {
		Parse1DMatrix( buf_p, x, m + i * x );
	}

	COM_MatchToken( buf_p, ")" );
}

void Parse3DMatrix( char **buf_p, int z, int y, int x, float *m ) {
	int i;

	COM_MatchToken( buf_p, "(" );

	for ( i = 0 ; i < z ; i++ ) {
		Parse2DMatrix( buf_p, y, x, m + i * x * y );
	}

	COM_MatchToken( buf_p, ")" );
}


/*
============================================================================

                    LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

int Q_isprint( int c ) {
	if ( c >= 0x20 && c <= 0x7E ) {
		return ( 1 );
	}
	return ( 0 );
}

int Q_islower( int c ) {
	if ( c >= 'a' && c <= 'z' ) {
		return ( 1 );
	}
	return ( 0 );
}

int Q_isupper( int c ) {
	if ( c >= 'A' && c <= 'Z' ) {
		return ( 1 );
	}
	return ( 0 );
}

int Q_isalpha( int c ) {
	if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ) {
		return ( 1 );
	}
	return ( 0 );
}

char* Q_strrchr( const char* string, int c ) {
	char cc = c;
	char *s;
	char *sp = (char *)0;

	s = (char*)string;

	while ( *s )
	{
		if ( *s == cc ) {
			sp = s;
		}
		s++;
	}
	if ( cc == 0 ) {
		sp = s;
	}

	return sp;
}

/*
=============
Q_strncpyz

Safe strncpy that ensures a trailing zero
=============
*/
void Q_strncpyz( char *dest, const char *src, int destsize ) {
	if ( !src ) {
		Com_Error( ERR_FATAL, "Q_strncpyz: NULL src" );
	}
	if ( destsize < 1 ) {
		Com_Error( ERR_FATAL,"Q_strncpyz: destsize < 1" );
	}

	strncpy( dest, src, destsize - 1 );
	dest[destsize - 1] = 0;
}

int Q_stricmpn( const char *s1, const char *s2, int n ) {
	int c1, c2;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if ( !n-- ) {
			return 0;       // strings are equal until end point
		}

		if ( c1 != c2 ) {
			if ( c1 >= 'a' && c1 <= 'z' ) {
				c1 -= ( 'a' - 'A' );
			}
			if ( c2 >= 'a' && c2 <= 'z' ) {
				c2 -= ( 'a' - 'A' );
			}
			if ( c1 != c2 ) {
				return c1 < c2 ? -1 : 1;
			}
		}
	} while ( c1 );

	return 0;       // strings are equal
}

int Q_strncmp( const char *s1, const char *s2, int n ) {
	int c1, c2;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if ( !n-- ) {
			return 0;       // strings are equal until end point
		}

		if ( c1 != c2 ) {
			return c1 < c2 ? -1 : 1;
		}
	} while ( c1 );

	return 0;       // strings are equal
}

int Q_stricmp( const char *s1, const char *s2 ) {
	return ( s1 && s2 ) ? Q_stricmpn( s1, s2, 99999 ) : -1;
}


char *Q_strlwr( char *s1 ) {
	char    *s;

	s = s1;
	while ( *s ) {
		*s = tolower( *s );
		s++;
	}
	return s1;
}

char *Q_strupr( char *s1 ) {
	char    *s;

	s = s1;
	while ( *s ) {
		*s = toupper( *s );
		s++;
	}
	return s1;
}


// never goes past bounds or leaves without a terminating 0
void Q_strcat( char *dest, int size, const char *src ) {
	int l1;

	l1 = strlen( dest );
	if ( l1 >= size ) {
		Com_Error( ERR_FATAL, "Q_strcat: already overflowed" );
	}
	Q_strncpyz( dest + l1, src, size - l1 );
}


int Q_PrintStrlen( const char *string ) {
	int len;
	const char  *p;

	if ( !string ) {
		return 0;
	}

	len = 0;
	p = string;
	while ( *p ) {
		if ( Q_IsColorString( p ) ) {
			p += 2;
			continue;
		}
		p++;
		len++;
	}

	return len;
}


char *Q_CleanStr( char *string ) {
	char*   d;
	char*   s;
	int c;

	s = string;
	d = string;
	while ( ( c = *s ) != 0 ) {
		if ( Q_IsColorString( s ) ) {
			s++;
		} else if ( c >= 0x20 && c <= 0x7E )   {
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

void CleanseString( char *in, char *out, qboolean fToLower ) {
	while ( *in ) {
		if ( *in == 27 || *in == '^' ) {
			in++;       // skip color code
			if ( *in ) {
				in++;
			}
			continue;
		}

		if ( *in < 32 ) {
			in++;
			continue;
		}

		*out++ = ( fToLower ) ? tolower( *in++ ) : *in++;
	}

	*out = 0;
}


void QDECL Com_sprintf( char *dest, int size, const char *fmt, ... ) {
	int len;
	va_list argptr;
	char bigbuffer[32000];      // big, but small enough to fit in PPC stack

	va_start( argptr,fmt );
	len = vsprintf( bigbuffer,fmt,argptr );
	va_end( argptr );
	if ( len >= sizeof( bigbuffer ) ) {
		Com_Error( ERR_FATAL, "Com_sprintf: overflowed bigbuffer" );
	}
	if ( len >= size ) {
		Com_Printf( "Com_sprintf: overflow of %i in %i\n", len, size );
	}
	Q_strncpyz( dest, bigbuffer, size );
}

// Ridah, ripped from l_bsp.c
int Q_strncasecmp( char *s1, char *s2, int n ) {
	int c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if ( !n-- ) {
			return 0;       // strings are equal until end point

		}
		if ( c1 != c2 ) {
			if ( c1 >= 'a' && c1 <= 'z' ) {
				c1 -= ( 'a' - 'A' );
			}
			if ( c2 >= 'a' && c2 <= 'z' ) {
				c2 -= ( 'a' - 'A' );
			}
			if ( c1 != c2 ) {
				return -1;      // strings not equal
			}
		}
	} while ( c1 );

	return 0;       // strings are equal
}

int Q_strcasecmp( char *s1, char *s2 ) {
	return Q_strncasecmp( s1, s2, 99999 );
}
// done.

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday

Ridah, modified this into a circular list, to further prevent stepping on
previous strings
============
*/
char    * QDECL va( char *format, ... ) {
	va_list argptr;
	#define MAX_VA_STRING   32000
	static char temp_buffer[MAX_VA_STRING];
	static char string[MAX_VA_STRING];      // in case va is called by nested functions
	static int index = 0;
	char    *buf;
	int len;


	va_start( argptr, format );
	vsprintf( temp_buffer, format,argptr );
	va_end( argptr );

	if ( ( len = strlen( temp_buffer ) ) >= MAX_VA_STRING ) {
		Com_Error( ERR_DROP, "Attempted to overrun string in call to va()\n" );
	}

	if ( len + index >= MAX_VA_STRING - 1 ) {
		index = 0;
	}

	buf = &string[index];
	memcpy( buf, temp_buffer, len + 1 );

	index += len + 1;

	return buf;
}

/*
=============
TempVector

(SA) this is straight out of g_utils.c around line 210

This is just a convenience function
for making temporary vectors for function calls
=============
*/
float   *tv( float x, float y, float z ) {
	static int index;
	static vec3_t vecs[8];
	float   *v;

	// use an array so that multiple tempvectors won't collide
	// for a while
	v = vecs[index];
	index = ( index + 1 ) & 7;

	v[0] = x;
	v[1] = y;
	v[2] = z;

	return v;
}

/*
=====================================================================

  INFO STRINGS

=====================================================================
*/

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
FIXME: overflow check?
===============
*/
char *Info_ValueForKey( const char *s, const char *key ) {
	char pkey[BIG_INFO_KEY];
	static char value[2][BIG_INFO_VALUE];   // use two buffers so compares
	                                        // work without stomping on each other
	static int valueindex = 0;
	char    *o;

	if ( !s || !key ) {
		return "";
	}

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_ValueForKey: oversize infostring" );
	}

	valueindex ^= 1;
	if ( *s == '\\' ) {
		s++;
	}
	while ( 1 )
	{
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s ) {
				return "";
			}
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while ( *s != '\\' && *s )
		{
			*o++ = *s++;
		}
		*o = 0;

		if ( !Q_stricmp( key, pkey ) ) {
			return value[valueindex];
		}

		if ( !*s ) {
			break;
		}
		s++;
	}

	return "";
}


/*
===================
Info_NextPair

Used to itterate through all the key/value pairs in an info string
===================
*/
void Info_NextPair( const char **head, char *key, char *value ) {
	char    *o;
	const char  *s;

	s = *head;

	if ( *s == '\\' ) {
		s++;
	}
	key[0] = 0;
	value[0] = 0;

	o = key;
	while ( *s != '\\' ) {
		if ( !*s ) {
			*o = 0;
			*head = s;
			return;
		}
		*o++ = *s++;
	}
	*o = 0;
	s++;

	o = value;
	while ( *s != '\\' && *s ) {
		*o++ = *s++;
	}
	*o = 0;

	*head = s;
}


/*
===================
Info_RemoveKey
===================
*/
void Info_RemoveKey( char *s, const char *key ) {
	char    *start;
	char pkey[MAX_INFO_KEY];
	char value[MAX_INFO_VALUE];
	char    *o;

	if ( strlen( s ) >= MAX_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_RemoveKey: oversize infostring" );
	}

	if ( strchr( key, '\\' ) ) {
		return;
	}

	while ( 1 )
	{
		start = s;
		if ( *s == '\\' ) {
			s++;
		}
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s ) {
				return;
			}
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while ( *s != '\\' && *s )
		{
			if ( !*s ) {
				return;
			}
			*o++ = *s++;
		}
		*o = 0;

		if ( !strcmp( key, pkey ) ) {
			//strcpy (start, s);	// remove this part
			memmove( start, s, strlen( s ) + 1 );
			return;
		}

		if ( !*s ) {
			return;
		}
	}

}

/*
===================
Info_RemoveKey_Big
===================
*/
void Info_RemoveKey_Big( char *s, const char *key ) {
	char    *start;
	char pkey[BIG_INFO_KEY];
	char value[BIG_INFO_VALUE];
	char    *o;

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_RemoveKey_Big: oversize infostring" );
	}

	if ( strchr( key, '\\' ) ) {
		return;
	}

	while ( 1 )
	{
		start = s;
		if ( *s == '\\' ) {
			s++;
		}
		o = pkey;
		while ( *s != '\\' )
		{
			if ( !*s ) {
				return;
			}
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while ( *s != '\\' && *s )
		{
			if ( !*s ) {
				return;
			}
			*o++ = *s++;
		}
		*o = 0;

		if ( !strcmp( key, pkey ) ) {
			strcpy( start, s );  // remove this part
			return;
		}

		if ( !*s ) {
			return;
		}
	}

}




/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
qboolean Info_Validate( const char *s ) {
	if ( strchr( s, '\"' ) ) {
		return qfalse;
	}
	if ( strchr( s, ';' ) ) {
		return qfalse;
	}
	return qtrue;
}

/*
==================
Info_SetValueForKey

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey( char *s, const char *key, const char *value ) {
	char newi[MAX_INFO_STRING];

	if ( strlen( s ) >= MAX_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
	}

	if ( strchr( key, '\\' ) || strchr( value, '\\' ) ) {
		Com_Printf( "Can't use keys or values with a \\\n" );
		return;
	}

	if ( strchr( key, ';' ) || strchr( value, ';' ) ) {
		Com_Printf( "Can't use keys or values with a semicolon\n" );
		return;
	}

	if ( strchr( key, '\"' ) || strchr( value, '\"' ) ) {
		Com_Printf( "Can't use keys or values with a \"\n" );
		return;
	}

	Info_RemoveKey( s, key );
	if ( !value || !strlen( value ) ) {
		return;
	}

	Com_sprintf( newi, sizeof( newi ), "\\%s\\%s", key, value );

	if ( strlen( newi ) + strlen( s ) > MAX_INFO_STRING ) {
		Com_Printf( "Info string length exceeded\n" );
		return;
	}

	strcat( s, newi );
}

/*
==================
Info_SetValueForKey_Big

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey_Big( char *s, const char *key, const char *value ) {
	char newi[BIG_INFO_STRING];

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
	}

	if ( strchr( key, '\\' ) || strchr( value, '\\' ) ) {
		Com_Printf( "Can't use keys or values with a \\\n" );
		return;
	}

	if ( strchr( key, ';' ) || strchr( value, ';' ) ) {
		Com_Printf( "Can't use keys or values with a semicolon\n" );
		return;
	}

	if ( strchr( key, '\"' ) || strchr( value, '\"' ) ) {
		Com_Printf( "Can't use keys or values with a \"\n" );
		return;
	}

	Info_RemoveKey_Big( s, key );
	if ( !value || !strlen( value ) ) {
		return;
	}

	Com_sprintf( newi, sizeof( newi ), "\\%s\\%s", key, value );

	if ( strlen( newi ) + strlen( s ) > BIG_INFO_STRING ) {
		Com_Printf( "BIG Info string length exceeded\n" );
		return;
	}

	strcat( s, newi );
}

char *Q_StrReplace( char *haystack, char *needle, char *newp ) {  // ETPUB
	static char final[MAX_STRING_CHARS] = {""};
	char dest[MAX_STRING_CHARS] = {""};
	char new[MAX_STRING_CHARS] = {""};
	char *destp;
	int needle_len = 0;
	int new_len = 0;

	if ( !*haystack ) {
		return final;
	}
	if ( !*needle ) {
		Q_strncpyz( final, haystack, sizeof( final ) );
		return final;
	}
	if ( *newp ) {
		Q_strncpyz( new, newp, sizeof( new ) );
	}

	dest[0] = '\0';
	needle_len = strlen( needle );
	new_len = strlen( new );
	destp = &dest[0];
	while ( *haystack ) {
		if ( !Q_stricmpn( haystack, needle, needle_len ) ) {
			Q_strcat( dest, sizeof( dest ), new );
			haystack += needle_len;
			destp += new_len;
			continue;
		}
		if ( MAX_STRING_CHARS > ( strlen( dest ) + 1 ) ) {
			*destp = *haystack;
			*++destp = '\0';
		}
		*haystack++;
	}
	// tjw: don't work with final return value in case haystack
	//      was pointing at it.
	Q_strncpyz( final, dest, sizeof( final ) );
	return final;
}



//====================================================================

vmCvar_t S4ND_GUID;
vmCvar_t cl_guid;
void        trap_Cvar_Set( const char *var_name, const char *value );
void        trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
/*
=============
S4ND_INIT_GUID
Will save the original cl_guid that the game loads to be sent to the server
This could still be hacked, but at least everyone will not have the same GUID
upon reconnecting or changing servers
=============
*/
void S4ND_INIT_GUID() {
//"E518C38465190671035CB7908DEAF2FB"
	char guid[33];
	//char guidTest[33];
	char clPunkbuster[4];
	char defaultGUID[] = "E518C38465190671035CB7908DEAF2FB";

	//Com_Printf( va("Sand:%s       cl:%s\n", S4ND_GUID, cl_guid) );

	// martin - 9/26/10 cl_guid only gets set when PB is turned on
	//if ( (int)trap_Cvar_VariableStringBuffer( "cl_punkbuster" ) == 0 )
	trap_Cvar_VariableStringBuffer( "cl_punkbuster", clPunkbuster, sizeof( clPunkbuster ));

	if ((int)clPunkbuster == 0 )
	{
		Com_Printf("return" );
		return;
	}
	trap_Cvar_VariableStringBuffer( "cl_guid", guid, sizeof( guid ) );
	//Com_Printf("\n\n\n\n\n\n\n%s\n", guid);

	//Com_Printf( va( "^1NEW RUN: Sand:%s       cl:%s\n", S4ND_GUID.string, guid ) );

// DEBUG2016
/*	if (!Q_stricmp( defaultGUID , guid ))
	{
		Com_Printf("^0match\n");
	}else{
		// When in here, want to save off this guid
		Com_Printf("^0NO match\n");
	}
	Com_Printf("Strlen: %i\n", strlen(S4ND_GUID.string));*/

// END DEBUG2016

	// guid is not INIT yet, so return
	if ( ( !guid ) || ( !Q_stricmp( "", guid ) ) ) 
	{
		// if (!strlen(guid))
		//Com_Printf("^4return;\n");
		return;
	}

	// Compare the default value (E518C38465190671035CB7908DEAF2FB) with what is currently stored in cl_guid. 
	// If they don't match (eg game first starts), replace save cl_guid into s4nd_guid.
	// This ensures s4nd_guid has the actual guid.
	if ( ( Q_stricmp( defaultGUID, guid )) && ( strlen(S4ND_GUID.string) == 0 )) 
	{
		//Com_Printf( "^11\n" );
		//Com_Printf( va( "BEFORE - Sand:%s       cl:%s\n", S4ND_GUID.string, guid ) );
		trap_Cvar_Set( "S4ND_GUID", va( "%s", guid ));
		//Com_Printf( va( "AFTER - Sand:%s       cl:%s\n", S4ND_GUID.string, guid ) );
	}

	// If they do match, save off s4nd_guid into cl_guid
	// Scenario occurs after connecting/disconnecting from a server
	else
	{
		//Com_Printf("^22\n");
		//Com_Printf( va( "Sand:%s       cl:%s\n", S4ND_GUID.string, guid ) );
		trap_Cvar_Set( "cl_guid", S4ND_GUID.string);
	}
}