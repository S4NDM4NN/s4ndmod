


// this file holds commands that can be executed by the server console, but not remote clients

#include "g_local.h"
#include "S4ND_local.h"

#ifdef OMNIBOT
	#include  "g_rtcwbot_interface.h"
#endif


/*
==============================================================================

PACKET FILTERING


You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and any unspecified digits will match any value, so you can specify an entire class C network with "addip 192.246.40".

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

g_filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.


==============================================================================
*/





typedef struct ipFilter_s
{
	unsigned mask;
	unsigned compare;
} ipFilter_t;

#define MAX_IPFILTERS   1024

static ipFilter_t ipFilters[MAX_IPFILTERS];
static int numIPFilters;

/*
=================
StringToFilter
=================
*/
static qboolean StringToFilter( char *s, ipFilter_t *f ) {
	char num[128];
	int i, j;
	byte b[4];
	byte m[4];

	for ( i = 0 ; i < 4 ; i++ )
	{
		b[i] = 0;
		m[i] = 0;
	}

	for ( i = 0 ; i < 4 ; i++ )
	{
		if ( *s < '0' || *s > '9' ) {
			G_Printf( "Bad filter address: %s\n", s );
			return qfalse;
		}

		j = 0;
		while ( *s >= '0' && *s <= '9' )
		{
			num[j++] = *s++;
		}
		num[j] = 0;
		b[i] = atoi( num );
		if ( b[i] != 0 ) {
			m[i] = 255;
		}

		if ( !*s ) {
			break;
		}
		s++;
	}

	f->mask = *(unsigned *)m;
	f->compare = *(unsigned *)b;

	return qtrue;
}

/*
=================
UpdateIPBans
=================
*/
static void UpdateIPBans( void ) {
	byte b[4];
	int i;
	char iplist[MAX_INFO_STRING];

	*iplist = 0;
	for ( i = 0 ; i < numIPFilters ; i++ )
	{
		if ( ipFilters[i].compare == 0xffffffff ) {
			continue;
		}

		*(unsigned *)b = ipFilters[i].compare;
		Com_sprintf( iplist + strlen( iplist ), sizeof( iplist ) - strlen( iplist ),
					 "%i.%i.%i.%i ", b[0], b[1], b[2], b[3] );
	}

	trap_Cvar_Set( "g_banIPs", iplist );
}

/*
=================
G_FilterPacket
=================
*/
qboolean G_FilterPacket( char *from ) {
	int i;
	unsigned in;
	byte m[4];
	char *p;

	i = 0;
	p = from;
	while ( *p && i < 4 ) {
		m[i] = 0;
		while ( *p >= '0' && *p <= '9' ) {
			m[i] = m[i] * 10 + ( *p - '0' );
			p++;
		}
		if ( !*p || *p == ':' ) {
			break;
		}
		i++, p++;
	}

	in = *(unsigned *)m;

	for ( i = 0 ; i < numIPFilters ; i++ )
		if ( ( in & ipFilters[i].mask ) == ipFilters[i].compare ) {
			return g_filterBan.integer != 0;
		}

	return g_filterBan.integer == 0;
}

/*
=================
AddIP
=================
*/
static void AddIP( char *str ) {
	int i;

	for ( i = 0 ; i < numIPFilters ; i++ )
		if ( ipFilters[i].compare == 0xffffffff ) {
			break;
		}               // free spot
	if ( i == numIPFilters ) {
		if ( numIPFilters == MAX_IPFILTERS ) {
			G_Printf( "IP filter list is full\n" );
			return;
		}
		numIPFilters++;
	}

	if ( !StringToFilter( str, &ipFilters[i] ) ) {
		ipFilters[i].compare = 0xffffffffu;
	}

	UpdateIPBans();
}

/*
=================
G_ProcessIPBans
=================
*/
void G_ProcessIPBans( void ) {
	char *s, *t;
	char str[MAX_TOKEN_CHARS];

	Q_strncpyz( str, g_banIPs.string, sizeof( str ) );

	for ( t = s = g_banIPs.string; *t; /* */ ) {
		s = strchr( s, ' ' );
		if ( !s ) {
			break;
		}
		while ( *s == ' ' )
			*s++ = 0;
		if ( *t ) {
			AddIP( t );
		}
		t = s;
	}
}


/*
=================
Svcmd_AddIP_f
=================
*/
void Svcmd_AddIP_f( void ) {
	char str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		G_Printf( "Usage:  addip <ip-mask>\n" );
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	AddIP( str );

}

/*
=================
Svcmd_RemoveIP_f
=================
*/
void Svcmd_RemoveIP_f( void ) {
	ipFilter_t f;
	int i;
	char str[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		G_Printf( "Usage:  sv removeip <ip-mask>\n" );
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	if ( !StringToFilter( str, &f ) ) {
		return;
	}

	for ( i = 0 ; i < numIPFilters ; i++ ) {
		if ( ipFilters[i].mask == f.mask &&
			 ipFilters[i].compare == f.compare ) {
			ipFilters[i].compare = 0xffffffffu;
			G_Printf( "Removed.\n" );

			UpdateIPBans();
			return;
		}
	}

	G_Printf( "Didn't find %s.\n", str );
}

/*
===================
Svcmd_EntityList_f
===================
*/
void    Svcmd_EntityList_f( void ) {
	int e;
	gentity_t       *check;

	check = g_entities + 1;
	for ( e = 1; e < level.num_entities ; e++, check++ ) {
		if ( !check->inuse ) {
			continue;
		}
		G_Printf( "%3i:", e );
		switch ( check->s.eType ) {
		case ET_GENERAL:
			G_Printf( "ET_GENERAL          " );
			break;
		case ET_PLAYER:
			G_Printf( "ET_PLAYER           " );
			break;
		case ET_ITEM:
			G_Printf( "ET_ITEM             " );
			break;
		case ET_MISSILE:
			G_Printf( "ET_MISSILE          " );
			break;
		case ET_MOVER:
			G_Printf( "ET_MOVER            " );
			break;
		case ET_BEAM:
			G_Printf( "ET_BEAM             " );
			break;
		case ET_PORTAL:
			G_Printf( "ET_PORTAL           " );
			break;
		case ET_SPEAKER:
			G_Printf( "ET_SPEAKER          " );
			break;
		case ET_PUSH_TRIGGER:
			G_Printf( "ET_PUSH_TRIGGER     " );
			break;
// JPW NERVE
		case ET_CONCUSSIVE_TRIGGER:
			G_Printf( "ET_CONCUSSIVE_TRIGGR" );
			break;
// jpw
		case ET_TELEPORT_TRIGGER:
			G_Printf( "ET_TELEPORT_TRIGGER " );
			break;
		case ET_INVISIBLE:
			G_Printf( "ET_INVISIBLE        " );
			break;
		case ET_GRAPPLE:
			G_Printf( "ET_GRAPPLE          " );
			break;
		case ET_EXPLOSIVE:
			G_Printf( "ET_EXPLOSIVE        " );
			break;
		case ET_EF_TESLA:
			G_Printf( "ET_EF_TESLA         " );
			break;
		case ET_EF_SPOTLIGHT:
			G_Printf( "ET_EF_SPOTLIGHT     " );
			break;
		case ET_EFFECT3:
			G_Printf( "ET_EFFECT3          " );
			break;
		case ET_ALARMBOX:
			G_Printf( "ET_ALARMBOX          " );
			break;
		default:
			G_Printf( "%3i                 ", check->s.eType );
			break;
		}

		if ( check->classname ) {
			G_Printf( "%s", check->classname );
		}
		G_Printf( "\n" );
	}
}

gclient_t   *ClientForString( const char *s ) {
	gclient_t   *cl;
	int i;
	int idnum;

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			Com_Printf( "Bad client slot: %i\n", idnum );
			return NULL;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			G_Printf( "Client %i is not connected\n", idnum );
			return NULL;
		}
		return cl;
	}

	// check for a name match
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		cl = &level.clients[i];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( !Q_stricmp( cl->pers.netname, s ) ) {
			return cl;
		}
	}

	G_Printf( "User %s is not on the server\n", s );

	return NULL;
}

/*
==================
Svcmd_RotateMap_f
==================
*/
void Svcmd_RotateMap_f( void ) {
	trap_SendConsoleCommand( EXEC_APPEND, "vstr map_rotate\n" );
}
/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
void    Svcmd_ForceTeam_f( void ) {
	gclient_t   *cl;
	char str[MAX_TOKEN_CHARS];

	// find the player
	trap_Argv( 1, str, sizeof( str ) );
	cl = ClientForString( str );
	if ( !cl ) {
		return;
	}

	// set the team
	trap_Argv( 2, str, sizeof( str ) );

	if ( !Q_stricmp( str, "spectator" ) || !Q_stricmp( str, "s" ) ) {
		trap_SendServerCommand( -1, va( "chat \"console: %s ^7was forced to ^3Spectator^7.\n\"", g_entities[cl - level.clients].client->pers.netname ) );
	} else if ( !Q_stricmp( str, "red" ) || !Q_stricmp( str, "r" ) )   {
		trap_SendServerCommand( -1, va( "chat \"console: %s ^7was forced to the ^1Axis^7 team^7.\n\"", g_entities[cl - level.clients].client->pers.netname ) );
	} else if ( !Q_stricmp( str, "blue" ) || !Q_stricmp( str, "b" ) ) {
		trap_SendServerCommand( -1, va( "chat \"console: %s ^7was forced to the ^4Allied ^7team.\n\"", g_entities[cl - level.clients].client->pers.netname ) );
	}

	SetTeam( &g_entities[cl - level.clients], str,qtrue );
}



/*
============
Svcmd_StartMatch_f

NERVE - SMF - starts match if in tournament mode
============
*/
void Svcmd_StartMatch_f() {
	if ( !g_noTeamSwitching.integer ) {
		trap_SendServerCommand( -1, va( "print \"g_noTeamSwitching not activated.\n\"" ) );
		return;
	}

	if ( level.numPlayingClients <= 1 ) {
		trap_SendServerCommand( -1, va( "print \"Not enough playing clients to start match.\n\"" ) );
		return;
	}

	if ( g_gamestate.integer == GS_PLAYING ) {
		trap_SendServerCommand( -1, va( "print \"Match is already in progress.\n\"" ) );
		return;
	}

	if ( g_gamestate.integer == GS_WAITING_FOR_PLAYERS ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
	}
}

/*
============
Svcmd_ResetMatch_f

NERVE - SMF - this has three behaviors
- if not in tournament mode, do a map_restart
- if in tournament mode, go back to waitingForPlayers mode
- if in stopwatch mode, reset back to first round
============
*/
void Svcmd_ResetMatch_f() {
	if ( g_gametype.integer == GT_WOLF_STOPWATCH ) {
		trap_Cvar_Set( "g_currentRound", "0" );
		trap_Cvar_Set( "g_nextTimeLimit", "0" );
	}

	if ( !g_noTeamSwitching.integer || ( g_minGameClients.integer > 1 && level.numPlayingClients >= g_minGameClients.integer ) ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
		return;
	} else {
		trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WAITING_FOR_PLAYERS ) );
		return;
	}
}

/*
============
Svcmd_SwapTeams_f

NERVE - SMF - swaps all clients to opposite team
============
*/
void Svcmd_SwapTeams_f() {
//  if ( g_gamestate.integer != GS_PLAYING ) {
	if ( ( g_gamestate.integer == GS_INITIALIZE ) || // JPW NERVE -- so teams can swap between checkpoint rounds
		 ( g_gamestate.integer == GS_WAITING_FOR_PLAYERS ) ||
		 ( g_gamestate.integer == GS_RESET ) ) {
		trap_SendServerCommand( -1, va( "print \"Match must be in progress to swap teams.\n\"" ) );
		return;
	}

	if ( g_gametype.integer == GT_WOLF_STOPWATCH ) {
		trap_Cvar_Set( "g_currentRound", "0" );
		trap_Cvar_Set( "g_nextTimeLimit", "0" );
	}



	trap_Cvar_Set( "g_swapteams", "1" );
	trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
}



char    *ConcatArgs( int start );

/*
=================
ConsoleCommand

=================
*/
qboolean    ConsoleCommand( void ) {
	char cmd[MAX_TOKEN_CHARS];
//	FILE *f;
//    char s[1000];
	trap_Argv( 0, cmd, sizeof( cmd ) );

	// Ridah, savegame
	if ( Q_stricmp( cmd, "savegame" ) == 0 ) {
		trap_Argv( 1, cmd, sizeof( cmd ) );
		if ( strlen( cmd ) > 0 ) {
			// strip the extension if provided
			if ( strrchr( cmd, '.' ) ) {
				cmd[strrchr( cmd,'.' ) - cmd] = '\0';
			}
//			if (G_SaveGame( cmd ))
//				G_Printf( "Game saved.\n" );
// (SA) change to cp
			//			trap_SendServerCommand(-1, "cp \"Game Saved\n\"");
			//		else
			//			G_Printf( "Unable to save game.\n" );
		} else {    // need a name
			G_Printf( "syntax: savegame <name>\n" );
		}

		return qtrue;
	}
	// done.

	if ( Q_stricmp( cmd, "entitylist" ) == 0 ) {
		Svcmd_EntityList_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "forceteam" ) == 0 ) {
		Svcmd_ForceTeam_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "rotatemap" ) == 0 ) {
		Svcmd_RotateMap_f();
		return qtrue;
	}


	if ( Q_stricmp( cmd, "game_memory" ) == 0 ) {
		Svcmd_GameMem_f();
		return qtrue;
	}

//	if (Q_stricmp (cmd, "addbot") == 0) {
//		Svcmd_AddBot_f();
//		return qtrue;
//	}

	if ( Q_stricmp( cmd, "addip" ) == 0 ) {
		G_Printf( "Please refer to /addban" );
		return qfalse;
	}
//		Svcmd_AddIP_f();
//		return qtrue;
//	}

//	if (Q_stricmp (cmd, "removeip") == 0) {
//		Svcmd_RemoveIP_f();
//		return qtrue;
//	}

	//if (Q_stricmp (cmd, "listip") == 0) {
	//	trap_SendConsoleCommand( EXEC_INSERT, "g_banIPs\n" );
	//	return qtrue;
	//}

	// NERVE - SMF
	if ( Q_stricmp( cmd, "start_match" ) == 0 ) {
		Svcmd_StartMatch_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "reset_match" ) == 0 ) {
		Svcmd_ResetMatch_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "swap_teams" ) == 0 ) {
		Svcmd_SwapTeams_f();
		return qtrue;
	}
	// -NERVE - SMF

	if ( Q_stricmp( cmd, "ignore" ) == 0 ) {
		Svcmd_Ignore_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "test" ) == 0 ) {
		Svcmd_Test_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "headshots_only" ) == 0 ) {
		Svcmd_Headshots_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "unignore" ) == 0 ) {
		Svcmd_Unignore_f();
		return qtrue;
	}
	if ( Q_stricmp( cmd, "lock" ) == 0 ) {
		Cmd_Lock_f();
		return qtrue;
	}
	if ( Q_stricmp( cmd, "unlock" ) == 0 ) {
		Cmd_Unlock_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "kill" ) == 0 ) {
		Svcmd_Kill_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "slap" ) == 0 ) {
		Svcmd_Slap_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "shuffle" ) == 0 ) {
		Svcmd_Shuffle_f();
		Svcmd_Shuffle_f();
		Svcmd_Shuffle_f();
		//let everyone know teams were shuffled
		trap_SendServerCommand( -1, "chat \"console: Teams have been shuffled.\n\"" );
		//do a map_restart
		//trap_SendConsoleCommand(EXEC_APPEND, va("map_restart 0 %i\n", GS_WARMUP));
		trap_SendConsoleCommand( EXEC_APPEND, va( "reset_match\n" ) );
		return qtrue;
	}

	if ( Q_stricmp( cmd, "sortclans" ) == 0 ) {
		Svcmd_SortClan_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "forceclan" ) == 0 ) {
		Svcmd_ForceClan_f();
		return qtrue;
	}
	if ( Q_stricmp( cmd, "addban" ) == 0 ) {
		Svcmd_banip_f();
		return qtrue;
	}
	if ( Q_stricmp( cmd, "addbypass" ) == 0 ) {
		Svcmd_addbypass_f();
		return qtrue;
	}
	if ( Q_stricmp( cmd, "removeban" ) == 0 ) {
		Svcmd_removeban_f();
		return qtrue;
	}
	if ( Q_stricmp( cmd, "removetempban" ) == 0 ) {
		Svcmd_removetempban_f();
		return qtrue;
	}
	if ( Q_stricmp( cmd, "removebypass" ) == 0 ) {
		Svcmd_removebypass_f();
		return qtrue;
	}
	if ( Q_stricmp( cmd, "listbans" ) == 0 ) {
		printList( 3 );
		return qtrue;
	}
	if ( Q_stricmp( cmd, "listtempbans" ) == 0 ) {
		listTempBans();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "listbypasses" ) == 0 ) {
		printList( 4 );
		return qtrue;
	}

	if ( Q_stricmp( cmd, "tempban" ) == 0 ) {
		Svcmd_tempban_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "hostnameban" ) == 0 ) {
//		Svcmd_hostnameBan_f();
		return qtrue;
	}	

	// L0
	if ( Q_stricmp( cmd, "pause" ) == 0 ) {
		Svcmd_Pause();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "unpause" ) == 0 ) {
		Svcmd_UnPause();
		return qtrue;
	} // L0 - end


/*
    if (Q_stricmp(cmd, "bannedips") == 0){
    f=fopen("banned.txt","r");
    if (!f)
    G_Printf("no banned.txt found\n");
    else{
    while (fgets(s,20,f)!=NULL){
      G_Printf("%s",s);
    }
    fclose(f);
    }
    return qtrue;
    }
*/
#ifdef OMNIBOT
	if ( Q_stricmp( cmd, "bot" ) == 0 ) {
		Bot_Interface_ConsoleCommand();
		return qtrue;
	}
#endif
	if ( g_dedicated.integer ) {
		if ( Q_stricmp( cmd, "say" ) == 0 ) {
			trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", ConcatArgs( 1 ) ) );
			return qtrue;
		}

		if ( Q_stricmp( cmd, "cp" ) == 0 ) {
			trap_SendServerCommand( -1, va( "cp \"%s\n\"", ConcatArgs( 1 ) ) );
			return qtrue;
		}
		// everything else will also be printed as a say command
		trap_SendServerCommand( -1, va( "print \"server: %s\n\"", ConcatArgs( 0 ) ) );
		return qtrue;
	}

	return qfalse;
}


