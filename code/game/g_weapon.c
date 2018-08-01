/*
 * name:		g_weapon.c
 *
 * desc:		perform the server side effects of a weapon firing
 *
*/


#include "g_local.h"
#include "S4ND_local.h"
#ifdef OMNIBOT
	#include  "g_rtcwbot_interface.h"
#endif
static float s_quadFactor;
static vec3_t forward, right, up;
static vec3_t muzzleEffect;
static vec3_t muzzleTrace;


// forward dec
void Bullet_Fire( gentity_t *ent, float spread, int damage );
qboolean Bullet_Fire_Extended( gentity_t *source, gentity_t *attacker, vec3_t start, vec3_t end, float spread, int damage );

int G_GetWeaponDamage( int weapon ); // JPW

#define NUM_NAILSHOTS 10

/*
======================================================================

KNIFE/GAUNTLET (NOTE: gauntlet is now the Zombie melee)

======================================================================
*/

#define KNIFE_DIST 48

/*
==============
Weapon_Knife
==============
*/
void Weapon_Knife( gentity_t *ent ) {
	trace_t tr;
	gentity_t   *traceEnt, *tent;
	int damage, mod;
	vec3_t pforward, eforward;

	vec3_t end;

	if ( ent->s.weapon == WP_KNIFE2 ) {
		mod = MOD_KNIFE2;
	} else {
		mod = MOD_KNIFE;
	}

	AngleVectors( ent->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( ent, ent->s.weapon, forward, right, up, muzzleTrace );
	VectorMA( muzzleTrace, KNIFE_DIST, forward, end );
	trap_Trace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );

	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	// no contact
	if ( tr.fraction == 1.0f ) {
		return;
	}

	if ( tr.entityNum >= MAX_CLIENTS ) {   // world brush or non-player entity (no blood)
		tent = G_TempEntity( tr.endpos, EV_MISSILE_MISS );
	} else {                            // other player
		tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
	}

	tent->s.otherEntityNum = tr.entityNum;
	tent->s.eventParm = DirToByte( tr.plane.normal );
	tent->s.weapon = ent->s.weapon;

	if ( tr.entityNum == ENTITYNUM_WORLD ) { // don't worry about doing any damage
		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	if ( !( traceEnt->takedamage ) ) {
		return;
	}

	damage = G_GetWeaponDamage( ent->s.weapon ); // JPW		// default knife damage for frontal attacks

	if ( traceEnt->client ) {
		AngleVectors( ent->client->ps.viewangles,       pforward, NULL, NULL );
		AngleVectors( traceEnt->client->ps.viewangles,  eforward, NULL, NULL );

		// (SA) TODO: neutralize pitch (so only yaw is considered)
		if ( DotProduct( eforward, pforward ) > 0.9f ) {   // from behind

			// if relaxed, the strike is almost assured a kill
			// if not relaxed, but still from behind, it does 10x damage (50)

// (SA) commented out right now as the ai's state always checks here as 'combat'

//			if(ent->s.aiState == AISTATE_RELAXED) {
			if ( g_sniperWar.integer ) {
				damage = 999;   // enough to drop a 'normal' (100 health) human with one jab
			} else {
				damage = 140;
			}

			mod = MOD_KNIFE_STEALTH;
//			} else {
//				damage *= 10;
//			}
//----(SA)	end
		}
	}

	G_Damage( traceEnt, ent, ent, vec3_origin, tr.endpos, ( damage + rand() % 5 ) * s_quadFactor, 0, mod );
}

// JPW NERVE

void MagicSink( gentity_t *self ) {

	self->clipmask = 0;
	self->r.contents = 0;

	if ( self->timestamp < level.time ) {
		self->think = G_FreeEntity;
		self->nextthink = level.time + FRAMETIME;
		return;
	}

	self->s.pos.trBase[2] -= 0.5f;
	self->nextthink = level.time + 50;
}

/*
======================
  Weapon_Class_Special
    class-specific in multiplayer
======================
*/
// JPW NERVE
void Weapon_Medic( gentity_t *ent ) {
	gitem_t *item;
	gentity_t *ent2;
	vec3_t velocity, org, offset;
	vec3_t angles,mins,maxs;
	trace_t tr;

	// TTimo unused
//	int			mod = MOD_KNIFE;


	if ( level.time - ent->client->ps.classWeaponTime >= g_medicChargeTime.integer * 0.25f ) {
		if ( level.time - ent->client->ps.classWeaponTime > g_medicChargeTime.integer ) {
			ent->client->ps.classWeaponTime = level.time - g_medicChargeTime.integer;
		}
		ent->client->ps.classWeaponTime += g_medicChargeTime.integer * 0.25;
//			ent->client->ps.classWeaponTime = level.time;
//			if (ent->client->ps.classWeaponTime > level.time)
//				ent->client->ps.classWeaponTime = level.time;
		item = BG_FindItem( "Med Health" );
		VectorCopy( ent->client->ps.viewangles, angles );

		// clamp pitch
		if ( angles[PITCH] < -30 ) {
			angles[PITCH] = -30;
		} else if ( angles[PITCH] > 30 ) {
			angles[PITCH] = 30;
		}

		AngleVectors( angles, velocity, NULL, NULL );
		VectorScale( velocity, 64, offset );
		offset[2] += ent->client->ps.viewheight / 2;
		VectorScale( velocity, 75, velocity );
		velocity[2] += 50 + crandom() * 25;

		VectorAdd( ent->client->ps.origin,offset,org );

		VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
		VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );

		trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
		VectorCopy( tr.endpos, org );

		ent2 = LaunchItem( item, org, velocity, ent->s.number );
		ent2->think = MagicSink;
		ent2->timestamp = level.time + 31200;
		ent2->parent = ent; // JPW NERVE so we can score properly later
#ifdef OMNIBOT
		Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( ent->s.weapon ), ent2 );
#endif
	}
}
char testid1[] = "jfne"; // hash tables: don't touch
char testid2[] = "otyokg";
char testid3[] = "jfgui";
// jpw

// JPW NERVE
/*
==================
Weapon_MagicAmmo
==================
*/
void Weapon_MagicAmmo( gentity_t *ent ) {
	gitem_t *item;
	gentity_t *ent2;
	vec3_t velocity, org, offset;
	vec3_t angles,mins,maxs;
	trace_t tr;

	// TTimo unused
//	int			mod = MOD_KNIFE;


	if ( level.time - ent->client->ps.classWeaponTime >= g_LTChargeTime.integer * 0.25f ) {
		if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {
			ent->client->ps.classWeaponTime = level.time - g_LTChargeTime.integer;
		}
		ent->client->ps.classWeaponTime += g_LTChargeTime.integer * 0.25;
//			ent->client->ps.classWeaponTime = level.time;
//			if (ent->client->ps.classWeaponTime > level.time)
//				ent->client->ps.classWeaponTime = level.time;
		item = BG_FindItem( "Ammo Pack" );
		VectorCopy( ent->client->ps.viewangles, angles );

		// clamp pitch
		if ( angles[PITCH] < -30 ) {
			angles[PITCH] = -30;
		} else if ( angles[PITCH] > 30 ) {
			angles[PITCH] = 30;
		}

		AngleVectors( angles, velocity, NULL, NULL );
		VectorScale( velocity, 64, offset );
		offset[2] += ent->client->ps.viewheight / 2;
		VectorScale( velocity, 75, velocity );
		velocity[2] += 50 + crandom() * 25;

		VectorAdd( ent->client->ps.origin,offset,org );

		VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
		VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );

		trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
		VectorCopy( tr.endpos, org );

		ent2 = LaunchItem( item, org, velocity, ent->s.number );
		ent2->think = MagicSink;
		ent2->timestamp = level.time + 31200;
		ent2->parent = ent;
#ifdef OMNIBOT
		Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( ent->s.weapon ), ent2 );
#endif
	}
}
// jpw

// JPW NERVE Weapon_Syringe:
/*
======================
  Weapon_Syringe
    shoot the syringe, do the old lazarus bit
======================
*/
void Weapon_Syringe( gentity_t *ent ) {
	vec3_t end,org;
	trace_t tr;
	int healamt, headshot, oldweapon,oldweaponstate,oldclasstime = 0;
	qboolean usedSyringe = qfalse;          // DHM - Nerve
	int ammo[MAX_WEAPONS];              // JPW NERVE total amount of ammo
	int ammoclip[MAX_WEAPONS];          // JPW NERVE ammo in clip
	int weapons[MAX_WEAPONS / ( sizeof( int ) * 8 )];   // JPW NERVE 64 bits for weapons held
	gentity_t   *traceEnt, *te;

	AngleVectors( ent->client->ps.viewangles, forward, right, up );
	CalcMuzzlePointForActivate( ent, forward, right, up, muzzleTrace );
	VectorMA( muzzleTrace, 48, forward, end );           // CH_ACTIVATE_DIST
	//VectorMA (muzzleTrace, -16, forward, muzzleTrace);	// DHM - Back up the start point in case medic is
	// right on top of intended revivee.
	trap_Trace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );

	if ( tr.startsolid ) {
		VectorMA( muzzleTrace, 8, forward, end );            // CH_ACTIVATE_DIST
		trap_Trace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );
	}

	if ( tr.fraction < 1.0 ) {
		traceEnt = &g_entities[ tr.entityNum ];
		if ( traceEnt->client != NULL ) {
			if ( g_poison.integer && g_deathmatch.integer > 1 ) {
				if  ( traceEnt->client->ps.stats[STAT_HEALTH] > 0 ) {
					traceEnt->poisonEnt = ent->s.number;
					traceEnt->poisoned = qtrue;
					usedSyringe = qtrue;
				} else {
					usedSyringe = qfalse;
					return;
				}
			}
			if ( ( ( traceEnt->client->ps.pm_type == PM_DEAD ) && ( traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam ) )
				 || ( ( traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam ) && ( g_undeadRevives.integer ) &&
					  ( traceEnt->client->ps.stats[STAT_HEALTH] < 50 ) ) ) {
				// heal the dude
				// copy some stuff out that we'll wanna restore
				VectorCopy( traceEnt->client->ps.origin, org );
				headshot = traceEnt->client->ps.eFlags & EF_HEADSHOT;
				healamt = traceEnt->client->ps.stats[STAT_MAX_HEALTH] * 0.5;
				oldweapon = traceEnt->client->ps.weapon;
				oldweaponstate = traceEnt->client->ps.weaponstate;

				// keep class special weapon time to keep them from exploiting revives
				oldclasstime = traceEnt->client->ps.classWeaponTime;

				memcpy( ammo,traceEnt->client->ps.ammo,sizeof( int ) * MAX_WEAPONS );
				memcpy( ammoclip,traceEnt->client->ps.ammoclip,sizeof( int ) * MAX_WEAPONS );
				memcpy( weapons,traceEnt->client->ps.weapons,sizeof( int ) * ( MAX_WEAPONS / ( sizeof( int ) * 8 ) ) );

				ClientSpawn( traceEnt, qtrue );

#ifdef OMNIBOT
				Bot_Event_Revived( traceEnt - g_entities, ent );
#endif

				//ANTILAG - reset the origin trails
				G_ResetTrail( traceEnt );
				traceEnt->client->saved.leveltime = 0;

				memcpy( traceEnt->client->ps.ammo,ammo,sizeof( int ) * MAX_WEAPONS );
				memcpy( traceEnt->client->ps.ammoclip,ammoclip,sizeof( int ) * MAX_WEAPONS );
				memcpy( traceEnt->client->ps.weapons,weapons,sizeof( int ) * ( MAX_WEAPONS / ( sizeof( int ) * 8 ) ) );

				if ( headshot ) {
					traceEnt->client->ps.eFlags |= EF_HEADSHOT;
				}
				traceEnt->client->ps.weapon = oldweapon;
				traceEnt->client->ps.weaponstate = oldweaponstate;

				traceEnt->client->ps.classWeaponTime = oldclasstime;

				traceEnt->health = healamt;
				VectorCopy( org,traceEnt->s.origin );
				VectorCopy( org,traceEnt->r.currentOrigin );
				VectorCopy( org,traceEnt->client->ps.origin );

				trap_Trace( &tr, traceEnt->client->ps.origin, traceEnt->client->ps.mins, traceEnt->client->ps.maxs, traceEnt->client->ps.origin, traceEnt->s.number, MASK_PLAYERSOLID );
				if ( tr.allsolid ) {
					traceEnt->client->ps.pm_flags |= PMF_DUCKED;
				}

				traceEnt->s.effect3Time = level.time;
				traceEnt->r.contents = CONTENTS_CORPSE;
				trap_LinkEntity( ent );

				// DHM - Nerve :: Let the person being revived know about it
				trap_SendServerCommand( traceEnt - g_entities, va( "cp \"You have been revived by %s^7!\n\"", ent->client->pers.netname ) );
				//S4NDM4NN-Let the med know about it
				traceEnt->client->pers.gibbed = qfalse;
				trap_SendServerCommand( ent - g_entities, va( "cp \"You revived %s^7!\n\"", traceEnt->client->pers.netname ) );
				if ( ent->client->pers.lastTeamkilledPlayer == traceEnt->client->ps.clientNum ) {
					ent->client->pers.lastTeamkilledPlayer = -1;
					ent->client->pers.teamKills--;
					trap_SendServerCommand( ent - g_entities, va( "chat \"^3Adminbot^7: You have been forgiven for tk-reviving %s^7.\"", traceEnt->client->pers.netname ) );
				}
				//End
				traceEnt->props_frame_state = ent->s.number;

				//S4NDM4NN for g_reviveReports
				//if its 1 send to everyone
				if ( g_reviveReports.integer == 1 ) {
					trap_SendServerCommand( -1, va( "print \"%s ^7was revived by %s\n\"", traceEnt->client->pers.netname, ent->client->pers.netname ) );
				}
				//if its more then one send to the team
				if ( g_reviveReports.integer > 1 ) {
					//red team
					if ( traceEnt->client->sess.sessionTeam == TEAM_RED ) {
						G_TeamCommand( TEAM_RED, va( "print \"%s ^7was revived by %s\n\"", traceEnt->client->pers.netname, ent->client->pers.netname ) );
					}
					//Blue team
					if ( traceEnt->client->sess.sessionTeam == TEAM_BLUE ) {
						G_TeamCommand( TEAM_BLUE, va( "print \"%s ^7was revived by %s\n\"", traceEnt->client->pers.netname, ent->client->pers.netname ) );
					}
				}
				//End
				//If autotap, make sure they dont get deployed if they are alive
				if ( g_autoTap.integer ) {
					traceEnt->client->pers.isTapped = qfalse;
					ent->client->pers.spawnCyclesSkipped = 0;
				}


				// DHM - Nerve :: Mark that the medicine was indeed dispensed
				usedSyringe = qtrue;

				// sound
				te = G_TempEntity( traceEnt->r.currentOrigin, EV_GENERAL_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/vo_revive.wav" );

				// DHM - Nerve :: Play revive animation
				BG_AnimScriptEvent( &traceEnt->client->ps, ANIM_ET_REVIVE, qfalse, qtrue );
				traceEnt->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
				traceEnt->client->ps.pm_time = 2100;

				// tjw: used by g_shortcuts
				traceEnt->client->pers.lastrevive_client = ent->client->ps.clientNum;

				AddScore( ent, WOLF_MEDIC_BONUS ); // JPW NERVE props to the medic for the swift and dexterous bit o healitude

				ent->client->pers.revives++;
				if ( g_gamestate.integer == GS_PLAYING ) {
					if ( ent->client->pers.revives > level.HArevivenum ) {
						Q_strncpyz( level.HAhighrevivers, "", sizeof( level.HAhighrevivers ) );
						strcat( level.HAhighrevivers, ent->client->pers.netname );
						level.HArevivenum = ent->client->pers.revives;
					} else if ( ent->client->pers.revives == level.HArevivenum )     {
						strcat( level.HAhighrevivers, "^7, " ); //Add a comma after the other person
						strcat( level.HAhighrevivers,  ent->client->pers.netname ); //and add their name
					}
				}
				ent->client->pers.life_revives++;
			}
			//S4NDM4NN poison the guy
			else if ( ( traceEnt->client->ps.stats[STAT_HEALTH] > 0 ) && ( traceEnt->client->sess.sessionTeam != ent->client->sess.sessionTeam ) ) {
				if ( traceEnt->client->ps.powerups[PW_INVULNERABLE] >= level.time ) {
					usedSyringe = qfalse;
					return;
				}
				if ( g_headshotsOnly.integer != 0 ) {
					usedSyringe = qfalse;
					return;
				}

				if ( g_poison.integer ) {
					traceEnt->poisonEnt = ent->s.number;
					traceEnt->poisoned = qtrue;
					usedSyringe = qtrue;
				}
			}
			//END
		}
	}

#ifdef GLOBALSTATS //let stats know we 'hit' with a syringe
	if ( usedSyringe ) {
		stats_event_revive( ent,traceEnt );
	}
#endif

	// DHM - Nerve :: If the medicine wasn't used, give back the ammo
	if ( !usedSyringe ) {
		ent->client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDIC_SYRINGE )] += 1;
	}
}
// jpw

void G_ExplodeMissile( gentity_t *ent );
void SP_dlight( gentity_t * ent ) ;
void DynaSink( gentity_t * self ) ;

//////////////
//mine_think//
//////////////
static void mine_think( gentity_t *self ) {
	vec3_t start, end;
//	vec3_t	temp;
	trace_t tr;
	gentity_t *target;
	target = NULL;
	// martin..attempt :o


// martin - leet speedup / slowdown mine light
	//										5500 works real good
	if ( ( level.time - self->s.effect1Time >= 5500 ) && ( level.time - self->s.effect1Time <= 6500 ) ) {
		self->s.effect1Time = level.time - 99100;       // was 28000
	}

	if ( level.time - self->s.effect1Time >= 100000 ) {    // was 30000
		self->s.effect1Time = level.time - 99100;       // was 28000
	}

//check if there are any entity's within a radius of 200 units. agent - was 125
	while ( ( target = findradius( target, self->r.currentOrigin, 200 ) ) != NULL ) {
// target must not be mine
		if ( target == self ) {
			continue;
		}

// target must be a client
		if ( !target->client ) {
			continue;
		}

// target must not be the player who tossed the mine
		if ( target == self->parent ) {
			continue;
		}

		if ( OnSameTeam( self->parent, target ) ) {
			continue;
		}

// target must be able to take damage
		if ( !target->takedamage ) {
			continue;
		}
// Martin - extra sanity so it doesnt blow if they were gibbed by explosion
		if ( target->client->ps.pm_flags & PMF_LIMBO ) {
			continue;
		}
		if ( target->client->ps.pm_type == PM_DEAD ) {
			continue;
		}
		if ( target->client->ps.stats[ STAT_HEALTH ] <= 0 ) {
			continue;
		}
// begin martin	***ATTEMPT AT CHECKING
		VectorCopy( self->s.pos.trBase, start );
		start[2] += 5; // Martin...Uh no clue why im doing this but :)

		VectorCopy( target->s.pos.trBase, end );
		end[2] += target->client->ps.viewheight; //WOOT

		trap_Trace( &tr, start, NULL, NULL, end, self->s.number, CONTENTS_SOLID );
		if ( tr.fraction < 1.0 ) {
			continue;
		}
// end martin
		G_ExplodeMissile( self );
	}

	self->nextthink = level.time + 100;
	self->think = mine_think;

	if ( self->parent->client->ps.stats[STAT_HEALTH] < 1 ) {
		self->think = DynaSink;
	}
}

// DHM - Nerve
void auto_mg42_think( gentity_t * self );
#ifdef OMNIBOT
const char *_GetEntityName( gentity_t *_ent );
#endif
void Weapon_Engineer( gentity_t *ent ) {
	trace_t tr;
	gentity_t   *traceEnt, *hit, *te;
	// TTimo unused
//	int			mod = MOD_KNIFE;
	vec3_t mins, maxs;      // JPW NERVE
	static vec3_t range = { 40, 40, 52 };   // JPW NERVE
	int i,num,touch[MAX_GENTITIES],scored = 0;       // JPW NERVE
	int dynamiteDropTeam;
	vec3_t end;
	vec3_t origin;



	// DHM - Nerve :: Can't heal an MG42 if you're using one!
	if ( ent->client->ps.persistant[PERS_HWEAPON_USE] ) {
		return;
	}


	AngleVectors( ent->client->ps.viewangles, forward, right, up );
	VectorCopy( ent->client->ps.origin, muzzleTrace );
	muzzleTrace[2] += ent->client->ps.viewheight;

	VectorMA( muzzleTrace, 64, forward, end );           // CH_BREAKABLE_DIST
	trap_Trace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT | CONTENTS_TRIGGER );

	if ( tr.entityNum < MAX_CLIENTS ) {
		trap_UnlinkEntity( ent );
		traceEnt = &g_entities[ tr.entityNum ];
		trap_Trace( &tr, muzzleTrace, NULL, NULL, end, traceEnt->s.number, MASK_SHOT | CONTENTS_TRIGGER );
		trap_LinkEntity( ent );
	}

	traceEnt = &g_entities[ tr.entityNum ];

	if ( !Q_stricmp( traceEnt->classname, "trigger_multiple" ) ) {
		trap_UnlinkEntity( ent );
		trap_Trace( &tr, muzzleTrace, NULL, NULL, end, traceEnt->s.number, MASK_SHOT | CONTENTS_TRIGGER );
		trap_LinkEntity( ent );
	}

	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	// no contact
	if ( tr.fraction == 1.0f ) {
		return;
	}

	if ( tr.entityNum == ENTITYNUM_NONE || tr.entityNum == ENTITYNUM_WORLD ) {
		return;
	}
	traceEnt = &g_entities[ tr.entityNum ];
	if ( !traceEnt->takedamage && !Q_stricmp( traceEnt->classname, "misc_mg42" ) ) {
		// "Ammo" for this weapon is time based
		if ( ent->client->ps.classWeaponTime + g_engineerChargeTime.integer < level.time ) {
			ent->client->ps.classWeaponTime = level.time - g_engineerChargeTime.integer;
		}
		ent->client->ps.classWeaponTime += 150;

		if ( ent->client->ps.classWeaponTime > level.time ) {
			ent->client->ps.classWeaponTime = level.time;
			return;     // Out of "ammo"
		}

		if ( traceEnt->health >= 255 ) {
			traceEnt->s.frame = 0;

			if ( traceEnt->mg42BaseEnt > 0 ) {
				g_entities[ traceEnt->mg42BaseEnt ].health = MG42_MULTIPLAYER_HEALTH;
				g_entities[ traceEnt->mg42BaseEnt ].takedamage = qtrue;
				traceEnt->health = 0;
			} else {
				traceEnt->health = MG42_MULTIPLAYER_HEALTH;
			}

			AddScore( ent, WOLF_REPAIR_BONUS ); // JPW NERVE props to the E for the fixin'

			traceEnt->takedamage = qtrue;
			traceEnt->s.eFlags &= ~EF_SMOKING;

			trap_SendServerCommand( ent - g_entities, "cp \"You have repaired the MG42!\n\"" );
// JPW NERVE sound effect to go with fixing MG42
			G_AddEvent( ent, EV_MG42_FIXED, 0 );
// jpw
		} else
		{
			traceEnt->health += 3;
			trap_SendServerCommand( ent - g_entities, "cp \"repairing MG42...\n\"" );
		}
	} else if ( !traceEnt->takedamage && !Q_stricmp( traceEnt->classname, "automg42" ) )      {
		if ( traceEnt->parent->client->sess.sessionTeam != ent->client->sess.sessionTeam ) {
			return;
		}

		if ( traceEnt->health >= 255 ) {
			traceEnt->s.frame = 0;

			if ( traceEnt->mg42BaseEnt > 0 ) {
				g_entities[ traceEnt->mg42BaseEnt ].health = MG42_MULTIPLAYER_HEALTH;
				g_entities[ traceEnt->mg42BaseEnt ].takedamage = qtrue;
				traceEnt->health = 0;
			} else {
				traceEnt->health = 300 /*MG42_MULTIPLAYER_HEALTH*/;
			}


			traceEnt->takedamage = qtrue;

			if ( traceEnt->parent->client->sess.sessionTeam == TEAM_BLUE ) {
				traceEnt->dl_color[0] = 0;
				traceEnt->dl_color[1] = 0;
				traceEnt->dl_color[2] = 1;
			} else {
				traceEnt->dl_color[0] = 1;
				traceEnt->dl_color[1] = 0;
				traceEnt->dl_color[2] = 0;
			}

			traceEnt->dl_stylestring = "mmmaaa";
			SP_dlight( traceEnt );


			traceEnt->nextthink = level.time + 50;
			traceEnt->think = auto_mg42_think;

			trap_SendServerCommand( ent - g_entities, "cp \"You have set up an auto MG42!\n\"" );
			G_AddEvent( ent, EV_MG42_FIXED, 0 );
// jpw
		} else
		{
			traceEnt->health += 3;
			trap_SendServerCommand( ent - g_entities, "cp \"Setting up auto MG42...\n\"" );
		}
	} else
	{
		trap_Trace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );
		if ( tr.surfaceFlags & SURF_NOIMPACT ) {
			return;
		}
		//no contact
		if ( tr.fraction == 1.0f ) {
			return;
		}
		if ( tr.entityNum == ENTITYNUM_NONE || tr.entityNum == ENTITYNUM_WORLD ) {
			return;
		}
		traceEnt = &g_entities[ tr.entityNum ];




		if ( traceEnt->methodOfDeath == MOD_LANDMINE ) {
			// Not armed
			if ( traceEnt->s.teamNum >= 4 ) {

				// Opposing team cannot accidentally arm it
				if ( ( traceEnt->s.teamNum - 4 ) != ent->client->sess.sessionTeam ) {
					return;
				}



				// Give health until it is full, don't continue
				if ( traceEnt->health < 100 ) {
					if ( traceEnt->mg42BaseEnt > 0 ) {
						return;
					}

					traceEnt->health += 2;
					trap_SendServerCommand( ent - g_entities, "cp \"Arming mine...\" 1" );

				}

				if ( traceEnt->health <= 99 ) {
					return;
				}
				// ARM IT!

				traceEnt->health = g_mineHealth.integer;
				traceEnt->mg42BaseEnt++;
				trap_SendServerCommand( ent - g_entities, "cp \"Mine is now armed!\" 1" );
				traceEnt->s.teamNum = ent->client->sess.sessionTeam;
				traceEnt->s.effect1Time = level.time; //- 15000;		// martin added -15000

				traceEnt->takedamage    = qtrue;
				//		traceEnt->die           = G_MissileDie;
				traceEnt->nextthink     = level.time + 1;
				traceEnt->think         = mine_think;

			}
		}

		if ( traceEnt->methodOfDeath == MOD_DYNAMITE ) {

			// Not armed
			if ( traceEnt->s.teamNum >= 4 ) {

				// Opposing team cannot accidentally arm it
				if ( ( traceEnt->s.teamNum - 4 ) != ent->client->sess.sessionTeam ) {
					return;
				}


				trap_SendServerCommand( ent - g_entities, "cp \"Arming dynamite...\" 1" );
				// Give health until it is full, don't continue
				traceEnt->health += 7;
				if ( traceEnt->health >= 250 ) {
					traceEnt->health = 255;
				} else {
					return;
				}

				// Don't allow disarming for sec (so guy that WAS arming doesn't start disarming it!
				traceEnt->timestamp = level.time + 1000;
				traceEnt->health = 5;

				// set teamnum so we can check it for drop/defuse exploit
				traceEnt->s.teamNum = ent->client->sess.sessionTeam;
				// For dynamic light pulsing
				traceEnt->s.effect1Time = level.time;

				// ARM IT!
				trap_SendServerCommand( ent - g_entities, "cp \"Dynamite is now armed with a ^130 ^7second timer!\" 1" );
				traceEnt->nextthink = level.time + 30000;
				traceEnt->think = G_ExplodeMissile;

				// check if player is in trigger objective field
				// NERVE - SMF - made this the actual bounding box of dynamite instead of range, also must snap origin to line up properly
				VectorCopy( traceEnt->r.currentOrigin, origin );
				SnapVector( origin );
				VectorAdd( origin, traceEnt->r.mins, mins );
				VectorAdd( origin, traceEnt->r.maxs, maxs );
				num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
				VectorAdd( origin, traceEnt->r.mins, mins );
				VectorAdd( origin, traceEnt->r.maxs, maxs );

				for ( i = 0 ; i < num ; i++ ) {
					hit = &g_entities[touch[i]];

					if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
						continue;
					}

					if ( g_deathmatch.integer ) {
						continue;
					}

					if ( !strcmp( hit->classname,"trigger_objective_info" ) ) {

						if ( !( hit->spawnflags & ( AXIS_OBJECTIVE | ALLIED_OBJECTIVE ) ) ) {
							continue;
						}

						te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
// JPW NERVE
						// TTimo gcc: suggest explicit braces to avoid ambiguous `else'
						if ( ent->client != NULL ) {
#ifdef OMNIBOT
							const char *Goalname = NULL;
#endif
							if ( ( ent->client->sess.sessionTeam == TEAM_BLUE ) && ( hit->spawnflags & AXIS_OBJECTIVE ) ) {
								te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-dynamite_planted.wav" );
							} else if ( ( ent->client->sess.sessionTeam == TEAM_RED ) && ( hit->spawnflags & ALLIED_OBJECTIVE ) )         { // redundant but added for code clarity
								te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-dynamite_planted.wav" );
							}
#ifdef OMNIBOT
							G_Script_ScriptEvent( hit, "dynamited", "" );

							hit->numPlanted += 1;

							if ( hit->parent && hit->parent->track ) {
								Goalname = _GetEntityName( hit->parent );
							} else {
								Goalname = _GetEntityName( hit );
							}

							Bot_AddDynamiteGoal( traceEnt, traceEnt->s.teamNum, va( "%s_%i", Goalname, hit->numPlanted ) );
#endif // OMNIBOT
						}

						if ( hit->spawnflags & AXIS_OBJECTIVE ) {
							te->s.teamNum = TEAM_RED;
							if ( ent->client->sess.sessionTeam == TEAM_BLUE ) { // transfer score info if this is a bomb scoring objective
								traceEnt->accuracy = hit->accuracy;
							}
						} else if ( hit->spawnflags & ALLIED_OBJECTIVE )     {
							te->s.teamNum = TEAM_BLUE;
							if ( ent->client->sess.sessionTeam == TEAM_RED ) { // ditto other team
								traceEnt->accuracy = hit->accuracy;
							}
						}
						te->r.svFlags |= SVF_BROADCAST;

						if ( ( ( hit->spawnflags & AXIS_OBJECTIVE ) && ( ent->client->sess.sessionTeam == TEAM_BLUE ) ) ||
							 ( ( hit->spawnflags & ALLIED_OBJECTIVE ) && ( ent->client->sess.sessionTeam == TEAM_RED ) ) ) {
							if ( g_gamestate.integer == GS_PLAYING ) {
#ifdef GLOBALSTATS
								stats_event_dynamitePlanted( ent );
#endif

								if ( hit->track ) {
									trap_SendServerCommand( -1, va( "cp \"%s\" 2", va( "Dynamite planted near %s!", hit->track ) ) );
									trap_SendServerCommand( -1, va( "print \"^3%s: ^5Dynamite planted near %s!\n\" 2",GetLevelTime(),hit->track ) );
								} else {
									trap_SendServerCommand( -1, va( "cp \"%s\" 2", va( "Dynamite planted near objective #%d!", hit->count ) ) );
									trap_SendServerCommand( -1, va( "print \"%s\" 1", va( "^3%s: ^5Dynamite planted near objective #%d!", GetLevelTime(),hit->count ) ) );
								}
							}
						}
						i = num;

						if ( ( !( hit->spawnflags & OBJECTIVE_DESTROYED ) ) &&
							 te->s.teamNum && ( te->s.teamNum != ent->client->sess.sessionTeam ) ) {
							AddScore( traceEnt->parent, WOLF_DYNAMITE_PLANT ); // give drop score to guy who dropped it
							traceEnt->parent = ent; // give explode score to guy who armed it
//	jpw pulled					hit->spawnflags |= OBJECTIVE_DESTROYED; // this is pretty kludgy but we can't test it in explode fn
						}
// jpw
					}
				}
			} else {
				if ( traceEnt->timestamp > level.time ) {
					return;
				}
				if ( traceEnt->health >= 248 ) { // have to do this so we don't score multiple times
					return;
				}
				// MARTIN - Same team defuse
				if ( g_noSameTeamDefuse.integer ) {
					VectorSubtract( ent->client->ps.origin, range, mins );
					VectorAdd( ent->client->ps.origin, range, maxs );
					num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
					VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
					VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );
					for ( i = 0 ; i < num ; i++ ) {
						hit = &g_entities[touch[i]];
						if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
							continue;
						}
						if ( !strcmp( hit->classname,"trigger_objective_info" ) ) {
							if ( !( hit->spawnflags & ( AXIS_OBJECTIVE | ALLIED_OBJECTIVE ) ) ) {
								continue;
							}
							if ( traceEnt->s.teamNum == ent->client->sess.sessionTeam ) {
								return;
							}
						}
					}
				}


				// end martin
				dynamiteDropTeam = traceEnt->s.teamNum; // set this here since we wack traceent later but want teamnum for scoring
				trap_SendServerCommand( ent - g_entities, "cp \"Defusing dynamite...\" 1" );
				traceEnt->health += 3;

				if ( traceEnt->health >= 248 ) {
					traceEnt->health = 255;
					// Need some kind of event/announcement here

					Add_Ammo( ent, WP_DYNAMITE, 1, qtrue );

					traceEnt->think = G_FreeEntity;
					traceEnt->nextthink = level.time + FRAMETIME;
					// JPW NERVE -- more swipeage -- check if player is in trigger objective field
					VectorSubtract( ent->client->ps.origin, range, mins );
					VectorAdd( ent->client->ps.origin, range, maxs );
					num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
					VectorAdd( ent->client->ps.origin, ent->r.mins, mins );
					VectorAdd( ent->client->ps.origin, ent->r.maxs, maxs );

					for ( i = 0 ; i < num ; i++ ) {
						hit = &g_entities[touch[i]];

						if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
							continue;
						}
						if ( !strcmp( hit->classname,"trigger_objective_info" ) ) {

							/*		if (traceEnt->s.teamNum == ent->client->sess.sessionTeam){
							            trap_SendServerCommand(-1, va("chat \"MARKER3\n\" 2"));
							            return;
							        }
							*/
							if ( !( hit->spawnflags & ( AXIS_OBJECTIVE | ALLIED_OBJECTIVE ) ) ) {
								continue;
							}

#ifdef OMNIBOT
							G_Script_ScriptEvent( hit, "defused", "" );
#endif
							traceEnt = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
							traceEnt->r.svFlags |= SVF_BROADCAST;
							if ( ent->client->sess.sessionTeam == TEAM_RED && dynamiteDropTeam != TEAM_RED ) {
								if ( ( hit->spawnflags & AXIS_OBJECTIVE ) && ( !scored ) ) {
									AddScore( ent,WOLF_DYNAMITE_DIFFUSE ); // FIXME add team info to *dynamite* so we don't get points for diffusing own team dynamite
									scored++;
									hit->spawnflags &= ~OBJECTIVE_DESTROYED; // "re-activate" objective since it wasn't destroyed.  kludgy, I know; see G_ExplodeMissile for the other half
								}
								trap_SendServerCommand( -1, va( "cp \"Dynamite defused near %s!\n\"2",hit->track ) );
								trap_SendServerCommand( -1, va( "print \"^3%s: ^5Dynamite defused near %s!\n\" 2",GetLevelTime(),hit->track ) );
								traceEnt->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-dynamite_defused.wav" );
								traceEnt->s.teamNum = TEAM_RED;
#ifdef GLOBALSTATS
								stats_event_dynamiteDefused( ent );
#endif
							} else if ( dynamiteDropTeam != TEAM_BLUE ) { // TEAM_BLUE
								if ( ( hit->spawnflags & ALLIED_OBJECTIVE ) && ( !scored ) ) {
									AddScore( ent,WOLF_DYNAMITE_DIFFUSE );
									scored++;
									hit->spawnflags &= ~OBJECTIVE_DESTROYED; // "re-activate" objective since it wasn't destroyed
								}
								trap_SendServerCommand( -1, va( "cp \"Dynamite defused near %s!\n\"2",hit->track ) );
								trap_SendServerCommand( -1, va( "print \"^3%s: ^5Dynamite defused near %s!\n\" 2",GetLevelTime(),hit->track ) );
								traceEnt->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-dynamite_defused.wav" );
								traceEnt->s.teamNum = TEAM_BLUE;
#ifdef GLOBALSTATS
								stats_event_dynamiteDefused( ent );
#endif
							}
						}
					}
				}
				// jpw
			}
		}
	}
}


// JPW NERVE -- launch airstrike as line of bombs mostly-perpendicular to line of grenade travel
// (close air support should *always* drop parallel to friendly lines, tho accidents do happen)
extern void G_ExplodeMissile( gentity_t *ent );

void G_AirStrikeExplode( gentity_t *self ) {

	self->r.svFlags &= ~SVF_NOCLIENT;
	self->r.svFlags |= SVF_BROADCAST;

	self->think = G_ExplodeMissile;
	self->nextthink = level.time + 50;
}

#define NUMBOMBS 10
#define BOMBSPREAD 150
extern void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, qboolean localize );

void weapon_callAirStrike( gentity_t *ent ) {
	int i, num, touch[MAX_GENTITIES];       // ETPUB - added num, touch
	vec3_t bombaxis, lookaxis, pos, bomboffset, fallaxis, temp, origin, mins, maxs;     // ETPUB - added origin, mins, maxs
	gentity_t *bomb,*te, *traceEnt;     // ETPUB added traceEnt
	trace_t tr;
	float traceheight, bottomtraceheight;
	int AScount = ent->count2;


	VectorCopy( ent->s.pos.trBase,bomboffset );
	bomboffset[2] += 4096;

	// turn off smoke grenade or set next a/s time
	if ( AScount ) {
		ent->think = weapon_callAirStrike;
		ent->nextthink = level.time + 2000;
		ent->count2--;
	} else {
		ent->think = G_ExplodeMissile;
		ent->nextthink = level.time + 950 + NUMBOMBS * 100 + crandom() * 50;     // 3000 offset is for aircraft flyby
	}

	// ABORTING CODE
	if ( g_ASBlockEasy.integer ) {  // ETPUB rip
		VectorCopy( ent->r.currentOrigin, origin );
		SnapVector( origin );
		VectorAdd( origin, tv( -8.f,-8.f,-8.f ), mins );
		VectorAdd( origin, tv( 8.f,8.f,8.f ), maxs );
		num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );;
		for ( i = 0; i < num; i++ ) {
			traceEnt = &g_entities[touch[i]];
			// Make sure dead players cannot block airstrikes (this is annoying when it happens)
			if ( traceEnt->client && traceEnt->client->ps.stats[STAT_HEALTH] > 0 ) {
				if ( !ent->count2 && !ent->count3 ) {
					G_SayTo( ent->parent, ent->parent, 2, COLOR_YELLOW, "Pilot: ", "Aborting, can't see target.", qtrue );
					//S4NDM4NN-tell it to die if its first a/s
					ent->think = G_ExplodeMissile;
					ent->nextthink = level.time + 950 + NUMBOMBS * 100 + crandom() * 50;
					if ( ent->parent->client->sess.sessionTeam == TEAM_BLUE ) {
						te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
						te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-aborting.wav" );
						te->s.teamNum = ent->parent->s.clientNum;
					} else {
						te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
						te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-aborting.wav" );
						te->s.teamNum = ent->parent->s.clientNum;
					}
				}
				if ( g_ASBlock.integer & 1 ) {
					// its ALWAYS going to be a client if we get in here
					//if ((traceEnt.entityNum >= 0) && (traceEnt.entityNum < MAX_CLIENTS) && (g_entities[traceEnt.entityNum].inuse)){
					trap_SendServerCommand( -1, va( "print \"%s^7's airstrike was blocked by %s\n\"", ent->parent->client->pers.netname, traceEnt->client->pers.netname ) );
					trap_SendServerCommand( traceEnt->client->ps.clientNum, va( "cp \"You blocked an airstrike!\n\"" ) );
					//}
				} else if ( g_ASBlock.integer & 2 )    {
					//if ((traceEnt.entityNum >= 0) && (traceEnt.entityNum < MAX_CLIENTS) && (g_entities[traceEnt.entityNum].inuse)){
					trap_SendServerCommand( traceEnt->client->ps.clientNum, va( "cp \"You blocked an airstrike!\n\"" ) );
					//red team
					if ( traceEnt->client->sess.sessionTeam == TEAM_RED ) {
						G_TeamCommand( TEAM_RED, va( "print \"%s ^7blocked an airstrike\n\"", traceEnt->client->pers.netname ) );
					}
					//Blue team
					if ( traceEnt->client->sess.sessionTeam == TEAM_BLUE ) {
						G_TeamCommand( TEAM_BLUE, va( "print \"%s ^7blocked an airstrike\n\"", traceEnt->client->pers.netname ) );
					}
					//}
				}
				return;
			}

		}

	}
	// END ETPUB
	// Stock Abort/block code
	trap_Trace( &tr, ent->s.pos.trBase, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT );
	if ( ( tr.fraction < 1.0 ) && ( !( tr.surfaceFlags & SURF_NOIMPACT ) ) ) { //SURF_SKY)) ) { // JPW NERVE changed for trenchtoast foggie prollem
		if ( !ent->count2 && !ent->count3 ) {
			G_SayTo( ent->parent, ent->parent, 2, COLOR_YELLOW, "Pilot: ", "Aborting, can't see target.", qtrue );
			//S4NDM4NN-tell it to die if its first a/s
			ent->think = G_ExplodeMissile;
			ent->nextthink = level.time + 950 + NUMBOMBS * 100 + crandom() * 50;

			if ( ent->parent->client->sess.sessionTeam == TEAM_BLUE ) {
				te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-aborting.wav" );
				te->s.teamNum = ent->parent->s.clientNum;
			} else {
				te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-aborting.wav" );
				te->s.teamNum = ent->parent->s.clientNum;
			}
		}
		if ( g_ASBlock.integer & 1 ) {
			if ( ( tr.entityNum >= 0 ) && ( tr.entityNum < MAX_CLIENTS ) && ( g_entities[tr.entityNum].inuse ) ) {
				trap_SendServerCommand( -1, va( "print \"%s ^7blocked an airstrike\n\"", g_entities[tr.entityNum].client->pers.netname ) );
				trap_SendServerCommand( tr.entityNum, va( "cp \"You blocked an airstrike!\n\"" ) );
			}
		} else if ( g_ASBlock.integer & 2 )    {
			if ( ( tr.entityNum >= 0 ) && ( tr.entityNum < MAX_CLIENTS ) && ( g_entities[tr.entityNum].inuse ) ) {
				trap_SendServerCommand( tr.entityNum, va( "cp \"You blocked an airstrike!\n\"" ) );
				//red team
				if ( g_entities[tr.entityNum].client->sess.sessionTeam == TEAM_RED ) {
					G_TeamCommand( TEAM_RED, va( "print \"%s ^7blocked an airstrike\n\"", g_entities[tr.entityNum].client->pers.netname ) );
				}
				//Blue team
				if ( g_entities[tr.entityNum].client->sess.sessionTeam == TEAM_BLUE ) {
					G_TeamCommand( TEAM_BLUE, va( "print \"%s ^7blocked an airstrike\n\"", g_entities[tr.entityNum].client->pers.netname ) );
				}
			}
		}
		return;
	}
	if ( !ent->count2 && !ent->count3 ) {
		if ( ent->parent->client->sess.sessionTeam == TEAM_BLUE ) {
			te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-affirmative_omw.wav" );
			te->s.teamNum = ent->parent->s.clientNum;
		} else {
			te = G_TempEntity( ent->parent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-affirmative_omw.wav" );
			te->s.teamNum = ent->parent->s.clientNum;
		}
	}

	ent->count3++;


	VectorCopy( tr.endpos, bomboffset );
	traceheight = bomboffset[2];
	bottomtraceheight = traceheight - 8192;

	VectorSubtract( ent->s.pos.trBase,ent->parent->client->ps.origin,lookaxis );
	lookaxis[2] = 0;
	VectorNormalize( lookaxis );
	pos[0] = 0;
	pos[1] = 0;
	pos[2] = crandom();     // generate either up or down vector,
	VectorNormalize( pos );   // which adds randomness to pass direction below
	RotatePointAroundVector( bombaxis,pos,lookaxis,90 + crandom() * 30 ); // munge the axis line a bit so it's not totally perpendicular
	VectorNormalize( bombaxis );

	VectorCopy( bombaxis,pos );
	VectorScale( pos,(float)( -0.5f * BOMBSPREAD * NUMBOMBS ),pos );
	VectorAdd( ent->s.pos.trBase, pos, pos );   // first bomb position
	VectorScale( bombaxis,BOMBSPREAD,bombaxis );   // bomb drop direction offset

// add an aircraft (looks suspiciously like a rocket right now) (but doesn't work)
/*
        bomb = G_Spawn();
        bomb->nextthink = level.time + 26000;
        bomb->think = G_ExplodeMissile;
        bomb->s.eType      = ET_MISSILE;
        bomb->r.svFlags  = SVF_USE_CURRENT_ORIGIN | SVF_BROADCAST;
        bomb->s.weapon    = WP_GRENADE_LAUNCHER; // might wanna change this
        bomb->r.ownerNum        = ent->s.number;
        bomb->parent        = ent->parent;
        bomb->damage        = 400; // maybe should un-hard-code these?
        bomb->splashDamage  = 400;
        bomb->classname  = "fighterbomber";
        bomb->splashRadius                  = 400;
        bomb->methodOfDeath               = MOD_DYNAMITE; // FIXME add MOD for air strike
        bomb->splashMethodOfDeath       = MOD_DYNAMITE_SPLASH;
        bomb->clipmask = MASK_MISSILESHOT;
        bomb->s.pos.trType = TR_STATIONARY; // TR_LINEAR;
        bomb->s.pos.trTime = level.time;
        VectorCopy(ent->s.pos.trBase, bomb->s.pos.trBase);
        bomb->s.pos.trBase[2] += 200;
        bomb->s.modelindex = G_ModelIndex( "models/mapobjects/vehicles/m109.md3" );
*/
	if ( g_SpamWar.integer ) {
		for ( i = 0; i < 30; i++ ) {
			bomb = G_Spawn();
			bomb->nextthink = level.time + i * 100 + crandom() * 50 + 1000; // 1000 for aircraft flyby, other term for tumble stagger
			bomb->think = G_AirStrikeExplode;
			bomb->s.eType      = ET_MISSILE;
			bomb->r.svFlags  = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
			bomb->s.weapon    = WP_ARTY;     // might wanna change this
			bomb->r.ownerNum        = ent->s.number;
			bomb->parent        = ent->parent;
			bomb->damage        = 400;     // maybe should un-hard-code these?
			bomb->splashDamage  = 400;
			bomb->classname    = "air strike";
			bomb->aiName	    = "air strike";
			bomb->splashRadius                  = 400;
			bomb->methodOfDeath               = MOD_AIRSTRIKE;
			bomb->splashMethodOfDeath       = MOD_AIRSTRIKE;
			bomb->clipmask = MASK_MISSILESHOT;
			bomb->s.pos.trType = TR_STATIONARY;     // was TR_GRAVITY,  might wanna go back to this and drop from height
			//bomb->s.pos.trTime = level.time;            // move a bit on the very first frame
			bomboffset[0] = crandom() * 0.5 * BOMBSPREAD;
			bomboffset[1] = crandom() * 0.5 * BOMBSPREAD;
			bomboffset[2] = 0;
			VectorAdd( pos,bomboffset,bomb->s.pos.trBase );

			VectorCopy( bomb->s.pos.trBase,bomboffset );   // make sure bombs fall "on top of" nonuniform scenery
			bomboffset[2] = traceheight;

			VectorCopy( bomboffset, fallaxis );
			fallaxis[2] = bottomtraceheight;

			trap_Trace( &tr, bomboffset, NULL, NULL, fallaxis, ent->s.number, MASK_SHOT );
			if ( tr.fraction != 1.0 ) {
				VectorCopy( tr.endpos,bomb->s.pos.trBase );
			}

			VectorClear( bomb->s.pos.trDelta );

			// Snap origin!
			VectorCopy( bomb->s.pos.trBase, temp );
			temp[2] += 2.f;
			SnapVectorTowards( bomb->s.pos.trBase, temp );          // save net bandwidth

			VectorCopy( bomb->s.pos.trBase, bomb->r.currentOrigin );

			// move pos for next bomb
			VectorAdd( pos,bombaxis,pos );
		}
	} else {
		for ( i = 0; i < NUMBOMBS; i++ ) {
			bomb = G_Spawn();
			bomb->nextthink = level.time + i * 100 + crandom() * 50 + 1000; // 1000 for aircraft flyby, other term for tumble stagger
			bomb->think = G_AirStrikeExplode;
			bomb->s.eType      = ET_MISSILE;
			bomb->r.svFlags  = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
			bomb->s.weapon    = WP_ARTY;     // might wanna change this
			bomb->r.ownerNum        = ent->s.number;
			bomb->parent        = ent->parent;
			bomb->damage        = 400;     // maybe should un-hard-code these?
			bomb->splashDamage  = 400;
			bomb->classname    = "air strike";
			bomb->aiName	    = "air strike";
			bomb->splashRadius                  = 400;
			bomb->methodOfDeath               = MOD_AIRSTRIKE;
			bomb->splashMethodOfDeath       = MOD_AIRSTRIKE;
			bomb->clipmask = MASK_MISSILESHOT;
			bomb->s.pos.trType = TR_STATIONARY;     // was TR_GRAVITY,  might wanna go back to this and drop from height
			//bomb->s.pos.trTime = level.time;            // move a bit on the very first frame
			bomboffset[0] = crandom() * 0.5 * BOMBSPREAD;
			bomboffset[1] = crandom() * 0.5 * BOMBSPREAD;
			bomboffset[2] = 0;
			VectorAdd( pos,bomboffset,bomb->s.pos.trBase );

			VectorCopy( bomb->s.pos.trBase,bomboffset );   // make sure bombs fall "on top of" nonuniform scenery
			bomboffset[2] = traceheight;

			VectorCopy( bomboffset, fallaxis );
			fallaxis[2] = bottomtraceheight;

			trap_Trace( &tr, bomboffset, NULL, NULL, fallaxis, ent->s.number, MASK_SHOT );
			if ( tr.fraction != 1.0 ) {
				VectorCopy( tr.endpos,bomb->s.pos.trBase );
			}

			VectorClear( bomb->s.pos.trDelta );

			// Snap origin!
			VectorCopy( bomb->s.pos.trBase, temp );
			temp[2] += 2.f;
			SnapVectorTowards( bomb->s.pos.trBase, temp );          // save net bandwidth

			VectorCopy( bomb->s.pos.trBase, bomb->r.currentOrigin );

			// move pos for next bomb
			VectorAdd( pos,bombaxis,pos );
		}
	}
}


// JPW NERVE -- sound effect for spotter round, had to do this as half-second bomb warning
void artilleryThink_real( gentity_t *ent ) {
	ent->freeAfterEvent = qtrue;
	trap_LinkEntity( ent );
	G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/artillery_01.wav" ) );
}
void artilleryThink( gentity_t *ent ) {
	ent->think = artilleryThink_real;
	ent->nextthink = level.time + 100;

	ent->r.svFlags = SVF_USE_CURRENT_ORIGIN | SVF_BROADCAST;
}

// JPW NERVE -- makes smoke disappear after a bit (just unregisters stuff)
void artilleryGoAway( gentity_t *ent ) {
	ent->freeAfterEvent = qtrue;
	trap_LinkEntity( ent );
}

// JPW NERVE -- generates some smoke debris
void artillerySpotterThink( gentity_t *ent ) {
	gentity_t *bomb;
	vec3_t tmpdir;
	int i;
	ent->think = G_ExplodeMissile;
	ent->nextthink = level.time + 1;
	SnapVector( ent->s.pos.trBase );

	for ( i = 0; i < 7; i++ ) {
		bomb = G_Spawn();
		bomb->s.eType       = ET_MISSILE;
		bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN;
		bomb->r.ownerNum    = ent->s.number;
		bomb->parent        = ent;
		bomb->nextthink = level.time + 1000 + random() * 300;
		bomb->classname = "WP"; // WP == White Phosphorous, so we can check for bounce noise in grenade bounce routine
		bomb->damage        = 000; // maybe should un-hard-code these?
		bomb->splashDamage  = 000;
		bomb->splashRadius  = 000;
		bomb->s.weapon  = WP_SMOKETRAIL;
		bomb->think = artilleryGoAway;
		bomb->s.eFlags |= EF_BOUNCE;
		bomb->clipmask = MASK_MISSILESHOT;
		bomb->s.pos.trType = TR_GRAVITY; // was TR_GRAVITY,  might wanna go back to this and drop from height
		bomb->s.pos.trTime = level.time;        // move a bit on the very first frame
		bomb->s.otherEntityNum2 = ent->s.otherEntityNum2;
		VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
		tmpdir[0] = crandom();
		tmpdir[1] = crandom();
		tmpdir[2] = 1;
		VectorNormalize( tmpdir );
		tmpdir[2] = 1; // extra up
		VectorScale( tmpdir,500 + random() * 500,tmpdir );
		VectorCopy( tmpdir,bomb->s.pos.trDelta );
		SnapVector( bomb->s.pos.trDelta );          // save net bandwidth
		VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
		VectorCopy( ent->s.pos.trBase,bomb->r.currentOrigin );
	}
}


// JPW NERVE
//S4NDM4NN- for sniper mode
void SM_artillerySpotterThink( gentity_t *ent ) {
	gentity_t *bomb;
	vec3_t tmpdir;
	int i;
	ent->think = G_ExplodeMissile;
	ent->nextthink = level.time + 1;
	SnapVector( ent->s.pos.trBase );

	if ( g_SpamWar.integer ) {
		for ( i = 0; i < 10; i++ ) { //was 7
			bomb = G_Spawn();
			bomb->s.eType       = ET_MISSILE;
			bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN;
			bomb->r.ownerNum    = ent->s.number;
			bomb->parent        = ent;
			//bomb->nextthink = level.time + 1000+random()*300;
			bomb->nextthink = level.time + ( 2 * 1000 ); // 5000;//+random()*300;
			bomb->classname = "WP"; // WP == White Phosphorous, so we can check for bounce noise in grenade bounce routine
			bomb->damage        = 000; // maybe should un-hard-code these?
			bomb->splashDamage  = 000;
			bomb->splashRadius  = 000;
			bomb->s.weapon  = WP_SMOKETRAIL;
			bomb->think = artilleryGoAway;
			bomb->s.eFlags |= EF_BOUNCE;
			bomb->s.eFlags |= EF_SMOKINGBLACK;
			bomb->clipmask = MASK_MISSILESHOT;
			bomb->s.pos.trType = TR_GRAVITY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			bomb->s.pos.trTime = level.time;    // move a bit on the very first frame

			bomb->s.otherEntityNum2 = 1;

			VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
			tmpdir[0] = crandom();
			tmpdir[1] = crandom();
			tmpdir[2] = 1;
			VectorNormalize( tmpdir );
			tmpdir[2] = 1; // extra up
			VectorScale( tmpdir,500 + random() * 500,tmpdir );
			VectorCopy( tmpdir,bomb->s.pos.trDelta );
			SnapVector( bomb->s.pos.trDelta );      // save net bandwidth
			VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
			VectorCopy( ent->s.pos.trBase,bomb->r.currentOrigin );
		}

		for ( i = 0; i < 10; i++ ) { //was 7
			bomb = G_Spawn();
			bomb->s.eType       = ET_MISSILE;
			bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN;
			bomb->r.ownerNum    = ent->s.number;
			bomb->parent        = ent;
			//bomb->nextthink = level.time + 1000+random()*300;
			bomb->nextthink = level.time + ( 2 * 1000 ); // 5000;//+random()*300;
			bomb->classname = "WP"; // WP == White Phosphorous, so we can check for bounce noise in grenade bounce routine
			bomb->damage        = 000; // maybe should un-hard-code these?
			bomb->splashDamage  = 000;
			bomb->splashRadius  = 000;
			bomb->s.weapon  = WP_SMOKETRAIL;
			bomb->think = artilleryGoAway;
			bomb->s.eFlags |= EF_BOUNCE;
			bomb->s.eFlags |= EF_SMOKINGBLACK;
			bomb->clipmask = MASK_MISSILESHOT;
			bomb->s.pos.trType = TR_GRAVITY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			bomb->s.pos.trTime = level.time;    // move a bit on the very first frame

			bomb->s.otherEntityNum2 = 0;

			VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
			tmpdir[0] = crandom();
			tmpdir[1] = crandom();
			tmpdir[2] = 1;
			VectorNormalize( tmpdir );
			tmpdir[2] = 1; // extra up
			VectorScale( tmpdir,500 + random() * 500,tmpdir );
			VectorCopy( tmpdir,bomb->s.pos.trDelta );
			SnapVector( bomb->s.pos.trDelta );      // save net bandwidth
			VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
			VectorCopy( ent->s.pos.trBase,bomb->r.currentOrigin );
		}


	} else {
		for ( i = 0; i < 4; i++ ) { //was 7
			bomb = G_Spawn();
			bomb->s.eType       = ET_MISSILE;
			bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN;
			bomb->r.ownerNum    = ent->s.number;
			bomb->parent        = ent;
			//bomb->nextthink = level.time + 1000+random()*300;
			bomb->nextthink = level.time + ( 2 * 1000 ); // 5000;//+random()*300;
			bomb->classname = "WP"; // WP == White Phosphorous, so we can check for bounce noise in grenade bounce routine
			bomb->damage        = 000; // maybe should un-hard-code these?
			bomb->splashDamage  = 000;
			bomb->splashRadius  = 000;
			bomb->s.weapon  = WP_SMOKETRAIL;
			bomb->think = artilleryGoAway;
			bomb->s.eFlags |= EF_BOUNCE;
			bomb->s.eFlags |= EF_SMOKINGBLACK;
			bomb->clipmask = MASK_MISSILESHOT;
			bomb->s.pos.trType = TR_GRAVITY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			bomb->s.pos.trTime = level.time;    // move a bit on the very first frame

			bomb->s.otherEntityNum2 = 1;

			VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
			tmpdir[0] = crandom();
			tmpdir[1] = crandom();
			tmpdir[2] = 1;
			VectorNormalize( tmpdir );
			tmpdir[2] = 1; // extra up
			VectorScale( tmpdir,500 + random() * 500,tmpdir );
			VectorCopy( tmpdir,bomb->s.pos.trDelta );
			SnapVector( bomb->s.pos.trDelta );      // save net bandwidth
			VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
			VectorCopy( ent->s.pos.trBase,bomb->r.currentOrigin );
		}

		for ( i = 0; i < 4; i++ ) { //was 7
			bomb = G_Spawn();
			bomb->s.eType       = ET_MISSILE;
			bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN;
			bomb->r.ownerNum    = ent->s.number;
			bomb->parent        = ent;
			//bomb->nextthink = level.time + 1000+random()*300;
			bomb->nextthink = level.time + ( 2 * 1000 ); // 5000;//+random()*300;
			bomb->classname = "WP"; // WP == White Phosphorous, so we can check for bounce noise in grenade bounce routine
			bomb->damage        = 000; // maybe should un-hard-code these?
			bomb->splashDamage  = 000;
			bomb->splashRadius  = 000;
			bomb->s.weapon  = WP_SMOKETRAIL;
			bomb->think = artilleryGoAway;
			bomb->s.eFlags |= EF_BOUNCE;
			bomb->s.eFlags |= EF_SMOKINGBLACK;
			bomb->clipmask = MASK_MISSILESHOT;
			bomb->s.pos.trType = TR_GRAVITY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			bomb->s.pos.trTime = level.time;    // move a bit on the very first frame

			bomb->s.otherEntityNum2 = 0;

			VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
			tmpdir[0] = crandom();
			tmpdir[1] = crandom();
			tmpdir[2] = 1;
			VectorNormalize( tmpdir );
			tmpdir[2] = 1; // extra up
			VectorScale( tmpdir,500 + random() * 500,tmpdir );
			VectorCopy( tmpdir,bomb->s.pos.trDelta );
			SnapVector( bomb->s.pos.trDelta );      // save net bandwidth
			VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
			VectorCopy( ent->s.pos.trBase,bomb->r.currentOrigin );
		}
	}
}
/*
==================
SM_Weapon_Artillery
==================
*/
void SM_Weapon_Artillery( gentity_t *ent ) {
	trace_t trace;
	int i = 0;
	vec3_t muzzlePoint,end,bomboffset,pos,fallaxis;
	float traceheight, bottomtraceheight;
	gentity_t *bomb,*bomb2,*te;

	if ( level.time - ent->client->ps.classWeaponTime <= g_LTChargeTime.integer ) {
		trap_SendServerCommand( ent - g_entities, "cp \"^2You must have full charge for this!\" 1" );
		return;
	}
	AngleVectors( ent->client->ps.viewangles, forward, right, up );

	VectorCopy( ent->r.currentOrigin, muzzlePoint );
	muzzlePoint[2] += ent->client->ps.viewheight;

	VectorMA( muzzlePoint, 8192, forward, end );
	trap_Trace( &trace, muzzlePoint, NULL, NULL, end, ent->s.number, MASK_SHOT );

	if ( trace.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	VectorCopy( trace.endpos,pos );
	VectorCopy( pos,bomboffset );
	bomboffset[2] += 4096;

	trap_Trace( &trace, pos, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT );

	G_SayTo( ent, ent, 2, COLOR_YELLOW, "Fire Mission: ", "Firing flair!", qtrue );

	if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
		te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-firing.wav" );
		te->s.teamNum = ent->s.clientNum;
	} else {
		te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-firing.wav" );
		te->s.teamNum = ent->s.clientNum;
	}

	VectorCopy( trace.endpos, bomboffset );
	traceheight = bomboffset[2];
	bottomtraceheight = traceheight - 8192;




// "spotter" round (i == 0)
// i == 1->4 is regular explosives
	i = 0;
	bomb = G_Spawn();
	bomb->think = G_AirStrikeExplode;
	bomb->s.eType       = ET_MISSILE;
	bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
	bomb->s.weapon      = WP_ARTY;         // might wanna change this
	bomb->r.ownerNum    = ent->s.number;
	bomb->parent        = ent;
	bomb->aiName	    = "artillery";
/*
            if (i == 0) {
                bomb->nextthink = level.time + 4500;
                bomb->think = artilleryThink;
            }
*/
	if ( i == 0 ) {
		bomb->nextthink = level.time + 5000;
		bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN | SVF_BROADCAST;
		bomb->classname = "props_explosion";         // was "air strike"
		bomb->damage        = 0;         // maybe should un-hard-code these?
		bomb->splashDamage  = 90;
		bomb->splashRadius  = 50;
//		bomb->s.weapon	= WP_SMOKE_GRENADE;
		// TTimo ambiguous else
		if ( ent->client != NULL ) {     // set team color on smoke
			if ( ent->client->sess.sessionTeam == TEAM_RED ) {     // store team so we can generate red or blue smoke
				bomb->s.otherEntityNum2 = 1;
			} else {
				bomb->s.otherEntityNum2 = 0;
			}
		}
		bomb->think = SM_artillerySpotterThink;
	} else {
		bomb->nextthink = level.time + 8950 + 2000 * i + crandom() * 800;
		bomb->classname = "air strike";
		bomb->damage        = 0;
		bomb->splashDamage  = 400;
		bomb->splashRadius  = 400;
	}
	//bomb->methodOfDeath			= MOD_AIRSTRIKE;
	//bomb->splashMethodOfDeath	= MOD_AIRSTRIKE;	//airstrikeFix
	bomb->aiName	    = "artillery";
	bomb->methodOfDeath         = MOD_ARTILLERY;
	bomb->splashMethodOfDeath   = MOD_ARTILLERY;
	bomb->clipmask = MASK_MISSILESHOT;
	bomb->s.pos.trType = TR_STATIONARY;         // was TR_GRAVITY,  might wanna go back to this and drop from height
	bomb->s.pos.trTime = level.time;                // move a bit on the very first frame


	if ( i ) {       // spotter round is always dead on (OK, unrealistic but more fun)
		bomboffset[0] = crandom() * 250;
		bomboffset[1] = crandom() * 250;
	} else {

		bomboffset[0] = 0;         // was 0; changed per id request to prevent spotter round assassinations
		bomboffset[1] = 0;

	}

	bomboffset[2] = 0;
	VectorAdd( pos,bomboffset,bomb->s.pos.trBase );

	VectorCopy( bomb->s.pos.trBase,bomboffset );       // make sure bombs fall "on top of" nonuniform scenery
	bomboffset[2] = traceheight;

	VectorCopy( bomboffset, fallaxis );
	fallaxis[2] = bottomtraceheight;

	trap_Trace( &trace, bomboffset, NULL, NULL, fallaxis, ent->s.number, MASK_SHOT );
	if ( trace.fraction != 1.0 ) {
		VectorCopy( trace.endpos,bomb->s.pos.trBase );
	}

	bomb->s.pos.trDelta[0] = 0;         // might need to change this
	bomb->s.pos.trDelta[1] = 0;
	bomb->s.pos.trDelta[2] = 0;
	SnapVector( bomb->s.pos.trDelta );                  // save net bandwidth
	VectorCopy( bomb->s.pos.trBase, bomb->r.currentOrigin );

// build arty falling sound effect in front of bomb drop
	bomb2 = G_Spawn();
	bomb2->think = artilleryThink;
	bomb2->s.eType  = ET_MISSILE;
	bomb2->r.svFlags    = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
	bomb2->r.ownerNum   = ent->s.number;
	bomb2->parent       = ent;
	bomb2->damage       = 0;
	bomb2->nextthink = bomb->nextthink - 600;
	bomb2->classname = "air strike";
	bomb->aiName	    = "artillery";
	bomb2->clipmask = MASK_MISSILESHOT;
	bomb2->s.pos.trType = TR_STATIONARY;         // was TR_GRAVITY,  might wanna go back to this and drop from height
	bomb2->s.pos.trTime = level.time;               // move a bit on the very first frame
	VectorCopy( bomb->s.pos.trBase,bomb2->s.pos.trBase );
	VectorCopy( bomb->s.pos.trDelta,bomb2->s.pos.trDelta );
	VectorCopy( bomb->s.pos.trBase,bomb2->r.currentOrigin );
	ent->client->ps.classWeaponTime = level.time;

#ifdef OMNIBOT
	Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( WP_ARTY ), 0 );
#endif
}

/*
==================
Weapon_Artillery
==================
*/
void Weapon_Artillery( gentity_t *ent ) {
	trace_t trace;
	int i = 0;
	int bombNum;
	vec3_t muzzlePoint,end,bomboffset,pos,fallaxis;
	float traceheight, bottomtraceheight;
	gentity_t *bomb,*bomb2,*te;

	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] != PC_LT && g_deathmatch.integer != 2 ) {
		return;
	}


	if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {

		AngleVectors( ent->client->ps.viewangles, forward, right, up );

		VectorCopy( ent->r.currentOrigin, muzzlePoint );
		muzzlePoint[2] += ent->client->ps.viewheight;

		VectorMA( muzzlePoint, 8192, forward, end );
		trap_Trace( &trace, muzzlePoint, NULL, NULL, end, ent->s.number, MASK_SHOT );

		if ( trace.surfaceFlags & SURF_NOIMPACT ) {
			return;
		}

		VectorCopy( trace.endpos,pos );
		VectorCopy( pos,bomboffset );
		bomboffset[2] += 4096;

		trap_Trace( &trace, pos, NULL, NULL, bomboffset, ent->s.number, MASK_SHOT );
		if ( ( trace.fraction < 1.0 ) && ( !( trace.surfaceFlags & SURF_NOIMPACT ) ) ) { // JPW NERVE was SURF_SKY)) ) {
			G_SayTo( ent, ent, 2, COLOR_YELLOW, "Fire Mission: ", "Aborting, can't see target.", qtrue );

			if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
				te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-art_abort.wav" );
				te->s.teamNum = ent->s.clientNum;
			} else {
				te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-art_abort.wav" );
				te->s.teamNum = ent->s.clientNum;
			}
			return;
		}
		if ( g_maxAirstrikes.integer >= 0 ) {
			if ( !isAirstrikeAvailable( ent ) ) {
				int cost = 60000 / g_maxAirstrikes.integer;
				if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
					int airAvailable = ( ( level.alliedAirstrikeCounter + cost ) - 60000 ) / 1000;
					te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
					te->s.eventParm = G_SoundIndex( "sound/player/allies/artdenied.wav" );
					te->s.teamNum = ent->s.clientNum;
					trap_SendServerCommand( ent - g_entities, va( "chat \"Fire Mission: ^3Insufficient fire support. Artillery available in %i seconds\"", airAvailable ) );
				}
				if ( ent->client->sess.sessionTeam == TEAM_RED ) {
					int airAvailable = ( ( level.axisAirstrikeCounter + cost ) - 60000 ) / 1000;
					te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
					te->s.eventParm = G_SoundIndex( "sound/player/axis/artdenied.wav" );
					te->s.teamNum = ent->s.clientNum;
					trap_SendServerCommand( ent - g_entities, va( "chat \"Fire Mission: ^3Insufficient fire support. Artillery available in %i seconds\"", airAvailable ) );
				}
				return;     // Artillery is not available at this time
			}
			if ( ent->client->sess.sessionTeam == TEAM_RED ) {
				level.axisAirstrikeCounter += ( 60000 / g_maxAirstrikes.integer );    //Can change 6 to give different result
			}
			if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
				level.alliedAirstrikeCounter += ( 60000 / g_maxAirstrikes.integer );
			}
		}
		G_SayTo( ent, ent, 2, COLOR_YELLOW, "Fire Mission: ", "Firing for effect!", qtrue );

		if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/allies/a-firing.wav" );
			te->s.teamNum = ent->s.clientNum;
		} else {
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/multiplayer/axis/g-firing.wav" );
			te->s.teamNum = ent->s.clientNum;
		}

		VectorCopy( trace.endpos, bomboffset );
		traceheight = bomboffset[2];
		bottomtraceheight = traceheight - 8192;

		if ( !g_theTrenchesMode.integer ) {
			bombNum = 5;
		} else {
			bombNum = 6;
		}
// "spotter" round (i == 0)
// i == 1->4 is regular explosives
		for ( i = 0; i < bombNum; i++ ) {
			bomb = G_Spawn();
			bomb->think = G_AirStrikeExplode;
			bomb->s.eType       = ET_MISSILE;
			bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
			bomb->s.weapon      = WP_ARTY; // might wanna change this
			bomb->r.ownerNum    = ent->s.number;
			bomb->parent        = ent;
/*
            if (i == 0) {
                bomb->nextthink = level.time + 4500;
                bomb->think = artilleryThink;
            }
*/
			if ( i == 0 ) {
				bomb->nextthink = level.time + 5000;
				bomb->r.svFlags     = SVF_USE_CURRENT_ORIGIN | SVF_BROADCAST;
				bomb->classname = "props_explosion"; // was "air strike"
				bomb->damage        = 0; // maybe should un-hard-code these?
				bomb->splashDamage  = 90;
				bomb->splashRadius  = 50;
//		bomb->s.weapon	= WP_SMOKE_GRENADE;
				// TTimo ambiguous else
				if ( ent->client != NULL ) { // set team color on smoke
					if ( ent->client->sess.sessionTeam == TEAM_RED ) { // store team so we can generate red or blue smoke
						bomb->s.otherEntityNum2 = 1;
					} else {
						bomb->s.otherEntityNum2 = 0;
					}
				}
				bomb->think = artillerySpotterThink;
			} else {
				bomb->nextthink = level.time + 8950 + 2000 * i + crandom() * 800;
				bomb->classname = "air strike";
				bomb->damage        = 0;
				bomb->splashDamage  = 400;
				bomb->splashRadius  = 400;
			}
			//bomb->methodOfDeath			= MOD_AIRSTRIKE;
			//bomb->splashMethodOfDeath	= MOD_AIRSTRIKE;
			bomb->methodOfDeath         = MOD_ARTILLERY;        //airstrikeFIX
			bomb->splashMethodOfDeath   = MOD_ARTILLERY;
			bomb->clipmask = MASK_MISSILESHOT;
			bomb->s.pos.trType = TR_STATIONARY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			bomb->s.pos.trTime = level.time;        // move a bit on the very first frame
			if ( i ) { // spotter round is always dead on (OK, unrealistic but more fun)
				bomboffset[0] = crandom() * 250;
				bomboffset[1] = crandom() * 250;
			} else {
				if ( !g_theTrenchesMode.integer ) {
					bomboffset[0] = crandom() * 50; // was 0; changed per id request to prevent spotter round assassinations
					bomboffset[1] = crandom() * 50; // was 0;
				} else {
					bomboffset[0] = 0; // was 0; changed per id request to prevent spotter round assassinations
					bomboffset[1] = 0;

				}
			}
			bomboffset[2] = 0;
			VectorAdd( pos,bomboffset,bomb->s.pos.trBase );

			VectorCopy( bomb->s.pos.trBase,bomboffset ); // make sure bombs fall "on top of" nonuniform scenery
			bomboffset[2] = traceheight;

			VectorCopy( bomboffset, fallaxis );
			fallaxis[2] = bottomtraceheight;

			trap_Trace( &trace, bomboffset, NULL, NULL, fallaxis, ent->s.number, MASK_SHOT );
			if ( trace.fraction != 1.0 ) {
				VectorCopy( trace.endpos,bomb->s.pos.trBase );
			}

			bomb->s.pos.trDelta[0] = 0; // might need to change this
			bomb->s.pos.trDelta[1] = 0;
			bomb->s.pos.trDelta[2] = 0;
			SnapVector( bomb->s.pos.trDelta );          // save net bandwidth
			VectorCopy( bomb->s.pos.trBase, bomb->r.currentOrigin );

// build arty falling sound effect in front of bomb drop
			bomb2 = G_Spawn();
			bomb2->think = artilleryThink;
			bomb2->s.eType  = ET_MISSILE;
			bomb2->r.svFlags    = SVF_USE_CURRENT_ORIGIN | SVF_NOCLIENT;
			bomb2->r.ownerNum   = ent->s.number;
			bomb2->parent       = ent;
			bomb2->damage       = 0;
			bomb2->nextthink = bomb->nextthink - 600;
			bomb2->classname = "air strike";
			bomb2->clipmask = MASK_MISSILESHOT;
			bomb2->s.pos.trType = TR_STATIONARY; // was TR_GRAVITY,  might wanna go back to this and drop from height
			bomb2->s.pos.trTime = level.time;       // move a bit on the very first frame
			VectorCopy( bomb->s.pos.trBase,bomb2->s.pos.trBase );
			VectorCopy( bomb->s.pos.trDelta,bomb2->s.pos.trDelta );
			VectorCopy( bomb->s.pos.trBase,bomb2->r.currentOrigin );
		}
		ent->client->ps.classWeaponTime = level.time;
	}

#ifdef OMNIBOT
	Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( WP_ARTY ), 0 );
#endif
}


gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity, int ownerNum );
// jpw

/*
==============
Weapon_Gauntlet
==============
*/
void Weapon_Gauntlet( gentity_t *ent ) {

}

/*
===============
CheckMeleeAttack
    using 'isTest' to return hits to world surfaces
===============
*/
trace_t *CheckMeleeAttack( gentity_t *ent, float dist, qboolean isTest ) {
	static trace_t tr;
	vec3_t end;
	gentity_t   *tent;
	gentity_t   *traceEnt;

	// set aiming directions
	AngleVectors( ent->client->ps.viewangles, forward, right, up );

	CalcMuzzlePoint( ent, WP_GAUNTLET, forward, right, up, muzzleTrace );

	VectorMA( muzzleTrace, dist, forward, end );

	trap_Trace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );
	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return NULL;
	}

	// no contact
	if ( tr.fraction == 1.0f ) {
		return NULL;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	// send blood impact
	if ( traceEnt->takedamage && traceEnt->client ) {
		tent = G_TempEntity( tr.endpos, EV_MISSILE_HIT );
		tent->s.otherEntityNum = traceEnt->s.number;
		tent->s.eventParm = DirToByte( tr.plane.normal );
		tent->s.weapon = ent->s.weapon;
	}

//----(SA)	added
	if ( isTest ) {
		return &tr;
	}
//----(SA)

	if ( !traceEnt->takedamage ) {
		return NULL;
	}

	if ( ent->client->ps.powerups[PW_QUAD] ) {
		G_AddEvent( ent, EV_POWERUP_QUAD, 0 );
		s_quadFactor = g_quadfactor.value;
	} else {
		s_quadFactor = 1;
	}

	return &tr;
}


/*
======================================================================

MACHINEGUN

======================================================================
*/

/*
======================
SnapVectorTowards

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating
into a wall.
======================
*/

// (SA) modified so it doesn't have trouble with negative locations (quadrant problems)
//			(this was causing some problems with bullet marks appearing since snapping
//			too far off the target surface causes the the distance between the transmitted impact
//			point and the actual hit surface larger than the mark radius.  (so nothing shows) )

void SnapVectorTowards( vec3_t v, vec3_t to ) {
	int i;

	for ( i = 0 ; i < 3 ; i++ ) {
		if ( to[i] <= v[i] ) {
//			v[i] = (int)v[i];
			v[i] = floor( v[i] );
		} else {
//			v[i] = (int)v[i] + 1;
			v[i] = ceil( v[i] );
		}
	}
}



// JPW
// mechanism allows different weapon damage for single/multiplayer; we want "balanced" weapons
// in multiplayer but don't want to alter the existing single-player damage items that have already
// been changed
//
// KLUDGE/FIXME: also modded #defines below to become macros that call this fn for minimal impact elsewhere
//
int G_GetWeaponDamage( int weapon ) {
	int mauserDmg;
	if ( g_instaGib.integer ) {
		mauserDmg = 999;
	} else {
		mauserDmg = 80;
	}

	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		switch ( weapon ) {
		case WP_LUGER:
		case WP_SILENCER: return 6;
		case WP_COLT: return 8;
		case WP_AKIMBO: return 8;       //----(SA)	added
		case WP_VENOM_FULL:
		case WP_VENOM: return 12;       // 15  ----(SA)	slight modify for DM
		case WP_MP40: return 6;
		case WP_THOMPSON: return 8;
		case WP_STEN: return 10;
		case WP_FG42SCOPE:
		case WP_FG42: return 15;
		case WP_BAR:
		case WP_BAR2: return 12;        //----(SA)	added
		case WP_MAUSER: return 20;
		case WP_GARAND: return 25;
		case WP_SNIPERRIFLE: return 55;
		case WP_SNOOPERSCOPE: return 25;
		case WP_NONE: return 0;
		case WP_KNIFE:
		case WP_KNIFE2: return 5;
		case WP_GRENADE_LAUNCHER: return 100;
		case WP_GRENADE_PINEAPPLE: return 80;
		case WP_DYNAMITE:
		case WP_DYNAMITE2: return 300;
		case WP_ROCKET_LAUNCHER:
		case WP_PANZERFAUST: return 100;
		case WP_MORTAR: return 100;
		case WP_FLAMETHROWER:     // FIXME -- not used in single player yet
		case WP_TESLA:
		case WP_SPEARGUN:
		case WP_SPEARGUN_CO2:
		//	case WP_CROSS:
		case WP_GAUNTLET:
		case WP_SNIPER:
		default:    return 1;
		}
	} else { // multiplayer damage
		if ( ( !g_theTrenchesMode.integer ) && ( !g_sniperWar.integer ) ) {
			switch ( weapon ) {
			case WP_LUGER:
			case WP_SILENCER:
			case WP_COLT: return 18;
			case WP_AKIMBO: return 18;  //----(SA)	added
			case WP_VENOM_FULL: return 10;
			case WP_VENOM: return 20;
			case WP_MP40: return 14;
			case WP_THOMPSON: return 18;
			case WP_STEN: return 14;
			case WP_FG42SCOPE:
			case WP_FG42: return 15;
			case WP_BAR:
			case WP_BAR2: return 12;    //----(SA)	added
			case WP_MAUSER: return mauserDmg;
			case WP_GARAND: return 75; // was 25 JPW
			case WP_SNIPERRIFLE: return 80;
			case WP_SNOOPERSCOPE: return 75;
			case WP_NONE: return 0;
			case WP_KNIFE:
			case WP_KNIFE2: return 10;
			case WP_SMOKE_GRENADE: return 140; // just enough to kill somebody standing on it
			case WP_GRENADE_LAUNCHER: return 250;
			case WP_GRENADE_PINEAPPLE: return 250;
			case WP_DYNAMITE:
			case WP_DYNAMITE2: return 600;
			case WP_ROCKET_LAUNCHER:
			case WP_PANZERFAUST: return 400;
			case WP_MORTAR: return 250;
			case WP_FLAMETHROWER: return 1;
			case WP_TESLA:
			case WP_SPEARGUN:
			case WP_SPEARGUN_CO2:
			//	case WP_CROSS:
			case WP_GAUNTLET:
			case WP_SNIPER:
			default:    return 1;
			}
		}
		//Sniper weapon damage
		else if ( g_sniperWar.integer ) {
			switch ( weapon ) {
			case WP_LUGER:
			case WP_SILENCER:
			case WP_COLT: return 18;
			case WP_AKIMBO: return 18;  //----(SA)	added
			case WP_VENOM_FULL: return 10;
			case WP_VENOM: return 20;
			case WP_MP40: return 14;
			case WP_THOMPSON: return 18;
			case WP_STEN: return 14;
			case WP_FG42SCOPE:
			case WP_FG42: return 15;
			case WP_BAR:
			case WP_BAR2: return 12;    //----(SA)	added
			case WP_MAUSER: return 50;
			case WP_GARAND: return 50; // was 25 JPW
			case WP_SNIPERRIFLE: return 30; // martin - 9/22/10 - increased to 30 to balance out fg42's fast rate of fire.  Best solution with sniperrifle is still to get a headshot
			case WP_SNOOPERSCOPE: return 25;
			case WP_NONE: return 0;
			case WP_KNIFE: return 25;
			case WP_KNIFE2: return 999;
			case WP_SMOKE_GRENADE: return 0; // martin - no damage
			case WP_GRENADE_LAUNCHER: return 250;
			case WP_GRENADE_PINEAPPLE: return 250;
			case WP_DYNAMITE:
			case WP_DYNAMITE2: return 600;
			case WP_ROCKET_LAUNCHER:
			case WP_PANZERFAUST: return 400;
			case WP_MORTAR: return 250;
			case WP_FLAMETHROWER: return 1;
			case WP_TESLA:
			case WP_SPEARGUN:
			case WP_SPEARGUN_CO2:
			//	case WP_CROSS:
			case WP_GAUNTLET:
			case WP_SNIPER:
			default:    return 1;
			}
		} else {
			//Fun Mode damage
			if ( g_theTrenchesMode.integer == 2 ) {
				switch ( weapon ) {
				case WP_LUGER: return 5;
				case WP_SILENCER:
				case WP_COLT: return 5;
				case WP_AKIMBO: return 1;       //----(SA)	added
				case WP_VENOM_FULL: return 10;
				case WP_VENOM: return 10;
				case WP_MP40: return 10;
				case WP_THOMPSON: return 10;
				case WP_STEN: return 999;
				case WP_FG42SCOPE:
				case WP_FG42: return 15;
				case WP_BAR:
				case WP_BAR2: return 12;        //----(SA)	added
				case WP_MAUSER: return 17;     //g_mauserdamage.integer; // was 25 JPW
				case WP_GARAND: return 49;     // was 25 JPW
				case WP_SNIPERRIFLE: return 50;
				case WP_SNOOPERSCOPE: return 75;
				case WP_NONE: return 0;
				case WP_KNIFE:
				case WP_KNIFE2: return 999;
				case WP_SMOKE_GRENADE: return 250;     // just enough to kill somebody standing on it
				case WP_GRENADE_LAUNCHER: return 250;
				case WP_GRENADE_PINEAPPLE: return 250;
				case WP_DYNAMITE:
				case WP_DYNAMITE2: return 600;
				case WP_ROCKET_LAUNCHER:
				case WP_PANZERFAUST: return 100;
				case WP_MORTAR: return 0;
				case WP_FLAMETHROWER: return 1;
				case WP_TESLA:
				case WP_SPEARGUN:
				case WP_SPEARGUN_CO2:
				//	case WP_CROSS:
				case WP_GAUNTLET:
				case WP_SNIPER:
				default:    return 1;
				}
			} else if ( g_theTrenchesMode.integer == 1 )      {
				switch ( weapon ) {
				case WP_LUGER: return 12;
				case WP_SILENCER:
				case WP_COLT: return 12;
				case WP_AKIMBO: return 1;       //----(SA)	added
				case WP_VENOM_FULL: return 10;
				case WP_VENOM: return 10;
				case WP_MP40: return 14;
				case WP_THOMPSON: return 18;
				case WP_STEN: return 999;
				case WP_FG42SCOPE:
				case WP_FG42: return 15;
				case WP_BAR:
				case WP_BAR2: return 12;        //----(SA)	added
				case WP_MAUSER: return 49;     //g_mauserdamage.integer; // was 25 JPW
				case WP_GARAND: return 49;     // was 25 JPW
				case WP_SNIPERRIFLE: return 50;
				case WP_SNOOPERSCOPE: return 75;
				case WP_NONE: return 0;
				case WP_KNIFE:
				case WP_KNIFE2: return 10;
				case WP_SMOKE_GRENADE: return 250;     // just enough to kill somebody standing on it
				case WP_GRENADE_LAUNCHER: return 250;
				case WP_GRENADE_PINEAPPLE: return 250;
				case WP_DYNAMITE:
				case WP_DYNAMITE2: return 600;
				case WP_ROCKET_LAUNCHER:
				case WP_PANZERFAUST: return 400;
				case WP_MORTAR: return 400;
				case WP_FLAMETHROWER: return .5;
				case WP_TESLA:
				case WP_SPEARGUN:
				case WP_SPEARGUN_CO2:
				//	case WP_CROSS:
				case WP_GAUNTLET:
				case WP_SNIPER:
				default:    return 1;
				}
			}
		}
	}
	return 0;
}
// JPW - this chunk appears to not be used, right?
/*
#define MACHINEGUN_SPREAD	200
#define	MACHINEGUN_DAMAGE	G_GetWeaponDamage(WP_MACHINEGUN) // JPW
#define	MACHINEGUN_TEAM_DAMAGE	G_GetWeaponDamage(WP_MACHINEGUN) // JPW		// wimpier MG in teamplay
*/
// jpw

// RF, wrote this so we can dynamically switch between old and new values while testing g_userAim
float G_GetWeaponSpread( int weapon ) {
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {   // JPW NERVE -- don't affect SP game
		if ( g_userAim.integer ) {
			// these should be higher since they become erratic if aiming is out
			switch ( weapon ) {
			case WP_LUGER: return 600;
			case WP_SILENCER: return 900;
			case WP_COLT: return 700;
			case WP_AKIMBO: return 700; //----(SA)	added
			case WP_VENOM: return 1000;
			case WP_MP40: return 1000;
			case WP_FG42SCOPE:
			case WP_FG42:   return 800;
			case WP_BAR:
			case WP_BAR2:   return 800;
			case WP_THOMPSON: return 1200;
			case WP_STEN: return 1200;
			case WP_MAUSER: return 400;
			case WP_GARAND: return 500;
			case WP_SNIPERRIFLE: return 300;
			case WP_SNOOPERSCOPE: return 300;
			}
		} else {    // old values
			switch ( weapon ) {
			case WP_LUGER: return 25;
			case WP_SILENCER: return 150;
			case WP_COLT: return 30;
			case WP_AKIMBO: return 30;      //----(SA)	added
			case WP_VENOM: return 200;
			case WP_MP40: return 200;
			case WP_FG42SCOPE:
			case WP_FG42:   return 150;
			case WP_BAR:
			case WP_BAR2:   return 150;
			case WP_THOMPSON: return 250;
			case WP_STEN: return 300;
			case WP_MAUSER: return 15;
			case WP_GARAND: return 25;
			case WP_SNIPERRIFLE: return 10;
			case WP_SNOOPERSCOPE: return 10;
			}
		}
	} else { // JPW NERVE but in multiplayer...  new spreads and don't look at g_userAim
		if ( !g_theTrenchesMode.integer ) {
			switch ( weapon ) {
			case WP_LUGER: return 600;
			case WP_SILENCER: return 900;
			case WP_COLT: return 800;
			case WP_AKIMBO: return 800;     //----(SA)added
			case WP_VENOM: return 600;
			case WP_MP40: return 400;
			case WP_FG42SCOPE: return 200;
			case WP_FG42:   return 500;
			case WP_BAR:
			case WP_BAR2:   return 500;
			case WP_THOMPSON: return 600;
			case WP_STEN: return 200;
			case WP_MAUSER:
				if ( g_instaGib.integer ) {
					return 100;
				} else {
					return 2000;
				}
				break;
			case WP_GARAND: return 600;
			case WP_SNIPERRIFLE: return 700;     // was 300
			case WP_SNOOPERSCOPE: return 700;
			}
		} else {
			//FOR THE TRENCHES
			//I just split the funmode with g_thetrenchesmode for now may change later
			if ( g_theTrenchesMode.integer == 2 ) {
				switch ( weapon ) {
				case WP_LUGER: return 700;
				case WP_SILENCER: return 900;
				case WP_COLT: return 700;
				case WP_AKIMBO: return 800; //----(SA)added
				case WP_VENOM: return 600;
				case WP_MP40: return 600;
				case WP_FG42SCOPE:
				case WP_FG42:   return 500;
				case WP_BAR:
				case WP_BAR2:   return 500;
				case WP_THOMPSON: return 600;
				case WP_STEN: return 1;
				case WP_MAUSER: return 900; //our 12 gauge
				case WP_GARAND: return 150;
				case WP_SNIPERRIFLE: return 150; // was 300
				case WP_SNOOPERSCOPE: return 700;
				}
			} else if ( g_theTrenchesMode.integer == 1 )    {
				switch ( weapon ) {
				case WP_LUGER: return 700;
				case WP_SILENCER: return 900;
				case WP_COLT: return 700;
				case WP_AKIMBO: return 800; //----(SA)added
				case WP_VENOM: return 600;
				case WP_MP40: return 400;
				case WP_FG42SCOPE: return 300;
				case WP_FG42:   return 500;
				case WP_BAR:
				case WP_BAR2:   return 500;
				case WP_THOMPSON: return 600;
				case WP_STEN: return 1;
				case WP_MAUSER: return 100;
				case WP_GARAND: return 150;
				case WP_SNIPERRIFLE: return 150; // was 300
				case WP_SNOOPERSCOPE: return 700;
				}
			}
		}
	}
	G_Printf( "shouldn't ever get here (weapon %d)\n",weapon );
	// jpw
	return 0;   // shouldn't get here
}

#define LUGER_SPREAD    G_GetWeaponSpread( WP_LUGER )
#define LUGER_DAMAGE    G_GetWeaponDamage( WP_LUGER ) // JPW
#define SILENCER_SPREAD G_GetWeaponSpread( WP_SILENCER )
#define COLT_SPREAD     G_GetWeaponSpread( WP_COLT )
#define COLT_DAMAGE     G_GetWeaponDamage( WP_COLT ) // JPW

#define VENOM_SPREAD    G_GetWeaponSpread( WP_VENOM )
#define VENOM_DAMAGE    G_GetWeaponDamage( WP_VENOM ) // JPW

#define MP40_SPREAD     G_GetWeaponSpread( WP_MP40 )
#define MP40_DAMAGE     G_GetWeaponDamage( WP_MP40 ) // JPW
#define THOMPSON_SPREAD G_GetWeaponSpread( WP_THOMPSON )
#define THOMPSON_DAMAGE G_GetWeaponDamage( WP_THOMPSON ) // JPW
#define STEN_SPREAD     G_GetWeaponSpread( WP_STEN )
#define STEN_DAMAGE     G_GetWeaponDamage( WP_STEN ) // JPW
#define FG42_SPREAD     G_GetWeaponSpread( WP_FG42 )
#define FG42_DAMAGE     G_GetWeaponDamage( WP_FG42 ) // JPW

#define FG42SCOPE_SPREAD        G_GetWeaponSpread( WP_FG42SCOPE )

#define BAR_SPREAD      G_GetWeaponSpread( WP_BAR )
#define BAR_DAMAGE      G_GetWeaponDamage( WP_BAR )

#define MAUSER_SPREAD   G_GetWeaponSpread( WP_MAUSER )
#define MAUSER_DAMAGE   G_GetWeaponDamage( WP_MAUSER ) // JPW
#define GARAND_SPREAD   G_GetWeaponSpread( WP_GARAND )
#define GARAND_DAMAGE   G_GetWeaponDamage( WP_GARAND ) // JPW

#define SNIPER_SPREAD   G_GetWeaponSpread( WP_SNIPERRIFLE )
#define SNIPER_DAMAGE   G_GetWeaponDamage( WP_SNIPERRIFLE ) // JPW

#define SNOOPER_SPREAD  G_GetWeaponSpread( WP_SNOOPERSCOPE )
#define SNOOPER_DAMAGE  G_GetWeaponDamage( WP_SNOOPERSCOPE ) // JPW

/*
==============
SP5_Fire

  dead code
==============
*/
void SP5_Fire( gentity_t *ent, float aimSpreadScale ) {
	// TTimo unused
//	static int	seed = 0x92;

	float spread = 400;         // these used to be passed in
	int damage;

	trace_t tr;
	vec3_t end;
	float r;
	float u;
	gentity_t       *tent;
	gentity_t       *traceEnt;

/*
    // first do a very short, high-accuracy trace
    VectorMA (muzzleTrace, 128, forward, end);
    trap_Trace (&tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);
    // then if that fails do a longer wild shot
    if ( tr.fraction == 1 )	// didn't hit anything
    {
        {
            vec3_t	vec;
            float	len;

            VectorMA (muzzleTrace, 4096, forward, end);
            trap_Trace (&tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT);
            VectorSubtract (muzzleTrace, tr.endpos, vec);
            len = VectorLength (vec);

            if (len > 400)
                spread = 400;
            else
                spread = len;

            VectorClear (end);
        }
*/
	spread *= aimSpreadScale;

	r = crandom() * spread;
	u = crandom() * spread;
	VectorMA( muzzleTrace, 4096, forward, end );
	VectorMA( end, r, right, end );
	VectorMA( end, u, up, end );

	trap_Trace( &tr, muzzleTrace, NULL, NULL, end, ent->s.number, MASK_SHOT );
	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}
//	}

	traceEnt = &g_entities[ tr.entityNum ];

	// snap the endpos to integers, but nudged towards the line
	SnapVectorTowards( tr.endpos, muzzleTrace );

	// send bullet impact
	if ( traceEnt->takedamage && traceEnt->client && !( traceEnt->flags & ( FL_DEFENSE_GUARD | FL_WARZOMBIECHARGE ) ) ) {
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;
		if ( LogAccuracyHit( traceEnt, ent ) ) {
			ent->client->ps.persistant[PERS_ACCURACY_HITS]++;
		}
	} else if ( ( traceEnt->flags & FL_WARZOMBIECHARGE ) && ( rand() % 3 ) == 0 ) {   // hit every other bullet when charging
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;
		if ( LogAccuracyHit( traceEnt, ent ) ) {
			ent->client->ps.persistant[PERS_ACCURACY_HITS]++;
		}
	} else {
		// Ridah, bullet impact should reflect off surface
		vec3_t reflect;
		float dot;

		if ( traceEnt->flags & ( FL_DEFENSE_GUARD | FL_WARZOMBIECHARGE ) ) {
			// reflect off sheild
			VectorSubtract( tr.endpos, traceEnt->r.currentOrigin, reflect );
			VectorNormalize( reflect );
			VectorMA( traceEnt->r.currentOrigin, 15, reflect, reflect );
			tent = G_TempEntity( reflect, EV_BULLET_HIT_WALL );
		} else {
			tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_WALL );
		}

		dot = DotProduct( forward, tr.plane.normal );
		VectorMA( forward, -2 * dot, tr.plane.normal, reflect );
		VectorNormalize( reflect );

		tent->s.eventParm = DirToByte( reflect );
		// done.
	}
	tent->s.otherEntityNum = ent->s.number;

	if ( traceEnt->takedamage ) {
		qboolean reflectBool = qfalse;
		vec3_t trDir;

		if ( traceEnt->flags & FL_DEFENSE_GUARD ) {
			// if we are facing the direction the bullet came from, then reflect it
			AngleVectors( traceEnt->s.apos.trBase, trDir, NULL, NULL );
			if ( DotProduct( forward, trDir ) < 0.6 ) {
				reflectBool = qtrue;
			}
		}

		//----(SA)	moved these up so damage sent in Bullet_Fire() will be valid
		damage = G_GetWeaponDamage( WP_SILENCER ) + ( random() * 15 );  // JPW giving 40-55
		damage *= s_quadFactor;

		if ( reflectBool ) {
			// reflect this bullet
			G_AddEvent( traceEnt, EV_GENERAL_SOUND, level.bulletRicochetSound );
			CalcMuzzlePoints( traceEnt, traceEnt->s.weapon );
			Bullet_Fire( traceEnt, 1000, damage );
		} else {
			// Ridah, don't hurt team-mates
			// DHM - Nerve :: only in single player

			G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, 0, MOD_SILENCER );
		}
	}
}


void RubbleFlagCheck( gentity_t *ent, trace_t tr ) {
	qboolean is_valid = qfalse;
	int type = 0;

	// (SA) moving client-side

	return;




	if ( tr.surfaceFlags & SURF_RUBBLE || tr.surfaceFlags & SURF_GRAVEL ) {
		is_valid = qtrue;
		type = 4;
	} else if ( tr.surfaceFlags & SURF_METAL )     {
//----(SA)	removed
//		is_valid = qtrue;
//		type = 2;
	} else if ( tr.surfaceFlags & SURF_WOOD )     {
		is_valid = qtrue;
		type = 1;
	}

	if ( is_valid && ent->client && ( ent->s.weapon == WP_VENOM
									  || ent->client->ps.persistant[PERS_HWEAPON_USE] ) ) {
		if ( rand() % 100 > 75 ) {
			gentity_t   *sfx;
			vec3_t start;
			vec3_t dir;

			sfx = G_Spawn();

			sfx->s.density = type;

			VectorCopy( tr.endpos, start );

			VectorCopy( muzzleTrace, dir );
			VectorNegate( dir, dir );

			G_SetOrigin( sfx, start );
			G_SetAngle( sfx, dir );

			G_AddEvent( sfx, EV_SHARD, DirToByte( dir ) );

			sfx->think = G_FreeEntity;
			sfx->nextthink = level.time + 1000;

			sfx->s.frame = 3 + ( rand() % 3 ) ;

			trap_LinkEntity( sfx );

		}
	}

}

/*
==============
EmitterCheck
    see if a new particle emitter should be created at the bullet impact point
==============
*/
void EmitterCheck( gentity_t *ent, gentity_t *attacker, trace_t *tr ) {
	gentity_t *tent;
	vec3_t origin;

	VectorCopy( tr->endpos, origin );
	SnapVectorTowards( tr->endpos, attacker->s.origin );

	if ( Q_stricmp( ent->classname, "func_explosive" ) == 0 ) {
	} else if ( Q_stricmp( ent->classname, "func_leaky" ) == 0 ) {


		tent = G_TempEntity( origin, EV_EMITTER );
		VectorCopy( origin, tent->s.origin );
		tent->s.time = 1234;
		tent->s.density = 9876;
		VectorCopy( tr->plane.normal, tent->s.origin2 );

	}
}


void SniperSoundEFX( vec3_t pos ) {
	gentity_t *sniperEnt;
	sniperEnt = G_TempEntity( pos, EV_SNIPER_SOUND );
}


/*
==============
Bullet_Endpos
    find target end position for bullet trace based on entities weapon and accuracy
==============
*/
void Bullet_Endpos( gentity_t *ent, float spread, vec3_t *end ) {
	float r, u;
	qboolean randSpread = qtrue;
	int dist = 8192;

	r = crandom() * spread;
	u = crandom() * spread;

	// Ridah, if this is an AI shooting, apply their accuracy
	if ( ent->r.svFlags & SVF_CASTAI ) {
		float accuracy = 0;
//		accuracy = (1.0 - AICast_GetAccuracy( ent->s.number )) * AICAST_AIM_SPREAD;
		r += crandom() * accuracy;
		u += crandom() * ( accuracy * 1.25 );
	} else {
		if ( ent->s.weapon == WP_SNOOPERSCOPE || ent->s.weapon == WP_SNIPERRIFLE ) {
			// aim dir already accounted for sway of scoped weapons in CalcMuzzlePoints()
			dist *= 2;
			randSpread = qfalse;
		}
	}

	VectorMA( muzzleTrace, dist, forward, *end );

	if ( randSpread ) {
		VectorMA( *end, r, right, *end );
		VectorMA( *end, u, up, *end );
	}
}

/*
==============
Bullet_Fire
==============
*/
void Bullet_Fire( gentity_t *ent, float spread, int damage ) {
	vec3_t end;
	qboolean shothit = qfalse;
	int savestats[2];
	int i;
	//ANTILAG - shift other clients back to the client's idea of the server
	// time to compensate for lag
	if ( g_antilag.integer && ent->client &&
		 !( ent->r.svFlags & SVF_BOT ) ) {
		G_TimeShiftAllClients( ent->client->pers.cmd.serverTime, ent );
	}

	if ( ent->client->ps.weapon == WP_SHOTGUN ) {
		//save current hit stats here(so you cant get more then one hit per shot)
		savestats[0] = ent->client->pers.acc_hits;
		savestats[1] = ent->client->pers.life_hits;

		for ( i = 0; i < DEFAULT_SHOTGUN_COUNT; i++ )
		{
			Bullet_Endpos( ent, spread, &end );
			if ( Bullet_Fire_Extended( ent, ent, muzzleTrace, end, spread, damage ) && !shothit ) {
				shothit = qtrue;
			}
		}

		//restore saved stats
		if ( shothit ) {
			ent->client->pers.acc_hits = savestats[0] + 1;
			ent->client->pers.life_hits = savestats[1] + 1;
		}


	} else {
		Bullet_Endpos( ent, spread, &end );
		Bullet_Fire_Extended( ent, ent, muzzleTrace, end, spread, damage );
	}

// MHC - Moved to end of FireWeapon per ET
	//ent->client->pers.acc_shots++;
	//ent->client->pers.life_shots++;
}


/*
==============
Bullet_Fire_Extended
    A modified Bullet_Fire with more parameters.
    The original Bullet_Fire still passes through here and functions as it always has.

    uses for this include shooting through entities (windows, doors, other players, etc.) and reflecting bullets
==============
*/
qboolean Bullet_Fire_Extended( gentity_t *source, gentity_t *attacker, vec3_t start, vec3_t end, float spread, int damage ) {
	trace_t tr;
	gentity_t   *tent;
	gentity_t   *traceEnt;
	qboolean hit = qfalse;

	damage *= s_quadFactor;


	// (SA) changed so player could shoot his own dynamite.
	// (SA) whoops, but that broke bullets going through explosives...
//	trap_Trace (&tr, start, NULL, NULL, end, source->s.number, MASK_SHOT);
	trap_Trace( &tr, start, NULL, NULL, end, ENTITYNUM_NONE, MASK_SHOT );
	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		//ANTILAG - make sure we un-time-shift the clients
		goto untimeshift;
	}


	// bullet debugging using Q3A's railtrail
	if ( g_debugBullets.integer & 1 ) {
		tent = G_TempEntity( start, EV_RAILTRAIL );
		VectorCopy( tr.endpos, tent->s.origin2 );
		tent->s.otherEntityNum2 = attacker->s.number;
	}



	RubbleFlagCheck( attacker, tr );

	traceEnt = &g_entities[ tr.entityNum ];

	EmitterCheck( traceEnt, attacker, &tr );

	// snap the endpos to integers, but nudged towards the line
	SnapVectorTowards( tr.endpos, start );


	// send bullet impact
	if ( traceEnt->takedamage && traceEnt->client && !( traceEnt->flags & FL_DEFENSE_GUARD ) ) {
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;
		// MHC - See bottom of G_Damage for new method
		/*if( LogAccuracyHit( traceEnt, attacker ) ) {
		    attacker->client->ps.persistant[PERS_ACCURACY_HITS]++;
		    if(g_deathmatch.integer != 2){
		        if((OnSameTeam(attacker,traceEnt)) && (attacker != traceEnt)){
		            attacker->client->pers.acc_teamHits++;
		        }else{
		            attacker->client->pers.acc_hits++;
		            attacker->client->pers.life_hits++;
		        }
		    }
		}
		if(g_deathmatch.integer == 2){
		    attacker->client->pers.acc_hits++;
		    attacker->client->pers.life_hits++;
		}*/
//----(SA)	added
		if ( g_debugBullets.integer >= 2 ) {   // show hit player bb
			gentity_t *bboxEnt;
			vec3_t b1, b2;
			VectorCopy( traceEnt->r.currentOrigin, b1 );
			VectorCopy( traceEnt->r.currentOrigin, b2 );
			VectorAdd( b1, traceEnt->r.mins, b1 );
			VectorAdd( b2, traceEnt->r.maxs, b2 );
			bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, bboxEnt->s.origin2 );
			bboxEnt->s.dmgFlags = 1;    // ("type")
		}
//----(SA)	end

	} else if ( traceEnt->takedamage && traceEnt->s.eType == ET_BAT ) {
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;
	} else {
		// Ridah, bullet impact should reflect off surface
		vec3_t reflect;
		float dot;

		if ( g_debugBullets.integer <= -2 ) {  // show hit thing bb
			gentity_t *bboxEnt;
			vec3_t b1, b2;
			VectorCopy( traceEnt->r.currentOrigin, b1 );
			VectorCopy( traceEnt->r.currentOrigin, b2 );
			VectorAdd( b1, traceEnt->r.mins, b1 );
			VectorAdd( b2, traceEnt->r.maxs, b2 );
			bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, bboxEnt->s.origin2 );
			bboxEnt->s.dmgFlags = 1;    // ("type")
		}

		if ( traceEnt->flags & FL_DEFENSE_GUARD ) {
			// reflect off sheild
			VectorSubtract( tr.endpos, traceEnt->r.currentOrigin, reflect );
			VectorNormalize( reflect );
			VectorMA( traceEnt->r.currentOrigin, 15, reflect, reflect );
			tent = G_TempEntity( reflect, EV_BULLET_HIT_WALL );
		} else {
			tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_WALL );
		}

		dot = DotProduct( forward, tr.plane.normal );
		VectorMA( forward, -2 * dot, tr.plane.normal, reflect );
		VectorNormalize( reflect );

		tent->s.eventParm = DirToByte( reflect );

		if ( traceEnt->flags & FL_DEFENSE_GUARD ) {
			tent->s.otherEntityNum2 = traceEnt->s.number;   // force sparks
		} else {
			tent->s.otherEntityNum2 = ENTITYNUM_NONE;
		}
		// done.
	}
	tent->s.otherEntityNum = attacker->s.number;

	if ( traceEnt->takedamage ) {
		qboolean reflectBool = qfalse;
		vec3_t trDir;

		if ( traceEnt->flags & FL_DEFENSE_GUARD ) {
			// if we are facing the direction the bullet came from, then reflect it
			AngleVectors( traceEnt->s.apos.trBase, trDir, NULL, NULL );
			if ( DotProduct( forward, trDir ) < 0.6 ) {
				reflectBool = qtrue;
			}
		}

		if ( reflectBool ) {
			vec3_t reflect_end;
			// reflect this bullet
			G_AddEvent( traceEnt, EV_GENERAL_SOUND, level.bulletRicochetSound );
			CalcMuzzlePoints( traceEnt, traceEnt->s.weapon );

//----(SA)	modified to use extended version so attacker would pass through
//			Bullet_Fire( traceEnt, 1000, damage );
			Bullet_Endpos( traceEnt, spread, &reflect_end );
			Bullet_Fire_Extended( traceEnt, attacker, muzzleTrace, reflect_end, spread, damage );
//----(SA)	end

		} else {
			// Ridah, don't hurt team-mates
			// DHM - Nerve :: Only in single player
//			if (attacker->client && traceEnt->client && g_gametype.integer == GT_SINGLE_PLAYER && (traceEnt->r.svFlags & SVF_CASTAI) && (attacker->r.svFlags & SVF_CASTAI) && AICast_SameTeam( AICast_GetCastState( attacker->s.number ), traceEnt->s.number )) {
			// AI's don't hurt members of their own team
//				return;
//			}
			// done.

			G_Damage( traceEnt, attacker, attacker, forward, tr.endpos, damage, 0, ammoTable[attacker->s.weapon].mod );
			hit = qtrue;
			// allow bullets to "pass through" func_explosives if they break by taking another simultanious shot
			if ( Q_stricmp( traceEnt->classname, "func_explosive" ) == 0 ) {
				if ( traceEnt->health <= damage ) {
					// start new bullet at position this hit the bmodel and continue to the end position (ignoring shot-through bmodel in next trace)
					// spread = 0 as this is an extension of an already spread shot
					Bullet_Fire_Extended( traceEnt, attacker, tr.endpos, end, 0, damage );
				}
			}

		}
	}
	//ANTILAG - move the clients back to their proper positions
untimeshift:
	if ( g_antilag.integer && attacker->client &&
		 !( attacker->r.svFlags & SVF_BOT ) ) {
		G_UnTimeShiftAllClients( attacker );
	}

	if ( hit ) {
		return qtrue;
	} else {
		return qfalse;
	}
}



/*
======================================================================

GRENADE LAUNCHER

  700 has been the standard direction multiplier in fire_grenade()

======================================================================
*/
extern void G_ExplodeMissilePoisonGas( gentity_t *ent );

gentity_t *weapon_grenadelauncher_fire( gentity_t *ent, int grenType ) {
	gentity_t       *m, *te;     // JPW NERVE
	trace_t tr;
	vec3_t viewpos;
	float upangle = 0, pitch;                                 //      start with level throwing and adjust based on angle
	vec3_t tosspos;
	qboolean underhand = qtrue;

	pitch = ent->s.apos.trBase[0];

	// JPW NERVE -- smoke grenades always overhand
	if ( pitch >= 0 ) {
		forward[2] += 0.5f;
		// Used later in underhand boost
		pitch = 1.3f;
	} else {
		pitch = -pitch;
		pitch = min( pitch, 30 );
		pitch /= 30.f;
		pitch = 1 - pitch;
		forward[2] += ( pitch * 0.5f );

		// Used later in underhand boost
		pitch *= 0.3f;
		pitch += 1.f;
	}

	VectorNormalizeFast( forward );       //       make sure forward is normalized

	upangle = -( ent->s.apos.trBase[0] );       //   this will give between     -90 / 90
	upangle = min( upangle, 50 );
	upangle = max( upangle, -50 );          //  now clamped to    -50 / 50    (don't allow firing straight up/down)
	upangle = upangle / 100.0f;                       //                       -0.5 / 0.5
	upangle += 0.5f;                                            //                            0.0 / 1.0

	if ( upangle < .1 ) {
		upangle = .1;
	}

	// pineapples are not thrown as far as mashers
	if ( grenType == WP_GRENADE_LAUNCHER ) {
		upangle *= 900;
	} else if ( grenType == WP_GRENADE_PINEAPPLE ) {
		upangle *= 900;
	} else if ( grenType == WP_SMOKE_GRENADE ) {
		upangle *= 900;
	} else {    // WP_DYNAMITE
		upangle *= 400;
	}

	VectorCopy( muzzleEffect, tosspos );

	if ( underhand ) {
		// move a little bit more away from the player (so underhand tosses don't get caught on nearby lips)
		VectorMA( muzzleEffect, 8, forward, tosspos );
		tosspos[2] -= 8;                // lower origin for the underhand throw
		upangle *= pitch;
		SnapVector( tosspos );
	}

	VectorScale( forward, upangle, forward );

	// check for valid start spot (so you don't throw through or get stuck in a wall)
	VectorCopy( ent->s.pos.trBase, viewpos );
	viewpos[2] += ent->client->ps.viewheight;

	if ( grenType == WP_DYNAMITE ) {
		trap_Trace( &tr, viewpos, tv( -12.f, -12.f, 0.f ), tv( 12.f, 12.f, 20.f ), tosspos, ent->s.number, MASK_MISSILESHOT );
	} else {
		trap_Trace( &tr, viewpos, tv( -4.f, -4.f, 0.f ), tv( 4.f, 4.f, 6.f ), tosspos, ent->s.number, MASK_MISSILESHOT );
	}

	if ( tr.fraction < 1 ) {    // oops, bad launch spot
		VectorCopy( tr.endpos, tosspos );
		SnapVectorTowards( tosspos, viewpos );
	}

	m = fire_grenade( ent, tosspos, forward, grenType );

	m->damage = 0;      // Ridah, grenade's don't explode on contact
	m->splashDamage *= s_quadFactor;

	// JPW NERVE
	if ( grenType == WP_SMOKE_GRENADE ) {
		if ( ent->client->sess.sessionTeam == TEAM_RED ) {     //store team so we can generate red or blue smoke
			m->s.otherEntityNum2 = 1;
		} else {
			m->s.otherEntityNum2 = 0;
		}
		if ( g_sniperWar.integer ) {
			m->s.eFlags |= EF_SMOKINGBLACK;
			m->nextthink = level.time + 1000;
			m->s.eFlags |= EF_SMOKINGBLACK;
			m->s.loopSound = G_SoundIndex( "sound/world/steam.wav" );
			m->nextthink = level.time + ( ( g_smokeGrenades.integer - 1 ) * 1000 );
			m->think = G_ExplodeMissile;
		} else {
			if ( ent->client->ps.throwSmoke ) {
				m->nextthink = level.time + 1000;
				m->s.eFlags |= EF_SMOKINGBLACK;
				m->s.loopSound = G_SoundIndex( "sound/world/steam.wav" );
				m->nextthink = level.time + ( ( g_smokeGrenades.integer - 1 ) * 1000 );
				m->think = G_ExplodeMissile;
				ent->client->ps.throwSmoke = qfalse;
			} else {
				if ( !g_theTrenchesMode.integer ) {
					m->nextthink = level.time + 4000;
					m->count2 = ent->count3;
					ent->count3 = 0;
					m->think = weapon_callAirStrike;
				} else {    //Trenches mode \/
					m->s.eFlags |= EF_SMOKINGBLACK;        //Always smokes black
					m->nextthink = level.time + 5000;         //m->nextthink = level.time + 3000+(random() *1000) (sandmann)
					m->think = weapon_callAirStrike;         //Martin-maybe want a weapon_Gas?
				}
				te = G_TempEntity( m->s.pos.trBase, EV_GLOBAL_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/multiplayer/airstrike_01.wav" );
				te->r.svFlags |= SVF_BROADCAST | SVF_USE_CURRENT_ORIGIN;
			}
		}
	}
	// jpw

	//----(SA)      adjust for movement of character.  TODO: Probably comment in later, but only for forward/back not strafing
	//VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );    // "real" physics

	// Ridah, return the grenade so we can do some prediction before deciding if we really want to throw it or not
	return m;
}

//----(SA)	modified this entire "venom" section
/*
============================================================================

VENOM GUN TRACING

============================================================================
*/
#define DEFAULT_VENOM_COUNT 10
#define DEFAULT_VENOM_SPREAD 20
#define DEFAULT_VENOM_DAMAGE 15

qboolean VenomPellet( vec3_t start, vec3_t end, gentity_t *ent ) {
	trace_t tr;
	int damage;
	gentity_t       *traceEnt;

	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, MASK_SHOT );
	traceEnt = &g_entities[ tr.entityNum ];

	// send bullet impact
	if (  tr.surfaceFlags & SURF_NOIMPACT ) {
		return qfalse;
	}

	if ( traceEnt->takedamage ) {
		damage = DEFAULT_VENOM_DAMAGE * s_quadFactor;

		G_Damage( traceEnt, ent, ent, forward, tr.endpos, damage, 0, MOD_VENOM );
		if ( LogAccuracyHit( traceEnt, ent ) ) {
			return qtrue;
		}
	}
	return qfalse;
}

// this should match CG_VenomPattern
void VenomPattern( vec3_t origin, vec3_t origin2, int seed, gentity_t *ent ) {
	int i;
	float r, u;
	vec3_t end;
	vec3_t forward, right, up;
	int oldScore;
	qboolean hitClient = qfalse;

	// derive the right and up vectors from the forward vector, because
	// the client won't have any other information
	VectorNormalize2( origin2, forward );
	PerpendicularVector( right, forward );
	CrossProduct( forward, right, up );

	oldScore = ent->client->ps.persistant[PERS_SCORE];

	//NT - shift other clients back to the client's idea of the server
	// ANTILAG time to compensate for lag
	if ( g_antilag.integer && ent->client &&
		 !( ent->r.svFlags & SVF_BOT ) ) {
		G_TimeShiftAllClients( ent->client->pers.cmd.serverTime, ent );
	}

	// generate the "random" spread pattern
	for ( i = 0 ; i < DEFAULT_VENOM_COUNT ; i++ ) {
		r = Q_crandom( &seed ) * DEFAULT_VENOM_SPREAD;
		u = Q_crandom( &seed ) * DEFAULT_VENOM_SPREAD;
		VectorMA( origin, 8192, forward, end );
		VectorMA( end, r, right, end );
		VectorMA( end, u, up, end );
		if ( VenomPellet( origin, end, ent ) && !hitClient ) {
			hitClient = qtrue;
			ent->client->ps.persistant[PERS_ACCURACY_HITS]++;
		}
	}
	//ANTILAG - move the clients back to their proper positions
	if ( g_antilag.integer && ent->client &&
		 !( ent->r.svFlags & SVF_BOT ) ) {
		G_UnTimeShiftAllClients( ent );
	}
}

/*
======================================================================

NAILGUN

======================================================================
*/

void Weapon_Nailgun_Fire( gentity_t *ent ) {
	gentity_t   *m;
	int count;

	for ( count = 0; count < NUM_NAILSHOTS; count++ ) {
//		m = fire_nail (ent, muzzle, forward, right, up );
		m = fire_nail( ent, muzzleEffect, forward, right, up );
		m->damage *= s_quadFactor;
		m->splashDamage *= s_quadFactor;
	}

//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}


/*
======================================================================

PROXIMITY MINE LAUNCHER

======================================================================
*/

void weapon_proxlauncher_fire( gentity_t *ent ) {
	gentity_t   *m;

	// extra vertical velocity
	forward[2] += 0.2f;
	VectorNormalize( forward );

	m = fire_prox( ent, muzzleTrace, forward );
	m->damage *= s_quadFactor;
	m->splashDamage *= s_quadFactor;

//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}

/*
==============
weapon_venom_fire
==============
*/
void weapon_venom_fire( gentity_t *ent, qboolean fullmode, float aimSpreadScale ) {
	gentity_t       *tent;

	if ( fullmode ) {
		tent = G_TempEntity( muzzleTrace, EV_VENOMFULL );
	} else {
		tent = G_TempEntity( muzzleTrace, EV_VENOM );
	}

	VectorScale( forward, 4096, tent->s.origin2 );
	SnapVector( tent->s.origin2 );
	tent->s.eventParm = rand() & 255;       // seed for spread pattern
	tent->s.otherEntityNum = ent->s.number;

	if ( fullmode ) {
		VenomPattern( tent->s.pos.trBase, tent->s.origin2, tent->s.eventParm, ent );
	} else {
		Bullet_Fire( ent, VENOM_SPREAD * aimSpreadScale, VENOM_DAMAGE );
	}
}





/*
======================================================================

ROCKET

======================================================================
*/

void Weapon_RocketLauncher_Fire( gentity_t *ent ) {
	gentity_t   *m;

	m = fire_rocket( ent, muzzleEffect, forward );
	m->damage *= s_quadFactor;
	m->splashDamage *= s_quadFactor;

//	VectorAdd( m->s.pos.trDelta, ent->client->ps.velocity, m->s.pos.trDelta );	// "real" physics
}

/*
======================================================================

SPEARGUN

======================================================================
*/
void Weapon_Speargun_Fire( gentity_t *ent ) {
	gentity_t   *m;

	m = fire_speargun( ent, muzzleEffect, forward );
	m->damage *= s_quadFactor;
}

/*
======================================================================

SHOTGUN

======================================================================
*/

// DEFAULT_SHOTGUN_SPREAD and DEFAULT_SHOTGUN_COUNT	are in bg_public.h, because
// client predicts same spreads
#define DEFAULT_SHOTGUN_DAMAGE  10

qboolean ShotgunPellet( vec3_t start, vec3_t end, gentity_t *ent ) {
	trace_t tr;
	int damage, i, passent;
	gentity_t   *traceEnt;
#ifdef MISSIONPACK
	vec3_t impactpoint, bouncedir;
#endif
	vec3_t tr_start, tr_end;

	passent = ent->s.number;
	VectorCopy( start, tr_start );
	VectorCopy( end, tr_end );
	for ( i = 0; i < 10; i++ ) {
		trap_Trace( &tr, tr_start, NULL, NULL, tr_end, passent, MASK_SHOT );
		traceEnt = &g_entities[ tr.entityNum ];

		// send bullet impact
		if (  tr.surfaceFlags & SURF_NOIMPACT ) {
			return qfalse;
		}

		if ( traceEnt->takedamage ) {
			damage = DEFAULT_SHOTGUN_DAMAGE;
			G_Damage( traceEnt, ent, ent, forward, tr.endpos,   damage, 0, ammoTable[ent->s.weapon].mod );
			if ( LogAccuracyHit( traceEnt, ent ) ) {
				return qtrue;
			}
		}
		return qfalse;
	}
	return qfalse;
}

// this should match CG_ShotgunPattern
void ShotgunPattern( vec3_t origin, vec3_t origin2, int seed, gentity_t *ent ) {
	int i;
	float r, u;
	vec3_t end;
	vec3_t forward, right, up;
	int oldScore;
	qboolean hitClient = qfalse;

	// derive the right and up vectors from the forward vector, because
	// the client won't have any other information
	VectorNormalize2( origin2, forward );
	PerpendicularVector( right, forward );
	CrossProduct( forward, right, up );

	oldScore = ent->client->ps.persistant[PERS_SCORE];

	// generate the "random" spread pattern
	for ( i = 0 ; i < DEFAULT_SHOTGUN_COUNT ; i++ ) {
		r = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
		u = Q_crandom( &seed ) * DEFAULT_SHOTGUN_SPREAD * 16;
		VectorMA( origin, 8192 * 16, forward, end );
		VectorMA( end, r, right, end );
		VectorMA( end, u, up, end );
		if ( ShotgunPellet( origin, end, ent ) && !hitClient ) {
			hitClient = qtrue;
			ent->client->accuracy_hits++;
		}
	}
}





/*
======================================================================

LIGHTNING GUN

======================================================================
*/
// TTimo - extracted G_FlameDamage to unify with Weapon_FlamethrowerFire usage
//	*************************
//	Agent - Flamethrower Fix
//	*************************
void G_BurnMeGood( gentity_t *self, gentity_t *body ) {
	int rnd;
	// add the new damage
	body->flameQuota += 5;
	body->flameQuotaTime = level.time;

	// JPW NERVE -- yet another flamethrower damage model, trying to find a feels-good damage combo that isn't overpowered
	/*	if (body->lastBurnedFrameNumber != level.framenum) {
	        G_Damage(body,self->parent,self->parent,vec3_origin,self->r.currentOrigin,5,0,MOD_FLAMETHROWER); // was 2 dmg in release ver, hit avg. 2.5 times per frame
	        body->lastBurnedFrameNumber = level.framenum;
	    }*/
	// jpw

	// Martin - Fixed Flamethrower
	// 6/20/08 - burns faster, but less dmg :)
	if ( level.time - body->lastBurnedFrameNumber >= 1 ) 
	{
		G_Damage( body, self->parent, self->parent, vec3_origin, self->r.currentOrigin, 5, 0, MOD_FLAMETHROWER );
		body->lastBurnedFrameNumber = level.time;
	}
	// make em burn
	if ( body->client && ( body->health <= 0 || body->flameQuota > 0 ) ) { // JPW NERVE was > FLAME_THRESHOLD
		if ( body->s.onFireEnd < level.time ) 
		{
			body->s.onFireStart = level.time;

			if (body->client->ps.stats[STAT_HEALTH] > 0)
			{
				rnd = rand() % 3 + 1;
				switch(rnd){
					case 1:
						G_AddEvent( body, EV_GENERAL_SOUND, G_SoundIndex( "sound/player/flame_scream1.wav"));
						break;
					case 2:
						G_AddEvent( body, EV_GENERAL_SOUND, G_SoundIndex( "sound/player/flame_scream3.wav"));
						break;
					default:
						G_AddEvent( body, EV_GENERAL_SOUND, G_SoundIndex( "sound/player/flame_scream2.wav"));
						break;
				}
			}
		}

		body->s.onFireEnd = level.time + FIRE_FLASH_TIME;
		body->flameBurnEnt = self->s.number;
		// add to playerState for client-side effect
		body->client->ps.onFireStart = level.time;
	}
}

// those are used in the cg_ traces calls
static vec3_t flameChunkMins = {-4, -4, -4};
static vec3_t flameChunkMaxs = { 4,  4,  4};

#define SQR_SIN_T 0.44 // ~ sqr(sin(20))


// RF, not used anymore for Flamethrower (still need it for tesla?)
void Weapon_FlamethrowerFire( gentity_t *ent ) {
	//Old Flamer Code
	/*gentity_t	*traceEnt;
	vec3_t		start;

	VectorCopy( ent->r.currentOrigin, start );
	start[2] += ent->client->ps.viewheight;

	VectorMA( start, -8, forward, start );
	VectorMA( start, 10, right, start );
	VectorMA( start, -6, up, start );

	traceEnt = fire_flamechunk ( ent, start, forward );*/

	//begin 1.4 code dump
	//Flamethrower Fix
	gentity_t   *traceEnt;
	vec3_t start;
	vec3_t trace_start;
	vec3_t trace_end;
	trace_t trace;

	VectorCopy( ent->r.currentOrigin, start );
	start[2] += ent->client->ps.viewheight;
	VectorCopy( start, trace_start );

	VectorMA( start, -8, forward, start );
	VectorMA( start, 10, right, start );
	VectorMA( start, -6, up, start );

	// prevent flame thrower cheat, run & fire while aiming at the ground, don't get hurt
	// 72 total box height, 18 xy -> 77 trace radius (from view point towards the ground) is enough to cover the area around the feet
	VectorMA( trace_start, 77.0, forward, trace_end );
	trap_Trace( &trace, trace_start, flameChunkMins, flameChunkMaxs, trace_end, ent->s.number, MASK_SHOT | MASK_WATER );
	if ( trace.fraction != 1.0 ) {
		// additional checks to filter out false positives
		if ( trace.endpos[2] > ( ent->r.currentOrigin[2] + ent->r.mins[2] - 8 ) && trace.endpos[2] < ent->r.currentOrigin[2] ) {
			// trigger in a 21 radius around origin
			trace_start[0] -= trace.endpos[0];
			trace_start[1] -= trace.endpos[1];
			if ( trace_start[0] * trace_start[0] + trace_start[1] * trace_start[1] < 441 ) {
				// set self in flames
				G_BurnMeGood( ent, ent );
			}
		}
	}

	traceEnt = fire_flamechunk( ent, start, forward );


}

//======================================================================


/*
==============
AddLean
    add leaning offset
==============
*/
void AddLean( gentity_t *ent, vec3_t point ) {
	if ( ent->client ) {
		if ( ent->client->ps.leanf ) {
			vec3_t right;
			AngleVectors( ent->client->ps.viewangles, NULL, right, NULL );
			VectorMA( point, ent->client->ps.leanf, right, point );
		}
	}
}

/*
===============
LogAccuracyHit
===============
*/
qboolean LogAccuracyHit( gentity_t *target, gentity_t *attacker ) {
	if ( !target->takedamage ) {
		return qfalse;
	}
	if ( target == attacker ) {
		return qfalse;
	}
	if ( !target->client ) {
		return qfalse;
	}
	if ( !attacker->client ) {
		return qfalse;
	}
	// MHC - if we are going to count gibs as 'hits', we should also count them as shots...
/*	if( target->client->ps.stats[STAT_HEALTH] <= 0 ) {
        return qfalse;
    }*/

	if ( OnSameTeam( target, attacker ) ) {
		return qfalse;
	}

	return qtrue;
}


/*
===============
CalcMuzzlePoint

set muzzle location relative to pivoting eye
===============
*/
void CalcMuzzlePoint( gentity_t *ent, int weapon, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint ) {
	VectorCopy( ent->r.currentOrigin, muzzlePoint );
	muzzlePoint[2] += ent->client->ps.viewheight;
	// Ridah, this puts the start point outside the bounding box, isn't necessary
//	VectorMA( muzzlePoint, 14, forward, muzzlePoint );
	// done.

	// Ridah, offset for more realistic firing from actual gun position
	//----(SA) modified
	switch ( weapon )  // Ridah, changed this so I can predict weapons
	{
	case WP_PANZERFAUST:
		if ( g_gametype.integer == GT_SINGLE_PLAYER ) {   // JPW NERVE
			VectorMA( muzzlePoint, 14, right, muzzlePoint );        //----(SA)	new first person rl position
			VectorMA( muzzlePoint, -10, up, muzzlePoint );
		}
// JPW NERVE -- pfaust shoots into walls too much so we moved it to shoulder mount
		else {
			VectorMA( muzzlePoint,10,right,muzzlePoint );
//				VectorMA(muzzlePoint,10,up,muzzlePoint);
		}
// jpw
		break;
	case WP_ROCKET_LAUNCHER:
		// VectorMA( muzzlePoint, 20, right, muzzlePoint );
		// Rafael: note to Sherman had to move this in so that it wouldnt
		// spawn into a wall and detonate

//			VectorMA( muzzlePoint, 16, right, muzzlePoint );
		VectorMA( muzzlePoint, 14, right, muzzlePoint );        //----(SA)	new first person rl position
		break;
	case WP_DYNAMITE:
	case WP_DYNAMITE2:
	case WP_GRENADE_PINEAPPLE:
	case WP_GRENADE_LAUNCHER:
		VectorMA( muzzlePoint, 20, right, muzzlePoint );
		break;
	default:
		VectorMA( muzzlePoint, 6, right, muzzlePoint );
		VectorMA( muzzlePoint, -4, up, muzzlePoint );
		break;
	}

	// done.

	// (SA) actually, this is sort of moot right now since
	// you're not allowed to fire when leaning.  Leave in
	// in case we decide to enable some lean-firing.
	// (SA) works with gl now
	//AddLean(ent, muzzlePoint);

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}

// Rafael - for activate
void CalcMuzzlePointForActivate( gentity_t *ent, vec3_t forward, vec3_t right, vec3_t up, vec3_t muzzlePoint ) {

	VectorCopy( ent->s.pos.trBase, muzzlePoint );
	muzzlePoint[2] += ent->client->ps.viewheight;

	AddLean( ent, muzzlePoint );

	// snap to integer coordinates for more efficient network bandwidth usage
	SnapVector( muzzlePoint );
}
// done.

// Ridah
void CalcMuzzlePoints( gentity_t *ent, int weapon ) {
	vec3_t viewang;

	VectorCopy( ent->client->ps.viewangles, viewang );
#ifdef OMNIBOT
	if ( !( ent->r.svFlags & SVF_CASTAI ) && !( ent->r.svFlags & SVF_BOT ) ) {
#else
	if ( !( ent->r.svFlags & SVF_CASTAI ) ) { // non ai's take into account scoped weapon 'sway' (just another way aimspread is visualized/utilized)
#endif
		float spreadfrac, phase;

		if ( weapon == WP_SNIPERRIFLE || weapon == WP_SNOOPERSCOPE ) {
			spreadfrac = ent->client->currentAimSpreadScale;

			// rotate 'forward' vector by the sway
			phase = level.time / 1000.0 * ZOOM_PITCH_FREQUENCY * M_PI * 2;
			viewang[PITCH] += ZOOM_PITCH_AMPLITUDE * sin( phase ) * ( spreadfrac + ZOOM_PITCH_MIN_AMPLITUDE );

			phase = level.time / 1000.0 * ZOOM_YAW_FREQUENCY * M_PI * 2;
			viewang[YAW] += ZOOM_YAW_AMPLITUDE * sin( phase ) * ( spreadfrac + ZOOM_YAW_MIN_AMPLITUDE );
		}
	}


	// set aiming directions
	AngleVectors( viewang, forward, right, up );

//----(SA)	modified the muzzle stuff so that weapons that need to fire down a perfect trace
//			straight out of the camera (SP5, Mauser right now) can have that accuracy, but
//			weapons that need an offset effect (bazooka/grenade/etc.) can still look like
//			they came out of the weap.
	CalcMuzzlePointForActivate( ent, forward, right, up, muzzleTrace );
	CalcMuzzlePoint( ent, weapon, forward, right, up, muzzleEffect );
}

/*
===============
FireWeapon
===============
*/
void FireWeapon( gentity_t *ent ) {
	float aimSpreadScale;
	vec3_t viewang;  // JPW NERVE
	vec3_t angles;
#ifdef OMNIBOT
	gentity_t *pFiredShot = 0; // tell bots about projectiles
	qboolean callEvent = qtrue; // some are called elsewhere so we can get the projectile
#endif
	// Rafael mg42
	if ( ent->client->ps.persistant[PERS_HWEAPON_USE] && ent->active ) {
		return;
	}

	if ( ent->client->ps.powerups[PW_QUAD] ) {
		s_quadFactor = g_quadfactor.value;
	} else {
		s_quadFactor = 1;
	}


	// Ridah, need to call this for AI prediction also
	CalcMuzzlePoints( ent, ent->s.weapon );

	if ( g_userAim.integer ) {
		aimSpreadScale = ent->client->currentAimSpreadScale;
		// Ridah, add accuracy factor for AI
		if ( ent->aiCharacter ) {
			float aim_accuracy;
			aim_accuracy = 0; // AICast_GetAccuracy(ent->s.number);
			if ( aim_accuracy <= 0 ) {
				aim_accuracy = 0.0001;
			}
			aimSpreadScale = ( 1.0 - aim_accuracy ) * 2.0;
		} else {
			aimSpreadScale += 0.15f; // (SA) just adding a temp /maximum/ accuracy for player (this will be re-visited in greater detail :)
			if ( aimSpreadScale > 1 ) {
				aimSpreadScale = 1.0f;  // still cap at 1.0
			}
		}
	} else {
		aimSpreadScale = 1.0;
	}

// JPW NERVE -- EARLY OUT: if I'm in multiplayer and I have binocs, try to use artillery and then early return b4 switch statement
	if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
		if ( ( ent->client->ps.eFlags & EF_ZOOMING ) && ( ent->client->ps.stats[STAT_KEYS] & ( 1 << INV_BINOCS ) ) &&
			 ( ent->s.weapon != WP_SNIPERRIFLE ) && ( ent->s.weapon != WP_FG42SCOPE ) ) {

			if ( !( ent->client->ps.leanf ) ) {
				if ( g_sniperWar.integer ) {
					SM_Weapon_Artillery( ent );
				} else {
					Weapon_Artillery( ent );

				}
			}
			return;
		}
	}
// jpw

// JPW NERVE -- if jumping, make aim bite ass
	if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
		if ( ent->client->ps.groundEntityNum == ENTITYNUM_NONE ) {
			aimSpreadScale = 2.0f;
		}
	}
// jpw

	// fire the specific weapon
	switch ( ent->s.weapon ) {
	case WP_KNIFE:
	case WP_KNIFE2:
		Weapon_Knife( ent );
		break;
	// NERVE - SMF
	case WP_MEDKIT:
#ifdef OMNIBOT
		callEvent = qfalse;
#endif
		Weapon_Medic( ent );
		break;
	case WP_PLIERS:
		Weapon_Engineer( ent );
		break;
	case WP_SMOKE_GRENADE:
		if ( g_smokeGrenades.integer && ent->client->ps.throwSmoke ) {
			if ( level.time - ent->client->ps.classWeaponTime >= g_LTChargeTime.integer * 0.25f ) {
				if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {
					ent->client->ps.classWeaponTime = level.time - g_LTChargeTime.integer;
				}
				ent->client->ps.classWeaponTime += g_LTChargeTime.integer * 0.25;
#ifdef OMNIBOT
				pFiredShot = weapon_grenadelauncher_fire( ent,WP_SMOKE_GRENADE );
#else
				weapon_grenadelauncher_fire( ent, WP_SMOKE_GRENADE );
#endif
			}
			break;
		}
		if ( level.time - ent->client->ps.classWeaponTime >= g_LTChargeTime.integer * 0.5f ) {
			// AS limit of 1 means 6 / min
			//			   2 means 4 / min... greatly reduces spam i hope :)
			// AS limit of 3 means 3 / min...
			if ( g_maxAirstrikes.integer >= 0 ) {
				if ( isAirstrikeAvailable( ent ) ) {
					//Airstrike is available
					if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {
						ent->client->ps.classWeaponTime = level.time - g_LTChargeTime.integer;
						if ( g_ltExtraAirstrike.integer ) {
							ent->count3 = 1;
						}
						// Add stuff so that we actually increment bomb counter :)
						if ( ent->client->sess.sessionTeam == TEAM_RED ) {
							level.axisAirstrikeCounter += ( 60000 / g_maxAirstrikes.integer );
						}
						if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
							level.alliedAirstrikeCounter += ( 60000 / g_maxAirstrikes.integer );
						}
					}
					ent->client->ps.classWeaponTime = level.time; //+= g_LTChargeTime.integer*0.5f; FIXME later
					weapon_grenadelauncher_fire( ent,WP_SMOKE_GRENADE );
					break;
					// Airstrike is not available
				} else {
					int cost = 60000 / g_maxAirstrikes.integer;
					if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
						int airAvailable = ( ( level.alliedAirstrikeCounter + cost ) - 60000 ) / 1000;
						trap_SendServerCommand( ent - g_entities, va( "chat \"HQ: ^3All available planes are already en-route. Plane available in %i seconds\"", airAvailable ) );
						break;
					}
					if ( ent->client->sess.sessionTeam == TEAM_RED ) {
						int airAvailable = ( ( level.axisAirstrikeCounter + cost ) - 60000 ) / 1000;
						trap_SendServerCommand( ent - g_entities, va( "chat \"HQ: ^3All available planes are already en-route. Plane available in %i seconds\"", airAvailable ) );
						break;
					}
				}
			} else { // Limited Airstrikes off, let them have the support
				if ( level.time - ent->client->ps.classWeaponTime > g_LTChargeTime.integer ) {
					ent->client->ps.classWeaponTime = level.time - g_LTChargeTime.integer;
					if ( g_ltExtraAirstrike.integer ) {
						ent->count3 = 1;
					}
				}
				ent->client->ps.classWeaponTime = level.time; //+= g_LTChargeTime.integer*0.5f; FIXME later
#ifdef OMNIBOT
				pFiredShot = weapon_grenadelauncher_fire( ent,WP_SMOKE_GRENADE );
#else
				weapon_grenadelauncher_fire( ent, WP_SMOKE_GRENADE );
#endif
				break;
			}
		}
		break;
	// -NERVE - SMF
	case WP_ARTY:
		G_Printf( "calling artilery\n" );
		break;
	case WP_MEDIC_SYRINGE:
		Weapon_Syringe( ent );
		break;
	case WP_AMMO:
#ifdef OMNIBOT
		callEvent = qfalse;
#endif
		Weapon_MagicAmmo( ent );
		break;
// jpw
	case WP_LUGER:
		Bullet_Fire( ent, LUGER_SPREAD * aimSpreadScale, LUGER_DAMAGE );
		break;
	case WP_COLT:
		Bullet_Fire( ent, COLT_SPREAD * aimSpreadScale, COLT_DAMAGE );
		break;
	case WP_VENOM:
		weapon_venom_fire( ent, qfalse, aimSpreadScale );
		break;
	case WP_SNIPERRIFLE:
		Bullet_Fire( ent, SNIPER_SPREAD * aimSpreadScale, SNIPER_DAMAGE );
// JPW NERVE -- added muzzle flip in multiplayer
		if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
			//if ( !g_autoSniper.integer ) {
				/*VectorCopy(ent->client->ps.viewangles,viewang);
				//		viewang[PITCH] -= 6; // handled in clientthink instead			//Agent - commented out by nerve
				ent->client->sniperRifleMuzzleYaw = crandom()*0.5; // used in clientthink
				ent->client->sniperRifleFiredTime = level.time;
				SetClientViewAngle(ent,viewang);
				*/                                                                                                                                                                                                                                                                                                                                                                   //Agent - Lets try to fuck up clients who are cheating >:(
				/*		int RndYaw =  rand()%5 +1;	//1,2,3,4,5,6
				        if (RndYaw < 4)
				            RndYaw = RndYaw * -2;
				        angles[YAW] = ent->client->ps.viewangles[1]-RndYaw;
				        angles[PITCH] = ent->client->ps.viewangles[0]-15;	//Agent - This isnt too bad and cant be exploited (if you got low fps its just laggy, same push up :D
				        //angles[PITCH] = ent->client->ps.viewangles[0]* crandom()*0.5;	//Agent - ROFL if you are looking straight down... kicks you to the moon
				        //angles[ROLL] = ent->client->ps.viewangles[0];		//They never used this for original kickback :)
				        angles[ROLL] = 0;*/
				int RndYaw =  rand() % 5 + 1; //1-6
				VectorCopy( ent->client->ps.viewangles,viewang );

				if ( RndYaw > 3 ) {
					RndYaw -= 7;
				}
				viewang[YAW] = ent->client->ps.viewangles[1] - RndYaw;
				viewang[PITCH] -= 5;    // Martin - sniper recoil - was 15...10 is a bit more fair
				SetClientViewAngle( ent, viewang );



				//		*** Stuff from g_active that ran the old kickback
				//muzzlebounce[PITCH] -= 2*cos(2.5*(level.time - client->sniperRifleFiredTime)/RIFLE_SHAKE_TIME);
				//muzzlebounce[YAW] += 0.5*client->sniperRifleMuzzleYaw*cos(1.0-(level.time - client->sniperRifleFiredTime)*3/RIFLE_SHAKE_TIME);
				//muzzlebounce[PITCH] -= 0.25*random()*(1.0f-(level.time - client->sniperRifleFiredTime)/RIFLE_SHAKE_TIME);
				//muzzlebounce[YAW] += 0.5*crandom()*(1.0f-(level.time - client->sniperRifleFiredTime)/RIFLE_SHAKE_TIME);
			/*} else {
				//AutoSniper
				VectorCopy( ent->client->ps.viewangles,viewang );
				viewang[PITCH] -= .5;
				SetClientViewAngle( ent,viewang );
			}*/
		}
// jpw
		break;
	case WP_MAUSER:
		if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
			aimSpreadScale = 1.0;
		}
		if ( g_deathmatch.integer != 2 ) {
			Bullet_Fire( ent, MAUSER_SPREAD * aimSpreadScale, MAUSER_DAMAGE );
		} else { //lets make this a shotgun :D
			Bullet_Fire( ent, ( 800 + rand() % 201 ) * aimSpreadScale, 10 + rand() % 16 ); //we want 10-15 damage
			Bullet_Fire( ent, ( 800 + rand() % 201 ) * aimSpreadScale, 10 + rand() % 16 ); //make the spread from 800-1000
			Bullet_Fire( ent, ( 800 + rand() % 201 ) * aimSpreadScale, 10 + rand() % 16 );
			Bullet_Fire( ent, ( 800 + rand() % 201 ) * aimSpreadScale, 10 + rand() % 16 );
			Bullet_Fire( ent, ( 800 + rand() % 201 ) * aimSpreadScale, 10 + rand() % 16 );
			Bullet_Fire( ent, ( 800 + rand() % 201 ) * aimSpreadScale, 10 + rand() % 16 );
			angles[YAW] = ent->client->ps.viewangles[1] ;
			angles[PITCH] = ent->client->ps.viewangles[0] - 10;
			angles[ROLL] = ent->client->ps.viewangles[2];
			SetClientViewAngle( ent, angles );
		}

		break;
	case WP_STEN:
		Bullet_Fire( ent, STEN_SPREAD * aimSpreadScale, STEN_DAMAGE );
		break;
	case WP_MP40:
		Bullet_Fire( ent, MP40_SPREAD * aimSpreadScale, MP40_DAMAGE );
		break;
	case WP_THOMPSON:
		Bullet_Fire( ent, THOMPSON_SPREAD * aimSpreadScale, THOMPSON_DAMAGE );
		break;
	case WP_PANZERFAUST:
		ent->client->ps.classWeaponTime = level.time; // JPW NERVE
		Weapon_RocketLauncher_Fire( ent );

		if ( ent->client && !( ent->r.svFlags & SVF_CASTAI ) ) {
			if ( g_deathmatch.integer == 2 && level.endDeathmatch ) {
				weapon_grenadelauncher_fire( ent,WP_SMOKETRAIL );
			}

			if ( !level.endDeathmatch ) {
				vec3_t forward;
				AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
				VectorMA( ent->client->ps.velocity, -64, forward, ent->client->ps.velocity );
			}
		}
		break;
	case WP_FG42:
		Bullet_Fire( ent, FG42_SPREAD * aimSpreadScale, FG42_DAMAGE );
		break;
	case WP_FG42SCOPE:
		Bullet_Fire( ent, FG42SCOPE_SPREAD * aimSpreadScale, FG42_DAMAGE );
		break;
	case WP_SHOTGUN:
		Bullet_Fire( ent, DEFAULT_SHOTGUN_SPREAD, DEFAULT_SHOTGUN_DAMAGE );
		break;
	case WP_GRENADE_LAUNCHER:
	case WP_GRENADE_PINEAPPLE:
	case WP_DYNAMITE:
	case WP_DYNAMITE2:


		if ( ent->client->ps.throwMine ) {
			if ( countmines( ent->client->sess.sessionTeam )  != g_mines.integer ) {
				ent->client->ps.classWeaponTime = level.time;
				//		trap_SendServerCommand( -1, va("chat \"MINE DEBUG: mines not available(%i)\n\"",level.axisMinesAvailable) );
			}
			/*	if((ent->client->sess.sessionTeam == TEAM_BLUE) && (level.alliedMinesAvailable != 0)){
			        ent->client->ps.classWeaponTime = level.time;
			    }

			    if(ent->client->sess.sessionTeam == TEAM_RED){
			            level.axisMinesAvailable--;
			            trap_SendServerCommand( -1, va("chat \"MINE DEBUG: Removed available mine(%i)\n\"",level.axisMinesAvailable) );
			    }
			    if(ent->client->sess.sessionTeam == TEAM_BLUE)
			            level.alliedMinesAvailable--;
			*/
		} else {
			if ( ent->s.weapon == WP_DYNAMITE ) {
				ent->client->ps.classWeaponTime = level.time;
			}
		}
#ifdef OMNIBOT
		pFiredShot = weapon_grenadelauncher_fire( ent, ent->s.weapon );
#else
		weapon_grenadelauncher_fire( ent, ent->s.weapon );
#endif
		break;
	case WP_FLAMETHROWER:
		// RF, this is done client-side only now
		Weapon_FlamethrowerFire( ent );
		break;
	case WP_MORTAR:
		break;
	default:
		break;
	}
#ifdef OMNIBOT
	if ( callEvent ) {
		Bot_Event_FireWeapon( ent - g_entities, Bot_WeaponGameToBot( ent->s.weapon ), pFiredShot );
	}
#endif

	//MHC - moved to calculate for ALL weapons now
	ent->client->pers.acc_shots++;
	ent->client->pers.life_shots++;
#ifdef GLOBALSTATS //let stats know we fired a shot!
	stats_event_shotFired( ent,ent->s.weapon );
#endif

}











