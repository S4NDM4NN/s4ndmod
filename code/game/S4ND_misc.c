#include "g_local.h"
#include "S4ND_local.h"
/*==================================================
L0 25.02/09
I've created this file so i can start putting stuff
here and link it thru out the system.
It's so stuff can be traced easier.
Although i'm still using S4ND_local file to declare
functions =)
==================================================*/

/* First Blood */
/*===================================================
It's moved here as I will add functionality as well.
1 = spam only, 2 = in case of tied game decide vinner
by team that draw first blood..
NOTE: I've rip it off from NQ mod :$
NOTE2: DONE!
- it only kicks in on maps like mp_depot if it's set
to 2, it ignores CTF maps, tho note that playing ie
edited SP map in gametype 5 with flag and FirstBlood
cvar set to 2 will kick in since game is tied till
time for holding the flag is reached so never on 2..
===================================================*/

void FirstBlood( gentity_t *self, gentity_t *attacker ) {
	qboolean onSameTeam = OnSameTeam( self, attacker );
	gentity_t *te;

	// Spam is visible if it's enabled...
	if ( g_FirstBlood.integer ) {

		if ( !firstblood &&
			 self &&
			 self->client &&
			 attacker &&
			 attacker->client &&
			 attacker->s.number != ENTITYNUM_NONE &&
			 attacker->s.number != ENTITYNUM_WORLD &&
			 attacker != self &&
			 g_gamestate.integer == GS_PLAYING &&
			 !onSameTeam ) {

			/*	trap_SendServerCommand( -1, va("chat \"%s ^7drew ^1FIRST BLOOD ^7from %s\"", attacker->client->pers.netname, self->client->pers.netname));
			    G_AddEvent (self, EV_GLOBAL_SOUND, G_SoundIndex("sound/player/firstblood.wav"));	// Martin - Need to do it twice because client doenst play it first time :S
			    G_AddEvent (self, EV_GLOBAL_SOUND, G_SoundIndex("sound/player/firstblood.wav"));
			    firstblood = qtrue;
			*/
			// L0 - This one works just fine :)
			trap_SendServerCommand( -1, va( "chat \"%s ^7drew ^1FIRST BLOOD ^7from ^7%s\"", attacker->client->pers.netname, self->client->pers.netname ) );
			te = G_TempEntity( attacker->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/firstblood.wav" );
			te->r.svFlags |= SVF_BROADCAST;
			firstblood = qtrue;
		}
	}
}

/* First Headshot */
/*===============
Just an eye candy
===============*/
void FirstHeadshot( gentity_t *attacker, gentity_t *targ ) {
	qboolean onSameTeam = OnSameTeam( targ, attacker );
	gentity_t *te;


	if ( !firstheadshot &&
		 targ &&
		 targ->client &&
		 attacker &&
		 attacker->client &&
		 attacker->s.number != ENTITYNUM_NONE &&
		 attacker->s.number != ENTITYNUM_WORLD &&
		 attacker != targ &&
		 g_gamestate.integer == GS_PLAYING &&
		 !onSameTeam ) {
		te = G_TempEntity( attacker->s.pos.trBase, EV_GLOBAL_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/player/firstheadshot.wav" );        // If i forget remind me to send you the sounds for stuff added in this file..
		te->r.svFlags |= SVF_BROADCAST;

		trap_SendServerCommand( -1, va( "chat \"%s ^7blew out %s^7's brains with the ^3FIRST HEAD SHOT^7!\"", attacker->client->pers.netname, targ->client->pers.netname ) );
		firstheadshot = qtrue;
	}
}

/* Double Kills */
/*======================================
Spams for double/triple and quad kills
It only spams if kill is made in certain
time frame to avoid mass spamming...
NOTE: 4.03/09 FIXED - added MODs check
======================================*/
void DoubleKills( gentity_t *ent, int meansOfDeath ) {
	char    *message;
	gentity_t *te;

	if ( !ent || !ent->client ) {
		return;
	}

	// Some ppl may want to run multi kills but will hate this so new cvar for it...
	if ( g_doubleKills.integer ) {

		message = "";

		// Not all are used, they are just here so they can be uncommented if you decide you want to use it at any point.
		// I will just use the most common ones for the sake of long ars scan on any potential double... kills :0)
		if ( meansOfDeath == MOD_LUGER // handgun
			 || meansOfDeath == MOD_COLT // handgun
		     //	|| meansOfDeath == MOD_KNIFE2 // knife  -- impossible with knife only unless if poison hits in..
		     //	|| meansOfDeath == MOD_KNIFE  // knife  -- - || -
			 || meansOfDeath == MOD_KNIFE_STEALTH // knife -- can be done :)
		     //	|| meansOfDeath == MOD_MACHINEGUN // MG42  -- bleh...
			 || meansOfDeath == MOD_THOMPSON // Thompson
			 || meansOfDeath == MOD_MP40 // MP40
			 || meansOfDeath == MOD_STEN // STEN
			 || meansOfDeath == MOD_MAUSER // Mauser  -- due instant gib/in goldrush mode
		     //	|| meansOfDeath == MOD_SNIPERRIFLE // Sniper Rifle -- basically impossible
			 || meansOfDeath == MOD_POISON_SYRINGE // Poison -- does happen while poisoning one and shoting/stabbing(stealth only atm) another one =)
		     //	|| meansOfDeath == MOD_VENOM		// venom  -- to easy tbh..
		     //	|| meansOfDeath == MOD_FLAMETHROWER  // flamethrower  -- why use it anyway...
			 ) {

			if ( ( level.time - ent->client->lastKillTime ) > 1000 ) {
				ent->client->doublekill = 0;
				ent->client->lastKillTime = level.time;
				return;
			} else {
				ent->client->doublekill++;
				ent->client->lastKillTime = level.time;
			}

			// EDIT: I think i should add smth like "nextKill" so it checks in waves for each kill
			// if it came one after another and not if all kills came inside "doubleKill"...?
			// Although it does get stuck in loop of kills "if double- then tripple, then if tripple is quad etc.." but could be done better as accidents tend to happen...
			// Well that's smth i'll do on next launch break or so... so:  traceMark

			switch ( ent->client->doublekill ) {
			case 1: // 2 kills
				message = "Double kill!";
				te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/player/doublekill.wav" );
				te->r.svFlags |= SVF_BROADCAST;
				break;
			case 2: // 3 kills
				message = "Triple kill!";
				te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/player/tripplekill.wav" );
				te->r.svFlags |= SVF_BROADCAST;
				break;
			case 3: // 4 kills
				message = "Pure Ownage!";
				te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/player/oneandonly.wav" );
				te->r.svFlags |= SVF_BROADCAST;
				break;
			}

			if ( ent->client->doublekill == 1 ) {
				trap_SendServerCommand( -1, va( "chat \"^3%s ^7%s\n\"", message, ent->client->pers.netname ) );
			}

			// to avoid spamming the 3rd kill is printed to console only...
			if ( ent->client->doublekill == 2 ) {
				trap_SendServerCommand( -1, va( "print \"^3%s ^7%s\n\"", message, ent->client->pers.netname ) );
			}

		}
		// 4 kills -ownage needs to be seen *twisted* so yes, back to screen >:)
		if ( ent->client->doublekill == 3 ) {
			trap_SendServerCommand( -1, va( "chat \"^3%s ^7was brought to you by %s\n\"", message, ent->client->pers.netname ) );
		}
	}
}


/*Death Sprees */
/*======================================================
If player dies for 10, 15, 20, 25 times without scoring
at least one kill let every one know about it :P
Scoring a kill resets the count- enemy kill tho not tk..
======================================================*/
void deathSpree( gentity_t *ent ) {

	int deaths = ent->client->pers.deathSpree;
	gentity_t *te;

	// Since it's not annoying (unless if you're the one making the news lol)
	// and doesn't happen often, let us use existing cvar for it :)
	if ( g_ShowHighlights.integer ) {

		if ( deaths <= 0 ) {
			return;
		}

		// if they have stuff for us start spaming >=)
		if ( deaths == 10 ) {
			trap_SendServerCommand( -1,
									va( "chat \"^0DEATHSPREE! ^7%s ^7refuses to defend him self with ^010 ^7deaths without a kill^0!\n\"",
										ent->client->pers.netname ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/dSpree1.wav" );
			te->r.svFlags |= SVF_BROADCAST;
		}

		if ( deaths == 15 ) {
			trap_SendServerCommand( -1,
									va( "chat \"^0DEATHSPREE! ^7%s ^7is raising a bar with ^015 ^7deaths without a kill^0!\n\"",
										ent->client->pers.netname ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/dSpree2.wav" );
			te->r.svFlags |= SVF_BROADCAST;
		}

		if ( deaths == 20 ) {
			trap_SendServerCommand( -1,
									va( "chat \"^0DEATHSPREE! ^7%s ^7is having a hard time with ^020 ^7deaths without a kill^0!\n\"",
										ent->client->pers.netname ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/dSpree3.wav" );
			te->r.svFlags |= SVF_BROADCAST;
		}
		// 25 deaths is maximum.. seriously- i mean c'mon, 25? If they pass 25 deaths without a single kill then wolf simply isn't for them.. =X
		if ( deaths == 25 ) {
			trap_SendServerCommand( -1,
									va( "chat \"^0DEATHSPREE! ^7%s ^7is just out of luck with ^025 ^7deaths without a kill^0!\n\"",
										ent->client->pers.netname ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/dSpree4.wav" );
			te->r.svFlags |= SVF_BROADCAST;
		}
	}
}



/* Ready System */
/*===========
  ready
===========*/
void Cmd_Ready_f( gentity_t *ent ) {
	gclient_t *client = ent->client;

	if ( g_tournymode.integer ) {

		if ( !level.warmupTime ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"^3The match is already in progress!\n\"" ) );
			return;
		}

		if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"^3Spectators can not use ready feature!\n\"" ) );
			return;
		}

		if ( client->pers.ready ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"You already are ^2ready^7!\n\"" ) );
			return;
		} else if ( !client->pers.ready ) {
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7is ^2ready^7!\n\"", client->pers.netname ) );
		}
		client->pers.ready = 1;
		if ( client->sess.sessionTeam == TEAM_RED ) {
			client->ps.powerups[PW_REDFLAG] = 0; // I've took this from existing /ready stuff.. although i probably will add print in /getstatus as well (not sure yet).
		} else {
			client->ps.powerups[PW_BLUEFLAG] = 0;
		}
	}
}

/*===========
 not ready
===========*/
void Cmd_NotReady_f( gentity_t *ent ) {
	gclient_t *client = ent->client;

	if ( g_tournymode.integer ) {

		if ( !level.warmupTime ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"^3The match is already in progress!\n\"" ) );
			return;
		}

		// When admin/ref does "!readyAll" it sets countEnd to 1 so players can't unReady them selfs..
		if ( level.countEnd == 1 ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"^3The count down has already started!\n\"" ) );
			return;
		}

		if ( client->sess.sessionTeam == TEAM_SPECTATOR ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"^3Spectators can not use ready feature!\n\"" ) );
			return;
		}


		if ( !client->pers.ready ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"You already are ^1not ready^7!\n\"" ) );
			return;
		} else if ( client->pers.ready ) {
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7is ^1NOT^7 ready!\n\"", client->pers.netname ) );
		}
		client->pers.ready = 0;
		if ( client->sess.sessionTeam == TEAM_RED ) {
			client->ps.powerups[PW_REDFLAG] = INT_MAX;
		} else {
			client->ps.powerups[PW_BLUEFLAG] = INT_MAX;
		}
	}
}

/*===================================================================================================================
S4NDM4NN locked ET_MISSILES in air(mortars, nades, rockets, a/s...), also locked movers. Disabled a few other client things.

L0 - Pause the match functions
This version is based around DutchMeat's "pause" but it's more sofisticated also in version i'm currently working
@ other clean source i use for testing only certain functions, much is changed..but for now I'll use what does work.

Stuff that needs to get done is:

- Adding a function to trap any rockets in air at the time match was paused,
    - S4NDM4NN done
- Seperating the calls for grenade and Air Strike so we know what was what when match was paused
- Trapping respawn times (game time is trapped!),
- Lock the objective so it doesn't disapper after 30 seconds,
    - S4NDM4NN done
- Lock any specs on their spot so they don't go around flying around,
    - L0 done :)
- Trap the dynamite counter (time passed - time left).

OPTIONAL: - Adding a check if some one decided to reset the match while pause was on as by default when game gets
paused it paritally fades the screen so if it doesn't get un-paused players are stuck with faded screen until new
map is loaded or they do vid_restart since the unPause never happend and stuff wasn't unRegistered...

What is done (just in case if I missed smth...):
- Traps game time,
- Locks fully player on spot (even if in air..),
- Barbwire and alike damage is ignored (it doesn't damage..),
- It locks stamina and weapon recharge.
- Countdown before resuming game added - note: if you want to unlock it for more then 1 per game you'll need to patch
        the code in g_cmds under callvote and S4ND_svcmds as well.. and also note- 2nd time around game doesn't
        get unpaused..if you want more pauses per game let me know and i'll fix it.
- Added ability to pause or unpause the match via callvote.

====================================================================================================================*/
extern void G_ExplodeMissile( gentity_t *ent );
extern void weapon_callAirStrike( gentity_t * ent );
// Un-pause Call
void UnPauseGame( void ) {
	gentity_t *who = NULL;
	gentity_t *grenades = NULL;
	int i;

	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		who = g_entities + i;
		if ( who->client && who->client->sess.sessionTeam != TEAM_SPECTATOR ) {


			while ( ( grenades = G_Find( grenades, FOFS( classname ), "grenade" ) ) != NULL )
			{
				if ( grenades->r.ownerNum == who->s.clientNum && !who->smokegren ) {

					if ( grenades->smokegren ) {

						grenades->think = weapon_callAirStrike;
						grenades->nextthink = level.time + 5000;

					} else {
						// Check doesn't work... so count all as nade atm..
						grenades->think = G_ExplodeMissile;
						grenades->nextthink = level.time + 5000;     //countdown (8) + 5 secs
					}
				}
			}
			grenades = NULL;

			while ( ( grenades = G_Find( grenades, FOFS( classname ), "dynamite" ) ) != NULL )
			{
				grenades->think = G_ExplodeMissile;
				grenades->nextthink = level.time + 15000; // i'm to tired and to lazy to do it now so 50/50 chances it will swing in the right way..
				grenades->s.effect1Time = level.time + 15000;
			}
			grenades = NULL; //reset again for the next user in the for loop


			if ( who->client ) {
				who->client->pers.stuck = qfalse; // Unlock the player
			}
			who->client->ps.pm_flags = PM_NORMAL; // -||-
			trap_SetConfigstring( CS_SCREENFADE, va( "0 %i %i", level.time + 250, (int) who->delay ) ); // UnFade the screen now
		}

		level.paused = 0; // Kill the pause
	}
}

/*==============
Pause Countdown
==============*/
void CountDown( void ) {
	gentity_t *te;
	gentity_t *other;
	int i;
	char index[MAX_OSPATH];

	Q_strncpyz( index, "", sizeof( index ) );

	if ( level.CNyes == qfalse ) {
		return;
	}
	// Countdown...
	if ( level.CNstart == 0 ) {
		trap_SendServerCommand( -1, va( "cp \"^2Prepare to fight!\n\"2" ) );
		Q_strncpyz( index, "sound/player/prepare.wav", sizeof( index ) );
	}

	if ( level.CNstart == 1 ) {
		trap_SendServerCommand( -1, va( "cp \"Match Resumes in: ^25\n\"2" ) );
		Q_strncpyz( index, "sound/scenaric/rocket/ls_54321.wav", sizeof( index ) );
	}

	if ( level.CNstart == 2 ) {
		trap_SendServerCommand( -1, va( "cp \"Match Resumes in: ^24\n\"2" ) );
	}

	if ( level.CNstart == 3 ) {
		trap_SendServerCommand( -1, va( "cp \"Match Resumes in: ^23\n\"2" ) );
	}

	if ( level.CNstart == 4 ) {
		trap_SendServerCommand( -1, va( "cp \"Match Resumes in: ^22\n\"2" ) );
	}

	if ( level.CNstart == 5 ) {
		trap_SendServerCommand( -1, va( "cp \"Match Resumes in: ^21\n\"2" ) );
	}

	if ( level.CNstart == 6 ) {
		Q_strncpyz( index, "sound/player/fight_rb.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "cp \"^2FIGHT!\n\"2" ) );
		UnPauseGame(); // do the magic

		// Unlock any specs here...
		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			other = &g_entities[i];

			if ( other->client->sess.sessionTeam == TEAM_SPECTATOR ) {
				other->client->ps.pm_flags = PM_NORMAL;
			}
		} // Specs end..
	}

	if ( level.CNstart == 0 ) {
		level.CNpush = level.time + 3000;
	} else {
		level.CNpush = level.time + 1100;
	}

	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		other = &g_entities[i];
		te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( index );
		te->s.teamNum = other->s.clientNum;
	}
	level.CNstart++;
}
// Pause call
void PauseGame( void ) {
	gentity_t *who = NULL;
	gentity_t *grenades = NULL;
	int i;
	gentity_t *te = NULL;


	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		who = g_entities + i;
		if ( who->client && who->client->sess.sessionTeam != TEAM_SPECTATOR ) {

			while ( ( grenades = G_Find( grenades, FOFS( classname ), "grenade" ) ) != NULL ) //search for grenade entities
			{
				if ( grenades->r.ownerNum == who->s.clientNum ) { //make sure its our grenade
					grenades->think = G_ExplodeMissile;
					grenades->nextthink = -1;
				}
			}
			grenades = NULL;


			while ( ( grenades = G_Find( grenades, FOFS( classname ), "dynamite" ) ) != NULL )
			{
				grenades->think = G_ExplodeMissile;
				grenades->nextthink = -1;
				grenades->s.effect1Time = level.time;
			}

			grenades = NULL; //reset again for the next user in the for loop

			who->client->pers.stuck = qtrue;

		}
	}

	level.paused = 1;
	level.killCPmsg++;
	trap_SetConfigstring( CS_SCREENFADE, va( ".4 %i 500", level.time + 250 ) ); // Fade the screen partially..

	te = G_TempEntity( who->s.pos.trBase, EV_GLOBAL_SOUND );
	te->s.eventParm = G_SoundIndex( "sound/player/paused.wav" );
	te->r.svFlags |= SVF_BROADCAST;
}
/** Pause cmd's */

// pause game
void Cmd_Pause_f( gentity_t *ent ) {

	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	// L0 - Atm, 1 Pause per game!? If you decide to have more then make sure it works the 2nd time around.
	if ( level.pendingUnpause > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Pause is not available any more for this round!\n\"" ) );
		return;
	}

	if ( !level.paused ) {
		trap_SendServerCommand( -1, va( "chat \"console: Admin has ^1Paused ^7the game! \"" ) );
		PauseGame();
		level.pendingUnpause++;
	} else {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3The game is already paused!\n\"" ) );
	}
}

// unpause game
void Cmd_UnPause_f( gentity_t *ent ) {

	if ( level.paused ) {
		trap_SendServerCommand( -1, va( "chat \"console: the game has been ^2unpaused^7 by an Admin!\"" ) );
		level.CNyes = qtrue;
		CountDown();
	} else {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3The game is already unpaused!\n\"" ) );
	}


}



/* New commands... */
/* L0 - 13.07/09 - this is not need it anymore.. commands are now hooked to admins.
qboolean adminCmds (gentity_t *ent, char *cmd ) {

    // admins
    if(!Q_stricmp(cmd, "pause")) { Cmd_Pause_f(ent);
        return qtrue;
    }
    if(!Q_stricmp(cmd, "unpause")) { Cmd_UnPause_f(ent);
        return qtrue;
    }
    else
        return qfalse;
}
*/

/* Toggle incognito state */
void Cmd_incognito( gentity_t *ent ) {

	if ( ent->client->sess.admin ) {

		if ( ent->client->sess.incognito ) {
			trap_SendServerCommand( ent - g_entities, va( "cp \"Your status is now visible^1!\n\"" ) );
			ent->client->sess.incognito = 0;
			return;
		} else if ( !ent->client->sess.incognito )     {
			trap_SendServerCommand( ent - g_entities, va( "cp \"Your status is now hidden^2!\n\"" ) );
			ent->client->sess.incognito = 1;
			return;
		}
	} else {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3You need to be logged in to use this feature!\n\"" ) );
	}
	return;
}

extern void CheckVote( void );
/* cancel vote in progress */


void Cmd_Cancelvote_f( gentity_t *ent ) {
	if ( level.voteTime ) {
		level.voteNo = level.numConnectedClients;
		CheckVote();
		trap_SendServerCommand( -1, "print \"Admin has ^3Cancelled the vote!\n\"" );
		trap_SendServerCommand( -1, va( "chat \"console: ^3Turns out everyone voted No!\"" ) );
		return;
	}
}

/* Pass vote */
void Cmd_Passvote_f( gentity_t *ent ) {
	if ( level.voteTime ) {
		level.voteYes = level.numConnectedClients;
		CheckVote();
		trap_SendServerCommand( -1, "print \"Admin has ^3Passed the vote!\n\"" );
		trap_SendServerCommand( -1, va( "chat \"console: ^3Turns out everyone voted Yes!\"" ) );
		return;
	}
}