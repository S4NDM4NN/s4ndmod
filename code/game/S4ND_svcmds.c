#include "g_local.h"
#include "S4ND_local.h"

void Svcmd_removeban_f() {

	char arg1[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );

	if ( !strlen( arg1 ) ) {
		G_Printf( "removeban usage: /removeban <part of ip>\n^1*BE CARFUL*^3 This will look for your search string and delete all ranges containing it!" );
		return;
	}
	findAndRemoveNode( arg1,3 );
	rewriteBans();
}

void Svcmd_removetempban_f() {
	char arg1[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );

	if ( !strlen( arg1 ) ) {
		G_Printf( "removetempban usage: /removetempban <part of ip>\n^1*BE CARFUL*^3 This will look for your search string and delete all bans containing it!" );
		return;
	}

	findAndRemoveNode( arg1,5 );
	rewriteTempbans();
}

void Svcmd_removebypass_f() {

	char arg1[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );

	if ( !strlen( arg1 ) ) {
		G_Printf( "removebypass usage: /removebypass <part of ip>\n^1*BE CARFUL*^3 This will look for your search string and delete all ranges containing it!" );
		return;
	}
	findAndRemoveNode( arg1,4 );
	rewriteBans();
}

void Svcmd_addbypass_f() {
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];

	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );


	if ( !strlen( arg1 ) || !strlen( arg2 ) ) {
		G_Printf( "addbypass usage: /addbypass <ip range> <password>\n" );
		return;
	}

	appendNode( va( "%s",arg1 ),va( "%s",arg2 ),NULL,4 );
	G_LogPrintf( "A ban bypass was created for IP range: %s\n", arg1 );

	rewriteBans();

}

/*
==========
BANIP
==========
*/

void Svcmd_banip_f() {
//FILE		*bannedfile;
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if ( !strlen( arg1 ) ) {
		G_Printf( "banip usage:\n-Ban Single ip: /banip <ip>\n-Ban Range:     /banip <from ip> <to ip>\n" );
		return;
	}

	if ( strlen( arg2 ) ) {
		appendNode( va( "%s-%s",arg1,arg2 ),NULL,NULL,3 );
		G_LogPrintf( "%s-%s was added to the banned ip list\n", arg1,arg2 );
	} else {
		appendNode( va( "%s-%s",arg1,arg1 ),NULL,NULL,3 );
		G_LogPrintf( "%s was added to the banned ip list\n", arg1 );
	}

	rewriteBans();
}


void rewrite_banned( void ) {
	FILE        *bannedfile;
	FILE        *tempfile;
	char tempbannedips[1000];


	bannedfile = fopen( "banned.txt","w+" );
	tempfile = fopen( "bannedtemp.txt","r+" );

	while ( fgets( tempbannedips,20,tempfile ) != NULL )
	{
		fputs( tempbannedips,bannedfile );

	}
	fclose( bannedfile );
	fclose( tempfile );

	remove( "bannedtemp.txt" );
}

void write_bannedtemp( const char ip[18] ) {

	FILE        *bannedfile;
	FILE        *tempfile;
	char bannedips[18];

	bannedfile = fopen( "banned.txt","r+" );
	if ( bannedfile ) {
		tempfile = fopen( "bannedtemp.txt","w+" );
		while ( fgets( bannedips,18,bannedfile ) != NULL )
		{
			if ( Q_stricmp( bannedips, ip ) ) {
				fputs( bannedips,tempfile );
			}
		}
		fclose( bannedfile );
		fclose( tempfile );
		rewrite_banned();
	}
}
/*
==========
REMOVEIP
==========
*/
void Svcmd_Unbanip_f( void ) {
//char	ip[18];
	char arg1[MAX_STRING_TOKENS];

	trap_Argv( 1, arg1, sizeof( arg1 ) );

	write_bannedtemp( va( "%s\n",arg1 ) );

}

/*
============
Svcmd_Test_f
============
*/

void Svcmd_Test_f() {

	return;
}


/*///////////////////////////////////////////////////
Svcmd_Ignore_f
A command to allow admins to effectively silence annoying
players, while still allowing them to play.  An ignored
client cannot use any text chats or voice chats, cannot call
votes, and cannot send private messages.  This command is
also implemented through a vote, so that players can vote to
ignore an annoying player even when the admin isn't on the server
*/                                                                                                                                                                                                                                                                                                                                                                                                                                                      ///////////////////////////////////////////////////
void Svcmd_Ignore_f() {
	int clientNum;
	char buf[5];

	if ( trap_Argc() != 2 ) {
		G_Printf( "Usage: ignore <clientnum>\n" );
		return;
	}

	trap_Argv( 1, buf, sizeof( buf ) );
	clientNum = atoi( buf );

	if ( ( clientNum < 0 ) || ( clientNum >= MAX_CLIENTS ) ) {
		G_Printf( "Invalid client number\n" );
		return;
	}

	if ( ( !g_entities[clientNum].client ) || ( level.clients[clientNum].pers.connected != CON_CONNECTED ) ) {
		G_Printf( "Client not on server.\n" );
		return;
	}

	g_entities[clientNum].client->sess.ignored = qtrue;     //mark them as ignored
	trap_SendServerCommand( clientNum, "cp \"^3You are now ^1ignored\n\"" );  //and tell them, just so they know
	trap_SendServerCommand( -1, va( "print \"^7%s ^7has been ignored.\n\"",g_entities[clientNum].client->pers.netname ) );
}






/*
=================
Lock
=================
*/
void Cmd_Lock_f() {
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );


	if ( !Q_stricmp( arg1,"axis" ) ) {
		if ( g_lockteams.integer == 1 || g_lockteams.integer == 3 ) {
			return;
		}
		trap_Cvar_Set( "g_lockteams", "1" );
		trap_SendServerCommand( -1, va( "chat \"console^7: ^7locked the ^1Axis^7 team.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"1" ) ) {
		if ( g_lockteams.integer == 1 || g_lockteams.integer == 3 ) {
			return;
		}
		trap_Cvar_Set( "g_lockteams", "1" );
		trap_SendServerCommand( -1, va( "chat \"console^7: ^7locked the ^1Axis^7 team.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"allies" ) ) {
		if ( g_lockteams.integer == 2 || g_lockteams.integer == 3 ) {
			return;
		}
		trap_Cvar_Set( "g_lockteams", "2" );
		trap_SendServerCommand( -1, va( "chat \"console^7: ^7locked the ^4Allies^7 team.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"2" ) ) {
		if ( g_lockteams.integer == 2 || g_lockteams.integer == 3 ) {
			return;
		}
		trap_Cvar_Set( "g_lockteams", "2" );
		trap_SendServerCommand( -1, va( "chat \"console^7: ^7locked the ^4Allies^7 team.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"both" ) ) {
		if ( g_lockteams.integer == 3 ) {
			return;
		}
		trap_Cvar_Set( "g_lockteams", "3" );
		trap_SendServerCommand( -1, va( "chat \"console^7: ^7locked the ^1Axis^7 and ^4Allied^7 teams.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"3" ) ) {
		if ( g_lockteams.integer == 3 ) {
			return;
		}
		trap_Cvar_Set( "g_lockteams", "3" );
		trap_SendServerCommand( -1, va( "chat \"console^7: ^7locked the ^1Axis^7 and ^4Allied^7 teams.\n\"" ) );
	}
}


/*
=================
Unlock
=================
*/
void Cmd_Unlock_f() {
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );


	if ( !Q_stricmp( arg1,"axis" ) ) {
		if ( g_lockteams.integer != 1 && g_lockteams.integer != 3 ) {
			return;
		}
		if ( g_lockteams.integer == 3 ) {
			trap_Cvar_Set( "g_lockteams", "2" );
		}
		if ( g_lockteams.integer == 1 ) {
			trap_Cvar_Set( "g_lockteams", "0" );
		}
		trap_SendServerCommand( -1, va( "chat \"console^7: ^7Unlocked the ^1Axis^7 team.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"1" ) ) {
		if ( g_lockteams.integer != 1 && g_lockteams.integer != 3 ) {
			return;
		}
		if ( g_lockteams.integer == 3 ) {
			trap_Cvar_Set( "g_lockteams", "2" );
		}
		if ( g_lockteams.integer == 1 ) {
			trap_Cvar_Set( "g_lockteams", "0" );
		}
		trap_SendServerCommand( -1, va( "chat \"console^7: ^7Unlocked the ^1Axis^7 team.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"allies" ) ) {
		if ( g_lockteams.integer != 2 && g_lockteams.integer != 3 ) {
			return;
		}

		if ( g_lockteams.integer == 3 ) {
			trap_Cvar_Set( "g_lockteams", "1" );
		}

		if ( g_lockteams.integer == 2 ) {
			trap_Cvar_Set( "g_lockteams", "0" );
		}

		trap_SendServerCommand( -1, va( "chat \"console^7: ^7Unlocked the ^4Allies^7 team.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"2" ) ) {
		if ( g_lockteams.integer != 2 && g_lockteams.integer != 3 ) {
			return;
		}
		if ( g_lockteams.integer == 3 ) {
			trap_Cvar_Set( "g_lockteams", "1" );
		}
		if ( g_lockteams.integer == 1 ) {
			trap_Cvar_Set( "g_lockteams", "0" );
		}
		trap_SendServerCommand( -1, va( "chat \"^7console^7: ^7Unlocked the ^4Allies^7 team.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"both" ) ) {
		if ( g_lockteams.integer != 3 ) {
			return;
		}
		trap_Cvar_Set( "g_lockteams", "0" );
		trap_SendServerCommand( -1, va( "chat \"^7console^7: ^7Unlocked the ^1Axis^7 and ^4Allied^7 teams.\n\"" ) );
	}
	if ( !Q_stricmp( arg1,"3" ) ) {
		if ( g_lockteams.integer != 3 ) {
			return;
		}
		trap_Cvar_Set( "g_lockteams", "0" );
		trap_SendServerCommand( -1, va( "chat \"^7console^7: ^7Unlocked the ^1Axis^7 and ^4Allied^7 teams.\n\"" ) );
	}
}


/*///////////////////////////////////////////////////
Svcmd_Unignore_f
This command simply removes the 'ignore' placed on a
player previously.  It is also implemented as a client
vote.
*/                                                                                                                                                                                               ///////////////////////////////////////////////////
void Svcmd_Unignore_f() {
	int clientNum;
	char buf[5];

	if ( trap_Argc() != 2 ) {
		G_Printf( "Usage: unignore <clientnum>\n" );
		return;
	}

	trap_Argv( 1, buf, sizeof( buf ) );
	clientNum = atoi( buf );

	if ( ( clientNum < 0 ) || ( clientNum >= MAX_CLIENTS ) ) {
		G_Printf( "Invalid client number.\n" );
		return;
	}

	if ( ( !g_entities[clientNum].client ) || ( level.clients[clientNum].pers.connected != CON_CONNECTED ) ) {
		G_Printf( "Client not on server.\n" );
		return;
	}

	g_entities[clientNum].client->sess.ignored = qfalse;
	trap_SendServerCommand( clientNum, "cp \"^3You have been ^2unignored\n\"" );  //let them know they can talk again
	trap_SendServerCommand( -1, va( "print \"^7%s ^7has been un-ignored.\n\"",g_entities[clientNum].client->pers.netname ) );
}
/*
=================
kill
=================
*/
void Svcmd_Kill_f() {

	int clientNum;
	char buf[5];

	if ( trap_Argc() != 2 ) {
		G_Printf( "Usage: kill <clientnum>\n" );
		return;
	}

	trap_Argv( 1, buf, sizeof( buf ) );
	clientNum = atoi( buf );

	if ( ( clientNum < 0 ) || ( clientNum >= MAX_CLIENTS ) ) {
		G_Printf( "Invalid client number.\n" );
		return;
	}

	if ( ( !g_entities[clientNum].client ) || ( level.clients[clientNum].pers.connected != CON_CONNECTED ) ) {
		G_Printf( "Client not on server.\n" );
		return;
	}


	if ( g_entities[clientNum].client->ps.stats[STAT_HEALTH] > 0 ) {
		g_entities[clientNum].client->ps.persistant[PERS_HWEAPON_USE] = 0;
		G_Damage( &g_entities[clientNum], NULL, NULL, NULL, NULL, 1000000, 99999, MOD_ADMIN_KILL );
		trap_SendServerCommand( clientNum, "chat \"console: ^3You were killed by the admin.\n\"" );
	}

}


/*
=================
slap
=================
*/
void Svcmd_Slap_f() {
	int clientNum;
	char buf[5];

	if ( trap_Argc() != 2 ) {
		G_Printf( "Usage: slap <clientnum>\n" );
		return;
	}

	trap_Argv( 1, buf, sizeof( buf ) );
	clientNum = atoi( buf );

	if ( ( clientNum < 0 ) || ( clientNum >= MAX_CLIENTS ) ) {
		G_Printf( "Invalid client number.\n" );
		return;
	}

	if ( ( !g_entities[clientNum].client ) || ( level.clients[clientNum].pers.connected != CON_CONNECTED ) ) {
		G_Printf( "Client not on server.\n" );
		return;
	}


	if ( g_entities[clientNum].client->ps.stats[STAT_HEALTH] > 0 ) {
		g_entities[clientNum].client->ps.persistant[PERS_HWEAPON_USE] = 0;
		G_Damage( &g_entities[clientNum], NULL, NULL, NULL, NULL, 25, 0, MOD_ADMIN_SLAP );
		trap_SendServerCommand( clientNum, "chat \"console: ^3You were slapped by the admin.\n\"" );
		G_Sound( &g_entities[clientNum], G_SoundIndex( "sound/props/throw/chairthudgrunt.wav" ) );
	}

}

/*
================
Svcmd_tempban_f
================
*/
char*ConcatArgs( int start );
void Svcmd_tempban_f( void ) {
	int amountBanned;
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	time_t seconds;

	time( &seconds );

	if ( trap_Argc() < 2 ) {
		G_Printf( "Usage: tempban <ip> <mins> <other info>\n" );
		return;
	}

	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	amountBanned = atoi( arg2 );

	appendNode( arg1,va( "%i",seconds + ( amountBanned * 60 ) ),ConcatArgs( 3 ),5 );
	G_Printf( "Tempban added for %i mins, for ip: %s\n",amountBanned,arg1 );
	rewriteTempbans();
}







/*
=================
Headshots Only
=================
*/
void Svcmd_Headshots_f() {

	if ( g_headshotsOnly.integer != 0 ) {
		trap_SendConsoleCommand( EXEC_APPEND, "g_headshotsonly 0\n" );
		trap_SendServerCommand( -1, "chat \"console: ^3Headshots only has been ^1disabled\n\"" );
	} else {
		trap_SendConsoleCommand( EXEC_APPEND, "g_headshotsonly 1\n" );
		trap_SendServerCommand( -1, "chat \"console: ^3Headshots only has been ^2enabled\n\"" );
	}

}

/*
=================
shuffle
=================
*/
void Svcmd_Shuffle_f() {
	int count = 0, tmpCount, i;
	int players[MAX_CLIENTS];

	memset( players, -1, sizeof( players ) );

	if ( g_gamestate.integer == GS_RESET ) {
		return;
	}

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		//skip client numbers that aren't used
		if ( ( !g_entities[i].inuse ) || ( level.clients[i].pers.connected != CON_CONNECTED ) ) {
			continue;
		}

		//ignore spectators
		if ( ( level.clients[i].sess.sessionTeam != TEAM_RED ) && ( level.clients[i].sess.sessionTeam != TEAM_BLUE ) ) {
			continue;
		}

		players[count] = i;
		count++;
	}

	tmpCount = count;   //copy the number of active clients

	//loop through all the active players
	for ( i = 0; i < count; i++ )
	{
		int j;

		do {
			j = ( rand() % count );
		} while ( players[j] == -1 );

		//put every other random choice on allies
		if ( i & 1 ) {
			level.clients[players[j]].sess.sessionTeam = TEAM_BLUE;
		} else {
			level.clients[players[j]].sess.sessionTeam = TEAM_RED;
		}


		ClientUserinfoChanged( players[j] );
		ClientBegin( players[j] );


		players[j] = players[tmpCount - 1];
		players[tmpCount - 1] = -1;
		tmpCount--;
	}

}

/*
=================
SortClan
=================
*/
void Svcmd_SortClan_f() {
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	int allieteam[MAX_CLIENTS];
	int axisteam[MAX_CLIENTS];
	int alcount = 0;
	int axcount = 0;
	int i;


	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if ( g_gamestate.integer == GS_RESET ) {
		return;
	}


	for ( i = 0 ; i < level.maxclients ; i++ ) {
		//skip client numbers that aren't used
		if ( ( !g_entities[i].inuse ) || ( level.clients[i].pers.connected != CON_CONNECTED ) ) {
			continue;
		}

		level.clients[i].sess.sessionTeam = TEAM_SPECTATOR;
	}
	axcount = ClientNumberFromNameMatch( arg1, axisteam );
	alcount = ClientNumberFromNameMatch( arg2, allieteam );

	for ( i = 0; i < axcount; i++ ) {
		g_entities[axisteam[i]].client->sess.sessionTeam = TEAM_RED;
	}

	for ( i = 0; i < alcount; i++ ) {
		g_entities[allieteam[i]].client->sess.sessionTeam = TEAM_BLUE;
	}

	//let everyone know teams were sorted
	trap_SendServerCommand( -1, va( "chat \"console: ^3Clans ^1%s ^3and ^4%s ^3have been sorted.\n\"",arg1, arg2 ) );
	//do a map_restart
	trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
}

/*
=================
ForceClan
=================
*/
void Svcmd_ForceClan_f() {
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	int team[MAX_CLIENTS];
	int count = 0;
	int i;

	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	if ( g_gamestate.integer == GS_RESET ) {
		return;
	}


	for ( i = 0; i < level.maxclients; i++ ) {
		//skip client numbers that aren't used
		if ( ( !g_entities[i].inuse ) || ( level.clients[i].pers.connected != CON_CONNECTED ) ) {
			continue;
		}
		//ignore spectators
		if ( ( level.clients[i].sess.sessionTeam != TEAM_RED ) && ( level.clients[i].sess.sessionTeam != TEAM_BLUE ) ) {
			continue;
		}
		if ( !Q_stricmp( arg2,"allies" ) || !Q_stricmp( arg2,"b" ) || !Q_stricmp( arg2,"blue" ) ) {
			level.clients[i].sess.sessionTeam = TEAM_RED;
		}
		if ( !Q_stricmp( arg2,"axis" ) || !Q_stricmp( arg2,"r" ) || !Q_stricmp( arg2,"red" ) ) {
			level.clients[i].sess.sessionTeam = TEAM_BLUE;
		}
	}

	count = ClientNumberFromNameMatch( arg1, team );
	for ( i = 0; i < count; i++ ) {
		if ( !Q_stricmp( arg2,"allies" ) || !Q_stricmp( arg2,"b" ) || !Q_stricmp( arg2,"blue" ) ) {
			g_entities[team[i]].client->sess.sessionTeam = TEAM_BLUE;
		}
		if ( !Q_stricmp( arg2,"axis" ) || !Q_stricmp( arg2,"r" ) || !Q_stricmp( arg2,"red" ) ) {
			g_entities[team[i]].client->sess.sessionTeam = TEAM_RED;
		}
	}
	//let everyone know teams were sorted
	trap_SendServerCommand( -1, va( "chat \"console: ^3Clan ^5%s ^3has been forced to ^5%s\n\"",arg1, arg2 ) );
	//do a map_restart
	trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
}

// L0
/*==========
 Pause match
==========*/
void Svcmd_Pause() {

	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	// Just to make sure no1 calls it from console directly..
	if ( level.pendingUnpause > 0 ) {
		return;
	}

	if ( !level.paused ) {
		PauseGame();
		level.pendingUnpause++;
		trap_SendServerCommand( -1, va( "chat \"console: ^3Game has been Paused!\"" ) );
	}

}
/*============
 UnPause match
============*/
void Svcmd_UnPause() {

	if ( level.paused ) {
		trap_SendServerCommand( -1, va( "chat \"console: ^3Game will Resume now.\"" ) );
		level.CNyes = qtrue;
		CountDown();
	}
}
// L0 - end
