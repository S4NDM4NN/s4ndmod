#include "g_local.h"
#include "S4ND_local.h"
/*
 * name:		S4ND_Server.c
 *
 * desc:	Put things there that are automatically set on the server
 *

*/

/*
===============
S4ND_Punkbuster
===============
*/
//void S4ND_Punkbuster(gentity_t *ent) {
//}

/*
===============
S4ND_General

Server cvars are automatically set here.
Only things like sv_fps, etc should be set here,
things that will make it smoother on all servers.
===============
*/

void S4ND_General( gentity_t *ent ) {
	//char Modname[32];
//	Modname = GAMEVERSION;

	trap_Cvar_Set( "sv_fps", "20" );            //	Make math smooth
	trap_Cvar_Set( "sv_maxping", "0" );         //	Dont block out because ping to master server is weird (like me @ school :(
	trap_Cvar_Set( "sv_minping", "0" );         //	Why would u ever have this on unless ur a noob admin.
//	trap_Cvar_Set( "sv_maxrate", "25000" );		// removed so we can screw around on other servers and make it like 1000 :)



/*
if (GAMEVERSION != 'S' &&
'4' &&
'N' &&
'D' &&
'M' &&
'o' &&
'D' &&
' ' &&
'2' &&
'.' &&
'2' &&
'.' &&
'0'){
}

     if (Modname[0] == 'S' &&
         Modname[1] == '4' &&
         Modname[2] == 'N' &&
         Modname[3] == 'D' &&
         Modname[4] == 'M' &&
         Modname[5] == 'o' &&
         Modname[6] == 'D' &&
         Modname[7] == ' ' &&
         Modname[8] == '2' &&
         Modname[9] == '.' &&
         Modname[10]== '2'&&
         Modname[11]== '.'&&
         Modname[12]== '0');
*/
}
