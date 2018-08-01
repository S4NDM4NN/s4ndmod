#ifdef GLOBALSTATS
#include "g_local.h"
#include "S4ND_local.h"


clientStatData statData[MAX_CLIENTS];

/*
===================================
stats_dump

Dumps stats to a text file (Debugging)
===================================
*/
void stats_dump() {
	struct statEntry *node;
	FILE *statsDump;

	statsDump = fopen( "s4ndmod_stats.log", "a+" ); // Linux friendly
	fputs( "\n==== STATS OUTPUT ====\n\n", statsDump );

	while ( statHead != NULL )
	{
		node = statHead;
		fputs( va( "%s\n",node->info ), statsDump );
		statHead = statHead->next;

	}
	fclose( statsDump );
}
/*
===================================
stats_event_weapSwitch

Records time with paticular weapon
===================================
*/
void stats_event_weapSwitch( gentity_t*ent,statWeapons weapon ) {
	int clientNum;

	clientNum = ent->client - level.clients;
	statData[clientNum].weaponTime[weapon] += level.time - statData[clientNum].lastWeapSwitch;
	statData[clientNum].lastWeapSwitch = level.time;
}

/*
===================================
stats_event_rename

Records a new player's name
===================================
*/
void stats_event_rename( gentity_t*ent ) {
	char *newStatEntry;
	int clientNum;

	clientNum = ent->client - level.clients;

	newStatEntry = va( "2\\%i\\%s\\",
					   clientNum,
					   ent->client->pers.netname
					   );

	stats_addEntry( newStatEntry );   // Add it to the linked list
}


/*
==============================
stats_event_clientConnect

Called when a client connects
==============================
*/
void stats_event_clientConnect( gentity_t*ent ) {
	char *newStatEntry; //Hold the generated string

	ent->client->pers.statNum = statNumber++; //assign them a number and incriment it by 1

	// Client is a computer player
	if ( ent->r.svFlags & SVF_BOT ) {
		newStatEntry = va( "3\\%i\\%i\\000.000.000.000\\BOT\\%s\\",
						   ent->client->ps.clientNum,
						   level.time,
						   ent->client->pers.netname );
		//ent->client->sess.guid);	//Bots have a GUID assigned to them
	} else    // Human
	{
		newStatEntry = va( "3\\%i\\%i\\%s\\%s\\%s\\",
						   ent->client->ps.clientNum,
						   level.time,
						   ent->client->sess.ip,
						   ent->client->pers.netname,
						   ent->client->sess.guid );
	}

	stats_addEntry( newStatEntry );   // Add it to the linked list
}
/*
=================================
stats_event_clientDisconnect

Called when a client disconnects
=================================
*/
void stats_event_clientDisconnect( gentity_t*ent ) {
	char *newStatEntry; //Hold the generated string

	stats_event_sendClientData( ent );

	newStatEntry = va( "4\\%i\\%i\\",
					   ent->client->ps.clientNum,
					   level.time );

	stats_addEntry( newStatEntry );   // Add it to the linked list
}



/*
===================================
stats_event_roundBegin

Records info when a round begins
===================================
*/
void stats_event_roundBegin() {
	char *newStatEntry;
	char mapName[MAX_QPATH];    // 64

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	newStatEntry = va( "5\\%i\\%s\\%i\\%i\\%i\\",
					   level.time,
					   mapName,
					   g_gametype.integer,
					   g_timelimit.integer,
					   g_currentRound.integer
					   );
	//CurrentRound = 1 if second half of stopwatch

	stats_addEntry( newStatEntry );   // Add it to the linked list
}

/*
===================================
stats_event_switchTeam

Records info when a round begins
===================================
*/
void stats_event_switchTeam( gentity_t*ent, int team ) {
	char *newStatEntry;
	int clientNum;

	clientNum = ent->client - level.clients;

	newStatEntry = va( "9\\%i\\%i\\%i\\",
					   clientNum,
					   team,
					   level.time
					   );

	stats_addEntry( newStatEntry );   // Add it to the linked list
}


/*
===================================
stats_event_roundEnd

Records time when a round ends
===================================
*/
void stats_event_roundEnd() {
	char *newStatEntry;
	int i;

	for ( i = 0; i < g_maxclients.integer; i++ ) {
		//if they are not connected they are not important
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}

		//if they are not on a team they are not important
		if ( level.clients[i].sess.sessionTeam != TEAM_RED && level.clients[i].sess.sessionTeam != TEAM_BLUE ) {
			continue;
		}

		stats_event_sendClientData( &g_entities[i] );
	}

	newStatEntry = va( "6\\%i\\",
					   level.time );

	stats_addEntry( newStatEntry );   // Add it to the linked list
}

/*
===================================
stats_event_serverUser

Records time with paticular weapon
===================================
*/
void stats_event_serverUser() {
	char *newStatEntry; //Hold the generated string
	char net_ip[18];
	char net_port[10];
	char hostname[64];

	trap_Cvar_VariableStringBuffer( "net_ip", net_ip, sizeof( net_ip ) );
	trap_Cvar_VariableStringBuffer( "net_port", net_port, sizeof( net_port ) );
	trap_Cvar_VariableStringBuffer( "hostname", hostname, sizeof( hostname ) );

	newStatEntry = va( "8\\%s\\%s\\%s\\%s\\%s\\",
					   net_ip,
					   net_port,
					   hostname,
					   g_statsUsername.string,
					   g_statsPassword.string );

	stats_addEntry( newStatEntry );   // Add it to the linked list
}


void stats_event_sendClientData( gentity_t*ent ) {
	int clientNum;
	char newStatEntry[2048]; //should be big enough? too big?
	int i;
	clientStatData *cd; //clientData

	clientNum = ent->client - level.clients;

	cd = &statData[clientNum];

	strcpy( newStatEntry,va( "10\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\%i\\",
							 clientNum,
							 level.time,
							 cd->medPacksCollected,
							 cd->medPacksDistributed,
							 cd->ammoPacksCollected,
							 cd->ammoPacksDistributed,
							 cd->nadePacksCollected,
							 cd->nadePacksDistributed,
							 cd->revivesReceived,
							 cd->objSteals,
							 cd->objReturns,
							 cd->objDestroyed,
							 cd->spawnflagCaptures,
							 cd->spawnflagReclaims,
							 cd->dynaPlanted,
							 cd->dynaDefused,
							 cd->timeAxis,
							 cd->timeAllies,
							 cd->checkpointCaptures,
							 cd->ping,
							 cd->tGibs,
							 cd->gibs,
							 ent->client->ps.persistant[PERS_SCORE]
							 ) );

	for ( i = 0; i < TOTAL_STATS_WEAPONS; i++ ) {
		strcat( newStatEntry, va( "%i %i %i %i %i %i %i %i\\",
								  cd->shotsFired[i],
								  cd->shotsHit[i],
								  cd->headshots[i],
								  cd->dmgGvn[i],
								  cd->dmgRcv[i],
								  cd->tDmgGvn[i],
								  cd->tDmgRcv[i],
								  cd->weaponTime[i] ) );
	}

	stats_addEntry( newStatEntry );
}

/*
=========================
stats_event_revive

Called when a player is
revived.
=========================
*/
void stats_event_revive( gentity_t*ent, gentity_t*target ) {
	int clientNum;

	clientNum = ent->client - level.clients;

	if ( OnSameTeam( ent,target ) ) {
		statData[clientNum].shotsHit[STATS_SYRINGE]++;
	} else {
		statData[clientNum].shotsHit[STATS_POISON]++;
	}

}


/*
=======================
stats_event_pickupPack

Called when anytype of pack is picked up.
Also gives props to the supplier of goods.
=======================
*/
void stats_event_pickupPack( gentity_t*ent, gentity_t*giver, statWeapons weapon ) {
	int clientNum,giverNum;

	clientNum = ent->client - level.clients;

	//medpacks distributed
	if ( giver && giver->client && giver != ent ) { //must be a client, and cant be giving it to them selves!
		giverNum = giver->client - level.clients;   // Martin - Sanity check
		if ( OnSameTeam( ent,giver ) ) {           //must be on same team to get credit!
			switch ( weapon )
			{
			case STATS_MEDKIT:
				statData[clientNum].medPacksCollected++;
				break;
			case STATS_AMMO:
				statData[clientNum].ammoPacksCollected++;
				break;
			case STATS_NADEPACK:
				statData[clientNum].nadePacksCollected++;
				break;

			}
		}

	}

	//med packs collected
	switch ( weapon )
	{
	case STATS_MEDKIT:
		statData[clientNum].medPacksCollected++;
		break;
	case STATS_AMMO:
		statData[clientNum].ammoPacksCollected++;
		break;
	case STATS_NADEPACK:
		statData[clientNum].nadePacksCollected++;
		break;
	}
}



/*
==================================================
stats_even_damageTaken

code was originaly in shotHit,
took it out since sometimes damage is taken when
players are not shooting(goomba, falling, poison)
==================================================
*/
void stats_event_damageTaken( gentity_t*attacker, gentity_t*target,int dmg,meansOfDeath_t mod ) {
	int attackerNum;
	int targetNum;
	int sMod;

	attackerNum = attacker->client - level.clients;
	targetNum = target->client - level.clients;

	if ( mod == MOD_ADMIN_KILL || mod == MOD_ADMIN_SLAP ) { //sorry admins we don't count this against players.
		return;
	}

	sMod = stat_mpMODtoStatsWeap( mod );

	if ( attacker->client && target->client ) {
		if ( OnSameTeam( attacker,target ) ) {
			statData[attackerNum].tDmgGvn[sMod] += dmg;
			statData[targetNum].tDmgRcv[sMod] += dmg;
		} else {
			statData[attackerNum].dmgGvn[sMod] += dmg;
			statData[targetNum].dmgRcv[sMod] += dmg;
		}
	} else {
		if ( attacker->client ) {
			statData[attackerNum].dmgGvn[sMod] += dmg;
		}
		if ( target->client ) {
			statData[targetNum].dmgRcv[sMod] += dmg;
		}
	}
}


/*
=================================
stats_event_shotFired

Called when a player fires a shot
==================================
*/
void stats_event_shotFired( gentity_t*attacker,int weapon ) {
	statWeapons sWeapon;
	int clientNum;

	clientNum = attacker->client - level.clients;

	if ( attacker->s.eFlags & EF_MG42_ACTIVE ) {
		sWeapon = STATS_MG42;
	} else {
		sWeapon = stat_mpWeapToStatsWeap( weapon );
	}

	statData[clientNum].shotsFired[sWeapon]++;

}


/*
=======================
stats_event_shotHit

Called when a player
shoots another player
=======================
*/
void stats_event_shotHit( gentity_t*attacker, gentity_t*target,int mod,int dmg,qboolean headshot ) {
	statWeapons sWeapon;
	int attackerNum;
	int targetNum;

	attackerNum = attacker->client - level.clients;
	targetNum = target->client - level.clients;

	sWeapon = stat_mpMODtoStatsWeap( mod );


	statData[attackerNum].shotsHit[sWeapon]++;

	if ( headshot ) {
		statData[attackerNum].headshots[sWeapon]++;
	}



}

/*
===========================
stats_event_respawn

Called when a player spawns
============================
*/
void stats_event_respawn( gentity_t*ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].lastWeapSwitch = level.time; //need to reset time they switched so we can start our time again
}


/*
==============================
stats_event_die

Called when a player dies
==============================
*/
void stats_event_die( gentity_t*ent ) {
	stats_event_weapSwitch( ent,ent->client->ps.weapon ); //call for weaponswitch
}


/*
=================================
stats_event_kill

Called when a player is killed
=================================
*/
void stats_event_kill( gentity_t*attacker, gentity_t*target,int MOD ) {
	char *newStatEntry;

	if ( MOD == MOD_ADMIN_KILL || MOD == MOD_ADMIN_SLAP ) { //sorry admins we don't count this against players.
		return;
	}

	if ( attacker->client ) {  //player killed player(or self)
		//float test = VectorLength(attacker->client->ps.velocity);
		//vec_t test2 = VectorLength(attacker->client->ps.velocity);
		newStatEntry = va( "1\\%i\\%i\\%s\\%i\\%s\\%i\\%i\\%i\\%s\\%i\\%s\\%i\\",
						   attacker->client->ps.clientNum,
						   attacker->client->sess.sessionTeam,
						   vtos( attacker->client->ps.velocity ),
		                   //VectorLength(attacker->client->ps.velocity),
						   stat_mpMODtoStatsWeap( MOD ),
						   vtos( attacker->client->ps.origin ),
						   attacker->client->ps.stats[STAT_PLAYER_CLASS],
						   target->client->ps.clientNum,
						   target->client->sess.sessionTeam,
						   vtos( target->client->ps.velocity ),
		                   //VectorLength(target->client->ps.velocity),	//Martin - Just a single number instead of x,y,z
						   stat_mpWeapToStatsWeap( target->client->ps.weapon ),
						   vtos( target->client->ps.origin ),
						   target->client->ps.stats[STAT_PLAYER_CLASS] );
	} else    //world killed player
	{
		newStatEntry = va( "1\\-1\\%i\\%i\\%s\\%i\\%s\\%i\\",
						   target->client->ps.clientNum,
						   target->client->sess.sessionTeam,
						   vtos( target->client->ps.velocity ),
		                   //VectorLength(target->client->ps.velocity ),
						   stat_mpWeapToStatsWeap( target->client->ps.weapon ),
						   vtos( target->client->ps.origin ),
						   target->client->ps.stats[STAT_PLAYER_CLASS] );
	}

	stats_addEntry( newStatEntry );   // Add it to the linked list
}


/*
==============================
stats_addEntry

Adds entry into linked list
to be sent to webserver later
==============================
*/
void stats_addEntry( char* entry ) {
	struct statEntry *node;

	//allocate memory space for this
	node = (struct statEntry *)malloc( sizeof( struct statEntry ) );

	strcpy( node->info,entry );

	if ( statHead == NULL ) {
		statHead = node;
	} else {
		statTail->next = node;
	}
	statTail = node;
	node->next = NULL;
}


/*
===========================================
stats_init

for any init. stuff thats needs to be done
===========================================
*/
void stats_init() {
	statNumber = 0; //init statNumber to 0

	//will add begin round to the link list.
	stats_event_roundBegin();
}


/*
==========================
stats_shutdown

Clears/frees linked lists
==========================
*/
void stats_shutdown() {
	struct statEntry *node;

	while ( statHead != NULL )
	{
		node = statHead;
		statHead = statHead->next;
		free( node );
	}
}

/*
==========================
stats_event_objectiveStolen

Records when an objective is stolen
==========================
*/
void stats_event_objectiveStolen( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].objSteals++;
}

/*
==========================
stats_event_objectiveReturned

Records when an objective is returned
==========================
*/
void stats_event_objectiveReturned( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].objReturns++;
}

/*
==========================
stats_event_objectiveDestroyed

Records when an objective is returned
==========================
*/
void stats_event_objectiveDestroyed( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].objDestroyed++;
}

/*
==========================
stats_event_dynamiteDefused

Records when dynamite is defused at an objective
==========================
*/
void stats_event_dynamiteDefused( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].dynaDefused++;
}

/*
==========================
stats_event_dynamitePlanted

Records when dynamite is planted at an objective
==========================
*/
void stats_event_dynamitePlanted( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].dynaPlanted++;
}

/*
==========================
stats_event_spawnflagCaptured

Records when a respawn flag is captured
    Dual flags (depot) are recorded with this.
==========================
*/
void stats_event_spawnflagCaptured( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].spawnflagCaptures++;
}

/*
==========================
stats_event_spawnflagReclaimed

Records when a respawn flag is reclaimed by defenders
==========================
*/
void stats_event_spawnflagReclaimed( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].spawnflagReclaims++;
}

/*
==========================
stats_event_checkpointCaptured

Records when a checkpoint is captured
    *	Does not save reclaims, etc...
    *	Calculating captures / Round or captures / life on the site
==========================
*/
void stats_event_checkpointCaptured( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	statData[clientNum].checkpointCaptures++;
}

/*
==========================
stats_event_wonRound

Records when a player wins the round by himself
    * Transmitts Documents
    * Dynamites Objective
==========================
*/
void stats_event_wonRound( gentity_t *ent ) {
	int clientNum;
	clientNum = ent->client - level.clients;

	//statData[clientNum].spawnflagReclaims++;
}



/*
========================================
stat_mpWeapToStatsWeap

Takes WP_* data to output STATS_* data
========================================
*/
statWeapons stat_mpWeapToStatsWeap( weapon_t weapon ) {
	switch ( weapon ) {
	case WP_MP40:
		return STATS_MP40;
	case WP_THOMPSON:
		return STATS_THOMPSON;
	case WP_STEN:
		return STATS_STEN;
	case WP_MAUSER:
		return STATS_MAUSER;
	case WP_SNIPERRIFLE:
		return STATS_SNIPERRIFLE;
	case WP_FLAMETHROWER:
		return STATS_FLAMETHROWER;
	case WP_PANZERFAUST:
		return STATS_PANZERFAUST;
	case WP_VENOM:
		return STATS_VENOM;
	case WP_GRENADE_LAUNCHER:
	case WP_GRENADE_PINEAPPLE:
		return STATS_GRENADE;
	case WP_LUGER:
		return STATS_LUGER;
	case WP_COLT:
		return STATS_COLT;
	case WP_KNIFE:
	case WP_KNIFE2:
		return STATS_KNIFE;
	case WP_ARTY:
		return STATS_ARTILLERY;
	case WP_SMOKE_GRENADE:
		return STATS_AIRSTRIKE_CANISTER;
	case WP_MEDIC_SYRINGE:
		return STATS_SYRINGE;
	case WP_DYNAMITE:
	case WP_DYNAMITE2:
		return STATS_DYNAMITE;
	case WP_AMMO:
		return STATS_AMMO;
	case WP_MEDKIT:
		return STATS_MEDKIT;
	default:
		return STATS_NONE;
	}
}


/*
========================================
stat_mpMODtoStatsWeap

Takes MOD_* data to output STATS_* data
========================================
*/
statWeapons stat_mpMODtoStatsWeap( meansOfDeath_t mod ) {
	switch ( mod ) {
	case MOD_MP40:
		return STATS_MP40;
	case MOD_THOMPSON:
		return STATS_THOMPSON;
	case MOD_STEN:
		return STATS_STEN;
	case MOD_MAUSER:
		return STATS_MAUSER;
	case MOD_SNIPERRIFLE:
		return STATS_SNIPERRIFLE;
	case MOD_FLAMETHROWER:
		return STATS_FLAMETHROWER;
	case MOD_ROCKET:
	case MOD_ROCKET_LAUNCHER:
	case MOD_ROCKET_SPLASH:
		return STATS_PANZERFAUST;
	case MOD_VENOM:
		return STATS_VENOM;
	case MOD_GRENADE_LAUNCHER:
	case MOD_GRENADE_SPLASH:
	case MOD_GRENADE_PINEAPPLE:
	case MOD_GRENADE:
		return STATS_GRENADE;
	case MOD_LUGER:
		return STATS_LUGER;
	case MOD_COLT:
		return STATS_COLT;
	case MOD_MACHINEGUN:
		return STATS_MG42;
	case MOD_DYNAMITE:
	case MOD_DYNAMITE_SPLASH:
		return STATS_DYNAMITE;
	case MOD_KNIFE2:
	case MOD_KNIFE:
	case MOD_KNIFE_STEALTH:
		return STATS_KNIFE;
	case MOD_THROWING_KNIFE:
		return STATS_THROWING_KNIFE;
	case MOD_AIRSTRIKE:
		return STATS_AIRSTRIKE;
	case MOD_ARTILLERY:
		return STATS_ARTILLERY;
	case MOD_AIRSTRIKE_CANISTER:
		return STATS_AIRSTRIKE_CANISTER;
	case MOD_POISON_SYRINGE:
		return STATS_POISON;
	case MOD_LANDMINE:
		return STATS_MINES;
	case MOD_GOOMBA:
		return STATS_GOOMBA;
	case MOD_FALLING:
		return STATS_FALLING;
	case MOD_MORTAR:
	case MOD_MORTAR_SPLASH:
		return STATS_MORTAR;
	case MOD_SUICIDE_GIB:
	case MOD_SUICIDE_KILL:
		return STATS_SUICIDE;
	case MOD_WATER:
		return STATS_DROWN;

	default:
		return STATS_NONE;
	}
}

#endif
