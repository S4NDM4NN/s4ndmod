#include "g_local.h"
#include "S4ND_local.h"
/*
static char *allowedChars[] = {
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "!",
    "@",
    "#",
    "$",
    "%",
    "^",
    "&",
    "*",
    "(",
    ")",
    "_",
    "-",
    "+",
    "=",
    "~",
    "`",
    "q",
    "Q",
    "w",
    "W",
    "e",
    "E",
    "r",
    "R",
    "t",
    "T",
    "y",
    "u",
    "Y",
    "U",
    "i",
    "I",
    "o",
    "O",
    "p",
    "P",
    "[",
    "{",
    "]",
    "}",
//	"\",
    "|",
    "A",
    "a",
    "S",
    "s",
    "D",
    "d",
    "F",
    "f",
    "G",
    "g",
    "H",
    "h",
    "J",
    "j",
    "K",
    "k",
    "L",
    "l",
    ":",
    ";",
    "",
    "'",
    "Z",
    "z",
    "X",
    "x",
    "C",
    "c",
    "V",
    "v",
    "B",
    "b",
    "N",
    "n",
    "M",
    "m",
    ",",
    "<",
    ".",
    ">",
    "?",
    "/",
};*/
/*
ModifyName
Removes Laggy chars from players name
*/
void modifyName( gentity_t *ent ) {
	char name[MAX_NETNAME];
	int i = 0;

	Q_strncpyz( name, ent->client->pers.netname, sizeof( ent->client->pers.netname ) );
	for ( i = 0; i < sizeof( name ); i++ ) {
		if ( ( (int)name[i] >= 32 ) && ( (int)name[i] <= 126 ) ) {
			continue;
			trap_SendServerCommand( ent - g_entities, va( "chat \"Char %c is OK\"", name[i] ) );
		} else {
			name[i] = '.';
			trap_SendServerCommand( ent - g_entities, va( "chat \"Changing char %c\"", name[i] ) );
		}
	}
	Q_strncpyz( ent->client->pers.netname, name, sizeof( ent->client->pers.netname ) );


	//Testing name length
	i = 0;
	while ( name[i] ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"NAME: %c\"", name[i] ) );
		i++;
	}
}

/*
===========
S4ND_nuke
===========
this code kicks players for using any nuke exploit
*/
void S4ND_nuke( gentity_t *ent ) {
	gentity_t *te;
	gentity_t *other;
	char        *value;
	char userinfo[MAX_INFO_STRING];
	char index[MAX_OSPATH];
	int i;

	Q_strncpyz( index, "sound/player/nuke1.wav", sizeof( index ) );
	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		other = &g_entities[i];
		te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( index );
		te->s.teamNum = other->s.clientNum;
	}
	//after the fun stuff, its v55.  I put it up above the kicking so we wouldent have a problemo with maxclients becoming 1 less while its looping :S
	trap_GetUserinfo( ent - g_entities, userinfo, sizeof( userinfo ) );
	value = Info_ValueForKey( userinfo, "ip" );
	Admin_Log( "***VIOLATION: Nuking: %s, IP: %s\n",ent->client->pers.netname, ent->client->sess.ip );
	trap_DropClient( ent - g_entities, "^7Player Kicked: ^3Nuking" );

}
/*
    Checks for minimum name length
*/
qboolean isNameShort( gentity_t *ent ) {
	char cleanName[64];
	int i;

#ifdef OMNIBOT
	if ( !AB_Enable.integer || ent->r.svFlags & SVF_BOT ) {
		return qfalse;
	}
#else
	if ( !AB_Enable.integer ) {
		return qfalse;
	}
#endif
	if ( !AB_MinNameChars.integer ) {
		return qfalse;
	}
	// Sanity checks.  No one wants players with 0 name length
	if ( AB_MinNameChars.integer <= 0 ) {
		trap_Cvar_Set( "AB_MinNameChars", "1" );
	}
	if ( AB_MinNameChars.integer > 6 ) {
		trap_Cvar_Set( "AB_MinNameChars", "6" );
	}

	Q_strncpyz( cleanName, ent->client->pers.netname, sizeof( cleanName ) );
	Q_CleanStr( cleanName );  // Players must have at least AB_MinNameChars english (non ascii) letters

	for ( i = 0; i < AB_MinNameChars.integer; i++ ) {
		if ( cleanName[i] == '\0' ) {
			return qtrue;               // Uh oh.. their name is too short.. kick them
		}
	}

	return qfalse;
}
/*
===========
S4ND_spam
Kicks namespammers
===========
*/
void S4ND_spam( gentity_t *ent ) {
	Admin_Log( "***VIOLATION: Namespam: %s, IP: %s\n",ent->client->pers.netname, ent->client->sess.ip );
	if (sv_punkbuster.integer)
		trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i 0 %s", ent - g_entities +1, "^3Name Spamming" ) );
	else
		trap_DropClient( ent - g_entities, "^7Player Kicked: ^3Name Spamming" );
}

/*
==================
Cheats Kick
==================
*/
void Kick_Cheats( gentity_t *ent ) {
	Admin_Log( "***VIOLATION: Single Player Cheats: %s, IP: %s\n",ent->client->pers.netname, ent->client->sess.ip );
	if (sv_punkbuster.integer)
		trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i 0 %s", ent - g_entities +1, "^3Single Player Cheats Are Not Allowed" ) );
	else
		trap_DropClient( ent - g_entities, "^7Player Kicked: ^3Single Player Cheats Are ^3Not Allowed" );
}

/*
================
check_adminbot
================
*/
void check_adminbot( gentity_t *attacker, gentity_t *self, int meansOfDeath ) {
/*1st lets look how they killed the guy and give them some tkpoints like wab would
then lets check the points and make sure they didnt go over
then lets check their score
if they went over get rid of them
Gonnam make it check score here so it will be checked
unless u are admin or private slot
now lets check their score

========
Jan 19 2007 - Agent
Removed the console lines for player kicked.  Added IP to log kick.

*/
#ifdef OMNIBOT
	if ( !AB_Enable.integer || attacker->r.svFlags & SVF_BOT ) {
		return;
	}
#else
	if ( !AB_Enable.integer ) {
		return;
	}
#endif

// Low score kick
	if ( ( attacker->client->ps.persistant[PERS_SCORE] <= AB_MinScore.integer ) && ( AB_MinScore.integer < 0 ) ) {
		if ( ( !attacker->client->sess.admin ) && ( !attacker->client->sess.developer ) ) 
		{
			if ( sv_punkbuster.integer ) 
				trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i %i %s\n", attacker->client->ps.clientNum+1, AB_TempBanTime.integer, "^3Kicked for Low Negative Score" ) );
			else
			{
				trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %i\n", attacker->client->ps.clientNum, AB_TempBanTime.integer ) );
				trap_DropClient( attacker->client->ps.clientNum, va( "Player Kicked: ^3Low Negative Score" ) );
			}
			Admin_Log( "***VIOLATION: Low Score: %s, IP: %s\n",attacker->client->pers.netname, attacker->client->sess.ip );
		}
	}
	if ( self->client != attacker->client ) {
		if ( ( self->client->ps.persistant[PERS_SCORE] <= AB_MinScore.integer ) && ( AB_MinScore.integer < 0 ) ) {
			if ( ( !self->client->sess.admin ) && ( !self->client->sess.developer ) ) 
			{
				if ( sv_punkbuster.integer ) 
					trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i %i %s\n", attacker->client->ps.clientNum+1, AB_TempBanTime.integer, "^3Kicked for Low Negative Score" ) );
				else
				{
					trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %i\n", attacker->client->ps.clientNum, AB_TempBanTime.integer ) );
					trap_DropClient( attacker->client->ps.clientNum, va( "Player Kicked: ^3Low Negative Score" ) );
				}
				Admin_Log( "***VIOLATION: Low Score: %s, IP: %s\n",self->client->pers.netname, self->client->sess.ip );
			}
		}
	}
// End Low score

//if it was their own grenade lets not check them at all
	if ( attacker->client == self->client ) {
		return;
	}

	if ( meansOfDeath == MOD_KNIFE2 || meansOfDeath == MOD_KNIFE || meansOfDeath == MOD_KNIFE_STEALTH ) {
		//DEFAULT: 50 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_KnifePoints.integer;
	} else if ( meansOfDeath == MOD_LUGER || meansOfDeath == MOD_COLT )     {
		//DEFAULT:50 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_PistolPoints.integer;
	} else if ( meansOfDeath == MOD_MP40 || meansOfDeath == MOD_THOMPSON || meansOfDeath == MOD_STEN )      {
		//DEFAULT: 30 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_MgPoints.integer;
	} else if ( meansOfDeath == MOD_MAUSER )     {
		//DEFAULT: 50 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_MauserPoints.integer;
	} else if ( meansOfDeath == MOD_SNIPERRIFLE )     {
		//DEFAULT: 50 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_SniperPoints.integer;
	} else if ( meansOfDeath == MOD_DYNAMITE_SPLASH || meansOfDeath == MOD_DYNAMITE )     {
		//DEFAULT: 20 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_DynamitePoints.integer;
	} else if ( meansOfDeath == MOD_ROCKET_SPLASH || meansOfDeath == MOD_ROCKET_LAUNCHER )     {
		//DEFAULT: 25 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_PanzerPoints.integer;
	} else if ( meansOfDeath == MOD_GRENADE_PINEAPPLE || meansOfDeath == MOD_GRENADE_SPLASH || meansOfDeath == MOD_GRENADE_LAUNCHER )      {
		//DEFAULT: 25 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_NadePoints.integer;
	} else if ( meansOfDeath == MOD_VENOM )     {
		//DEFAULT: 30 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_VenomPoints.integer;
	} else if ( meansOfDeath == MOD_FLAMETHROWER )     {
		//DEFAULT: 50 chenged this from WAB 'cause this is the way i hate being killed most
		attacker->client->pers.AB_TKpoints += AB_FlamerPoints.integer;
	} else if ( meansOfDeath == MOD_MACHINEGUN )     {
		//DEFAULT: 20 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_Mg42Points.integer;
	} else if ( meansOfDeath == MOD_AIRSTRIKE )     {
		//DEFAULT: 25 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_AsPoints.integer;
	} else if ( meansOfDeath == MOD_ARTILLERY )     {
		//DEFAULT: 25 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_AsPoints.integer;
	} else if ( meansOfDeath == MOD_THROWING_KNIFE )     { //throwing knives
		//DEFAULT: 40 copied values from WAB
		attacker->client->pers.AB_TKpoints += AB_ThrowKnivesPoints.integer;
	}


//warn that they are in danger
	if ( ( ( attacker->client->pers.teamKills + 1 == AB_Tks.integer ) && ( AB_Tks.integer > 1 ) ) || ( ( attacker->client->pers.AB_TKpoints + 20 >= AB_MaxTkPoints.integer ) && ( AB_MaxTkPoints.integer > 20 ) ) ) {
		if ( ( !attacker->client->sess.admin ) && ( !attacker->client->sess.developer ) ) {
			trap_SendServerCommand( attacker - g_entities, va( "chat \"^3AdminBot^7: get kicked on next teamkill!\"", attacker->client->pers.netname ) );
			//trap_SendServerCommand(attacker-g_entities, va("cp \"^3***^1WARNING^3***\n ^7GET KICKED ON NEXT TEAMKILL!\n\"1", self->client->pers.netname ));
		}
	}
//Teamkills
	if ( attacker->client->pers.connected == CON_CONNECTED ) {
		if ( AB_Tks.integer > 0 ) {
			if ( attacker->client->pers.teamKills >= AB_Tks.integer || attacker->client->pers.AB_TKpoints >= AB_MaxTkPoints.integer ) {
				if ( ( !attacker->client->sess.admin ) && ( !attacker->client->sess.developer ) ) 
				{	
					if (sv_punkbuster.integer)
						trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i %i %s\n", attacker->client->ps.clientNum+1, AB_TempBanTime.integer, "^3Teamkilling" ) );
					else
					{
						trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %i\n", attacker->client->ps.clientNum, AB_TempBanTime.integer ) );
						trap_DropClient( attacker->client->ps.clientNum, va( "Player Kicked: ^3Teamkilling" ) );
					}
					Admin_Log( "***VIOLATION: Teamkilling: %s, IP: %s\n",attacker->client->pers.netname, attacker->client->sess.ip );
				}
			}
		}
	}
//Bleeding
	if ( AB_MinHitsPercent.integer ) {
		double teamHitPct = 0;
		if ( attacker->client->pers.acc_hits > 0 ) {
			teamHitPct = ( attacker->client->pers.acc_teamHits / attacker->client->pers.acc_hits ) * ( 100 );
		}
		if ( ( attacker->client->pers.acc_hits >= AB_MinHits.integer ) && ( teamHitPct >= AB_MinHitsPercent.integer ) &&
			 ( !attacker->r.svFlags & SVF_BOT ) ) {	
			if (sv_punkbuster.integer)
				trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i %i %\ns", attacker->client->ps.clientNum+1, AB_TempBanTime.integer, "^3Teamkilling" ) );
			else
			{
				trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %i\n", attacker->client->ps.clientNum, AB_TempBanTime.integer ) );
				trap_DropClient( attacker->client->ps.clientNum, va( "Player Kicked: ^3Teamkilling" ) );
			}
			Admin_Log( "***VIOLATION: Teamkilling: %s, IP: %s\n",attacker->client->pers.netname, attacker->client->sess.ip );

		}
//Teamkilling
		if ( self->client->pers.acc_hits > 0 ) {
			teamHitPct = ( self->client->pers.acc_teamHits / self->client->pers.acc_hits ) * ( 100 );
		}
		if ( ( self->client->pers.acc_hits >= AB_MinHits.integer ) && ( teamHitPct >= AB_MinHitsPercent.integer ) &&
			 ( !self->r.svFlags & SVF_BOT ) ) {
			
			if (sv_punkbuster.integer)
				trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i %i %s\n", attacker->client->ps.clientNum+1, AB_TempBanTime.integer, "^3Teamkilling" ) );
			else
			{
				trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %i\n", attacker->client->ps.clientNum, AB_TempBanTime.integer ) );
				trap_DropClient( attacker->client->ps.clientNum, va( "Player Kicked: ^3Teamkilling" ) );
			}
			Admin_Log( "***VIOLATION: Teamkilling: %s, IP: %s\n",self->client->pers.netname, self->client->sess.ip );
		}
	}
}
/*
================
badUserinfo

Ripped from ETPub
================
*/
qboolean badUserinfo( int clientNum ) {
	char userinfo[MAX_INFO_STRING];
	int length = 0, i, slashCount = 0, count = 0;

// Always check...
	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	length = strlen( userinfo );
	if ( length < 1 ) {
		return qtrue;   // Userinfo too short
	}

	// Dens: userinfo always has to have a leading slash
	if ( userinfo[0] != '\\' ) {
		return qtrue;   // Missing leadingslash in userinfo
	}
	// Dens: the engine always adds ip\ip:port at the end, so there will never
	// be a trailing slash
	if ( userinfo[length - 1] == '\\' ) {
		return qtrue;   // Trailing slash in userinfo
	}

	for ( i = 0; userinfo[i]; i++ ) {
		if ( userinfo[i] == '\\' ) {
			slashCount++;
		}
	}
	if ( slashCount % 2 != 0 ) {
		return qtrue;   // Bad number of slashes in userinfo
	}

	// Dens: make sure there is only one ip, cl_guid, name and cl_punkbuster field
	if ( length > 4 ) {
		for ( i = 0; userinfo[i + 3]; i++ ) {
			if ( userinfo[i] == '\\' && userinfo[i + 1] == 'i' &&
				 userinfo[i + 2] == 'p' && userinfo[i + 3] == '\\' ) {
				count++;
			}
		}
	}
	if ( count > 1 ) {
		return qtrue;   // Too many IP fields
	}
	count = 0;

	if ( sv_punkbuster.integer ) {
		// cl_guid userinfo check
		if ( length > 9 ) {
			for ( i = 0; userinfo[i + 8]; i++ ) {
				if ( userinfo[i] == '\\' && userinfo[i + 1] == 'c' &&
					 userinfo[i + 2] == 'l' && userinfo[i + 3] == '_' &&
					 userinfo[i + 4] == 'g' && userinfo[i + 5] == 'u' &&
					 userinfo[i + 6] == 'i' && userinfo[i + 7] == 'd' &&
					 userinfo[i + 8] == '\\' ) {
					count++;
				}
			}
		}
		if ( count > 1 ) {
			return qtrue;
		}
		count = 0;
	}

	if ( length > 6 ) {
		for ( i = 0; userinfo[i + 5]; i++ ) {
			if ( userinfo[i] == '\\' && userinfo[i + 1] == 'n' &&
				 userinfo[i + 2] == 'a' && userinfo[i + 3] == 'm' &&
				 userinfo[i + 4] == 'e' && userinfo[i + 5] == '\\' ) {
				count++;
			}
		}
	}
	if ( count > 1 ) {
		return qtrue;   // Too many name fields
	}
	count = 0;

	if ( sv_punkbuster.integer ) {
		// cl_punkbuster userinfo check
		if ( length > 15 ) {
			for ( i = 0; userinfo[i + 14]; i++ ) {
				if ( userinfo[i] == '\\' && userinfo[i + 1] == 'c' &&
					 userinfo[i + 2] == 'l' && userinfo[i + 3] == '_' &&
					 userinfo[i + 4] == 'p' && userinfo[i + 5] == 'u' &&
					 userinfo[i + 6] == 'n' && userinfo[i + 7] == 'k' &&
					 userinfo[i + 8] == 'b' && userinfo[i + 9] == 'u' &&
					 userinfo[i + 10] == 's' && userinfo[i + 11] == 't' &&
					 userinfo[i + 12] == 'e' && userinfo[i + 13] == 'r' &&
					 userinfo[i + 14] == '\\' ) {
					count++;
				}
			}
		}
	}
	if ( count > 1 ) {
		return qtrue;
	}

	return qfalse;  // Their userinfo is clean
}


/*
================
hostnameBanCheck

If someone is caught, it automatically adds their IP to the regular ban list so they cannot
attempt to attack the server with fake players.
================
*/
qboolean hostnameBanCheck( int clientNum ) 
{
	FILE	*bannedFile;
	char	bannedHosts[256]; // 255 is the max set by the standard
	char	cleanBannedHosts[256];

	bannedFile = fopen("hostnamebans.txt", "r");
	if (!bannedFile)
	{
		G_Printf("hostnameBanCheck: could not open hostnamebans.txt\n");
		return qfalse;
	}

	while(fgets( bannedHosts, 255, bannedFile) != NULL)
	{
		//bannedHosts like to have a LF character at the end of it
		//fixme - this is an ugly way to clean up that LF
		Q_strncpyz(cleanBannedHosts, bannedHosts, strlen(bannedHosts));
		if (strstr(level.clients[clientNum].sess.hostname, cleanBannedHosts))
		{
			trap_SendConsoleCommand( EXEC_APPEND, va("addban %s\n", level.clients[clientNum].sess.ip ));
			fclose(bannedFile);
			return qtrue;
		}
		memset(cleanBannedHosts, '\0', sizeof(cleanBannedHosts));
		memset(bannedHosts, '\0', sizeof(bannedHosts));
	}
	fclose(bannedFile);
	return qfalse;
}
/*
================
hostnameBanWrite
================
*/
void Svcmd_hostnameBan_f() 
{
	/*FILE	*bannedFile;
	char	bannedHosts[256]; // 255 is the max set by the standard
	char	arg1[MAX_STRING_TOKENS];

	if ( trap_Argc() < 1 ) {
		G_Printf( "Usage: hostnameban <hostname>\n" );
		return;
	}

	trap_Argv( 1, arg1, sizeof( arg1 ) );

	bannedFile = fopen("hostnamebans.txt", "a+");
	if (!bannedFile)
	{
		G_Printf("hostnameBanCheck: could not open hostnamebans.txt\n");
		return;
	}

	while(fgets( bannedHosts, 255, bannedFile) != NULL)
	{
		if (strstr(hostname, bannedHosts))
		{
			Admin_Log( "Attempted to re-ban hostname: %s, but already in ban list!\n", hostname );
			return;
		}
		memset(bannedHosts, '\0', sizeof(bannedHosts));
	}

	Admin_Log( "Banned hostname: %s\n", hostname );
	fputs( va("%s\n", hostname), bannedFile);
	fclose(bannedFile);*/
	return;
}