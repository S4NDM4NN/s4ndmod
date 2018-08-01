/*
 * name:		g_combat.c
 *
 * desc:
 *
*/

#include "g_local.h"
#include "S4ND_local.h"

#ifdef OMNIBOT
	#include  "g_rtcwbot_interface.h"
#endif

static qboolean firstblood;

int BG_GetKillSpree( playerState_t *ps ) {
	if ( ps != NULL ) {
		return ps->persistant[PERS_KILLSPREE];
	}
	return 0;
}

int BG_SetKillSpree( playerState_t *ps, int value ) {
	if ( ps != NULL ) {
		ps->persistant[PERS_KILLSPREE] = value;
	}
	return 0;
}

const char *spreeMessage[] = {
	"^7is on a ^2Killing Spree^d!",
	"^7is on a ^2Rampage^d!",
	"^7is ^2Dominating^d!",
	"^7revels in his ^2Bloodbath^d!",
	"^7is a walking ^2Slaughterhouse^d!",
	"^7wreaks ^2Havoc ^dupon his foes!",
	"^7cuts through enemies like a ^2God ^2of ^2War^d!",
	"^7is the ^2Prophet of Doom^d!",
	"^7unleashes the ^>Sandstorm^d!",
	"^7is still unleashing the ^>Sandstorm^d!"
};

const char *humiliationMessage[] = {
	"^7must be having a bad day!",
	"^7is getting his ass kicked!",
	"^7is a death magnet!",
	"^7needs remedial combat training!",
	"^7seems well-aquainted with Death!",
	"^7still can't kill shit!"
};

const char *spreeSound[] = {
	"sound/player/killingspree.wav",
	"sound/player/rampage.wav",
	"sound/player/dominating.wav",
	"sound/player/unstoppable.wav",
	"sound/player/godlike.wav",
	"sound/player/whickedsick.wav",
	"sound/player/unreal.wav",
	"sound/player/impressive.wav",
	"sound/player/holyshit.wav",
	"sound/player/holyshit.wav"
};

static void G_UpdateKillingSpree( gentity_t *ent, gentity_t *att, qboolean death ) {
	int spree = 0, old_spree = 0, snd_idx = 0;
#ifdef OMNIBOT
	qboolean skipBotEnt = (ent && (ent->r.svFlags & SVF_BOT) && (g_OmniBotFlags.integer & OBF_NO_SPREE_ANNOUNCE));
#endif
	if ( &ent->client->ps ) {
		old_spree = ent->client->ps.persistant[PERS_KILLSPREE];
	}

	// Get new spree
	// Increment spree if the player makes a kill, if spree is still less than one, set it to one.
	// Set spree to zero if the player dies, unless it's already zero or less, in which case decrement it
	spree = death ? ( old_spree > 0 ? 0 : old_spree - 1 ) : ( ( old_spree < 0 ) ? 1 : old_spree + 1 );

	// set max kill / death sprees
	if ( spree < ent->client->pers.deathSpreeMax ) {
		ent->client->pers.deathSpreeMax = spree;
	} else if ( spree > ent->client->pers.killSpreeMax ) {
		ent->client->pers.killSpreeMax = spree;
	}

	// end of killing spree
	if ( old_spree >= 5 && death ) {
		if ( g_announcer.integer & ANNOUNCE_KILLINGSPREES 
#ifdef OMNIBOT
			&& !skipBotEnt 
#endif
			) {
			if ( att == NULL || !att->client || ent == att ) {
				trap_SendServerCommand( -1, va( "chat \"%s^d's killing spree ended after ^3%d^d kills! ^d(^1SUICIDE!^d)\"",
						ent->client->pers.netname, old_spree ) );
			} else {
				trap_SendServerCommand( -1, va( "chat \"%s^d's killing spree was brought to an end by ^*%s%s ^dafter ^3%d ^dkills!\"",
						ent->client->pers.netname, OnSameTeam( ent, att ) ? "^1TEAMMATE^7 " : "",
						att->client->pers.netname, old_spree ) );
			}
		}
	}

	// end of death spree
	if ( (g_announcer.integer & ANNOUNCE_DEATHSPREES) && old_spree <= -10 && !death 
#ifdef OMNIBOT
		&& !skipBotEnt
#endif 
		) {
		trap_SendServerCommand( -1, va( "chat \"%s^d's death spree has ended after ^3%d ^ddeaths!\"",
				ent->client->pers.netname, -old_spree ) );
	}

	// new killing spree (killing spree every 5 kills, % 5)
	if ( (g_announcer.integer & ANNOUNCE_KILLINGSPREES) && spree >= 5 && (spree % 5) == 0
#ifdef OMNIBOT		
		&& !skipBotEnt
#endif
		) {
		gentity_t *te;
		snd_idx = ( spree / 5 ) - 1;
		snd_idx = ( snd_idx <= 9 ) ? snd_idx : 9;
		trap_SendServerCommand( -1, va( "chat \"^1KILLING SPREE! ^7%s ^*%s ^7(^3%d ^7kills in a row!)\"",
				ent->client->pers.netname, spreeMessage[snd_idx], spree ) );
		te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
		te->s.eventParm = G_SoundIndex( spreeSound[snd_idx] );
		te->r.svFlags |= SVF_BROADCAST;
	}
	// new death spree (death spree every 10 deaths, % 10)
	else if ( (g_announcer.integer & ANNOUNCE_DEATHSPREES) && spree <= -10 && (-spree % 10) == 0 
#ifdef OMNIBOT		
		&& !skipBotEnt
#endif
		) {
		gentity_t *te;
		snd_idx = ( -spree / 10 ) - 1;
		snd_idx = ( snd_idx < 6 ) ? snd_idx : 5;
		trap_SendServerCommand( -1, va( "chat \"^1DEATH SPREE! ^*%s ^*%s ^7(^3%d ^7deaths without a kill!)\"",
				ent->client->pers.netname,  humiliationMessage[snd_idx], -spree ) );
		te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/spree/humiliation.wav" );
		te->r.svFlags |= SVF_BROADCAST;
	}

	ent->client->ps.persistant[PERS_KILLSPREE] = spree;
}

/*
============
AddScore

Adds score to both the client and his team
============
*/
void AddScore( gentity_t *ent, int score ) {
	if ( !ent->client ) {
		return;
	}
	// no scoring during pre-match warmup
	if ( level.warmupTime ) {
		return;
	}

	// Ridah, no scoring during single player
	// DHM - Nerve :: fix typo
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		return;
	}
	// done.


	ent->client->ps.persistant[PERS_SCORE] += score;
	if ( g_gametype.integer >= GT_TEAM ) { // JPW NERVE changed to >=
		level.teamScores[ ent->client->ps.persistant[PERS_TEAM] ] += score;
	}
	CalculateRanks();
}

/*
=================
TossClientItems

Toss the weapon and powerups for the killed player
=================
*/
extern void MagicSink( gentity_t *self );
void TossClientItems( gentity_t *self ) {
	gitem_t     *item;
	int weapon;
	gentity_t   *drop = 0;

	// drop the weapon if not a gauntlet or machinegun
	weapon = self->s.weapon;

	// make a special check to see if they are changing to a new
	// weapon that isn't the mg or gauntlet.  Without this, a client
	// can pick up a weapon, be killed, and not drop the weapon because
	// their weapon change hasn't completed yet and they are still holding the MG.

	// (SA) always drop what you were switching to
	if ( 1 ) {
		if ( self->client->ps.weaponstate == WEAPON_DROPPING ) {
			weapon = self->client->pers.cmd.weapon;
		}
		if ( !( COM_BitCheck( self->client->ps.weapons, weapon ) ) ) {
			weapon = WP_NONE;
		}
	}
	//Agent - we should let everything drop, panzer does, so all these should
	/*if(g_deathmatch.integer != 2){
	// JPW NERVE don't drop these weapon types
	if ((weapon == WP_FLAMETHROWER) || (weapon == WP_GARAND) || (weapon == WP_MAUSER) || (weapon == WP_VENOM))
	    weapon = WP_NONE;
	// jpw
	}*/
	if ( weapon > WP_NONE && weapon < WP_MONSTER_ATTACK1 && self->client->ps.ammo[ BG_FindAmmoForWeapon( weapon )] ) {
		// find the item type for this weapon
		item = BG_FindItemForWeapon( weapon );
		// spawn the item

		// Rafael
		if ( !( self->client->ps.persistant[PERS_HWEAPON_USE] ) ) {
			drop = Drop_Item( self, item, 0, qfalse );
			// JPW NERVE -- fix ammo counts
			drop->count = self->client->ps.ammoclip[BG_FindClipForWeapon( weapon )];
			drop->item->quantity = self->client->ps.ammoclip[BG_FindClipForWeapon( weapon )];
			// jpw

			if ( g_dropClips.integer ) {
				if ( g_dropReload.integer ) {
					drop->count += self->client->ps.ammo[BG_FindAmmoForWeapon( weapon )];
					drop->item->quantity += self->client->ps.ammo[BG_FindAmmoForWeapon( weapon )];
				}

			}
		}
	}

	if ( ( !g_sniperWar.integer ) && ( g_deathmatch.integer != 2 ) && ( !g_instaGib.integer ) ) {
		if ( g_medPacks.integer ) {
			if ( self->client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {
				int i;
				item = BG_FindItem( "Med Health" );

				for ( i = 0; i < g_medPacks.integer; i++ ) {
					drop = Drop_Item( self, item, 0, qfalse );
					drop->think = MagicSink;
					drop->timestamp = level.time + 31200;
					drop->parent = NULL;
				}
			}
		}
		if ( ( !g_sniperWar.integer ) && ( g_deathmatch.integer != 2 ) && ( !g_instaGib.integer ) ) {
			if ( g_ammoPacks.integer ) {
				extern void MagicSink( gentity_t *self );
				if ( self->client->ps.stats[STAT_PLAYER_CLASS] == PC_LT ) {
					int i;
					item = BG_FindItem( "Ammo Pack" );

					for ( i = 0; i < g_ammoPacks.integer; i++ ) {
						drop = Drop_Item( self, item, 0, qfalse );
						drop->think = MagicSink;
						drop->timestamp = level.time + 31200;
						drop->parent = NULL;
					}
				}
			}
			if ( self->client->ps.stats[STAT_KEYS] & ( 1 << INV_BINOCS ) && g_dropBinocs.integer ) {
				item = &bg_itemlist[82];
				drop = Drop_Item( self, item, g_test1.integer, qfalse );
				drop->think = MagicSink;
				drop->timestamp = level.time + 31200;
				drop->parent = NULL;
			}

		}
		if ( ( !g_sniperWar.integer ) && ( g_deathmatch.integer != 2 ) && ( !g_instaGib.integer ) ) {
			if ( g_nadePacks.integer ) {

				if ( self->client->ps.stats[STAT_PLAYER_CLASS] == PC_ENGINEER ) {
					int i;
					if ( self->client->sess.sessionTeam == TEAM_RED ) {
						item = BG_FindItem( "Grenades" );
					} else {
						item = BG_FindItem( "Pineapples" );
					}

					for ( i = 0; i < g_nadePacks.integer; i++ ) {
						drop = Drop_Item( self, item, 0, qfalse );
						drop->think = MagicSink;
						drop->timestamp = level.time + 31200;
						drop->parent = NULL;
					}
				}
			}
		}
	} else if ( g_deathmatch.integer == 2 )    {
		if ( g_medPacks.integer ) {
			int i;
			item = BG_FindItem( "Med Health" );

			for ( i = 0; i < g_medPacks.integer; i++ ) {
				drop = Drop_Item( self, item, 0, qfalse );
				drop->think = MagicSink;
				drop->timestamp = level.time + 31200;
				drop->parent = NULL;
			}
		}
	}
}


/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker ) {
	vec3_t dir;
	vec3_t angles;

	if ( attacker && attacker != self ) {
		VectorSubtract( attacker->s.pos.trBase, self->s.pos.trBase, dir );
	} else if ( inflictor && inflictor != self ) {
		VectorSubtract( inflictor->s.pos.trBase, self->s.pos.trBase, dir );
	} else {
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	}

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw( dir );

	angles[YAW] = vectoyaw( dir );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
}


/*
==============
GibHead
==============
*/
void GibHead( gentity_t *self, int killer ) {
	G_AddEvent( self, EV_GIB_HEAD, killer );
}

/*
==================
GibEntity
==================
*/
void GibEntity( gentity_t *self, int killer ) {
	gentity_t *other = &g_entities[killer];
	vec3_t dir;

	VectorClear( dir );
	if ( other->inuse ) {
		if ( other->client ) {
			VectorSubtract( self->r.currentOrigin, other->r.currentOrigin, dir );
			VectorNormalize( dir );
		} else if ( !VectorCompare( other->s.pos.trDelta, vec3_origin ) ) {
			VectorNormalize2( other->s.pos.trDelta, dir );
		}
	}

	G_AddEvent( self, EV_GIB_PLAYER, DirToByte( dir ) );
	self->takedamage = qfalse;
	self->s.eType = ET_INVISIBLE;
	self->r.contents = 0;
}

/*
==================
body_die
==================
*/
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	if ( self->health > GIB_HEALTH ) {
		return;
	}

	GibEntity( self, 0 );
}


// these are just for logging, the client prints its own messages
//	Agent - Changing the MOD so VSP will keep track of them better and it will look better
// AGENT - This just counts down compaired to the list in bg_public.h to determine which mod to log
char    *modNames[] = {
	"MOD_UNKNOWN",
	//Sandmod below.
	"MOD_POISON_SYRINGE",
	"MOD_GOOMBA",
	"MOD_THROWING_KNIFE",
	"MOD_LANDMINE",
	"MOD_ADMIN_KILL",
	"MOD_ADMIN_SLAP",
	"MOD_AIRSTRIKE_CANISTER",
	"MOD_SUICIDE_KILL",
	"MOD_SUICIDE_GIB",
	//end sandmod
	"MOD_SHOTGUN",
	"MOD_GAUNTLET",
	"MOD_MACHINEGUN",
	"MOD_GRENADE",
	"MOD_GRENADE_SPLASH",
	"MOD_PANZERFAUST",  //MOD_ROCKET		-	This is a body shot
	"MOD_PANZERFAUST",  //MOD_ROCKET_SPLASH
	"MOD_RAILGUN",
	"MOD_LIGHTNING",
	"MOD_BFG",
	"MOD_BFG_SPLASH",
	"MOD_KNIFE",
	"MOD_KNIFE2",
	"MOD_KNIFE_STEALTH",
	"MOD_LUGER",
	"MOD_COLT",
	"MOD_MP40",
	"MOD_THOMPSON",
	"MOD_STEN",
	"MOD_MAUSER",
	"MOD_SNIPER_RIFLE", //MOD_SNIPERRIFLE	-	Its spelled SNIPER RIFLE, so make it look pretty in the stats
	"MOD_GARAND",
	"MOD_SNOOPERSCOPE",
	"MOD_SILENCER", //----(SA)
	"MOD_AKIMBO",   //----(SA)
	"MOD_BAR",  //----(SA)
	"MOD_FG42",
	"MOD_FG42SCOPE",
	"MOD_PANZERFAUST",
	"MOD_ROCKET_LAUNCHER",
	"MOD_GRENADE_LAUNCHER",
	"MOD_VENOM",
	"MOD_VENOM_FULL",
	"MOD_FLAMETHROWER",
	"MOD_TESLA",
	"MOD_SPEARGUN",
	"MOD_SPEARGUN_CO2",
	"MOD_GRENADE_PINEAPPLE",
	"MOD_CROSS",
	"MOD_MORTAR",
	"MOD_MORTAR_SPLASH",
	"MOD_KICKED",
	"MOD_GRABBER",
	"MOD_WATER",
	"MOD_DYNAMITE",         //MOD_SLIME Changed to MOD_Dynamite
	"MOD_AIRSTRIKE",        //MOD_LAVA	Changed to MOD_AIRSTRIKE
	"MOD_ARTILLERY",        //added by agent.  now vsp will differentiate between a/s and ffe
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TARGET_LASER",
	"MOD_TRIGGER_HURT",
	"MOD_CRUSH",            //MOD_GRAPPLE
	"MOD_EXPLOSIVE",
	"MOD_FALL_TO_DEATH",    //MOD_POISONGAS
	"MOD_SUICIDE",          //MOD_ZOMBIESPIT
	"MOD_ZOMBIESPIT_SPLASH",
	"MOD_WORLD",            //MOD_ZOMBIESPIRIT	-	Fire, barbwire,etc
	"MOD_ZOMBIESPIRIT_SPLASH",
	"MOD_LOPER_LEAP",
	"MOD_LOPER_GROUND",
	"MOD_LOPER_HIT",
// JPW NERVE
	"MOD_LT_ARTILLERY",
	"MOD_LT_AIRSTRIKE",
	"MOD_ENGINEER", // not sure if we'll use
	"MOD_MEDIC",        // these like this or not
// jpw
	"MOD_BAT"

};

/*
==================
player_die
==================
*/
void limbo( gentity_t *ent, qboolean makeCorpse ); // JPW NERVE

void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath ) {
	gentity_t       *ent;
	// TTimo might be used uninitialized
	int contents = 0;
	int killer;
	int i;
	//int shots = self->client->pers.life_shots;
	float acc = 0.0f;
	char        *killerName, *obit;
	qboolean nogib = qtrue;
	gitem_t     *item = NULL; // JPW NERVE for flag drop
	vec3_t launchvel,launchspot;      // JPW NERVE
	gentity_t   *flag; // JPW NERVE
	int killerhealth = 0;




	if ( self->client->ps.pm_type == PM_DEAD ) {
		return;
	}

	if ( level.intermissiontime ) {
		return;
	}

	G_UpdateKillingSpree( self, attacker, qtrue );

	self->client->ps.pm_type = PM_DEAD;
	self->client->ps.persistant[PERS_HWEAPON_USE] = 0; // cs: dead players can't be mounted

	G_AddEvent( self, EV_STOPSTREAMINGSOUND, 0 );

	//Martin - Shoving is a teamkill / kill
	//	Falls from high distance can be 3 seconds
	if ( ( self->client->pers.lastShovedTime + 3000 >= level.time ) && ( meansOfDeath == MOD_FALLING ) ) {
		attacker = self->client->pers.lastShovedClient;
	}

	// tjw: for g_shortcuts
	if ( attacker && attacker->client ) {
		self->client->pers.lastkiller_client = attacker->client->ps.clientNum;
		attacker->client->pers.lastkilled_client = self->client->ps.clientNum;
	}

	if ( attacker ) {
		killer = attacker->s.number;
		if ( attacker->client ) {
			killerName = attacker->client->pers.netname;
		} else {
			killerName = "<non-client>";
		}
	} else {
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( killer < 0 || killer >= MAX_CLIENTS ) {
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) ) {
		obit = "<bad obituary>";
	} else {
		obit = modNames[ meansOfDeath ];
	}

	if ( g_gamestate.integer == GS_PLAYING ) {
		G_LogPrintf( "Kill: %i %i %i: %s killed %s by %s\n",
					 killer, self->s.number, meansOfDeath, killerName,
					 self->client->pers.netname, obit );
	}

	if ( attacker && attacker->client ) {
		if ( !OnSameTeam( attacker, self ) ) {

			attacker->client->multikill++;
			G_UpdateKillingSpree( attacker, NULL, qfalse );

			attacker->client->pers.deathSpree = 0;  // L0 - reset deaths if kill is made for deathSpree
		}
	} // L0 - end

#ifdef OMNIBOT
	Bot_Event_Death( self - g_entities, &g_entities[attacker - g_entities], obit );
	Bot_Event_KilledSomeone( attacker - g_entities, &g_entities[self - g_entities], obit );
#endif

#ifdef GLOBALSTATS //let stats know somebody got a kill, and somebody died!
	stats_event_die( self );
	stats_event_kill( attacker,self,meansOfDeath );
#endif

	if ( g_deathmatch.integer != 2 ) {
		if ( attacker && attacker->client ) {
			if ( !OnSameTeam( attacker, self ) ) {
				attacker->client->pers.kills++;
				attacker->client->pers.life_kills++;
				if ( g_gamestate.integer == GS_PLAYING ) {
					if ( attacker->client->pers.kills > level.HAkillnum ) {
						Q_strncpyz( level.HAhighkillers, "", sizeof( level.HAhighkillers ) );
						strcat( level.HAhighkillers, attacker->client->pers.netname );
						level.HAkillnum = attacker->client->pers.kills;
					} else if ( attacker->client->pers.kills == level.HAkillnum )     {
						strcat( level.HAhighkillers, "^7, " ); //Add a comma after the other person
						strcat( level.HAhighkillers, attacker->client->pers.netname ); //and add their name
					}
				}
			} else {  // On same team
				if ( attacker != self ) {
					attacker->client->pers.teamKills++;
					attacker->client->pers.lastTeamkilledPlayer = self->client->ps.clientNum;   //Forgive for tk-revive
					if ( g_gamestate.integer == GS_PLAYING ) {
						if ( attacker->client->pers.teamKills > level.HAtknum ) {
							Q_strncpyz( level.HAtkers, "", sizeof( level.HAtkers ) );
							strcat( level.HAtkers, attacker->client->pers.netname );
							level.HAtknum = attacker->client->pers.teamKills;
						} else if ( attacker->client->pers.teamKills == level.HAtknum )     {
							strcat( level.HAtkers, "^7, " ); //Add a comma after the other person
							strcat( level.HAtkers, attacker->client->pers.netname ); //and add their name
						}
					}
				}
			}
		}
	} else {
		if ( attacker && attacker->client ) {
			if ( attacker != self ) {
				attacker->client->pers.kills++;
				attacker->client->pers.life_kills++;
				//begin pers.kills for round
				if ( attacker->client->pers.life_kills == 10 ) {
					G_AddEvent( attacker, EV_GLOBAL_SOUND, G_SoundIndex( "sound/player/excellent.wav" ) );
					trap_SendServerCommand( -1, va( "chat \"Console: %.99s^7: ^3EXCELLENT with ^110 ^3kills this life!\n\"", attacker->client->pers.netname ) );
				}
				if ( attacker->client->pers.life_kills == 5 ) {
					G_AddEvent( attacker, EV_GLOBAL_SOUND, G_SoundIndex( "sound/player/impressive.wav" ) );
					trap_SendServerCommand( -1, va( "chat \"Console: %.99s^7: ^3Impressive with ^15 ^3kills this life!\n\"", attacker->client->pers.netname ) );
				}
				//Multi
				if ( attacker->client->pers.kills == 5 ) {
					G_AddEvent( attacker, EV_GLOBAL_SOUND, G_SoundIndex( "sound/player/multikill.wav" ) );
					trap_SendServerCommand( -1, va( "chat \"Console: %.99s ^7: ^3Multikill with ^75 ^3kills this game!\n\"", attacker->client->pers.netname ) );
				}
				//MEGA
				if ( attacker->client->pers.kills == 10 ) {
					G_AddEvent( attacker, EV_GLOBAL_SOUND, G_SoundIndex( "sound/player/megakill.wav" ) );
					trap_SendServerCommand( -1, va( "chat \"Console: %.99s ^7: ^3MEGAKILL with ^710 ^3kills this game!\n\"", attacker->client->pers.netname ) );
				}
			}
		}
	}

	self->client->pers.deaths++;
	if ( g_gamestate.integer == GS_PLAYING ) {
		if ( self->client->pers.deaths > level.HAdeathnum ) {
			Q_strncpyz( level.HAhighdeaths, "", sizeof( level.HAhighdeaths ) );
			strcat( level.HAhighdeaths, self->client->pers.netname );
			level.HAdeathnum = self->client->pers.deaths;
		} else if ( self->client->pers.deaths == level.HAdeathnum )     {
			strcat( level.HAhighdeaths, "^7, " ); //Add a comma after the other person
			strcat( level.HAhighdeaths, self->client->pers.netname ); //and add their name
		}
	}


	//S4NDM4NN - Moved this around a bit.
	// L0 - deathSprees
	self->client->pers.deathSpree++;    // L0 - add count for deathSprees to self if you die
	deathSpree( self );

	if (!S4ND_IsDemo())
	{
		// broadcast the death event to everyone
		ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
		ent->s.eventParm = meansOfDeath;
		ent->s.otherEntityNum = self->s.number;
		ent->s.otherEntityNum2 = killer;
		ent->r.svFlags = SVF_BROADCAST; // send to everyone
	}



	self->enemy = attacker;

	//S4NDM4NN the client is a spy so change them back
	if ( self->client->ps.isSpy ) {
		self->client->ps.isSpy = qfalse;
		ClientUserinfoChanged( self->client->ps.clientNum );
	}


	// Martin 12/3/2009 - Does this server side for demo
	//S4NDM4NN - Doing this client side again 12/27/2009
	//S4NDM4NN-bring means of death from server side
	//this will allow all vs all and also diff /sex
	if (S4ND_IsDemo())
	{
		print_mod(attacker, self, meansOfDeath );
	}
	//END S4NDM4NN

	if ( ( self && self->client ) && ( self->client->pers.gold - 1 > 4 ) && ( g_goldrush.integer ) ) {
		trap_SendServerCommand( -1, va( "cp \"^3GOLDRUSH!\n\"2" ) );
		trap_SendServerCommand( -1, va( "print \"^3%s: ^5GOLDRUSH!\n\" 2",GetLevelTime() ) );
	}

	if ( g_showLifeStats.integer ) {
		//avoid divide-by-0 error, will be 0.0 until the player hits someone
		if ( ( self->client->pers.life_shots ) > 0 ) {
			acc = ( (float)self->client->pers.life_hits / (float)self->client->pers.life_shots ) * 100.0f;
		}

		if ( attacker && attacker->client ) {
			killerhealth = attacker->health;
			if ( killerhealth < 0 ) {
				killerhealth = 0;
			}
		}
		if ( ( g_deathmatch.integer != 2 ) && ( g_gamestate.integer == GS_PLAYING ) ) {
			if ( attacker && attacker->client && ( !OnSameTeam( attacker, self ) ) ) {
				if ( self->client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {
					trap_SendServerCommand( self - g_entities, va( "chat \"^7Last Life: ^3Kills: ^7%d, ^3Revives ^7%d, ^3Accuracy: ^7%2.2f (%d/%d), ^3Killer Health: ^7%d\"", self->client->pers.life_kills, self->client->pers.life_revives, acc, self->client->pers.life_hits, self->client->pers.life_shots, killerhealth ) );
				} else {
					trap_SendServerCommand( self - g_entities, va( "chat \"^7Last Life: ^3Kills: ^7%d, ^3Accuracy: ^7%2.2f (%d/%d), ^3Killer Health: ^7%d\"", self->client->pers.life_kills, acc, self->client->pers.life_hits, self->client->pers.life_shots, killerhealth ) );
				}
			} else {
				if ( self->client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {
					trap_SendServerCommand( self - g_entities, va( "chat \"^7Last Life: ^3Kills: ^7%d, ^3Revives ^7%d, ^3Accuracy: ^7%2.2f (%d/%d)\"", self->client->pers.life_kills, self->client->pers.life_revives, acc, self->client->pers.life_hits, self->client->pers.life_shots ) );
				} else {
					trap_SendServerCommand( self - g_entities, va( "chat \"^7Last Life: ^3Kills: ^7%d, ^3Accuracy: ^7%2.2f (%d/%d)\"", self->client->pers.life_kills, acc, self->client->pers.life_hits, self->client->pers.life_shots ) );
				}
			}
		} else {
			if ( ( attacker && attacker->client ) && ( self != attacker ) ) {
				trap_SendServerCommand( self - g_entities, va( "chat \"^7Last Life: ^3Kills: ^7%d, ^3Accuracy: ^7%2.2f (%d/%d), ^3Killer Health: ^7%d\"", self->client->pers.life_kills, acc, self->client->pers.life_hits, self->client->pers.life_shots, killerhealth ) );
			} else {
				trap_SendServerCommand( self - g_entities, va( "chat \"^7Last Life: ^3Kills: ^7%d, ^3Accuracy: ^7%2.2f (%d/%d)\"", self->client->pers.life_kills, acc, self->client->pers.life_hits, self->client->pers.life_shots ) );
			}
		}
	}


	self->client->ps.persistant[PERS_KILLED]++;


	//Fraglimit stuff...
	//if(g_sniperWar.integer){dont make this based on only sniper mode
	if ( ( g_fraglimit.integer ) && ( attacker && attacker->client ) && ( g_gamestate.integer == GS_PLAYING ) ) {
		if ( ( self != attacker ) && ( !OnSameTeam( attacker, self ) ) ) {
			if ( attacker->client->sess.sessionTeam == TEAM_RED ) {
				level.teamRedKills++;
			} else {
				level.teamBlueKills++;
			}
		}
		if ( g_deathmatch.integer != 2 ) {
			if ( ( self == attacker ) || ( OnSameTeam( attacker, self ) ) ) {
				if ( attacker->client->sess.sessionTeam == TEAM_RED ) {
					level.teamRedKills--;
					trap_SendServerCommand( -1, va( "print \"%s ^7just cost the Axis a kill\n\"",attacker->client->pers.netname ) );
				} else {
					level.teamBlueKills--;
					trap_SendServerCommand( -1, va( "print \"%s ^7just cost the Allies a kill\n\"",attacker->client->pers.netname ) );
				}
			}
		}
	}


// JPW NERVE -- if player is holding ticking grenade, drop it
	if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
		if ( ( self->client->ps.grenadeTimeLeft ) && ( self->s.weapon != WP_DYNAMITE ) ) {
			launchvel[0] = crandom();
			launchvel[1] = crandom();
			launchvel[2] = random();
			VectorScale( launchvel, 160, launchvel );
			VectorCopy( self->r.currentOrigin, launchspot );
			launchspot[2] += 40;
			fire_grenade( self, launchspot, launchvel, self->s.weapon );

		}
	}
// jpw
	if ( g_deathmatch.integer < 2 ) {
		if ( attacker && attacker->client ) {
			if ( attacker == self || OnSameTeam( self, attacker ) ) {

				// DHM - Nerve :: Complaint lodging
				if ( g_complaintlimit.integer ) {
					if ( attacker != self && level.warmupTime <= 0 ) {
#ifdef OMNIBOT
						if ( attacker->client->pers.localClient && !( attacker->r.svFlags & SVF_BOT ) ) {
#else
						if ( attacker->client->pers.localClient ) {
#endif // OMNIBOT
							trap_SendServerCommand( self - g_entities, "complaint -4" );
						} else {
							trap_SendServerCommand( self - g_entities, va( "complaint %i", attacker->s.number ) );
							self->client->pers.complaintClient = attacker->s.clientNum;
							self->client->pers.complaintEndTime = level.time + 20500;
						}
					}
				}

				// dhm

				// JPW NERVE



				if ( g_gametype.integer >= GT_WOLF ) { // high penalty to offset medic heal
					AddScore( attacker, WOLF_FRIENDLY_PENALTY );
				} else {
					// jpw
					AddScore( attacker, -1 );
				}

			} else {
				// L0 - FirstBlood
				if ( g_gametype.integer >= GT_WOLF && g_FirstBlood.integer == 2 ) {
					if ( level.firstbloodTeam == -1 ) {
						level.firstbloodTeam = attacker->client->sess.sessionTeam;
					}
				} // L0 - end
				  // JPW NERVE -- mostly added as conveneience so we can tweak from the #defines all in one place
				else if ( g_gametype.integer >= GT_WOLF ) {
					AddScore( attacker, WOLF_FRAG_BONUS );
				} else {
					// jpw
					AddScore( attacker, 1 );
				}
				DoubleKills( attacker, meansOfDeath );  // L0 - check for double kills.. - NOTE: 4.03/09 Added MOD, way better way to track kills and control print... :)

				attacker->client->lastKillTime = level.time;
			}
		} else {
			AddScore( self, -1 );
		}

		// Add team bonuses
		Team_FragBonuses( self, inflictor, attacker );

	} else {
		if ( attacker == self ) {
			AddScore( attacker, -1 );
		} else {
			AddScore( attacker, WOLF_FRAG_BONUS );
		}
	}

//======================MOVED================================================================
//was checking score before it even added the score oops
	//S4NDM4NN-for the admin bot
	if ( g_deathmatch.integer != 2 ) {
		if ( g_gamestate.integer == GS_PLAYING ) {
			if ( ( attacker && attacker->client ) && ( OnSameTeam( attacker, self ) ) && ( AB_Enable.integer ) ) {
				check_adminbot( attacker, self, meansOfDeath );
			}
		}
	}
	//End S4NDM4NN
//=============================================================================================
//S4NDM4NN - going to make a CheckDeathmatch to check high score and stuff i think
//This will be used for top agent, and just for the taken the lead messages.
	if ( g_deathmatch.integer == 2 ) {
		if ( !level.endDeathmatch ) {
			CheckDeathmatch( attacker, self );
		}
	}
//End

//Check our stats to see if they need to be highlighted as doing well Agent
	if ( ( g_gamestate.integer == GS_PLAYING ) && ( g_ShowHighlights.integer ) ) {
		if ( ( attacker->client && attacker ) && ( !OnSameTeam( attacker,self ) ) ) {
			StatsCheck( attacker );
		}
	}

// if client is in a nodrop area, don't drop anything
// JPW NERVE new drop behavior
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {   // only drop here in single player; in multiplayer, drop @ limbo
		contents = trap_PointContents( self->r.currentOrigin, -1 );
		if ( !( contents & CONTENTS_NODROP ) ) {
			TossClientItems( self );
		}
	}
	if ( OnSameTeam( self, attacker ) && g_goldrush.integer ) {
		self->client->pers.gold--;
	}

	if ( g_goldrush.integer && self->client->pers.gold ) {
		int i;

		item = BG_FindItem( "Gold" );


		for ( i = 0; i < self->client->pers.gold - 1; i++ ) {
			if ( item ) {

				launchvel[0] = 100 + crandom() * 500;
				launchvel[1] = 100 + crandom() * 500;
				launchvel[2] = 10 + random() * 100;

				flag = LaunchItem( item,self->r.currentOrigin,launchvel,self->s.number );
				// default is the same color as the tesla weapon
				//	flag->dl_color[0] = 0.2f;
				//	flag->dl_color[1] = 0.6f;
				//	flag->dl_color[2] = 1.0f;
				// 2 = blue 1 = green?  0 = red
				flag->dl_color[0] = 1;
				flag->dl_color[1] = 1;
				flag->dl_color[2] = 0;

//			SP_dlight(flag);
			}
		}
	}


	if ( g_tournymode.integer && g_gamestate.integer != GS_PLAYING ) {
		self->client->ps.powerups[PW_REDFLAG] = 0;
	}
	// drop flag regardless
	if ( g_gamestate.integer == GS_PLAYING ) {
		//Agent - dropflag sound fix (yea proabley not the best way to do this shit, but eh..)
		char index[MAX_OSPATH];
		gentity_t *other;
		gentity_t *te;
		if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
			if ( self->client->ps.powerups[PW_REDFLAG] ) {
				item = BG_FindItem( "Red Flag" );
				if ( !item ) {
					item = BG_FindItem( "Objective" );
				}
				Q_strncpyz( index, "sound/player/allies/hq_objlost.wav",sizeof( index ) );
				trap_SendServerCommand( -1, va( "cp \"Allies have lost %s!\n\" 2", self->message ) );
				trap_SendServerCommand( -1, va( "print \"^3%s: ^5Allies have lost %s!\n\" 2",GetLevelTime(), self->message ) );
				self->client->ps.powerups[PW_REDFLAG] = 0;
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
			if ( self->client->ps.powerups[PW_BLUEFLAG] ) {
				item = BG_FindItem( "Blue Flag" );
				if ( !item ) {
					item = BG_FindItem( "Objective" );
				}
				Q_strncpyz( index, "sound/player/axis/hq_objlost.wav",sizeof( index ) );
				trap_SendServerCommand( -1, va( "cp \"Axis have lost %s!\n\" 2", self->message ) );
				trap_SendServerCommand( -1, va( "print \"^3%s: ^5Axis have lost %s!\n\" 2",GetLevelTime(), self->message ) );

				self->client->ps.powerups[PW_BLUEFLAG] = 0;
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

				flag = LaunchItem( item,self->r.currentOrigin,launchvel,self->s.number );
				flag->s.modelindex2 = self->s.otherEntityNum2; // JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
				flag->message = self->message; // DHM - Nerve :: also restore item name
				// Clear out player's temp copies
				self->s.otherEntityNum2 = 0;
				self->message = NULL;
			}
		}
		// send a fancy "MEDIC!" scream.  Sissies, ain' they?
		if ( self->client != NULL ) {
			if ( self->health > GIB_HEALTH && meansOfDeath != MOD_SUICIDE_GIB ) {
				if ( self->client->sess.sessionTeam == TEAM_RED ) {
					if ( random() > 0.5 ) {
						G_AddEvent( self, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/axis/g-medic2.wav" ) );
					} else {
						G_AddEvent( self, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/axis/g-medic3.wav" ) );
					}
				} else {
					if ( random() > 0.5 ) {
						G_AddEvent( self, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/allies/a-medic3.wav" ) );
					} else {
						G_AddEvent( self, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/allies/a-medic2.wav" ) );
					}
				}
#ifdef OMNIBOT
				// ATM: only register the goal if the target isn't in water.
				if ( self->waterlevel <= 1 /*|| g_underwaterRevive.integer*/ ) {
					Bot_AddFallenTeammateGoals( self, self->client->sess.sessionTeam );
				}
#endif
			}
		}
	}
// jpw

	Cmd_Score_f( self );        // show scores
	// send updated scores to any clients that are following this one,
	// or they would get stale scoreboards
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		gclient_t   *client;

		client = &level.clients[i];
		if ( client->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			continue;
		}
		if ( client->sess.spectatorClient == self->s.number ) {
			Cmd_Score_f( g_entities + i );
		}
	}

	self->takedamage = qtrue;   // can still be gibbed
	self->r.contents = CONTENTS_CORPSE;

	self->s.powerups = 0;
// JPW NERVE -- only corpse in SP; in MP, need CONTENTS_BODY so medic can operate
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		self->s.weapon = WP_NONE;
		self->s.angles[0] = 0;
	} else {
		self->client->limboDropWeapon = self->s.weapon; // store this so it can be dropped in limbo
	}
// jpw
	self->s.angles[2] = 0;
	LookAtKiller( self, inflictor, attacker );

#ifdef OMNIBOT
	if ( !( self->r.svFlags & SVF_BOT ) )
#endif
	VectorCopy( self->s.angles, self->client->ps.viewangles );
	self->s.loopSound = 0;

	trap_UnlinkEntity( self );
	self->r.maxs[2] = 0;
	self->client->ps.maxs[2] = 0;
	trap_LinkEntity( self );

	// don't allow respawn until the death anim is done
	// g_forcerespawn may force spawning at some later time
	self->client->respawnTime = level.time + 800;

	// remove powerups
	memset( self->client->ps.powerups, 0, sizeof( self->client->ps.powerups ) );

	// never gib in a nodrop
	if ( self->health <= GIB_HEALTH && !( contents & CONTENTS_NODROP ) ) {
		GibEntity( self, killer );
		nogib = qfalse;
	}

	if ( nogib ) {
		// normal death
		// for the no-blood option, we need to prevent the health
		// from going to gib level
		if ( self->health <= GIB_HEALTH ) {
			self->health = GIB_HEALTH + 1;
		}

// JPW NERVE for medic
		self->client->medicHealAmt = 0;
// jpw

		// DHM - Play death animation, and set pm_time to delay 'fallen' animation
		self->client->ps.pm_time = BG_AnimScriptEvent( &self->client->ps, ANIM_ET_DEATH, qfalse, qtrue );

		G_AddEvent( self, EV_DEATH1 + 1, killer );

		// the body can still be gibbed
		self->die = body_die;
	}

	if ( g_announcer.integer &&
		 !firstblood &&
		 self &&
		 self->client &&
		 attacker &&
		 attacker->client &&
		 attacker->s.number != ENTITYNUM_NONE &&
		 attacker->s.number != ENTITYNUM_WORLD &&
		 attacker != self &&
		 g_gamestate.integer == GS_PLAYING &&
		 !OnSameTeam( self, attacker ) ) {
		gentity_t *te = G_TempEntity( level.intermission_origin, EV_GLOBAL_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/spree/firstblood.wav" );
		te->r.svFlags |= SVF_BROADCAST;
		AP( va( "chat \"^7%s ^7drew ^1FIRST BLOOD ^7from ^7%s^7!\" -1",
				attacker->client->pers.netname, self->client->pers.netname ) );

		firstblood = qtrue;
	}

	// L0 - First Blood is now separated and controlled in function...
//	if(g_ShowHighlights.integer){
	FirstBlood( self, attacker );
//	} // L0 - end

	trap_LinkEntity( self );

	if ( g_gametype.integer >= GT_WOLF && meansOfDeath == MOD_SUICIDE ) {
		limbo( self, qtrue );
	}
}


/*
================
CheckArmor
================
*/
int CheckArmor( gentity_t *ent, int damage, int dflags ) {
	gclient_t   *client;
	int save;
	int count;

	if ( !damage ) {
		return 0;
	}

	client = ent->client;

	if ( !client ) {
		return 0;
	}

	if ( dflags & DAMAGE_NO_ARMOR ) {
		return 0;
	}

	// armor
	count = client->ps.stats[STAT_ARMOR];
	save = ceil( damage * ARMOR_PROTECTION );
	if ( save >= count ) {
		save = count;
	}

	if ( !save ) {
		return 0;
	}

	client->ps.stats[STAT_ARMOR] -= save;

	return save;
}

qboolean IsHeadShotWeapon( int mod, qboolean aicharacter ) {
#ifndef NO_BOT_SUPPORT
	if ( aicharacter ) {       // ai's are allowed headshots from these weapons
		if ( mod == MOD_SNIPERRIFLE ||
			 mod == MOD_SNOOPERSCOPE ) {
			return qtrue;
		}

		return qfalse;
	}
#endif // NO_BOT_SUPPORT

	// players are allowed headshots from these weapons
	if (    mod == MOD_LUGER ||
			mod == MOD_COLT ||
			mod == MOD_AKIMBO ||    //----(SA)	added
			mod == MOD_MP40 ||
			mod == MOD_THOMPSON ||
			mod == MOD_STEN ||
			mod == MOD_BAR ||
			mod == MOD_FG42 ||
			mod == MOD_FG42SCOPE ||
			( mod == MOD_MAUSER && g_deathmatch.integer != 2 ) ||
			mod == MOD_GARAND || // JPW NERVE this was left out
			mod == MOD_SNIPERRIFLE ||
			mod == MOD_SNOOPERSCOPE ||
			mod == MOD_SILENCER ||  //----(SA)	modified
			mod == MOD_SNIPERRIFLE ) {
		return qtrue;
	}

	return qfalse;
}

qboolean IsHeadShot( gentity_t *targ, qboolean isAICharacter, vec3_t dir, vec3_t point, int mod ) {
	gentity_t   *head;
	trace_t tr;
	vec3_t start, end;
	gentity_t   *traceEnt;
	orientation_t or;           // DHM - Nerve

	qboolean head_shot_weapon = qfalse;

	// not a player or critter so bail
	if ( !( targ->client ) ) {
		return qfalse;
	}

	if ( targ->health <= 0 ) {
		return qfalse;
	}

	head_shot_weapon = IsHeadShotWeapon( mod, isAICharacter );

	if ( head_shot_weapon ) {
		head = G_Spawn();

		if ( trap_GetTag( targ->s.number, "tag_head", &or ) ) {
			G_SetOrigin( head, or.origin );
		} else {
			float height, dest;
			vec3_t v, angles, forward, up, right;

			G_SetOrigin( head, targ->r.currentOrigin );

			if ( targ->client->ps.pm_flags & PMF_DUCKED ) { // closer fake offset for 'head' box when crouching
				height = targ->client->ps.crouchViewHeight - 12;
			} else {
				height = targ->client->ps.viewheight;
			}

			// NERVE - SMF - this matches more closely with WolfMP models
			VectorCopy( targ->client->ps.viewangles, angles );
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
		VectorCopy( targ->r.currentAngles, head->s.angles );
		VectorCopy( head->s.angles, head->s.apos.trBase );
		VectorCopy( head->s.angles, head->s.apos.trDelta );
		VectorSet( head->r.mins, -6, -6, -2 ); // JPW NERVE changed this z from -12 to -6 for crouching, also removed standing offset
		VectorSet( head->r.maxs, 6, 6, 10 ); // changed this z from 0 to 6
		head->clipmask = CONTENTS_SOLID;
		head->r.contents = CONTENTS_SOLID;

		trap_LinkEntity( head );

		// trace another shot see if we hit the head
		VectorCopy( point, start );
		VectorMA( start, 64, dir, end );
		trap_Trace( &tr, start, NULL, NULL, end, targ->s.number, MASK_SHOT );

		traceEnt = &g_entities[ tr.entityNum ];

		if ( g_debugBullets.integer >= 3 ) {   // show hit player head bb
			gentity_t *tent;
			vec3_t b1, b2;
			VectorCopy( head->r.currentOrigin, b1 );
			VectorCopy( head->r.currentOrigin, b2 );
			VectorAdd( b1, head->r.mins, b1 );
			VectorAdd( b2, head->r.maxs, b2 );
			tent = G_TempEntity( b1, EV_RAILTRAIL );
			VectorCopy( b2, tent->s.origin2 );
			tent->s.dmgFlags = 1;

			// show headshot trace
			// end the headshot trace at the head box if it hits
			if ( tr.fraction != 1 ) {
				VectorMA( start, ( tr.fraction * 64 ), dir, end );
			}
			tent = G_TempEntity( start, EV_RAILTRAIL );
			VectorCopy( end, tent->s.origin2 );
			tent->s.dmgFlags = 0;
		}

		G_FreeEntity( head );

		if ( traceEnt == head ) {
			level.totalHeadshots++;         // NERVE - SMF
			return qtrue;
		} else {
			level.missedHeadshots++;    // NERVE - SMF
		}
	}

	return qfalse;
}





/*
==============
G_ArmorDamage
    brokeparts is how many should be broken off now
    curbroke is how many are broken
    the difference is how many to pop off this time
==============
*/
void G_ArmorDamage( gentity_t *targ ) {
	int brokeparts, curbroke;
	int numParts;
	int dmgbits = 16;   // 32/2;
	int i;

	if ( !targ->client ) {
		return;
	}

	if ( targ->s.aiChar == AICHAR_PROTOSOLDIER ) {
		numParts = 9;
	} else if ( targ->s.aiChar == AICHAR_SUPERSOLDIER ) {
		numParts = 14;
	} else if ( targ->s.aiChar == AICHAR_HEINRICH ) {
		numParts = 20;
	} else {
		return;
	}

	if ( numParts > dmgbits ) {
		numParts = dmgbits; // lock this down so it doesn't overwrite any bits that it shouldn't.  TODO: fix this


	}
	// determined here (on server) by location of hit and existing armor, you're updating here so
	// the client knows which pieces are still in place, and by difference with previous state, which
	// pieces to play an effect where the part is blown off.
	// Need to do it here so we have info on where the hit registered (head, torso, legs or if we go with more detail; arm, leg, chest, codpiece, etc)

	// ... Ick, just discovered that the refined hit detection ("hit nearest to which tag") is clientside...

	// For now, I'll randomly pick a part that hasn't been cleared.  This might end up looking okay, and we won't need the refined hits.
	//	however, we still have control on the server-side of which parts come off, regardless of what shceme is used.

	brokeparts = (int)( ( 1 - ( (float)( targ->health ) / (float)( targ->client->ps.stats[STAT_MAX_HEALTH] ) ) ) * numParts );

	if ( brokeparts && ( ( targ->s.dmgFlags & ( ( 1 << numParts ) - 1 ) ) != ( 1 << numParts ) - 1 ) ) {   // there are still parts left to clear

		// how many are removed already?
		curbroke = 0;
		for ( i = 0; i < numParts; i++ ) {
			if ( targ->s.dmgFlags & ( 1 << i ) ) {
				curbroke++;
			}
		}

		// need to remove more
		if ( brokeparts - curbroke >= 1 && curbroke < numParts ) {
			for ( i = 0; i < ( brokeparts - curbroke ); i++ ) {
				int remove = rand() % ( numParts );

				if ( !( ( targ->s.dmgFlags & ( ( 1 << numParts ) - 1 ) ) != ( 1 << numParts ) - 1 ) ) { // no parts are available any more
					break;
				}

				// FIXME: lose the 'while' loop.  Still should be safe though, since the check above verifies that it will eventually find a valid part
				while ( targ->s.dmgFlags & ( 1 << remove ) ) {
					remove = rand() % ( numParts );
				}

				targ->s.dmgFlags |= ( 1 << remove );    // turn off 'undamaged' part
				if ( (int)( random() + 0.5 ) ) {                       // choose one of two possible replacements
					targ->s.dmgFlags |= ( 1 << ( numParts + remove ) );
				}
			}
		}
	}
}


//S4NDM4NN-Location damage
/*
============
G_LocationDamage
============
*/
int G_LocationDamage( vec3_t point, gentity_t* targ, gentity_t* attacker, int take ) {
	vec3_t bulletPath;
	vec3_t bulletAngle;

	int clientHeight;
	int clientFeetZ;
	int clientRotation;
	int bulletHeight;
	int bulletRotation; // Degrees rotation around client.
	// used to check Back of head vs. Face
	int impactRotation;


	// First things first.  If we're not damaging them, why are we here?
	if ( !take ) {
		return 0;
	}

	// Point[2] is the REAL world Z. We want Z relative to the clients feet

	// Where the feet are at [real Z]
	clientFeetZ  = targ->r.currentOrigin[2] + targ->r.mins[2];
	// How tall the client is [Relative Z]
	clientHeight = targ->r.maxs[2] - targ->r.mins[2];
	// Where the bullet struck [Relative Z]
	bulletHeight = point[2] - clientFeetZ;

	// Get a vector aiming from the client to the bullet hit
	VectorSubtract( targ->r.currentOrigin, point, bulletPath );
	// Convert it into PITCH, ROLL, YAW
	vectoangles( bulletPath, bulletAngle );

	clientRotation = targ->client->ps.viewangles[YAW];
	bulletRotation = bulletAngle[YAW];

	impactRotation = abs( clientRotation - bulletRotation );

	impactRotation += 45; // just to make it easier to work with
	impactRotation = impactRotation % 360; // Keep it in the 0-359 range

	if ( impactRotation < 90 ) {
		targ->client->lasthurt_location = LOCATION_BACK;
	} else if ( impactRotation < 180 ) {
		targ->client->lasthurt_location = LOCATION_RIGHT;
	} else if ( impactRotation < 270 ) {
		targ->client->lasthurt_location = LOCATION_FRONT;
	} else if ( impactRotation < 360 ) {
		targ->client->lasthurt_location = LOCATION_LEFT;
	} else {
		targ->client->lasthurt_location = LOCATION_NONE;
	}

	// The upper body never changes height, just distance from the feet
	if ( bulletHeight > clientHeight - 2 ) {
		targ->client->lasthurt_location |= LOCATION_HEAD;
	} else if ( bulletHeight > clientHeight - 8 ) {
		targ->client->lasthurt_location |= LOCATION_FACE;
	} else if ( bulletHeight > clientHeight - 10 ) {
		targ->client->lasthurt_location |= LOCATION_SHOULDER;
	} else if ( bulletHeight > clientHeight - 16 ) {
		targ->client->lasthurt_location |= LOCATION_CHEST;
	} else if ( bulletHeight > clientHeight - 26 ) {
		targ->client->lasthurt_location |= LOCATION_STOMACH;
	} else if ( bulletHeight > clientHeight - 29 ) {
		targ->client->lasthurt_location |= LOCATION_GROIN;
	} else if ( bulletHeight < 4 ) {
		targ->client->lasthurt_location |= LOCATION_FOOT;
	} else {
		// The leg is the only thing that changes size when you duck,
		// so we check for every other parts RELATIVE location, and
		// whats left over must be the leg.
		targ->client->lasthurt_location |= LOCATION_LEG;
	}



	// Check the location ignoring the rotation info
	switch ( targ->client->lasthurt_location &
			 ~( LOCATION_BACK | LOCATION_LEFT | LOCATION_RIGHT | LOCATION_FRONT ) )
	{
	case LOCATION_HEAD:
		take *= 1;
		//	trap_SendServerCommand( attacker-g_entities, "chat \"head\n\"" );
		if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
			attacker->client->pers.hithead++;
		}
		break;
	case LOCATION_FACE:
		if ( targ->client->lasthurt_location & LOCATION_FRONT ) {
			take *= 1;     // Faceshots REALLY suck
			//		trap_SendServerCommand(attacker-g_entities, "chat \"face\n\"" );
			if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
				attacker->client->pers.hitface++;
			}
		} else {
			take *= 1;
			//		trap_SendServerCommand( attacker-g_entities, "chat \"head i think?1?\n\"" );
			if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
				attacker->client->pers.hithead++;
			}
		}
		break;
	case LOCATION_SHOULDER:
		if ( targ->client->lasthurt_location & ( LOCATION_FRONT | LOCATION_BACK ) ) {
			take *= 1;     // Throat or nape of neck
			//	trap_SendServerCommand( attacker-g_entities, "chat \"throat\n\"" );
			if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
				attacker->client->pers.hitthroat++;
			}
		} else {
			take *= 1;     // Shoulders
			//trap_SendServerCommand( attacker-g_entities, "chat \"shoulders\n\"" );
			if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
				attacker->client->pers.hitarm++;
			}
		}
		break;
	case LOCATION_CHEST:
		if ( targ->client->lasthurt_location & ( LOCATION_FRONT | LOCATION_BACK ) ) {
			take *= 1;     // Belly or back
			//	trap_SendServerCommand( attacker-g_entities, "chat \"belly/back\n\"" );
			if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
				attacker->client->pers.hitbody++;
			}
		} else {
			take *= 1;     // Arms
			//		trap_SendServerCommand( attacker-g_entities, "chat \"arm\n\"" );
			if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
				attacker->client->pers.hitarm++;
			}
		}
		break;
	case LOCATION_STOMACH:
		take *= 1;
		//	trap_SendServerCommand( attacker-g_entities, "chat \"shomach\n\"" );
		if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
			attacker->client->pers.hitbody++;
		}
		break;
	case LOCATION_GROIN:
		if ( targ->client->lasthurt_location & LOCATION_FRONT ) { // Groin shot
			//	trap_SendServerCommand( attacker-g_entities, "chat \"groin\n\"" );
			take *= 1;
			if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
				attacker->client->pers.hitleg++;
			}
		}
		break;
	case LOCATION_LEG:
		take *= 1;
		//	trap_SendServerCommand( attacker-g_entities, "chat \"leg\n\"" );
		if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
			attacker->client->pers.hitleg++;
		}
		break;
	case LOCATION_FOOT:
		take *= 1;
		//	trap_SendServerCommand( attacker-g_entities, "chat \"foot\n\"" );
		if ( !OnSameTeam( targ, attacker ) || g_deathmatch.integer == 2 ) {
			attacker->client->pers.hitfoot++;
		}
		break;

	}
	//	if ( !OnSameTeam( targ, attacker ) ){
	//	attacker->client->pers.dmgGiven += take;
	//	targ->client->pers.dmgreceived += take;
	//	}
	return take;

}
//End S4NDM4NN


/*
============
T_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
    example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
    DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
    DAMAGE_NO_ARMOR			armor does not protect from this damage
    DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
    DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/
#ifdef OMNIBOT
const char *_GetEntityName( gentity_t *_ent );
#endif
void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
			   vec3_t dir, vec3_t point, int damage, int dflags, int mod ) {
	gclient_t   *client;
	int take;
	int save;
	int asave;
	int knockback;
	qboolean isHeadshot = qfalse;

	if ( !targ->takedamage ) {
		return;
	}

	// the intermission has allready been qualified for, so don't
	// allow any extra scoring
	if ( level.intermissionQueued || g_gamestate.integer != GS_PLAYING ) {
		//warmupdmg
		if ( ( !g_warmupDamage.integer ) ) { //|| ((g_gamestate.integer != GS_WARMUP) && (g_gamestate.integer != GS_WARMUP_COUNTDOWN)))
			return;
		}
	}

	if ( level.intermissiontime ) { //we are in intermission DONT allow this
		return;
	}

	if ( !inflictor ) {
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}
	if ( !attacker ) {
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

// JPW NERVE
	if ( ( targ->waterlevel >= 3 ) && ( mod == MOD_FLAMETHROWER ) ) {
		return;
	}
// jpw

	// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER && !( targ->aiName ) && !( targ->isProp ) && !targ->scriptName ) {
		if ( targ->use && targ->moverState == MOVER_POS1 ) {
			targ->use( targ, inflictor, attacker );
		}
		return;
	}

	if ( targ->s.eType == ET_MOVER && targ->aiName && !( targ->isProp ) && !targ->scriptName ) {
		switch ( mod ) {
		case MOD_GRENADE:
		case MOD_GRENADE_SPLASH:
		case MOD_ROCKET:
		case MOD_ROCKET_SPLASH:
			break;
		default:
			return; // no damage from other weapons
		}
	} else if ( targ->s.eType == ET_EXPLOSIVE )   {
		// 32 Explosive
		// 64 Dynamite only
		if ( ( targ->spawnflags & 32 ) || ( targ->spawnflags & 64 ) ) {
			switch ( mod ) {
			case MOD_GRENADE:
			case MOD_GRENADE_SPLASH:
			case MOD_ROCKET:
			case MOD_ROCKET_SPLASH:
			case MOD_AIRSTRIKE:
			case MOD_ARTILLERY:
			case MOD_GRENADE_PINEAPPLE:
			case MOD_MORTAR:
			case MOD_MORTAR_SPLASH:
			case MOD_EXPLOSIVE:
				if ( targ->spawnflags & 64 ) {
					return;
				}
				break;

			case MOD_DYNAMITE:
			case MOD_DYNAMITE_SPLASH:
				break;

			default:
				return;
			}
		}
	}

	client = targ->client;

	if ( client ) {
		if ( client->noclip ) {
			return;
		}
	}

	if ( !dir ) {
		dflags |= DAMAGE_NO_KNOCKBACK;
	} else {
		VectorNormalize( dir );
	}

	knockback = damage;
	if ( knockback > 200 ) {
		knockback = 200;
	}
	if ( targ->flags & FL_NO_KNOCKBACK ) {
		knockback = 0;
	}
	if ( dflags & DAMAGE_NO_KNOCKBACK ) {
		knockback = 0;
	}

	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client && ( g_friendlyFire.integer || !OnSameTeam( targ, attacker ) || g_friendlyFireKnockback.integer ) ) {
		vec3_t kvel;
		float mass;

		mass = 200;

		if ( mod == MOD_LIGHTNING && !( ( level.time + targ->s.number * 50 ) % 400 ) ) {
			knockback = 60;
			dir[2] = 0.3;
		}

		VectorScale( dir, g_knockback.value * (float)knockback / mass, kvel );
		VectorAdd( targ->client->ps.velocity, kvel, targ->client->ps.velocity );

		if ( targ == attacker && !(  mod != MOD_ROCKET &&
									 mod != MOD_ROCKET_SPLASH &&
									 mod != MOD_GRENADE &&
									 mod != MOD_GRENADE_SPLASH &&
									 mod != MOD_DYNAMITE ) ) {
			targ->client->ps.velocity[2] *= 0.25;

		}

		// set the timer so that the other client can't cancel
		// out the movement immediately
		if ( !targ->client->ps.pm_time ) {
			int t;

			t = knockback * 2;
			if ( t < 50 ) {
				t = 50;
			}
			if ( t > 200 ) {
				t = 200;
			}
			targ->client->ps.pm_time = t;
			targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		}
	}

	// check for completely getting out of the damage
	if ( !( dflags & DAMAGE_NO_PROTECTION ) ) {

		// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
		// if the attacker was on the same team
		if ( targ != attacker && OnSameTeam( targ, attacker )  ) {
			if ( !g_friendlyFire.integer ) {
				return;
			}
		}

		// check for godmode
		if ( targ->flags & FL_GODMODE ) {
			return;
		}

		// RF, warzombie defense position is basically godmode for the time being
		if ( targ->flags & FL_DEFENSE_GUARD ) {
			return;
		}

		// check for invulnerability // (SA) moved from below so DAMAGE_NO_PROTECTION will still work
		if ( client && client->ps.powerups[PW_INVULNERABLE] ) { //----(SA)	added
			return;
		}

	}

	// battlesuit protects from all radius damage (but takes knockback)
	// and protects 50% against all damage
	if ( client && client->ps.powerups[PW_BATTLESUIT] ) {
		G_AddEvent( targ, EV_POWERUP_BATTLESUIT, 0 );
		if ( dflags & DAMAGE_RADIUS ) {
			return;
		}
		damage *= 0.5;
	}

	// add to the attacker's hit counter
	if ( attacker->client && targ != attacker && targ->health > 0 ) {
		if ( OnSameTeam( targ, attacker ) ) {
			attacker->client->ps.persistant[PERS_HITS] -= damage;
		} else {
			attacker->client->ps.persistant[PERS_HITS] += damage;
		}
	}

	if ( damage < 1 ) {
		damage = 1;
	}
	take = damage;
	save = 0;

	// save some from armor
	asave = CheckArmor( targ, take, dflags );
	take -= asave;

	if ( IsHeadShot( targ, qfalse, dir, point, mod ) ) {

		if ( take * 2 < g_headshotDmg.integer ) {
			if ( ( !( targ->client->ps.eFlags & EF_HEADSHOT ) ) && ( g_helmetProtects.integer ) ) {
				take = g_helmetDmg.integer;
			} else {
				take = g_headshotDmg.integer;
			}
		} else {
			if ( g_sniperWar.integer ) {
				take *= 20;     // sniper rifles destroy them
				if ( !OnSameTeam( attacker,targ ) ) {
					G_AddEvent( attacker, EV_GLOBAL_SOUND, G_SoundIndex( "sound/player/headshot.wav" ) );
					//	trap_SendServerCommand(-1, va("chat \"console: %.99s ^7killed %.99s ^7with a ^1HEADSHOT!^7\n\" 0", attacker->client->pers.netname, targ->client->pers.netname));
				}
			} else {
				take = 150;         // Headshot with sniper rifles DOES full kill


			}
		}

		// L0 FirstHeadShot
		if ( g_ShowHighlights.integer ) {
			FirstHeadshot( attacker, targ );
		} // L0 - end

		if ( !( targ->client->ps.eFlags & EF_HEADSHOT ) ) {  // only toss hat on first headshot
			G_AddEvent( targ, EV_LOSE_HAT, DirToByte( dir ) );
		}

		targ->client->ps.eFlags |= EF_HEADSHOT;
		isHeadshot = qtrue;

		if ( g_deathmatch.integer != 2 ) {
			if ( !OnSameTeam( attacker, targ ) ) {
				attacker->client->pers.headshots++;
				if ( g_gamestate.integer == GS_PLAYING ) {
					if ( attacker->client->pers.headshots > level.HAHshotnum ) {
						Q_strncpyz( level.HAheadhunters, "", sizeof( level.HAheadhunters ) );
						strcat( level.HAheadhunters, attacker->client->pers.netname );
						level.HAHshotnum = attacker->client->pers.headshots;
					} else if ( attacker->client->pers.headshots == level.HAHshotnum )     {
						strcat( level.HAheadhunters, "^7, " ); //Add a comma after the other person
						strcat( level.HAheadhunters, attacker->client->pers.netname ); //and add their name
					}
				}
			}
		} else {
			attacker->client->pers.headshots++;
			if ( g_gamestate.integer == GS_PLAYING ) {
				if ( attacker->client->pers.headshots > level.HAHshotnum ) {
					Q_strncpyz( level.HAheadhunters, "", sizeof( level.HAheadhunters ) );
					strcat( level.HAheadhunters, attacker->client->pers.netname );
					level.HAHshotnum = attacker->client->pers.headshots;
				} else if ( attacker->client->pers.headshots == level.HAHshotnum )     {
					strcat( level.HAheadhunters, "^7, " ); //Add a comma after the other person
					strcat( level.HAheadhunters, attacker->client->pers.netname ); //and add their name
				}
			}
		}



	} else {
		if ( ( targ && targ->client ) && ( g_headshotsOnly.integer != 0 ) && ( targ->client->ps.stats[STAT_HEALTH] > 0 ) ) {
			return;
		}
	}

	if ( g_instaGib.integer ) {
		take = 1000;
	}



	if ( g_debugDamage.integer ) {
		G_Printf( "client:%i health:%i damage:%i armor:%i\n", targ->s.number,
				  targ->health, take, asave );
	}

#ifdef GLOBALSTATS //let stats know somebody was damaged/hit
	stats_event_damageTaken( attacker,targ,take,mod );
	if ( attacker && attacker->client ) {
		stats_event_shotHit( attacker,targ,mod,take,isHeadshot );
	}
#endif
	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( client ) {
		if ( attacker ) {
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
		} else {
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}
		client->damage_armor += asave;
		client->damage_blood += take;

		client->damage_knockback += knockback;

		if ( dir ) {
			VectorCopy( dir, client->damage_from );
			client->damage_fromWorld = qfalse;
		} else {
			VectorCopy( targ->r.currentOrigin, client->damage_from );
			client->damage_fromWorld = qtrue;
		}
	}

	// See if it's the player hurting the emeny flag carrier
	Team_CheckHurtCarrier( targ, attacker );

	if ( targ->client ) {
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_mod = mod;

		//S4NDM4NN
		// Modify the damage for location damage
		//Added all the guns i think dont want this to work for grenades or w/e
		//even though this is sniper mod and you dont get anythign else :)
		if ( mod == MOD_SNIPERRIFLE ||
			 mod == MOD_SNOOPERSCOPE ||
			 mod == MOD_LUGER ||
			 mod == MOD_COLT ||
			 mod == MOD_AKIMBO ||
			 mod == MOD_MP40 ||
			 mod == MOD_THOMPSON ||
			 mod == MOD_STEN ||
			 mod == MOD_BAR ||
			 mod == MOD_FG42 ||
			 mod == MOD_FG42SCOPE ||
			 mod == MOD_MAUSER ||
			 mod == MOD_GARAND ||
			 mod == MOD_SNIPERRIFLE ||
			 mod == MOD_SNOOPERSCOPE ||
			 mod == MOD_SILENCER ||
			 mod == MOD_SNIPERRIFLE ) {
			if ( point && targ && targ->health > 0 && attacker && take ) {
				take = G_LocationDamage( point, targ, attacker, take );
			} else {
				targ->client->lasthurt_location = LOCATION_NONE;
			}
		}
		//END S4NDM4NN

	}

//S4NDM4NN-for damage taken/recieved
	//Dont really want it to count when you are gibbing
	//although it might count some anyway
	if ( ( attacker && attacker->client ) && ( targ && targ->client ) ) {

		if ( targ->client->ps.stats[STAT_HEALTH] > 0 ) {
			if ( g_deathmatch.integer == 2 ) {
				attacker->client->pers.dmgGiven += take;
				targ->client->pers.dmgreceived += take;
			} else {
				if ( !OnSameTeam( attacker, targ ) ) {
					attacker->client->pers.dmgGiven += take;
					targ->client->pers.dmgreceived += take;
				}
				//1/17/07	Agent	Added for team damage... this will reduce the door blockers who team flame... :)which is me lol
				if ( OnSameTeam( attacker,targ ) ) {
					attacker->client->pers.dmgTeam += take;
				}
			}
		}
	}
//End
//	Agent 1/8/2007
//	Dont play hitsounds for non "instant hit" weapons.  It gives away too much info when you hear it for a
//	grenade or your own mine :S.  I proabley didnt need 1002-1007, but oh well :)
	if ( ( mod != MOD_POISON_SYRINGE ) && ( mod != MOD_LAVA ) && ( mod != MOD_ARTILLERY ) && ( mod != MOD_AIRSTRIKE ) && ( mod != MOD_GRENADE )
		 && ( mod != MOD_GRENADE_SPLASH ) && ( mod != MOD_ROCKET ) && ( mod != MOD_ROCKET_SPLASH ) /*&& (mod != MOD_FLAMETHROWER) */
		 && ( mod != MOD_DYNAMITE ) && ( mod != MOD_GOOMBA ) && ( mod != MOD_THROWING_KNIFE ) && ( mod != 1004 ) && ( mod != 1005 )
		 && ( mod != MOD_ADMIN_SLAP ) && ( mod != MOD_LAVA ) && ( mod != MOD_ADMIN_KILL ) && ( mod != MOD_LANDMINE ) && ( mod != MOD_DYNAMITE_SPLASH )
		 && ( ( targ && targ->client ) && ( attacker && attacker->client ) ) && ( attacker != targ ) && ( g_allowhitsounds.integer ) ) {
		HitSounds( attacker,targ,isHeadshot );
	}




#ifdef OMNIBOT
	// need to trigger the explodes filtered for health before the damage ...
	if ( take && take > targ->health && targ->s.number >= MAX_CLIENTS && targ->health > 50 ) {
		if ( targ->spawnflags & 16 ) {
			if ( Q_stricmp( _GetEntityName( targ ), "" ) ) {
				Bot_Util_SendTrigger( targ, NULL, va( "Explode_%s Exploded.", _GetEntityName( targ ) ), "exploded" );
			} else {
				Bot_Util_SendTrigger( targ, NULL, va( "Explode_%d Exploded", targ->s.number ), "exploded" );
			}
		}
	}
#endif

// do the damage
	if ( take ) {
		targ->health = targ->health - take;

		// Ridah, can't gib with bullet weapons (except VENOM)
		if ( mod != MOD_VENOM && attacker == inflictor && targ->health <= GIB_HEALTH ) {
			if ( targ->aiCharacter != AICHAR_ZOMBIE ) { // zombie needs to be able to gib so we can kill him (although he doesn't actually GIB, he just dies)
				targ->health = GIB_HEALTH + 1;
			}
		}


// JPW NERVE overcome previous chunk of code for making grenades work again
		if ( ( g_gametype.integer != GT_SINGLE_PLAYER ) && ( take > 190 ) ) { // 190 is greater than 2x mauser headshot, so headshots don't gib
			targ->health = GIB_HEALTH - 1;
// jpw
		}
		//G_Printf("health at: %d\n", targ->health);
		if ( targ->health <= 0 ) {
			if ( client ) {
				if ( ( targ->health == GIB_HEALTH - 1 ) && ( !( targ->client->ps.pm_flags & PMF_LIMBO ) ) ) {
					targ->client->pers.gibbed = qtrue;
//Agent - i made it more shrubmod style where its _ALWAYS_ victem was gibbed/killed by attacker
					if ( g_deathmatch.integer != 2 ) {
						if ( !attacker->client ) {
							if ( g_gibReports.integer ) {
								trap_SendServerCommand( -1, va( "print \"%s ^7was gibbed\n\"", targ->client->pers.netname ) );
							}
						} else if ( attacker == targ )    {
							if ( g_autoTap.integer ) {
								targ->client->pers.isTapped = qfalse; //autotap
								targ->client->pers.spawnCyclesSkipped = 0;
							}
							if ( g_gibReports.integer ) {
								if ( targ->client->sess.gender == 0 ) {
									trap_SendServerCommand( -1, va( "print \"%s ^7gibbed himself\n\"", targ->client->pers.netname ) );
								} else {
									trap_SendServerCommand( -1, va( "print \"%s ^7gibbed herself\n\"", targ->client->pers.netname ) );
								}
							}
						} else {
							if ( !OnSameTeam( attacker, targ ) ) { //	*** ENEMY GIB
								if ( g_autoTap.integer ) {
									targ->client->pers.isTapped = qfalse; //autotap
									targ->client->pers.spawnCyclesSkipped = 0;
								}
								if ( g_gibReports.integer ) {
									trap_SendServerCommand( -1, va( "print \"%s ^7was gibbed by %s\n\"", targ->client->pers.netname, attacker->client->pers.netname ) );
								}
								attacker->client->pers.gibs++;
								if ( g_gibPoints.integer ) {
									attacker->client->ps.persistant[PERS_SCORE]++;
								}
								if ( g_gamestate.integer == GS_PLAYING ) {
									if ( attacker->client->pers.gibs > level.HAgibnum ) {
										Q_strncpyz( level.HAhighgibbers, "", sizeof( level.HAhighgibbers ) );
										strcat( level.HAhighgibbers, attacker->client->pers.netname );
										level.HAgibnum = attacker->client->pers.gibs;
									} else if ( attacker->client->pers.gibs == level.HAgibnum )     {
										strcat( level.HAhighgibbers, "^7, " ); //Add a comma after the other person
										strcat( level.HAhighgibbers, attacker->client->pers.netname ); //and add their name
									}
								}
							} else { // *** TEAM GIB
								if ( g_autoTap.integer ) {
									targ->client->pers.isTapped = qfalse; //autotap
									targ->client->pers.spawnCyclesSkipped = 0;
								}
								if ( g_gibReports.integer ) {
									trap_SendServerCommand( -1, va( "print \"%s ^3WAS GIBBED BY TEAMMATE ^7%s\n\"", targ->client->pers.netname, attacker->client->pers.netname ) );
								}
								attacker->client->pers.teamGibs++;
							}
						}
					}
				}
				targ->flags |= FL_NO_KNOCKBACK;
// JPW NERVE -- repeated shooting sends to limbo
				if ( g_gametype.integer >= GT_WOLF ) {
					if ( ( targ->health < FORCE_LIMBO_HEALTH ) && ( targ->health > GIB_HEALTH ) && ( !( targ->client->ps.pm_flags & PMF_LIMBO ) ) ) {
						limbo( targ, qtrue );
						if ( g_deathmatch.integer != 2 ) {
							if ( !attacker->client ) {
								if ( g_gibReports.integer ) {
									trap_SendServerCommand( -1, va( "print \"%s ^7was gibbed\n\"", targ->client->pers.netname ) );
								}
							} else if ( attacker == targ )    {
								if ( g_gibReports.integer ) {
									if ( targ->client->sess.gender == 0 ) {
										trap_SendServerCommand( -1, va( "print \"%s ^7gibbed himself\n\"", targ->client->pers.netname ) );
									} else {
										trap_SendServerCommand( -1, va( "print \"%s ^7gibbed herself\n\"", targ->client->pers.netname ) );
									}
								}
							} else {
								if ( !OnSameTeam( attacker, targ ) ) {
									if ( g_gibReports.integer ) {
										trap_SendServerCommand( -1, va( "print \"%s ^7was gibbed by %s\n\"", targ->client->pers.netname, attacker->client->pers.netname ) );
									}
									attacker->client->pers.gibs++;
									if ( g_gibPoints.integer ) {
										attacker->client->ps.persistant[PERS_SCORE]++;
									}
									if ( g_gamestate.integer == GS_PLAYING ) {
										if ( attacker->client->pers.gibs > level.HAgibnum ) {
											Q_strncpyz( level.HAhighgibbers, "", sizeof( level.HAhighgibbers ) );
											strcat( level.HAhighgibbers, attacker->client->pers.netname );
											level.HAgibnum = attacker->client->pers.gibs;
										} else if ( attacker->client->pers.gibs == level.HAgibnum )     {
											strcat( level.HAhighgibbers, "^7, " ); //Add a comma after the other person
											strcat( level.HAhighgibbers, attacker->client->pers.netname ); //and add their name
										}
									}
								} else {
									if ( g_gibReports.integer ) {
										trap_SendServerCommand( -1, va( "print \"%s^3 WAS GIBBED BY TEAMMATE^7 %s\n\"", targ->client->pers.netname, attacker->client->pers.netname ) );
									}
									attacker->client->pers.teamGibs++;
								}
							}
						}
					}
				}
			}
			if ( targ->health < -999 ) {
				targ->health = -999;
			}

			targ->enemy = attacker;
			if ( targ->die ) { // Ridah, mg42 doesn't have die func (FIXME)
				targ->die( targ, inflictor, attacker, take, mod );
			}

			// if we freed ourselves in death function
			if ( !targ->inuse ) {
				return;
			}

			// RF, entity scripting
			if ( targ->s.number >= MAX_CLIENTS && targ->health <= 0 ) { // might have revived itself in death function
				G_Script_ScriptEvent( targ, "death", "" );
			}

		} else if ( targ->pain ) {
			if ( dir ) {  // Ridah, had to add this to fix NULL dir crash
				VectorCopy( dir, targ->rotate );
				VectorCopy( point, targ->pos3 ); // this will pass loc of hit
			} else {
				VectorClear( targ->rotate );
				VectorClear( targ->pos3 );
			}

			targ->pain( targ, attacker, take, point );

			// RF, entity scripting
			if ( targ->s.number >= MAX_CLIENTS ) {
				G_Script_ScriptEvent( targ, "pain", va( "%d %d", targ->health, targ->health + take ) );
			}
		}

		//G_ArmorDamage(targ);	//----(SA)	moved out to separate routine

		// Ridah, this needs to be done last, incase the health is altered in one of the event calls
		if ( targ->client ) {
			targ->client->ps.stats[STAT_HEALTH] = targ->health;
		}
		if ( damage > 0 ) {   // MHC - new method for calculating hits...if any damage is given its a hit
			if ( LogAccuracyHit( targ, attacker ) ) {
				attacker->client->ps.persistant[PERS_ACCURACY_HITS]++;
				if ( g_deathmatch.integer != 2 ) {
					if ( ( OnSameTeam( attacker,targ ) ) && ( attacker != targ ) ) {
						attacker->client->pers.acc_teamHits++;
					} else {
						attacker->client->pers.acc_hits++;
						attacker->client->pers.life_hits++;
					}
				}
			}
			if ( g_deathmatch.integer == 2 ) { // This may cause a crash in the future...
				attacker->client->pers.acc_hits++;
				attacker->client->pers.life_hits++;
			}
		}

#ifdef OMNIBOT
		if ( targ->s.number < MAX_CLIENTS ) {
			// notify omni-bot framework
			Bot_Event_TakeDamage( targ - g_entities, attacker );
		}
#endif // OMNIBOT
	}

}


/*

void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
               vec3_t dir, vec3_t point, int damage, int dflags, int mod ) {
    gclient_t	*client;
    int			take;
    int			save;
    int			asave;
    int			knockback;

    if (!targ->takedamage) {
        return;
    }

    // the intermission has allready been qualified for, so don't
    // allow any extra scoring
    if ( level.intermissionQueued || g_gamestate.integer != GS_PLAYING ) {
        if ((!g_warmupDamage.integer) || ((g_gamestate.integer != GS_WARMUP) && (g_gamestate.integer != GS_WARMUP_COUNTDOWN)))
  return;
    }

    if ( !inflictor ) {
        inflictor = &g_entities[ENTITYNUM_WORLD];
    }
    if ( !attacker ) {
        attacker = &g_entities[ENTITYNUM_WORLD];
    }

// JPW NERVE
    if ((targ->waterlevel >= 3) && (mod == MOD_FLAMETHROWER))
        return;
// jpw

    // shootable doors / buttons don't actually have any health
    if ( targ->s.eType == ET_MOVER && !(targ->aiName) && !(targ->isProp) && !targ->scriptName) {
        if ( targ->use && targ->moverState == MOVER_POS1 ) {
            targ->use( targ, inflictor, attacker );
        }
        return;
    }

    if ( targ->s.eType == ET_MOVER && targ->aiName && !(targ->isProp) && !targ->scriptName)
    {
        switch (mod) {
        case MOD_GRENADE:
        case MOD_GRENADE_SPLASH:
        case MOD_ROCKET:
        case MOD_ROCKET_SPLASH:
            break;
        default:
            return;	// no damage from other weapons
        }
    }
    else if ( targ->s.eType == ET_EXPLOSIVE )
    {
        // 32 Explosive
        // 64 Dynamite only
        if ((targ->spawnflags & 32) || (targ->spawnflags & 64))
        {
            switch (mod) {
            case MOD_GRENADE:
            case MOD_GRENADE_SPLASH:
            case MOD_ROCKET:
            case MOD_ROCKET_SPLASH:
            case MOD_AIRSTRIKE:
            case MOD_GRENADE_PINEAPPLE:
            case MOD_MORTAR:
            case MOD_MORTAR_SPLASH:
            case MOD_EXPLOSIVE:
                if (targ->spawnflags & 64)
                    return;

                break;

            case MOD_DYNAMITE:
            case MOD_DYNAMITE_SPLASH:
                break;

            default:
                return;
            }
        }
    }

    client = targ->client;

    if ( client ) {
        if ( client->noclip ) {
            return;
        }
    }

    if ( !dir ) {
        dflags |= DAMAGE_NO_KNOCKBACK;
    } else {
        VectorNormalize(dir);
    }

    knockback = damage;
    if ( knockback > 200 ) {
        knockback = 200;
    }
    if ( targ->flags & FL_NO_KNOCKBACK ) {
        knockback = 0;
    }
    if ( dflags & DAMAGE_NO_KNOCKBACK ) {
        knockback = 0;
    }

    // figure momentum add, even if the damage won't be taken
    if ( knockback && targ->client && (g_friendlyFire.integer || !OnSameTeam(targ, attacker)) ) {
        vec3_t	kvel;
        float	mass;

        mass = 200;

        if (mod == MOD_LIGHTNING && !((level.time+targ->s.number*50)%400)) {
            knockback = 60;
            dir[2] = 0.3;
        }

        VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
        VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

        if (targ == attacker && !(	mod != MOD_ROCKET &&
                                    mod != MOD_ROCKET_SPLASH &&
                                    mod != MOD_GRENADE &&
                                    mod != MOD_GRENADE_SPLASH &&
                                    mod != MOD_DYNAMITE))
        {
            targ->client->ps.velocity[2] *= 0.25;
        }

        // set the timer so that the other client can't cancel
        // out the movement immediately
        if ( !targ->client->ps.pm_time ) {
            int		t;

            t = knockback * 2;
            if ( t < 50 ) {
                t = 50;
            }
            if ( t > 200 ) {
                t = 200;
            }
            targ->client->ps.pm_time = t;
            targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
        }
    }

    // check for completely getting out of the damage
    if ( !(dflags & DAMAGE_NO_PROTECTION) ) {

        // if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
        // if the attacker was on the same team
        if ( targ != attacker && OnSameTeam (targ, attacker)  ) {
            if ( !g_friendlyFire.integer ) {
                return;
            }
        }

        // check for godmode
        if ( targ->flags & FL_GODMODE ) {
            return;
        }

        // RF, warzombie defense position is basically godmode for the time being
        if ( targ->flags & FL_DEFENSE_GUARD ) {
            return;
        }

        // check for invulnerability // (SA) moved from below so DAMAGE_NO_PROTECTION will still work
        if ( client && client->ps.powerups[PW_INVULNERABLE] ) {	//----(SA)	added
            return;
        }

    }

    // battlesuit protects from all radius damage (but takes knockback)
    // and protects 50% against all damage
    if ( client && client->ps.powerups[PW_BATTLESUIT] ) {
        G_AddEvent( targ, EV_POWERUP_BATTLESUIT, 0 );
        if ( dflags & DAMAGE_RADIUS ) {
            return;
        }
        damage *= 0.5;
    }

    // add to the attacker's hit counter
    if ( attacker->client && targ != attacker && targ->health > 0 ) {
        if ( OnSameTeam( targ, attacker ) ) {
            attacker->client->ps.persistant[PERS_HITS] -= damage;
        } else {
            attacker->client->ps.persistant[PERS_HITS] += damage;
        }
    }

    if ( damage < 1 ) {
        damage = 1;
    }
    take = damage;
    save = 0;

    // save some from armor
    asave = CheckArmor (targ, take, dflags);
    take -= asave;


    if ( IsHeadShot(targ, qfalse, dir, point, mod) ) {
        if (take*2 < g_headshotDmg.integer)	// head shots, all weapons, do minimum 50 points damage
        {

            if((!(targ->client->ps.eFlags & EF_HEADSHOT)) && (g_helmetProtects.integer))	//BOTS TW - added cvar check
                take = g_helmetDmg.integer;		//BOTS TW - added customizable damage via cvar
            else

                take = g_headshotDmg.integer;	//will do 50 damage if no helmet, or if g_helmetProtects is 0
        }
        else
         take *= 2; // sniper rifles can do full-kill (and knock into limbo)
        //mal: end of helmet code.


        if(!(targ->client->ps.eFlags & EF_HEADSHOT))	// only toss hat on first headshot
            G_AddEvent( targ, EV_LOSE_HAT, DirToByte(dir) );

        targ->client->ps.eFlags |= EF_HEADSHOT;
            if (!OnSameTeam(attacker, targ)){
            attacker->client->pers.headshots++;

 }

    if ( g_debugDamage.integer ) {
        G_Printf( "client:%i health:%i damage:%i armor:%i\n", targ->s.number,
            targ->health, take, asave );
    }

    // add to the damage inflicted on a player this frame
    // the total will be turned into screen blends and view angle kicks
    // at the end of the frame
    if ( client ) {
        if ( attacker ) {
            client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
        } else {
            client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
        }
        client->damage_armor += asave;
        client->damage_blood += take;
        client->damage_knockback += knockback;

        if ( dir ) {
            VectorCopy ( dir, client->damage_from );
            client->damage_fromWorld = qfalse;
        } else {
            VectorCopy ( targ->r.currentOrigin, client->damage_from );
            client->damage_fromWorld = qtrue;
        }
    }

    // See if it's the player hurting the emeny flag carrier
    Team_CheckHurtCarrier(targ, attacker);

    if (targ->client) {
        // set the last client who damaged the target
        targ->client->lasthurt_client = attacker->s.number;
        targ->client->lasthurt_mod = mod;
    }

    if (take)
    {
        targ->health = targ->health - take;

        // Ridah, can't gib with bullet weapons (except VENOM)
        if ( mod != MOD_VENOM && attacker == inflictor && targ->health <= GIB_HEALTH) {
            if (targ->aiCharacter != AICHAR_ZOMBIE)	// zombie needs to be able to gib so we can kill him (although he doesn't actually GIB, he just dies)
                targ->health= GIB_HEALTH+1;
        }

// JPW NERVE overcome previous chunk of code for making grenades work again
        if ((g_gametype.integer != GT_SINGLE_PLAYER) && (take > 190)) // 190 is greater than 2x mauser headshot, so headshots don't gib
        {
            targ->health = GIB_HEALTH - 1;
// jpw
            }
        //G_Printf("health at: %d\n", targ->health);
        if ( targ->health <= 0 )
        {
            if ( client )
            {
   if ((targ->health == GIB_HEALTH - 1) && (!(targ->client->ps.pm_flags & PMF_LIMBO))){
   if (!attacker->client)
            {
                if (g_gibReports.integer)
                    trap_SendServerCommand(-1, va("print \"%s^7 was gibbed.\n\"", targ->client->pers.netname));
            }
            else if (attacker == targ)
            {
                if (g_gibReports.integer)
                    trap_SendServerCommand(-1, va("print \"%s^7 gibbed himself.\n\"", targ->client->pers.netname));
            }
            else
            {
                if (!OnSameTeam(attacker, targ))
                {
                    if (g_gibReports.integer)
                        trap_SendServerCommand(-1, va("print \"%s^7 gibbed %s^7.\n\"", attacker->client->pers.netname, targ->client->pers.netname));

                    attacker->client->pers.gibs++;
                }
                else
                {
                    if (g_gibReports.integer)
                        trap_SendServerCommand(-1, va("print \"%s^3 was gibbed by ^1TEAMMATE^7 %s.\n\"", targ->client->pers.netname, attacker->client->pers.netname));
                 attacker->client->pers.teamGibs++;
                 }

                }
            }
                targ->flags |= FL_NO_KNOCKBACK;
// JPW NERVE -- repeated shooting sends to limbo
                    if (g_gametype.integer >= GT_WOLF)
                    {
                        if ((targ->health < FORCE_LIMBO_HEALTH) && (targ->health > GIB_HEALTH) && (!(targ->client->ps.pm_flags & PMF_LIMBO)))
                        {
                            limbo(targ, qtrue);
// jpw


                            if (!attacker->client)
                            {
                                if (g_gibReports.integer)
                                    trap_SendServerCommand(-1, va("print \"%s^7 was gibbed.\n\"", targ->client->pers.netname));
                            }
                            else if (attacker == targ)
                            {
                                if (g_gibReports.integer)
                                    trap_SendServerCommand(-1, va("print \"%s^7 gibbed himself.\n\"", targ->client->pers.netname));
                            }
                            else
                            {
                                if (!OnSameTeam(attacker, targ))
                                {
                                    if (g_gibReports.integer)
                                        trap_SendServerCommand(-1, va("print \"%s^7 gibbed %s^7.\n\"", attacker->client->pers.netname, targ->client->pers.netname));

                                    attacker->client->pers.gibs++;
                                }
                                else
                                {
                                    if (g_gibReports.integer)
                                        trap_SendServerCommand(-1, va("print \"%s^3 was gibbed by ^1TEAMMATE^7 %s\n\"", targ->client->pers.netname, attacker->client->pers.netname));

                                    attacker->client->pers.teamGibs++;
                                }
                            }

                        }
                    }
            }

            if (targ->health < -999)
                targ->health = -999;

            targ->enemy = attacker;
            if (targ->die)	// Ridah, mg42 doesn't have die func (FIXME)
                targ->die (targ, inflictor, attacker, take, mod);

            // if we freed ourselves in death function
            if (!targ->inuse)
                return;

            // RF, entity scripting
            if (targ->s.number >= MAX_CLIENTS && targ->health <= 0)	// might have revived itself in death function
                G_Script_ScriptEvent( targ, "death", "" );

        } else if ( targ->pain ) {
            if (dir) {	// Ridah, had to add this to fix NULL dir crash
                VectorCopy (dir, targ->rotate);
                VectorCopy (point, targ->pos3); // this will pass loc of hit
            } else {
                VectorClear( targ->rotate );
                VectorClear( targ->pos3 );
            }

            targ->pain (targ, attacker, take, point);

            // RF, entity scripting
            if (targ->s.number >= MAX_CLIENTS)
                G_Script_ScriptEvent( targ, "pain", va("%d %d", targ->health, targ->health+take) );
        }

        //G_ArmorDamage(targ);	//----(SA)	moved out to separate routine

        // Ridah, this needs to be done last, incase the health is altered in one of the event calls
        if ( targ->client ) {
            targ->client->ps.stats[STAT_HEALTH] = targ->health;
        }
    }

}

  ///////////////////////////////////////////////////////////////////////////////
    if (take) {
        targ->health = targ->health - take;

        // Ridah, can't gib with bullet weapons (except VENOM)
        if ( mod != MOD_VENOM && attacker == inflictor && targ->health <= GIB_HEALTH) {
            if (targ->aiCharacter != AICHAR_ZOMBIE)	// zombie needs to be able to gib so we can kill him (although he doesn't actually GIB, he just dies)
                targ->health= GIB_HEALTH+1;
        }


// JPW NERVE overcome previous chunk of code for making grenades work again
        if ((g_gametype.integer != GT_SINGLE_PLAYER) && (take > 190)) // 190 is greater than 2x mauser headshot, so headshots don't gib
        {
            targ->health = GIB_HEALTH - 1;
// jpw
            }
        //G_Printf("health at: %d\n", targ->health);
        if ( targ->health <= 0 )
        {
            if ( client )
            {
   if ((targ->health == GIB_HEALTH - 1) && (!(targ->client->ps.pm_flags & PMF_LIMBO))){
   if (!attacker->client)
            {
                if (g_gibReports.integer)
                    trap_SendServerCommand(-1, va("print \"%s^7 was gibbed.\n\"", targ->client->pers.netname));
            }
            else if (attacker == targ)
            {
                if (g_gibReports.integer)
                    trap_SendServerCommand(-1, va("print \"%s^7 gibbed himself.\n\"", targ->client->pers.netname));
            }
            else
            {
                if (!OnSameTeam(attacker, targ))
                {
                    if (g_gibReports.integer)
                        trap_SendServerCommand(-1, va("print \"%s^7 gibbed %s^7.\n\"", attacker->client->pers.netname, targ->client->pers.netname));

                    attacker->client->pers.gibs++;
                }
                else
                {
                    if (g_gibReports.integer)
                        trap_SendServerCommand(-1, va("print \"%s^3 was gibbed by ^1TEAMMATE^7 %s.\n\"", targ->client->pers.netname, attacker->client->pers.netname));
                 attacker->client->pers.teamGibs++;
                 }

                }
            }
                targ->flags |= FL_NO_KNOCKBACK;
// JPW NERVE -- repeated shooting sends to limbo
                    if (g_gametype.integer >= GT_WOLF)
                    {
                        if ((targ->health < FORCE_LIMBO_HEALTH) && (targ->health > GIB_HEALTH) && (!(targ->client->ps.pm_flags & PMF_LIMBO)))
                        {
                            limbo(targ, qtrue);
// jpw


                            if (!attacker->client)
                            {
                                if (g_gibReports.integer)
                                    trap_SendServerCommand(-1, va("print \"%s^7 was gibbed.\n\"", targ->client->pers.netname));
                            }
                            else if (attacker == targ)
                            {
                                if (g_gibReports.integer)
                                    trap_SendServerCommand(-1, va("print \"%s^7 gibbed himself.\n\"", targ->client->pers.netname));
                            }
                            else
                            {
                                if (!OnSameTeam(attacker, targ))
                                {
                                    if (g_gibReports.integer)
                                        trap_SendServerCommand(-1, va("print \"%s^7 gibbed %s^7.\n\"", attacker->client->pers.netname, targ->client->pers.netname));

                                    attacker->client->pers.gibs++;
                                }
                                else
                                {
                                    if (g_gibReports.integer)
                                        trap_SendServerCommand(-1, va("print \"%s^3 was gibbed by ^1TEAMMATE^7 %s\n\"", targ->client->pers.netname, attacker->client->pers.netname));

                                    attacker->client->pers.teamGibs++;
                                }
                            }

                        }
                    }
            }

            if (targ->health < -999)
                targ->health = -999;

            targ->enemy = attacker;
            if (targ->die)	// Ridah, mg42 doesn't have die func (FIXME)
                targ->die (targ, inflictor, attacker, take, mod);

            // if we freed ourselves in death function
            if (!targ->inuse)
                return;

            // RF, entity scripting
            if (targ->s.number >= MAX_CLIENTS && targ->health <= 0)	// might have revived itself in death function
                G_Script_ScriptEvent( targ, "death", "" );

        } else if ( targ->pain ) {
            if (dir) {	// Ridah, had to add this to fix NULL dir crash
                VectorCopy (dir, targ->rotate);
                VectorCopy (point, targ->pos3); // this will pass loc of hit
            } else {
                VectorClear( targ->rotate );
                VectorClear( targ->pos3 );
            }

            targ->pain (targ, attacker, take, point);

            // RF, entity scripting
            if (targ->s.number >= MAX_CLIENTS)
                G_Script_ScriptEvent( targ, "pain", va("%d %d", targ->health, targ->health+take) );
        }

        //G_ArmorDamage(targ);	//----(SA)	moved out to separate routine

        // Ridah, this needs to be done last, incase the health is altered in one of the event calls
        if ( targ->client ) {
            targ->client->ps.stats[STAT_HEALTH] = targ->health;
        }
    }

}

*/

/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*//*
qboolean CanDamage (gentity_t *targ, vec3_t origin) {
    vec3_t	dest;
    trace_t	tr;
    vec3_t	midpoint;

    // use the midpoint of the bounds instead of the origin, because
    // bmodels may have their origin is 0,0,0
    VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
    VectorScale (midpoint, 0.5, midpoint);

    VectorCopy (midpoint, dest);
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;



    if(&g_entities[tr.entityNum] == targ)
        return qtrue;

    // this should probably check in the plane of projection,
    // rather than in world coordinate, and also include Z
    VectorCopy (midpoint, dest);
    dest[0] += 15.0;
    dest[1] += 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] += 15.0;
    dest[1] -= 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] -= 15.0;
    dest[1] += 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;

    VectorCopy (midpoint, dest);
    dest[0] -= 15.0;
    dest[1] -= 15.0;
    trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
    if (tr.fraction == 1.0)
        return qtrue;


    return qfalse;
}*/
/*
============
CanDamage ET version
============
*/
//#define MASK_CAN_DAMAGE		(CONTENTS_SOLID | CONTENTS_BODY)		// 6/14/08 - Martin Greades dont kill through other players
#define MASK_CAN_DAMAGE     CONTENTS_SOLID                              // New version..just contense_solid
qboolean CanDamage( gentity_t *targ, vec3_t origin ) {
	vec3_t dest;
	trace_t tr;
	vec3_t midpoint;
	vec3_t offsetmins = { -16.f, -16.f, -16.f };
	vec3_t offsetmaxs = { 16.f, 16.f, 16.f };

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	// Gordon: well, um, just check then...
	if ( targ->r.currentOrigin[0] || targ->r.currentOrigin[1] || targ->r.currentOrigin[2] ) {
		VectorCopy( targ->r.currentOrigin, midpoint );

		if ( targ->s.eType == ET_MOVER ) {
			midpoint[2] += 32;
		}
	} else {
		VectorAdd( targ->r.absmin, targ->r.absmax, midpoint );
		VectorScale( midpoint, 0.5, midpoint );
	}

//	G_RailTrail( origin, dest );

	trap_Trace( &tr, origin, vec3_origin, vec3_origin, midpoint, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1.0 ) {
		return qtrue;
	}

	if ( &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	if ( targ->client ) {
		VectorCopy( targ->client->ps.mins, offsetmins );
		VectorCopy( targ->client->ps.maxs, offsetmaxs );
	}

	// this should probably check in the plane of projection,
	// rather than in world coordinate
	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmaxs[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	// =========================

	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmaxs[0];
	dest[1] += offsetmins[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmaxs[1];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	VectorCopy( midpoint, dest );
	dest[0] += offsetmins[0];
	dest[1] += offsetmins[2];
	dest[2] += offsetmins[2];
	trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_CAN_DAMAGE );
	if ( tr.fraction == 1 || &g_entities[tr.entityNum] == targ ) {
		return qtrue;
	}

	return qfalse;
}
/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage( vec3_t origin, gentity_t *attacker, float damage, float radius,
						 gentity_t *ignore, int mod ) {
	float points, dist;
	gentity_t   *ent;
	int entityList[MAX_GENTITIES];
	int numListedEntities;
	vec3_t mins, maxs;
	vec3_t v;
	vec3_t dir;
	int i, e;
	qboolean hitClient = qfalse;
// JPW NERVE
	float boxradius;
	vec3_t dest;
	trace_t tr;
	vec3_t midpoint;
// jpw


	if ( radius < 1 ) {
		radius = 1;
	}

	boxradius = 1.41421356 * radius; // radius * sqrt(2) for bounding box enlargement --
	// bounding box was checking against radius / sqrt(2) if collision is along box plane
	for ( i = 0 ; i < 3 ; i++ ) {
		mins[i] = origin[i] - boxradius; // JPW NERVE
		maxs[i] = origin[i] + boxradius; // JPW NERVE
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( e = 0 ; e < numListedEntities ; e++ ) {
		ent = &g_entities[entityList[ e ]];

		if ( ent == ignore ) {
			continue;
		}
		if ( !ent->takedamage ) {
			continue;
		}

/* JPW NERVE -- we can put this back if we need to, but it kinna sucks for human-sized bboxes
        // find the distance from the edge of the bounding box
        for ( i = 0 ; i < 3 ; i++ ) {
            if ( origin[i] < ent->r.absmin[i] ) {
                v[i] = ent->r.absmin[i] - origin[i];
            } else if ( origin[i] > ent->r.absmax[i] ) {
                v[i] = origin[i] - ent->r.absmax[i];
            } else {
                v[i] = 0;
            }
        }
*/
// JPW NERVE
		if ( !ent->r.bmodel ) {
			VectorSubtract( ent->r.currentOrigin,origin,v ); // JPW NERVE simpler centroid check that doesn't have box alignment weirdness
		} else {
			for ( i = 0 ; i < 3 ; i++ ) {
				if ( origin[i] < ent->r.absmin[i] ) {
					v[i] = ent->r.absmin[i] - origin[i];
				} else if ( origin[i] > ent->r.absmax[i] ) {
					v[i] = origin[i] - ent->r.absmax[i];
				} else {
					v[i] = 0;
				}
			}
		}
// jpw
		dist = VectorLength( v );
		if ( dist >= radius ) {
			continue;
		}

		points = damage * ( 1.0 - dist / radius );

// JPW NERVE -- different radiusdmg behavior for MP -- big explosions should do less damage (over less distance) through failed traces
		if ( CanDamage( ent, origin ) ) {
			if ( LogAccuracyHit( ent, attacker ) ) {
				hitClient = qtrue;
			}
			VectorSubtract( ent->r.currentOrigin, origin, dir );
			// push the center of mass higher than the origin so players
			// get knocked into the air more
			dir[2] += 24;
			G_Damage( ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS, mod );
		}
// JPW NERVE --  MP weapons should do 1/8 damage through walls over 1/8th distance
		else {
			if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
				VectorAdd( ent->r.absmin, ent->r.absmax, midpoint );
				VectorScale( midpoint, 0.5, midpoint );
				VectorCopy( midpoint, dest );

				trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
				if ( tr.fraction < 1.0 ) {
					VectorSubtract( dest,origin,dest );
					dist = VectorLength( dest );
					if ( dist < radius * 0.2f ) { // closer than 1/4 dist
						if ( LogAccuracyHit( ent, attacker ) ) {
							hitClient = qtrue;
						}
						VectorSubtract( ent->r.currentOrigin, origin, dir );
						dir[2] += 24;
						G_Damage( ent, NULL, attacker, dir, origin, (int)( points * 0.1f ), DAMAGE_RADIUS, mod );
					}
				}
			}
		}
// jpw
	}
	return hitClient;
}

