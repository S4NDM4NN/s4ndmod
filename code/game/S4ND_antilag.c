#include "g_local.h"
#include "S4ND_local.h"

/*
============
G_ResetTrail

Clear out the given client's origin trails (should be called from ClientBegin and when
the teleport bit is toggled)
============
*/
void G_ResetTrail( gentity_t *ent ) {
	int i, time;

	// fill up the origin trails with data (assume the current position
	// for the last 1/2 second or so)
	ent->client->trailHead = NUM_CLIENT_TRAILS - 1;
	for ( i = ent->client->trailHead, time = level.time; i >= 0; i--, time -= 50 ) {
		VectorCopy( ent->r.mins, ent->client->trail[i].mins );
		VectorCopy( ent->r.maxs, ent->client->trail[i].maxs );
		VectorCopy( ent->r.currentOrigin, ent->client->trail[i].currentOrigin );
		ent->client->trail[i].leveltime = time;
		ent->client->trail[i].time = time;
	}
}


/*
============
G_StoreTrail

Keep track of where the client's been (usually called every ClientThink)
============
*/
void G_StoreTrail( gentity_t *ent ) {
	int head, newtime;

	head = ent->client->trailHead;

	// if we're on a new frame
	if ( ent->client->trail[head].leveltime < level.time ) {
		// snap the last head up to the end of frame time
		ent->client->trail[head].time = level.previousTime;

		// increment the head
		ent->client->trailHead++;
		if ( ent->client->trailHead >= NUM_CLIENT_TRAILS ) {
			ent->client->trailHead = 0;
		}
		head = ent->client->trailHead;
	}

	if ( ent->r.svFlags & SVF_BOT ) {
		// bots move only once per frame
		newtime = level.time;
	} else {
		// calculate the actual server time
		// (we set level.frameStartTime every G_RunFrame)
		newtime = level.previousTime + trap_Milliseconds() - level.frameStartTime;
		if ( newtime > level.time ) {
			newtime = level.time;
		} else if ( newtime <= level.previousTime ) {
			newtime = level.previousTime + 1;
		}
	}

	// store all the collision-detection info and the time
	VectorCopy( ent->r.mins, ent->client->trail[head].mins );
	VectorCopy( ent->r.maxs, ent->client->trail[head].maxs );
	VectorCopy( ent->r.currentOrigin, ent->client->trail[head].currentOrigin );
	ent->client->trail[head].leveltime = level.time;
	ent->client->trail[head].time = newtime;

	// FOR TESTING ONLY
//	Com_Printf("level.previousTime: %d, level.time: %d, newtime: %d\n", level.previousTime, level.time, newtime);
}


/*
=============
TimeShiftLerp

Used below to interpolate between two previous vectors
Returns a vector "frac" times the distance between "start" and "end"
=============
*/
static void TimeShiftLerp( float frac, vec3_t start, vec3_t end, vec3_t result ) {
	float comp = 1.0f - frac;

	result[0] = frac * start[0] + comp * end[0];
	result[1] = frac * start[1] + comp * end[1];
	result[2] = frac * start[2] + comp * end[2];
}


/*
=================
G_TimeShiftClient

Move a client back to where he was at the specified "time"
=================
*/
void G_TimeShiftClient( gentity_t *ent, int time ) {
	int j, k;

	if ( time > level.time ) {
		time = level.time;
	}

	// find two entries in the origin trail whose times sandwich "time"
	// assumes no two adjacent trail records have the same timestamp
	j = k = ent->client->trailHead;
	do {
		if ( ent->client->trail[j].time <= time ) {
			break;
		}

		k = j;
		j--;
		if ( j < 0 ) {
			j = NUM_CLIENT_TRAILS - 1;
		}
	}
	while ( j != ent->client->trailHead );

	// if we got past the first iteration above, we've sandwiched (or wrapped)
	if ( j != k ) {
		// make sure it doesn't get re-saved
		if ( ent->client->saved.leveltime != level.time ) {
			// save the current origin and bounding box
			VectorCopy( ent->r.mins, ent->client->saved.mins );
			VectorCopy( ent->r.maxs, ent->client->saved.maxs );
			VectorCopy( ent->r.currentOrigin, ent->client->saved.currentOrigin );
			ent->client->saved.leveltime = level.time;
		}

		// if we haven't wrapped back to the head, we've sandwiched, so
		// we shift the client's position back to where he was at "time"
		if ( j != ent->client->trailHead ) {
			float frac = (float)( ent->client->trail[k].time - time ) /
						 (float)( ent->client->trail[k].time - ent->client->trail[j].time );

			// FOR TESTING ONLY
//			Com_Printf( "level time: %d, fire time: %d, j time: %d, k time: %d\n", level.time, time, ent->client->trail[j].time, ent->client->trail[k].time );

			// interpolate between the two origins to give position at time index "time"
			TimeShiftLerp( frac,
						   ent->client->trail[k].currentOrigin, ent->client->trail[j].currentOrigin,
						   ent->r.currentOrigin );

			// lerp these too, just for fun (and ducking)
			TimeShiftLerp( frac,
						   ent->client->trail[k].mins, ent->client->trail[j].mins,
						   ent->r.mins );

			TimeShiftLerp( frac,
						   ent->client->trail[k].maxs, ent->client->trail[j].maxs,
						   ent->r.maxs );

			// this will recalculate absmin and absmax
			trap_LinkEntity( ent );
		} else {
			// we wrapped, so grab the earliest
			VectorCopy( ent->client->trail[k].currentOrigin, ent->r.currentOrigin );
			VectorCopy( ent->client->trail[k].mins, ent->r.mins );
			VectorCopy( ent->client->trail[k].maxs, ent->r.maxs );

			// this will recalculate absmin and absmax
			trap_LinkEntity( ent );
		}
	}
}


/*
=====================
G_TimeShiftAllClients

Move ALL clients back to where they were at the specified "time",
except for "skip"
=====================
*/
void G_TimeShiftAllClients( int time, gentity_t *skip ) {
	int i;
	gentity_t   *ent;

	if ( time > level.time ) {
		time = level.time;
	}

	// for every client
	ent = &g_entities[0];
	for ( i = 0; i < MAX_CLIENTS; i++, ent++ ) {
		if ( ent->client && ent->inuse && ent->client->sess.sessionTeam < TEAM_SPECTATOR && ent != skip ) {
			if ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) {
				G_TimeShiftClient( ent, time );
			}
		}
	}
}


/*
===================
G_UnTimeShiftClient

Move a client back to where he was before the time shift
===================
*/
void G_UnTimeShiftClient( gentity_t *ent ) {
	// if it was saved
	if ( ent->client->saved.leveltime == level.time ) {
		// move it back
		VectorCopy( ent->client->saved.mins, ent->r.mins );
		VectorCopy( ent->client->saved.maxs, ent->r.maxs );
		VectorCopy( ent->client->saved.currentOrigin, ent->r.currentOrigin );
		ent->client->saved.leveltime = 0;

		// this will recalculate absmin and absmax
		trap_LinkEntity( ent );
	}
}

/*
=======================
G_UnTimeShiftAllClients

Move ALL the clients back to where they were before the time shift,
except for "skip"
=======================
*/
void G_UnTimeShiftAllClients( gentity_t *skip ) {
	int i;
	gentity_t   *ent;

	ent = &g_entities[0];
	for ( i = 0; i < MAX_CLIENTS; i++, ent++ ) {
		if ( ent->client && ent->inuse && ent->client->sess.sessionTeam < TEAM_SPECTATOR && ent != skip ) {
			if ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) {
				G_UnTimeShiftClient( ent );
			}
		}
	}
}




//S4NDM4NN - antiwarp

// Dens: fixes spectator bugs
qboolean G_DoAntiwarp( gentity_t *ent ) {
	// only antiwarp if requested
	if ( !g_antiwarp.integer ) {
		return qfalse;
	}

	if ( level.intermissiontime ) { //dont use for intermission to fix high ping on results screen
		return qfalse;
	}

	if ( ent && ent->client ) {
		// don't antiwarp spectators
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ||
			 ent->client->ps.pm_flags & PMF_LIMBO ) {
			return qfalse;
		}

		// don't antiwarp during map load
		if ( ent->client->pers.connected == CON_CONNECTING ) {
			return qfalse;
		}

		// don't antiwarp if they haven't been connected for 5 seconds
		// note: this check is generally only triggered during mid-map
		// connects, because clients connect before loading the map.
		if ( ( level.time - ent->client->pers.connectTime ) < 5000 ) {
			return qfalse;
		}
	}

	return qtrue;
}


void etpro_AddUsercmd( int clientNum, usercmd_t *cmd ) {
	gentity_t *ent = g_entities + clientNum;

	int idx = ( ent->client->cmdhead + ent->client->cmdcount ) % LAG_MAX_COMMANDS;
	ent->client->cmds[idx] = *cmd;

	if ( ent->client->cmdcount < LAG_MAX_COMMANDS ) {
		ent->client->cmdcount++;
	} else {
		ent->client->cmdhead = ( ent->client->cmdhead + 1 ) % LAG_MAX_COMMANDS;
	}
}

// zinx - G_CmdScale is a hack :x
extern float pm_proneSpeedScale;
static float G_CmdScale( gentity_t *ent, usercmd_t *cmd ) {
	float scale;

#ifdef CGAMEDLL
	int gametype = cg_gameType.integer;
	int movespeed = cg_movespeed.integer;
#elif GAMEDLL
	int gametype = g_gametype.integer;
	int movespeed = g_speed.integer;
#endif

	scale = abs( cmd->forwardmove );
	if ( abs( cmd->rightmove ) > scale ) {
		scale = abs( cmd->rightmove );
	}
	// zinx - don't count crouch/jump; just count moving in water
	if ( ent->waterlevel && abs( cmd->upmove ) > scale ) {
		scale = abs( cmd->upmove );
	}

	scale /= 127.f;

// JPW NERVE -- half move speed if heavy weapon is carried
// this is the counterstrike way of doing it -- ie you can switch to a non-heavy weapon and move at
// full speed.  not completely realistic (well, sure, you can run faster with the weapon strapped to your
// back than in carry position) but more fun to play.  If it doesn't play well this way we'll bog down the
// player if the own the weapon at all.
//
#if 0   // zinx - not letting them go at sprint speed for now.
	if ( ( ent->client->ps.weapon == WP_PANZERFAUST ) ||
		 ( ent->client->ps.weapon == WP_MOBILE_MG42 ) ||
		 ( ent->client->ps.weapon == WP_MOBILE_MG42_SET ) ||
		 ( ent->client->ps.weapon == WP_MORTAR ) ) {
		if ( ent->client->sess.skill[SK_HEAVY_WEAPONS] >= 3 ) {
			scale *= 0.75;
		} else {
			scale *= 0.5;
		}
	}

	if ( ent->client->ps.weapon == WP_FLAMETHROWER ) { // trying some different balance for the FT
		if ( !( ent->client->sess.skill[SK_HEAVY_WEAPONS] >= 3 ) || cmd->buttons & BUTTON_ATTACK ) {
			scale *= 0.7;
		}
	}
#endif

	if ( gametype == GT_SINGLE_PLAYER ) {
		// Adjust the movespeed
		scale *= ( ( (float) movespeed ) / (float) 127 );

	} // if (gametype == GT_SINGLE_PLAYER)...

#if 0   // zinx - not letting them go at sprint speed for now.
	if ( ent->client->ps.eFlags & EF_PRONE ) {
		scale *= pm_proneSpeedScale;
	} else if ( ent->client->ps.eFlags & PMF_DUCKED ) {
		scale *= ent->client->ps.crouchSpeedScale;
	}
#endif

	return scale;
}

void ClientThink_cmd( gentity_t *ent, usercmd_t *cmd );
void DoClientThinks( gentity_t *ent ) {
	int lastCmd, lastTime;
	int latestTime;
	int drop_threshold = LAG_MAX_DROP_THRESHOLD;
	int startPackets = ent->client->cmdcount;
//	usercmd_t newcmd;
	if ( ent->client->cmdcount <= 0 ) {
		return;
	}

	// allow some more movement if time has passed
	latestTime = trap_Milliseconds();
	if ( ent->client->lastCmdRealTime > latestTime ) {
		// zinx - stoopid server went backwards in time, reset the delta
		// instead of giving them even -less- movement ability
		ent->client->cmddelta = 0;
	} else {
		ent->client->cmddelta -= ( latestTime - ent->client->lastCmdRealTime );
	}
	if ( ent->client->cmdcount <= 1 && ent->client->cmddelta < 0 ) {
		ent->client->cmddelta = 0;
	}
	ent->client->lastCmdRealTime = latestTime;

	lastCmd = ( ent->client->cmdhead + ent->client->cmdcount - 1 ) % LAG_MAX_COMMANDS;

	lastTime = ent->client->ps.commandTime;
	latestTime = ent->client->cmds[lastCmd].serverTime;

	while ( ent->client->cmdcount > 0 ) {
		usercmd_t *cmd = &ent->client->cmds[ent->client->cmdhead];
		float speed, delta, scale;
		int savedTime;
		qboolean deltahax = qfalse;

		int serverTime = cmd->serverTime;
		int totalDelta = latestTime - cmd->serverTime;
		int timeDelta;

		if ( ent->client->pers.pmoveFixed ) {
			serverTime = ( ( serverTime + pmove_msec.integer - 1 ) / pmove_msec.integer ) * pmove_msec.integer;
		}

		timeDelta = serverTime - lastTime;

		if ( totalDelta >= drop_threshold ) {
			// zinx - whoops. too lagged.
			drop_threshold = LAG_MIN_DROP_THRESHOLD;
			lastTime = ent->client->ps.commandTime = cmd->serverTime;
			goto drop_packet;
		}

		if ( totalDelta < 0 ) {
			// zinx - oro? packet from the future
			goto drop_packet;
		}

		if ( timeDelta <= 0 ) {
			// zinx - packet from the past
			goto drop_packet;
		}

		scale = 1.f / LAG_DECAY;

		speed = G_CmdScale( ent, cmd );
		delta = ( speed * (float)timeDelta );
		delta *= scale;

		if ( timeDelta > 50 ) {
			timeDelta = 50;
			delta = ( speed * (float)timeDelta );
			delta *= scale;
			deltahax = qtrue;
		}

		if ( ( ent->client->cmddelta + delta ) >= LAG_MAX_DELTA ) {
			// too many commands this server frame

			// if it'll fit in the next frame, just wait until then.
			if ( delta < LAG_MAX_DELTA
				 && ( totalDelta + delta ) < LAG_MIN_DROP_THRESHOLD ) {
				break;
			}

			// try to split it up in to smaller commands

			delta = ( (float)LAG_MAX_DELTA - ent->client->cmddelta );
			timeDelta = ceil( delta / speed ); // prefer speedup
			delta = (float)timeDelta * speed;

			if ( timeDelta < 1 ) {
				break;
			}

			delta *= scale;
			deltahax = qtrue;
		}

		ent->client->cmddelta += delta;

		if ( deltahax ) {
			savedTime = cmd->serverTime;
			cmd->serverTime = lastTime + timeDelta;
		} else {
			savedTime = 0;  // zinx - shut up compiler
		}

		// zinx - erh.  hack, really. make it run for the proper amount of time.
		ent->client->ps.commandTime = lastTime;
		ClientThink_cmd( ent, cmd );
		lastTime = ent->client->ps.commandTime;

		if ( deltahax ) {
			cmd->serverTime = savedTime;

			if ( delta <= 0.1f ) {
				break;
			}

			continue;
		}

drop_packet:
		if ( ent->client->cmdcount <= 0 ) {
			// ent->client was cleared...
			break;
		}

		ent->client->cmdhead = ( ent->client->cmdhead + 1 ) % LAG_MAX_COMMANDS;
		ent->client->cmdcount--;
		continue;
	}

	// zinx - added ping, packets processed this frame
	// warning: eats bandwidth like popcorn
	if ( g_antiwarp.integer & 32 ) {
		trap_SendServerCommand(
			ent - g_entities,
			va( "cp \"%d %d\n\"", latestTime - lastTime, startPackets - ent->client->cmdcount )
			);
	}

	// zinx - debug; size is added lag (amount above player's network lag)
	// rotation is time
	if ( ( g_antiwarp.integer & 16 ) && ent->client->cmdcount ) {
		vec3_t org, parms;

		VectorCopy( ent->client->ps.origin, org );
		SnapVector( org );

		parms[0] = 3;
		parms[1] = (float)( latestTime - ent->client->ps.commandTime ) / 10.f;
		if ( parms[1] < 1.f ) {
			parms[1] = 1.f;
		}
		parms[2] = ( ent->client->ps.commandTime * 180.f ) / 1000.f;

		//etpro_AddDebugLine( org, parms, ((ent - g_entities) % 32), LINEMODE_SPOKES, LINESHADER_RAILCORE, 0, qfalse );
	}

	//ent->client->ps.stats[STAT_ANTIWARP_DELAY] = latestTime - ent->client->ps.commandTime;
	//if (ent->client->ps.stats[STAT_ANTIWARP_DELAY] < 0)
	//	ent->client->ps.stats[STAT_ANTIWARP_DELAY] = 0;
}
