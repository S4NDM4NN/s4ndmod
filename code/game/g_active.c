
#include "g_local.h"
#include "S4ND_local.h"

#ifdef OMNIBOT
	#include  "g_rtcwbot_interface.h"
#endif
//#include "ai_cast_fight.h"	// need these for avoidance


extern void G_CheckForCursorHints( gentity_t *ent );

#ifdef OMNIBOT
void PushBot( gentity_t *ent, gentity_t *other ) {
	vec3_t dir, ang, f, r;
	float oldspeed;

	// dont push when scripted not to be pushed
	if ( other->client && !other->client->sess.botPush ) {
		return;
	}

	oldspeed = VectorLength( other->client->ps.velocity );
	if ( oldspeed < 200 ) {
		oldspeed = 200;
	}
	//
	VectorSubtract( other->r.currentOrigin, ent->r.currentOrigin, dir );
	VectorNormalize( dir );
	vectoangles( dir, ang );
	AngleVectors( ang, f, r, NULL );
	f[2] = 0;
	r[2] = 0;
	//
	VectorMA( other->client->ps.velocity, 200, f, other->client->ps.velocity );
	VectorMA( other->client->ps.velocity, 100 * ( ( level.time + ( ent->s.number * 1000 ) ) % 4000 < 2000 ? 1.0 : -1.0 ), r, other->client->ps.velocity );
	//
	if ( VectorLengthSquared( other->client->ps.velocity ) > SQR( oldspeed ) ) {
		VectorNormalize( other->client->ps.velocity );
		VectorScale( other->client->ps.velocity, oldspeed, other->client->ps.velocity );
	}
}
#endif // OMNIBOT

/*
===============
G_DamageFeedback

Called just before a snapshot is sent to the given player.
Totals up all damage and generates both the player_state_t
damage values to that client for pain blends and kicks, and
global pain sound events for all clients.
===============
*/
void P_DamageFeedback( gentity_t *player ) {
	gclient_t   *client;
	float count;
	vec3_t angles;

	client = player->client;


	if ( client->ps.pm_type == PM_DEAD ) {
		return;
	}



	// total points of damage shot at the player this frame
	count = client->damage_blood + client->damage_armor;
	if ( count == 0 ) {
		return;     // didn't take any damage
	}

	if ( count > 127 ) {
		count = 127;
	}


	// send the information to the client

	// world damage (falling, slime, etc) uses a special code
	// to make the blend blob centered instead of positional
	if ( client->damage_fromWorld ) {
		client->ps.damagePitch = 255;
		client->ps.damageYaw = 255;

		client->damage_fromWorld = qfalse;
	} else {
		vectoangles( client->damage_from, angles );
		client->ps.damagePitch = angles[PITCH] / 360.0 * 256;
		client->ps.damageYaw = angles[YAW] / 360.0 * 256;
	}

	// play an apropriate pain sound
	if ( ( level.time > player->pain_debounce_time ) && !( player->flags & FL_GODMODE ) && !( player->r.svFlags & SVF_BOT ) && !( player->s.powerups & PW_INVULNERABLE ) ) {    //----(SA)
		player->pain_debounce_time = level.time + 700;
		G_AddEvent( player, EV_PAIN, player->health );
	}

	client->ps.damageEvent++;   // Ridah, always increment this since we do multiple view damage anims

	client->ps.damageCount = count;

	//
	// clear totals
	//
	client->damage_blood = 0;
	client->damage_armor = 0;
	client->damage_knockback = 0;
}


#define MIN_BURN_INTERVAL 399 // JPW NERVE set burn timeinterval so we can do more precise damage (was 199 old model)

/*
=============
P_WorldEffects

Check for lava / slime contents and drowning
=============
*/
void P_WorldEffects( gentity_t *ent ) {
	qboolean envirosuit;
	int waterlevel;
	// TTimo: unused
//	static int	lastflameburntime = 0; // JPW NERVE for slowing flamethrower burn intervals and doing more damage per interval

	if ( ent->client->noclip ) {
		ent->client->airOutTime = level.time + 12000;   // don't need air
		return;
	}

	waterlevel = ent->waterlevel;

	envirosuit = ent->client->ps.powerups[PW_BATTLESUIT] > level.time;

//	G_Printf("breathe: %d   invuln: %d   nofatigue: %d\n", ent->client->ps.powerups[PW_BREATHER], level.time - ent->client->ps.powerups[PW_INVULNERABLE], ent->client->ps.powerups[PW_NOFATIGUE]);

	//
	// check for drowning
	//
	if ( waterlevel == 3 ) {
		// envirosuit give air
		if ( envirosuit ) {
			ent->client->airOutTime = level.time + 10000;
		}

		//----(SA)	both these will end up being by virtue of having the 'breather' powerup
		if ( ent->client->ps.aiChar == AICHAR_FROGMAN ) {  // let frogmen breathe forever
			ent->client->airOutTime = level.time + 10000;
		}

		if ( ent->client->ps.aiChar == AICHAR_SEALOPER ) { // ditto
			ent->client->airOutTime = level.time + 10000;
		}

		// if out of air, start drowning
		if ( ent->client->airOutTime < level.time ) {

			if ( ent->client->ps.powerups[PW_BREATHER] ) { // take air from the breather now that we need it
				ent->client->ps.powerups[PW_BREATHER] -= ( level.time - ent->client->airOutTime );
				ent->client->airOutTime = level.time + ( level.time - ent->client->airOutTime );
			} else {


				// drown!
				if ( g_nodrowning.integer ) {
					return;
				}

				ent->client->airOutTime += 1000;
				if ( ent->health > 0 ) {
					// take more damage the longer underwater
					ent->damage += 2;
					if ( ent->damage > 15 ) {
						ent->damage = 15;
					}
#ifdef OMNIBOT
					Bot_Event_Drowning( ent->client->ps.clientNum );
#endif
					// play a gurp sound instead of a normal pain sound
					if ( ent->health <= ent->damage ) {
						G_Sound( ent, G_SoundIndex( "*drown.wav" ) );
					} else if ( rand() & 1 ) {
						G_Sound( ent, G_SoundIndex( "sound/player/gurp1.wav" ) );
					} else {
						G_Sound( ent, G_SoundIndex( "sound/player/gurp2.wav" ) );
					}

					// don't play a normal pain sound
					ent->pain_debounce_time = level.time + 200;

					G_Damage( ent, NULL, NULL, NULL, NULL,
							  ent->damage, DAMAGE_NO_ARMOR, MOD_WATER );
				}
			}
		}
	} else {
		ent->client->airOutTime = level.time + 12000;
		ent->damage = 2;
	}

	//
	// check for sizzle damage (move to pmove?)
	//
	if ( waterlevel && ( ent->watertype & CONTENTS_LAVA ) ) {
		if ( ent->health > 0 && ent->pain_debounce_time <= level.time ) {

			if ( envirosuit ) {
				G_AddEvent( ent, EV_POWERUP_BATTLESUIT, 0 );
			} else {
				if ( ent->watertype & CONTENTS_LAVA ) {
					G_Damage( ent, NULL, NULL, NULL, NULL,
							  30 * waterlevel, 0, MOD_LAVA ); // was MOD_LAVA
				}
			}

		}
	}

	//
	// check for burning from flamethrower
	//
	// JPW NERVE MP way
	if ( ent->s.onFireEnd && ent->client ) {
		//if (level.time - ent->client->lastBurnTime >= MIN_BURN_INTERVAL) {
/*
        // Martin - Fixed Flamethrower
        if (level.time - ent->client->lastBurnTime >= 600) {
        // End martin

            // JPW NERVE server-side incremental damage routine / player damage/health is int (not float)
            // so I can't allocate 1.5 points per server tick, and 1 is too weak and 2 is too strong.
            // solution: allocate damage far less often (MIN_BURN_INTERVAL often) and do more damage.
            // That way minimum resolution (1 point) damage changes become less critical.

            ent->client->lastBurnTime = level.time;
            if ((ent->s.onFireEnd > level.time) && (ent->health > 0)) {
                gentity_t *attacker;
                attacker = g_entities + ent->flameBurnEnt;
                G_Damage (ent, attacker->parent, attacker->parent, NULL, NULL, 5, DAMAGE_NO_KNOCKBACK, MOD_FLAMETHROWER); // JPW NERVE was 7
            }
        }*/
//Martin 6/28/08 REDONE
		if ( ( level.time - ent->client->lastBurnTime >= 1000 ) && ( ent->health > 0 ) ) {
			ent->client->lastBurnTime = level.time;
			if ( ( ent->s.onFireEnd > level.time ) && ( ent->health > 0 ) ) {
				gentity_t *attacker;
				attacker = g_entities + ent->flameBurnEnt;
				G_Damage( ent, attacker->parent, attacker->parent, NULL, NULL, 5, DAMAGE_NO_KNOCKBACK, MOD_FLAMETHROWER ); // JPW NERVE was 7
			}
			// G_burnMeGood isnt called if they are dead so do the work here.
		} else if ( ( level.time - ent->client->lastBurnTime >= 10 ) && ( ent->health <= 0 ) )         {
			ent->client->lastBurnTime = level.time;
			if ( ( ent->s.onFireEnd > level.time ) && ( ent->health <= 0 ) ) {
				gentity_t *attacker;
				attacker = g_entities + ent->flameBurnEnt;
				G_Damage( ent, attacker->parent, attacker->parent, NULL, NULL, 5, DAMAGE_NO_KNOCKBACK, MOD_FLAMETHROWER );
			}
		}
	}
	// jpw

//S4NDM4NN-poison blah blah blah
	if ( ent->poisoned && ent->client ) {

		if ( ( level.time >= ( ent->lastPoisonTime + 1000 ) ) && ( ent->health > 0 ) ) {

			gentity_t *attacker = g_entities + ent->poisonEnt;
			vec3_t angles;


			G_Sound( ent, G_SoundIndex( "sound/props/throw/chairthudgrunt.wav" ) );

			angles[YAW] = ( crandom() * 90 );
			angles[PITCH] = ( crandom() * 90 );
			angles[ROLL] = 0;
			SetClientViewAngle( ent, angles );

			G_Damage( ent, attacker, attacker, NULL, NULL, g_poison.integer, 0, MOD_POISON_SYRINGE );
			ent->lastPoisonTime = level.time;


		}
	}
//END S4NDM4NN
}



/*
===============
G_SetClientSound
===============
*/
void G_SetClientSound( gentity_t *ent ) {
	if ( ent->aiCharacter ) {
		return;
	}
#ifdef OMNIBOT
	if ( ent->r.svFlags & SVF_BOT ) {
		return;
	}
#endif

	if ( ent->waterlevel && ( ent->watertype & CONTENTS_LAVA ) ) { //----(SA)	modified since slime is no longer deadly
		ent->s.loopSound = level.snd_fry;
	} else {
		ent->s.loopSound = 0;
	}
}



//==============================================================

/*
==============
ClientImpacts
==============
*/
void ClientImpacts( gentity_t *ent, pmove_t *pm ) {
	int i, j;
	trace_t trace;
	gentity_t   *other;

	memset( &trace, 0, sizeof( trace ) );
	for ( i = 0 ; i < pm->numtouch ; i++ ) {
		for ( j = 0 ; j < i ; j++ ) {
			if ( pm->touchents[j] == pm->touchents[i] ) {
				break;
			}
		}
		if ( j != i ) {
			continue;   // duplicated
		}
		other = &g_entities[ pm->touchents[i] ];

#ifndef NO_BOT_SUPPORT
		if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) ) {
			ent->touch( ent, other, &trace );
		}
#endif
#ifdef OMNIBOT
		// cs: modified so they push inv humans too (for annoying revives)
		if ( ( ent->client ) &&
			 ( ( other->r.svFlags & SVF_BOT ) || ( other->client && other->client->ps.powerups[PW_INVULNERABLE] ) ) ) {
			PushBot( ent, other );
		}

		// if we are standing on their head, then we should be pushed also
		if ( ( ent->r.svFlags & SVF_BOT || ( other->client && other->client->ps.powerups[PW_INVULNERABLE] ) ) &&
			 ( ent->s.groundEntityNum == other->s.number && other->client ) ) {
			PushBot( other, ent );
		}
#endif // OMNIBOT
		if ( !other->touch ) {
			continue;
		}

		other->touch( other, ent, &trace );
	}

}

/*
============
G_TouchTriggers

Find all trigger entities that ent's current position touches.
Spectators will only interact with teleporters.
============
*/
void    G_TouchTriggers( gentity_t *ent ) {
	int i, num;
	int touch[MAX_GENTITIES];
	gentity_t   *hit;
	trace_t trace;
	vec3_t mins, maxs;
	static vec3_t range = { 40, 40, 52 };

	if ( !ent->client ) {
		return;
	}

	// dead clients don't activate triggers!
	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	VectorSubtract( ent->client->ps.origin, range, mins );
	VectorAdd( ent->client->ps.origin, range, maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	// can't use ent->absmin, because that has a one unit pad
	VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
	VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );

	for ( i = 0 ; i < num ; i++ ) {
		hit = &g_entities[touch[i]];

		if ( !hit->touch && !ent->touch ) {
			continue;
		}
		if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
			continue;
		}

		// ignore most entities if a spectator
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
			if ( hit->s.eType != ET_TELEPORT_TRIGGER ) {
				continue;
			}
		}

		// use seperate code for determining if an item is picked up
		// so you don't have to actually contact its bounding box
		if ( hit->s.eType == ET_ITEM ) {
			if ( !BG_PlayerTouchesItem( &ent->client->ps, &hit->s, level.time ) ) {
				continue;
			}
		} else {
			// MrE: always use capsule for player
			//if ( !trap_EntityContactCapsule( mins, maxs, hit ) ) {
			if ( !trap_EntityContact( mins, maxs, hit ) ) {
				continue;
			}
		}

		memset( &trace, 0, sizeof( trace ) );

		if ( hit->touch ) {
			hit->touch( hit, ent, &trace );
		}
#ifndef NO_BOT_SUPPORT
		if ( ( ent->r.svFlags & SVF_BOT ) && ( ent->touch ) ) {
			ent->touch( ent, hit, &trace );
		}
#endif
	}
}

qboolean checkMG42spec( gentity_t *ent ) {
/*
    client->ps.pm_type = PM_SPECTATOR;
        client->ps.speed = 400;	// faster than normal
        if (client->ps.sprintExertTime)
            client->ps.speed *= 3;	// (SA) allow sprint in free-cam mode


        // set up for pmove
        memset (&pm, 0, sizeof(pm));
        pm.ps = &client->ps;
        pm.cmd = *ucmd;
        pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;	// spectators can fly through bodies
        pm.trace = trap_Trace;
        pm.pointcontents = trap_PointContents;

        Pmove(&pm); // JPW NERVE

        // Rafael - Activate
        // Ridah, made it a latched event (occurs on keydown only)
        if (client->latched_buttons & BUTTON_ACTIVATE)
        {
            Cmd_Activate_f (ent);
        }

        // save results of pmove
//		VectorCopy( client->ps.origin, ent->s.origin );
//
//		G_TouchTriggers( ent );
//		trap_UnlinkEntity( ent );
//	}

//	if (ent->flags & FL_NOFATIGUE)
//		ent->client->ps.sprintTime = 20000;


//	client->oldbuttons = client->buttons;
//	client->buttons = ucmd->buttons;

//----(SA)	added
//	client->oldwbuttons = client->wbuttons;
//	client->wbuttons = ucmd->wbuttons;

    // attack button cycles through spectators
//	if ( ( client->buttons & BUTTON_ATTACK ) && ! ( client->oldbuttons & BUTTON_ATTACK ) ) {
//		Cmd_FollowCycle_f( ent, 1 );
//	}*/
	return qfalse;
}

/*
=================
SpectatorThink
=================
*/
void SpectatorThink( gentity_t *ent, usercmd_t *ucmd ) {
	pmove_t pm;
	gclient_t   *client;
	int clientNum;
	client = ent->client;
	clientNum = client - level.clients;


	if ( g_gamestate.integer == GS_PLAYING ) {
		if ( level.time > ( g_lateJoinTime.integer * 1000 ) ) {
		}
	}

	checkSpeclock( ent );

	checkMG42spec( ent );

	if ( client->sess.spectatorState != SPECTATOR_FOLLOW ) {



		if ( client->sess.spectatorState != SPECTATOR_SCOREBOARD ) {
			client->ps.pm_flags &= ~PMF_SCOREBOARD;
			client->sess.spectatorState = SPECTATOR_FREE;
		}

		// L0 - pause - lock specs on spot..
		if ( level.paused ) {
			client->sess.spectatorState = SPECTATOR_FREE;
			client->ps.pm_type = PM_FREEZE;
			client->ps.pm_flags = PMF_TIME_LOCKPLAYER;
		} // L0 - end



		client->ps.pm_type = PM_SPECTATOR;
		client->ps.speed = 800; //Martin - Per ET, let specs move faster //400;	// faster than normal
		if ( client->ps.sprintExertTime ) {
			client->ps.speed *= 3;  // (SA) allow sprint in free-cam mode

		}
		if ( ( client->ps.pm_flags & PMF_LIMBO ) && level.paused ) {
			client->ps.pm_type = PM_FREEZE;
		} else if ( client->noclip ) {
			client->ps.pm_type = PM_NOCLIP;
		}


		// set up for pmove
		memset( &pm, 0, sizeof( pm ) );
		pm.ps = &client->ps;
		pm.cmd = *ucmd;
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;   // spectators can fly through bodies
		pm.trace = trap_Trace;
		pm.pointcontents = trap_PointContents;

		Pmove( &pm ); // JPW NERVE

		// Rafael - Activate
		// Ridah, made it a latched event (occurs on keydown only)
		if ( client->latched_buttons & BUTTON_ACTIVATE ) {
			Cmd_Activate_f( ent );
		}

		// save results of pmove
		VectorCopy( client->ps.origin, ent->s.origin );

		G_TouchTriggers( ent );
		trap_UnlinkEntity( ent );
	}

	if ( ent->flags & FL_NOFATIGUE ) {
		ent->client->ps.sprintTime = 20000;
	}


	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;

//----(SA)	added
	client->oldwbuttons = client->wbuttons;
	client->wbuttons = ucmd->wbuttons;

	// attack button cycles through spectators					// L0 - pause - cycle only if game is not paused
	if ( ( client->buttons & BUTTON_ATTACK ) && !( client->oldbuttons & BUTTON_ATTACK ) && !level.paused ) {
		Cmd_FollowCycle_f( ent, 1 );

	}

}



/*
=================
ClientInactivityTimer

Returns qfalse if the client is dropped
=================
*/
qboolean ClientInactivityTimer( gclient_t *client ) {
	// Martin - a NECESSARY sanity check.  Otherwise kicks people instantly when its zero
	if ( !g_inactivity.integer ) {
		return qtrue;
	}
	if ( !g_inactivity.integer ) {
		// give everyone some time, so if the operator sets g_inactivity during
		// gameplay, everyone isn't kicked
		client->inactivityTime = level.time + 60 * 1000;
		client->inactivityWarning = qfalse;
	} else if ( client->pers.cmd.forwardmove ||
				client->pers.cmd.rightmove ||
				client->pers.cmd.upmove ||
				( client->pers.cmd.wbuttons & WBUTTON_ATTACK2 ) ||
				( client->pers.cmd.buttons & BUTTON_ATTACK ) ) {
		client->inactivityTime = level.time + g_inactivity.integer * 1000;
		client->inactivityWarning = qfalse;
	} else if ( !client->pers.localClient ) {
		if ( level.time > client->inactivityTime ) {
			trap_DropClient( client - level.clients, "Player Kicked: ^3Due to Inactivity" );
			return qfalse;
		}
		if ( level.time > client->inactivityTime - 10000 && !client->inactivityWarning ) {
			client->inactivityWarning = qtrue;
			trap_SendServerCommand( client - level.clients, "cp \"Ten seconds until inactivity drop!\n\"2" );
		}
	}
	return qtrue;
}

/*
==================
ClientTimerActions

Actions that happen once a second
==================
*/
void ClientTimerActions( gentity_t *ent, int msec ) {
	gclient_t *client;

	client = ent->client;
	client->timeResidual += msec;

	while ( client->timeResidual >= 1000 ) {
		client->timeResidual -= 1000;

		// regenerate
		// JPW NERVE, split these completely
		if ( g_gametype.integer < GT_WOLF ) {
			if ( client->ps.powerups[PW_REGEN] ) {
				if ( ent->health < client->ps.stats[STAT_MAX_HEALTH] ) {
					ent->health += 15;
					if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] * 1.1 ) {
						ent->health = client->ps.stats[STAT_MAX_HEALTH] * 1.1;
					}
					G_AddEvent( ent, EV_POWERUP_REGEN, 0 );
				} else if ( ent->health < client->ps.stats[STAT_MAX_HEALTH] * 2 ) {
					ent->health += 2;
					if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] * 2 ) {
						ent->health = client->ps.stats[STAT_MAX_HEALTH] * 2;
					}
					G_AddEvent( ent, EV_POWERUP_REGEN, 0 );
				}
			} else {
				// count down health when over max
				if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] ) {
					ent->health--;
				}
			}
		}
// JPW NERVE
		else { // GT_WOLF
			if ( client->ps.powerups[PW_REGEN] ) {
				if ( ent->health < client->ps.stats[STAT_MAX_HEALTH] ) {
					if ( g_deathmatch.integer != 2 && !g_sniperWar.integer ) {
						ent->health += 3;
					}
					if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] * 1.1 ) {
						ent->health = client->ps.stats[STAT_MAX_HEALTH] * 1.1;
					}
				} else if ( ent->health < client->ps.stats[STAT_MAX_HEALTH] * 1.12 ) {
					if ( g_deathmatch.integer != 2 ) {
						ent->health += 2;
					}
					if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] * 1.12 ) {
						ent->health = client->ps.stats[STAT_MAX_HEALTH] * 1.12;
					}
				}
			} else {
				// count down health when over max
				if ( ent->health > client->ps.stats[STAT_MAX_HEALTH] ) {
					ent->health--;
				}
			}
		}
// jpw
		// count down armor when over max
		if ( client->ps.stats[STAT_ARMOR] > client->ps.stats[STAT_MAX_HEALTH] ) {
			client->ps.stats[STAT_ARMOR]--;
		}
	}
}

/*
====================
ClientIntermissionThink
====================
*/
void ClientIntermissionThink( gclient_t *client ) {
	client->ps.eFlags &= ~EF_TALK;
	client->ps.eFlags &= ~EF_FIRING;

	// the level will exit when everyone wants to or after timeouts

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = client->pers.cmd.buttons;

//----(SA)	added
	client->oldwbuttons = client->wbuttons;
	client->wbuttons = client->pers.cmd.wbuttons;

	if ( ( client->buttons & ( BUTTON_ATTACK | BUTTON_USE_HOLDABLE ) & ( client->oldbuttons ^ client->buttons ) ) ||
		 ( client->wbuttons & WBUTTON_ATTACK2 & ( client->oldwbuttons ^ client->wbuttons ) ) ) {
		client->readyToExit ^= 1;
	}
}


/*
================
ClientEvents

Events will be passed on to the clients for presentation,
but any server game effects are handled here
================
*/
void ClientEvents( gentity_t *ent, int oldEventSequence ) {
	int i;
	int event;
	gclient_t   *client;
	int damage;
	vec3_t dir;

	client = ent->client;

	if ( oldEventSequence < client->ps.eventSequence - MAX_EVENTS ) {
		oldEventSequence = client->ps.eventSequence - MAX_EVENTS;
	}
	for ( i = oldEventSequence ; i < client->ps.eventSequence ; i++ ) {
		event = client->ps.events[ i & ( MAX_EVENTS - 1 ) ];

		switch ( event ) {
		case EV_FALL_NDIE:
		//case EV_FALL_SHORT:
		case EV_FALL_DMG_10:
		case EV_FALL_DMG_15:
		case EV_FALL_DMG_25:
		//case EV_FALL_DMG_30:
		case EV_FALL_DMG_50:
			//case EV_FALL_DMG_75:

			if ( ent->s.eType != ET_PLAYER ) {
				break;      // not in the player model
			}
			if ( g_dmflags.integer & DF_NO_FALLING ) {
				break;
			}
			if ( event == EV_FALL_NDIE ) {
				damage = 9999;
			} else if ( event == EV_FALL_DMG_50 )     {
				damage = 50;
				ent->client->ps.pm_time = 1000;
				ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
				VectorClear( ent->client->ps.velocity );
			} else if ( event == EV_FALL_DMG_25 )     {
				damage = 25;
				ent->client->ps.pm_time = 250;
				ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
				VectorClear( ent->client->ps.velocity );
			} else if ( event == EV_FALL_DMG_15 )     {
				damage = 15;
				ent->client->ps.pm_time = 1000;
				ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
				VectorClear( ent->client->ps.velocity );
			} else if ( event == EV_FALL_DMG_10 )     {
				damage = 10;
				ent->client->ps.pm_time = 1000;
				ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
				VectorClear( ent->client->ps.velocity );
			} else {
				damage = 5; // never used

			}
			if ( ( g_goomba.value > 0 ) && ( ent->s.groundEntityNum < MAX_CLIENTS ) && ( ent->s.groundEntityNum != -1 ) && ( g_entities[ent->s.groundEntityNum].client->ps.stats[STAT_HEALTH] > 0 ) ) {
				damage *= g_goomba.value;

				if ( !OnSameTeam( &g_entities[ent->s.groundEntityNum], ent ) ) {
					G_Damage( &g_entities[ent->s.groundEntityNum], ent, ent, tv( 0, 0, -1 ), NULL, damage, 0, MOD_GOOMBA );
				}
				break;
			}


			VectorSet( dir, 0, 0, 1 );
			ent->pain_debounce_time = level.time + 200; // no normal pain sound
			G_Damage( ent, NULL, NULL, NULL, NULL, damage, 0, MOD_FALLING );
			break;
// JPW NERVE
		case EV_TESTID1:
		case EV_TESTID2:
		case EV_ENDTEST:
			break;
// jpw

		case EV_FIRE_WEAPON_MG42:
		case EV_FIRE_WEAPON:
		case EV_FIRE_WEAPONB:
		case EV_FIRE_WEAPON_LASTSHOT:
			FireWeapon( ent );
			break;

//----(SA)	modified
		case EV_USE_ITEM1:      // ( HI_MEDKIT )	medkit
		case EV_USE_ITEM2:      // ( HI_WINE )		wine
		case EV_USE_ITEM3:      // ( HI_SKULL )		skull of invulnerable
		case EV_USE_ITEM4:      // ( HI_WATER )		protection from drowning
		case EV_USE_ITEM5:      // ( HI_ELECTRIC )	protection from electric attacks
		case EV_USE_ITEM6:      // ( HI_FIRE )		protection from fire attacks
		case EV_USE_ITEM7:      // ( HI_STAMINA )	restores fatigue bar and sets "nofatigue" for a time period
		case EV_USE_ITEM8:      // ( HI_BOOK1 )
		case EV_USE_ITEM9:      // ( HI_BOOK2 )
		case EV_USE_ITEM10:     // ( HI_BOOK3 )
			UseHoldableItem( ent, event - EV_USE_ITEM0 );
			break;
//----(SA)	end

		default:
			break;
		}
	}

}

/*
==============
SendPendingPredictableEvents
==============
*/
void SendPendingPredictableEvents( playerState_t *ps ) {
	/*
	gentity_t *t;
	int event, seq;
	int extEvent, number;

	// if there are still events pending
	if ( ps->entityEventSequence < ps->eventSequence ) {
	    // create a temporary entity for this event which is sent to everyone
	    // except the client generated the event
	    seq = ps->entityEventSequence & (MAX_EVENTS-1);
	    event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
	    // set external event to zero before calling BG_PlayerStateToEntityState
	    extEvent = ps->externalEvent;
	    ps->externalEvent = 0;
	    // create temporary entity for event
	    t = G_TempEntity( ps->origin, event );
	    number = t->s.number;
	    BG_PlayerStateToEntityState( ps, &t->s, qtrue );
	    t->s.number = number;
	    t->s.eType = ET_EVENTS + event;
	    t->s.eFlags |= EF_PLAYER_EVENT;
	    t->s.otherEntityNum = ps->clientNum;
	    // send to everyone except the client who generated the event
	    t->r.svFlags |= SVF_NOTSINGLECLIENT;
	    t->r.singleClient = ps->clientNum;
	    // set back external event
	    ps->externalEvent = extEvent;
	}
	*/
}
// Martin - OLD 1.0 way
// DHM - Nerve
/*
void WolfFindMedic( gentity_t *self ) {
    int i, medic=-1;
    gclient_t	*cl;
    vec3_t	start, end, temp;
    trace_t	tr;
    float	bestdist=1024, dist;

    self->client->ps.viewlocked_entNum = 0;
    self->client->ps.viewlocked = 0;
    self->client->ps.stats[STAT_DEAD_YAW] = 999;

    VectorCopy( self->s.pos.trBase, start );
    start[2] += self->client->ps.viewheight;

    for ( i=0; i<level.numPlayingClients; i++ ) {
        cl = &level.clients[ level.sortedClients[i] ];

        if ( cl->ps.clientNum == self->client->ps.clientNum )
            continue;
        if ( cl->sess.sessionTeam != self->client->sess.sessionTeam )
            continue;
        if ( cl->ps.stats[ STAT_HEALTH ] <= 0 )
            continue;
        if ( cl->ps.stats[ STAT_PLAYER_CLASS ] != PC_MEDIC )
            continue;

        VectorCopy( g_entities[level.sortedClients[i]].s.pos.trBase, end );
        end[2] += cl->ps.viewheight;

        trap_Trace (&tr, start, NULL, NULL, end, self->s.number, CONTENTS_SOLID);
        if ( tr.fraction < 0.95 )
            continue;

        VectorSubtract( end, start, end );
        dist = VectorNormalize( end );

        if ( dist < bestdist ) {
            medic = cl->ps.clientNum;
            vectoangles( end, temp );
            self->client->ps.stats[STAT_DEAD_YAW] = temp[YAW];
            bestdist = dist;
        }
    }

    if ( medic >= 0 ) {
        self->client->ps.viewlocked_entNum = medic;
        self->client->ps.viewlocked = 7;
    }
}
*/
// ET way, should work MUCH better
void WolfFindMedic( gentity_t *self ) {
	int i, medic = -1;
	gclient_t   *cl;
	vec3_t start, end;
	vec3_t temp;
	trace_t tr;
	float bestdist = 1024, dist;

	self->client->ps.viewlocked_entNum = 0;
	self->client->ps.viewlocked = 0;
	self->client->ps.stats[STAT_DEAD_YAW] = 999;

	VectorCopy( self->s.pos.trBase, start );
	start[2] += self->client->ps.viewheight;

	for ( i = 0; i < level.numConnectedClients; i++ ) {
		cl = &level.clients[ level.sortedClients[i] ];

		if ( level.sortedClients[i] == self->client->ps.clientNum ) {
			continue;
		}
		if ( cl->sess.sessionTeam != self->client->sess.sessionTeam ) {
			continue;
		}
		if ( cl->ps.pm_type == PM_DEAD ) {
			continue;
		}
		// zinx - limbo'd players are not PM_DEAD or STAT_HEALTH <= 0.
		// and we certainly don't want to lock to them
		// fix for bug #345
		if ( cl->ps.pm_flags & PMF_LIMBO ) {
			continue;
		}
		if ( cl->ps.stats[ STAT_HEALTH ] <= 0 ) {
			continue;
		}
		// tjw: ps.stats updated before spawn?
		//if( cl->ps.stats[ STAT_PLAYER_CLASS ] != PC_MEDIC ) {
		if ( cl->sess.playerType != PC_MEDIC ) {
			continue;
		}
		VectorCopy( g_entities[level.sortedClients[i]].s.pos.trBase, end );
		end[2] += cl->ps.viewheight;

		trap_Trace( &tr, start, NULL, NULL, end, self->s.number, CONTENTS_SOLID );
		if ( tr.fraction < 0.95 ) {
			continue;
		}

		VectorSubtract( end, start, end );
		dist = VectorNormalize( end );

		if ( dist < bestdist ) {
			medic = cl->ps.clientNum;
			vectoangles( end, temp );
			self->client->ps.stats[STAT_DEAD_YAW] = temp[YAW];
			bestdist = dist;
		}
	}
	if ( medic >= 0 ) {
		self->client->ps.viewlocked_entNum = medic;
		self->client->ps.viewlocked = 7;
		//trap_SendServerCommand( -1, va("chat \"%i\n\"", level.clients[medic].ps.weapon));	// 26 == syringe
	}
}
// END MARTIN's dump of more et code :D
void limbo( gentity_t *ent, qboolean makeCorpse ); // JPW NERVE
void reinforce( gentity_t *ent ); // JPW NERVE

void ClientDamage( gentity_t *clent, int entnum, int enemynum, int id );        // NERVE - SMF

/*
==============
ClientThink

This will be called once for each client frame, which will
usually be a couple times for each server frame on fast clients.

If "g_synchronousClients 1" is set, this will be called exactly
once for each server frame, which makes for smooth demo recording.
==============
*/
void ClientThink_real( gentity_t *ent ) {
	gclient_t   *client;
	pmove_t pm;
//	vec3_t		oldOrigin;
	int oldEventSequence;
	int msec;
	usercmd_t   *ucmd;
	int monsterslick = 0;
// JPW NERVE
	int i;
	vec3_t muzzlebounce;
	gitem_t *item;
	gentity_t *ent2;
	vec3_t velocity, org, offset;
	vec3_t angles,mins,maxs;
	int weapon;
	trace_t tr;
	int goldPercentage;
	int goldWeight;
	// jpw
//	vec3_t	drunk;
	// Rafael wolfkick
	//int			validkick;
	//static int	wolfkicktimer = 0;





	client = ent->client;


	// don't think if the client is not yet connected (and thus not yet spawned in)
	if ( client->pers.connected != CON_CONNECTED ) {
		return;
	}
	if ( g_showHitBoxes.integer ) {
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			gentity_t *bboxEnt;
			gentity_t *tent;
			gentity_t   *head;
			vec3_t b1, b2;
			orientation_t or;       // DHM - Nerve
			VectorCopy( ent->r.currentOrigin, b1 );
			VectorCopy( ent->r.currentOrigin, b2 );
			VectorAdd( b1, ent->r.mins, b1 );
			VectorAdd( b2, ent->r.maxs, b2 );
			bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, bboxEnt->s.origin2 );
			bboxEnt->s.dmgFlags = 1; // ("type")

			head = G_Spawn();

			if ( trap_GetTag( ent->s.number, "tag_head", &or ) ) {
				G_SetOrigin( head, or.origin );
			} else {
				float height, dest;
				vec3_t v, angles, forward, up, right;

				G_SetOrigin( head, ent->r.currentOrigin );

				if ( ent->client->ps.pm_flags & PMF_DUCKED ) { // closer fake offset for 'head' box when crouching
					height = ent->client->ps.crouchViewHeight - 12;
				} else {
					height = ent->client->ps.viewheight;
				}

				// NERVE - SMF - this matches more closely with WolfMP models
				VectorCopy( ent->client->ps.viewangles, angles );
				if ( angles[PITCH] > 180 ) {
					dest = ( -360 + angles[PITCH] ) * 0.75;
				} else {
					dest = angles[PITCH] * 0.75;
				}
				angles[PITCH] = dest;

				AngleVectors( angles, forward, right, up );
				VectorScale( forward, 5, v );
				VectorMA( v, 18, up, v );

				VectorAdd( v, head->r.currentOrigin, head->r.currentOrigin );
				head->r.currentOrigin[2] += height / 2;
				// -NERVE - SMF
			}


			VectorCopy( head->r.currentOrigin, head->s.origin );
			VectorCopy( head->s.angles, head->s.apos.trBase );
			VectorCopy( head->s.angles, head->s.apos.trDelta );
			VectorSet( head->r.mins, -6, -6, -2 ); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
			VectorSet( head->r.maxs, 6, 6, 10 ); // changed this z from 0 to 6
			head->clipmask = CONTENTS_SOLID;
			head->r.contents = CONTENTS_SOLID;

			trap_LinkEntity( head );


			VectorCopy( head->r.currentOrigin, b1 );
			VectorCopy( head->r.currentOrigin, b2 );
			VectorAdd( b1, head->r.mins, b1 );
			VectorAdd( b2, head->r.maxs, b2 );
			tent = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, tent->s.origin2 );
			tent->s.dmgFlags = 1;

			G_FreeEntity( head );
		}
	}

//S4NDM4NN - CV checking
	if ( !g_allowCenterview.integer 
		&& !g_autoAim.integer ) {
#ifdef OMNIBOT
		if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR && !( ent->r.svFlags & SVF_BOT ) ) {
#else
		if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
#endif
				if ( client->ps.stats[STAT_HEALTH] > 0 ) {
					if ( ent->client->ps.viewangles[PITCH] == 0.000000 &&  client->pers.lastAngle[PITCH] != 0.000000) {
							SetClientViewAngle( ent, client->pers.lastAngle );
					}
					VectorCopy(ent->client->ps.viewangles, ent->client->pers.lastAngle);
				} else {
					client->pers.lastAngle[PITCH] = 1.0;
				}
			
		}
	}
//END - CV checking
/*	if((!strlen(ent->client->sess.cdkey)) && (ent->client->pers.checkpuretime != 0) && (ent->client->pers.checkpuretime+1000 < level.time)){
        trap_DropClient(ent-g_entities, "You need the S4NDMoD client, visit www.S4NDMoD.com to download it!");
    }
*/

#ifdef GLOBALSTATS //Global stats code to detect weaponswitching
	if ( ( ent->s.eFlags & EF_MG42_ACTIVE ) && !ent->client->pers.usingMg42 ) { //ON MG42 & using mg not set
		ent->client->pers.usingMg42 = 1;
		stats_event_weapSwitch( ent,stat_mpWeapToStatsWeap( ent->client->pers.lastWeapon ) );
	} else if ( !( ent->s.eFlags & EF_MG42_ACTIVE ) && ent->client->pers.usingMg42 )        { //NOT ON MG42 & using mg set
		ent->client->pers.usingMg42 = 0;
		stats_event_weapSwitch( ent,STATS_MG42 );
	} else if ( ( ent->s.eFlags & EF_ZOOMING ) && ( ent->client->ps.weapon != WP_SNIPERRIFLE ) && !ent->client->pers.usingBinoc )           { //ZOOMING & using binoc not set
		ent->client->pers.usingBinoc = 1;
		stats_event_weapSwitch( ent,stat_mpWeapToStatsWeap( ent->client->pers.lastWeapon ) );
	} else if ( !( ent->s.eFlags & EF_ZOOMING ) && ent->client->pers.usingBinoc )        { //NOT ZOOMING & using binoc
		ent->client->pers.usingBinoc = 0;
		stats_event_weapSwitch( ent,STATS_ARTILLERY );
	} else if ( ent->client->pers.lastWeapon != ent->client->ps.weapon )      {
		stats_event_weapSwitch( ent,stat_mpWeapToStatsWeap( ent->client->pers.lastWeapon ) );
	}

	ent->client->pers.lastWeapon = ent->client->ps.weapon;
#endif

	if ( g_deathmatch.integer == 2 && ( !level.endDeathmatch ) ) {
		DeathmatchCheckLocation( ent );
	}

	if ( g_sniperWar.integer ) {
		SniperCheckLocation( ent );
	}


	if ( ( g_LTinfomsg.integer ) && ( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_LT ) && ( level.time >= ent->client->lastLTinfotime + 1000 ) ) {
		GetLTinfo( ent );
	} else if ( g_allowSpys.integer && g_reportSpys.integer && ( level.time >= ent->client->lastLTinfotime + 1000 ) )        {
		checkSpys( ent );
	}

	if ( client->pers.realPing > 750 && !( client->ps.eFlags & EF_CONNECTION ) ) {
		client->ps.eFlags |= EF_CONNECTION;
		client->lastHighPingTime = level.time;
	}
	if ( client->pers.realPing < 750 && ( client->ps.eFlags & EF_CONNECTION ) ) {
		client->ps.eFlags &= ~EF_CONNECTION;
	}
	if ( client->pers.realPing < 750 ) {
		client->lastHighPingTime = level.time;
	}

	//Martin - Antiwarp can take care of high ping players.  With wolf at death's door, no need to kick players for high ping.
/*	if( client->lastHighPingTime + 60000 < level.time){
        G_LogPrintf("Kicked for No Packet Flow: %s, IP: %s\n",client->pers.netname, client->sess.ip);
        trap_DropClient(ent-g_entities, "^7Player Kicked: ^3No Packet Flow");
        //G_LogPrintf("%.99s KICKED for no packet flow.\n",client->pers.netname );
    }*/


	if ( g_autoAim.integer ) {
		if ( ent->client->buttons & BUTTON_ATTACK ) {
			DoAutoAim( ent );
		}
	}
	if ( ent->client->pers.spawnTimerActive ) {   // Martin - Break it into several checks to lighten the load on the server
		if ( ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) && ( level.time >= ent->client->pers.lastSpawnTimerCheck + 50 ) ) {
			SpawnTimer( ent );
		}
	}


	if ( client->cameraPortal ) {
		G_SetOrigin( client->cameraPortal, client->ps.origin );
		trap_LinkEntity( client->cameraPortal );
		VectorCopy( client->cameraOrigin, client->cameraPortal->s.origin2 );
	}


	// mark the time, so the connection sprite can be removed
	ucmd = &ent->client->pers.cmd;


/*	if (zbc_ZbotCheck (ent, ucmd)){
        trap_SendServerCommand(-1, va("chat \"console: %s ^7is aimbotting!\n\"", client->pers.netname));
    }*/

	ent->client->ps.identifyClient = ucmd->identClient;     // NERVE - SMF

//unlagged - true ping
	// save the estimated ping in a queue for averaging later

	// we use level.previousTime to account for 50ms lag correction
	// besides, this will turn out numbers more like what players are used to
	// josh: changed it back. People don't like it.

	if ( ent->r.svFlags & SVF_BOT ) {
		if ( g_fakePing.integer ) {
			int flux = rand() % 10;
			flux -= 5;
			client->pers.realPing = client->pers.seedPing + flux;
		} else {  // No fake pings, just set bot's ping to 0
			client->pers.realPing = 0;
		}
	} else {
		client->pers.pingsamples[client->pers.samplehead] = level.previousTime + client->frameOffset - ucmd->serverTime;
		client->pers.samplehead++;
		if ( client->pers.samplehead >= NUM_PING_SAMPLES ) {
			client->pers.samplehead -= NUM_PING_SAMPLES;
		}

		// initialize the real ping
		if ( g_truePing.integer ) {
			int i, sum = 0;

			// get an average of the samples we saved up
			for ( i = 0; i < NUM_PING_SAMPLES; i++ ) {
				sum += client->pers.pingsamples[i];
			}

			client->pers.realPing = sum / NUM_PING_SAMPLES;
		} else {
			// if g_truePing is off, use the normal ping
			client->pers.realPing = client->ps.ping;
		}
		//unlagged - true ping
		// make sure the true ping is over 0 - with cl_timenudge it can be less
		if ( client->pers.realPing < 0 ) {
			client->pers.realPing = 0;
		}
	}
	//unlagged - true ping



// JPW NERVE -- update counter for capture & hold display
	if ( g_gametype.integer == GT_WOLF_CPH ) {
		client->ps.stats[STAT_CAPTUREHOLD_RED] = level.capturetimes[TEAM_RED];
		client->ps.stats[STAT_CAPTUREHOLD_BLUE] = level.capturetimes[TEAM_BLUE];
	}
// jpw

	// sanity check the command time to prevent speedup cheating
	if ( ucmd->serverTime > level.time + 200 ) {
		ucmd->serverTime = level.time + 200;
//		G_Printf("serverTime <<<<<\n" );
	}
	if ( ucmd->serverTime < level.time - 1000 ) {
		ucmd->serverTime = level.time - 1000;
//		G_Printf("serverTime >>>>>\n" );
	}

	msec = ucmd->serverTime - client->ps.commandTime;
	// following others may result in bad times, but we still want
	// to check for follow toggles
	if ( msec < 1 && client->sess.spectatorState != SPECTATOR_FOLLOW ) {
		return;
		/*
		// Ridah, fixes savegame timing issue
		if (msec < -100) {
		    client->ps.commandTime = ucmd->serverTime - 100;
		    msec = 100;
		} else {
		    return;
		}
		*/
		// done.
	}
	if ( msec > 200 ) {
		msec = 200;
	}

	if ( pmove_msec.integer < 8 ) {
		trap_Cvar_Set( "pmove_msec", "8" );
	} else if ( pmove_msec.integer > 33 )     {
		trap_Cvar_Set( "pmove_msec", "33" );
	}

	if ( pmove_fixed.integer || client->pers.pmoveFixed ) {
		ucmd->serverTime = ( ( ucmd->serverTime + pmove_msec.integer - 1 ) / pmove_msec.integer ) * pmove_msec.integer;
		//if (ucmd->serverTime - client->ps.commandTime <= 0)
		//	return;
	}

	//
	// check for exiting intermission
	//
	if ( level.intermissiontime ) {
		ClientIntermissionThink( client );
		return;
	}




	// spectators don't do much
	// DHM - Nerve :: In limbo use SpectatorThink
	if ( client->sess.sessionTeam == TEAM_SPECTATOR || client->ps.pm_flags & PMF_LIMBO ) {
		SpectatorThink( ent, ucmd );
		return;
	}

	if ( level.paused ) {
		ucmd->buttons = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->wbuttons = 0;
		ucmd->wolfkick = 0;

		client->ps.pm_type = PM_FREEZE;
	}


	// JPW NERVE do some time-based muzzle flip -- this never gets touched in single player (see g_weapon.c)
	// #define RIFLE_SHAKE_TIME 150 // JPW NERVE this one goes with the commented out old damped "realistic" behavior below
	#define RIFLE_SHAKE_TIME 300 // per Id request, longer recoil time
	if ( client->sniperRifleFiredTime ) {
		if ( level.time - client->sniperRifleFiredTime > RIFLE_SHAKE_TIME ) {
			client->sniperRifleFiredTime = 0;
		} else {
			VectorCopy( client->ps.viewangles,muzzlebounce );

			// JPW per Id request, longer recoil time
			muzzlebounce[PITCH] -= 2 * cos( 2.5 * ( level.time - client->sniperRifleFiredTime ) / RIFLE_SHAKE_TIME );
			muzzlebounce[YAW] += 0.5*client->sniperRifleMuzzleYaw*cos( 1.0 - ( level.time - client->sniperRifleFiredTime )*3 / RIFLE_SHAKE_TIME );
			muzzlebounce[PITCH] -= 0.25 * random() * ( 1.0f - ( level.time - client->sniperRifleFiredTime ) / RIFLE_SHAKE_TIME );
			muzzlebounce[YAW] += 0.5 * crandom() * ( 1.0f - ( level.time - client->sniperRifleFiredTime ) / RIFLE_SHAKE_TIME );
			SetClientViewAngle( ent,muzzlebounce );
		}
	}
	// L0 - don't regen the hp if game is paused..
//	if (!level.paused) {
	if ( client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {

		if ( level.time > client->ps.powerups[PW_REGEN] + 5000 ) {
			client->ps.powerups[PW_REGEN] = level.time;
		}
	}
//	} // L0 - end
	// also update weapon recharge time

	// JPW drop button drops secondary weapon so new one can be picked up
	// TTimo explicit braces to avoid ambiguous 'else'
	if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
		if ( ucmd->wbuttons & WBUTTON_DROP ) {
			if ( !client->dropWeaponTime ) {
				client->dropWeaponTime = 1; // just latch it for now
				// L0 - paused don't allow weapon dropping...
				if ( level.paused ) {
					return;
				}                             // L0 - end
				if ( g_instaGib.integer ) {
					return;
				}

				if ( client->ps.weapon == WP_KNIFE ) {
					if ( client->ps.stats[STAT_HEALTH] > 0 ) {
						if ( g_dropObj.integer > 0 ) {
							Cmd_dropObj_f( ent );
						} else {
							if ( g_throwableKnives.integer != 0 ) {
								Cmd_ThrowKnife_f( ent );
							}
						}
						return;
					}
				}
				if ( !level.endDeathmatch ) {
					if ( g_deathmatch.integer == 2 &&
						 ( client->ps.weapon != WP_GRENADE_PINEAPPLE &&
						   client->ps.weapon != WP_GRENADE_LAUNCHER ) &&
						 ( client->ps.weapon ) ) {

						weapon = client->ps.weapon;
						item = BG_FindItemForWeapon( weapon );
						VectorCopy( client->ps.viewangles, angles );

						// clamp pitch
						if ( angles[PITCH] < -30 ) {
							angles[PITCH] = -30;
						} else if ( angles[PITCH] > 30 ) {
							angles[PITCH] = 30;
						}

						AngleVectors( angles, velocity, NULL, NULL );
						VectorScale( velocity, 64, offset );
						offset[2] += client->ps.viewheight / 2;
						VectorScale( velocity, 75, velocity );
						velocity[2] += 50 + random() * 35;

						VectorAdd( client->ps.origin,offset,org );

						VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
						VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );

						trap_Trace( &tr, client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
						VectorCopy( tr.endpos, org );

						ent2 = LaunchItem( item, org, velocity, client->ps.clientNum );
						COM_BitClear( client->ps.weapons,weapon );

						// Clear out empty weapon, change to next best weapon
						G_AddEvent( ent, EV_NOAMMO, 0 );

						if ( weapon == client->ps.weapon ) {
							client->ps.weapon = 0;
						}

						if ( g_dropClips.integer && g_dropReload.integer ) {
							ent2->count += client->ps.ammo[BG_FindAmmoForWeapon( weapon )];
							ent2->item->quantity += client->ps.ammo[BG_FindAmmoForWeapon( weapon )];
							client->ps.ammo[BG_FindAmmoForWeapon( weapon )] = 0;
						} else
						{
							ent2->count = client->ps.ammoclip[BG_FindClipForWeapon( weapon )];
							ent2->item->quantity = client->ps.ammoclip[BG_FindClipForWeapon( weapon )];
							client->ps.ammoclip[BG_FindClipForWeapon( weapon )] = 0;
						}
#ifdef OMNIBOT
						Bot_Event_RemoveWeapon( client->ps.clientNum, Bot_WeaponGameToBot( weapon ) );
#endif

					} else {
						if ( ( client->ps.stats[STAT_PLAYER_CLASS] == PC_SOLDIER ) || ( client->ps.stats[STAT_PLAYER_CLASS] == PC_LT ) || ( g_unlockWeapons.integer ) || ( g_deathmatch.integer == 2 ) ) {
							for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ ) {
								weapon = weapBanksMultiPlayer[3][i];

//						if ((weapon != WP_FLAMETHROWER) && (weapon != WP_VENOM)) // JPW NERVE pulled per id request
								if ( COM_BitCheck( client->ps.weapons,weapon ) ) {

									item = BG_FindItemForWeapon( weapon );
									VectorCopy( client->ps.viewangles, angles );

									// clamp pitch
									if ( angles[PITCH] < -30 ) {
										angles[PITCH] = -30;
									} else if ( angles[PITCH] > 30 ) {
										angles[PITCH] = 30;
									}

									AngleVectors( angles, velocity, NULL, NULL );
									VectorScale( velocity, 64, offset );
									offset[2] += client->ps.viewheight / 2;
									VectorScale( velocity, 75, velocity );
									velocity[2] += 50 + random() * 35;

									VectorAdd( client->ps.origin,offset,org );

									VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
									VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );

									trap_Trace( &tr, client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
									VectorCopy( tr.endpos, org );

									ent2 = LaunchItem( item, org, velocity, client->ps.clientNum );
									COM_BitClear( client->ps.weapons,weapon );

									if ( weapon == WP_MAUSER ) {
										COM_BitClear( client->ps.weapons,WP_SNIPERRIFLE );
									} else if ( weapon == WP_FG42 ) {
										COM_BitClear( client->ps.weapons,WP_FG42SCOPE );
									}
									// Clear out empty weapon, change to next best weapon
									G_AddEvent( ent, EV_NOAMMO, 0 );



									i = MAX_WEAPS_IN_BANK_MP;
									if ( weapon == client->ps.weapon ) {
										client->ps.weapon = 0;
									}

									if ( g_dropClips.integer && g_dropReload.integer ) {
										ent2->count += client->ps.ammo[BG_FindAmmoForWeapon( weapon )];
										ent2->item->quantity += client->ps.ammo[BG_FindAmmoForWeapon( weapon )];
										client->ps.ammo[BG_FindAmmoForWeapon( weapon )] = 0;
									} else
									{
										ent2->count = client->ps.ammoclip[BG_FindClipForWeapon( weapon )];
										ent2->item->quantity = client->ps.ammoclip[BG_FindClipForWeapon( weapon )];
										client->ps.ammoclip[BG_FindClipForWeapon( weapon )] = 0;
									}
#ifdef OMNIBOT
									Bot_Event_RemoveWeapon( client->ps.clientNum, Bot_WeaponGameToBot( weapon ) );
#endif

								}
							}
						}
					}
				}
			}
		}       else {
			client->dropWeaponTime = 0;
		}
	}

// jpw

	// check for inactivity timer, but never drop the local client of a non-dedicated server
	if ( !ClientInactivityTimer( client ) ) {
		return;
	}

	if ( reloading || client->cameraPortal ) {
		ucmd->buttons = 0;
		ucmd->forwardmove = 0;
		ucmd->rightmove = 0;
		ucmd->upmove = 0;
		ucmd->wbuttons = 0;
		ucmd->wolfkick = 0;
		if ( client->cameraPortal ) {
			client->ps.pm_type = PM_FREEZE;
		}
	} else if ( client->noclip ) {
		client->ps.pm_type = PM_NOCLIP;
	} else if ( client->ps.stats[STAT_HEALTH] <= 0 ) {
		client->ps.pm_type = PM_DEAD;
		// L0 - pause
	} else if ( level.paused ) {
		client->ps.pm_type = PM_FREEZE;
		client->ps.pm_flags = PMF_TIME_LOCKPLAYER;
		// end
	} else {
		client->ps.pm_type = PM_NORMAL;
	}

	// set parachute anim condition flag
	BG_UpdateConditionValue( ent->s.number, ANIM_COND_PARACHUTE, ( ent->flags & FL_PARACHUTE ) != 0, qfalse );

	// all playing clients are assumed to be in combat mode
	if ( !client->ps.aiChar ) {
		client->ps.aiState = AISTATE_COMBAT;
	}

	client->ps.gravity = g_gravity.value;
//S4NDM4NN-Goldrush
	if ( g_goldrush.integer ) {
		if ( ent->client->pers.gold ) {
			goldPercentage = ( ent->client->pers.gold - 1 ) * g_goldWeight.integer;
			goldWeight = ( ( goldPercentage * .01 ) * ( client->ps.gravity ) );
			client->ps.gravity += goldWeight;
		}
	}
//End
	// set speed
	client->ps.speed = g_speed.value;
//S4NDM4NN-Goldrush
	if ( g_goldrush.integer ) {
		if ( ent->client->pers.gold ) {
			goldPercentage = ( ent->client->pers.gold - 1 ) * g_goldWeight.integer;
			goldWeight = ( ( goldPercentage * .01 ) * ( client->ps.speed ) );
			client->ps.speed -= goldWeight;
		}
	}
//End


	if ( client->ps.powerups[PW_HASTE] ) {
		client->ps.speed *= 1.3;
	}

	// set up for pmove
	oldEventSequence = client->ps.eventSequence;

	client->currentAimSpreadScale = (float)client->ps.aimSpreadScale / 255.0;

	memset( &pm, 0, sizeof( pm ) );

	pm.ps = &client->ps;
	pm.cmd = *ucmd;
	pm.oldcmd = client->pers.oldcmd;
	if ( pm.ps->pm_type == PM_DEAD ) {
		pm.tracemask = MASK_PLAYERSOLID & ~CONTENTS_BODY;
		// DHM-Nerve added:: EF_DEAD is checked for in Pmove functions, but wasn't being set
		//              until after Pmove
		pm.ps->eFlags |= EF_DEAD;
		// dhm-Nerve end
	} else {
		pm.tracemask = MASK_PLAYERSOLID;
	}
	// MrE: always use capsule for AI and player
	//pm.trace = trap_TraceCapsule;//trap_Trace;
	//DHM - Nerve :: We've gone back to using normal bbox traces
	pm.trace = trap_Trace;
	pm.pointcontents = trap_PointContents;
	pm.debugLevel = g_debugMove.integer;
	pm.noFootsteps = ( g_dmflags.integer & DF_NO_FOOTSTEPS ) > 0;

	pm.pmove_fixed = pmove_fixed.integer | client->pers.pmoveFixed;
	pm.pmove_msec = pmove_msec.integer;

	// Per Ken, forced bots to always use 125 fps movement.

	//martin - 6/1/2010 - what the fuck? why was fixedphysicsfps set to 333 for bots...
	if ( ent->r.svFlags & SVF_BOT ) {
		//pm.fixedphysics = 1;
		pm.fixedphysicsfps = 125;
	} else
	{
		//pm.fixedphysics = g_fixedphysics.integer;
		if ( g_fixedphysics.integer ) {
			pm.fixedphysicsfps = 125;
		}
		//else if (g_fixedphysics.integer == 2)
		//	pm.fixedphysicsfps = 333;

	}

	pm.noWeapClips = ( g_dmflags.integer & DF_NO_WEAPRELOAD ) > 0;
//	if (ent->aiCharacter && AICast_NoReload(ent->s.number))
//		pm.noWeapClips = qtrue;	// ensure AI characters don't use clips if they're not supposed to.

	// Ridah
//	if (ent->r.svFlags & SVF_NOFOOTSTEPS)
//		pm.noFootsteps = qtrue;

	VectorCopy( client->ps.origin, client->oldOrigin );

	// NERVE - SMF
	pm.gametype = g_gametype.integer;
	if ( g_deathmatch.integer != 2 ) {
		pm.ltChargeTime = g_LTChargeTime.integer;
		pm.soldierChargeTime = g_soldierChargeTime.integer;
		pm.engineerChargeTime = g_engineerChargeTime.integer;
		pm.medicChargeTime = g_medicChargeTime.integer;
	} else {
		pm.ltChargeTime = 10000;
		pm.soldierChargeTime = 10000;
		pm.engineerChargeTime = 10000;
		pm.medicChargeTime = 10000;
	}
	// -NERVE - SMF

	if ( ent->playdeadState == 1 || ent->playdeadState == 2 ) {       //1 is the falling anim

		//set a few vars as if the player is dead - must be reset every frame because some are 'fixed' elsewhere
		pm.ps->pm_type = PM_DEAD;
		pm.tracemask = MASK_PLAYERSOLID;
		pm.ps->eFlags |= EF_DEAD;

		//clear all the buttons/movement info - they can look around and talk, but nothing else
		pm.cmd.buttons = 0;
		pm.cmd.forwardmove = 0;
		pm.cmd.rightmove = 0;
		pm.cmd.upmove = 0;
		pm.cmd.wbuttons = 0;
		pm.cmd.wolfkick = 0;

		pm.cmd.weapon = pm.ps->weapon = WP_NONE;    //don't display a gun

		ent->client->ps.persistant[PERS_WOLFKICK] = 0;  //stop any kick stuff

		//set the new state once the anim is done
		if ( ( ent->playdeadState == 1 ) && ( ent->client->ps.pm_time == 0 ) ) {
			//set up new bbox, copied from player_die
			trap_UnlinkEntity( ent );
			ent->r.maxs[2] = 0;
			ent->client->ps.maxs[2] = 0;
			trap_LinkEntity( ent );

			//set the new state
			ent->playdeadState = 2;     //2 is in the act of playing dead, the main state between falling and rising
		}
	} else if ( ent->playdeadState == 3 )    { //getting up (anim + wait)
		//BG_AnimScriptEvent( &ent->client->ps, ANIM_ET_REVIVE, qfalse, qtrue );//added by agent
		//ent->client->ps.pm_time = 2100;			//added by agent
		//traceEnt->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
		//traceEnt->client->ps.pm_time = 2100;

		if ( ent->client->ps.pm_time == 0 ) { //if the anim is done
			vec3_t playerMins = {-18, -18, -24};    //copied from g_client.c
			vec3_t playerMaxs = {18, 18, 48};

			// cs: do this stuff here so it's only called once
			pm.ps->pm_type = PM_NORMAL;
			pm.tracemask = MASK_PLAYERSOLID;

			//bboxes and view code copied from ClientSpawn
			VectorCopy( playerMins, ent->r.mins );
			VectorCopy( playerMaxs, ent->r.maxs );
			VectorCopy( ent->r.mins, client->ps.mins );
			VectorCopy( ent->r.maxs, client->ps.maxs );

			client->ps.crouchViewHeight = CROUCH_VIEWHEIGHT;
			client->ps.standViewHeight = DEFAULT_VIEWHEIGHT;
			client->ps.deadViewHeight = DEAD_VIEWHEIGHT;
			client->ps.crouchMaxZ = client->ps.maxs[2] - ( client->ps.standViewHeight - client->ps.crouchViewHeight );

			//reset the movement and the state to normal
			pm.ps->pm_flags &= ~PMF_TIME_LOCKPLAYER;
			ent->playdeadState = 0;
		}
	}

	monsterslick = Pmove( &pm );

	if ( monsterslick && !( ent->flags & FL_NO_MONSTERSLICK ) ) {
		//vec3_t	dir;
		//vec3_t	kvel;
		//vec3_t	forward;
		// TTimo gcc: might be used unitialized in this function
		float angle = 0.0f;
		qboolean bogus = qfalse;

		// NE
		if ( ( monsterslick & SURF_MONSLICK_N ) && ( monsterslick & SURF_MONSLICK_E ) ) {
			angle = 45;
		}
		// NW
		else if ( ( monsterslick & SURF_MONSLICK_N ) && ( monsterslick & SURF_MONSLICK_W ) ) {
			angle = 135;
		}
		// N
		else if ( monsterslick & SURF_MONSLICK_N ) {
			angle = 90;
		}
		// SE
		else if ( ( monsterslick & SURF_MONSLICK_S ) && ( monsterslick & SURF_MONSLICK_E ) ) {
			angle = 315;
		}
		// SW
		else if ( ( monsterslick & SURF_MONSLICK_S ) && ( monsterslick & SURF_MONSLICK_W ) ) {
			angle = 225;
		}
		// S
		else if ( monsterslick & SURF_MONSLICK_S ) {
			angle = 270;
		}
		// E
		else if ( monsterslick & SURF_MONSLICK_E ) {
			angle = 0;
		}
		// W
		else if ( monsterslick & SURF_MONSLICK_W ) {
			angle = 180;
		} else
		{
			bogus = qtrue;
		}
	}

	// server cursor hints
	if ( ent->lastHintCheckTime < level.time ) {
		G_CheckForCursorHints( ent );

		ent->lastHintCheckTime = level.time + FRAMETIME;
	}

	// DHM - Nerve :: Set animMovetype to 1 if ducking
	if ( ent->client->ps.pm_flags & PMF_DUCKED ) {
		ent->s.animMovetype = 1;
	} else {
		ent->s.animMovetype = 0;
	}

	// save results of pmove
	if ( ent->client->ps.eventSequence != oldEventSequence ) {
		ent->eventTime = level.time;
		ent->r.eventTime = level.time;
	}

	// Ridah, fixes jittery zombie movement
	if ( g_smoothClients.integer ) {
		BG_PlayerStateToEntityStateExtraPolate( &ent->client->ps, &ent->s, ent->client->ps.commandTime, qtrue );
	} else {
		BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, qtrue );
	}

	if ( !( ent->client->ps.eFlags & EF_FIRING ) ) {
		client->fireHeld = qfalse;      // for grapple
	}

//
//	// use the precise origin for linking
//	VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
//
//	// use the snapped origin for linking so it matches client predicted versions
	VectorCopy( ent->s.pos.trBase, ent->r.currentOrigin );

	VectorCopy( pm.mins, ent->r.mins );
	VectorCopy( pm.maxs, ent->r.maxs );

	ent->waterlevel = pm.waterlevel;
	ent->watertype = pm.watertype;


	//S4NDM4NN - pause disables events during pause
	if ( !level.paused ) {
		ClientEvents( ent, oldEventSequence ); // execute client events

	}
	// link entity now, after any personal teleporters have been used
	trap_LinkEntity( ent );
	if ( !ent->client->noclip ) {
		G_TouchTriggers( ent );
	}

	// NOTE: now copy the exact origin over otherwise clients can be snapped into solid
	VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );

	//ANTILAG - store the client's new position
	G_StoreTrail( ent );


	// touch other objects
	ClientImpacts( ent, &pm );

	// save results of triggers and client events
	if ( ent->client->ps.eventSequence != oldEventSequence ) {
		ent->eventTime = level.time;
	}

	// swap and latch button actions
	client->oldbuttons = client->buttons;
	client->buttons = ucmd->buttons;
	client->latched_buttons = client->buttons & ~client->oldbuttons;
//	client->latched_buttons |= client->buttons & ~client->oldbuttons;	// FIXME:? (SA) MP method (causes problems for us.  activate 'sticks')

	//----(SA)	added
	client->oldwbuttons = client->wbuttons;
	client->wbuttons = ucmd->wbuttons;
	client->latched_wbuttons = client->wbuttons & ~client->oldwbuttons;
//	client->latched_wbuttons |= client->wbuttons & ~client->oldwbuttons;	// FIXME:? (SA) MP method
	// Rafael - Activate
	// Ridah, made it a latched event (occurs on keydown only)
	if ( client->latched_buttons & BUTTON_ACTIVATE ) {
		Cmd_Activate_f( ent );
//		Cmd_KickAS_f(ent);
//		Cmd_KickGrenade_f(ent);
		G_CanisterKick( ent );

		Cmd_Activate2_f( ent ); //agent - also added this here to allow newbies to try and shove door blockers.
		// if this turns into shoving spam, we will have to take it out :/
	}

	if ( client->buttons & BUTTON_ACTIVATE ) {
		Cmd_Drag_f( ent );
	}

	//S4NDM4NN-this is for the +salute
	if ( client->latched_buttons & BUTTON_GESTURE ) {
		//Cmd_Push_f(ent);
//	Cmd_KickAS_f(ent);
//	Cmd_KickGrenade_f(ent);
		G_CanisterKick( ent );
		Cmd_Activate2_f( ent ); // controls our new push


	}

	if ( client->buttons & BUTTON_GESTURE ) {
		Cmd_Drag_f( ent );
		//Cmd_KickAS_f(ent);
		//Cmd_KickGrenade_f(ent);		//Agent - 09/25/07 Moved to latched so people cant go around kicking everything
	}
	//End

	//S4NDM4NN
	if ( client->ps.persistant[PERS_WOLFKICK] != 0 ) {
		if ( level.time > ( ent->lastKickTime + 1000 ) ) {
			client->ps.persistant[PERS_WOLFKICK] = 0;
		} else
		{
			client->ps.persistant[PERS_WOLFKICK] = (int) ( 22 - ( ( ( ent->lastKickTime + 1000 ) - level.time ) / 50 ) );

			if ( client->ps.persistant[PERS_WOLFKICK] > 8 ) {
				client->ps.persistant[PERS_WOLFKICK] = 0;
			}
		}
	}
	//End

	if ( ent->flags & FL_NOFATIGUE ) {
		ent->client->ps.sprintTime = 20000;
	}

#ifdef OMNIBOT
	Bot_Util_CheckForSuicide( ent );
#endif

	// check for respawning
	if ( client->ps.stats[STAT_HEALTH] <= 0 ) {

		// L0 - paused - if game is paused player can't tap.
		if ( level.paused ) {
			return;
		}
		// end

		// DHM - Nerve
		if ( g_gametype.integer >= GT_WOLF ) {
			WolfFindMedic( ent );
		}
		// dhm - end

		// wait for the attack button to be pressed
		if ( level.time > client->respawnTime ) {
			// forcerespawn is to prevent users from waiting out powerups
			if ( ( g_gametype.integer != GT_SINGLE_PLAYER ) &&
				 ( g_forcerespawn.integer > 0 ) &&
				 ( ( level.time - client->respawnTime ) > g_forcerespawn.integer * 1000 )  &&
				 ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) ) { // JPW NERVE
				// JPW NERVE
				if ( g_gametype.integer >= GT_WOLF ) {
					limbo( ent, qtrue );
				} else {
					respawn( ent );
				}
				// jpw
				return;
			}

			// DHM - Nerve :: Single player game respawns immediately as before,
			//				  but in multiplayer, require button press before respawn
			if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
				respawn( ent );
			}
			// NERVE - SMF - we want to only respawn on jump button now
			if ( ( client->latched_buttons & BUTTON_ACTIVATE ) && ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) ) {
				if ( ent->client->pers.isTapped == qtrue ) {
					ent->client->pers.isTapped = qfalse;
					ent->client->pers.spawnCyclesSkipped = 0;
					if ( level.time >= ent->client->pers.lastTapMessageTime + 250 ) {       // Flood protection for themselves.
						trap_SendServerCommand( ent - g_entities, va( "chat \"You are waiting for a medic.\"" ) );
						trap_SendServerCommand( ent - g_entities, va( "chat \"Press ^1JUMP ^7deploy in the next reinforcement wave.\"" ) );
						trap_SendServerCommand( ent - g_entities, va( "chat \"Press ^1CROUCH ^7to tap out into limbo.\"" ) );
						ent->client->pers.lastTapMessageTime = level.time;
					}
				}
				// New martin autotap
				// negative upmove is for 'crouch'
			} else if ( ( ucmd->upmove < 0 ) && ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) && ( g_autoTap.integer ) )            {
				ent->client->pers.isTapped = qfalse;
				ent->client->pers.spawnCyclesSkipped = 0;
				ent->client->pers.gibbed = qtrue;
				limbo( ent, qtrue );
				if ( g_deathmatch.integer != 2 ) {
					if ( g_tapReports.integer == 1 ) {
						trap_SendServerCommand( -1, va( "print \"%s ^7tapped out\n\"", ent->client->pers.netname ) );
					}
					//if its more then one send to the team
					if ( g_tapReports.integer > 1 ) {
						//red team
						if ( ent->client->sess.sessionTeam == TEAM_RED ) {
							G_TeamCommand( TEAM_RED, va( "print \"%s ^7tapped out\n\"", ent->client->pers.netname ) );
						}
						//Blue team
						if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
							G_TeamCommand( TEAM_BLUE, va( "print \"%s ^7tapped out\n\"",  ent->client->pers.netname ) );
						}
					}
				}
				//end martin
			} else if ( ( ucmd->upmove > 0 ) && ( !( ent->client->ps.pm_flags & PMF_LIMBO ) ) )       { // JPW NERVE
				// JPW NERVE
				if ( g_gametype.integer >= GT_WOLF ) {
					// They are 'tapped', but can still be revived
					if ( ( g_autoTap.integer ) && ( g_gamestate.integer == GS_PLAYING ) ) {
						ent->client->pers.isTapped = qtrue;
						if ( level.time >= ent->client->pers.lastTapMessageTime + 250 ) {           // Flood protection for themselves.
							trap_SendServerCommand( ent - g_entities, va( "chat \"You will deploy in the next reinforcement wave.\"" ) );
							trap_SendServerCommand( ent - g_entities, va( "chat \"Press ^1ACTIVATE/USE ^7to wait for a medic.\"" ) );
							trap_SendServerCommand( ent - g_entities, va( "chat \"Press ^1CROUCH ^7to tap out into limbo.\"" ) );
							ent->client->pers.lastTapMessageTime = level.time;
						}
						//return;

					} else {
						ent->client->pers.gibbed = qtrue;
						limbo( ent, qtrue );
						//S4NDM4NN-for the tap reports junk
						//if its 1 send to everyone
						if ( g_deathmatch.integer != 2 ) {
							if ( g_tapReports.integer == 1 ) {
								trap_SendServerCommand( -1, va( "print \"%s ^7tapped out\n\"", ent->client->pers.netname ) );
							}
							//if its more then one send to the team
							if ( g_tapReports.integer > 1 ) {
								//red team
								if ( ent->client->sess.sessionTeam == TEAM_RED ) {
									G_TeamCommand( TEAM_RED, va( "print \"%s ^7tapped out\n\"", ent->client->pers.netname ) );
								}
								//Blue team
								if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
									G_TeamCommand( TEAM_BLUE, va( "print \"%s ^7tapped out\n\"",  ent->client->pers.netname ) );
								}
							}
						}
					}
					//End
				} else {
					respawn( ent );
				}
				// jpw
			}
			// dhm - Nerve :: end
			// NERVE - SMF - we want to immediately go to limbo mode if gibbed
			else if ( client->ps.stats[STAT_HEALTH] <= GIB_HEALTH && !( ent->client->ps.pm_flags & PMF_LIMBO ) ) {
				if ( g_gametype.integer >= GT_WOLF ) {
					limbo( ent, qfalse );
				} else {
					respawn( ent );
				}
			}
			// -NERVE - SMF
		}
		return;
	}




	//S4NDM4NN - pause we don't do this when paused
	if ( !level.paused ) {
		ClientTimerActions( ent, msec ); // perform once-a-second actions
	}
}

/*
==================
ClientThink

A new command has arrived from the client
==================
*/

void ClientThink_cmd( gentity_t *ent, usercmd_t *cmd ) {
	ent->client->pers.oldcmd = ent->client->pers.cmd;
	ent->client->pers.cmd = *cmd;
	ClientThink_real( ent );
}

void ClientThink( int clientNum ) {
	gentity_t *ent;
	usercmd_t newcmd;

	ent = g_entities + clientNum;

	trap_GetUsercmd( clientNum, &newcmd /*ent->client->pers.cmd*/ );

	// mark the time we got info, so we can display the
	// phone jack if they don't get any for a while
	ent->client->lastCmdTime = level.time;

	if ( g_gamestate.integer == GS_PLAYING ) {
		if ( g_autoTap.integer ) {
			if ( level.time >= ( ent->lastAutoTapCheckTime + 250 ) ) {
				CheckAutoTap( ent );
			}
		}
	}

	//S4NDM4NN - antiwarp
	if ( G_DoAntiwarp( ent ) ) {
		// josh: use zinx antiwarp code
		etpro_AddUsercmd( clientNum, &newcmd );
		DoClientThinks( ent );
	} else {
		ClientThink_cmd( ent,&newcmd );
	}
	//END - antiwarp

}


void G_RunClient( gentity_t *ent ) {

	if ( G_DoAntiwarp( ent ) ) {
		// josh: use zinx antiwarp code
		DoClientThinks( ent );
	}

	if ( !g_syncronousClients.integer ) {
		return;
	}



	ent->client->pers.cmd.serverTime = level.time;

	ClientThink_real( ent );
}

/*
==================
SpectatorClientEndFrame

==================
*/
void SpectatorClientEndFrame( gentity_t *ent ) {
	gclient_t   *cl;
	int do_respawn = 0; // JPW NERVE
	int savedScore;     // DHM - Nerve
	int savedRespawns;  // DHM - Nerve
	int savedClass;     // NERVE - SMF
	int flags;
	int testtime;

	// if we are doing a chase cam or a remote view, grab the latest info
	if ( ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) || ( ent->client->ps.pm_flags & PMF_LIMBO ) ) { // JPW NERVE for limbo
		int clientNum;

		if ( ent->client->sess.sessionTeam == TEAM_RED ) {
			testtime = ( level.dwRedReinfOffset + level.time - level.startTime ) % g_redlimbotime.integer;

			if ( g_deathmatch.integer == 2 && ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
				do_respawn = 1;
			}

			if ( g_forceWarmupSpawn.integer ) {
				if ( g_gamestate.integer != GS_PLAYING ) {
					do_respawn = 1;
				}
			}

			if ( testtime < ent->client->pers.lastReinforceTime ) {
				do_respawn = 1;
			}
			ent->client->pers.lastReinforceTime = testtime;
		} else if ( ent->client->sess.sessionTeam == TEAM_BLUE )     {
			testtime = ( level.dwBlueReinfOffset + level.time - level.startTime ) % g_bluelimbotime.integer;

			if ( g_deathmatch.integer == 2 && ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
				do_respawn = 1;
			}

			if ( g_forceWarmupSpawn.integer ) {
				if ( g_gamestate.integer != GS_PLAYING ) {
					do_respawn = 1;
				}
			}
			if ( testtime < ent->client->pers.lastReinforceTime ) {
				do_respawn = 1;
			}
			ent->client->pers.lastReinforceTime = testtime;
		}

		if ( g_maxlives.integer > 0 && ent->client->ps.persistant[PERS_RESPAWNS_LEFT] == 0 ) {
			do_respawn = 0;
		}

		if ( do_respawn ) {
			reinforce( ent );
			return;
		}

		clientNum = ent->client->sess.spectatorClient;

		// team follow1 and team follow2 go to whatever clients are playing
		if ( clientNum == -1 ) {
			clientNum = level.follow1;
		} else if ( clientNum == -2 ) {
			clientNum = level.follow2;
		}
		if ( clientNum >= 0 ) {
			cl = &level.clients[ clientNum ];
			if ( cl->pers.connected == CON_CONNECTED && cl->sess.sessionTeam != TEAM_SPECTATOR ) {
				// DHM - Nerve :: carry flags over
				flags = ( cl->ps.eFlags & ~( EF_VOTED ) ) | ( ent->client->ps.eFlags & ( EF_VOTED ) );
				// JPW NERVE -- limbo latch
				if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR && ent->client->ps.pm_flags & PMF_LIMBO ) {
					// abuse do_respawn var
					savedScore = ent->client->ps.persistant[PERS_SCORE];
					do_respawn = ent->client->ps.pm_time;
					savedRespawns = ent->client->ps.persistant[PERS_RESPAWNS_LEFT];
					savedClass = ent->client->ps.stats[STAT_PLAYER_CLASS];

					ent->client->ps = cl->ps;
					ent->client->ps.pm_flags |= PMF_FOLLOW;
					ent->client->ps.pm_flags |= PMF_LIMBO;

					ent->client->ps.persistant[PERS_RESPAWNS_LEFT] = savedRespawns;
					ent->client->ps.pm_time = do_respawn;                           // put pm_time back
					ent->client->ps.persistant[PERS_SCORE] = savedScore;            // put score back
					ent->client->ps.stats[STAT_PLAYER_CLASS] = savedClass;          // NERVE - SMF - put player class back
				} else {
					ent->client->ps = cl->ps;
					ent->client->ps.pm_flags |= PMF_FOLLOW;
				}
				// jpw
				// DHM - Nerve :: carry flags over
				ent->client->ps.eFlags = flags;
				return;
			} else {

				// drop them to free spectators unless they are dedicated camera followers
				if ( ent->client->sess.spectatorClient >= 0 ) {
					ent->client->sess.spectatorState = SPECTATOR_FREE;
					ClientBegin( ent->client - level.clients );
				}
			}
		}
	}

	if ( ent->client->sess.spectatorState == SPECTATOR_SCOREBOARD ) {
		ent->client->ps.pm_flags |= PMF_SCOREBOARD;
	} else {
		ent->client->ps.pm_flags &= ~PMF_SCOREBOARD;
	}
}



// DHM - Nerve :: After reviving a player, their contents stay CONTENTS_CORPSE until it is determined
//					to be safe to return them to PLAYERSOLID

qboolean StuckInClient( gentity_t *self ) {
	gentity_t *hit;
	vec3_t hitmin, hitmax;
	vec3_t selfmin, selfmax;
	int i;

	hit = &g_entities[0];
	for ( i = 0; i < level.maxclients; i++, hit++ ) {
		if ( !hit->inuse ) {
			continue;
		}
		if ( hit == self ) {
			continue;
		}
		if ( !hit->client ) {
			continue;
		}
		if ( !hit->s.solid ) {
			continue;
		}
		if ( hit->health <= 0 ) {
			continue;
		}

		VectorAdd( hit->r.currentOrigin, hit->r.mins, hitmin );
		VectorAdd( hit->r.currentOrigin, hit->r.maxs, hitmax );
		VectorAdd( self->r.currentOrigin, self->r.mins, selfmin );
		VectorAdd( self->r.currentOrigin, self->r.maxs, selfmax );

		if ( hitmin[0] > selfmax[0] ) {
			continue;
		}
		if ( hitmax[0] < selfmin[0] ) {
			continue;
		}
		if ( hitmin[1] > selfmax[1] ) {
			continue;
		}
		if ( hitmax[1] < selfmin[1] ) {
			continue;
		}
		if ( hitmin[2] > selfmax[2] ) {
			continue;
		}
		if ( hitmax[2] < selfmin[2] ) {
			continue;
		}

		return qtrue;
	}
	return qfalse;
}

extern vec3_t playerMins, playerMaxs;
#define WR_PUSHAMOUNT 25

void WolfRevivePushEnt( gentity_t *self, gentity_t *other ) {
	vec3_t dir, push;

	VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir );
	dir[2] = 0;
	VectorNormalizeFast( dir );

	VectorScale( dir, WR_PUSHAMOUNT, push );

	if ( self->client ) {
		VectorAdd( self->s.pos.trDelta, push, self->s.pos.trDelta );
		VectorAdd( self->client->ps.velocity, push, self->client->ps.velocity );
	}

	VectorScale( dir, -WR_PUSHAMOUNT, push );
	push[2] = WR_PUSHAMOUNT / 2;

	VectorAdd( other->s.pos.trDelta, push, other->s.pos.trDelta );
	VectorAdd( other->client->ps.velocity, push, other->client->ps.velocity );
}

void WolfReviveBbox( gentity_t *self ) {
	int touch[MAX_GENTITIES];
	int num,i, touchnum = 0;
	gentity_t   *hit;
	vec3_t mins, maxs;

	VectorAdd( self->r.currentOrigin, playerMins, mins );
	VectorAdd( self->r.currentOrigin, playerMaxs, maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for ( i = 0 ; i < num ; i++ ) {
		hit = &g_entities[touch[i]];
		if ( hit->client && hit->health > 0 ) {
			if ( hit->s.number != self->s.number ) {
				WolfRevivePushEnt( hit, self );
				touchnum++;
			}
		} else if ( hit->r.contents & ( CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_PLAYERCLIP ) )   {
			// Move corpse directly to the person who revived them
			if ( self->props_frame_state >= 0 ) {
				trap_UnlinkEntity( self );
				VectorCopy( g_entities[self->props_frame_state].client->ps.origin, self->client->ps.origin );
				VectorCopy( self->client->ps.origin, self->r.currentOrigin );
				trap_LinkEntity( self );

				// Reset value so we don't continue to warp them
				self->props_frame_state = -1;
			}
		}
	}

	G_DPrintf( "WolfReviveBbox: touchnum: %d\n", touchnum );

	if ( touchnum == 0 ) {
		G_DPrintf( "WolfReviveBbox:  Player is solid now!\n" );
		self->r.contents = CONTENTS_BODY;
	}
}
// dhm

/*
==============
ClientEndFrame

Called at the end of each server frame for each connected client
A fast client will have multiple ClientThink for each ClientEdFrame,
while a slow client may have multiple ClientEndFrame between ClientThink.
==============
*/
void ClientEndFrame( gentity_t *ent ) {
	int i;

	if ( ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) || ( ent->client->ps.pm_flags & PMF_LIMBO ) ) { // JPW NERVE
		SpectatorClientEndFrame( ent );
		return;
	}

	if ( !ent->aiCharacter ) {
		// turn off any expired powerups
		// L0 - changed to PW_NUP_POWERUPS for paused :)
		for ( i = 0 ; i < PW_NUM_POWERUPS ; i++ ) {

			if ( i == PW_FIRE ||             // these aren't dependant on level.time
				 i == PW_ELECTRIC ||
				 i == PW_BREATHER ||
				 ent->client->ps.powerups[i] == 0       // L0
				 || i == PW_NOFATIGUE ) {

				continue;
			}


			// L0 - paused
			if ( level.paused &&
				 ent->client->ps.powerups[i] != INT_MAX ) {
				ent->client->ps.powerups[i] += level.time - level.previousTime;
			} // end

			if ( ent->client->ps.powerups[ i ] < level.time ) {
				ent->client->ps.powerups[ i ] = 0;
			}

			// L0 - paused - FIXME/ need to restore it back
			if ( level.paused ) {
				int time_delta = level.time - level.previousTime;

				ent->client->airOutTime += time_delta;
				ent->client->ps.classWeaponTime += time_delta;
				ent->client->respawnTime = time_delta;
				ent->client->inactivityTime += time_delta;
				ent->lastHintCheckTime += time_delta;
				ent->client->ps.powerups[PW_REGEN] += time_delta;
				// Fixes
				ent->client->pers.connectTime += time_delta;
				ent->client->pers.enterTime += time_delta;

			} // end

		}
	}

	// save network bandwidth
#if 0
	if ( !g_synchronousClients->integer && ent->client->ps.pm_type == PM_NORMAL ) {
		// FIXME: this must change eventually for non-sync demo recording
		VectorClear( ent->client->ps.viewangles );
	}
#endif

	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if ( level.intermissiontime ) {
		return;
	}

	// burn from lava, etc
	P_WorldEffects( ent );

	// apply all the damage taken this frame
	P_DamageFeedback( ent );

	// add the EF_CONNECTION flag if we haven't gotten commands recently
	if ( level.time - ent->client->lastCmdTime > 1000 ) {
		ent->s.eFlags |= EF_CONNECTION;
	} else {
		ent->s.eFlags &= ~EF_CONNECTION;
	}

	ent->client->ps.stats[STAT_HEALTH] = ent->health;   // FIXME: get rid of ent->health...

	G_SetClientSound( ent );

	// set the latest infor

	// Ridah, fixes jittery zombie movement
	if ( g_smoothClients.integer ) {
		BG_PlayerStateToEntityStateExtraPolate( &ent->client->ps, &ent->s, ent->client->ps.commandTime, ( ( ent->r.svFlags & SVF_CASTAI ) == 0 ) );
	} else {
		BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, ( ( ent->r.svFlags & SVF_CASTAI ) == 0 ) );
	}

	//SendPendingPredictableEvents( &ent->client->ps );

	// DHM - Nerve :: If it's been a couple frames since being revived, and props_frame_state
	//					wasn't reset, go ahead and reset it
	if ( ent->props_frame_state >= 0 && ( ( level.time - ent->s.effect3Time ) > 100 ) ) {
		ent->props_frame_state = -1;
	}

	if ( ent->health > 0 && StuckInClient( ent ) ) {
		G_DPrintf( "%s is stuck in a client.\n", ent->client->pers.netname );
		ent->r.contents = CONTENTS_CORPSE;
	}

	if ( g_gametype.integer >= GT_WOLF && ent->health > 0 && ent->r.contents == CONTENTS_CORPSE ) {
		WolfReviveBbox( ent );
	}

	// DHM - Nerve :: Reset 'count2' for flamethrower
	if ( !( ent->client->buttons & BUTTON_ATTACK ) ) {
		ent->count2 = 0;
	}
	// dhm


}


