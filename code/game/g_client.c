#include "g_local.h"
#include "S4ND_local.h"

#ifdef OMNIBOT
	#include  "g_rtcwbot_interface.h"
#endif
// g_client.c -- client functions that don't happen every frame

// Ridah, new bounding box
//static vec3_t	playerMins = {-15, -15, -24};
//static vec3_t	playerMaxs = {15, 15, 32};
vec3_t playerMins = {-18, -18, -24};
vec3_t playerMaxs = {18, 18, 48};
// done.

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 32) initial
potential spawning position for deathmatch games.
The first time a player enters the game, they will be at an 'initial' spot.
Targets will be fired when someone spawns in on them.
"nobots" will prevent bots from using this spot.
"nohumans" will prevent non-bots from using this spot.
If the start position is targeting an entity, the players camera will start out facing that ent (like an info_notnull)
*/
void SP_info_player_deathmatch( gentity_t *ent ) {
	int i;
	vec3_t dir;

	G_SpawnInt( "nobots", "0", &i );
	if ( i ) {
		ent->flags |= FL_NO_BOTS;
	}
	G_SpawnInt( "nohumans", "0", &i );
	if ( i ) {
		ent->flags |= FL_NO_HUMANS;
	}

	ent->enemy = G_PickTarget( ent->target );
	if ( ent->enemy ) {
		VectorSubtract( ent->enemy->s.origin, ent->s.origin, dir );
		vectoangles( dir, ent->s.angles );
	}

}

//----(SA) added
/*QUAKED info_player_checkpoint (1 0 0) (-16 -16 -24) (16 16 32) a b c d
these are start points /after/ the level start
the letter (a b c d) designates the checkpoint that needs to be complete in order to use this start position
*/
void SP_info_player_checkpoint( gentity_t *ent ) {

	ent->classname = "info_player_checkpoint";
	SP_info_player_deathmatch( ent );

}

//----(SA) end


/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 32)
equivelant to info_player_deathmatch
*/
void SP_info_player_start( gentity_t *ent ) {
	ent->classname = "info_player_deathmatch";
	SP_info_player_deathmatch( ent );
}

/*QUAKED info_player_intermission (1 0 1) (-16 -16 -24) (16 16 32) AXIS ALLIED
The intermission will be viewed from this point.  Target an info_notnull for the view direction.
*/
void SP_info_player_intermission( gentity_t *ent ) {

}



/*
=======================================================================

  SelectSpawnPoint

=======================================================================
*/

/*
================
SpotWouldTelefrag

================
*/
qboolean SpotWouldTelefrag( gentity_t *spot ) {
	int i, num;
	int touch[MAX_GENTITIES];
	gentity_t   *hit;
	vec3_t mins, maxs;

	VectorAdd( spot->s.origin, playerMins, mins );
	VectorAdd( spot->s.origin, playerMaxs, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for ( i = 0 ; i < num ; i++ ) {
		hit = &g_entities[touch[i]];
		if ( hit->client && hit->client->ps.stats[STAT_HEALTH] > 0 ) {
			return qtrue;
		}

	}

	return qfalse;
}

/*
================
SelectNearestDeathmatchSpawnPoint

Find the spot that we DON'T want to use
================
*/
#define MAX_SPAWN_POINTS    128
gentity_t *SelectNearestDeathmatchSpawnPoint( vec3_t from ) {
	gentity_t   *spot;
	vec3_t delta;
	float dist, nearestDist;
	gentity_t   *nearestSpot;

	nearestDist = 999999;
	nearestSpot = NULL;
	spot = NULL;

	while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL ) {

		VectorSubtract( spot->s.origin, from, delta );
		dist = VectorLength( delta );
		if ( dist < nearestDist ) {
			nearestDist = dist;
			nearestSpot = spot;
		}
	}

	return nearestSpot;
}


/*
================
SelectRandomDeathmatchSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define MAX_SPAWN_POINTS    128
gentity_t *SelectRandomDeathmatchSpawnPoint( void ) {
	gentity_t   *spot;
	int count;
	int selection;
	gentity_t   *spots[MAX_SPAWN_POINTS];

	count = 0;
	spot = NULL;

	while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL ) {
		if ( SpotWouldTelefrag( spot ) ) {
			continue;
		}
		spots[ count ] = spot;
		count++;
	}

	if ( !count ) { // no spots that won't telefrag
		return G_Find( NULL, FOFS( classname ), "info_player_deathmatch" );
	}

	selection = rand() % count;
	return spots[ selection ];
}


/*
===========
SelectSpawnPoint

Chooses a player start, deathmatch start, etc
============
*/
gentity_t *SelectSpawnPoint( vec3_t avoidPoint, vec3_t origin, vec3_t angles ) {
	gentity_t   *spot;
	gentity_t   *nearestSpot;

	nearestSpot = SelectNearestDeathmatchSpawnPoint( avoidPoint );

	spot = SelectRandomDeathmatchSpawnPoint();
	if ( spot == nearestSpot ) {
		// roll again if it would be real close to point of death
		spot = SelectRandomDeathmatchSpawnPoint();
		if ( spot == nearestSpot ) {
			// last try
			spot = SelectRandomDeathmatchSpawnPoint();
		}
	}

	// find a single player start spot
	if ( !spot ) {
		G_Error( "Couldn't find a spawn point" );
	}

	VectorCopy( spot->s.origin, origin );
	origin[2] += 9;
	VectorCopy( spot->s.angles, angles );

	return spot;
}

/*
===========
SelectInitialSpawnPoint

Try to find a spawn point marked 'initial', otherwise
use normal spawn selection.
============
*/
gentity_t *SelectInitialSpawnPoint( vec3_t origin, vec3_t angles ) {
	gentity_t   *spot;

	spot = NULL;
	while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_deathmatch" ) ) != NULL ) {
		if ( spot->spawnflags & 1 ) {
			break;
		}
	}

	if ( !spot || SpotWouldTelefrag( spot ) ) {
		return SelectSpawnPoint( vec3_origin, origin, angles );
	}

	VectorCopy( spot->s.origin, origin );
	origin[2] += 9;
	VectorCopy( spot->s.angles, angles );

	return spot;
}

/*
===========
SelectSpectatorSpawnPoint

============
*/
gentity_t *SelectSpectatorSpawnPoint( vec3_t origin, vec3_t angles ) {
	FindIntermissionPoint();

	VectorCopy( level.intermission_origin, origin );
	VectorCopy( level.intermission_angle, angles );

	return NULL;
}

/*
=======================================================================

BODYQUE

=======================================================================
*/

/*
===============
InitBodyQue
===============
*/
void InitBodyQue( void ) {
	int i;
	gentity_t   *ent;

	level.bodyQueIndex = 0;
	for ( i = 0; i < BODY_QUEUE_SIZE ; i++ ) {
		ent = G_Spawn();
		ent->classname = "bodyque";
		ent->neverFree = qtrue;
		level.bodyQue[i] = ent;
	}
}

/*
=============
BodySink

After sitting around for five seconds, fall into the ground and dissapear
=============
*/
void BodySink( gentity_t *ent ) {
	if ( level.time - ent->timestamp > 6500 ) {
		//6500 was previous
		// the body ques are never actually freed, they are just unlinked
		trap_UnlinkEntity( ent );
		ent->physicsObject = qfalse;
		return;
	}
	ent->nextthink = level.time + 100;
	ent->s.pos.trBase[2] -= 1;
}

/*
=============
CopyToBodyQue

A player is respawning, so make an entity that looks
just like the existing corpse to leave behind.
=============
*/
void CopyToBodyQue( gentity_t *ent ) {
	gentity_t       *body;
	int contents, i;

	trap_UnlinkEntity( ent );

	// if client is in a nodrop area, don't leave the body
	contents = trap_PointContents( ent->s.origin, -1 );
	if ( contents & CONTENTS_NODROP ) {
		return;
	}

	// grab a body que and cycle to the next one
	body = level.bodyQue[ level.bodyQueIndex ];
	level.bodyQueIndex = ( level.bodyQueIndex + 1 ) % BODY_QUEUE_SIZE;

//crapshoot: this can really be commented out ...
#ifndef OMNIBOT
	trap_UnlinkEntity( body );
#endif

	body->s = ent->s;
	body->s.eFlags = EF_DEAD;       // clear EF_TALK, etc

	if ( ent->client->ps.eFlags & EF_HEADSHOT ) {
		body->s.eFlags |= EF_HEADSHOT;          // make sure the dead body draws no head (if killed that way)

	}
	body->s.powerups = 0;   // clear powerups
	body->s.loopSound = 0;  // clear lava burning
	body->s.number = body - g_entities;
	body->timestamp = level.time;
	body->physicsObject = qtrue;
	body->physicsBounce = 0;        // don't bounce
	if ( body->s.groundEntityNum == ENTITYNUM_NONE ) {
		body->s.pos.trType = TR_GRAVITY;
		body->s.pos.trTime = level.time;
		VectorCopy( ent->client->ps.velocity, body->s.pos.trDelta );
	} else {
		body->s.pos.trType = TR_STATIONARY;
	}
	body->s.event = 0;

	// DHM - Clear out event system
	for ( i = 0; i < MAX_EVENTS; i++ )
		body->s.events[i] = 0;
	body->s.eventSequence = 0;

	// DHM - Nerve
	if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
		// change the animation to the last-frame only, so the sequence
		// doesn't repeat anew for the body
		switch ( body->s.legsAnim & ~ANIM_TOGGLEBIT ) {
		case BOTH_DEATH1:
		case BOTH_DEAD1:
			body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD1;
			break;
		case BOTH_DEATH2:
		case BOTH_DEAD2:
			body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD2;
			break;
		case BOTH_DEATH3:
		case BOTH_DEAD3:
		default:
			body->s.torsoAnim = body->s.legsAnim = BOTH_DEAD3;
			break;
		}
	}
	// dhm

#ifndef OMNIBOT
	body->r.svFlags = ent->r.svFlags;
#else
	body->r.svFlags = ent->r.svFlags & ~SVF_BOT;
#endif //OMNIBOT

	VectorCopy( ent->r.mins, body->r.mins );
	VectorCopy( ent->r.maxs, body->r.maxs );
	VectorCopy( ent->r.absmin, body->r.absmin );
	VectorCopy( ent->r.absmax, body->r.absmax );

	body->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	// DHM - Nerve :: allow bullets to pass through bbox
	body->r.contents = 0;
	body->r.ownerNum = ent->r.ownerNum;

	body->nextthink = level.time + 5000;    //Body sink time
	body->think = BodySink;

	body->die = body_die;

	// don't take more damage if already gibbed
	if ( ent->health <= GIB_HEALTH ) {
		body->takedamage = qfalse;
	} else {
		body->takedamage = qtrue;
	}


	VectorCopy( body->s.pos.trBase, body->r.currentOrigin );
	trap_LinkEntity( body );
}

//======================================================================


/*
==================
SetClientViewAngle

==================
*/
void SetClientViewAngle( gentity_t *ent, vec3_t angle ) {
	int i;

	// set the delta angle
	for ( i = 0 ; i < 3 ; i++ ) {
		int cmdAngle;

		cmdAngle = ANGLE2SHORT( angle[i] );
		ent->client->ps.delta_angles[i] = cmdAngle - ent->client->pers.cmd.angles[i];
	}
	VectorCopy( angle, ent->s.angles );
	VectorCopy( ent->s.angles, ent->client->ps.viewangles );
}

/* JPW NERVE
================
limbo
================
*/
void limbo( gentity_t *ent, qboolean makeCorpse ) {
	int i,contents;
	//int startclient = ent->client->sess.spectatorClient;
	int startclient = ent->client->ps.clientNum;

	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		G_Printf( "FIXME: limbo called from single player game.  Shouldn't see this\n" );
		return;
	}
	if ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) {

		// DHM - Nerve :: First save off persistant info we'll need for respawn
		for ( i = 0; i < MAX_PERSISTANT; i++ )
			ent->client->saved_persistant[i] = ent->client->ps.persistant[i];
		// dhm

		ent->client->ps.pm_flags |= PMF_LIMBO;

#ifdef OMNIBOT
		// bots should not follow, causes problems
		if ( !ent->r.svFlags & SVF_BOT ) {
			ent->client->ps.pm_flags |= PMF_FOLLOW;
		}

		if ( !ent->client->sess.botSuicidePersist || g_gamestate.integer != GS_PLAYING ) {
			ent->client->sess.botSuicide = qfalse;
		}
#else
		ent->client->ps.pm_flags |= PMF_FOLLOW;
#endif // OMNIBOT

		if ( makeCorpse ) {
			CopyToBodyQue( ent ); // make a nice looking corpse
		} else {
			trap_UnlinkEntity( ent );
		}

		// DHM - Nerve :: reset these values
		ent->client->ps.viewlocked = 0;
		ent->client->ps.viewlocked_entNum = 0;

		ent->r.maxs[2] = 0;
		ent->r.currentOrigin[2] += 8;
		contents = trap_PointContents( ent->r.currentOrigin, -1 ); // drop stuff
		ent->s.weapon = ent->client->limboDropWeapon; // stored in player_die()
		if ( makeCorpse && !( contents & CONTENTS_NODROP ) ) {
			TossClientItems( ent );
		}

#ifdef OMNIBOT
		//bots can't follow
		if ( ent->r.svFlags & SVF_BOT ) {
			ent->client->sess.spectatorClient = ent->client->ps.clientNum;
			ent->client->sess.spectatorState = SPECTATOR_FREE;
		} else
		{
			ent->client->sess.spectatorClient = startclient;
			Cmd_FollowCycle_f( ent,1 ); // get fresh spectatorClient

			if ( ent->client->sess.spectatorClient == startclient ) {
				// No one to follow, so just stay put
				ent->client->sess.spectatorState = SPECTATOR_FREE;
			} else {
				ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
			}
		}
#else
		ent->client->sess.spectatorClient = startclient;
		Cmd_FollowCycle_f( ent,1 ); // get fresh spectatorClient

		if ( ent->client->sess.spectatorClient == startclient ) {
			// No one to follow, so just stay put
			ent->client->sess.spectatorState = SPECTATOR_FREE;
		} else {
			ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
		}
#endif // OMNIBOT

//		ClientUserinfoChanged( ent->client - level.clients );		// NERVE - SMF - don't do this
		if ( ent->client->sess.sessionTeam == TEAM_RED ) {
			ent->client->deployQueueNumber = level.redNumWaiting;
			level.redNumWaiting++;
		} else if ( ent->client->sess.sessionTeam == TEAM_BLUE )     {
			ent->client->deployQueueNumber = level.blueNumWaiting;
			level.blueNumWaiting++;
		}

#ifdef OMNIBOT
		// one last anal check..really don't want them following
		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if ( ( &g_entities[i] )->r.svFlags & SVF_BOT ) {
				continue;
			}
		}
#endif // OMNIBOT

		for ( i = 0 ; i < level.maxclients ; i++ ) {
			if ( level.clients[i].ps.pm_flags & PMF_LIMBO
				 && level.clients[i].sess.spectatorClient == ent->s.number ) {
				Cmd_FollowCycle_f( &g_entities[i], 1 );
			}
		}
	}
}

/* JPW NERVE
================
reinforce
================
// -- called when time expires for a team deployment cycle and there is at least one guy ready to go
*/
void reinforce( gentity_t *ent ) {
	int p, team; // numDeployable=0, finished=0; // TTimo unused
	char *classname;
	gclient_t *rclient;


	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		G_Printf( "FIXME: reinforce called from single player game.  Shouldn't see this\n" );
		return;
	}
	if ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) {
		G_Printf( "player already deployed, skipping\n" );
		return;
	}
	// get team to deploy from passed entity

	team = ent->client->sess.sessionTeam;



	// find number active team spawnpoints
	if ( team == TEAM_RED ) {
		classname = "team_CTF_redspawn";
	} else if ( team == TEAM_BLUE ) {
		classname = "team_CTF_bluespawn"; // team_CTF_blueflag // blueplayer //bluespawn //team_WOLF_checkpoint//team_WOLF_objective//info_player_checkpoint// deathmatch//intermission//start
	} else {
		assert( 0 );
	}

	// DHM - Nerve :: restore persistant data now that we're out of Limbo
	rclient = ent->client;
	for ( p = 0; p < MAX_PERSISTANT; p++ )
		rclient->ps.persistant[p] = rclient->saved_persistant[p];
	// dhm

	respawn( ent );
}
// jpw


/*
================
respawn
================
*/
void respawn( gentity_t *ent ) {

	ent->client->ps.pm_flags &= ~PMF_LIMBO; // JPW NERVE turns off limbo

	// DHM - Nerve :: Decrease the number of respawns left
	if ( g_gamestate.integer == GS_PLAYING ) {
		if ( g_maxlives.integer > 0 && ent->client->ps.persistant[PERS_RESPAWNS_LEFT] > 0 ) {
			ent->client->ps.persistant[PERS_RESPAWNS_LEFT]--;
		}
	}

	G_DPrintf( "Respawning %s, %i lives left\n", ent->client->pers.netname, ent->client->ps.persistant[PERS_RESPAWNS_LEFT] );

	// DHM - Nerve :: Already handled in 'limbo()'
	if ( g_gametype.integer < GT_WOLF ) {
		CopyToBodyQue( ent );
	}

	ClientSpawn( ent, qfalse );

	//ANTILAG - reset the origin trails
	G_ResetTrail( ent );
	ent->client->saved.leveltime = 0;



//Begin Agent respawn sound
	//only play if we are during a match ;)
	/*
	if(g_gamestate.integer == GS_PLAYING){
	te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
	te->s.eventParm = G_SoundIndex("sound/multiplayer/respawn.wav");
	te->s.teamNum = ent->s.clientNum;
	}*/                                                                                                                                                                                                                        //JULY 23, 2007		this is really annoying its getting /deleted now
//End Agent

	// DHM - Nerve :: Add back if we decide to have a spawn effect
	// add a teleportation effect
//	tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
//	tent->s.clientNum = ent->s.clientNum;
}

// NERVE - SMF - merge from team arena
/*
================
TeamCount

Returns number of players on a team
================
*/
team_t TeamCount( int ignoreClientNum, int team ) {
	int i;
	int count = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( i == ignoreClientNum ) {
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == team ) {
			count++;
		}
	}

	return count;
}
// -NERVE - SMF

/*
================
PickTeam

================
*/
team_t PickTeam( int ignoreClientNum ) {
	int i;
	int counts[TEAM_NUM_TEAMS];

	memset( counts, 0, sizeof( counts ) );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( i == ignoreClientNum ) {
			continue;
		}
		if ( level.clients[i].pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
			counts[TEAM_BLUE]++;
		} else if ( level.clients[i].sess.sessionTeam == TEAM_RED )   {
			counts[TEAM_RED]++;
		}
	}

	if ( counts[TEAM_BLUE] > counts[TEAM_RED] ) {
		return TEAM_RED;
	}
	if ( counts[TEAM_RED] > counts[TEAM_BLUE] ) {
		return TEAM_BLUE;
	}
	// equal team count, so join the team with the lowest score
	if ( level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED] ) {
		return TEAM_RED;
	}
	return TEAM_BLUE;
}

/*
===========
ForceClientSkin

Forces a client's skin (for teamplay)
===========
*/
void ForceClientSkin( gclient_t *client, char *model, const char *skin ) {
	char *p;

	if ( ( p = strchr( model, '/' ) ) != NULL ) {
		*p = 0;
	}

	Q_strcat( model, MAX_QPATH, "/" );
	Q_strcat( model, MAX_QPATH, skin );
}

// NERVE - SMF
/*
===========
SetWolfUserVars
===========
*/
void SetWolfUserVars( gentity_t *ent, char *wolfinfo ) {
	gclient_t *client;
	int mask, team;

	client = ent->client;
	if ( !client ) {
		return;
	}

	// check if we have a valid snapshot
	mask = MP_TEAM_MASK;
	team = ( client->pers.cmd.mpSetup & mask ) >> MP_TEAM_OFFSET;

	if ( !team ) {
		return;
	}

	// set player class
	mask = MP_CLASS_MASK;
	client->sess.latchPlayerType = ( client->pers.cmd.mpSetup & mask ) >> MP_CLASS_OFFSET;

	// set weapon
	mask = MP_WEAPON_MASK;
	client->sess.latchPlayerWeapon = ( client->pers.cmd.mpSetup & mask ) >> MP_WEAPON_OFFSET;
}

// -NERVE - SMF


// DHM - Nerve
/*
===========
SetWolfSkin

Forces a client's skin (for Wolfenstein teamplay)
===========
*/

#define MULTIPLAYER_ALLIEDMODEL "multi"
#define MULTIPLAYER_AXISMODEL   "multi_axis"
//S4NDM4NN-edited this to work with spy
void SetWolfSkin( gclient_t *client, char *model ) {

	switch ( client->sess.sessionTeam ) {
	case TEAM_RED:
		if ( !client->ps.isSpy ) {
			Q_strcat( model, MAX_QPATH, "red" );
		} else {
			Q_strcat( model, MAX_QPATH, "blue" );
		}
		break;
	case TEAM_BLUE:
		if ( !client->ps.isSpy ) {
			Q_strcat( model, MAX_QPATH, "blue" );
		} else {
			Q_strcat( model, MAX_QPATH, "red" );
		}
		break;
	default:
		Q_strcat( model, MAX_QPATH, "red" );
		break;
	}

	switch ( client->sess.playerType ) {
	case PC_SOLDIER:
		if ( !client->ps.isSpy ) {
			Q_strcat( model, MAX_QPATH, "soldier" );
		} else {
			if ( client->ps.spyType == 0 ) {
				Q_strcat( model, MAX_QPATH, "soldier" );
			} else if ( client->ps.spyType == 1 ) {
				Q_strcat( model, MAX_QPATH, "medic" );
			} else if ( client->ps.spyType == 2 ) {
				Q_strcat( model, MAX_QPATH, "engineer" );
			} else {
				Q_strcat( model, MAX_QPATH, "lieutenant" );
			}
		}
		break;
	case PC_MEDIC:
		if ( !client->ps.isSpy ) {
			Q_strcat( model, MAX_QPATH, "medic" );
		} else {
			if ( client->ps.spyType == 0 ) {
				Q_strcat( model, MAX_QPATH, "soldier" );
			} else if ( client->ps.spyType == 1 ) {
				Q_strcat( model, MAX_QPATH, "medic" );
			} else if ( client->ps.spyType == 2 ) {
				Q_strcat( model, MAX_QPATH, "engineer" );
			} else {
				Q_strcat( model, MAX_QPATH, "lieutenant" );
			}
		}
		break;
	case PC_ENGINEER:
		if ( !client->ps.isSpy ) {
			Q_strcat( model, MAX_QPATH, "engineer" );
		} else {
			if ( client->ps.spyType == 0 ) {
				Q_strcat( model, MAX_QPATH, "soldier" );
			} else if ( client->ps.spyType == 1 ) {
				Q_strcat( model, MAX_QPATH, "medic" );
			} else if ( client->ps.spyType == 2 ) {
				Q_strcat( model, MAX_QPATH, "engineer" );
			} else {
				Q_strcat( model, MAX_QPATH, "lieutenant" );
			}
		}
		break;
	case PC_LT:
		if ( !client->ps.isSpy ) {
			Q_strcat( model, MAX_QPATH, "lieutenant" );
		} else {
			if ( client->ps.spyType == 0 ) {
				Q_strcat( model, MAX_QPATH, "soldier" );
			} else if ( client->ps.spyType == 1 ) {
				Q_strcat( model, MAX_QPATH, "medic" );
			} else if ( client->ps.spyType == 2 ) {
				Q_strcat( model, MAX_QPATH, "engineer" );
			} else {
				Q_strcat( model, MAX_QPATH, "lieutenant" );
			}
		}
		break;
	default:
		if ( !client->ps.isSpy ) {
			Q_strcat( model, MAX_QPATH, "soldier" );
		} else {
			if ( client->ps.spyType == 0 ) {
				Q_strcat( model, MAX_QPATH, "soldier" );
			} else if ( client->ps.spyType == 1 ) {
				Q_strcat( model, MAX_QPATH, "medic" );
			} else if ( client->ps.spyType == 2 ) {
				Q_strcat( model, MAX_QPATH, "engineer" );
			} else {
				Q_strcat( model, MAX_QPATH, "lieutenant" );
			}
		}
		break;
	}

	// DHM - A skinnum will be in the session data soon...
	switch ( client->sess.playerSkin ) {
	case 1:
		Q_strcat( model, MAX_QPATH, "1" );
		break;
	case 2:
		Q_strcat( model, MAX_QPATH, "2" );
		break;
	case 3:
		Q_strcat( model, MAX_QPATH, "3" );
		break;
	default:
		Q_strcat( model, MAX_QPATH, "1" );
		break;
	}
}

void SetWolfSpawnWeapons( gclient_t *client ) {
	int pc = client->sess.playerType;
	int starthealth = 100, i,numMedics = 0;    // JPW NERVE
	qboolean weapGiven = qfalse;
	if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}

#ifdef OMNIBOT
	Bot_Event_ResetWeapons( client->ps.clientNum );
#endif
	client->ps.classWeaponTime = -999999;

	if ( g_deathmatch.integer != 2 ) {
		if ( g_gamestate.integer == GS_PLAYING ) { //S4NDM4NN-you dont need to be invulnarable in warmup makes a slaughter house withough
			client->ps.powerups[PW_INVULNERABLE] = level.time + 3000; // JPW NERVE some time to find cover
		}
	} else {
		client->ps.powerups[PW_INVULNERABLE] = level.time + 1000;
	}



	if ( g_smokingPlayers.integer ) {
		client->ps.eFlags |= EF_SMOKINGBLACK;
	}


	// Communicate it to cgame
	client->ps.stats[STAT_PLAYER_CLASS] = pc;

	// Abuse teamNum to store player class as well (can't see stats for all clients in cgame)
	client->ps.teamNum = pc;

	// JPW NERVE -- zero out all ammo counts
	memset( client->ps.ammo,MAX_WEAPONS,sizeof( int ) );






	if ( g_makeDMspawns.integer ) { //Give them nothing so we dont get confused
		client->ps.weapons[0] = 0;
		client->ps.weapons[1] = 0;
	} else if ( g_panzerWar.integer )    {
		COM_BitSet( client->ps.weapons, WP_PANZERFAUST );
		if ( g_panzerClips.integer > 1 ) {
			client->ps.ammoclip[BG_FindClipForWeapon( WP_PANZERFAUST )] = g_panzerClips.integer;
		}
		client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = g_extraPanzerClips.integer;
		client->ps.weapon = WP_PANZERFAUST;
#ifdef OMNIBOT
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PANZERFAUST ) );
#endif
		//=============	SPAM WAR!
	} else if ( g_SpamWar.integer )    {
		COM_BitSet( client->ps.weapons, WP_KNIFE );
		client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )] = 1;
		client->ps.weapon = WP_KNIFE;
		client->ps.weaponstate = WEAPON_READY;

		COM_BitSet( client->ps.weapons, WP_PANZERFAUST );
		client->ps.ammoclip[BG_FindClipForWeapon( WP_PANZERFAUST )] = 99999;
		client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = 999999;
		client->ps.weapon = WP_PANZERFAUST;

		COM_BitSet( client->ps.weapons, WP_DYNAMITE );
		client->ps.ammo[BG_FindAmmoForWeapon( WP_DYNAMITE )] = 0;
		client->ps.ammoclip[BG_FindClipForWeapon( WP_DYNAMITE )] = 1;

		// NERVE - SMF
		COM_BitSet( client->ps.weapons, WP_PLIERS );
		client->ps.ammoclip[BG_FindClipForWeapon( WP_PLIERS )] = 1;
		client->ps.ammo[WP_PLIERS] = 1;

		client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
		COM_BitSet( client->ps.weapons, WP_ARTY );
		client->ps.ammo[BG_FindAmmoForWeapon( WP_ARTY )] = 0;
		client->ps.ammoclip[BG_FindClipForWeapon( WP_ARTY )] = 1;

		// NERVE - SMF
		COM_BitSet( client->ps.weapons, WP_SMOKE_GRENADE );
		client->ps.ammoclip[BG_FindClipForWeapon( WP_SMOKE_GRENADE )] = 1;
		client->ps.ammo[WP_SMOKE_GRENADE] = 1;

		COM_BitSet( client->ps.weapons, WP_VENOM );
		client->ps.ammoclip[BG_FindAmmoForWeapon( WP_VENOM )] = 99999;
		client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )] = 99999;
		client->ps.weapon = WP_VENOM;

		COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
		client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 99999;

		COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
		client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = 99999;

		COM_BitSet( client->ps.weapons, WP_FLAMETHROWER );
		client->ps.ammoclip[BG_FindAmmoForWeapon( WP_FLAMETHROWER )] = 999999;
		client->ps.weapon = WP_FLAMETHROWER;

#ifdef OMNIBOT
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_KNIFE ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PANZERFAUST ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PLIERS ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SMOKE_GRENADE ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_VENOM ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_LAUNCHER ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FLAMETHROWER ) );

		// sort of a hack. bots need it for ammo table lookup so they can use the call arty goal
		COM_BitSet( client->ps.weapons, WP_BINOCULARS );
		client->ps.ammo[BG_FindAmmoForWeapon( WP_BINOCULARS )] = 0;
		client->ps.ammoclip[BG_FindClipForWeapon( WP_BINOCULARS )] = 1;

		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_BINOCULARS ) );
#endif


	} else if ( g_instaGib.integer )    {
		COM_BitSet( client->ps.weapons, WP_MAUSER );
		client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 10;
		client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 999;
		client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif
		//if mines are enabled give them to everybody in goldrush/instagib
		if ( g_mines.integer ) {
			COM_BitSet( client->ps.weapons, WP_DYNAMITE );
			client->ps.ammo[BG_FindAmmoForWeapon( WP_DYNAMITE )] = 0;
			client->ps.ammoclip[BG_FindClipForWeapon( WP_DYNAMITE )] = 1;

			// NERVE - SMF
			COM_BitSet( client->ps.weapons, WP_PLIERS );
			client->ps.ammoclip[BG_FindClipForWeapon( WP_PLIERS )] = 1;
			client->ps.ammo[WP_PLIERS] = 1;
#ifdef OMNIBOT
			Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_DYNAMITE ) );
			Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PLIERS ) );
#endif
		} else if ( !g_deathmatch.integer && !g_goldrush.integer )     {
			if ( pc == PC_ENGINEER ) {
				COM_BitSet( client->ps.weapons, WP_DYNAMITE );
				client->ps.ammo[BG_FindAmmoForWeapon( WP_DYNAMITE )] = 0;
				client->ps.ammoclip[BG_FindClipForWeapon( WP_DYNAMITE )] = 1;

				// NERVE - SMF
				COM_BitSet( client->ps.weapons, WP_PLIERS );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_PLIERS )] = 1;
				client->ps.ammo[WP_PLIERS] = 1;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_DYNAMITE ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PLIERS ) );
#endif
			}
		}
	} else if ( g_deathmatch.integer == 2 )    {
		client->ps.weapons[0] = 0;
		client->ps.weapons[1] = 0;

		if ( level.endDeathmatch ) {
			client->ps.endDeathmatch++;
		}

		if ( !level.endDeathmatch ) {
			COM_BitSet( client->ps.weapons, WP_KNIFE );

			if ( g_throwableKnives.integer > 0 ) {
				client->pers.throwingKnives = g_throwableKnives.integer;
			}

			client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )] = 1;
			client->ps.weapon = WP_KNIFE;
			client->ps.weaponstate = WEAPON_READY;

			COM_BitSet( client->ps.weapons, WP_COLT );
			client->ps.ammoclip[BG_FindClipForWeapon( WP_COLT )] = 8;
			client->ps.ammo[BG_FindAmmoForWeapon( WP_COLT )] = 24;
			client->ps.weapon = WP_COLT;
#ifdef OMNIBOT
			Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_KNIFE ) );
			Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_COLT ) );
#endif
		} else if ( level.endDeathmatch && ( level.highkiller->client == client ) )       {
			COM_BitSet( client->ps.weapons, WP_PANZERFAUST );
			//give him ammo and lots of it
			client->ps.ammoclip[BG_FindClipForWeapon( WP_PANZERFAUST )] = 1000000;
			client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = 1000000;
			client->ps.powerups[PW_REDFLAG] = INT_MAX;
			client->ps.powerups[PW_QUAD] = INT_MAX;
			//give him some god he deserves it
			level.highkiller->flags |= FL_GODMODE;
			//no fatige is always nice
			level.highkiller->flags |= FL_NOFATIGUE;

#ifdef OMNIBOT
			Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PANZERFAUST ) );
#endif


		}
	} 
// Sniper War ================================================================================================
	else if ( g_sniperWar.integer )    
	{
		client->ps.weapons[0] = 0;
		client->ps.weapons[1] = 0;
		COM_BitSet( client->ps.weapons, WP_KNIFE );

		client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )] = 1;
		client->ps.weapon = WP_KNIFE;
		client->ps.weaponstate = WEAPON_READY;

#ifdef OMNIBOT
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_KNIFE ) );
#endif
		switch ( client->sess.playerWeapon )
		{
			case 7: //FG42
				COM_BitSet( client->ps.weapons, WP_FG42SCOPE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_FG42SCOPE )] = 20;
				client->ps.weapon = WP_FG42SCOPE;

				COM_BitSet( client->ps.weapons, WP_FG42 );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_FG42 )] = 20;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42 )] = 999999;
				client->ps.weapon = WP_FG42;

#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FG42 ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FG42SCOPE ) );
#endif
				break;

			default: // Mauser by default
				COM_BitSet( client->ps.weapons, WP_SNIPERRIFLE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_SNIPERRIFLE )] = 10;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] = 999999;
				client->ps.weapon = WP_SNIPERRIFLE;

				COM_BitSet( client->ps.weapons, WP_MAUSER );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 10;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 999999;
				client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
			Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SNIPERRIFLE ) );
			Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif
		}
		COM_BitSet( client->ps.weapons, WP_SMOKE_GRENADE );
		client->ps.ammoclip[BG_FindClipForWeapon( WP_SMOKE_GRENADE )] = 1;
		client->ps.ammo[WP_SMOKE_GRENADE] = 1;

		COM_BitSet( client->ps.weapons, WP_MEDIC_SYRINGE );
		client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDIC_SYRINGE )] = 1;

		client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
		COM_BitSet( client->ps.weapons, WP_ARTY );
		client->ps.ammo[BG_FindAmmoForWeapon( WP_ARTY )] = 0;
		client->ps.ammoclip[BG_FindClipForWeapon( WP_ARTY )] = 1;

#ifdef OMNIBOT
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SMOKE_GRENADE ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MEDIC_SYRINGE ) );
		Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_ARTY ) );
#endif
	} 
// End Sniper war ========================================================================================
	else
	{
		if ( !g_theTrenchesMode.integer ) {
			// All players start with a knife (not OR-ing so that it clears previous weapons)
			client->ps.weapons[0] = 0;
			client->ps.weapons[1] = 0;
			COM_BitSet( client->ps.weapons, WP_KNIFE );

			if ( g_throwableKnives.integer > 0 ) {
				client->pers.throwingKnives = g_throwableKnives.integer;
			}

			client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )] = 1;
			client->ps.weapon = WP_KNIFE;
			client->ps.weaponstate = WEAPON_READY;

#ifdef OMNIBOT
			Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_KNIFE ) );
#endif

			// Engineer gets dynamite
			if ( pc == PC_ENGINEER ) {
				COM_BitSet( client->ps.weapons, WP_DYNAMITE );
				client->ps.ammo[BG_FindAmmoForWeapon( WP_DYNAMITE )] = 0;
				client->ps.ammoclip[BG_FindClipForWeapon( WP_DYNAMITE )] = 1;

				// NERVE - SMF
				COM_BitSet( client->ps.weapons, WP_PLIERS );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_PLIERS )] = 1;
				client->ps.ammo[WP_PLIERS] = 1;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_DYNAMITE ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PLIERS ) );
#endif
			}

			// Scouts gets binoculars and 2 grenades
			if ( pc == PC_SCOUT ) {
				client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
				COM_BitSet( client->ps.weapons, WP_STEN );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_STEN )] += 32;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_STEN )] += 32;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_STEN ) );

				// sort of a hack. bots need it for ammo table lookup so they can use the call arty goal
				COM_BitSet( client->ps.weapons, WP_BINOCULARS );
				client->ps.ammo[BG_FindAmmoForWeapon( WP_BINOCULARS )] = 0;
				client->ps.ammoclip[BG_FindClipForWeapon( WP_BINOCULARS )] = 1;

				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_BINOCULARS ) );
#endif
				switch ( client->sess.sessionTeam ) {
				case TEAM_BLUE:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 2;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				case TEAM_RED:
					COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = 2;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_LAUNCHER ) );
#endif
					break;
				default:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 2;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				}

			}
			// Lieutenant gets binoculars, ammo pack, artillery, and a grenade
			if ( pc == PC_LT ) {
				client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
				COM_BitSet( client->ps.weapons, WP_AMMO );
				client->ps.ammo[BG_FindAmmoForWeapon( WP_AMMO )] = 0;
				client->ps.ammoclip[BG_FindClipForWeapon( WP_AMMO )] = 1;
				COM_BitSet( client->ps.weapons, WP_ARTY );
				client->ps.ammo[BG_FindAmmoForWeapon( WP_ARTY )] = 0;
				client->ps.ammoclip[BG_FindClipForWeapon( WP_ARTY )] = 1;

				// NERVE - SMF
				COM_BitSet( client->ps.weapons, WP_SMOKE_GRENADE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_SMOKE_GRENADE )] = 1;
				client->ps.ammo[WP_SMOKE_GRENADE] = 1;

#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_AMMO ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_ARTY ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SMOKE_GRENADE ) );

				// sort of a hack. bots need it for ammo table lookup so they can use the call arty goal
				COM_BitSet( client->ps.weapons, WP_BINOCULARS );
				client->ps.ammo[BG_FindAmmoForWeapon( WP_BINOCULARS )] = 0;
				client->ps.ammoclip[BG_FindClipForWeapon( WP_BINOCULARS )] = 1;

				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_BINOCULARS ) );
#endif
				switch ( client->sess.sessionTeam ) {
				case TEAM_BLUE:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = g_ltNades.integer;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				case TEAM_RED:
					COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = g_ltNades.integer;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_LAUNCHER ) );
#endif
					break;
				default:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 1;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				}
			}

			// Everyone gets a pistol
			switch ( client->sess.sessionTeam ) { // JPW NERVE was playerPistol

			case TEAM_RED: // JPW NERVE
				COM_BitSet( client->ps.weapons, WP_LUGER );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_LUGER )] += 8;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_LUGER )] += g_pistolExtraClips.integer * 8;
				client->ps.weapon = WP_LUGER;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_LUGER ) );
#endif
				break;
			default: // '0' // TEAM_BLUE
				COM_BitSet( client->ps.weapons, WP_COLT );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_COLT )] += 8;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_COLT )] += g_pistolExtraClips.integer * 8;
				client->ps.weapon = WP_COLT;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_COLT ) );
#endif
				break;
			}

			// Everyone except Medic and LT get some grenades
			if ( pc == PC_ENGINEER ) { // JPW NERVE

				switch ( client->sess.sessionTeam ) { // was playerItem

				case TEAM_BLUE:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammo[BG_FindAmmoForWeapon( WP_GRENADE_PINEAPPLE )] = 0;
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = g_engNades.integer; // JPW NERVE
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				case TEAM_RED:
					COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
					client->ps.ammo[BG_FindAmmoForWeapon( WP_GRENADE_LAUNCHER )] = 0;
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = g_engNades.integer; // JPW NERVE
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_LAUNCHER ) );
#endif
					break;
				default:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammo[BG_FindAmmoForWeapon( WP_GRENADE_PINEAPPLE )] = 0;
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 8; // JPW NERVE
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				}
			}
			if ( pc == PC_SOLDIER ) { // JPW NERVE

				switch ( client->sess.sessionTeam ) { // was playerItem

				case TEAM_BLUE:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammo[BG_FindAmmoForWeapon( WP_GRENADE_PINEAPPLE )] = 0;
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = g_soldNades.integer; // JPW NERVE
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				case TEAM_RED:
					COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
					client->ps.ammo[BG_FindAmmoForWeapon( WP_GRENADE_LAUNCHER )] = 0;
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = g_soldNades.integer; // JPW NERVE
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_LAUNCHER ) );
#endif
					break;
				default:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammo[BG_FindAmmoForWeapon( WP_GRENADE_PINEAPPLE )] = 0;
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 4; // JPW NERVE
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				}
			}

			// JPW NERVE
			if ( pc == PC_MEDIC ) {
				COM_BitSet( client->ps.weapons, WP_MEDIC_SYRINGE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDIC_SYRINGE )] = 10;

				// NERVE - SMF
				COM_BitSet( client->ps.weapons, WP_MEDKIT );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDKIT )] = 1;
				client->ps.ammo[WP_MEDKIT] = 1;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MEDIC_SYRINGE ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MEDKIT ) );
#endif
				switch ( client->sess.sessionTeam ) { // was playerItem
				case TEAM_BLUE:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = g_medNades.integer;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				case TEAM_RED:
					COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = g_medNades.integer;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_LAUNCHER ) );
#endif
					break;
				default:
					COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 1;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_GRENADE_PINEAPPLE ) );
#endif
					break;
				}
			}
			// jpw


			//redoing weapon handouts!!!
			switch ( client->sess.playerWeapon )
			{
			case 3: //MP40
				COM_BitSet( client->ps.weapons, WP_MP40 );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_MP40 )] += 32;
				if ( pc == PC_SOLDIER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += g_soldExtraClips.integer * 32;
				} else if ( pc == PC_ENGINEER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += g_engExtraClips.integer * 32;
				} else if ( pc == PC_MEDIC ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += g_medExtraClips.integer * 32;
				} else if ( pc == PC_LT ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += g_ltExtraClips.integer * 32;
				}
				client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MP40 ) );
#endif
				weapGiven = qtrue;
				break;
			case 4: //Thompson
				COM_BitSet( client->ps.weapons, WP_THOMPSON );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_THOMPSON )] += 30;
				if ( pc == PC_SOLDIER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += g_soldExtraClips.integer * 30;
				} else if ( pc == PC_ENGINEER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += g_engExtraClips.integer * 30;
				} else if ( pc == PC_MEDIC ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += g_medExtraClips.integer * 30;
				} else if ( pc == PC_LT ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += g_ltExtraClips.integer * 30;
				}
				client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_THOMPSON ) );
#endif
				weapGiven = qtrue;
				break;
			case 5: //Sten
				COM_BitSet( client->ps.weapons, WP_STEN );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_STEN )] += 32;
				if ( pc == PC_SOLDIER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_STEN )] += g_soldExtraClips.integer * 32;
				} else if ( pc == PC_ENGINEER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_STEN )] += g_engExtraClips.integer * 32;
				} else if ( pc == PC_MEDIC ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_STEN )] += g_medExtraClips.integer * 32;
				} else if ( pc == PC_LT ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_STEN )] += g_ltExtraClips.integer * 32;
				}
				client->ps.weapon = WP_STEN;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_STEN ) );
#endif
				weapGiven = qtrue;
				break;
			case 6: //Mauser
				if ( pc != PC_SOLDIER ) {
					break;
				}

				if ( g_maxMauser.integer == 0 ) {
					trap_SendServerCommand( client->ps.clientNum, va( "cp \"Snipers are disabled. \nPlease select a different weapon.\n\"" ) );
					break;
				}

				if ( g_maxMauser.integer != -1 ) {
					int count = ( client->sess.sessionTeam == TEAM_RED ) ? level.axisMauser : level.alliedMauser;
					if ( ( count >= g_maxMauser.integer ) && ( client->pers.restrictedWeapon != WP_MAUSER ) ) {
						trap_SendServerCommand( client->ps.clientNum, va( "cp \"Maximum number of snipers reached(%d).\nPlease select a different weapon.\n\"", g_maxMauser.integer ) );
						break;
					}
				}

				if ( client->pers.restrictedWeapon != WP_MAUSER ) {
					( client->sess.sessionTeam == TEAM_RED ) ? level.axisMauser++ : level.alliedMauser++;
					client->pers.restrictedWeapon = WP_MAUSER;
				}


				COM_BitSet( client->ps.weapons, WP_SNIPERRIFLE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_SNIPERRIFLE )] = 10;
				//client->ps.ammo[BG_FindAmmoForWeapon(WP_SNIPERRIFLE)] = g_mauserExtraClips.integer*10;	// Was 10
				client->ps.weapon = WP_SNIPERRIFLE;

				COM_BitSet( client->ps.weapons, WP_MAUSER );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 10;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] += g_mauserExtraClips.integer * 10; // Was 10
				client->ps.weapon = WP_MAUSER;

#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SNIPERRIFLE ) );
#endif
				weapGiven = qtrue;
				break;

// *** FG42
			case 7:
				if ( pc != PC_SOLDIER && pc != PC_ENGINEER ) {
					break;
				}
				if (S4ND_IsDemo())
					break;
				COM_BitSet( client->ps.weapons, WP_FG42SCOPE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_FG42SCOPE )] = 20;
				client->ps.weapon = WP_FG42SCOPE;

				COM_BitSet( client->ps.weapons, WP_FG42 );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_FG42 )] = 20;
				if ( pc == PC_SOLDIER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42 )] = (20 * g_soldExtraClips.integer) + 20;
				} else {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42 )] = (20 * g_engExtraClips.integer) + 20;
				}
				client->ps.weapon = WP_FG42;

#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FG42 ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FG42SCOPE ) );
#endif
				weapGiven = qtrue;
				break;

			case 8: //Panzer
				if ( pc != PC_SOLDIER ) {
					break;
				}

				if ( g_maxPf.integer == 0 ) {
					trap_SendServerCommand( client->ps.clientNum, va( "cp \"Panzers are disabled. \nPlease select a different weapon.\n\"" ) );
					break;
				}

				if ( g_maxPf.integer != -1 ) {
					int count = ( client->sess.sessionTeam == TEAM_RED ) ? level.axisPanz : level.alliedPanz;
					if ( ( count >= g_maxPf.integer ) && ( client->pers.restrictedWeapon != WP_PANZERFAUST ) ) {
						trap_SendServerCommand( client->ps.clientNum, va( "cp \"Maximum number of panzerfausts reached(%d).\nPlease select a different weapon.\n\"", g_maxPf.integer ) );
						break;
					}
				}

				if ( client->pers.restrictedWeapon != WP_PANZERFAUST ) {
					( client->sess.sessionTeam == TEAM_RED ) ? level.axisPanz++ : level.alliedPanz++;
					client->pers.restrictedWeapon = WP_PANZERFAUST;
				}

				COM_BitSet( client->ps.weapons, WP_PANZERFAUST );
				if ( g_panzerClips.integer > 1 ) {
					client->ps.ammoclip[BG_FindClipForWeapon( WP_PANZERFAUST )] = g_panzerClips.integer;
				}
				client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = g_extraPanzerClips.integer;
				client->ps.weapon = WP_PANZERFAUST;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PANZERFAUST ) );
#endif
				weapGiven = qtrue;
				break;
			case 9: //Venom
				if ( pc != PC_SOLDIER ) {
					break;
				}
				if ( g_maxVenom.integer == 0 ) {
					trap_SendServerCommand( client->ps.clientNum, va( "cp \"Venoms are disabled. \nPlease select a different weapon.\n\"" ) );
					break;
				}

				if ( g_maxVenom.integer != -1 ) {
					int count = ( client->sess.sessionTeam == TEAM_RED ) ? level.axisVenom : level.alliedVenom;
					if ( ( count >= g_maxPf.integer ) && ( client->pers.restrictedWeapon != WP_VENOM ) ) {
						trap_SendServerCommand( client->ps.clientNum, va( "cp \"Maximum number of venoms reached(%d).\nPlease select a different weapon.\n\"", g_maxVenom.integer ) );
						break;
					}
				}

				if ( client->pers.restrictedWeapon != WP_VENOM ) {
					( client->sess.sessionTeam == TEAM_RED ) ? level.axisVenom++ : level.alliedVenom++;
					client->pers.restrictedWeapon = WP_VENOM;
				}

				COM_BitSet( client->ps.weapons, WP_VENOM );
				client->ps.ammoclip[BG_FindAmmoForWeapon( WP_VENOM )] = 500;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )] += g_venomExtraClips.integer * 500;
				client->ps.weapon = WP_VENOM;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_VENOM ) );
#endif
				weapGiven = qtrue;
				break;

			case 10: //Flamethrower
				if ( pc != PC_SOLDIER ) {
					break;
				}
				if ( g_maxFlamer.integer == 0 ) {
					trap_SendServerCommand( client->ps.clientNum, va( "cp \"Flamethrowers are disabled. \nPlease select a different weapon.\n\"" ) );
					break;
				}

				if ( g_maxFlamer.integer != -1 ) {
					int count = ( client->sess.sessionTeam == TEAM_RED ) ? level.axisFlame : level.alliedFlame;
					if ( ( count >= g_maxFlamer.integer ) && ( client->pers.restrictedWeapon != WP_FLAMETHROWER ) ) {
						trap_SendServerCommand( client->ps.clientNum, va( "cp \"Maximum number of flamethrowers reached(%d).\nPlease select a different weapon.\n\"", g_maxFlamer.integer ) );
						break;
					}
				}

				if ( client->pers.restrictedWeapon != WP_FLAMETHROWER ) {
					( client->sess.sessionTeam == TEAM_RED ) ? level.axisFlame++ : level.alliedFlame++;
					client->pers.restrictedWeapon = WP_FLAMETHROWER;
				}

				COM_BitSet( client->ps.weapons, WP_FLAMETHROWER );
				client->ps.ammoclip[BG_FindAmmoForWeapon( WP_FLAMETHROWER )] = 200;
				client->ps.weapon = WP_FLAMETHROWER;

#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FLAMETHROWER ) );
#endif
				weapGiven = qtrue;
				break;

			//*** Shotgun
			case 11:
				if ( pc == PC_MEDIC ) {
					break;
				}
				if (S4ND_IsDemo())
					break;
				COM_BitSet( client->ps.weapons, WP_SHOTGUN );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_SHOTGUN )] = 6;
				/*client->ps.ammo[BG_FindAmmoForWeapon(WP_SHOTGUN)] = 24;*/
				if ( pc == PC_SOLDIER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_SHOTGUN )] += g_soldExtraClips.integer * 6;
				} else if ( pc == PC_ENGINEER ) {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_SHOTGUN )] += g_engExtraClips.integer * 6;
				} else {
					client->ps.ammo[BG_FindAmmoForWeapon( WP_SHOTGUN )] += g_ltExtraClips.integer * 6;
				}

				client->ps.weapon = WP_SHOTGUN;

#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SHOTGUN ) );
#endif
				weapGiven = qtrue;
				break;


			}
			//we did not find a weapon to give them
			if ( !weapGiven ) {
				if ( client->sess.sessionTeam == TEAM_BLUE ) {
					COM_BitSet( client->ps.weapons, WP_THOMPSON );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_THOMPSON )] += 30;
					if ( pc == PC_SOLDIER ) {
						client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += g_soldExtraClips.integer * 30;
					} else if ( pc == PC_ENGINEER ) {
						client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += g_engExtraClips.integer * 30;
					} else if ( pc == PC_MEDIC ) {
						client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += g_medExtraClips.integer * 30;
					} else if ( pc == PC_LT ) {
						client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += g_ltExtraClips.integer * 30;
					}
					client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_THOMPSON ) );
#endif
				} else if ( client->sess.sessionTeam == TEAM_RED )      {
					COM_BitSet( client->ps.weapons, WP_MP40 );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_MP40 )] += 32;
					if ( pc == PC_SOLDIER ) {
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += g_soldExtraClips.integer * 32;
					} else if ( pc == PC_ENGINEER ) {
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += g_engExtraClips.integer * 32;
					} else if ( pc == PC_MEDIC ) {
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += g_medExtraClips.integer * 32;
					} else if ( pc == PC_LT ) {
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += g_ltExtraClips.integer * 32;
					}
					client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MP40 ) );
#endif
				}
			}
/* OLD CODE
    // Soldiers and Lieutenants get a 2-handed weapon
    if ( pc == PC_SOLDIER || pc == PC_LT ) {

        // JPW NERVE -- if LT is selected but illegal weapon, set to team-specific SMG
        if ((pc == PC_LT) && (client->sess.playerWeapon > 5)) {
            if (client->sess.sessionTeam == TEAM_RED)
                client->sess.playerWeapon = 3;
            else
                client->sess.playerWeapon = 4;
        }
        // jpw
        switch( client->sess.playerWeapon ) {

        case 3:		// WP_MP40
            COM_BitSet( client->ps.weapons, WP_MP40 );
            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
            if (pc == PC_SOLDIER)
                client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_soldExtraClips.integer*32;
            else
                client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_ltExtraClips.integer*32;
            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
            break;

        case 4:		// WP_THOMPSON
            COM_BitSet( client->ps.weapons, WP_THOMPSON );
            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
            if (pc == PC_SOLDIER)
                client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_soldExtraClips.integer*30;
            else
                client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_ltExtraClips.integer*30;
            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
            break;

        case 5:		// WP_STEN
            COM_BitSet( client->ps.weapons, WP_STEN );
            client->ps.ammoclip[BG_FindClipForWeapon(WP_STEN)] += 32;
            if (pc == PC_SOLDIER)
                client->ps.ammo[BG_FindAmmoForWeapon(WP_STEN)] += g_soldExtraClips.integer*32;
            else
                client->ps.ammo[BG_FindAmmoForWeapon(WP_STEN)] += g_ltExtraClips.integer*32;
            client->ps.weapon = WP_STEN;
#ifdef OMNIBOT
            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_STEN));
#endif
            break;

        case 6:		// WP_MAUSER, WP_SNIPERRIFLE
                    if ( pc != PC_SOLDIER )
                    return;

                if(g_maxMauser.integer == 0){
                    if (client->sess.sessionTeam == TEAM_RED){
                            COM_BitSet(client->ps.weapons, WP_MP40);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_soldExtraClips.integer*32;	// Martin - Nov 3 2008 wow old bug
                            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
                        }else{
                            COM_BitSet(client->ps.weapons, WP_THOMPSON);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_soldExtraClips.integer*30;
                            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
                        }
                        trap_SendServerCommand(client->ps.clientNum, va("cp \"Snipers are disabled. \nPlease select a different weapon.\n\""));
                        break;
                    }
                if (g_maxMauser.integer != -1){
                    int count = (client->sess.sessionTeam == TEAM_RED) ? level.axisMauser : level.alliedMauser;

                    if ((count >= g_maxMauser.integer) && (client->pers.restrictedWeapon != WP_MAUSER)){
                        trap_SendServerCommand(client->ps.clientNum, va("cp \"Maximum number of mausers reached (%d). \nPlease select a different weapon.\n\"", g_maxMauser.integer));

                        if (client->sess.sessionTeam == TEAM_RED){
                            COM_BitSet(client->ps.weapons, WP_MP40);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_soldExtraClips.integer*32;
                            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
                        }else{
                            COM_BitSet(client->ps.weapons, WP_THOMPSON);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_soldExtraClips.integer*30;
                            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
                        }
                        break;
                    }

                    if (client->pers.restrictedWeapon != WP_MAUSER){
                        (client->sess.sessionTeam == TEAM_RED) ? level.axisMauser++ : level.alliedMauser++;
                        client->pers.restrictedWeapon = WP_MAUSER;
#ifdef OMNIBOT
                        Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MAUSER));
#endif
                    }
                }


                COM_BitSet( client->ps.weapons, WP_SNIPERRIFLE );
                client->ps.ammoclip[BG_FindClipForWeapon(WP_SNIPERRIFLE)] = 10;
            //	client->ps.ammo[BG_FindAmmoForWeapon(WP_SNIPERRIFLE)] = g_mauserExtraClips.integer*10;	// Was 10
                client->ps.weapon = WP_SNIPERRIFLE;

                COM_BitSet( client->ps.weapons, WP_MAUSER );
                client->ps.ammoclip[BG_FindClipForWeapon(WP_MAUSER)] = 10;
                client->ps.ammo[BG_FindAmmoForWeapon(WP_MAUSER)] += g_mauserExtraClips.integer*10;	// Was 10
                client->ps.weapon = WP_MAUSER;

#ifdef OMNIBOT
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MAUSER));
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_SNIPERRIFLE));
#endif

                break;

            case 7://WP_FG42
                if ( pc != PC_SOLDIER )
                    return;
                COM_BitSet( client->ps.weapons, WP_FG42SCOPE );
                client->ps.ammoclip[BG_FindClipForWeapon(WP_FG42SCOPE)] = 20;
                client->ps.ammo[BG_FindAmmoForWeapon(WP_FG42SCOPE)] = 40;
                client->ps.weapon = WP_FG42SCOPE;

                COM_BitSet( client->ps.weapons, WP_FG42 );
                client->ps.ammoclip[BG_FindClipForWeapon(WP_FG42)] = 20;
                client->ps.ammo[BG_FindAmmoForWeapon(WP_FG42)] = 40;
                client->ps.weapon = WP_FG42;

#ifdef OMNIBOT
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_FG42));
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_FG42SCOPE));
#endif
                break;

            case 8:		// WP_PANZERFAUST
                if ( pc != PC_SOLDIER )
                    return;

            if(g_maxPf.integer == 0){
                    if (client->sess.sessionTeam == TEAM_RED){
                            COM_BitSet(client->ps.weapons, WP_MP40);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += 64;
                            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
                        }
                        else
                        {
                            COM_BitSet(client->ps.weapons, WP_THOMPSON);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += 60;
                            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
                        }
                        trap_SendServerCommand(client->ps.clientNum, va("cp \"Panzers are disabled. \nPlease select a different weapon.\n\""));
                        break;
                    }



                if (g_maxPf.integer != -1)
                {
                    int count = (client->sess.sessionTeam == TEAM_RED) ? level.axisPanz : level.alliedPanz;

                    if ((count >= g_maxPf.integer) && (client->pers.restrictedWeapon != WP_PANZERFAUST))
                    {
                        trap_SendServerCommand(client->ps.clientNum, va("cp \"Maximum number of panzerfausts reached(%d).\nPlease select a different weapon.\n\"", g_maxPf.integer));

                        if (client->sess.sessionTeam == TEAM_RED)
                        {
                            COM_BitSet(client->ps.weapons, WP_MP40);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += 64;
                            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
                        }
                        else
                        {
                            COM_BitSet(client->ps.weapons, WP_THOMPSON);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += 60;
                            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
                        }

                        break;
                    }

                    if (client->pers.restrictedWeapon != WP_PANZERFAUST)
                    {
                        (client->sess.sessionTeam == TEAM_RED) ? level.axisPanz++ : level.alliedPanz++;
                        client->pers.restrictedWeapon = WP_PANZERFAUST;
#ifdef OMNIBOT
                        Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_PANZERFAUST));
#endif
                    }
                }

                COM_BitSet( client->ps.weapons, WP_PANZERFAUST );
                if (g_panzerClips.integer > 1)
                client->ps.ammoclip[BG_FindClipForWeapon(WP_PANZERFAUST)] = g_panzerClips.integer;
                client->ps.ammo[BG_FindAmmoForWeapon(WP_PANZERFAUST)] = g_extraPanzerClips.integer;
                client->ps.weapon = WP_PANZERFAUST;
#ifdef OMNIBOT
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_PANZERFAUST));
#endif
                break;

            case 9:	// *** WP_VENOM ***
                if ( pc != PC_SOLDIER )
                    return;

                if(g_maxVenom.integer == 0){
                    if (client->sess.sessionTeam == TEAM_RED){
                            COM_BitSet(client->ps.weapons, WP_MP40);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_soldExtraClips.integer*32;
                            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
                        }else{
                            COM_BitSet(client->ps.weapons, WP_THOMPSON);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_soldExtraClips.integer*30;
                            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
                        }
                        trap_SendServerCommand(client->ps.clientNum, va("cp \"Venoms are disabled. \nPlease select a different weapon.\n\""));
                        break;
                }
                if (g_maxVenom.integer != -1){
                    int count = (client->sess.sessionTeam == TEAM_RED) ? level.axisVenom : level.alliedVenom;

                    if ((count >= g_maxVenom.integer) && (client->pers.restrictedWeapon != WP_VENOM)){
                        trap_SendServerCommand(client->ps.clientNum, va("cp \"Maximum number of venoms reached (%d). \nPlease select a different weapon.\n\"", g_maxVenom.integer));

                        if (client->sess.sessionTeam == TEAM_RED){
                            COM_BitSet(client->ps.weapons, WP_MP40);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_soldExtraClips.integer*32;
                            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
                        }else{
                            COM_BitSet(client->ps.weapons, WP_THOMPSON);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_soldExtraClips.integer*30;
                            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
                        }

                        break;
                    }
                    if (client->pers.restrictedWeapon != WP_VENOM){
                        (client->sess.sessionTeam == TEAM_RED) ? level.axisVenom++ : level.alliedVenom++;
                        client->pers.restrictedWeapon = WP_VENOM;
#ifdef OMNIBOT
                        Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_VENOM));
#endif
                    }
                }


                COM_BitSet( client->ps.weapons, WP_VENOM );
                client->ps.ammoclip[BG_FindAmmoForWeapon(WP_VENOM)] = 500;
                client->ps.ammo[BG_FindAmmoForWeapon(WP_VENOM)] += g_venomExtraClips.integer*500;
                client->ps.weapon = WP_VENOM;
#ifdef OMNIBOT
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_VENOM));
#endif
                break;

            case 10:	// WP_FLAMETHROWER
                if ( pc != PC_SOLDIER )
                    return;

                if(g_maxFlamer.integer == 0){
                    if (client->sess.sessionTeam == TEAM_RED){
                            COM_BitSet(client->ps.weapons, WP_MP40);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_soldExtraClips.integer*32;
                            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
                        }else{
                            COM_BitSet(client->ps.weapons, WP_THOMPSON);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_soldExtraClips.integer*30;
                            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
                        }
                        trap_SendServerCommand(client->ps.clientNum, va("cp \"Flamethrowers are disabled. \nPlease select a different weapon.\n\""));
                        break;
                    }


                if (g_maxFlamer.integer != -1){
                    int count = (client->sess.sessionTeam == TEAM_RED) ? level.axisFlame : level.alliedFlame;

                    if ((count >= g_maxFlamer.integer) && (client->pers.restrictedWeapon != WP_FLAMETHROWER))
                    {
                        trap_SendServerCommand(client->ps.clientNum, va("cp \"Maximum number of flamethrowers reached (%d). \nPlease select a different weapon.\n\"", g_maxFlamer.integer));

                        if (client->sess.sessionTeam == TEAM_RED)
                        {
                            COM_BitSet(client->ps.weapons, WP_MP40);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_soldExtraClips.integer*32;
                            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
                        }
                        else
                        {
                            COM_BitSet(client->ps.weapons, WP_THOMPSON);
                            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                            client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_soldExtraClips.integer*30;
                            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
                        }

                        break;
                    }

                    if (client->pers.restrictedWeapon != WP_FLAMETHROWER)
                    {
                        (client->sess.sessionTeam == TEAM_RED) ? level.axisFlame++ : level.alliedFlame++;
                        client->pers.restrictedWeapon = WP_FLAMETHROWER;
#ifdef OMNIBOT
                        Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_FLAMETHROWER));
#endif
                    }
                }


                COM_BitSet( client->ps.weapons, WP_FLAMETHROWER );
                client->ps.ammoclip[BG_FindAmmoForWeapon(WP_FLAMETHROWER)] = 200;
                client->ps.weapon = WP_FLAMETHROWER;
#ifdef OMNIBOT
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_FLAMETHROWER));
#endif
                break;

        default:	// give MP40 if given invalid weapon number
            if (client->sess.sessionTeam == TEAM_RED) { // JPW NERVE
                COM_BitSet( client->ps.weapons, WP_MP40 );
                client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
                client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] +=  g_soldExtraClips.integer* 32;
                client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
            }
            else { // TEAM_BLUE
                COM_BitSet( client->ps.weapons, WP_THOMPSON );
                client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
                client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_soldExtraClips.integer* 30;
                client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
                Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
            }
            break;
        }
    }else{ // medic or engineer gets assigned MP40 or Thompson with one magazine ammo

        if ((g_chooseMg.integer !=0) && ( client->sess.mp40>0 || client->sess.colorFlags & USE_MP40 ) ){
            COM_BitSet( client->ps.weapons, WP_MP40 );
            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;

            if (pc == PC_ENGINEER) // OK so engineers get two mags
                client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_engExtraClips.integer*32;

            if (pc == PC_MEDIC) // OK so engineers get two mags
                client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_medExtraClips.integer*32;

                client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif

        }else if ((g_chooseMg.integer !=0) && (client->sess.thompson>0 || client->sess.colorFlags & USE_THOMPSON)){

            COM_BitSet( client->ps.weapons, WP_THOMPSON );
            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;

            if (pc == PC_ENGINEER)
                client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] +=  g_engExtraClips.integer*30;

             if (pc == PC_MEDIC)
                client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_medExtraClips.integer*30;

             client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif

        }else if ((g_chooseMg.integer !=0) && (client->sess.sten>0 || client->sess.colorFlags & USE_STEN)){
            COM_BitSet( client->ps.weapons, WP_STEN );
            client->ps.ammoclip[BG_FindClipForWeapon(WP_STEN)] += 32;

            if (pc == PC_ENGINEER) // OK so engineers get two mags
                client->ps.ammo[BG_FindAmmoForWeapon(WP_STEN)] += g_engExtraClips.integer*32;

            if (pc == PC_MEDIC)
                client->ps.ammo[BG_FindAmmoForWeapon(WP_STEN)] += g_medExtraClips.integer*32;

                client->ps.weapon = WP_STEN;

#ifdef OMNIBOT
            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_STEN));
#endif

        }else if (client->sess.sessionTeam == TEAM_RED){ // axis
            COM_BitSet( client->ps.weapons, WP_MP40 );
            client->ps.ammoclip[BG_FindClipForWeapon(WP_MP40)] += 32;
// JPW NERVE
            if (pc == PC_ENGINEER) // OK so engineers get two mags
                client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_engExtraClips.integer*32;

            if (pc == PC_MEDIC) // OK so engineers get two mags
                client->ps.ammo[BG_FindAmmoForWeapon(WP_MP40)] += g_medExtraClips.integer*32;
// jpw
            client->ps.weapon = WP_MP40;
#ifdef OMNIBOT
            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_MP40));
#endif
        }else if (client->sess.sessionTeam == TEAM_BLUE){
            COM_BitSet( client->ps.weapons, WP_THOMPSON );
            client->ps.ammoclip[BG_FindClipForWeapon(WP_THOMPSON)] += 30;
// JPW NERVE
            if (pc == PC_ENGINEER)
                client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] +=  g_engExtraClips.integer*30;

             if (pc == PC_MEDIC)
                client->ps.ammo[BG_FindAmmoForWeapon(WP_THOMPSON)] += g_medExtraClips.integer*30;
// jpw
            client->ps.weapon = WP_THOMPSON;
#ifdef OMNIBOT
            Bot_Event_AddWeapon(client->ps.clientNum, Bot_WeaponGameToBot(WP_THOMPSON));
#endif
        }
    }
*/
		} else {
			//S4NDM4NN-This is for the trenches Just a matter of copy/paste!
			/*
			===============================
			FUN MODE!!!
			===============================
			*/
//mp40 or thompson=venom
			//Panzer has reduced dmg
			//
			if ( g_theTrenchesMode.integer == 2 ) {
				if ( client->sess.sessionTeam == TEAM_RED ) {
					//AXIS ENGIE
					if ( pc == PC_ENGINEER ) {
						COM_BitSet( client->ps.weapons, WP_MP40 );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_MP40 )] += 100;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += 0;
						client->ps.weapon = WP_MP40;

						COM_BitSet( client->ps.weapons, WP_MAUSER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 20;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 0;
						client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MP40 ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif

					}
					//AXIS MEDIC
					else if ( pc == PC_MEDIC ) {
						COM_BitSet( client->ps.weapons, WP_MP40 );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_MP40 )] += 100;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += 0;
						client->ps.weapon = WP_MP40;

						COM_BitSet( client->ps.weapons, WP_STEN );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_STEN )] += 5;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_STEN )] += 0;
						client->ps.weapon = WP_STEN;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MP40 ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_STEN ) );
#endif
					}
					//AXIS LT
					else if ( pc == PC_LT ) {
						COM_BitSet( client->ps.weapons, WP_MP40 );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_MP40 )] += 100;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += 0;
						client->ps.weapon = WP_MP40;

						COM_BitSet( client->ps.weapons, WP_PANZERFAUST );
						client->ps.ammo[BG_FindClipForWeapon( WP_PANZERFAUST )] = 15;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = 15;
						client->ps.weapon = WP_PANZERFAUST;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MP40 ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PANZERFAUST ) );
#endif
					}
					//AXIS SOLDIER/the default
					else {
						COM_BitSet( client->ps.weapons, WP_MP40 );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_MP40 )] += 100;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MP40 )] += 0;
						client->ps.weapon = WP_MP40;

						COM_BitSet( client->ps.weapons, WP_FLAMETHROWER );
						client->ps.ammoclip[BG_FindAmmoForWeapon( WP_FLAMETHROWER )] = 200;
						client->ps.weapon = WP_FLAMETHROWER;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MP40 ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FLAMETHROWER ) );
#endif
					}
				}
				//ALLIES
				else {
					//ALLIES ENGIE
					if ( pc == PC_ENGINEER ) {
						COM_BitSet( client->ps.weapons, WP_THOMPSON );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_THOMPSON )] += 100;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += 0;
						client->ps.weapon = WP_THOMPSON;

						COM_BitSet( client->ps.weapons, WP_MAUSER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 20;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 0;
						client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_THOMPSON ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif
					}
					//ALLIES MEDIC
					else if ( pc == PC_MEDIC ) {
						COM_BitSet( client->ps.weapons, WP_THOMPSON );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_THOMPSON )] += 100;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += 0;
						client->ps.weapon = WP_THOMPSON;

						COM_BitSet( client->ps.weapons, WP_STEN );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_STEN )] += 5;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_STEN )] += 0;
						client->ps.weapon = WP_STEN;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_THOMPSON ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_STEN ) );
#endif
					}
					//ALLIES LT
					else if ( pc == PC_LT ) {
						COM_BitSet( client->ps.weapons, WP_THOMPSON );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_THOMPSON )] += 100;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += 0;
						client->ps.weapon = WP_THOMPSON;

						COM_BitSet( client->ps.weapons, WP_PANZERFAUST );
						client->ps.ammo[BG_FindClipForWeapon( WP_PANZERFAUST )] = 15;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = 15;
						client->ps.weapon = WP_PANZERFAUST;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_THOMPSON ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PANZERFAUST ) );
#endif
					}
					//ALLIES SOLDIER/default default
					else {
						COM_BitSet( client->ps.weapons, WP_THOMPSON );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_THOMPSON )] += 100;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_THOMPSON )] += 0;
						client->ps.weapon = WP_THOMPSON;

						COM_BitSet( client->ps.weapons, WP_FLAMETHROWER );
						client->ps.ammoclip[BG_FindAmmoForWeapon( WP_FLAMETHROWER )] = 200;
						client->ps.weapon = WP_FLAMETHROWER;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_THOMPSON ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FLAMETHROWER ) );
#endif
					}
				}
			}
/*
================================
        END FUN MODE
================================



================================
        SNIPER WAR MODE
================================
*/
			else if ( g_theTrenchesMode.integer == 3 ) {
				COM_BitSet( client->ps.weapons, WP_SNIPERRIFLE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_SNIPERRIFLE )] = 0;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] = 999999;
				client->ps.weapon = WP_SNIPERRIFLE;

				COM_BitSet( client->ps.weapons, WP_MAUSER );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 0;
				client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 999999;
				client->ps.weapon = WP_MAUSER;

				COM_BitSet( client->ps.weapons, WP_SMOKE_GRENADE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_SMOKE_GRENADE )] = 1;
				client->ps.ammo[WP_SMOKE_GRENADE] = 1;

				COM_BitSet( client->ps.weapons, WP_MEDIC_SYRINGE );
				client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDIC_SYRINGE )] = 1;

				client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
				COM_BitSet( client->ps.weapons, WP_ARTY );
				client->ps.ammo[BG_FindAmmoForWeapon( WP_ARTY )] = 0;
				client->ps.ammoclip[BG_FindClipForWeapon( WP_ARTY )] = 1;
#ifdef OMNIBOT
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SNIPERRIFLE ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SMOKE_GRENADE ) );
				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MEDIC_SYRINGE ) );

				// sort of a hack. bots need it for ammo table lookup so they can use the call arty goal
				COM_BitSet( client->ps.weapons, WP_BINOCULARS );
				client->ps.ammo[BG_FindAmmoForWeapon( WP_BINOCULARS )] = 0;
				client->ps.ammoclip[BG_FindClipForWeapon( WP_BINOCULARS )] = 1;

				Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_BINOCULARS ) );
#endif
			}
/*
====================================
        END FUN, BEGIN REGUALR WW1
====================================
*/
			else {
				switch ( client->sess.sessionTeam ) { // JPW NERVE was playerPistol

				case TEAM_RED: // JPW NERVE
					COM_BitSet( client->ps.weapons, WP_LUGER );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_LUGER )] += 10;
					client->ps.ammo[BG_FindAmmoForWeapon( WP_LUGER )] += 30;
					client->ps.weapon = WP_LUGER;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_LUGER ) );
#endif
					break;
				default: // '0' // TEAM_BLUE
					COM_BitSet( client->ps.weapons, WP_COLT );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_COLT )] += 10;
					client->ps.ammo[BG_FindAmmoForWeapon( WP_COLT )] += 30;
					client->ps.weapon = WP_COLT;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_COLT ) );
#endif
					break;
				}

				//client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );

				//*****COMMENT THIS BACK IN*****
				//COM_BitSet( client->ps.weapons, WP_SMOKE_GRENADE);
				//client->ps.ammoclip[BG_FindClipForWeapon(WP_SMOKE_GRENADE)] = 1;
				//client->ps.ammo[WP_SMOKE_GRENADE] = 1;
				// Engineer gets knife, mauser, smoke canister
				if ( pc == PC_ENGINEER ) {
					COM_BitSet( client->ps.weapons, WP_MAUSER );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 6;
					client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 24;
					client->ps.weapon = WP_MAUSER;

#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif

					//this will be a gas attack???-no more as?
					COM_BitSet( client->ps.weapons, WP_SMOKE_GRENADE );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_SMOKE_GRENADE )] = 1;
					client->ps.ammo[WP_SMOKE_GRENADE] = 1;

					//Grenades for Engie:
					if ( client->sess.sessionTeam == TEAM_RED ) {
						COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = 3;
					} else {
						COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 3;
					}

					//We'd like to be able to get through walls
					COM_BitSet( client->ps.weapons, WP_DYNAMITE );
					client->ps.ammo[BG_FindAmmoForWeapon( WP_DYNAMITE )] = 0;
					client->ps.ammoclip[BG_FindClipForWeapon( WP_DYNAMITE )] = 1;

					//And be able to stop them from coming
					COM_BitSet( client->ps.weapons, WP_PLIERS );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_PLIERS )] = 1;
					client->ps.ammo[WP_PLIERS] = 1;
				}
//Medic gets knife, pistol, medpacks
				if ( pc == PC_MEDIC ) {
					//Medics have to have some bonus...
					if ( client->sess.sessionTeam == TEAM_RED ) {
						COM_BitSet( client->ps.weapons, WP_LUGER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_LUGER )] += 0;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_LUGER )] += 10;
						client->ps.weapon = WP_LUGER;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_LUGER ) );
#endif
					} else {
						COM_BitSet( client->ps.weapons, WP_COLT );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_COLT )] += 0;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_COLT )] += 10;
						client->ps.weapon = WP_COLT;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_COLT ) );
#endif
					}
					COM_BitSet( client->ps.weapons, WP_MAUSER );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 6;
					client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 0;
					client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif

					COM_BitSet( client->ps.weapons, WP_MEDKIT );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDKIT )] = 1;
					client->ps.ammo[WP_MEDKIT] = 1;

					//Not enough technology to have all this "fancys"
					COM_BitSet( client->ps.weapons, WP_MEDIC_SYRINGE );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDIC_SYRINGE )] = 2;


					//Grenades just 2 because we feel sorry for med with no primary
					//MEDS HAVE A PRIMARY, so only 1 grenade
					if ( client->sess.sessionTeam == TEAM_RED ) {
						COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = 1;
					} else {
						COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 1;
					}
				}

//soldier gets knife, pistol, mauser, flamer, venom or sniper
				if ( pc == PC_SOLDIER ) {
					switch ( client->sess.playerWeapon ) {
					case 6: // WP_MAUSER, WP_SNIPERRIFLE
						COM_BitSet( client->ps.weapons, WP_SNIPERRIFLE );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_SNIPERRIFLE )] = 6;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] = 12;
						client->ps.weapon = WP_SNIPERRIFLE;

						COM_BitSet( client->ps.weapons, WP_MAUSER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 6;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 12;
						client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_SNIPERRIFLE ) );
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif
						break;

					case 8: // WP_PANZERFAUST
						if ( g_maxPf.integer != -1 ) {
							int count = ( client->sess.sessionTeam == TEAM_RED ) ? level.axisPanz : level.alliedPanz;

							if ( ( count >= g_maxPf.integer ) && ( client->pers.restrictedWeapon != WP_PANZERFAUST ) ) {
								trap_SendServerCommand( client->ps.clientNum, va( "cp \"Maximum number of panzerfausts reached(%d).\nPlease select a different weapon.\n\"", g_maxPf.integer ) );

								COM_BitSet( client->ps.weapons, WP_MAUSER );
								client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 6;
								client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 12;
								client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
								Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif
								break;
							}

							if ( client->pers.restrictedWeapon != WP_PANZERFAUST ) {
								( client->sess.sessionTeam == TEAM_RED ) ? level.axisPanz++ : level.alliedPanz++;
								client->pers.restrictedWeapon = WP_PANZERFAUST;
#ifdef OMNIBOT
								Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PANZERFAUST ) );
#endif
							}
						}


						COM_BitSet( client->ps.weapons, WP_PANZERFAUST );
						client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = 3;
						client->ps.weapon = WP_PANZERFAUST;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_PANZERFAUST ) );
#endif
						break;

					case 9: // WP_VENOM
						if ( g_maxVenom.integer != -1 ) {
							int count = ( client->sess.sessionTeam == TEAM_RED ) ? level.axisVenom : level.alliedVenom;

							if ( ( count >= g_maxVenom.integer ) && ( client->pers.restrictedWeapon != WP_VENOM ) ) {
								trap_SendServerCommand( client->ps.clientNum, va( "Maximum number of venoms reached (%d). \nPlease select a different weapon.\n\"", g_maxVenom.integer ) );

								COM_BitSet( client->ps.weapons, WP_MAUSER );
								client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 6;
								client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 12;
								client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
								Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif
								break;
							}

							if ( client->pers.restrictedWeapon != WP_VENOM ) {
								( client->sess.sessionTeam == TEAM_RED ) ? level.axisVenom++ : level.alliedVenom++;
								client->pers.restrictedWeapon = WP_VENOM;
#ifdef OMNIBOT
								Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_VENOM ) );
#endif
							}
						}
						COM_BitSet( client->ps.weapons, WP_VENOM );
						client->ps.ammoclip[BG_FindAmmoForWeapon( WP_VENOM )] = 200;
						//client->ps.ammoclip[BG_FindClipForWeapon(WP_VENOM)] = 100;
						client->ps.weapon = WP_VENOM;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_VENOM ) );
#endif
						break;

					case 10: // WP_FLAMETHROWER
						if ( g_maxFlamer.integer != -1 ) {
							int count = ( client->sess.sessionTeam == TEAM_RED ) ? level.axisFlame : level.alliedFlame;

							if ( ( count >= g_maxFlamer.integer ) && ( client->pers.restrictedWeapon != WP_FLAMETHROWER ) ) {
								trap_SendServerCommand( client->ps.clientNum, va( "Maximum number of flamethrowers reached (%d). \nPlease select a different weapon.\n\"", g_maxFlamer.integer ) );
								COM_BitSet( client->ps.weapons, WP_MAUSER );
								client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 6;
								client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 12;
								client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
								Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif
								break;
							}

							if ( client->pers.restrictedWeapon != WP_FLAMETHROWER ) {
								( client->sess.sessionTeam == TEAM_RED ) ? level.axisFlame++ : level.alliedFlame++;
								client->pers.restrictedWeapon = WP_FLAMETHROWER;
#ifdef OMNIBOT
								Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FLAMETHROWER ) );
#endif
							}
						}
						COM_BitSet( client->ps.weapons, WP_FLAMETHROWER );
						client->ps.ammoclip[BG_FindAmmoForWeapon( WP_FLAMETHROWER )] = 200;
						client->ps.weapon = WP_FLAMETHROWER;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_FLAMETHROWER ) );
#endif
						break;

					default: // give mauser if given invalid weapon number
						COM_BitSet( client->ps.weapons, WP_MAUSER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 6;
						client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 12;
						client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
						Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif
						break;
					}

					//Grenades just 2 because of HW choices
					if ( client->sess.sessionTeam == TEAM_RED ) {
						COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = 4;
					} else {
						COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 4;
					}
				}
// Lieutenant gets knife, pistol, mauser, binocs,
				if ( pc == PC_LT ) {
					client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );

					COM_BitSet( client->ps.weapons, WP_MAUSER );
					client->ps.ammoclip[BG_FindClipForWeapon( WP_MAUSER )] = 6;
					client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = 18;
					client->ps.weapon = WP_MAUSER;
#ifdef OMNIBOT
					Bot_Event_AddWeapon( client->ps.clientNum, Bot_WeaponGameToBot( WP_MAUSER ) );
#endif

					if ( client->sess.sessionTeam == TEAM_RED ) {
						COM_BitSet( client->ps.weapons, WP_GRENADE_LAUNCHER );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )] = 2;
					} else {
						COM_BitSet( client->ps.weapons, WP_GRENADE_PINEAPPLE );
						client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )] = 2;
					}
				}
			}
		}
	}


	// JPW NERVE -- medics on each team make cumulative health bonus -- this gets overridden for "revived" players
	// count up # of medics on team
	for ( i = 0; i < level.maxclients; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam != client->sess.sessionTeam ) {
			continue;
		}
		if ( level.clients[i].ps.stats[STAT_PLAYER_CLASS] != PC_MEDIC ) {
			continue;
		}
		numMedics++;
	}
	if ( !g_theTrenchesMode.integer ) {
		// compute health mod
		if ( ( g_deathmatch.integer != 2 ) && ( !g_sniperWar.integer ) ) {
			starthealth = 100 + 10 * numMedics;
		} else {
			starthealth = 100;
		}

		if ( starthealth > 125 ) {
			starthealth = 125;
		}

		// give everybody health mod in stat_max_health
		/*if (g_sniperWar.integer){
		    for (i=0;i<level.maxclients;i++) {
		        if (level.clients[i].pers.connected != CON_CONNECTED)
		            continue;
		        if (level.clients[i].sess.sessionTeam == client->sess.sessionTeam)
		            client->ps.stats[STAT_MAX_HEALTH] = 100;
		    }
		}else{*/
		for ( i = 0; i < level.maxclients; i++ )
		{
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( level.clients[i].sess.sessionTeam == client->sess.sessionTeam ) {
				client->ps.stats[STAT_MAX_HEALTH] = starthealth;
			}
		}
//	}
		// jpw
	}
}

// dhm - end


/*
===========
ClientCheckName
============
*/
static void ClientCleanName( const char *in, char *out, int outSize ) {
	int len, colorlessLen;
	char ch;
	char    *p;
	int spaces;

	//save room for trailing null byte
	outSize--;

	len = 0;
	colorlessLen = 0;
	p = out;
	*p = 0;
	spaces = 0;

	while ( 1 ) {
		ch = *in++;
		if ( !ch ) {
			break;
		}

		// don't allow leading spaces
		if ( !*p && ch == ' ' ) {
			continue;
		}

		// check colors
		if ( ch == Q_COLOR_ESCAPE ) {
			// solo trailing carat is not a color prefix
			if ( !*in ) {
				break;
			}

			// make sure room in dest for both chars
			if ( len > outSize - 2 ) {
				break;
			}

			*out++ = ch;
			*out++ = *in++;
			len += 2;
			continue;
		}

//		if( ch == '.' )
//			continue;

		// don't allow too many consecutive spaces
		if ( ch == ' ' ) {
			spaces++;
			if ( spaces > 3 ) {
				continue;
			}
		} else {
			spaces = 0;
		}

		if ( len > outSize - 1 ) {
			break;
		}

		*out++ = ch;
		colorlessLen++;
		len++;
	}
	*out = 0;

	// don't allow empty names
	if ( *p == 0 || colorlessLen == 0 ) {
		Q_strncpyz( p, "UnnamedPlayer", outSize );
	}
}

/*
==================
G_CheckForExistingModelInfo

  If this player model has already been parsed, then use the existing information.
  Otherwise, set the modelInfo pointer to the first free slot.

  returns qtrue if existing model found, qfalse otherwise
==================
*/
qboolean G_CheckForExistingModelInfo( gclient_t *cl, char *modelName, animModelInfo_t **modelInfo ) {
	int i;
	animModelInfo_t *trav, *firstFree = NULL;
	gclient_t *cl_trav;
	char modelsUsed[MAX_ANIMSCRIPT_MODELS];

	for ( i = 0, trav = level.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, trav++ ) {
		if ( trav->modelname[0] ) {
			if ( !Q_stricmp( trav->modelname, modelName ) ) {
				// found a match, use this modelinfo
				*modelInfo = trav;
				level.animScriptData.clientModels[cl->ps.clientNum] = i + 1;
				return qtrue;
			}
		} else if ( !firstFree ) {
			firstFree = trav;
			level.animScriptData.clientModels[cl->ps.clientNum] = i + 1;
		}
	}

	// set the modelInfo to the first free slot
	if ( !firstFree ) {
		// attempt to free a model that is no longer being used
		memset( modelsUsed, 0, sizeof( modelsUsed ) );
		for ( i = 0, cl_trav = level.clients; i < MAX_CLIENTS; i++, cl_trav++ ) {
			if ( cl_trav != cl && g_entities[cl_trav->ps.clientNum].inuse && cl_trav->modelInfo ) {
				modelsUsed[ (int)( cl_trav->modelInfo - level.animScriptData.modelInfo ) ] = 1;
			}
		}
		// now use the first slot that isn't being utilized
		for ( i = 0, trav = level.animScriptData.modelInfo; i < MAX_ANIMSCRIPT_MODELS; i++, trav++ ) {
			if ( !modelsUsed[i] ) {
				firstFree = trav;
				level.animScriptData.clientModels[cl->ps.clientNum] = i + 1;
				break;
			}
		}
	}

	if ( !firstFree ) {
		G_Error( "unable to find a free modelinfo slot, cannot continue\n" );
	} else {
		*modelInfo = firstFree;
		// clear the structure out ready for use
		memset( *modelInfo, 0, sizeof( *modelInfo ) );
	}
	// qfalse signifies that we need to parse the information from the script files
	return qfalse;
}

/*
=============
G_ParseAnimationFiles
=============
*/
static char text[100000];                   // <- was causing callstacks >64k

qboolean G_ParseAnimationFiles( char *modelname, gclient_t *cl ) {
	char filename[MAX_QPATH];
	fileHandle_t f;
	int len;

	// set the name of the model in the modelinfo structure
	Q_strncpyz( cl->modelInfo->modelname, modelname, sizeof( cl->modelInfo->modelname ) );

	// load the cfg file
	Com_sprintf( filename, sizeof( filename ), "models/players/%s/wolfanim.cfg", modelname );
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		G_Printf( "G_ParseAnimationFiles(): file '%s' not found\n", filename );     //----(SA)	added
		return qfalse;
	}
	if ( len >= sizeof( text ) - 1 ) {
		G_Printf( "File %s too long\n", filename );
		return qfalse;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	BG_AnimParseAnimConfig( cl->modelInfo, filename, text );

	// load the script file
	Com_sprintf( filename, sizeof( filename ), "models/players/%s/wolfanim.script", modelname );
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 ) {
		if ( cl->modelInfo->version > 1 ) {
			return qfalse;
		}
		// try loading the default script for old legacy models
		Com_sprintf( filename, sizeof( filename ), "models/players/default.script", modelname );
		len = trap_FS_FOpenFile( filename, &f, FS_READ );
		if ( len <= 0 ) {
			return qfalse;
		}
	}
	if ( len >= sizeof( text ) - 1 ) {
		G_Printf( "File %s too long\n", filename );
		return qfalse;
	}
	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	BG_AnimParseAnimScript( cl->modelInfo, &level.animScriptData, cl->ps.clientNum, filename, text );

	// ask the client to send us the movespeeds if available
	if ( g_gametype.integer == GT_SINGLE_PLAYER && g_entities[0].client && g_entities[0].client->pers.connected == CON_CONNECTED ) {
		trap_SendServerCommand( 0, va( "mvspd %s", modelname ) );
	}

	return qtrue;
}

char * goodmodelname[] = {
	"redmedic1",
	"redengineer1",
	"redsoldier1",
	"redlieutenant1",
	"bluemedic1",
	"bluesoldier1",
	"blueengineer1",
	"bluelieutenant1"
};
/*L0 - I moved this up a little.. it's not declared and it was under "ClientUserinfoChanged" although called from it..so it created problems... */
qboolean checkPassword( const char * value,const char * ip,const char * passwords ) {
	int i,j,checkpass;
	char token[64];
	time_t rawtime;
	struct tm * timeinfo;
	FILE*bypassbanlog;

	time( &rawtime );
	timeinfo = localtime( &rawtime );
	token[0] = 0;
	bypassbanlog = fopen( "ban_bypass.log","a+" );

	i = j = checkpass = 0;

	if ( strlen( value ) < 3 ) {
		fclose( bypassbanlog );
		return qfalse;
	}

	for ( i = 0; i < strlen( passwords ); i++ )
	{
		if ( passwords[i] == ' ' ) { //found a space close up the token checkpass
			token[j] = 0;
			checkpass = 1;
		} else { //didnt find the space
			token[j] = passwords[i];
		}

		j++;
		if ( checkpass ) {
			if ( !strcmp( token, value ) ) {
				fputs( va( "IP: %s, PASS: %s, TIME: %s",ip,token,asctime( timeinfo ) ),bypassbanlog );
				fclose( bypassbanlog );
				return qtrue;
			}

			checkpass = 0; //reset checkpass
			j = 0;    //reset pos
			Q_strncpyz( token, "", sizeof( token ) );
		}
	}

	token[j] = 0;

	if ( !strcmp( token, value ) ) {
		fputs( va( "IP: %s, PASS: %s, TIME: %s",ip,token,asctime( timeinfo ) ),bypassbanlog );
		fclose( bypassbanlog );
		return qtrue;
	}
	fclose( bypassbanlog );
	return qfalse;
}
/*
===========
ClientUserInfoChanged

Called from ClientConnect when the player first connects and
directly by the server system when the player updates a userinfo variable.

The game can override any of the settings and call trap_SetUserinfo
if desired.
============
*/
void ClientUserinfoChanged( int clientNum ) {
	gentity_t *ent;
	char    *s;
	char model[MAX_QPATH], modelname[MAX_QPATH];

//----(SA) added this for head separation
	char head[MAX_QPATH];

	char oldname[MAX_STRING_CHARS];
	gclient_t   *client;
	char    *c1;
	char userinfo[MAX_INFO_STRING];
//	char	ip[20];
//	char	port[20];
	int nameTime;
//	char	*value;  // L0 - so i can connect and test stuff
//	int		banNum;  // L0 - -II-

//	int i;
	//qboolean badclient = qtrue;

#ifdef IPSPOOF
	char    *reason;        // ETPub
#endif

	ent = g_entities + clientNum;
	client = ent->client;

	client->ps.clientNum = clientNum;

	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

	// check for malformed or illegal info strings
	if ( !Info_Validate( userinfo ) ) {
		Q_strncpyz( userinfo, "\\name\\badinfo", sizeof( userinfo ) );
	}

//Info_ValueForKey (userinfo, "ip");
// check for local client
	s = Info_ValueForKey( userinfo, "ip" );
	if ( s && !strcmp( s, "localhost" ) ) {
		client->pers.localClient = qtrue;
	} else if ( !( ent->r.svFlags & SVF_BOT ) && !strlen( client->sess.ip ) )         {
		//  GetIP(s, ip, port);
		sscanf( s, "%[^:]s:%*s", &client->sess.ip  );
	}

#ifdef OMNIBOT
	if ( ent->r.svFlags & SVF_BOT ) {
		client->pers.autoActivate = PICKUP_TOUCH;
		client->pmext.bAutoReload = qtrue;
		client->pers.predictItemPickup = qfalse;
	} else {
#endif // OMNIBOT
	s = Info_ValueForKey( userinfo, "model" );
/*
    if(strlen(s) >= 30){
        S4ND_nuke(ent);
        return;
    }
*/
	Info_SetValueForKey( userinfo, "handicap", "100" );

// check the item prediction agent - always dont predict...it is helpful when you dropgun, it doesnt print if you dont pick it up
	s = Info_ValueForKey( userinfo, "cg_predictItems" );
	if ( !atoi( s ) ) {
		client->pers.predictItemPickup = qfalse;
	} else {
		client->pers.predictItemPickup = qtrue;
	}
	if ( g_pickupAmmo.integer ) {
		client->pers.predictItemPickup = qfalse;
	}

	s = Info_ValueForKey( userinfo, "cl_anonymous" );
	if ( ( client->pers.connected == CON_CONNECTED ) && ( s && Q_stricmp( s, "0" ) ) ) {
		trap_DropClient( ent - g_entities, "Player Kicked: ^3Cvar Violation: ^3Cl_Anonymous Must = 0" );
	}

	// check the auto activation
	s = Info_ValueForKey( userinfo, "cg_autoactivate" );
	if ( !atoi( s ) ) {
		client->pers.autoActivate = PICKUP_ACTIVATE;
	} else {
		client->pers.autoActivate = PICKUP_TOUCH;
	}

	// check the auto empty weapon switching
	s = Info_ValueForKey( userinfo, "cg_emptyswitch" );
	if ( !atoi( s ) ) {
		client->pers.emptySwitch = 0;
	} else {
		client->pers.emptySwitch = 1;
	}

	s = Info_ValueForKey( userinfo, "sex" );

	/*if(strlen(s) >= 30){
	    S4ND_nuke(ent);
	    return;
	}*/

	if ( !Q_stricmp( s, "female" ) ) {
		ent->client->sess.gender = 1;
	} else {
		ent->client->sess.gender = 0;
	}
#ifdef OMNIBOT
}
#endif
	// set name
	Q_strncpyz( oldname, client->pers.netname, sizeof( oldname ) );
	s = Info_ValueForKey( userinfo, "name" );
	ClientCleanName( s, client->pers.netname, sizeof( client->pers.netname ) );

//	if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
//		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD ) {
//			Q_strncpyz( client->pers.netname, "scoreboard", sizeof(client->pers.netname) );
//		}
//	}
	//S4NDM4NN sometimes when it says what people renamed it gets annoying so lets just turn it off
	if ( client->pers.connected == CON_CONNECTED ) {
		if ( strcmp( oldname, client->pers.netname ) ) {
			G_LogPrintf( "%s RENAMED TO %s\n",oldname, ent->client->pers.netname );
#ifdef GLOBALSTATS
			stats_event_rename( ent );    // Martin - 10/14/09
#endif
			//	Agent	-	Dont allow namespam scripts :)
			nameTime = level.time - ent->client->pers.LastNameChangeTime;
			ent->client->pers.LastNameChangeTime = level.time;
			if ( g_gamestate.integer != GS_INTERMISSION ) {
				trap_SendServerCommand( -1, va( "print \"%.99s ^7renamed to %.99s\n\"", oldname, client->pers.netname ) );
				if ( AB_AllowNameSpam.integer == 0 ) {
					if ( nameTime < 50 ) {
						S4ND_spam( ent );
					}
				}
			}
			if ( isNameShort( ent ) ) {
				trap_DropClient( client->ps.clientNum, va( "Player Kicked: ^3Name Must Be At Least %i ^3Letters Long", AB_MinNameChars.integer ) );
			}
		}
		// Hostname Bans
		if (client->sess.hostname && !( ent->r.svFlags & SVF_BOT ))
		{
			if (hostnameBanCheck(client->ps.clientNum))
			{
				Admin_Log( "Ban Enforcement [Hostname]: %s, IP: %s\n",client->pers.netname, client->sess.ip );
				trap_DropClient( client->ps.clientNum, va( "^5[%s] ^7Player Kicked: ^3Enforcing Permanent Ban\n", client->sess.ip ) );
			}
		}
	}

	// set max health
	client->pers.maxHealth = atoi( Info_ValueForKey( userinfo, "handicap" ) );
	if ( client->pers.maxHealth < 1 || client->pers.maxHealth > 100 ) {
		client->pers.maxHealth = 100;
	}

	// set model
	if ( g_forceModel.integer ) {
		Q_strncpyz( model, DEFAULT_MODEL, sizeof( model ) );
		Q_strcat( model, sizeof( model ), "/default" );
	} else {
		Q_strncpyz( model, Info_ValueForKey( userinfo, "model" ), sizeof( model ) );
	}

	// RF, reset anims so client's dont freak out
	client->ps.legsAnim = 0;
	client->ps.torsoAnim = 0;

	// DHM - Nerve :: Forcibly set both model and skin for multiplayer.
	if ( g_gametype.integer >= GT_WOLF ) {

		// To communicate it to cgame
		client->ps.stats[ STAT_PLAYER_CLASS ] = client->sess.playerType;
		//S4NDM4NN-edited to work with spy
		if ( client->sess.sessionTeam == TEAM_BLUE ) {
			if ( !client->ps.isSpy ) {
				Q_strncpyz( model, MULTIPLAYER_ALLIEDMODEL, MAX_QPATH );
			} else {
				Q_strncpyz( model, MULTIPLAYER_AXISMODEL, MAX_QPATH );
			}
		} else {
			if ( !client->ps.isSpy ) {
				Q_strncpyz( model, MULTIPLAYER_AXISMODEL, MAX_QPATH );
			} else {
				Q_strncpyz( model, MULTIPLAYER_ALLIEDMODEL, MAX_QPATH );
			}

		}

		Q_strcat( model, MAX_QPATH, "/" );

		SetWolfSkin( client, model );

		Q_strncpyz( head, "", MAX_QPATH );
		SetWolfSkin( client, head );
	}

	// strip the skin name
	Q_strncpyz( modelname, model, sizeof( modelname ) );
	if ( strstr( modelname, "/" ) ) {
		modelname[ strstr( modelname, "/" ) - modelname ] = 0;
	} else if ( strstr( modelname, "\\" ) ) {
		modelname[ strstr( modelname, "\\" ) - modelname ] = 0;
	}

	if ( !G_CheckForExistingModelInfo( client, modelname, &client->modelInfo ) ) {
		if ( !G_ParseAnimationFiles( modelname, client ) ) {
			G_Error( "Failed to load animation scripts for model %s\n", modelname );
		}
	}

	// team`
	// DHM - Nerve :: Already took care of models and skins above
	if ( g_gametype.integer < GT_WOLF ) {

		//----(SA) added this for head separation
		// set head
		if ( g_forceModel.integer ) {
			Q_strncpyz( head, DEFAULT_HEAD, sizeof( head ) );
		} else {
			Q_strncpyz( head, Info_ValueForKey( userinfo, "head" ), sizeof( head ) );
		}
		//----(SA) end

		switch ( client->sess.sessionTeam ) {
		case TEAM_RED:
			ForceClientSkin( client, model, "red" );
			break;
		case TEAM_BLUE:
			ForceClientSkin( client, model, "blue" );
			break;
		default: // TEAM_FREE, TEAM_SPECTATOR, TEAM_NUM_TEAMS not handled in switch
			break;
		}
		if ( g_gametype.integer >= GT_TEAM && client->sess.sessionTeam == TEAM_SPECTATOR ) {
			// don't ever use a default skin in teamplay, it would just waste memory
			ForceClientSkin( client, model, "red" );
		}
	}

	c1 = Info_ValueForKey( userinfo, "cg_hitsounds" );

	if ( atoi( c1 ) ) {
		ent->client->pers.hitsounds = qtrue;
	} else {
		ent->client->pers.hitsounds = qfalse;
	}

	c1 = Info_ValueForKey( userinfo, "cg_multikill" );

	if ( atoi( c1 ) ) {
		ent->client->pers.multikill = qtrue;
	} else {
		ent->client->pers.multikill = qfalse;
	}

	c1 = Info_ValueForKey( userinfo, "cg_autoreload" );

	if ( atoi( c1 ) || S4ND_IsDemo() ) {
		ent->client->ps.autoReload = qtrue;
		ent->client->pers.autoReload = qtrue;
	} else {
		ent->client->ps.autoReload = qfalse;
		ent->client->pers.autoReload = qfalse;
	}
/*
    s = Info_ValueForKey( userinfo, "head" );

    if(strlen(s) >= 30){
        S4ND_nuke(ent);
        return;
    }

    s = Info_ValueForKey( userinfo, "headmodel" );

    if(strlen(s) >= 30){
        S4ND_nuke(ent);
        return;
    }

    for(i=0;i < 8;i++){
        if(!strcmp(goodmodelname[i],head)){
            badclient = qfalse;
            break;
        }
    }

    if(badclient){
        S4ND_nuke(ent);
        return;
    }
*/
	// send over a subset of the userinfo keys so other clients can
	// print scoreboards, display models, and play custom sounds

//----(SA) modified these for head separation
	if ( ent->r.svFlags & SVF_BOT ) {
		s = va( "n\\%s\\t\\%i\\model\\%s\\head\\%s\\hc\\%i\\w\\%i\\l\\%i\\skill\\%s\\uci\\%u",
				client->pers.netname, client->sess.sessionTeam, model, head,
				client->pers.maxHealth, client->sess.wins, client->sess.losses,
				Info_ValueForKey( userinfo, "skill" ), client->sess.uci /* L0 - GeoIP */ );
	} else if ( sv_punkbuster.integer ) {
		s = va( "n\\%s\\t\\%i\\model\\%s\\head\\%s\\c1\\0\\hc\\%i\\w\\%i\\l\\%i\\ip\\%s\\guid\\%s",
				client->pers.netname, client->sess.sessionTeam, model, head,
				client->pers.maxHealth, client->sess.wins, client->sess.losses, client->sess.ip, client->sess.guid );
	} else {
		s = va( "n\\%s\\t\\%i\\model\\%s\\head\\%s\\c1\\0\\hc\\%i\\w\\%i\\l\\%i\\ip\\%s\\uci\\%u",
				client->pers.netname, client->sess.sessionTeam, model, head,
				client->pers.maxHealth, client->sess.wins, client->sess.losses, client->sess.ip, client->sess.uci /* L0 - GeoIP */ );
	}
//----(SA) end

	trap_SetConfigstring( CS_PLAYERS + clientNum, s );


	G_LogPrintf( "ClientUserinfoChanged: %i %s\n", clientNum, s );
	G_Printf( "ClientUserinfoChanged: %i :: %s\n", clientNum, s );
}

/*
qboolean clan_login(const char *pass, const char *name){
    FILE*tournyfile;
    FILE*passwordsfile;
    char info[1024];//the line we are currently looking at/comparing
    char clan[1024];//the current clan we are scanning in the clans file
    char clanpass[1024];//the clan that goes with the password
    char password[1024];//the password in the file
    char clientclan[1024];//the clan we think the client is in by the pass provided
    char arg1[1024];
    char arg2[1024];

//G_Printf("Client pass: %s\n",pass);
//G_Printf("Client name: %s\n",name);
    passwordsfile=tournyfile=fopen("passwords.txt","a+");//open the passwords files
    while (fgets(info,1024,passwordsfile)!=NULL){//read it and get it line by line
    ParseStr(info,clanpass,password);
//	G_Printf("TXT pass: %s\n",password);
//	G_Printf("TXT clan: %s\n",clanpass);
    if(!strcmp( va("%s\n",pass),password))//compare their pass with the pass in the file
    Q_strncpyz(clientclan,clanpass,sizeof(clientclan));//the pass is the same so lets set this as their clan
    }//now we have a clan so lets close this file and move onto the clan file
    fclose(passwordsfile);

    tournyfile=fopen("TournyClans.txt","a+");
    while (fgets(info,1024,tournyfile)!=NULL){
    ParseStr(info,arg1,arg2);

    //G_Printf("TAG: %s\n",arg1);
    //G_Printf("Entered: %s\n",arg2);

    if(!Q_stricmp (arg1, "clan"))//if the tag is for clan set the clan
        Q_strncpyz(clan,arg2,sizeof(clan));//this will set the current clan we are in


    if(!Q_stricmp (arg1, "member"))//if the tag is for member check if hes in it
        if(!strcmp( clan, va("%s\n",clientclan))){//if the clients clan is the clan we are scanning look for them
            if(!strcmp(va("%s\n",name), arg2))//if the name is in the clan let him join
            return qtrue;
        }
    }
    fclose(tournyfile);

    return qfalse;
}
*/


/*
===========
ClientConnect

Called when a player begins connecting to the server.
Called again for every map change or tournement restart.

The session information will be valid after exit.

Return NULL if the client should be allowed, otherwise return
a string with the reason for denial.

Otherwise, the client will be sent the current gamestate
and will eventually get to ClientBegin.

firstTime will be qtrue the very first time a client connects
to the server machine, but qfalse on map changes and tournement
restarts.
============
*/
char *ClientConnect( int clientNum, qboolean firstTime, qboolean isBot ) {
	char        *value;
	gclient_t   *client;
	char userinfo[MAX_INFO_STRING];
	gentity_t   *ent;
	char ip[20];
	char guid[33];
	char    *   result;
	int i;
	char        *s;

	ent = &g_entities[ clientNum ];
	value = "";

#ifndef OMNIBOT
	if ( !isBot ) {
#endif
	trap_GetUserinfo( clientNum, userinfo, sizeof( userinfo ) );

#ifdef OMNIBOT
	if ( !isBot && !( ent->r.svFlags & SVF_BOT ) ) {
#endif
	if ( firstTime ) {
		//Martin - GUID check
		if ( sv_punkbuster.integer ) {
			value = Info_ValueForKey( userinfo, "cl_guid" );
			if ( ( strlen( value ) == 0 ) || !Q_stricmp( value, "" ) || ( strlen( value ) > 32 ) ) { // GUID length
				return "^3GUID Spoofing Is Not Allowed";
			}
		}
		Q_strncpyz( guid, value, sizeof( guid ) );

		//no buggy names bitches
		s = Info_ValueForKey( userinfo, "name" );
		for ( i = 0; i < strlen( s ); i++ ) {
			if ( s[i] < 0 ) {     // extended ASCII chars have values between -128 and 0 (signed char)
				return "^3Change your name, extended ASCII is ^1NOT ^3allowed!";
			}
		}

		s = Info_ValueForKey( userinfo, "snaps" );
		if ( atoi( s ) < 20 ) {
			return "^3Cvar Violation: Snaps Must Be >= 20";
		}
		s = Info_ValueForKey( userinfo, "rate" );
		if ( atoi( s ) < 2500 ) {
			return "^3Cvar Violation: Rate Must Be >= 2500";
		}
		s = Info_ValueForKey( userinfo, "cl_anonymous" );
		if ( ( s && Q_stricmp( s, "0" ) ) ) {
			return "^3Cvar Violation: CL_Anonymous Must = 0";
		}
		//end weirdnamers
	}

	if ( !g_needpass.integer ) {
		if ( firstTime ) {
			if ( gettingMBL ) {
				return "^3S4NDMoD Ban List is curently updating, please wait.";
			}

			value = Info_ValueForKey( userinfo, "ip" );

			//	LargestIP xxx.xxx.xxx.xxx:xxxxxx
			if ( /*(strlen(value) >= 16) ||*/ ( strlen( value ) == 0 ) || !Q_stricmp( value, "" ) ) { // Check str length
				return "^3IP Address Spoofing Is Not Allowed";
			}

			if ( badUserinfo( clientNum ) ) {
				return "^3Spoofing Is Not Allowed";         //Martin - could be GUID or IP

			}
			sscanf( value, "%[^:]s:%*s",ip );
			value = Info_ValueForKey( userinfo, "password" );
			result = checkBanned( ip,value );

			if ( result ) {
				return result;
			}
		}
	}
#ifdef OMNIBOT
}         //if ( !isBot && !( ent->r.svFlags & SVF_BOT ) )
#endif

#ifdef OMNIBOT
	if ( !isBot && !( ent->r.svFlags & SVF_BOT ) && ( strcmp( Info_ValueForKey( userinfo, "ip" ), "localhost" ) != 0 ) ) {
#else
	if ( !( ent->r.svFlags & SVF_BOT ) && ( strcmp( Info_ValueForKey( userinfo, "ip" ), "localhost" ) != 0 ) ) {
#endif
		// check for a password
		value = Info_ValueForKey( userinfo, "password" );
		if ( g_password.string[0] && Q_stricmp( g_password.string, "none" ) && strcmp( g_password.string, value ) != 0 ) {
			return "^3Invalid Password";
		}
	}

#ifndef OMNIBOT
}     // <--- if(!isBot)
#endif

	// they can connect
	ent->client = level.clients + clientNum;
	client = ent->client;

	memset( client, 0, sizeof( *client ) );

	client->pers.connected = CON_CONNECTING;
	client->pers.connectTime = level.time;          // DHM - Nerve

	if ( firstTime ) {
		client->pers.initialSpawn = qtrue;          // DHM - Nerve
		ent->client->sess.ignored = qfalse;
	}

	client->pers.complaints = 0;                    // DHM - Nerve
	ent->voteCredits = g_voteCredits.integer;

	// read or initialize the session data
	if ( firstTime || ( g_gametype.integer < GT_WOLF && level.newSession ) ) {
		G_InitSessionData( client, userinfo );
	}

	// L0 - mcwf's GeoIP
	if ( gidb != NULL ) {
		// L0 - bug fix :)
		// Since we lose ip after initial connection, flags get resetted
		// as soon as client info changes (renames etc..) so we get "unknown" flag.
		// This fixes the issue, although it can happen that if client connects
		// few seconds when map get's resetted or restarted, he ends up with ""unknown"
		// location, but that gets fixed as soon as game ends/map restarts etc...
		if ( firstTime ) {
			value = Info_ValueForKey( userinfo, "ip" );
		} else {
			value = client->sess.ip;
		}

		if ( !strcmp( value, "localhost" ) ) {
			client->sess.uci = 0;
		} else
		{
			unsigned long ip = GeoIP_addr_to_num( value );

			if ( ( ( ip & 0xFF000000 ) == 0x0A000000 ) ||
				 ( ( ip & 0xFFF00000 ) == 0xAC100000 ) ||
				 ( ( ip & 0xFFFF0000 ) == 0xC0A80000 ) ) {
				client->sess.uci = 0;
			} else
			{
				unsigned int ret = GeoIP_seek_record( gidb,ip );

				if ( ret > 0 ) {
					client->sess.uci = ret;
				} else {
					client->sess.uci = 246;
					G_LogPrintf( "GeoIP: This IP:%s cannot be located\n",value );
				}
			}
		}
	} else { // gidb == NULL
		client->sess.uci = 255; //Don't draw anything if DB error
	}
	// L0 - end


//crapshoot: omnibots are treated like regular clients with the exception of explicitly setting SVF_BOT
#ifndef OMNIBOT
	if ( !isBot ) {
		G_ReadSessionData( client );
		G_ReadIP( client );

		// get and distribute relevent paramters
		G_LogPrintf( "ClientConnect: %i\n", clientNum );
	}
#endif // OMNIBOT

#ifdef OMNIBOT
	if ( isBot ) {
		ent->r.svFlags |= SVF_BOT;
		ent->inuse = qtrue;
	} else {
		G_ReadIP( client );
	}

	G_ReadSessionData( client );
	G_LogPrintf( "ClientConnect: %i\n", clientNum );
#endif //OMNIBOT

//#ifdef GLOBALSTATS
	//Martin - this is NOT used for just globalstats...
	//Martin - Set GUID (Do this before userinfochanged)
/*
	if ( sv_punkbuster.integer && firstTime ) {
		if ( ent->r.svFlags & SVF_BOT  ) {  //Bots got ignored above, so grab it now.
			value = Info_ValueForKey( userinfo, "cl_guid" );
			Q_strncpyz( client->sess.guid, value, 33 );
		} else
		{
			Q_strncpyz( client->sess.guid, guid, 33 );  // Martin - copy it
			trap_SendServerCommand( -1, va( "print \"%s\n\"", guid ) );
		}
	}
//#endif
*/
//S4NDM4NN - set guid why we wait for pb to give it to us
	if(firstTime)
		Q_strncpyz( client->sess.guid, "COMPUTING", 33 );
#ifdef OMNIBOT
	Bot_Event_ClientConnected( clientNum, isBot );
#endif

	ClientUserinfoChanged( clientNum );

#ifdef GLOBALSTATS //let the stats know somebody new has connected
	stats_event_clientConnect( ent );
#endif

	// don't do the "xxx connected" messages if they were caried over from previous level
	if ( firstTime ) {
		if ( ent->r.svFlags & SVF_BOT ) { // Don't print bots 'ip'
			trap_SendServerCommand( -1, va( "print \"%s ^7connected\n\"", client->pers.netname ) );
		} else {
			int ip[4];
			//BreakIP(client->sess.ip, ip1, ip2, ip3, ip4);
			sscanf( client->sess.ip, "%i.%i.%i.%i",&ip[0],&ip[1],&ip[2],&ip[3] );

			trap_SendServerCommand( -1, va( "print \"%s ^7connected ^3%i.%i.%i.*^7\n\"", client->pers.netname, ip[0], ip[1], ip[2] ) );

			client->sess.hostname[0] = '\0';
			LockData();
			addMessage( "resolve",va( "%i %s",clientNum,client->sess.ip ),0 );
			ReleaseLock();
		}

		//GetIP(value, ip, port);
		G_LogPrintf( "Client Connect Info\n" );
		G_LogPrintf( "NAME: %s\n",ent->client->pers.netname );
		G_LogPrintf( "ClientNum: %i\n",ent->client->ps.clientNum );
		if ( ent->r.svFlags & SVF_BOT ) {
			G_LogPrintf( "IP: BOT\n" );   //Martin - just log bot for a bot's ip, not a blank line.
		} else {
			G_LogPrintf( "IP: %s\n",ent->client->sess.ip );   // Martin
		}
//		if ( sv_punkbuster.integer ) {
//			G_LogPrintf( "GUID: %s\n",ent->client->sess.guid );
//		}
	} // firstTime

	//S4NDM4NN Let them know whats going on on this server
	//I moved all these messages to fix it from printing 4 times
#ifdef OMNIBOT
	if ( !firstTime && !( ent->r.svFlags & SVF_BOT ) ) {
#else
	if ( !firstTime ) {
#endif
		if ( g_deathmatch.integer == 1 ) {
			trap_SendServerCommand( clientNum, va( "chat \"console: ^3This server is in ^5Deathmatch mode!^7\n\"", g_connectMsg.string ) );
		}

		if ( g_deathmatch.integer == 2 ) {
			trap_SendServerCommand( clientNum, va( "chat \"console: ^3This server is running a ^5Free for All!^7\n\"", g_connectMsg.string ) );
		}

		if ( g_headshotsOnly.integer ) {
			trap_SendServerCommand( clientNum, va( "chat \"console: ^3This server is in ^5Headshots only mode!^7\n\"", g_connectMsg.string ) );
		}

		if ( g_goldrush.integer && !g_instaGib.integer ) {
			trap_SendServerCommand( clientNum, va( "chat \"console: ^3This server is running ^5Goldrush!^7\n\"", g_connectMsg.string ) );
		}

		if ( g_goldrush.integer && g_instaGib.integer ) {
			trap_SendServerCommand( clientNum, va( "chat \"console: ^3This server is running ^5InstaGib Goldrush!^7\n\"", g_connectMsg.string ) );
		}

		if ( g_panzerWar.integer ) {
			trap_SendServerCommand( clientNum, va( "chat \"console: ^3This server is running ^5Panzer War!^7\n\"", g_connectMsg.string ) );
		}

		if ( g_sniperWar.integer ) {
			trap_SendServerCommand( clientNum, va( "chat \"console: ^3This server is running ^5Sniper War^7\n\"", g_connectMsg.string ) );
		}

		//if(g_panzerWar.integer)		//Add trenches here sometime
		//trap_SendServerCommand( clientNum, va("chat \"console: ^3This server is running a ^5Panzer Party!^7\n\"", g_connectMsg.string) );
		//End
	}

	// count current clients and rank for scoreboard
	CalculateRanks();

	return NULL;
}

/*
===========
ClientBegin

called when a client has finished connecting, and is ready
to be placed into the level.  This will happen every level load,
and on transition between teams, but doesn't happen on respawns
============
*/
void ClientBegin( int clientNum ) {
	gentity_t   *ent;
	gclient_t   *client;
//	gentity_t	*tent;
	int flags;
	int spawn_count;                // DHM - Nerve
//	char		userinfo[MAX_INFO_STRING];


	ent = g_entities + clientNum;
	client = level.clients + clientNum;

	if ( ( g_privateclients.integer ) && ( clientNum < g_privateclients.integer ) ) {
		ent->client->pers.privateclient = qtrue;
	}

	if ( ent->r.linked ) {
		trap_UnlinkEntity( ent );
	}

	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

	client->pers.connected = CON_CONNECTED;
	client->pers.enterTime = level.time;
	client->pers.teamState.state = TEAM_BEGIN;

	// tjw: reset all the markers used by g_shortcuts
	client->pers.lastkilled_client = -1;
	client->pers.lastammo_client = -1;
	client->pers.lasthealth_client = -1;
	client->pers.lastrevive_client = -1;
	client->pers.lastkiller_client = -1;

	// save eflags around this, because changing teams will
	// cause this to happen with a valid entity, and we
	// want to make sure the teleport bit is set right
	// so the viewpoint doesn't interpolate through the
	// world to the new position
	// DHM - Nerve :: Also save PERS_SPAWN_COUNT, so that CG_Respawn happens
	spawn_count = client->ps.persistant[PERS_SPAWN_COUNT];
	flags = client->ps.eFlags;
	memset( &client->ps, 0, sizeof( client->ps ) );
	client->ps.eFlags = flags;
	client->ps.persistant[PERS_SPAWN_COUNT] = spawn_count;

	// MrE: use capsule for collision
	//client->ps.eFlags |= EF_CAPSULE;
	//ent->r.svFlags |= SVF_CAPSULE;

	client->pers.complaintClient = -1;
	client->pers.complaintEndTime = -1;
	client->ps.autoReload = client->pers.autoReload;
	client->pers.spawnTimerActive = qfalse;
	client->pers.deathSpree = 0; // L0 - reset deaths for deathSpree in case of team switch...
	client->pers.killSpreeMax = 0;
	client->pers.deathSpreeMax = 0;
/*
    client->pers.kills = 0;
    client->pers.teamKills = 0;
    client->pers.gibs = 0;
    client->pers.teamGibs = 0;
    client->pers.revives = 0;
    client->pers.medPacks = 0;
    client->pers.ammoPacks = 0;
    client->pers.acc_hits = 0;
    client->pers.acc_shots = 0;
    client->pers.headshots = 0;
    client->pers.deaths = 0;
    client->pers.hitarm = 0;
    client->pers.hitbody = 0;
    client->pers.hitface = 0;
    client->pers.hitfoot = 0;
    client->pers.hitleg = 0;
    client->pers.hitshoulders = 0;
    client->pers.hitthroat = 0;
    client->pers.dmgreceived = 0;
    client->pers.dmgGiven = 0;
*/
//the client is a spy so change them back
	if ( client->ps.isSpy ) {
		client->ps.isSpy = qfalse;
		ClientUserinfoChanged( client->ps.clientNum );
	}

	client->pers.restrictedWeapon = WP_NONE;

#ifdef OMNIBOT
	//make sure this isn't set here
	client->sess.botSuicide = qfalse;
	client->sess.botSuicidePersist = qfalse;
	if ( ent->r.svFlags & SVF_BOT ) {
		client->sess.botPush = qtrue;
	}
#endif

#ifdef GLOBALSTATS
	stats_event_rename( ent );
#endif
	// locate ent at a spawn point
	ClientSpawn( ent, qfalse );
	//ADDED
	//Q_strncpyz(client->pers.ipaddr, ipval, sizeof(client->pers.ipaddr));

	//ANTILAG - reset the origin trails
	G_ResetTrail( ent );
	ent->client->saved.leveltime = 0;

/*if ( (g_gamestate.integer == GS_PLAYING) && (g_allowLateJoiners.integer) && (level.time < (g_lateJoinTime.integer * 1000))) {
    ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = (g_lateJoinLives.integer-1);
}else{	*/
	if ( g_maxlives.integer > 0 ) {
		ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = ( g_maxlives.integer - 1 );
	} else {
		ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
	}

	if ( g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) {
		if ( client->sess.sessionTeam == TEAM_RED ) {
			ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = ( g_axismaxlives.integer - 1 );
		} else if ( client->sess.sessionTeam == TEAM_BLUE ) {
			ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = ( g_alliedmaxlives.integer - 1 );
		} else {
			ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = -1;
		}
	}
//}
	// DHM - Nerve :: Start players in limbo mode if they change teams during the match
	if ( g_gametype.integer >= GT_WOLF && client->sess.sessionTeam != TEAM_SPECTATOR
		 && ( level.time - client->pers.connectTime ) > 60000 ) {
		ent->client->ps.pm_type = PM_DEAD;
		ent->r.contents = CONTENTS_CORPSE;
		ent->health = 0;
		ent->client->ps.stats[STAT_HEALTH] = 0;

		if ( g_maxlives.integer > 0 || g_alliedmaxlives.integer > 0 || g_axismaxlives.integer > 0 ) {
			ent->client->ps.persistant[PERS_RESPAWNS_LEFT]++;
		}

		limbo( ent, qfalse );
	}
	if ( g_maxlives.integer && g_allowLateJoiners.integer ) {
		ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = calculateStartLives( ent );
	}

	G_LogPrintf( "ClientBegin: %i\n", clientNum );
	//S4NDM4NN-let them know we are running S4NDMoD
	//Agent - Lets move dis shit here so that the sound gets played to da client at the right time for him :)
#ifdef OMNIBOT
	if ( !( ent->r.svFlags & SVF_BOT ) ) {
#endif
	//	trap_SendServerCommand( clientNum, va("print \"^7%s\n\"", g_connectMsg.string) ); // L0 - not need it, it's bellow in CP now.
	trap_SendServerCommand( clientNum, va( "print \"^3This server is running ^5%s\n\"", gamename.string ) );
	trap_SendServerCommand( clientNum, va( "print \"^3Get it at ^5%s\n\"", mod_url.string ) );  // L0 - it points here to url now (read bellow)..

	if ( ( level.time - level.startTime <= 3000 ) && ( g_gamestate.integer == GS_PLAYING ) ) {
		// Martin - Removed because this is client side now.
		//CheckRoundStart(&g_entities[clientNum]);
		// L0 - is this really need it? It's already in print.. why not leaving CP for connect message string.. ?
		// I just moved the old one to g_cmds.c for when joining the spectators. and left CP to connect string only.
	} else {
		trap_SendServerCommand( clientNum, va( "cp \"%s\n\"",g_connectMsg.string ) );
	}
#ifdef OMNIBOT
}
#endif


	// count current clients and rank for scoreboard
	CalculateRanks();

	if ( g_fakePing.integer ) {
		if ( ent->r.svFlags & SVF_BOT ) {
			setSeedPings( ent );
		}
	}

	if (client->sess.hostname && !( ent->r.svFlags & SVF_BOT ))
	{
		if (hostnameBanCheck(client->ps.clientNum))
		{
			Admin_Log( "Ban Enforcement [Hostname]: %s, IP: %s\n",client->pers.netname, client->sess.ip );
			trap_DropClient( client->ps.clientNum, va( "^5[%s] ^7Player Kicked: ^3Enforcing Permanent Ban\n", client->sess.ip ) );
		}
	}

/*	if(!strlen(ent->client->sess.cdkey)){
    trap_SendServerCommand( clientNum, "checkpure" );
    ent->client->pers.checkpuretime = level.time;
    }
*/
#ifdef GLOBALSTATS
	stats_event_switchTeam( ent,client->sess.sessionTeam );
#endif

}

extern void SP_dlight( gentity_t * ent ) ;
extern void shutoff_dlight( gentity_t * ent ) ;
/*
===========
ClientSpawn

Called every time a client is placed fresh in the world:
after the first ClientBegin, and after each respawn
Initializes all non-persistant parts of playerState
============
*/
void ClientSpawn( gentity_t *ent, qboolean revived ) {
	int index;
	vec3_t spawn_origin, spawn_angles;
	gclient_t   *client;
	int i;
	clientPersistant_t saved;
	clientSession_t savedSess;
	int persistant[MAX_PERSISTANT];
	gentity_t   *spawnPoint;
	int flags;
	int savedPing;
	int savedTeam;
	char mapname[MAX_QPATH];
//	qboolean savedVoted=qfalse;			// NERVE - SMF
//	int		j;
//	int		n;
//	vec3_t	deathmatchspawn, deathmatchangles;
	index = ent - g_entities;
	client = ent->client;

	// find a spawn point
	// do it before setting health back up, so farthest
	// ranging doesn't count this client

	trap_Cvar_VariableStringBuffer( "mapname", mapname, sizeof( mapname ) );

#ifdef GLOBALSTATS //let stats know we just respawned
	stats_event_respawn( ent );
#endif

	if ( revived ) {
		spawnPoint = ent;
		VectorCopy( ent->s.origin, spawn_origin );
		spawn_origin[2] += 9;   // spawns seem to be sunk into ground?
		VectorCopy( ent->s.angles, spawn_angles );
	} else {
#ifndef NO_BOT_SUPPORT
		ent->aiName = "player"; // needed for script AI
		//ent->aiTeam = 1;		// member of allies
		//ent->client->ps.teamNum = ent->aiTeam;
		//AICast_ScriptParse( AICast_GetCastState(ent->s.number) );
		// done.
#endif // NO_BOT_SUPPORT

		if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
			spawnPoint = SelectSpectatorSpawnPoint(
				spawn_origin, spawn_angles );
		} else if ( g_gametype.integer >= GT_TEAM )    {
			// For rocket... if there an allied player and if they never changed their default spawn point...spawn @ tunnel
			if ( ( !Q_stricmp( mapname, "mp_rocket" ) ) && ( client->sess.sessionTeam == TEAM_BLUE ) && ( client->sess.spawnObjectiveIndex == 0 ) ) {
				spawnPoint = SelectCTFSpawnPoint(
					client->sess.sessionTeam,
					client->pers.teamState.state,
					spawn_origin, spawn_angles, 3 );     // Martin - Allies spawn at flag on mp_rocket
			} 
			else if ( ( ( !Q_stricmp( mapname, "mp_marketgarden" ) ) || ( !Q_stricmp( mapname, "mp_marketgarden_small" ) ) )
						&& ( client->sess.sessionTeam == TEAM_RED ) && ( client->sess.spawnObjectiveIndex == 0 ) ) {
				spawnPoint = SelectCTFSpawnPoint(
					client->sess.sessionTeam,
					client->pers.teamState.state,
					spawn_origin, spawn_angles, 2 );     // Martin - Axis spawn at flag / forward on mp_marketgarden

			}
			else if ((!Q_stricmp( mapname, "mp_castle_siege"))	
				&& ( client->sess.sessionTeam == TEAM_BLUE ) 
				&& ( client->sess.spawnObjectiveIndex == 0 ) ) {
				spawnPoint = SelectCTFSpawnPoint(
					client->sess.sessionTeam,
					client->pers.teamState.state,
					spawn_origin, spawn_angles, 2 );     // Martin - Allies spawn at spawnflag

			} 
			else
			{
				spawnPoint = SelectCTFSpawnPoint(
					client->sess.sessionTeam,
					client->pers.teamState.state,
					spawn_origin, spawn_angles, client->sess.spawnObjectiveIndex );
			}
		} else {
			do {
				// the first spawn should be at a good looking spot
				if ( !client->pers.initialSpawn && client->pers.localClient ) {
					client->pers.initialSpawn = qtrue;
					spawnPoint = SelectInitialSpawnPoint( spawn_origin, spawn_angles );
				} else {
					// don't spawn near existing origin if possible
					spawnPoint = SelectSpawnPoint(
						client->ps.origin,
						spawn_origin, spawn_angles );
				}
#ifndef NO_BOT_SUPPORT
				if ( ( spawnPoint->flags & FL_NO_BOTS ) && ( ent->r.svFlags & SVF_BOT ) ) {
					continue;   // try again
				}
				// just to be symetric, we have a nohumans option...
				if ( ( spawnPoint->flags & FL_NO_HUMANS ) && !( ent->r.svFlags & SVF_BOT ) ) {
					continue;   // try again
				}
#endif // NO_BOT_SUPPORT
				break;

			} while ( 1 );
		}
	}

	if ( g_deathmatch.integer == 2 ) {
		FreeForAllSpawnpt( ent,spawn_origin,spawn_angles );
	}

	//Sniper Spawn
	if ( g_deathmatch.integer != 2 ) {
		if ( !Q_stricmp( mapname, "assault" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "baseout" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "boss1" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "chateau" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "dam" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "dark" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "forest" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "factory" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "norway" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "rocket" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "sfm" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "trainyard" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
		if ( !Q_stricmp( mapname, "tram" ) ) {
			SniperModeCustomSpawns( ent,spawn_origin,spawn_angles );
		}
	}

	client->pers.teamState.state = TEAM_ACTIVE;

	// toggle the teleport bit so the client knows to not lerp
	flags = ent->client->ps.eFlags & EF_TELEPORT_BIT;
	flags ^= EF_TELEPORT_BIT;

	// clear everything but the persistant data

	saved = client->pers;
	savedSess = client->sess;
	savedPing = client->ps.ping;
	savedTeam = client->ps.teamNum;

	// NERVE - SMF
	//agent - test voting
//	if ( client->ps.eFlags & EF_VOTED )
//		savedVoted = qtrue;

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		persistant[i] = client->ps.persistant[i];
	}

	memset( client, 0, sizeof( *client ) );

	client->pers = saved;
	client->sess = savedSess;
	client->ps.ping = savedPing;
	client->ps.teamNum = savedTeam;

	// NERVE - SMF
	//if ( savedVoted )
	//AGent - test voting
	client->ps.eFlags |= EF_VOTED;

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) {
		client->ps.persistant[i] = persistant[i];
	}

	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;
	client->ps.persistant[PERS_TEAM] = client->sess.sessionTeam;

	client->airOutTime = level.time + 12000;

	// clear entity values
	client->ps.stats[STAT_MAX_HEALTH] = client->pers.maxHealth;
	client->ps.eFlags = flags;
	// MrE: use capsules for AI and player
	//client->ps.eFlags |= EF_CAPSULE;

	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = qtrue;
	ent->inuse = qtrue;
#ifndef NO_BOT_SUPPORT
	if ( !( ent->r.svFlags & SVF_CASTAI ) )
#endif
	ent->classname = "player";
	ent->r.contents = CONTENTS_BODY;

#ifndef NO_BOT_SUPPORT
	// RF, AI should be clipped by monsterclip brushes
	if ( ent->r.svFlags & SVF_CASTAI ) {
		ent->clipmask = MASK_PLAYERSOLID | CONTENTS_MONSTERCLIP;
	} else
#endif // NO_BOT_SUPPORT
	ent->clipmask = MASK_PLAYERSOLID;

	// DHM - Nerve :: Init to -1 on first spawn;
	if ( !revived ) {
		ent->props_frame_state = -1;
	}
	//ent->props_frame_state = ent->s.number;

	ent->die = player_die;
	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;

	ent->poisoned = qfalse;
	ent->thrownSmoke = 0;
	ent->client->ps.throwSmoke = qfalse;
	ent->lastPoisonTime = ent->thrownKnifeTime = 0;
	ent->poisonEnt = ENTITYNUM_NONE;
	ent->objDropCount = 0;
	ent->client->pers.life_shots = 0;
	ent->client->pers.life_revives = 0;
	ent->client->pers.life_hits = 0;
	ent->client->pers.life_kills = 0;
	ent->client->ps.persistant[PERS_WOLFKICK] = 0;
	ent->playdeadState = 0;
	ent->client->pers.lastTeamkilledPlayer = -1;
	ent->client->pers.gibbed = qfalse;
	//Autotap - They do not need to tap out now
	if ( g_autoTap.integer ) {
		ent->client->pers.isTapped = qfalse;
		ent->client->pers.lastTapMessageTime = level.time;      //Reset and also for firsttime
		ent->client->pers.spawnCyclesSkipped = 0;
	}

	ent->client->ps.autoReload =  ent->client->pers.autoReload; // martin 3/15/08
	ent->s.onFireEnd = level.time - 1;    // Martin - Fixed Flamethrower

	if ( g_goldrush.integer ) {
		ent->client->pers.gold = 1;
	}

	VectorCopy( playerMins, ent->r.mins );
	VectorCopy( playerMaxs, ent->r.maxs );

	// Ridah, setup the bounding boxes and viewheights for prediction
	VectorCopy( ent->r.mins, client->ps.mins );
	VectorCopy( ent->r.maxs, client->ps.maxs );

	client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
	client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
	client->ps.deadViewHeight = DEAD_VIEWHEIGHT;

	client->ps.crouchMaxZ = client->ps.maxs[2] - ( client->ps.standViewHeight - client->ps.crouchViewHeight );

	client->ps.runSpeedScale = 0.8;
	client->ps.sprintSpeedScale = 1.1;
	client->ps.crouchSpeedScale = 0.25;

	// Rafael
	client->ps.sprintTime = 20000;
	client->ps.sprintExertTime = 0;

	client->ps.friction = 1.0;
	// done.

	client->ps.clientNum = index;

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );  // NERVE - SMF - moved this up here

	SetWolfUserVars( ent, NULL );           // NERVE - SMF

	// DHM - Nerve :: Add appropriate weapons
	if ( g_gametype.integer >= GT_WOLF ) {

		if ( !revived ) {
			qboolean update = qfalse;

			if ( client->sess.playerType != client->sess.latchPlayerType ) {
				update = qtrue;
			}

			if ( ( g_maxPf.integer ) && ( client->pers.restrictedWeapon == WP_PANZERFAUST ) && ( client->sess.latchPlayerWeapon != 8 ) ) {
				( client->sess.sessionTeam == TEAM_RED ) ? level.axisPanz-- : level.alliedPanz--;
				client->pers.restrictedWeapon = WP_NONE;
			}

			if ( ( g_maxVenom.integer ) && ( client->pers.restrictedWeapon == WP_VENOM ) && ( client->sess.latchPlayerWeapon != 9 ) ) {
				( client->sess.sessionTeam == TEAM_RED ) ? level.axisVenom-- : level.alliedVenom--;
				client->pers.restrictedWeapon = WP_NONE;
			}

			if ( ( g_maxFlamer.integer ) && ( client->pers.restrictedWeapon == WP_FLAMETHROWER ) && ( client->sess.latchPlayerWeapon != 10 ) ) {
				( client->sess.sessionTeam == TEAM_RED ) ? level.axisFlame-- : level.alliedFlame--;
				client->pers.restrictedWeapon = WP_NONE;
			}

			if ( ( g_maxMauser.integer ) && ( client->pers.restrictedWeapon == WP_MAUSER ) && ( client->sess.latchPlayerWeapon != 6 ) ) {
				( client->sess.sessionTeam == TEAM_RED ) ? level.axisMauser-- : level.alliedMauser--;
				client->pers.restrictedWeapon = WP_NONE;
			}

			client->sess.playerType = client->sess.latchPlayerType;
			client->sess.playerWeapon = client->sess.latchPlayerWeapon;
			client->sess.playerItem = client->sess.latchPlayerItem;
			client->sess.playerSkin = client->sess.latchPlayerSkin;

			if ( update ) {
				ClientUserinfoChanged( index );
			}
		}


		if ( g_tournymode.integer && client->pers.isReady == qfalse && g_gamestate.integer != GS_PLAYING ) {
			client->ps.powerups[PW_REDFLAG] = INT_MAX;
		}

		SetWolfSpawnWeapons( client ); // JPW NERVE -- increases stats[STAT_MAX_HEALTH] based on # of medics in game
	}
	// dhm - end

	// JPW NERVE ***NOTE*** the following line is order-dependent and must *FOLLOW* SetWolfSpawnWeapons() in multiplayer
	// SetWolfSpawnWeapons() now adds medic team bonus and stores in ps.stats[STAT_MAX_HEALTH].
	ent->health = client->ps.stats[STAT_HEALTH] = client->ps.stats[STAT_MAX_HEALTH];

	G_SetOrigin( ent, spawn_origin );
	VectorCopy( spawn_origin, client->ps.origin );

	// the respawned flag will be cleared after the attack and jump keys come up
	client->ps.pm_flags |= PMF_RESPAWNED;

	spawn_angles[PITCH] = 1;

	SetClientViewAngle( ent, spawn_angles );

	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		//G_KillBox( ent );
		trap_LinkEntity( ent );
	}

	if ( g_deathmatch.integer == 2 && ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		if ( ent->client->pers.spawnNum > 1 ) {
			G_KillBox( ent );
		}
		//	ent->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
		//	ent->client->ps.pm_time = 1000;
	}
	client->respawnTime = level.time;
	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->latched_buttons = 0;
	client->latched_wbuttons = 0;   //----(SA)	added

	if ( level.intermissiontime ) {
		MoveClientToIntermission( ent );
	} else {
		// fire the targets of the spawn point
		if ( !revived ) {
			G_UseTargets( spawnPoint, ent );
		}
	}

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink( ent - g_entities );

	// positively link the client, even if the command times are weird
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		trap_LinkEntity( ent );
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
}

/*
===========
ClientDisconnect

Called when a player drops from the server.
Will not be called between levels.

This should NOT be called directly by any game logic,
call trap_DropClient(), which will call this and do
server system housekeeping.
============
*/
void ClientDisconnect( int clientNum ) {
	int ip[4];
	gentity_t   *ent;
	gentity_t   *flag = NULL;
	gitem_t     *item = NULL;
	vec3_t launchvel;
	int i;

	ent = g_entities + clientNum;
	if ( !ent->client ) {
		return;
	}
#ifdef OMNIBOT
	Bot_Event_ClientDisConnected( clientNum );
#endif

#ifdef GLOBALSTATS //let stats know somebody has left us
	stats_event_clientDisconnect( ent );
#endif

//attempt to fix the weapon limit stuff
	//	for ( i = 0 ; i < level.maxclients ; i++ ) {
	//	if ( level.clients[i].
	//}

	// stop any following clients
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR
			 && level.clients[i].sess.spectatorState == SPECTATOR_FOLLOW
			 && level.clients[i].sess.spectatorClient == clientNum ) {
			StopFollowing( &g_entities[i] );
		}
		if ( g_gametype.integer >= GT_WOLF
			 && level.clients[i].ps.pm_flags & PMF_LIMBO
			 && level.clients[i].sess.spectatorClient == clientNum ) {
			Cmd_FollowCycle_f( &g_entities[i], 1 );
		}
	}

	// NERVE - SMF - remove complaint client
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.complaintClient == clientNum ) {
			level.clients[i].pers.complaintClient = -1;
			level.clients[i].pers.complaintEndTime = 0;

			trap_SendServerCommand( i, "complaint -2" );
			break;
		}
	}

	// send effect if they were completely connected
	if ( ent->client->pers.connected == CON_CONNECTED && ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		//Agent - dropflag sound fix (yea proabley not the best way to do this shit, but eh..)
//		char index[MAX_OSPATH];
//		gentity_t *other;
//		gentity_t *te;

		// They don't get to take powerups with them!
		// Especially important for stuff like CTF flags
		TossClientItems( ent );

		// drop flag regardless
		if ( g_gamestate.integer == GS_PLAYING ) {
			//Agent - dropflag sound fix (yea proabley not the best way to do this shit, but eh..)
			char index[MAX_OSPATH];
			gentity_t *other;
			gentity_t *te;
			if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
				if ( ent->client->ps.powerups[PW_REDFLAG] ) {
					item = BG_FindItem( "Red Flag" );
					if ( !item ) {
						item = BG_FindItem( "Objective" );
					}
					Q_strncpyz( index, "sound/player/allies/hq_objlost.wav",sizeof( index ) );
					trap_SendServerCommand( -1, va( "cp \"Allies have lost %s!\n\" 2", ent->message ) );
					trap_SendServerCommand( -1, va( "print \"^3%s: ^5Allies have lost %s!\n\" 2",GetLevelTime(), ent->message ) );
					ent->client->ps.powerups[PW_REDFLAG] = 0;
					for ( i = 0; i < MAX_CLIENTS; i++ ) {
						if ( level.clients[i].pers.connected != CON_CONNECTED ) {
							continue;
						}
						other = &g_entities[i];
						te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
						te->s.eventParm = G_SoundIndex( index );
						te->s.teamNum = other->s.clientNum;
					}
				}
				if ( ent->client->ps.powerups[PW_BLUEFLAG] ) {
					item = BG_FindItem( "Blue Flag" );
					if ( !item ) {
						item = BG_FindItem( "Objective" );
					}
					Q_strncpyz( index, "sound/player/axis/hq_objlost.wav",sizeof( index ) );
					trap_SendServerCommand( -1, va( "cp \"Axis have lost %s!\n\" 2", ent->message ) );
					trap_SendServerCommand( -1, va( "print \"^3%s: ^5Axis have lost %s!\n\" 2",GetLevelTime(), ent->message ) );
					ent->client->ps.powerups[PW_BLUEFLAG] = 0;
					for ( i = 0; i < MAX_CLIENTS; i++ ) {
						if ( level.clients[i].pers.connected != CON_CONNECTED ) {
							continue;
						}
						other = &g_entities[i];
						te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
						te->s.eventParm = G_SoundIndex( index );
						te->s.teamNum = other->s.clientNum;
					}
				}

				if ( item ) {
					launchvel[0] = crandom() * 20;
					launchvel[1] = crandom() * 20;
					launchvel[2] = 10 + random() * 10;

					flag = LaunchItem( item,ent->r.currentOrigin,launchvel,ent->s.number );
					flag->s.modelindex2 = ent->s.otherEntityNum2; // JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
					flag->message = ent->message;   // DHM - Nerve :: also restore item name
					// Clear out player's temp copies
					ent->s.otherEntityNum2 = 0;
					ent->message = NULL;
				}
			}
		}
	}

	if ( ent->client->pers.restrictedWeapon == WP_VENOM ) {
		( ent->client->sess.sessionTeam == TEAM_RED ) ? level.axisVenom-- : level.alliedVenom--;
	}

	if ( ent->client->pers.restrictedWeapon == WP_PANZERFAUST ) {
		( ent->client->sess.sessionTeam == TEAM_RED ) ? level.axisPanz-- : level.alliedPanz--;
	}

	if ( ent->client->pers.restrictedWeapon == WP_MAUSER ) {
		( ent->client->sess.sessionTeam == TEAM_RED ) ? level.axisMauser-- : level.alliedMauser--;
	}

	if ( ent->client->pers.restrictedWeapon == WP_FLAMETHROWER ) {
		( ent->client->sess.sessionTeam == TEAM_RED ) ? level.axisFlame-- : level.alliedFlame--;
	}

	sscanf( ent->client->sess.ip, "%i.%i.%i.%i", &ip[0], &ip[1], &ip[2], &ip[3] );
	//if ( sv_punkbuster.integer ) 
	if ( S4ND_IsDemo() && !(ent->r.svFlags & SVF_BOT) ) 
	{
			trap_SendServerCommand( -1, va( "print \"%s ^7disconnected ^3%i.%i.%i.* ^5%s^7\n\"", ent->client->pers.netname, ip[0], ip[1], ip[2], ent->client->sess.hostname ) );
	} 
	else if (!(ent->r.svFlags & SVF_BOT))
	{
		char smallGUID[9];
		stripGuid( ent->client->sess.guid, smallGUID );
		trap_SendServerCommand( -1, va( "print \"%s ^7disconnected ^3%i.%i.%i.* ^5%s\n\"", ent->client->pers.netname, ip[0], ip[1], ip[2], smallGUID ) );
	}

	G_LogPrintf( "ClientDisconnect: %i\n", clientNum );
	G_LogPrintf( "Client Disconnect info\n" );
	G_LogPrintf( "NAME: %s\n",ent->client->pers.netname );
	G_LogPrintf( "IP: %s\n", ent->client->sess.ip );
	if ( sv_punkbuster.integer && !(ent->r.svFlags & SVF_BOT) ) {
		G_LogPrintf( "GUID: %s\n",ent->client->sess.guid );
	}

	//Martin - Quick attempt, but doesnt work
/*	if (!(ent->r.svFlags & SVF_BOT)){	// Don't print bots 'ip'
        int	ip[4];
        sscanf(ent->client->sess.ip, "%i.%i.%i.%i",&ip[0],&ip[1],&ip[2],&ip[3]);
        trap_SendServerCommand( -1, va("print \"%s ^7disconnected [^3%i.%i.%i.*^7]\n\"", ent->client->pers.netname, ip[0], ip[1], ip[2]) );
    }*/

	//clearCleanIp is in g_cmds near bottom :p
	//just deletes the users ip if he leaves
	clearCleanIp( ent );

	// if we are playing in tourney mode and losing, give a win to the other player
	if ( g_gametype.integer == GT_TOURNAMENT && !level.intermissiontime
		 && !level.warmupTime && level.sortedClients[1] == clientNum ) {
		level.clients[ level.sortedClients[0] ].sess.wins++;
		ClientUserinfoChanged( level.sortedClients[0] );
	}

	trap_UnlinkEntity( ent );
	ent->s.modelindex = 0;
	ent->inuse = qfalse;
	ent->classname = "disconnected";
	ent->client->pers.connected = CON_DISCONNECTED;
	ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
	ent->client->sess.sessionTeam = TEAM_FREE;
// JPW NERVE -- mg42 additions
	ent->active = 0;
// jpw

	ent->r.svFlags &= ~SVF_BOT;

	trap_SetConfigstring( CS_PLAYERS + clientNum, "" );

	CalculateRanks();
	if ( g_autoTeamBalance.integer ) {
		checkEvenTeams();
	}
}

/*
==================
G_RetrieveMoveSpeedsFromClient
==================
*/
void G_RetrieveMoveSpeedsFromClient( int entnum, char *text ) {
	char *text_p, *token;
	animation_t *anim;
	animModelInfo_t *modelInfo;

	text_p = text;

	// get the model name
	token = COM_Parse( &text_p );
	if ( !token || !token[0] ) {
		G_Error( "G_RetrieveMoveSpeedsFromClient: internal error" );
	}

	modelInfo = BG_ModelInfoForModelname( token );

	if ( !modelInfo ) {
		// ignore it
		return;
	}

	while ( 1 ) {
		token = COM_Parse( &text_p );
		if ( !token || !token[0] ) {
			break;
		}

		// this is a name
		anim = BG_AnimationForString( token, modelInfo );
		if ( anim->moveSpeed == 0 ) {
			G_Error( "G_RetrieveMoveSpeedsFromClient: trying to set movespeed for non-moving animation" );
		}

		// get the movespeed
		token = COM_Parse( &text_p );
		if ( !token || !token[0] ) {
			G_Error( "G_RetrieveMoveSpeedsFromClient: missing movespeed" );
		}
		anim->moveSpeed = atoi( token );
	}
}




