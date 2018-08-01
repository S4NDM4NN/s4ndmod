#include "g_local.h"
#include "S4ND_local.h"

/*
=======================================================================

  SESSION DATA

Session data is the only data that stays persistant across level loads
and tournament restarts.
=======================================================================
*/

/*
-S4NDM4NN-
I realized if i didnt do something i was gonna
be crashing server before long and thats never
good
================
G_WriteClientIP
================
*/
void G_WriteClientIP( gclient_t *client ) {
	const char  *in;
	const char  *ip;

	in = va( "%s %s", client->sess.ip,client->sess.hostname );

	ip = va( "ipof%i", client - level.clients );

	trap_Cvar_Set( ip, in );
}

/*
================
G_ReadIP
================
*/
void G_ReadIP( gclient_t *client ) {
	char s[MAX_STRING_CHARS];
	const char  *var;


	var = va( "ipof%i", client - level.clients );
	trap_Cvar_VariableStringBuffer( var, s, sizeof( s ) );

	sscanf( s, "%s %s",
			&client->sess.ip,
			&client->sess.hostname
			);
}

/*
================
G_WriteClientSessionData

Called on game shutdown
================
*/
void G_WriteClientSessionData( gclient_t *client ) {
	const char  *s;
	const char  *var;

	//		 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
	s = va( "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %s",       // DHM - Nerve
			client->sess.sessionTeam,       // 1
			client->sess.spectatorTime,     // 2
			client->sess.spectatorState,    // 3
			client->sess.spectatorClient,   // 4
			client->sess.wins,              // 5
			client->sess.losses,            // 6
			client->sess.playerType,        // DHM - Nerve 7
			client->sess.playerWeapon,      // DHM - Nerve 8
			client->sess.playerItem,        // DHM - Nerve 9
			client->sess.playerSkin,        // DHM - Nerve 10
			client->sess.spawnObjectiveIndex, // DHM - Nerve 11
			client->sess.latchPlayerType,   // DHM - Nerve 12
			client->sess.latchPlayerWeapon, // DHM - Nerve 13
			client->sess.latchPlayerItem,   // DHM - Nerve 14
			client->sess.latchPlayerSkin,   // DHM - Nerve 15
			client->sess.admin,             // 16
			client->sess.ignored,           // 17
			client->sess.mp40,              // 18
			client->sess.thompson,          // 19
			client->sess.sten,              // 20
			client->sess.speclock,          // 21
			client->sess.isspeclocked,      // 22
			client->sess.gender,            // 23
			client->sess.developer,         // 24
			client->sess.checkban,          // 25
			client->sess.incognito,         // 26
			client->sess.uci,               // 27
			client->sess.guid               // 28

	        //client->sess.autoReload
			);

	var = va( "session%i", client - level.clients );

	trap_Cvar_Set( var, s );
}


/*
================
G_ReadSessionData

Called on a reconnect
================
*/
void G_ReadSessionData( gclient_t *client ) {
	char s[MAX_STRING_CHARS];
	const char  *var;
	qboolean test;

	var = va( "session%i", client - level.clients );
	trap_Cvar_VariableStringBuffer( var, s, sizeof( s ) );

	//		 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
	sscanf( s, "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %s",       // DHM - Nerve
			(int *)&client->sess.sessionTeam,
			&client->sess.spectatorTime,
			(int *)&client->sess.spectatorState,
			&client->sess.spectatorClient,
			&client->sess.wins,
			&client->sess.losses,
			&client->sess.playerType,       // DHM - Nerve
			&client->sess.playerWeapon,     // DHM - Nerve
			&client->sess.playerItem,       // DHM - Nerve
			&client->sess.playerSkin,       // DHM - Nerve
			&client->sess.spawnObjectiveIndex, // DHM - Nerve
			&client->sess.latchPlayerType,  // DHM - Nerve
			&client->sess.latchPlayerWeapon, // DHM - Nerve
			&client->sess.latchPlayerItem,  // DHM - Nerve
			&client->sess.latchPlayerSkin,  // DHM - Nerve

			&client->sess.admin,
			&client->sess.ignored,
			&client->sess.mp40,
			&client->sess.thompson,
			&client->sess.sten,
			&client->sess.speclock,
			&client->sess.isspeclocked,
			&client->sess.gender,
			&client->sess.developer,
			&client->sess.checkban,
			&client->sess.incognito,
			&client->sess.uci, // L0 - mcwf GeoIP
			client->sess.guid


	        //&client->sess.autoReload
			);

	// NERVE - SMF
	if ( g_altStopwatchMode.integer ) {
		test = qtrue;
	} else {
		test = g_currentRound.integer == 1;
	}

	if ( g_gametype.integer == GT_WOLF_STOPWATCH && level.warmupTime > 0 && test ) {
		if ( client->sess.sessionTeam == TEAM_RED ) {
			client->sess.sessionTeam = TEAM_BLUE;
		} else if ( client->sess.sessionTeam == TEAM_BLUE )   {
			client->sess.sessionTeam = TEAM_RED;
		}
		// if teams are being swapped need to swap this!
		if ( client->sess.speclock == 2 ) {
			client->sess.speclock = 1;
		} else if ( client->sess.speclock == 1 ) {
			client->sess.speclock = 2;
		}
	}

	if ( g_swapteams.integer ) {
		trap_Cvar_Set( "g_swapteams", "0" );

		if ( client->sess.sessionTeam == TEAM_RED ) {
			client->sess.sessionTeam = TEAM_BLUE;
		} else if ( client->sess.sessionTeam == TEAM_BLUE )   {
			client->sess.sessionTeam = TEAM_RED;
		}
		// if teams are being swapped need to swap this!
		if ( client->sess.speclock == 2 ) {
			client->sess.speclock = 1;
		} else if ( client->sess.speclock == 1 ) {
			client->sess.speclock = 2;
		}
	}
}


/*
================
G_InitSessionData

Called on a first-time connect
================
*/
void G_InitSessionData( gclient_t *client, char *userinfo ) {
	clientSession_t *sess;
	const char      *value;

	sess = &client->sess;

	// initial team determination
	if ( g_gametype.integer >= GT_TEAM ) {
		// always spawn as spectator in team games
		sess->sessionTeam = TEAM_SPECTATOR;
	} else {
		value = Info_ValueForKey( userinfo, "team" );
		if ( value[0] == 's' ) {
			// a willing spectator, not a waiting-in-line
			sess->sessionTeam = TEAM_SPECTATOR;
		} else {
			switch ( g_gametype.integer ) {
			default:
			case GT_FFA:
			case GT_SINGLE_PLAYER:
				if ( g_maxGameClients.integer > 0 &&
					 level.numNonSpectatorClients >= g_maxGameClients.integer ) {
					sess->sessionTeam = TEAM_SPECTATOR;
				} else {
					sess->sessionTeam = TEAM_FREE;
				}
				break;
			case GT_TOURNAMENT:
				// if the game is full, go into a waiting mode
				if ( level.numNonSpectatorClients >= 2 ) {
					sess->sessionTeam = TEAM_SPECTATOR;
				} else {
					sess->sessionTeam = TEAM_FREE;
				}
				break;
			}
		}
	}

	sess->spectatorState = SPECTATOR_FREE;
	sess->spectatorTime = level.time;

	// DHM - Nerve
	sess->latchPlayerType = sess->playerType = 0;
	sess->latchPlayerWeapon = sess->playerWeapon = 0;
	sess->latchPlayerItem = sess->playerItem = 0;
	sess->latchPlayerSkin = sess->playerSkin = 0;
#ifdef OMNIBOT
	sess->botSuicide = qfalse;
	sess->botSuicidePersist = qfalse;
	sess->botPush = qtrue;
#endif
	sess->spawnObjectiveIndex = 0;
	// dhm - end

	sess->uci = 0;   //L0 - GeoIP

	G_WriteClientSessionData( client );
	G_WriteClientIP( client );
}


/*
==================
G_InitWorldSession

==================
*/
void G_InitWorldSession( void ) {
	char s[MAX_STRING_CHARS];
	int gt;

	trap_Cvar_VariableStringBuffer( "session", s, sizeof( s ) );
	gt = atoi( s );

	// if the gametype changed since the last session, don't use any
	// client sessions
	if ( g_gametype.integer != gt ) {
		level.newSession = qtrue;
		G_Printf( "Gametype changed, clearing session data.\n" );
	}
}

/*
==================
G_WriteSessionData

==================
*/
void G_WriteSessionData( void ) {
	int i;

	trap_Cvar_Set( "session", va( "%i", g_gametype.integer ) );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			G_WriteClientSessionData( &level.clients[i] );
			G_WriteClientIP( &level.clients[i] );
		}
	}
}

