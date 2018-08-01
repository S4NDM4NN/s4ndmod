#include "g_local.h"
#include "S4ND_local.h"
#ifdef OMNIBOT
	#include  "g_rtcwbot_interface.h"
#endif

void Cmd_giveAmmo_f( gentity_t* ent ) {
	gentity_t *target;
	trace_t tr;
	vec3_t start, end, forward;
	char arg1[MAX_STRING_TOKENS];
	int givenAmmo;
	int weapon, targWeap;
	int maxGive; //the max ammount of ammo you can give this player(just in case a player gives too much ammo they will get it back)

	if ( !g_allowGiveAmmo.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^7/giveammo^3 is not enabled on this server!\"" ) );
		return;
	}

	trap_Argv( 1,arg1,sizeof( arg1 ) );
	givenAmmo = atoi( arg1 );

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You have to be alive to do that!\"" ) );
		return;
	}

	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->s.pos.trBase, start );   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA( start, 128, forward, end );    //put 'end' 128 units forward of 'start'

	//see if we hit anything between 'start' and 'end'
	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, CONTENTS_BODY );

	//if we didn't hit a player, return
	if ( tr.entityNum >= MAX_CLIENTS ) {
		return;
	}

	target = &( g_entities[tr.entityNum] );

	if ( ( !target->inuse ) || ( !target->client ) ) { //if the player is lagged/disconnected/etc
		return;
	}
	if ( target->client->sess.sessionTeam != ent->client->sess.sessionTeam ) { //not on same team
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3What are you a spy or something!\"" ) );
		return;
	}

	if ( target->client->ps.stats[STAT_HEALTH] <= 0 ) {   //if they're dead
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You can not give ammo to the dead!\"" ) );
		return;
	}

	//if given ammo is 0 or unset make it the size of a clip
	if ( givenAmmo <= 0 ) {
		givenAmmo = ammoTable[ent->client->ps.weapon].maxclip;
	}

	weapon = ent->client->ps.weapon;

	switch ( weapon ) {
	case WP_LUGER:
	case WP_COLT:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_LUGER ) && !COM_BitCheck( target->client->ps.weapons, WP_COLT ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a Pistol!\"", target->client->pers.netname ) );
			return;
		}

		if ( COM_BitCheck( target->client->ps.weapons, WP_LUGER ) ) {
			targWeap = WP_LUGER;
		} else {
			targWeap = WP_COLT;
		}

		maxGive = ( ammoTable[targWeap].maxclip * 4 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] > ammoTable[targWeap].maxclip * 4 ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] = ammoTable[targWeap].maxclip * 4;
		}

		break;

	case WP_MP40:
	case WP_THOMPSON:
	case WP_STEN:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_MP40 ) && !COM_BitCheck( target->client->ps.weapons, WP_THOMPSON ) && !COM_BitCheck( target->client->ps.weapons, WP_STEN ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a SMG!\"", target->client->pers.netname ) );
			return;
		}

		if ( COM_BitCheck( target->client->ps.weapons, WP_MP40 ) ) {
			targWeap = WP_MP40;
		} else if ( COM_BitCheck( target->client->ps.weapons, WP_THOMPSON ) ) {
			targWeap = WP_THOMPSON;
		} else {
			targWeap = WP_STEN;
		}

		maxGive = ( ammoTable[targWeap].maxclip * 3 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] > ammoTable[targWeap].maxclip * 3 ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( targWeap )] = ammoTable[targWeap].maxclip * 3;
		}

		break;

	case WP_SHOTGUN:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_MP40 ) && !COM_BitCheck( target->client->ps.weapons, WP_THOMPSON ) && !COM_BitCheck( target->client->ps.weapons, WP_STEN ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a shotgun!\"", target->client->pers.netname ) );
			return;
		}

		maxGive = ( ammoTable[WP_SHOTGUN].maxclip * 3 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SHOTGUN )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SHOTGUN )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SHOTGUN )] > ammoTable[WP_SHOTGUN].maxclip * 3 ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SHOTGUN )] = ammoTable[WP_SHOTGUN].maxclip * 3;
		}

		break;

	case WP_MAUSER:
	case WP_SNIPERRIFLE:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_SNIPERRIFLE ) && !COM_BitCheck( target->client->ps.weapons, WP_MAUSER ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a Mauser!\"", target->client->pers.netname ) );
			return;
		}
		//dont give sniper ammo if instagib is enabled.
		if ( !g_instaGib.integer ) {
			maxGive = ( ammoTable[WP_SNIPERRIFLE].maxclip * 3 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )];
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] += givenAmmo;
			if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] > ammoTable[WP_SNIPERRIFLE].maxclip * 3 ) {
				target->client->ps.ammo[BG_FindAmmoForWeapon( WP_SNIPERRIFLE )] = ammoTable[WP_SNIPERRIFLE].maxclip * 3;
			}
		}

		maxGive = ( ammoTable[WP_MAUSER].maxclip * 3 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] > ammoTable[WP_MAUSER].maxclip * 3 ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_MAUSER )] = ammoTable[WP_MAUSER].maxclip * 3;
		}
		break;

	case WP_FG42:
	case WP_FG42SCOPE:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_FG42SCOPE ) && !COM_BitCheck( target->client->ps.weapons, WP_FG42 ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a FG42!\"", target->client->pers.netname ) );
			return;
		}
		//dont give sniper ammo if instagib is enabled.
		maxGive = ( ammoTable[WP_FG42SCOPE].maxclip * 3 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42SCOPE )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42SCOPE )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42SCOPE )] > ammoTable[WP_FG42SCOPE].maxclip * 3 ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42SCOPE )] = ammoTable[WP_FG42SCOPE].maxclip * 3;
		}


		maxGive = ( ammoTable[WP_FG42].maxclip * 3 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42 )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42 )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42 )] > ammoTable[WP_FG42].maxclip * 3 ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_FG42 )] = ammoTable[WP_FG42].maxclip * 3;
		}
		break;

	case WP_PANZERFAUST:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_PANZERFAUST ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a Panzerfaust!\"", target->client->pers.netname ) );
			return;
		}
		maxGive = ( ammoTable[WP_PANZERFAUST].maxclip * 3 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] > ammoTable[WP_PANZERFAUST].maxclip * 3 ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = ammoTable[WP_PANZERFAUST].maxclip * 3;
		}

		break;


	case WP_VENOM:

		if ( ent->client->ps.ammo[BG_FindClipForWeapon( ent->client->ps.weapon )] < givenAmmo ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^3You do not have enough ammo to give ^7%i^3!\"",givenAmmo ) );
			return;
		}

		if ( !COM_BitCheck( target->client->ps.weapons, WP_VENOM ) ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not have a Venom!\"", target->client->pers.netname ) );
			return;
		}
		maxGive = ( ammoTable[WP_VENOM].maxclip * 3 ) - target->client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )];
		target->client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )] += givenAmmo;
		if ( target->client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )] > ammoTable[WP_VENOM].maxclip * 3 ) {
			target->client->ps.ammo[BG_FindAmmoForWeapon( WP_VENOM )] = ammoTable[WP_VENOM].maxclip * 3;
		}
		break;

	default:
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You can not use ^7/giveammo^3 with this weapon!\"" ) );
		return;
	}

	if ( maxGive <= 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^7%s ^3does not need any ammo!\"",target->client->pers.netname ) );
		return;
	}

	if ( givenAmmo > maxGive ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You gave ^7%s^3 ammo, ^7%i ^3was all he could carry!\"",target->client->pers.netname,maxGive ) );
		trap_SendServerCommand( target - g_entities, va( "chat \"^7%s^3 gave you ^7%i ^3ammo!\"",ent->client->pers.netname,maxGive ) );
		ent->client->ps.ammo[BG_FindClipForWeapon( weapon )] -= maxGive;

	} else {

		trap_SendServerCommand( ent - g_entities, va( "chat \"^3You gave ^7%i^3 ammo to ^7%s^3!\"",givenAmmo,target->client->pers.netname ) );
		trap_SendServerCommand( target - g_entities, va( "chat \"^7%s^3 gave you ^7%i ^3ammo!\"",ent->client->pers.netname,givenAmmo ) );
		ent->client->ps.ammo[BG_FindClipForWeapon( weapon )] -= givenAmmo;
	}

	//S4NDM4NN - So bots can thank you for giving ammo this way
	#ifdef OMNIBOT
	Bot_Event_RecievedAmmo( target - g_entities, ent );
	#endif
}
/*
===========
Cmd_Push_f
===========
*/
/*void Cmd_Push_f( gentity_t* ent ) {
	gentity_t *target;
	trace_t tr;
	vec3_t start, end, forward;
	float shoveAmount;

	if ( !g_shove.integer ) {
		return;
	}

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	if ( level.time < ( ent->lastPushTime + 600 ) ) {
		return;
	}

	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->s.pos.trBase, start );   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA( start, 128, forward, end );    //put 'end' 128 units forward of 'start'

	//see if we hit anything between 'start' and 'end'
	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, CONTENTS_BODY );

	//if we didn't hit a player, return
	if ( tr.entityNum >= MAX_CLIENTS ) {
		return;
	}

	target = &( g_entities[tr.entityNum] );

	if ( ( !target->inuse ) || ( !target->client ) ) { //if the player is lagged/disconnected/etc
		return;
	}

	if ( target->client->ps.stats[STAT_HEALTH] <= 0 ) { //if they're dead
		return;
	}

	//push them by adding 512*g_shoveAmount units to their velocity, in the direction the pusher is facing
	//shoveAmount = 512 * g_shoveAmount.value;
	VectorMA( target->client->ps.velocity, shoveAmount, forward, target->client->ps.velocity );

	ent->lastPushTime = level.time;     //set this so we can't push too often
}*/

/*
=================
Cmd_Drag_f
=================
*/
void Cmd_Drag_f( gentity_t *ent ) {
	gentity_t *target;
	vec3_t start,dir,end;
	trace_t tr;
	target = NULL;

	if ( !g_dragCorpse.integer ) {
		return;
	}

	if ( level.time < ( ent->lastDragTime + 20 ) ) {
		return;
	}

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	AngleVectors( ent->client->ps.viewangles, dir, NULL, NULL );

	VectorCopy( ent->s.pos.trBase, start );   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA( start, 100, dir, end );    //put 'end' 128 units forward of 'start'


	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, CONTENTS_CORPSE );

	//if we didn't hit a player, return
	if ( tr.entityNum >= MAX_CLIENTS ) {
		return;
	}

	target = &( g_entities[tr.entityNum] );

	if ( ( !target->inuse ) || ( !target->client ) ) { //if the player is lagged/disconnected/etc
		return;
	}

// put target position in start
	VectorCopy( target->r.currentOrigin, start );
// put grenade position in end
	VectorCopy( ent->r.currentOrigin, end );
// subtract start from end to get directional vector
	VectorSubtract( end, start, dir );
	VectorNormalize( dir );
// scale directional vector by 100 and add to the targets velocity
	VectorScale( dir,100, target->client->ps.velocity );
// make targets move direction = to directional vector.
	VectorCopy( dir, target->movedir );

	ent->lastDragTime = level.time;
}


void G_CanisterKick( gentity_t *ent ) {

	gentity_t   *traceEnt;
	vec3_t forward, right, up, angles, mins, maxs, origin;
	float speed = 800.0f;       //default value, based on some testing
	int i, num;
	int touch[MAX_GENTITIES];

	if (
	    //dead players
		ent->client->ps.pm_flags & PMF_LIMBO ||
		ent->client->ps.pm_type == PM_DEAD
		) {
		// do not pass go, do not kick canisters
		return;
	}

	// Find straight ahead, no pitching...
	// We do this since we don't actually want to have to look at the item we
	// are going to kick
	VectorCopy( ent->client->ps.viewangles, angles );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
	AngleVectors( angles, forward, right, up );

	// Move straight ahead from origin 24 units
	// Not the full 32 since we want to be able to kick thing we are just about standing on.
	VectorMA( ent->r.currentOrigin,24,forward,origin );

	// Only kick things that are up to about CH_KNIFE_DIST away in front of us.
	// and only up to about waist high.
	VectorAdd( origin, tv( -32.f,-32.f,-24.f ), mins );
	VectorAdd( origin, tv( 32.f,32.f,8.f ), maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	if ( g_debugBullets.integer ) {
		gentity_t *tent;

		tent = G_TempEntity( mins, EV_RAILTRAIL );
		VectorCopy( maxs, tent->s.origin2 );
		tent->s.dmgFlags = 1;

	}

	for ( i = 0; i < num; i++ ) {

		traceEnt = &g_entities[touch[i]];

		// prevents a nasty kick corpses bug if they died holding any of
		// the kick weapon types.
		if ( traceEnt->s.eType != ET_MISSILE ) {
			continue;
		}

		// check for grenades, and misc canisters otherwise reject...
		if ( ( g_kickableNades.integer && traceEnt->s.weapon == WP_GRENADE_PINEAPPLE ) ||
			 ( g_kickableNades.integer && traceEnt->s.weapon == WP_GRENADE_LAUNCHER ) ||
			 ( g_kickableAS.integer && traceEnt->s.weapon == WP_SMOKE_GRENADE ) ) {

			traceEnt->s.pos.trType = TR_GRAVITY;            //have to set this in case it was reset to TR_STATIONARY
			VectorClear( angles );        //only angles[1] is used
			//set it to the direction we're facing, +/- 15 degrees
			angles[1] = ent->client->ps.viewangles[1] + ( crandom() * 15.0f );

			//get the 'forward' vector based on the direction we're facing
			AngleVectors( angles, forward, NULL, NULL );
			//add some randomness to the 'height' of the kick
			forward[2] = 0.5f + ( crandom() * 0.15f );
			VectorNormalizeFast( forward );               //re-normalize the vector

			speed += ( crandom() * 150.0f );              //add some randomness to the speed/distance of the kick
			VectorScale( forward, speed, forward );       //and multiply the vector by the speed

			//play the grenade throwing sound, just to let them know they successfully kicked it
			G_Sound( ent, G_SoundIndex( "sound/weapons/grenade/grenlf1a.wav" ) );

			trap_UnlinkEntity( traceEnt );                    //probably not necessary, may prevent an occasional error
			traceEnt->s.pos.trTime = level.time - 50;   //move a little on the first frame

			//set the grenade's initial position
			VectorCopy( traceEnt->r.currentOrigin, traceEnt->s.pos.trBase );
			VectorCopy( forward, traceEnt->s.pos.trDelta );   //essentially, set the velocity
			SnapVector( traceEnt->s.pos.trDelta );                //"save network bandwith" apparently...rounds everything to integers
			trap_LinkEntity( traceEnt );                      //re-link the grenade...have to if we unlinked it

			if ( g_kickerGetsOwner.integer ) {
				// canister owner gets set to kicker.
				traceEnt->parent = ent;
				traceEnt->r.ownerNum = ent->s.number;
				traceEnt->s.teamNum = ent->client->sess.sessionTeam;
			}
		}
	}
	return;
}




/*
==================================================================
Cmd_KickGrenade_f
Implements grenade kicking whenever a player presses +salute
while standing near a grenade (with some restrictions).
This is not an extremely cheap function to call, but it
shouldn't create any problems in virtually any situation.
There have been issues with it causing rare game freezes
while it was being developed, but these no longer seem to occur.
==================================================================
*/



void Cmd_KickGrenade_f( gentity_t *ent ) {
	gentity_t *e, *grenade = NULL;
	float dist, bestdist = -1;

	if ( !g_kickableNades.integer ) { //if it's not enabled, skip everything and just return
		return;
	}

	//loop through every non-player entity, and find the closest valid grenade
	//ordered for minimal calculations and impact - SHOULD be unnoticable
	for ( e = g_entities + MAX_CLIENTS; e < &g_entities[level.num_entities]; e++ )
	{
		//if it's invalid or unused, skip it
		if ( !e || !e->inuse ) {
			continue;
		}

		//if it's not a grenade, skip it (have to check both, flamebarrels etc use same mod, and airstrikes are 'grenades')
		if ( ( e->methodOfDeath != MOD_GRENADE ) || ( Q_stricmp( e->classname, "grenade" ) != 0 ) ) {
			continue;
		}

		//can't kick grenades that are above waist-height
		if ( e->r.currentOrigin[2] >= ent->r.currentOrigin[2] ) {
			continue;
		}

		//find the nearest valid grenade
		dist = VectorDistance( ent->r.currentOrigin, e->r.currentOrigin );
		if ( ( dist < bestdist ) || ( bestdist == -1 ) ) {
			grenade = e;
			bestdist = dist;
		}
	}

	if ( ( grenade ) && ( bestdist <= 70 ) ) { //if we found a grenade and it's close enough to kick
		//don't kick it if it's moving too fast or about to explode(causes errors? SOMEthing does...)
		if ( ( grenade->s.pos.trDelta[2] <= 140 ) && ( VectorLength( grenade->s.pos.trDelta ) <= 160 ) && ( grenade->nextthink - level.time >= 500 ) ) {
			float speed = 800.0f;       //default value, based on some testing
			vec3_t angles, forward;

			grenade->s.pos.trType = TR_GRAVITY;         //have to set this in case it was reset to TR_STATIONARY
			VectorClear( angles );        //only angles[1] is used
			//set it to the direction we're facing, +/- 15 degrees
			angles[1] = ent->client->ps.viewangles[1] + ( crandom() * 15.0f );

			//get the 'forward' vector based on the direction we're facing
			AngleVectors( angles, forward, NULL, NULL );
			//add some randomness to the 'height' of the kick
			forward[2] = 0.5f + ( crandom() * 0.15f );
			VectorNormalizeFast( forward );               //re-normalize the vector

			speed += ( crandom() * 150.0f );              //add some randomness to the speed/distance of the kick
			VectorScale( forward, speed, forward );       //and multiply the vector by the speed

			//play the grenade throwing sound, just to let them know they successfully kicked it
			G_Sound( ent, G_SoundIndex( "sound/weapons/grenade/grenlf1a.wav" ) );

			trap_UnlinkEntity( grenade );                 //probably not necessary, may prevent an occasional error
			grenade->s.pos.trTime = level.time - 50;    //move a little on the first frame

			//set the grenade's initial position
			VectorCopy( grenade->r.currentOrigin, grenade->s.pos.trBase );
			VectorCopy( forward, grenade->s.pos.trDelta );    //essentially, set the velocity
			SnapVector( grenade->s.pos.trDelta );             //"save network bandwith" apparently...rounds everything to integers
			trap_LinkEntity( grenade );                       //re-link the grenade...have to if we unlinked it
		}
	}
}

/*
=============
Cmd_KickAS_f
=============
*/
void Cmd_KickAS_f( gentity_t *ent ) {
	gentity_t *e, *grenade = NULL;
	float dist, bestdist = -1;

	if ( !g_kickableAS.integer ) { //just return if this isn't enabled
		return;
	}



	//loop through every non-player entity, and find the nearest valid airstrike canister (if any)
	for ( e = g_entities + MAX_CLIENTS; e < &g_entities[level.num_entities]; e++ )
	{
		if ( !e || !e->inuse ) {  //skip unused entities
			continue;
		}

		//skip non-airstrike-canisters
		if ( ( Q_stricmp( e->classname, "grenade" ) != 0 ) || ( e->s.weapon != WP_SMOKE_GRENADE ) ) {
			continue;
		}



		//it must be at or below waist-height
		if ( e->r.currentOrigin[2] >= ent->r.currentOrigin[2] ) {
			continue;
		}

		//it can't be moving/bouncing (unlike grenades)
		if ( VectorLength( e->s.pos.trDelta ) > 0 ) {
			continue;
		}

		//find the closest matching airstrike canister
		//this probably isn't necessary, since normally a lt can only have one canister out at a time
		//but if the lt charge time was VERY low, it might be possible to have multiple ones
		dist = VectorDistance( ent->r.currentOrigin, e->r.currentOrigin );
		if ( ( dist < bestdist ) || ( bestdist == -1 ) ) {
			grenade = e;
			bestdist = dist;
		}
	}

	if ( ( grenade ) && ( bestdist <= 70 ) ) { //if we found a canister that is close enough to kick
		float speed = 500.0f;   //much lower speed for these compared to grenades
		vec3_t angles, forward;

		//virtually the same as in Cmd_KickGrenade_f
		VectorClear( angles );
		angles[1] = ent->client->ps.viewangles[1] + ( crandom() * 15.0f );

		G_Sound( ent, G_SoundIndex( "sound/weapons/grenade/grenlf1a.wav" ) );

		AngleVectors( angles, forward, NULL, NULL );
		forward[2] = 0.4f + ( crandom() * 0.15f );    //slightly lower arc
		VectorNormalizeFast( forward );

		speed += ( crandom() * 150.0f );
		VectorScale( forward, speed, forward );

//		trap_UnlinkEntity(grenade);
		grenade->s.pos.trType = TR_GRAVITY;
		grenade->s.pos.trTime = level.time - 50;

		VectorCopy( grenade->r.currentOrigin, grenade->s.pos.trBase );
		VectorCopy( forward, grenade->s.pos.trDelta );
		SnapVector( grenade->s.pos.trDelta );
//		trap_LinkEntity(grenade);
	}
}

/*
===========
Cmd_Gib_f
===========
*/
void Cmd_Gib_f( gentity_t *ent ) {
	if ( g_deathmatch.integer == 2 ) {
		trap_SendServerCommand(  ent - g_entities, "print \"^3You can not /gib in free for all.\n\"" );
		return;
	}

	// L0 - don't allow suicide if match is paused
	if ( level.paused ) {
		return;
	}

	//Dont allow it if they just spawned - Agent, let them, if they really want to kill themself...
//	if (ent->client->ps.powerups[PW_INVULNERABLE] >= level.time)
//		return;

	if ( g_headshotsOnly.integer != 0 ) {
		return;
	}

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}
	if ( g_gametype.integer >= GT_WOLF && ent->client->ps.pm_flags & PMF_LIMBO ) {
		return;
	}
	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	ent->flags &= ~FL_GODMODE;
	ent->client->ps.stats[STAT_HEALTH] = ent->health = 0;
	ent->client->ps.persistant[PERS_HWEAPON_USE] = 0; // TTimo - if using /kill while at MG42
	player_die( ent, ent, ent, 100000, MOD_SUICIDE_GIB );
	ent->client->pers.gibbed = qtrue;
	limbo( ent, qtrue );
}

//From here down to agents commands are for S4NDM4NN i think(trying to clean up source some)
//==========================================================================================

/*
==================
For Nadepack drop
==================
*/
void Cmd_NadePack_f( gentity_t *ent ) {
	gclient_t   *client;
	gitem_t *item = NULL;
	vec3_t velocity, org, offset;
	vec3_t angles,mins,maxs;
	trace_t tr;
	gentity_t *flag;
	client = ent->client;

	if ( !g_dropNadepacks.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"Grenade packs are not enabled\"" ) );
		return;
	}

	//Bugfix...Players could drop if dead
	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"You must be alive to drop a grenade pack\"" ) );
		return;
	}

	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] != PC_ENGINEER ) 
	{
		trap_SendServerCommand( ent - g_entities, va( "chat \"You must be an engineer to drop a grenade pack\"" ) );
		return;
	}
	
	if ( level.time - ent->client->ps.classWeaponTime >= g_engineerChargeTime.integer * 0.75f ) {
		if ( level.time - ent->client->ps.classWeaponTime > g_engineerChargeTime.integer ) {
			ent->client->ps.classWeaponTime = level.time - g_engineerChargeTime.integer;
		}

		ent->client->ps.classWeaponTime += g_engineerChargeTime.integer * 0.25;
		if ( ent->client->sess.sessionTeam == TEAM_RED ) {
			item = BG_FindItem( "Grenades" );
		} else {
			item = BG_FindItem( "Pineapples" );
		}

		//much of the following copied from the dropweapon code below
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

		flag = LaunchItem( item, org, velocity, client->ps.clientNum );

		flag->s.modelindex2 = ent->s.otherEntityNum2; // JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = ent->message;   // DHM - Nerve :: also restore item name
		// Clear out player's temp copies
		ent->s.otherEntityNum2 = 0;
		ent->message = NULL;

		G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/bag_toss.wav" ) );

	}
}
/*///////////////////////////////////////////////////////
Cmd_ThrowKnife_f
Implements knife throwing when a player presses the
drop weapon key while holding a knife.  Players can only
throw a knife every 0.75 seconds.
*/                                                                                                                                                                                                                                ///////////////////////////////////////////////////////
void Cmd_ThrowKnife_f( gentity_t *ent ) {
	vec3_t velocity, angles, offset, org, mins, maxs;
	trace_t tr;
	gentity_t *ent2;
	gitem_t *item = BG_FindItemForWeapon( WP_KNIFE );

	if ( g_throwableKnives.integer == 0 ) {
		return;
	}

	if ( level.time < ( ent->thrownKnifeTime + 750 ) ) {    //can throw one every 0.75 seconds
		return;
	}

	//return if we have no more knives to throw (-1 is infinite)
	if ( ( ent->client->pers.throwingKnives == 0 ) && ( g_throwableKnives.integer != -1 ) ) {
		return;
	}

	//some of this is copied from the drop weapon code in g_active.c
	AngleVectors( ent->client->ps.viewangles, velocity, NULL, NULL );
	VectorScale( velocity, 64, offset );
	offset[2] += ent->client->ps.viewheight / 2;
	VectorScale( velocity, 800, velocity );       //make the knife go pretty quickly
	velocity[2] += 50 + random() * 35;

	VectorAdd( ent->client->ps.origin, offset, org );

	VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
	VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );   //these have something to do with making the item sit on the ground

	trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
	VectorCopy( tr.endpos, org );

	G_Sound( ent, G_SoundIndex( "sound/weapons/knife/knife_slash1.wav" ) );     //play a sound
	//set angle of knife
	ent2 = LaunchItem( item, org, velocity, ent->client->ps.clientNum );      //actually throw the knife
	VectorCopy( ent->client->ps.viewangles, angles );
	angles[1] += 90;
	G_SetAngle( ent2, angles );
	ent2->touch = Touch_Knife;  //set this to our own handler so we can customize what happens
	ent2->parent = ent;         //so we know who threw the knife

	if ( g_throwableKnives.integer > 0 ) {
		ent->client->pers.throwingKnives--;     //get rid of a knife

	}
	//only show the message if throwing knives are enabled
	if ( g_throwableKnives.integer > 0 ) {
		//prints the message in the chat/talking area
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3Knives left:^7 %d\" %i", ent->client->pers.throwingKnives, qfalse ) );
	}


	ent->thrownKnifeTime = level.time;                          //mark this so they can't throw a ton of them in a row
}

//NOTE: This is not a command but it is related to above.
/*////////////////////////////////////////////
Touch_Knife
Called whenever a client touches a thrown knife.
Made this a separate function rather than edit 4 or 5 id funcs.
Will either hurt the client or have them pick up the knife.
Most of the pickup code is copied from Touch_Item.
*/                                                                                                                                                                                                                                                                                                  ////////////////////////////////////////////
void Touch_Knife( gentity_t *ent, gentity_t *other, trace_t *trace ) {
	qboolean hurt = qfalse;
	ent->active = qfalse;   //set this just to be safe, but shouldn't be necessary

	//make sure this is a person, and that they're not dead
	if ( !other->client ) {
		return;
	}
	if ( other->health < 1 ) {
		return;     // dead people can't pickup



	}
	if ( VectorLength( ent->s.pos.trDelta ) != 0 ) { //if the knife is still moving (not on ground yet)
		//if friendly fire is off, or if the player who touched the thrown knife is on the other team
		if ( ( g_friendlyFire.integer ) || ( !OnSameTeam( other, ent->parent ) ) ) {
			int i;
			int sound;
			int damage = g_knifeDamage.integer;     //set the damage to its max value
			damage -= rand() % 10;                  //subtract 0-9 to make it a little varied



			if ( damage <= 0 ) {      //make sure there's still some damage
				damage = 1;
			}

			//pick a random sound to play
			i = rand() % 3;
			if ( i == 0 ) {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit1.wav" );
			} else if ( i == 1 ) {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit2.wav" );
			} else {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit3.wav" );
			}

			G_Sound( other, sound );  //play the sound
			G_Damage( other, ent->parent, ent->parent, NULL, trace->endpos, damage, 0, MOD_THROWING_KNIFE );  //hurt them! (for now, no dir, maybe change later)
			hurt = qtrue;   //mark that this knife damaged someone
		}
	}

	if ( hurt == qfalse ) {   //if we didn't hurt anyone (can't do 'else' because of nested if statements above)
		int makenoise = EV_ITEM_PICKUP;

//		G_DPrintf("Item: %i %s\n", other->s.number, ent->item->classname);	//log it

		if ( g_throwableKnives.integer > 0 ) {
			other->client->pers.throwingKnives++;   //add the knife

		}
		//don't let us get more knives than the max
		if ( g_maxKnives.integer != 0 ) { //max of 0 means no max
			if ( other->client->pers.throwingKnives > g_maxKnives.integer ) {
				other->client->pers.throwingKnives = g_maxKnives.integer;
			}
		}

		//no idea, copied from Touch_Item
		if ( ent->noise_index ) {
			makenoise = EV_ITEM_PICKUP_QUIET;
			G_AddEvent( other, EV_GENERAL_SOUND, ent->noise_index );
		}

		//also from Touch_Item
		//send the pickup event
		if ( other->client->pers.predictItemPickup ) {
			G_AddPredictableEvent( other, makenoise, ent->s.modelindex );
		} else {
			G_AddEvent( other, makenoise, ent->s.modelindex );
		}
	}

	ent->freeAfterEvent = qtrue;    //get rid of this item
	ent->flags |= FL_NODRAW;
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eFlags |= EF_NODRAW;
	ent->r.contents = 0;
	ent->nextthink = 0;
	ent->think = 0;
}




//////////////////////////////////Dev Commands/////////////////////////////////////////////
/*
=================
Cmd_MakeDmPoint_f
=================
*/
void Cmd_MakeDmPoint_f( gentity_t *ent ) {
	FILE        *dmpoints;
	char v1[10];
	char v2[10];
	char v3[10];
	char angle1[10];
	char angle2[10];
	char mapName[MAX_QPATH];
	gitem_t     *item;
	gentity_t   *spawnitem;
	vec3_t launchvel;

	launchvel[0] = 0;
	launchvel[1] = 0;
	launchvel[2] = 0;

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );



	if ( !g_makeDMspawns.integer ) {
		return;
	}

	sscanf( vtos( ent->client->ps.origin ),"( %s %s %s )",&v1, &v2, v3 );
	sscanf( vtos( ent->client->ps.viewangles ),"( %s %s 0 )",&angle1, &angle2 );


	dmpoints = fopen( va( "%s.txt",mapName ),"a" ); //Open file

	if ( level.LastDMpoint == 0 ) {
		fputs( va( "if(level.DMspawnpoint = 0){\n" ),dmpoints );
	} else {
		fputs( va( "else if(level.DMspawnpoint == %i){\n",level.LastDMpoint ),dmpoints );
	}

	fputs( va( "deathmatchspawn[0]= %s;\n",v1 ),dmpoints );
	fputs( va( "deathmatchspawn[1]= %s;\n",v2 ),dmpoints );
	fputs( va( "deathmatchspawn[2]= %s;\n",v3 ),dmpoints );
	fputs( va( "deathmatchangles[1]= %s;\n",angle2 ),dmpoints );
	fputs( "}",dmpoints );
	fclose( dmpoints );
	trap_SendServerCommand( -1, va( "chat \"^1INFO: ^3Deathmatch spawn point created at ORIGIN:(^5%s %s %s^3) ANGLE:(^5%s^3)\n\"",v1, v2, v3,angle2 ) );
	level.LastDMpoint++;

	item = BG_FindItem( "Gold" );
	spawnitem = LaunchItem( item, ent->client->ps.origin, launchvel, 99 );

	spawnitem->dl_color[0] = 0.2f;
	spawnitem->dl_color[1] = 0.6f;
	spawnitem->dl_color[2] = 1.0f;
	spawnitem->spawnflags = 1;
	SP_dlight( spawnitem );

}

void Cmd_Makepathcorner_f( gentity_t *ent ) {
	FILE        *dmpoints;
	char v1[10];
	char v2[10];
	char v3[10];
	char mapName[MAX_QPATH];
	gitem_t     *item;
	gentity_t   *spawnitem;
	vec3_t launchvel;

	launchvel[0] = 0;
	launchvel[1] = 0;
	launchvel[2] = 0;

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	if ( !g_makeDMspawns.integer ) {
		return;
	}

	sscanf( vtos( ent->client->ps.origin ),"( %s %s %s )",&v1, &v2, v3 );

	dmpoints = fopen( va( "path_corner%s.txt",mapName ),"a" ); //Open file

	if ( level.LastDMpoint == 0 ) {
		fputs( va( "if(SpawnsMade == 0){\n" ),dmpoints );
	} else {
		fputs( va( "else if(SpawnsMade == %i){\n",level.LastDMpoint ),dmpoints );
	}

	fputs( va( "ent->r.currentOrigin[0]= %s;\n",v1 ),dmpoints );
	fputs( va( "ent->r.currentOrigin[1]= %s;\n",v2 ),dmpoints );
	fputs( va( "ent->r.currentOrigin[2]= %s;\n",v3 ),dmpoints );
	fputs( "}",dmpoints );
	fclose( dmpoints );
	trap_SendServerCommand( -1, va( "chat \"^1INFO: ^3path_corner point created at ORIGIN:(^5%s %s %s^3)\n\"",v1, v2, v3 ) );
	level.LastDMpoint++;

	item = BG_FindItem( "Gold" );
	spawnitem = LaunchItem( item, ent->client->ps.origin, launchvel, 99 );

	spawnitem->dl_color[0] = 0.2f;
	spawnitem->dl_color[1] = 0.6f;
	spawnitem->dl_color[2] = 1.0f;
	spawnitem->spawnflags = 1;
	SP_dlight( spawnitem );

}


/*
=================
Cmd_MakeDmWeap_f
=================
*/
void Cmd_MakeDmWeap_f( gentity_t *ent ) {
	FILE        *dmweap;
	char v1[10];
	char v2[10];
	char v3[10];
	char arg[MAX_STRING_TOKENS];
	char arg1[MAX_STRING_TOKENS];
	char mapName[MAX_QPATH];
	gitem_t     *item;
	gentity_t   *spawnitem;
	vec3_t launchvel;
	item = NULL;

	if ( !g_makeDMspawns.integer ) {
		return;
	}


	launchvel[0] = 0;
	launchvel[1] = 0;
	launchvel[2] = 0;

	trap_Argv( 1, arg, sizeof( arg ) );
	trap_Argv( 2, arg1, sizeof( arg1 ) );
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	sscanf( vtos( ent->client->ps.origin ),"( %s %s %s )",&v1, &v2, v3 );


	dmweap = fopen( va( "%sWEAPONS.txt",mapName ),"a" ); //Open file

	if ( strlen( arg1 ) ) {
		fputs( va( "item = BG_FindItem(\"%s %s\");\n",arg, arg1 ),dmweap );
	} else {
		fputs( va( "item = BG_FindItem(\"%s\");\n",arg ),dmweap );
	}

	fputs( va( "spawnorigin[0] = %s;\n",v1 ),dmweap );
	fputs( va( "spawnorigin[1] = %s;\n",v2 ),dmweap );
	fputs( va( "spawnorigin[2] = %s;\n",v3 ),dmweap );
	fputs( "spawnitem = LaunchItem(item, spawnorigin, launchvel, 99);\n",dmweap );

	if ( !Q_stricmp( arg, "thompson" ) ) {
		fputs( "spawnitem->count = 60;\n",dmweap );
		fputs( "spawnitem->item->quantity = 60;\n",dmweap );
		item = BG_FindItem( "Thompson" );
	} else if ( !Q_stricmp( arg, "mp40" ) )     {
		fputs( "spawnitem->count = 64;\n",dmweap );
		fputs( "spawnitem->item->quantity = 64;\n",dmweap );
		item = BG_FindItem( "Mp40" );
	} else if ( !Q_stricmp( arg, "sten" ) )     {
		fputs( "spawnitem->count = 64;\n",dmweap );
		fputs( "spawnitem->item->quantity = 64;\n",dmweap );
		item = BG_FindItem( "Sten" );
	} else if ( !Q_stricmp( arg, "grenade" ) )     {
		fputs( "spawnitem->count = 1;\n",dmweap );
		fputs( "spawnitem->item->quantity = 1;\n",dmweap );
		item = BG_FindItem( "Grenade" );
	} else if ( !Q_stricmp( arg, "pineapple" ) )     {
		fputs( "spawnitem->count = 1;\n",dmweap );
		fputs( "spawnitem->item->quantity = 1;\n",dmweap );
		item = BG_FindItem( "Pineapple" );
	} else if ( !Q_stricmp( arg, "colt" ) )     {
		fputs( "spawnitem->count = 24;\n",dmweap );
		fputs( "spawnitem->item->quantity = 24;\n",dmweap );
		item = BG_FindItem( "Colt" );
	} else if ( !Q_stricmp( arg, "luger" ) )     {
		fputs( "spawnitem->count = 24;\n",dmweap );
		fputs( "spawnitem->item->quantity = 24;\n",dmweap );
		item = BG_FindItem( "Luger" );
	} else if ( !Q_stricmp( arg, "venom" ) )     {
		fputs( "spawnitem->count = 250;\n",dmweap );
		fputs( "spawnitem->item->quantity = 250;\n",dmweap );
		item = BG_FindItem( "Venom" );
	} else if ( !Q_stricmp( arg, "panzerfaust" ) )     {
		fputs( "spawnitem->count = 2;\n",dmweap );
		fputs( "spawnitem->item->quantity = 2;\n",dmweap );
		item = BG_FindItem( "Panzerfaust" );
	} else if ( !Q_stricmp( arg, "Flamethrower" ) )     {
		fputs( "spawnitem->count = 75;\n",dmweap );
		fputs( "spawnitem->item->quantity = 75;\n",dmweap );
		item = BG_FindItem( "Flamethrower" );
	} else if ( !Q_stricmp( arg, "mauser" ) )     {
		fputs( "spawnitem->count = 12;\n",dmweap );
		fputs( "spawnitem->item->quantity = 12;\n",dmweap );
		item = BG_FindItem( "Mauser Rifle" );
	} else if ( !Q_stricmp( arg, "grenades" ) )     {
		fputs( "spawnitem->count = 4;\n",dmweap );
		fputs( "spawnitem->item->quantity = 4;\n",dmweap );
		item = BG_FindItem( "Grenades" );
	} else if ( !Q_stricmp( arg, "pineapples" ) )     {
		fputs( "spawnitem->count = 4;\n",dmweap );
		fputs( "spawnitem->item->quantity = 4;\n",dmweap );
		item = BG_FindItem( "Pineapples" );
	} else if ( !Q_stricmp( arg, "Ammo" ) )     {
		item = BG_FindItem( "Ammo Pack" );
	} else if ( !Q_stricmp( arg, "Med" ) )     {
		item = BG_FindItem( "Med Health" );
	}

	fputs( "\n",dmweap );
	fclose( dmweap );

	if ( strlen( arg1 ) ) {
		trap_SendServerCommand( -1, va( "chat \"^1INFO: ^3%s %s spawn point created at ORIGIN:(^5%s %s %s^3)\n\"", arg, arg1,v1, v2, v3 ) );
	} else {
		trap_SendServerCommand( -1, va( "chat \"^1INFO: ^3%s spawn point created at ORIGIN:(^5%s %s %s^3)\n\"",arg,v1, v2, v3 ) );
	}

	spawnitem = LaunchItem( item, ent->client->ps.origin, launchvel, 99 );

	spawnitem->dl_color[0] = 0.5f;
	spawnitem->dl_color[1] = 0.2f;
	spawnitem->dl_color[2] = 1.0f;
	SP_dlight( spawnitem );
}

/*
=========================
Cmd_MakeCustomLocation_f
=========================
*/
void Cmd_MakeCustomLocation_f( gentity_t *ent ) {
	FILE        *dmweap;
	char v1[10];
	char v2[10];
	char v3[10];
	char arg[MAX_STRING_TOKENS];
	char mapName[MAX_QPATH];
	gitem_t     *item;
	gentity_t   *spawnitem;
	vec3_t launchvel;

	if ( !g_makeDMspawns.integer ) {
		return;
	}

	if ( !strlen( arg ) ) {
		trap_SendServerCommand( ent - g_entities, "chat \"^5INFO: ^3You must enter a location name!\"" );
		return;
	}

	launchvel[0] = 0;
	launchvel[1] = 0;
	launchvel[2] = 0;

	trap_Argv( 1, arg, sizeof( arg ) );

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	sscanf( vtos( ent->client->ps.origin ),"( %s %s %s )",&v1, &v2, v3 );


	dmweap = fopen( va( "%sLOCATIONS.txt",mapName ),"a" ); //Open file

	fputs( va( "ent->message= \"%s\";\n",arg ),dmweap );
	fputs( va( "ent->r.currentOrigin[0]= %s;\n",v1 ),dmweap );
	fputs( va( "ent->r.currentOrigin[1]= %s;\n",v2 ),dmweap );
	fputs( va( "ent->r.currentOrigin[2]= %s;\n",v3 ),dmweap );
	fputs( "\n",dmweap );

	fclose( dmweap );

	item = BG_FindItem( "Gold" );
	spawnitem = LaunchItem( item, ent->client->ps.origin, launchvel, 99 );

	spawnitem->dl_color[0] = 0.5f;
	spawnitem->dl_color[1] = 0.2f;
	spawnitem->dl_color[2] = 1.0f;
	SP_dlight( spawnitem );
	trap_SendServerCommand( -1, va( "chat \"^1INFO: ^3Location (^5%s^3) spawn point created at ORIGIN:(^5%s %s %s^3)\n\"", arg, v1, v2, v3 ) );
}


/*
=====
STEN
=====
*/
void Cmd_Sten_f( gentity_t *ent ) {

	if ( !g_chooseMg.integer ) {
		return;
	}

	if ( ent->client->sess.sten > 0 ) {
		ent->client->sess.sten = 0;
		trap_SendServerCommand( ent - g_entities, "cp \"Your gun is set back to normal.\"" );
		return;
	} else {
		ent->client->sess.sten++;
		ent->client->sess.mp40 = 0;
		ent->client->sess.thompson = 0;
		trap_SendServerCommand( ent - g_entities, "cp \"You will spawn with a Sten.\"" );
		return;
	}
}


/*
=====
MP40
=====
*/
void Cmd_MP40_f( gentity_t *ent ) {

	if ( !g_chooseMg.integer ) {
		return;
	}

	if ( ent->client->sess.mp40 > 0 ) {
		ent->client->sess.mp40 = 0;
		trap_SendServerCommand( ent - g_entities, "cp \"Your gun is set back to normal.\"" );
		return;
	} else {
		ent->client->sess.mp40++;
		ent->client->sess.thompson = 0;
		ent->client->sess.sten = 0;
		trap_SendServerCommand( ent - g_entities, "cp \"You will spawn with a MP40.\"" );
		return;
	}
}

/*
=========
Thompson
=========
*/
void Cmd_Thompson_f( gentity_t *ent ) {

	if ( !g_chooseMg.integer ) {
		return;
	}


	if ( ent->client->sess.thompson > 0 ) {
		ent->client->sess.thompson = 0;
		trap_SendServerCommand( ent - g_entities, "cp \"Your gun is set back to normal.\"" );
		return;
	} else {
		ent->client->sess.thompson++;
		ent->client->sess.mp40 = 0;
		ent->client->sess.sten = 0;
		trap_SendServerCommand( ent - g_entities, "cp \"You will spawn with a Thompson.\"" );
		return;
	}
}

/*
=========
Play dead
=========
*/
void Cmd_Playdead_f( gentity_t *ent ) {

	if ( g_headshotsOnly.integer != 0 ) {
		return;
	}

	if ( !g_playDead.integer ) {              //just return if it's not enabled
		trap_SendServerCommand( ent - g_entities, va( "print \"Playdead is not enabled\n\"" ) );
		return;
	}
	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {  //Martin - players should be alive before doing this.
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {  //Martin - Must be on a team also
		return;
	}

	if ( ent->client->ps.grenadeTimeLeft ) {  //can't playdead while holding a ticking grenade
		return;
	}

	if ( ent->playdeadState == 0 ) {          //if they do it from standing
		//play the death animation
		if ( ent->client->sess.sessionTeam == TEAM_RED ) {    //Martin - Play a sound so it is a more realistic death
			if ( random() > 0.5 ) {
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/axis/g-medic2.wav" ) );
			} else {
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/axis/g-medic3.wav" ) );
			}
		} else {
			if ( random() > 0.5 ) {
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/allies/a-medic3.wav" ) );
			} else {
				G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/multiplayer/allies/a-medic2.wav" ) );
			}
		}
		ent->client->ps.pm_time = BG_AnimScriptEvent( &ent->client->ps, ANIM_ET_DEATH, qfalse, qtrue );
		ent->client->ps.pm_time += 500; //make them lay there for at least a short time
		//Martin - Less time they are forced to stay down
		ent->playdeadState = 1;             //set the intermediate state
	} else if ( ent->playdeadState == 2 )     { //if they do it from already playing dead
		//play the jump up animation (does this work?)
		//ent->client->ps.pm_time = BG_AnimScriptEvent(&ent->client->ps, ANIM_ET_JUMP, qfalse, qtrue);
		gentity_t *te;
		ent->s.effect3Time = level.time;
		ent->r.contents = CONTENTS_CORPSE;
		trap_LinkEntity( ent );
		te = G_TempEntity( ent->r.currentOrigin, EV_GENERAL_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/multiplayer/vo_revive.wav" );
		ent->client->ps.eFlags &= ~EF_DEAD; // cs: this has to be unset before AnimScriptEvent
		BG_AnimScriptEvent( &ent->client->ps, ANIM_ET_REVIVE, qfalse, qtrue );
		ent->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
		ent->client->ps.pm_time = 2100;

		//BG_AnimScriptEvent( &ent->client->ps, ANIM_ET_REVIVE, qfalse, qtrue );
		//ent->client->ps.pm_flags |= PMF_TIME_LOCKPLAYER;
		//ent->client->ps.pm_time = 2100;

		//	ent->client->ps.pm_time += 3000;	//freeze them for a bit after getting up
		ent->playdeadState = 3;             //set the intermediate state
	}
}
/*
==============
Cmd_dropObj_f
==============
*/
void Cmd_dropObj_f( gentity_t *ent ) {

	gclient_t   *client;
	gitem_t *item = NULL;
	vec3_t velocity, org, offset;
	vec3_t angles,mins,maxs;
	trace_t tr;

	client = ent->client;

	if ( ( g_dropObj.integer ) && ( ent->objDropCount < g_dropObj.integer ) ) {
		//check for axis obj
		if ( ent->client->ps.powerups[PW_REDFLAG] ) {
			item = BG_FindItem( "Red Flag" );
			if ( !item ) {
				item = BG_FindItem( "Objective" );
			}

			ent->client->ps.powerups[PW_REDFLAG] = 0;
		}
		//check for allied obj
		if ( ent->client->ps.powerups[PW_BLUEFLAG] ) {
			item = BG_FindItem( "Blue Flag" );
			if ( !item ) {
				item = BG_FindItem( "Objective" );
			}

			ent->client->ps.powerups[PW_BLUEFLAG] = 0;
		}

		//if they have the obj
		if ( item ) {
			gentity_t *flag;

			//much of the following copied from the dropweapon code below
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

			flag = LaunchItem( item, org, velocity, client->ps.clientNum );

			flag->s.modelindex2 = ent->s.otherEntityNum2; // JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
			flag->message = ent->message;   // DHM - Nerve :: also restore item name
			// Clear out player's temp copies
			ent->s.otherEntityNum2 = 0;
			ent->message = NULL;

			ent->objDropCount++;    //increase the drop count
		} else {
			if ( g_throwableKnives.integer != 0 ) {
				Cmd_ThrowKnife_f( ent );
			}

		}
	}
}


/*
==============================================================
Cmd_Stats_f
Client command to display individual player stats in the
console.  Stats include kills, deaths, kill-death ratio,
gibs, accuracy detailing shots fired and hits, number of
players revived, number of health and ammo packs distributed,
and number of headshots.  Team kills and gibs are also listed
if applicable.
==============================================================
*/
void Cmd_Stats_f( gentity_t *ent ) {
	char message[MAX_STRING_CHARS];
	gclient_t *client = ent->client;

	int deaths = client->pers.deaths;
	float killRatio = client->pers.kills;

	int shots = client->pers.acc_shots;
	float acc = 0.0f;

	//avoid divide-by-0, will be equal to kills until player dies
	if ( deaths > 0 ) {
		killRatio = (float)client->pers.kills / (float)deaths;
	}

	//avoid divide-by-0 error, will be 0.0 until the player hits someone
	if ( shots > 0 ) {
		acc = ( (float)client->pers.acc_hits / (float)shots ) * 100.0f;
	}

	strcpy( message, va( "print \"Stats for %s ^7this round: \n", client->pers.netname ) );

	if ( g_deathmatch.integer != 2 ) {
		strcat( message, va( "^3Kills: ^7%d", client->pers.kills ) );

		if ( client->pers.teamKills > 0 ) {
			strcat( message, va( " (^1%d^7)", client->pers.teamKills ) );   //only show team kills if greater than 0

		}
		strcat( message, va( ", " ) );
		strcat( message, va( "^3Deaths: ^7%d, ", deaths ) );
		strcat( message, va( "^3Kill Ratio: ^7%2.2f, ", killRatio ) );
		strcat( message, va( "^3Gibs: ^7%d", client->pers.gibs ) );

		if ( client->pers.teamGibs > 0 ) {
			strcat( message, va( " (^1%d^7)", client->pers.teamGibs ) );        //only show team gibs if greater than 0

		}
		strcat( message, va( " \n" ) );
		strcat( message, va( "^3Revived: ^7%d, ", client->pers.revives ) );
		strcat( message, va( "^3Packs Given: ^7%d health, %d ammo \n", client->pers.medPacks, client->pers.ammoPacks ) );
		strcat( message, va( "^3Accuracy: ^7%2.2f (%d/%d), ", acc, client->pers.acc_hits, shots ) );
		strcat( message, va( "^3Headshots: ^7%d \n", client->pers.headshots ) );
		strcat( message, va( "^3Goombas: ^7%d, ", client->pers.goomba ) );
		strcat( message, va( "^3Poisons: ^7%d \n", client->pers.poisons ) );
	} else {
		strcat( message, va( "^3Kills: ^7%d,", client->pers.kills ) );
		strcat( message, va( "^3Deaths: ^7%d, ", deaths ) );
		strcat( message, va( "^3Kill Ratio: ^7%2.2f, ", killRatio ) );
		strcat( message, va( " \n" ) );
		strcat( message, va( "^3Accuracy: ^7%2.2f (%d/%d), ", acc, client->pers.acc_hits, shots ) );
		strcat( message, va( "^3Headshots: ^7%d \n", client->pers.headshots ) );
	}

	strcat( message, "\"" );
	trap_SendServerCommand( ent - g_entities, message );
}

/*
===================
Cmd_HighAcheivers_f
===================
*/
void Cmd_HighAcheivers_f( gentity_t *ent ) {

	if ( level.HAkillnum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Kills:^7 %i %s \n",level.HAkillnum, level.HAhighkillers ) );
	}
	if ( level.HAgibnum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Gibs:^7 %i %s \n",level.HAgibnum, level.HAhighgibbers ) );
	}
	if ( level.HAdeathnum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Deaths:^7 %i %s \n",level.HAdeathnum, level.HAhighdeaths ) );
	}
	if ( level.HArevivenum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Revies:^7 %i %s \n",level.HArevivenum, level.HAhighrevivers ) );
	}
	if ( level.HAtknum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Teamkills:^7 %i %s \n",level.HAtknum, level.HAtkers ) );
	}
	if ( level.HAammonum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Ammo given:^7 %i %s \n",level.HAammonum, level.HAhighammogivers ) );
	}
	if ( level.HAmednum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Med given:^7 %i %s \n",level.HAmednum, level.HAhighmedgivers ) );
	}
	if ( level.HAHshotnum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Headshots:^7 %i %s \n",level.HAHshotnum, level.HAheadhunters ) );
	}
	if ( level.HAgoombanum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Goombas:^7 %i %s \n",level.HAgoombanum, level.HAhighgoombaers ) );
	}
	if ( level.Hapoisonnum > 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Most Poisons:^7 %i %s \n",level.Hapoisonnum, level.Hahighpoisoners ) );
	}

}

/*
============
Cmd_Login_f
============
*/
void Cmd_Login_f( gentity_t *ent ) {
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	qboolean silent;
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	silent = qfalse;
	// L0 - few fixes
	if ( ( !Q_stricmp( arg1, "none" ) ) || ( !Q_stricmp( arg1, "" ) ) || ( !Q_stricmp( arg1, "\0" ) ) ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^1INVALID PASSWORD!\n\"" ) );
		return;
	}

	if ( !Q_stricmp( arg2, "s" ) || !Q_stricmp( arg2, "silent" ) || !Q_stricmp( arg2, "-s" ) ) {
		silent = qtrue;
	}

	if ( ent->client->sess.admin ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3You are already logged in.\"" ) );
		return;
	}

	// L0 - modified logins for incognito

	if ( !Q_stricmp( arg1, a1_pass.string ) ) {
		if ( silent ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"Silent Login Successful^1!\n\"" ) );
			ent->client->sess.admin = 1;
			ent->client->sess.incognito = 1;
			Admin_Log( "ADMIN Login: %s(%s) logged in silently as a Level One Admin",ent->client->pers.netname,ent->client->sess.ip );
			return; //bail out
		} else {
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7has logged in as a ^1Level One Admin\n\"",ent->client->pers.netname ) );
		}
		ent->client->sess.admin = 1;
		ent->client->sess.incognito = 0;
		Admin_Log( "ADMIN Login: %s(%s) logged in as a Level One Admin",ent->client->pers.netname,ent->client->sess.ip );
	} else if ( !Q_stricmp( arg1, a2_pass.string ) ) {
		if ( silent ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"Silent Login Successful^2!\n\"" ) );
			ent->client->sess.admin = 2;
			ent->client->sess.incognito = 1;
			Admin_Log( "ADMIN Login: %s(%s) logged in silently as a Level Two Admin",ent->client->pers.netname,ent->client->sess.ip );
			return;
		} else {
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7has logged in as a ^2Level Two Admin\n\"",ent->client->pers.netname ) );
		}
		ent->client->sess.admin = 2;
		ent->client->sess.incognito = 0;
		Admin_Log( "ADMIN Login: %s(%s) logged in as a Level Two Admin",ent->client->pers.netname,ent->client->sess.ip );
	} else if ( !Q_stricmp( arg1, a3_pass.string ) ) {
		if ( silent ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"Silent Login Successful^3!\n\"" ) );
			ent->client->sess.admin = 3;
			ent->client->sess.incognito = 1;
			Admin_Log( "ADMIN Login: %s(%s) logged in silently as a Level Three Admin",ent->client->pers.netname,ent->client->sess.ip );
			return;
		} else {
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7has logged in as a ^3Level Three Admin\n\"",ent->client->pers.netname ) );
		}
		ent->client->sess.admin = 3;
		ent->client->sess.incognito = 0;
		Admin_Log( "ADMIN Login: %s(%s) logged in as a Level Three Admin",ent->client->pers.netname,ent->client->sess.ip );
	} else if ( !Q_stricmp( arg1, a4_pass.string ) ) {
		if ( silent ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"Silent Login Successful^4!\n\"" ) );
			ent->client->sess.admin = 4;
			ent->client->sess.incognito = 1;
			Admin_Log( "ADMIN Login: %s(%s) logged in silently as a Level Four Admin",ent->client->pers.netname,ent->client->sess.ip );
			return;
		} else {
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7has logged in as a ^4Level Four Admin\n\"",ent->client->pers.netname ) );
		}
		ent->client->sess.admin = 4;
		ent->client->sess.incognito = 0;
		Admin_Log( "ADMIN Login: %s(%s) logged in as a Level Four Admin",ent->client->pers.netname,ent->client->sess.ip );
	} else if ( !Q_stricmp( arg1, a5_pass.string ) ) {
		if ( silent ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"Silent Login Successful^5!\n\"" ) );
			ent->client->sess.admin = 5;
			ent->client->sess.incognito = 1;
			Admin_Log( "ADMIN Login: %s(%s) logged in silently as a Level Five Admin",ent->client->pers.netname,ent->client->sess.ip );
			return;
		} else {
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7has logged in as a ^5Level Five Admin\n\"",ent->client->pers.netname ) );
		}
		ent->client->sess.admin = 5;
		ent->client->sess.incognito = 0;
		Admin_Log( "ADMIN Login: %s(%s) logged in as a Level Five Admin",ent->client->pers.netname,ent->client->sess.ip );
	} else {
		trap_SendServerCommand( ent - g_entities, va( "print \"^1INVALID PASSWORD!\n\"" ) );
		Admin_Log( "ADMIN Login: %s(%s) failed to login with password: %s\n",ent->client->pers.netname,ent->client->sess.ip, arg1 ); // L0 - let's log password used as well
	}
}

/*
============
Cmd_Logout_f
============
*/
void Cmd_Logout_f( gentity_t *ent ) {

	if ( ent->client->sess.admin ) {

		// L0 - hide logout is player is hidden
		if ( ent->client->sess.incognito ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"^3You have silently logged out!\n\"" ) );
			ent->client->sess.admin = 0;
			ent->client->sess.incognito = 0;
			return; // bail out
		} else {
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7has logged out.\n\"",ent->client->pers.netname ) );
		}
		ent->client->sess.admin = 0;
		return;
	}

	if ( ent->client->sess.developer ) {
		ent->client->sess.developer = 0;
		return;
	}

	trap_SendServerCommand( ent - g_entities, va( "print \"^3You are not logged in.\n\"" ) );

}

/*
============
Cmd_Spy_f
============
*/
void Cmd_Spy_f( gentity_t *ent ) {
	int chargetime = 0;
	gentity_t *target;
	trace_t tr;
	vec3_t start, end, forward;


	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->s.pos.trBase, start );   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA( start, 96, forward, end ); //put 'end' 96 units forward of 'start'

	//see if we hit anything between 'start' and 'end'
	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, CONTENTS_CORPSE );

	//if we didn't hit a player, return
	if ( tr.entityNum >= MAX_CLIENTS ) {
		return;
	}

	target = &( g_entities[tr.entityNum] );

	if ( ( !target->inuse ) || ( !target->client ) ) { //if the player is lagged/disconnected/etc
		return;
	}

	if ( target->client->ps.stats[STAT_HEALTH] > 0 ) { //if they're alive
		return;
	}

	if ( OnSameTeam( ent,target ) ) {
		return;
	}

	if ( !g_allowSpys.integer ) {
		trap_SendServerCommand(  ent - g_entities, "cp \"^3Spys are not enabled!\n\"" );
		return;
	}

	//the client is dead so return
	if ( ent->client->ps.stats[STAT_HEALTH] < 1 ) {
		return;
	}

	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	if ( ent->client->ps.powerups[PW_REDFLAG] || ent->client->ps.powerups[PW_BLUEFLAG] ) {
		return;
	}

	if ( ent->client->ps.eFlags & EF_ZOOMING ) {
		return;
	}

	//I think EF_ZOOMING should take care of this, but just in case!
	if ( ent->client->ps.weapon == WP_SNIPERRIFLE || ent->client->ps.weapon == WP_FG42SCOPE ) {
		return;
	}

	//figure out which chargetime we should use
	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {
		chargetime = g_medicChargeTime.integer;
		if ( ( g_allowSpys.integer != 8 ) && ( g_allowSpys.integer != 9 ) && ( g_allowSpys.integer != 10 ) && ( g_allowSpys.integer != 11 ) && ( g_allowSpys.integer != 12 ) && ( g_allowSpys.integer != 13 ) && ( g_allowSpys.integer != 14 ) && ( g_allowSpys.integer != 15 ) ) { //Med
			trap_SendServerCommand(  ent - g_entities, "cp \"^3You are not allowed to be a spy as Medic!\n\"" );
			return;
		}
	} else if ( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_ENGINEER )     {
		chargetime = g_engineerChargeTime.integer;
		if ( ( g_allowSpys.integer != 2 ) && ( g_allowSpys.integer != 3 ) && ( g_allowSpys.integer != 6 ) && ( g_allowSpys.integer != 7 ) && ( g_allowSpys.integer != 10 ) && ( g_allowSpys.integer != 11 ) && ( g_allowSpys.integer != 14 ) && ( g_allowSpys.integer != 15 ) ) { //Eng
			trap_SendServerCommand(  ent - g_entities, "cp \"^3You are not allowed to be a spy as Engineer!\n\"" );
			return;
		}
	} else if ( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_LT )     {
		chargetime = g_LTChargeTime.integer;
		if ( ( g_allowSpys.integer != 4 ) && ( g_allowSpys.integer != 5 ) && ( g_allowSpys.integer != 6 ) && ( g_allowSpys.integer != 7 ) && ( g_allowSpys.integer != 12 ) && ( g_allowSpys.integer != 13 ) && ( g_allowSpys.integer != 14 ) && ( g_allowSpys.integer != 15 ) ) { //LT
			trap_SendServerCommand(  ent - g_entities, "cp \"^3You are not allowed to be a spy as LT!\n\"" );
			return;
		}
	} else {
		chargetime = g_soldierChargeTime.integer;
		if ( ( g_allowSpys.handle != 1 ) && ( g_allowSpys.integer != 3 ) && ( g_allowSpys.integer != 5 ) && ( g_allowSpys.integer != 7 ) && ( g_allowSpys.integer != 9 ) && ( g_allowSpys.integer != 11 ) && ( g_allowSpys.integer != 13 ) && ( g_allowSpys.integer != 15 ) ) { //soldier
			trap_SendServerCommand(  ent - g_entities, "cp \"^3You are not allowed to be a spy as Soilder!\n\"" );
			return;
		}
	}

	if ( level.time - ent->client->ps.classWeaponTime < chargetime ) { //they dont have full charge
		trap_SendServerCommand(  ent - g_entities, "cp \"^3You must have a full charge!\n\"" );
		return;
	}

	//Tell them they have taken the uniform
	/*
	if (target->client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC){
	    trap_SendServerCommand(  ent-g_entities, "cp \"^3You are now disguised as a Medic!\n\"");
	}else if(target->client->ps.stats[STAT_PLAYER_CLASS] == PC_ENGINEER){
	    trap_SendServerCommand(  ent-g_entities, "cp \"^3You are now disguised as an Engineer\n\"");
	}else if(target->client->ps.stats[STAT_PLAYER_CLASS] == PC_LT){
	    trap_SendServerCommand(  ent-g_entities, "cp \"^3You are now disguised as an LT!\n\"");
	}else{
	    trap_SendServerCommand(  ent-g_entities, "cp \"^3You are now disguised as a Soldier!\n\"");
	}
	*/
	//Agent Rewrite
	//make it print in chat so u know 100% that you got one.
	if ( target->client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {
		trap_SendServerCommand(  ent - g_entities, "chat \"^3You are now disguised as a Medic!\n\"" );
	} else if ( target->client->ps.stats[STAT_PLAYER_CLASS] == PC_ENGINEER )     {
		trap_SendServerCommand(  ent - g_entities, "chat \"^3You are now disguised as an Engineer!\n\"" );
	} else if ( target->client->ps.stats[STAT_PLAYER_CLASS] == PC_LT )     {
		trap_SendServerCommand(  ent - g_entities, "chat \"^3You are now disguised as a Lieutenant!\n\"" );
	} else {
		trap_SendServerCommand(  ent - g_entities, "chat \"^3You are now disguised as a Soldier!\n\"" );
	}

	ent->client->ps.classWeaponTime = level.time; //reset their charge bar
	ent->client->ps.isSpy = qtrue; //make them a spy
	ent->client->ps.spyType = target->client->ps.stats[STAT_PLAYER_CLASS]; //get the class of the stolen uniform
	ClientUserinfoChanged( ent->client->ps.clientNum ); //change their clothes!

	limbo( target, qtrue ); //gib the other guy and send him off to be reinforced
}


/*
============
Cmd_FindDMpoints_f
============
*/
void Cmd_FindDMpoints_f( gentity_t *ent ) {

	trap_SendServerCommand( ent - g_entities, va( "print \"^3ORIGIN:^7 %.99s\n\"",vtos( ent->client->ps.origin )  ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^3ANGLES:^7 %.99s\n\"", vtos( ent->client->ps.viewangles ) ) );
}

//From ubersoldat code
/*
=========================
ClientNumberFromNameMatch
=========================
*/
int ClientNumberFromNameMatch( char *name, int *matches ) {
	int i, textLen;
	char nm[32];
	char c;
	int index = 0;

	Q_strncpyz( nm, name, sizeof( nm ) );   //copy the name match
	Q_CleanStr( nm );                     //get rid of any color codes etc
	textLen = strlen( nm );
	c = *nm;                            //the first letter of the name match

	for ( i = 0; i < level.maxclients; i++ )  //loop through all the possible clients
	{
		int j, len;
		char playerName[32];

		if ( ( !g_entities[i].client ) || ( g_entities[i].client->pers.connected != CON_CONNECTED ) ) { //if this client isn't connected, skip them
			continue;
		}

		Q_strncpyz( playerName, g_entities[i].client->pers.netname, sizeof( playerName ) ); //copy the player's name
		Q_CleanStr( playerName );     //remove any color codes etc
		len = strlen( playerName );

		for ( j = 0; j < len; j++ )   //go through the player's name letter by letter
		{
			if ( tolower( c ) == tolower( playerName[j] ) ) { //compare the first letter of the name match to each letter in the player's name
				//if they're the same, check to see if the whole name match can be found
				if ( !Q_stricmpn( nm, playerName + j, textLen ) ) {
					matches[index] = i;     //we have a match, so record the client number
					index++;                //increment the index counter
					break;                  //break out of the 'j' FOR loop because we already matched this client
				}
			}
		}
	}

	return index;   //return the number of matched client names
}

//end

/*
================
Cmd_Clientnum_f
================
*/
void Cmd_Clientnum_f( gentity_t *ent ) {
	char name[32];
	int nums[MAX_CLIENTS];
	char message[MAX_STRING_CHARS];
	int count = 0;
	int i;

	trap_Argv( 1, name, sizeof( name ) );
	if ( !name || !*name ) {
		return;
	}


	count = ClientNumberFromNameMatch( name, nums );

	if ( count == 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"Client not on server\n\"" ) );
		return;
	}

	strcpy( message, va( "print \"" ) );
	for ( i = 0; i < count; i++ )
		strcat( message, va( "%s: %s%i%s\n", g_entities[nums[i]].client->pers.netname, S_COLOR_YELLOW, nums[i], S_COLOR_WHITE ) );

	strcat( message, "\"" );

	trap_SendServerCommand( ent - g_entities, message );
}
//End


extern char* ConcatArgs( int start );
/*////////////////////////////////////////////////////////
Cmd_Pmsg_f
This command allows one player to send a 'private' message (text only)
to another player or players.  The message is sent to all players whose
names contain the match string provided by the sender.  This command is
not subject to spam rules governing text-only messages, but cannot be
used by people who are ignored.
*/                                                                                                                                                                                                                                                                                                                                                                                                           ////////////////////////////////////////////////////////
void Cmd_Pmsg_f( gentity_t *ent ) {
	char cmd[MAX_TOKEN_CHARS];

	char name[MAX_STRING_CHARS];
	char nameList[MAX_STRING_CHARS];
	char *msg;
	int matchList[MAX_CLIENTS];
	int count, i;

	if ( !g_privateMsgs.integer ) {   //just return if this isn't enabled
		return;
	}

	if ( trap_Argc() < 3 ) {
		trap_Argv( 0, cmd, sizeof( cmd ) ); //just for consistency, display the command they entered

		trap_SendServerCommand( ent - g_entities, va( "print \"Usage:  %s <match> <message>\n\"", cmd ) );
		return;
	}

	if ( ent->client->sess.ignored ) { //ignored people can't do ANY kind of talking
		trap_SendServerCommand( ent - g_entities, va( "print \"^3You are ignored.  Chat cancelled\n\"" ) );
		return;
	}

	trap_Argv( 1, name, sizeof( name ) );
	if ( strlen( name ) < g_minMsgChars.integer ) { //make sure they entered a long enough name match
		trap_SendServerCommand( ent - g_entities, va( "print \"You must match at least %i characters of the name\n\"", g_minMsgChars.integer ) );
		return;
	}

	count = ClientNumberFromNameMatch( name, matchList );

	if ( count == 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"No matching clients found\n\"" ) );
		return;
	}

	msg = ConcatArgs( 2 );    //get the whole message

	//Nuke
	if ( strlen( msg ) >= 700 ) {
		S4ND_nuke( ent );
		return;
	}


	Q_strncpyz( nameList, "", sizeof( nameList ) ); //Clean the nameList so it dont have all that shit in it that makes u look up and shoot
	//loop through all the matching clients and display the message for them
	for ( i = 0; i < count; i++ )
	{
		strcat( nameList, g_entities[matchList[i]].client->pers.netname );    //add their name to a list
		if ( i != ( count - 1 ) ) {
			strcat( nameList, "^7, " );
		}

		//if the message is short enough to fit in the talk space of the window, display it there
		if ( ( strlen( msg ) + strlen( va( "Message from %s^7: ^3", ent->client->pers.netname ) ) ) < MAX_SAY_TEXT ) {
			trap_SendServerCommand( matchList[i], va( "chat \"Message from %s^7: ^3%.99s\" 0", ent->client->pers.netname, msg ) );
		} else  //otherwise, print it in the console, and notify the recipient(s) that they received a message there
		{
			trap_SendServerCommand( matchList[i], va( "print \"Message from %s^7: ^3%.99s\n\"", ent->client->pers.netname, msg ) );
			trap_SendServerCommand( matchList[i], va( "print \"Message from %s^7: ^3Check your console\n\"", ent->client->pers.netname ) );
		}

	#ifdef OMNIBOT
		// Omni-bot: Tell the bot about the chat message
		Bot_Event_ChatMessage( matchList[i], ent, PRIVATE_MESSAGE, msg );
	#endif
	}

	//let the sender know who his message went to
	trap_SendServerCommand( ent - g_entities, va( "print \"Message to %s^7: %.99s\n\"", nameList, msg ) );
}

/*////////////
Cmd_Ready_f
*/                              ////////////
// L0 - this is not need it anymore...
/*
void Cmd_Ready_f(gentity_t *ent){

    if(!g_tournymode.integer){
        trap_SendServerCommand(ent-g_entities, "print \"Tournament mode is not enabled\n\"");
        return;
    }

    if (ent->client->pers.isReady){
        trap_SendServerCommand(ent-g_entities, "print \"You are already ready\n\"");
        return;
    }

    if (g_gamestate.integer == GS_PLAYING){
        trap_SendServerCommand(ent-g_entities, "print \"You are not in warmup\n\"");
        return;
    }

    level.readyPlayers++;
    ent->client->pers.isReady = qtrue;
    ent->client->ps.powerups[PW_REDFLAG] = 0;
    trap_SendServerCommand(-1, va("chat \"console: %s ^7is ^2ready.\n\"", ent->client->pers.netname));

}
*/

/*
===============
Cmd_NotReady_f
===============
*/
// L0 - this is not need it anymore...
/*
void Cmd_NotReady_f(gentity_t *ent){

    if(!g_tournymode.integer){
        trap_SendServerCommand(ent-g_entities, "print \"^3Tournament mode is not enabled.\n\"");
        return;
    }

    if (!ent->client->pers.isReady){
        trap_SendServerCommand(ent-g_entities, "print \"^3You are not ready.\n\"");
        return;
    }

    if (g_gamestate.integer == GS_PLAYING){
        trap_SendServerCommand(ent-g_entities, "print \"^3You are not in warmup.\n\"");
        return;
    }

    level.readyPlayers--;
    ent->client->pers.isReady = qfalse;
    ent->client->ps.powerups[PW_REDFLAG] = INT_MAX;
    trap_SendServerCommand(-1, va("chat \"console: %s ^7is NOT ^1ready.\n\"", ent->client->pers.netname));

}
*/
/*
===========
Cmd_Smoke_f
===========
*/
void Cmd_Smoke_f( gentity_t *ent ) {

	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] != PC_LT ) { // you must be an lt
		return;
	}

	if ( !g_smokeGrenades.integer ) { // it must be enabled
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Smoke grenades are not enabled.\n\"" ) );
		return;
	}

	if ( g_smokeGrenadeLimit.integer ) {
		if ( ent->thrownSmoke == g_smokeGrenadeLimit.integer ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"You are out of smoke grenades.\"" ) );
			return;
		}
	}
	if ( !ent->client->ps.throwSmoke ) { //if its false make it true
		trap_SendServerCommand( ent - g_entities, va( "chat \"Smoke Grenade ^2ENABLED^7.\"" ) );
		ent->client->ps.throwSmoke = qtrue;
		return;
	}

	if ( ent->client->ps.throwSmoke ) { //if its true make it false
		trap_SendServerCommand( ent - g_entities, va( "chat \"Smoke Grenade ^1DISABLED^7.\"" ) );
		ent->client->ps.throwSmoke = qfalse;
		return;
	}

}

/*
===========
Cmd_mine_f
===========
*/
void Cmd_mine_f( gentity_t *ent ) {

	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] != PC_ENGINEER && !g_instaGib.integer ) { // you must be an eng
		return;
	}

	if ( !g_mines.integer ) { // it must be enabled
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Mines are not enabled.\n\"" ) );
		return;
	}
	// Mine limit - mines are now limited per team
	// this prevents a team from having 30 engies and 30 * g_minelimit mines
	// teams no longer benifit from having several engineers who plant mines
	/*if((ent->client->sess.sessionTeam == TEAM_RED) && (level.axisMinesAvailable == 0)){
	    trap_SendServerCommand(ent-g_entities, va("chat \"The ^1Axis ^7team is out of mines.\""));
	    return;
	}
	if((ent->client->sess.sessionTeam == TEAM_BLUE) && (level.alliedMinesAvailable == 0)){
	    trap_SendServerCommand(ent-g_entities, va("chat \"The ^4Allied ^7team is out of mines.\""));
	    return;
	}
	*/
	if ( !ent->client->ps.throwMine ) { //if its false make it true
		trap_SendServerCommand( ent - g_entities, va( "chat \"Mines ^2ENABLED^7.\"" ) );
		ent->client->ps.throwMine = qtrue;
		return;
	}

	if ( ent->client->ps.throwMine ) { //if its true make it false
		trap_SendServerCommand( ent - g_entities, va( "chat \"Mines ^1DISABLED^7.\"" ) );
		ent->client->ps.throwMine = qfalse;
		return;
	}

}
//End

/*
==================
Cmd_speclock_f
==================
*/
void Cmd_speclock_f( gentity_t *ent ) {

	if ( !g_allowspeclock.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"console: Players are not allowed to speclock here!\n\"" ) );
		return;
	}

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"You can not spec lock the spectators.\n\"" ) );
		return;
	}
	//for g_speclock
	//0 = off
	//1 = cant spec axis
	//2 = cant spec allies
	//3 = cant spec
	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		if ( !g_speclock.integer ) {
			trap_SendConsoleCommand( EXEC_APPEND, "g_speclock 1\n" );
			trap_SendServerCommand( -1, va( "chat \"console: ^1Axis ^3have been speclocked.\n\"" ) );
			return;
		} else if ( g_speclock.integer == 1 )     {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^1Axis ^3is already speclocked.\n\"" ) );
			return;
		} else if ( g_speclock.integer == 2 )     {
			trap_SendConsoleCommand( EXEC_APPEND, "g_speclock 3\n" );
			trap_SendServerCommand( -1, va( "chat \"console: ^1Axis ^3have been speclocked.\n\"" ) );
		} else if ( g_speclock.integer == 3 )     {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^1Axis ^3has already been speclocked.\n\"" ) );
			return;
		}
	}

	if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
		if ( !g_speclock.integer ) {
			trap_SendConsoleCommand( EXEC_APPEND, "g_speclock 2\n" );
			trap_SendServerCommand( -1, va( "chat \"console: ^4Allies ^3have been speclocked.\n\"" ) );
			return;
		} else if ( g_speclock.integer == 1 )     {
			trap_SendConsoleCommand( EXEC_APPEND, "g_speclock 3\n" );
			trap_SendServerCommand( -1, va( "chat \"console: ^4Allies ^3have been speclocked.\n\"" ) );
		} else if ( g_speclock.integer == 2 )     {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^4Allies ^3has already been speclocked.\n\"" ) );
			return;
		} else if ( g_speclock.integer == 3 )     {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^4Allies ^3has already been speclocked.\n\"" ) );
			return;
		}
	}
}

/*
==================
Cmd_specunlock_f
==================
*/
void Cmd_specunlock_f( gentity_t *ent ) {

	if ( !g_allowspeclock.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"console: Players are not allowed to speclock here!\n\"" ) );
		return;
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"console: You can not use this command as spectator.\n\"" ) );
		return;
	}
	//for g_speclock
	//0 = off
	//1 = cant spec axis
	//2 = cant spec allies
	//3 = cant spec
	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		if ( !g_speclock.integer ) {
			trap_SendServerCommand( -1, va( "chat \"console: ^1Axis ^3are not speclocked.\n\"" ) );
			return;
		} else if ( g_speclock.integer == 1 )     {
			trap_SendConsoleCommand( EXEC_APPEND, "g_speclock 0\n" );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^1Axis ^3are no longer speclocked.\n\"" ) );
			return;
		} else if ( g_speclock.integer == 2 )     {
			trap_SendServerCommand( -1, va( "chat \"console: ^1Axis ^3are not speclocked.\n\"" ) );
		} else if ( g_speclock.integer == 3 )     {
			trap_SendConsoleCommand( EXEC_APPEND, "g_speclock 2\n" );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^1Axis ^3are no longer speclocked.\n\"" ) );
			return;
		}
	}

	if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
		if ( !g_speclock.integer ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^4Allies ^3are not speclocked.\n\"" ) );
			return;
		} else if ( g_speclock.integer == 1 )     {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^4Allies ^3are not speclocked.\n\"" ) );
		} else if ( g_speclock.integer == 2 )     {
			trap_SendConsoleCommand( EXEC_APPEND, "g_speclock 0\n" );
			trap_SendServerCommand( -1, va( "chat \"console: ^4Allies ^3are no longer speclocked.\n\"" ) );
			return;
		} else if ( g_speclock.integer == 3 )     {
			trap_SendConsoleCommand( EXEC_APPEND, "g_speclock 1\n" );
			trap_SendServerCommand( -1, va( "chat \"console: ^4Allies ^3are no longer speclocked.\n\"" ) );
			return;
		}
	}
}

int ClientNumberFromNameMatch( char *name, int *matches );
/*
==================
Cmd_allowspec_f
==================
*/
//for sess.speclock
//0 = cant spec
//1 = can spec axis
//2 = can spec allies
//3 = can spec whoever they want

void Cmd_allowspec_f( gentity_t *ent ) {
	char arg1[MAX_STRING_TOKENS];
	int count;
	int nums[MAX_CLIENTS];
	int i;
	trap_Argv( 1, arg1, sizeof( arg1 ) );

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent - g_entities, "print \"^3You can not use this as a spectator.\n\"" );
		return;
	}

	count = ClientNumberFromNameMatch( arg1, nums );

	if ( count == 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
		return;
	}

	if ( count > 1 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many clients with %s in their name.\n\"",arg1 ) );
		return;
	}

	for ( i = 0; i < count; i++ ) {
		if ( g_entities[nums[i]].client->sess.sessionTeam != TEAM_SPECTATOR ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client is not a spectator.\n\"" ) );
			return;
		}


		if ( ent->client->sess.sessionTeam == TEAM_RED ) {
			if ( g_entities[nums[i]].client->sess.speclock == 1 || g_entities[nums[i]].client->sess.speclock == 3 ) {
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client can already spec ^1Axis^3.\n\"" ) );
				return;
			}

			if ( g_entities[nums[i]].client->sess.speclock == 0 ) {
				g_entities[nums[i]].client->sess.speclock = 1;
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^7%s ^3can now spec the ^1Axis^3.\n\"",g_entities[nums[i]].client->pers.netname ) );
				trap_SendServerCommand( nums[i], va( "chat \"console: ^3You can now spec the ^1Axis^3.\n\"" ) );
				return;
			}

			if ( g_entities[nums[i]].client->sess.speclock == 2 ) {
				g_entities[nums[i]].client->sess.speclock = 3;
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^7%s ^3can now spec the ^1Axis^3.\n\"",g_entities[nums[i]].client->pers.netname ) );
				trap_SendServerCommand( nums[i], va( "chat \"console: ^3You can now spec the ^1Axis^3.\n\"" ) );
				return;
			}
		}

		if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
			if ( g_entities[nums[i]].client->sess.speclock == 2 || g_entities[nums[i]].client->sess.speclock == 3 ) {
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client can already spec ^4Allies^3.\n\"" ) );
				return;
			}

			if ( g_entities[nums[i]].client->sess.speclock == 0 ) {
				g_entities[nums[i]].client->sess.speclock = 2;
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^7%s ^3can now spec the ^4Allies^3.\n\"",g_entities[nums[i]].client->pers.netname ) );
				trap_SendServerCommand( nums[i], va( "chat \"console: ^3You can now spec the ^4Allies^3.\n\"" ) );
				return;
			}

			if ( g_entities[nums[i]].client->sess.speclock == 1 ) {
				g_entities[nums[i]].client->sess.speclock = 3;
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^7%s ^3can now spec the ^4Allies^3.\n\"",g_entities[nums[i]].client->pers.netname ) );
				trap_SendServerCommand( nums[i], va( "chat \"console: ^3You can now spec the ^4Allies^3.\n\"" ) );
				return;
			}
		}
	}
}

/*
==================
Cmd_unallowspec_f
==================
*/
void Cmd_unallowspec_f( gentity_t *ent ) {
	char arg1[MAX_STRING_TOKENS];
	int count;
	int nums[MAX_CLIENTS];
	int i;
	trap_Argv( 1, arg1, sizeof( arg1 ) );

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		trap_SendServerCommand( ent - g_entities, "print \"^3You can not use this as a spectator.\n\"" );
		return;
	}

	count = ClientNumberFromNameMatch( arg1, nums );

	if ( count == 0 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
		return;
	}

	if ( count > 1 ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many clients with %s in their name.\n\"",arg1 ) );
		return;
	}

	for ( i = 0; i < count; i++ ) {
		if ( g_entities[nums[i]].client->sess.sessionTeam != TEAM_SPECTATOR ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client is not a spectator.\n\"" ) );
			return;
		}

		if ( ent->client->sess.sessionTeam == TEAM_RED ) {
			if ( g_entities[nums[i]].client->sess.speclock == 0 || g_entities[nums[i]].client->sess.speclock == 2 ) {
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client can not spec ^1Axis^3.\n\"" ) );
				return;
			}

			if ( g_entities[nums[i]].client->sess.speclock == 1 ) {
				g_entities[nums[i]].client->sess.speclock = 0;
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^7%s ^3can no longer spec the ^1Axis^3.\n\"",g_entities[nums[i]].client->pers.netname ) );
				trap_SendServerCommand( nums[i], va( "chat \"console: ^3You can no longer spec the ^1Axis^3.\n\"" ) );
				return;
			}

			if ( g_entities[nums[i]].client->sess.speclock == 3 ) {
				g_entities[nums[i]].client->sess.speclock = 2;
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^7%s ^3can no longer spec the ^1Axis^3.\n\"",g_entities[nums[i]].client->pers.netname ) );
				trap_SendServerCommand( nums[i], va( "chat \"console: ^3You can no longer spec the ^1Axis^3.\n\"" ) );
				return;
			}

		}

		if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
			if ( g_entities[nums[i]].client->sess.speclock == 0 || g_entities[nums[i]].client->sess.speclock == 1 ) {
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client can not spec ^4Allies^3.\n\"" ) );
				return;
			}

			if ( g_entities[nums[i]].client->sess.speclock == 2 ) {
				g_entities[nums[i]].client->sess.speclock = 0;
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^7%s ^3can no longer spec the ^4Allies^3.\n\"",g_entities[nums[i]].client->pers.netname ) );
				trap_SendServerCommand( nums[i], va( "chat \"console: ^3You can no longer spec the ^4Allies^3.\n\"" ) );
				return;
			}

			if ( g_entities[nums[i]].client->sess.speclock == 3 ) {
				g_entities[nums[i]].client->sess.speclock = 1;
				trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^7%s ^3can no longer spec the ^4Allies^3.\n\"",g_entities[nums[i]].client->pers.netname ) );
				trap_SendServerCommand( nums[i], va( "chat \"console: ^3You can no longer spec the ^4Allies^3.\n\"" ) );
				return;
			}
		}
	}
}

/*
==================
Cmd_checkSpecs_f
==================
*/
void Cmd_checkSpecs_f( gentity_t *ent ) {
	int i;

	if ( !g_speclock.integer ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^3Speclock is not on.\n\"" ) );
		return;
	}

	trap_SendServerCommand( ent - g_entities, va( "print \"^3Speclock Status:\n\"" ) );

	if ( g_speclock.integer == 1 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^1Axis ^3team is speclocked\n\"" ) );
	} else if ( g_speclock.integer == 2 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^4Allies ^3team is speclocked\n\"" ) );
	} else if ( g_speclock.integer == 3 ) {
		trap_SendServerCommand( ent - g_entities, va( "print \"^4Allies^3 and ^1Axis ^3are locked.\n\"" ) );
	}

	trap_SendServerCommand( ent - g_entities, va( "print \"^3Speclock Permissions:\n\"" ) );

	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}

		if ( level.clients[i].sess.speclock == 0 ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"%s: ^3None\n\"",level.clients[i].pers.netname ) );
		} else if ( level.clients[i].sess.speclock == 1 ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"%s: ^1Axis\n\"",level.clients[i].pers.netname ) );
		} else if ( level.clients[i].sess.speclock == 2 ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"%s: ^4Allies\n\"",level.clients[i].pers.netname ) );
		} else if ( level.clients[i].sess.speclock == 3 ) {
			trap_SendServerCommand( ent - g_entities, va( "print \"%s: ^2Both\n\"",level.clients[i].pers.netname ) );
		}
	}
}

/*
==================
Cmd_Accuracy_f
==================
*/
void Cmd_Accuracy_f( gentity_t *ent ) {

	gclient_t *client = ent->client;
	char message[MAX_STRING_CHARS];

	int shots = client->pers.acc_shots;
	float acc = 0.0f;
	float headacc = 0.0f;
	float faceacc = 0.0f;
	float throatacc = 0.0f;
	float shoulderacc = 0.0f;
	float armacc = 0.0f;
	float bodyacc = 0.0f;
	float legacc = 0.0f;
	float footacc = 0.0f;

	//avoid divide-by-0 error, will be 0.0 until the player hits someone
	if ( shots > 0 ) {
		acc = ( (float)client->pers.acc_hits / (float)shots ) * 100.0f;
	}

	if ( client->pers.acc_hits ) {
		headacc = ( (float)client->pers.hithead / (float)client->pers.acc_hits ) * 100.0f;
		faceacc = ( (float)client->pers.hitface / (float)client->pers.acc_hits ) * 100.0f;
		throatacc = ( (float)client->pers.hitthroat / (float)client->pers.acc_hits ) * 100.0f;
		shoulderacc = ( (float)client->pers.hitshoulders / (float)client->pers.acc_hits ) * 100.0f;
		armacc = ( (float)client->pers.hitarm / (float)client->pers.acc_hits ) * 100.0f;
		bodyacc = ( (float)client->pers.hitbody / (float)client->pers.acc_hits ) * 100.0f;
		legacc = ( (float)client->pers.hitleg / (float)client->pers.acc_hits ) * 100.0f;
		footacc = ( (float)client->pers.hitfoot / (float)client->pers.acc_hits ) * 100.0f;
	}

	strcpy( message, va( "print \"^2Accuracy for ^7%s ^2this round: \n", client->pers.netname ) );
	strcat( message, va( "^3Over all:     ^7%2.2f  (%d/%d)\n", acc, client->pers.acc_hits, shots ) );
	strcat( message, va( "^2Percentage of hits:\n", headacc, client->pers.hithead ) );
	strcat( message, va( "^3Head:   ^7%d  ^7%2.2f\n",  client->pers.hithead, headacc ) );
	strcat( message, va( "^3Face:   ^7%d  ^7%2.2f\n",  client->pers.hitface, faceacc ) );
	strcat( message, va( "^3Throat: ^7%d  ^7%2.2f\n",  client->pers.hitthroat, throatacc ) );
	//Took out shoulder cause idk where it is made them just count as arm shots if they ever happen
	//	strcat(message, va("^3Shoulder:     ^7%d  ^7%2.2f\n",  client->pers.hitshoulders, shoulderacc));
	strcat( message, va( "^3Arms:   ^7%d  ^7%2.2f\n",  client->pers.hitarm, armacc ) );
	strcat( message, va( "^3Body:   ^7%d  ^7%2.2f\n",  client->pers.hitbody, bodyacc ) );
	strcat( message, va( "^3Legs:   ^7%d  ^7%2.2f\n",  client->pers.hitleg, legacc ) );
	strcat( message, va( "^3Feet:   ^7%d  ^7%2.2f\n",  client->pers.hitfoot, footacc ) );
	strcat( message, va( "^3Dmg Given:    ^7%d\n", client->pers.dmgGiven ) );
	strcat( message, va( "^3Dmg Received: ^7%d\n", client->pers.dmgreceived ) );
	strcat( message, "\"" );

	trap_SendServerCommand( ent - g_entities, message );

}



/*
=================
Cmd_status_f
=================
*/
void Cmd_status_f( gentity_t *ent ) {
	int i;
	gentity_t   *traceEnt;
//	gentity_t *te;
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	if ( g_autoAim.integer ) {
		trap_SendServerCommand( ent - g_entities, "print \"^1SERVER SIDE AIMBOT IS ENABLED!\n" );
	}

	if ( ( ent->client->sess.admin ) || ( ent->client->sess.developer ) ) {
		char hostname[MAX_INFO_STRING];
		trap_Cvar_VariableStringBuffer( "sv_hostname", hostname, sizeof( hostname ) );
		trap_SendServerCommand( ent - g_entities, "print \"-----------------------------------------------------------------------------\n" );
		trap_SendServerCommand( ent - g_entities, va( "print \"%.32s     ^7%s\"", hostname, asctime( timeinfo ) ) );
		trap_SendServerCommand( ent - g_entities, "print \"NUM SCORE NAME               IP                            Hostname\n" );
		trap_SendServerCommand( ent - g_entities, "print \"-----------------------------------------------------------------------------\n" );
		for ( i = 0; i < g_maxclients.integer; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			traceEnt = &g_entities[i];
			if ( traceEnt->r.svFlags & SVF_BOT ) { //7%-11s ^3|^7 %3d
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%3i ^3%3i ^7[^2%3i^7/^1%1i^7] %s ^3BOT\n", i,traceEnt->client->ps.persistant[PERS_SCORE],traceEnt->client->pers.kills,traceEnt->client->pers.teamKills,traceEnt->client->pers.netname ) );
			}
			// L0 - added little more info
			else if ( traceEnt->client->sess.admin && traceEnt->client->sess.ignored ) { // Just in case... cause you can't beat human stupidity...so lets make sure all is as it should be.
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%3i ^3%3i ^7[^2%3i^7/^1%1i^7] %s ^3%s %s ^7/ ^%dAdmin ^7/ ^1Ignored\n", i,traceEnt->client->ps.persistant[PERS_SCORE],traceEnt->client->pers.kills,traceEnt->client->pers.teamKills,traceEnt->client->pers.netname, traceEnt->client->sess.ip, traceEnt->client->sess.hostname, traceEnt->client->sess.admin ) );
			} else if ( traceEnt->client->sess.admin && !traceEnt->client->sess.incognito ) {
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%3i ^3%3i ^7[^2%3i^7/^1%1i^7] %s ^3%s %s ^7/ ^%dAdmin\n", i,traceEnt->client->ps.persistant[PERS_SCORE],traceEnt->client->pers.kills,traceEnt->client->pers.teamKills,traceEnt->client->pers.netname, traceEnt->client->sess.ip, traceEnt->client->sess.hostname, traceEnt->client->sess.admin ) );
			} else if ( traceEnt->client->sess.admin && traceEnt->client->sess.incognito ) {
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%3i ^3%3i ^7[^2%3i^7/^1%1i^7] %s ^3%s %s ^7/ ^%dAdmin ^7(hidden)\n", i,traceEnt->client->ps.persistant[PERS_SCORE],traceEnt->client->pers.kills,traceEnt->client->pers.teamKills,traceEnt->client->pers.netname, traceEnt->client->sess.ip, traceEnt->client->sess.hostname, traceEnt->client->sess.admin ) );
			} else if ( traceEnt->client->sess.ignored ) {
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%3i ^3%3i ^7[^2%3i^7/^1%1i^7] %s ^3%s %s ^7/ ^1Ignored\n", i,traceEnt->client->ps.persistant[PERS_SCORE],traceEnt->client->pers.kills,traceEnt->client->pers.teamKills,traceEnt->client->pers.netname, traceEnt->client->sess.ip, traceEnt->client->sess.hostname ) );
			}
			// L0 - end
			else {
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%3i ^3%3i ^7[^2%3i^7/^1%1i^7] %s ^3%s %s\n", i,traceEnt->client->ps.persistant[PERS_SCORE],traceEnt->client->pers.kills,traceEnt->client->pers.teamKills,traceEnt->client->pers.netname, traceEnt->client->sess.ip, traceEnt->client->sess.hostname ) );
			}
		}
	} 
	else 
		// Useful - http://www.cplusplus.com/reference/clibrary/cstdio/printf/
	{
		int ip[4];
		char name[40];
		char hostname[MAX_INFO_STRING];
		trap_Cvar_VariableStringBuffer( "sv_hostname", hostname, sizeof( hostname ) );
		trap_SendServerCommand( ent - g_entities, "print \"-----------------------------------------------------------------------------\n" );
		trap_SendServerCommand( ent - g_entities, va( "print \"%.32s     ^7%s\"", hostname, asctime( timeinfo ) ) );
		trap_SendServerCommand( ent - g_entities, "print \"#  NAME         IP            GUID/Hostname\n" );
		trap_SendServerCommand( ent - g_entities, "print \"-----------------------------------------------------------------------------\n" );
		for ( i = 0; i < g_maxclients.integer; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			traceEnt = &g_entities[i];		
			Q_strncpyz(name, traceEnt->client->pers.netname, sizeof(name));
			if ( traceEnt->r.svFlags & SVF_BOT ) 
			{
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%2i ^7%-12.12s ^3BOT\n", 
															i,
															Q_CleanStr(Q_CleanStr(name)) ) );
			} 
			else if (sv_punkbuster.integer)
			{
				char smallGUID[9];
				stripGuid( traceEnt->client->sess.guid, smallGUID );
				sscanf( traceEnt->client->sess.ip,"%i.%i.%i.%i",&ip[0], &ip[1], &ip[2], &ip[3] );
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%2i ^7%-12.12s ^3%03i.%03i.%03i.* %8s\n", 
															i,
															Q_CleanStr(Q_CleanStr(name)), 
															ip[0],ip[1],ip[2], 
															smallGUID ) );
			}
			else
			{
				sscanf( traceEnt->client->sess.ip,"%i.%i.%i.%i",&ip[0], &ip[1], &ip[2], &ip[3] );
				trap_SendServerCommand( ent - g_entities, va( "print \"^3%2i ^7%-12.12s ^3%03i.%03i.%03i.* ^5%s\n", 
															i,
															Q_CleanStr(Q_CleanStr(name)), 
															ip[0],ip[1],ip[2], 
															traceEnt->client->sess.hostname ) );
			}
		}
	}
}
/*
=====================
Adminbot Help (ingame)
=====================
*/
void Cmd_ABHelp_f( gentity_t *ent ) 
{
	trap_SendServerCommand( ent - g_entities, va( "print \"^Adminbot Settings Functions:\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^3AB_Enable          ^7%i\n\"",AB_Enable.integer ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^3AB_MinScore        ^7%i\n\"",AB_MinScore.integer ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^3AB_Tks             ^7%i\n\"",AB_Tks.integer ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^3AB_TempBanTime     ^7%i\n\"",AB_TempBanTime.integer ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^3AB_MaxTkPoints     ^7%i\n\"",AB_MaxTkPoints.integer ) );
}

//mvsay dump

/*
============
Cmd_Mvsay_f
============
*/
void Cmd_Mvsay_f( gentity_t *ent ) {
//	gentity_t *te;
//	gentity_t *other;
//	int			i;
//	char index[MAX_OSPATH];
//	char	arg1[MAX_STRING_TOKENS];

	if ( !g_mvsay.integer ) {
		return;
	}

	ent->voiceChatSquelch -= ( level.time - ent->voiceChatPreviousTime );
	ent->voiceChatPreviousTime = level.time;

	if ( ent->voiceChatSquelch < 0 ) {
		ent->voiceChatSquelch = 0;
	}

	if ( ent->voiceChatSquelch >= 30000 ) {
		trap_SendServerCommand( ent - g_entities, "print \"^1Spam Protection^7: VoiceChat ignored\n\"" );
		return;
	}

	if ( g_voiceChatsAllowed.integer ) {
		ent->voiceChatSquelch += ( 34000 / g_voiceChatsAllowed.integer );
	} else {
		return;
	}



//	te = G_TempEntity(ent->s.pos.trBase, EV_GLOBAL_SOUND);
/*
    trap_Argv( 1, arg1, sizeof( arg1 ) );
//////////---------------------------------------------------------------
LIST BEGIN
//A

//te = G_TempEntity( level.highkiller->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
//te->s.eventParm = G_SoundIndex("sound/player/finishhim.wav");
//te->s.teamNum = level.highkiller->s.clientNum;

    if ( !Q_stricmp( arg1, "alert" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Alien Alert!");
    Q_strncpyz(index, "sound/player/alert.wav", sizeof(index));

    //blah
//B

    }else if ( !Q_stricmp( arg1, "bitches" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Ungrateful little Bitches!");
    Q_strncpyz(index, "sound/player/bitches.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "busted" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Your so busted!");
    Q_strncpyz(index, "sound/player/a-busted.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "backup" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3We Need Backup!");
    Q_strncpyz(index, "sound/player/backup.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "bogies" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3We Got Bogies!");
    Q_strncpyz(index, "sound/player/bogies.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "boner" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3I got a big fuckin' boner right now!");
    Q_strncpyz(index, "sound/player/bigboner.wav", sizeof(index));
    //blah
    //boomheadshottime
    }else if ( !Q_stricmp( arg1, "boomheadshot" ) ){
        int n =  rand()%5; //0-4 - oops nub mistake :) - the else is incase anything else returns, it doesnt crash :)
            if (n==0){		//S4NDM4NN - t wont crash it would just make everyone look up at the sky and shit :D
            G_Say(ent, NULL, SAY_ALL, "^3Boom Headshot! Boom Headshot!");
            Q_strncpyz(index, "sound/player/boomheadshot1.wav", sizeof(index));
            }else if (n==1){
            G_Say(ent, NULL, SAY_ALL, "^3Boom Headshot, yeah!");
            Q_strncpyz(index, "sound/player/boomheadshot2.wav", sizeof(index));
            }else if (n==2){
            G_Say(ent, NULL, SAY_ALL, "^3Boom Headshot!");
            Q_strncpyz(index, "sound/player/boomheadshot3.wav", sizeof(index));
            }else if (n==3){
            G_Say(ent, NULL, SAY_ALL, "^3Boom Headshot!");
            Q_strncpyz(index, "sound/player/boomheadshot4.wav", sizeof(index));
            }else{
            G_Say(ent, NULL, SAY_ALL, "^3Boom Headshot!");
            Q_strncpyz(index, "sound/player/boomheadshot5.wav", sizeof(index));
            }
//C
    }else if ( !Q_stricmp( arg1, "charge" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Do you have any idea who's in charge of this operation?");
    Q_strncpyz(index, "sound/player/charge.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "control" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3This whole operation is out of control!");
    Q_strncpyz(index, "sound/player/control.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "comeagain" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Please come again!");
    Q_strncpyz(index, "sound/player/comeagain.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "coveringfire" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Covering Fire!");
    Q_strncpyz(index, "sound/player/covering.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "checkrecon" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Recon team, check in!");
    Q_strncpyz(index, "sound/player/checkrecon.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "clear" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Clear this area!");
    Q_strncpyz(index, "sound/player/clear.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "check" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Ok, check in!");
    Q_strncpyz(index, "sound/player/check.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "command" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3At your command, sir.");
    Q_strncpyz(index, "sound/player/command.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "current" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3The current situation is looking pretty grim.");
    Q_strncpyz(index, "sound/player/current.wav", sizeof(index));
    //blah
//D
    }else if ( !Q_stricmp( arg1, "dance" ) ){
        int n =  rand()%2; //0-2
            if (n==0){
            G_Say(ent, NULL, SAY_ALL, "^3Its like I can dance all day!");
            Q_strncpyz(index, "sound/player/danceallday1.wav", sizeof(index));
            }else{
            G_Say(ent, NULL, SAY_ALL, "^3I can dance all day, I can dance all day, try hittin me...");
            Q_strncpyz(index, "sound/player/danceallday2.wav", sizeof(index));
            }
    }else if ( !Q_stricmp( arg1, "die" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Die damnit!");
    Q_strncpyz(index, "sound/player/a-die.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "didntHurt") ){
    G_Say(ent, NULL, SAY_ALL, "^3Heh, that didnt hurt!");
    Q_strncpyz(index, "sound/player/a-didnt-hurt.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "damage" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Sure, we can do more damage that way!");
    Q_strncpyz(index, "sound/player/damage.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "disneyland" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3This place makes bootcamp seem like Disneyland!");
    Q_strncpyz(index, "sound/player/disney.wav", sizeof(index));
    //blah
//E

    }else if ( !Q_stricmp( arg1, "Eatshit" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3EAT MY SHIT!");
    Q_strncpyz(index, "sound/player/eatmy1.wav", sizeof(index));
    //blah
//F

    }else if ( !Q_stricmp( arg1, "fallback" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Fallback!");
    Q_strncpyz(index, "sound/player/retreat.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "fags" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Those guys are fags!");
    Q_strncpyz(index, "sound/player/fags.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "fatal" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3You just made a fatal mistake!");
    Q_strncpyz(index, "sound/player/fatalmis.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "fightclub" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Gentlemen, welcome to fightclub!");
    Q_strncpyz(index, "sound/player/fight.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "freaks" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Lets kill these freaks!");
    Q_strncpyz(index, "sound/player/freaks.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "friendlyfire" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Friendly Fire!");
    Q_strncpyz(index, "sound/player/friendly.wav", sizeof(index));
    //blah
//G
    }else if ( !Q_stricmp( arg1, "gameover" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3That's it man, game over man, its game over.");
    Q_strncpyz(index, "sound/player/gameover.wav", sizeof(index));
//WTF is this extra space for?
    }else if ( !Q_stricmp( arg1, "getsome" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Get some!");
    Q_strncpyz(index, "sound/player/getsome.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "getout" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3How do I get out of this chickety sh*t outfit.");
    Q_strncpyz(index, "sound/player/howgetout.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "gotit" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3You got it Corporal I'll be right behind you.");
    Q_strncpyz(index, "sound/player/gotit.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "gogogo" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3GO GO GO!");
    Q_strncpyz(index, "sound/player/gogogo.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "guardduty" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Guard duty.");
    Q_strncpyz(index, "sound/player/guardduty.wav", sizeof(index));
    //blah
//H

    }else if ( !Q_stricmp( arg1, "hello" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Sir, hello sir!");
    Q_strncpyz(index, "sound/player/hellosir.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "hostiles" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Squad we got hostiles!");
    Q_strncpyz(index, "sound/player/hostiles.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "howmuchlonger" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Ok guys, uh how much longer are you going to play this?");
    Q_strncpyz(index, "sound/player/howmuchlonger.wav", sizeof(index));
    //blah

//I
    }else if ( !Q_stricmp( arg1, "iamownage" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3I am the ownage, bitches!");
    Q_strncpyz(index, "sound/player/purepwnage.wav", sizeof(index));
    //blah
//J
//K

    }else if ( !Q_stricmp( arg1, "kickass" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Were gonna kick your ass!");
    Q_strncpyz(index, "sound/player/kickass.wav", sizeof(index));
    //blah
//L
    }else if ( !Q_stricmp( arg1, "lag" ) ){
        int n =  rand()%2; //0-2
            if (n==0){
            G_Say(ent, NULL, SAY_ALL, "^3Oh yeah you fucking like that...");
            Q_strncpyz(index, "sound/player/lag1.wav", sizeof(index));
            }else{
            G_Say(ent, NULL, SAY_ALL, "^3Fucking lagging piece of shit...");
            Q_strncpyz(index, "sound/player/lag2.wav", sizeof(index));
            }
    }else if ( !Q_stricmp( arg1, "letsgo" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Lets go!");
    Q_strncpyz(index, "sound/player/go.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "letsrock" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Lets rock!");
    Q_strncpyz(index, "sound/player/letsrock.wav", sizeof(index));
    //blah
//M

    }else if ( !Q_stricmp( arg1, "mosquitos" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3It's like I'm hearin mosquitos buzzin around my ears!");
    Q_strncpyz(index, "sound/player/a-mosquitos.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "MeantToDoThat") ){
    G_Say(ent, NULL, SAY_ALL, "^3I aaaa... Meant to do that!");
    Q_strncpyz(index, "sound/player/a-meant.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "marines" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Lets go Marines!");
    Q_strncpyz(index, "sound/player/marines.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "movein" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Move in!");
    Q_strncpyz(index, "sound/player/move.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "movingout" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Moving out corporal!");
    Q_strncpyz(index, "sound/player/moving.wav", sizeof(index));
    //blah
//N

    }else if ( !Q_stricmp( arg1, "nothing" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3You got nothin!");
    Q_strncpyz(index, "sound/player/nothing.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "needlight" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Need a light?");
    Q_strncpyz(index, "sound/player/needlight.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "napalm" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3I love the smell of napalm.");
    Q_strncpyz(index, "sound/player/smellnapalm.wav", sizeof(index));
    //blah
//O
    }else if ( !Q_stricmp( arg1, "oneshot" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3One shot, one kill!");
    Q_strncpyz(index, "sound/player/oneshot.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "onlyone" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3I'm like the only one with skills.");
    Q_strncpyz(index, "sound/player/onlyskills.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "ownage" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Ownage!");
    Q_strncpyz(index, "sound/player/a-ownage.wav", sizeof(index));
    //blah????
    }else if ( !Q_stricmp( arg1, "outofhere" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3If we make it out of here, you all get a beer!");
    Q_strncpyz(index, "sound/player/outof.wav", sizeof(index));
    //blah
//p
//q
//r
//s

    }else if ( !Q_stricmp( arg1, "scream" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Ahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh!");
    Q_strncpyz(index, "sound/player/scream.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "shazbot" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Shazbot!");
    Q_strncpyz(index, "sound/player/a-shazbot.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "seensquad" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Have you seen any other soldiers from my platoon?");
    Q_strncpyz(index, "sound/player/seensquad.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "suckdick" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Hey, try not to suck any dick on the way from the parking lot!");
    Q_strncpyz(index, "sound/player/suck.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "suppressing" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3I need suppressing fire!");
    Q_strncpyz(index, "sound/player/suppressing.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "stfu" ) ){
    int n =  rand()%2; //0-2
        if (n==0){
        G_Say(ent, NULL, SAY_ALL, "^3Shut the fuck up!");
        Q_strncpyz(index, "sound/player/stfu1.wav", sizeof(index));
        }else{
        G_Say(ent, NULL, SAY_ALL, "^3Shut the fuck up!");
        Q_strncpyz(index, "sound/player/stfu2.wav", sizeof(index));
        }
    }else if ( !Q_stricmp( arg1, "stillheadshot" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3I mean my hearts beating...");
    Q_strncpyz(index, "sound/player/stillheadshot.wav", sizeof(index));
    //blah
//T

    }else if ( !Q_stricmp( arg1, "tagem" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3I'll tag em, you bag em!");
    Q_strncpyz(index, "sound/player/tag.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "takethis" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Take this!");
    Q_strncpyz(index, "sound/player/take.wav", sizeof(index));
    //blah
//u
//v
//W

    }else if ( !Q_stricmp( arg1, "wantsome" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3You want some of me?");
    Q_strncpyz(index, "sound/player/wantsome.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "watchfire" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Watch your fire, sir!");
    Q_strncpyz(index, "sound/player/watchfire.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "watchit" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Watch it corporal!");
    Q_strncpyz(index, "sound/player/watchit.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "whatithought" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3That's what I thought!");
    Q_strncpyz(index, "sound/player/thatswhatithought.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "wantapiece" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3You want a piece of me, boy?");
    Q_strncpyz(index, "sound/player/wantapiece.wav", sizeof(index));
    //blah
//z

    }else if ( !Q_stricmp( arg1, "zone" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Check your zone!");
    Q_strncpyz(index, "sound/player/zone.wav", sizeof(index));
    //blah
///////-------------------------------------------------------------------------
Unreal Tournament/Quake 3 Sounds     ////////////////////\\\\\\\\\\\\\\\\\\\\\\\\|
///////-------------------------------------------------------------------------

    }else if ( !Q_stricmp( arg1, "dkill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Double kill!");
    Q_strncpyz(index, "sound/player/doublekill.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "combo" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Combo Whore!");
    Q_strncpyz(index, "sound/player/combowhore.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "dominating") ){
    G_Say(ent, NULL, SAY_ALL, "^3DOMINATING");
    Q_strncpyz(index, "sound/player/dominating.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "excellent" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Excellent!");
    Q_strncpyz(index, "sound/player/excellent.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "fblood" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Firstblood!");
    Q_strncpyz(index, "sound/player/firstblood.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "flawless" ) ){
    trap_SendServerCommand( -1, va("chat \"console: ^3Flawless Victory^7!\n\""));
    Q_strncpyz(index, "sound/player/flawlessvic.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "god" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Godlike!");
    Q_strncpyz(index, "sound/player/godlike.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "hat" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Hat Trick!");
    Q_strncpyz(index, "sound/player/hattrick.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "hhunter" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Head Hunter!");
    Q_strncpyz(index, "sound/player/headhunter.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "hshot") ) {
    G_Say(ent, NULL, SAY_ALL, "^3Headshot!");
    Q_strncpyz(index, "sound/player/headshot.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "humiliation" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Humiliation!");
    Q_strncpyz(index, "sound/player/humiliation.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "humiliating" ) ){
    trap_SendServerCommand( -1, va("chat \"console: ^3Humiliating ^1DEFEAT^7!\n\""));
    Q_strncpyz(index, "sound/player/humiliation_defeat.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "hshit" ) ){
    G_Say(ent, NULL, SAY_ALL, "^1HOLY SHIT!!!");
    Q_strncpyz(index, "sound/player/holyshit.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "impressive" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Impressive!");
    Q_strncpyz(index, "sound/player/impressive.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "kspree" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Killing Spree!");
    Q_strncpyz(index, "sound/player/killingspree.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "ludicrous" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Llllludicrous KILL!");
    Q_strncpyz(index, "sound/player/ludicrouskill.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "mekill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Mega kill!");
    Q_strncpyz(index, "sound/player/megakill.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "mokill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3MMMMMONSTER KILL!");
    Q_strncpyz(index, "sound/player/monsterkill.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "mukill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Multikill!");
    Q_strncpyz(index, "sound/player/multikill.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "play" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Play!");
    Q_strncpyz(index, "sound/player/play.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "rampage" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Rampage!");
    Q_strncpyz(index, "sound/player/rampage.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "reset" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Reset!");
    Q_strncpyz(index, "sound/player/reset.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "score" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Score!");
    Q_strncpyz(index, "sound/player/score.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "skiller" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Spawnkiller!");
    Q_strncpyz(index, "sound/player/spawnkiller.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "tkiller" ) ){
    G_Say(ent, NULL, SAY_ALL, "^1Teamkiller!");
    Q_strncpyz(index, "sound/player/teamkiller.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "tkill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Tripple Kill!");
    Q_strncpyz(index, "sound/player/tripplekill.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "ukill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Ultra Kill!");
    Q_strncpyz(index, "sound/player/ultrakill.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "unreal" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3UnReal!");
    Q_strncpyz(index, "sound/player/unreal.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "unstoppable" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Unstoppable!");
    Q_strncpyz(index, "sound/player/unstoppable.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "wicked" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Wicked Sick");
    Q_strncpyz(index, "sound/player/whickedsick.wav", sizeof(index));
    //blah

////////--------------------------------------------------------------------
END UNREAL/QUAKE SOUNDS														 |
///////---------------------------------------------------------------------
//////
BEGIN ADMIN ONLY SOUNDS
//////
    }else if ( !Q_stricmp( arg1, "adminmsg" ) ){
    trap_SendServerCommand( -1, va("chat \"console: Message from administration:\n\""));
    Q_strncpyz(index, "sound/player/adminmessage.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "authority" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3Don't confuse your rank with my authority, corporal!");
    Q_strncpyz(index, "sound/player/athority.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "bw" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/player/buzzwarn.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "tfc" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/player/(tfc) endgame.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "Matrix" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/player/Matrix.wav", sizeof(index));
    //blah
//WOLF MP SOUNDS!!

    }else if ( !Q_stricmp( arg1, "USAObjTak" ) ){
    //G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/multiplayer/allies/a-objective_taken.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "USAObjSec" ) ){
    //G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/multiplayer/allies/a-objective_secure.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "NaziObjSec" ) ){
    //G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/multiplayer/axis/g-objective_secure.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "Naziwin" ) ){
    //G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/multiplayer/music/s_stinglow.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "USAwin" ) ){
    //G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/multiplayer/music/l_complete_2.wav", sizeof(index));
    //blah
    //Wolf mp cheers

    }else if ( !Q_stricmp( arg1, "yalord" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3(unknown):^2Yes!");
    Q_strncpyz(index, "sound/multiplayer/axis/g-yeah1.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "daccor" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3(unknown):^2YES!");
    Q_strncpyz(index, "sound/multiplayer/axis/g-yeah2.wav", sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "hehheh" ) ){
    G_Say(ent, NULL, SAY_ALL, "^3(unknown):^2Heh Heh!");
    Q_strncpyz(index, "sound/multiplayer/axis/g-yeah3.wav", sizeof(index));
    //blah
    //DAloops:
    }else if ( !Q_stricmp( arg1, "daloop1") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop1.wav",sizeof(index));
        //blah
    }else if ( !Q_stricmp( arg1, "daloop2") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop2.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "daloop3") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop3.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "daloop4") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop4.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "daloop5") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop5.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "daloop6") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop6.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "daloop7") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop7.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "daloop8") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop8.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "daloop9") ){
    G_Say(ent, NULL, SAY_ALL, "^3Cool Tunes...");
    Q_strncpyz(index, "sound/player/daloop9.wav",sizeof(index));
    //blah
    //Songs:

    }else if ( !Q_stricmp( arg1, "369") ){
    G_Say(ent, NULL, SAY_ALL, "^3Damn She's Fine!");
    Q_strncpyz(index, "sound/player/admin1.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "welcome") ){
    trap_SendServerCommand( -1, va("chat \"console: Welcome to the jungle^7!\n\""));
    Q_strncpyz(index, "sound/player/admin2.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "woohoo") ){
    G_Say(ent, NULL, SAY_ALL, "^3Woo Hoo!");
    Q_strncpyz(index, "sound/player/admin3.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "halo") ){
    G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/player/admin4.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "thegame") ){
    G_Say(ent, NULL, SAY_ALL, "^3Play Again?");
    Q_strncpyz(index, "sound/player/admin5.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "hitground") ){
    G_Say(ent, NULL, SAY_ALL, "^3Hit the Ground");
    Q_strncpyz(index, "sound/player/admin6.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "clickboom") ){
    G_Say(ent, NULL, SAY_ALL, "^3Click Click BOOM!");
    Q_strncpyz(index, "sound/player/admin7.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "werdy") ){
    G_Say(ent, NULL, SAY_ALL, "^3We Ready");
    Q_strncpyz(index, "sound/player/admin8.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "irnmn") ){
    G_Say(ent, NULL, SAY_ALL, "^3");
    Q_strncpyz(index, "sound/player/admin9.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "lftbhnd") ){
    G_Say(ent, NULL, SAY_ALL, "^3We all got left behind");
    Q_strncpyz(index, "sound/player/admin10.wav",sizeof(index));
    //blah
    }else if ( !Q_stricmp( arg1, "trnsfrm") ){
    G_Say(ent, NULL, SAY_ALL, "^3Transform");
    Q_strncpyz(index, "sound/player/admin11.wav",sizeof(index));
    //blah
    }
    */
//Columned list-22 lines total in console																	    END OF LINE(in console)---------|
	//else{
	trap_SendServerCommand( ent - g_entities, va( "print \"^1MVSAY commands: ^7By ^0AG3NT\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^4USAGE: ^7/vsay <string>\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^1Get the soundpack from: ^3www.S4NDMoD.com ^1in downloads\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^21. ^3Alert      ^22.^3Bitches       ^23.^3Busted      ^24.^3Backup        ^25.^3Bogies\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^26. ^3Charge     ^27.^3Control       ^28.^3ComeAgain   ^29.^3CoveringFire ^210.^3Checkrecon\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^211.^3Clear     ^212.^3Check        ^213.^3Command    ^214.^3Current      ^215.^3Die\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^216.^3DidntHurt ^217.^3Damage       ^218.^3Disneyland ^219.^3Eatshit      ^220.^3Fallback\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^221.^3Fags      ^222.^3Fatal        ^223.^3FightClub  ^224.^3Freaks       ^225.^3Friendlyfire\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^226.^3Getsome   ^227.^3GotIt        ^228.^3Guardduty  ^229.^3Hello        ^230.^3Hostiles\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^231.^3Kickass   ^232.^3Letsgo       ^233.^3Mosquitos  ^234.^3MeantToDoThat^235.^3Movein\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^236.^3Movingout ^237.^3Nothing      ^238.^3Oneshot    ^239.^3Ownage       ^240.^3Outofhere\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^241.^3Scream    ^242.^3Shazbot      ^243.^3Seensquad  ^244.^3Suckdick     ^245.^3Suppressing\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^246.^3Tagem     ^247.^3Wantsome     ^248.^3Watchfire  ^249.^3Watchit      ^250.^3Zone\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^1NEW SOUNDS:\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^251.^3Boner     ^252.^3BoomHeadshot ^253.^3Dance      ^254.^3GameOver     ^255.^3GoGoGo\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^256.^3GetOut    ^257.^3HowMuchLonger^258.^3IamOwnage  ^259.^3Lag          ^260.^3LetsRock\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^261.^3Napalm    ^262.^3NeedLight    ^263.^3OnlyOne    ^264.^3STFU         ^265.^3StillHeadshot\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^266.^3WantAPiece ^267.^3WhatIThought\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "cp \"USE /vsay <string>\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"\n\"" ) );
	//}
	//Columned Lists end
	//NEW WAY!!
//	G_AddEvent (ent, EV_GLOBAL_SOUND, G_SoundIndex(index));
/*
for(i=0; i<MAX_CLIENTS; i++){
if (level.clients[i].pers.connected != CON_CONNECTED)
continue;
other = &g_entities[i];
    te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
    te->s.eventParm = G_SoundIndex (index);
    te->s.teamNum = other->s.clientNum;
}
*/
}
void use_dlight( gentity_t *ent, gentity_t *other, gentity_t *activator );
#define clientVersion 1.0
void Cmd_checkpure_f( gentity_t *ent ) {
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );


//	if(strlen(ent->client->sess.cdkey))
	return;
	if ( strcmp( arg1, va( "%f",clientVersion ) ) ) {
		trap_DropClient( ent - g_entities, "S4NDMoD client out of date, visit www.S4NDMoD.com to download new one!" );
		return;
	}
//	strcpy(ent->client->sess.cdkey,arg2);
	return;
}

void Cmd_checkcvar_f( gentity_t *ent ) {
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );

	Checkcvars( ent,arg1, atoi( arg2 ) );

	//trap_SendServerCommand(-1, va("chat \"TEST: %s^7 has %s set to %s!\n\"", ent->client->pers.netname, arg1, arg2));
}

/*
==================
Cmd_Rules_f
==================
This shows the fucking server info b/c infostring protection fucks up serverstatus
*** INFO: use %.99s to prevent malicios admins from crashing clients
*/
void Cmd_Rules_f( gentity_t *ent ) { /*
	char	arg1[MAX_STRING_TOKENS];	//get their arg that they sent :)

	if ((!Q_stricmp(arg1,"soldier"))||(!Q_stricmp(arg1,"sold"))||(!Q_stricmp(arg1,"FIXME0"))||(!Q_stricmp(arg1,"FIXME1"))){
	    trap_SendServerCommand(ent-g_entities, va("print \"^3Soldier help system.\n\""));
	    trap_SendServerCommand(ent-g_entities, va("print \"Special1 - No current function\n\""));
	    trap_SendServerCommand(ent-g_entities, va("print \"Special2 - No current function\n\""));
	}
	if ((!Q_stricmp(arg1,"engineer"))||(!Q_stricmp(arg1,"eng"))||(!Q_stricmp(arg1,"mine"))||(!Q_stricmp(arg1,"automg42"))){
	    trap_SendServerCommand(ent-g_entities, va("print \"^3Engineer help system.\n\""));
	    trap_SendServerCommand(ent-g_entities, va("print \"Special1 - Spawns AutoMG42\n\""));
	    trap_SendServerCommand(ent-g_entities, va("print \"Special2 - Enables/Disables a mine\n\""));
	}
	if ((!Q_stricmp(arg1,"medic"))||(!Q_stricmp(arg1,"med"))||(!Q_stricmp(arg1,"FIXME2"))||(!Q_stricmp(arg1,"FIXME3"))){
	    trap_SendServerCommand(ent-g_entities, va("print \"^3Medic help system.\n\""));
	    trap_SendServerCommand(ent-g_entities, va("print \"Special1 - No current function\n\""));
	    trap_SendServerCommand(ent-g_entities, va("print \"Special2 - No current function\n\""));
	}
	if ((!Q_stricmp(arg1,"lieutenant"))||(!Q_stricmp(arg1,"lt"))||(!Q_stricmp(arg1,"smoke"))||(!Q_stricmp(arg1,"nadepack"))){
	    trap_SendServerCommand(ent-g_entities, va("print \"^3Lieutenant help system.\n\""));
	    trap_SendServerCommand(ent-g_entities, va("print \"Special1 - Spawns a Grenadepack (must have full charge)\n\""));
	    trap_SendServerCommand(ent-g_entities, va("print \"Special2 - Enables/Disables a smoke grenade\n\""));
	}
	}
	int		AxisSpawnTime;
	int		AlliedSpawnTime;
	char	*s;
	char	mapName[1000];
	char	hostname[MAX_INFO_STRING];
	char	protocol[16];
	char	fps[16];
	char	maxrate[16];
	char	userinfo[MAX_INFO_STRING];
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof(mapName) );
	trap_Cvar_VariableStringBuffer( "protocol", protocol, sizeof(protocol) );
	trap_Cvar_VariableStringBuffer( "sv_hostname", hostname, sizeof(hostname) );
	trap_Cvar_VariableStringBuffer( "sv_fps", fps, sizeof(fps) );
	trap_Cvar_VariableStringBuffer( "sv_maxrate", maxrate, sizeof(maxrate) );
	//	trap_GetUserinfo( ent, userinfo, sizeof( userinfo ) );
	s = Info_ValueForKey( userinfo, "rate" );

	AxisSpawnTime = g_redlimbotime.integer/1000;
	AlliedSpawnTime = g_bluelimbotime.integer/1000;


	trap_SendServerCommand(ent-g_entities, va("print \"\n\""));
	trap_SendServerCommand(ent-g_entities, va("print \"Welcome to %.99s^7. ^3This server is running ^5%s\n\"",hostname, GAMEVERSION));
	trap_SendServerCommand(ent-g_entities, va("print \"Heavy Weapon Limits: %i Panzer. %i Flamer. %i Venom. %i Sniper.\n\"",g_maxPf.integer,g_maxFlamer.integer,g_maxVenom.integer,g_maxMauser.integer));
	trap_SendServerCommand(ent-g_entities, va("print \"Respawn Times: ^4Allied: %i. ^1Axis: %i.\n\"",AlliedSpawnTime,AxisSpawnTime));
	if (AB_Enable.integer){
	    trap_SendServerCommand(ent-g_entities, va("print \"AdminBot Enabled.  MinScore: %i. TeamKills: %i\n\"",AB_MinScore.integer, AB_Tks.integer));
	}else{
	    trap_SendServerCommand(ent-g_entities, va("print \"AdminBot Disabled.\n\""));
	}
	trap_SendServerCommand(ent-g_entities, va("print \"Server Net Settings: Antilag: %i. MaxRate: %s. FPS: %s.\n\"",g_antilag.integer, maxrate,fps));
	if (atoi(s) < 25000)
	    trap_SendServerCommand(ent-g_entities, va("print \"^1WARNING ^7you are NOT using optimal net settings, type /rate 25000 to fix.\n\""));
	trap_SendServerCommand(ent-g_entities, va("print \"Name  Knives Pistol Clips  SMG Clips  Nades ChargeTime\n\""));
	trap_SendServerCommand(ent-g_entities, va("print \"--------------------------------------------------------------------------\n\""));
	trap_SendServerCommand(ent-g_entities, va("print \"Soldier %i        %i          %i        %i     %i\n\"",g_maxKnives.integer,g_pistolExtraClips.integer,g_soldExtraClips.integer,g_soldNades.integer,g_soldierChargeTime.integer));
	trap_SendServerCommand(ent-g_entities, va("print \"Engie   %i        %i          %i        %i     %i\n\"",g_maxKnives.integer,g_pistolExtraClips.integer,g_engExtraClips.integer,g_engNades.integer,g_engineerChargeTime.integer));
	trap_SendServerCommand(ent-g_entities, va("print \"Medic   %i        %i          %i        %i     %i\n\"",g_maxKnives.integer,g_pistolExtraClips.integer,g_medExtraClips.integer,g_medNades.integer,g_medicChargeTime.integer));
	trap_SendServerCommand(ent-g_entities, va("print \"LT      %i        %i          %i        %i     %i\n\"",g_maxKnives.integer,g_pistolExtraClips.integer,g_ltExtraClips.integer,g_ltNades.integer,g_LTChargeTime.integer));
	if (g_fraglimit.integer)
	    trap_SendServerCommand(ent-g_entities, va("print \"Fraglimit: %i\n\"",g_fraglimit.integer));
	if (g_maxlives.integer)
	    trap_SendServerCommand(ent-g_entities, va("print \"MaxLives: %i\n\"",g_maxlives.integer));

	*/
}
/*
==================
Cmd_Classes_f
==================
Umm yea this is basically equivliant of shrubmod.  Hopefully it works :D
*/
void Cmd_Classes_f( gentity_t *ent ) {
	int i;
	int BlueSold = 0;
	int BlueEng = 0;
	int BlueMed = 0;
	int BlueLT = 0;
	int BluePlayers = 0;
	int RedSold = 0;
	int RedEng = 0;
	int RedMed = 0;
	int RedLT = 0;
	int RedPlayers = 0;
	int Spectators = 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR ) {            //Do we really care about them?
			Spectators++;
		}
		//	if (level.clients[i].sess.sessionTeam != client->sess.sessionTeam)	//Skip if they arnt his team ;)
		//		continue;
		if ( level.clients[i].ps.stats[STAT_PLAYER_CLASS] == PC_SOLDIER ) {
			if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
				BlueSold++;
			}
			if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
				RedSold++;
			}
		}
		if ( level.clients[i].ps.stats[STAT_PLAYER_CLASS] == PC_ENGINEER ) {
			if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
				BlueEng++;
			}
			if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
				RedEng++;
			}
		}
		if ( level.clients[i].ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {
			if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
				BlueMed++;
			}
			if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
				RedMed++;
			}
		}
		if ( level.clients[i].ps.stats[STAT_PLAYER_CLASS] == PC_LT ) {
			if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
				BlueLT++;
			}
			if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
				RedLT++;
			}
		}
	}
	RedPlayers = RedSold + RedEng + RedMed + RedLT;
	BluePlayers = BlueSold + BlueEng + BlueMed + BlueLT;

	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3Spectators: ^7%i ^3Allies: ^7%i ^3Axis: ^7%i ^3S: ^7%i ^3E: ^7%i ^3M: ^7%i ^3L: ^7%i\n\"",
													  Spectators, BluePlayers, RedPlayers, RedSold, RedEng, RedMed, RedLT ) );
	} else if ( ent->client->sess.sessionTeam == TEAM_BLUE )    {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3Spectators: ^7%i ^3Allies: ^7%i ^3Axis: ^7%i ^3S: ^7%i ^3E: ^7%i ^3M: ^7%i ^3L: ^7%i\n\"",
													  Spectators, BluePlayers, RedPlayers, BlueSold, BlueEng, BlueMed, BlueLT ) );
	} else {
		trap_SendServerCommand( ent - g_entities, va( "chat \"^3Spectators: ^7%i ^3Allies: ^7%i ^3Axis: ^7%i\n\"",Spectators, BluePlayers, RedPlayers ) );
	}
}
/*
==================
g_PushPlayer
==================
New Shove code...Ripped from ETPUB
*/
qboolean G_PushPlayer( gentity_t *ent, gentity_t *victim ) {
	vec3_t dir, push;


	if ( !g_shove.integer ) {
		return qfalse;
	}


	if ( ent->health <= 0 ) {
		return qfalse;
	}


	//if((level.time - ent->client->pmext.shoveTime) < 500){
	//	return qfalse;
	//}

	//ent->client->pmext.shoveTime = level.time;
	if ( ( level.time - ent->lastPushTime ) < 500 ) {
		return qfalse;
	}


	ent->lastPushTime = level.time;

	VectorSubtract( victim->r.currentOrigin, ent->r.currentOrigin, dir );

	VectorNormalizeFast( dir );

	VectorScale( dir, ( g_shove.value * 5.0f ), push );     // FIXME - Martin
/*
    if((abs(push[2]) > abs(push[0])) &&	(abs(push[2]) > abs(push[1]))) {
        // player is being boosted
        if(g_shoveNoZ.integer) {
            return qfalse;
        }
        //push[2] = g_shove.value;
        //push[0] = push[1] = 0;
        push[2] = 24;
    }
    else {
        // give them a little hop
        push[2] = 24;
    }
*/
	if ( g_shoveNoZ.integer ) {
		if ( ( abs( push[2] ) > abs( push[0] ) ) && ( abs( push[2] ) > abs( push[1] ) ) ) { //player is being boosted
			return qfalse;
		}
	}

	// give them a little hop
	//push[2] = 24;

	VectorAdd( victim->s.pos.trDelta, push, victim->s.pos.trDelta );
	VectorAdd( victim->client->ps.velocity, push, victim->client->ps.velocity );

	G_AddEvent( ent, EV_GENERAL_SOUND, G_SoundIndex( "sound/weapons/grenade/grenlf1a.wav" ) );

	victim->client->pers.lastShovedTime = level.time;
	victim->client->pers.lastShovedClient = ent;

	return qtrue;
}
/*
Cmd_Activate2_f() is only for:
    uniform stealing
    shove
    dragging
    class switch
 */
void Cmd_Activate2_f( gentity_t *ent ) {
	trace_t tr;
	vec3_t end;
	gentity_t   *traceEnt;
	vec3_t forward;     //, right, up, offset;

/*	// look for a guy to push
    trap_Trace(&tr, offset, NULL, NULL, end, ent->s.number, CONTENTS_BODY);
    if(tr.entityNum >= 0) {
        trap_SendServerCommand(-1, va("chat \"MARKER 3\n\""));
        trap_SendServerCommand(-1, va("chat \"%i\n\"", tr.entityNum));
        traceEnt = &g_entities[tr.entityNum];
        if(traceEnt->client) {
            trap_SendServerCommand(-1, va("chat \"MARKER 4\n\""));
            //if(traceEnt->client->ps.eFlags & EF_PLAYDEAD)
            //	G_DragCorpse(ent, traceEnt);
            //else
            //if(traceEnt->health > 0)
                G_PushPlayer(ent, traceEnt);
            return;
        }
    }

*/

	vec3_t start;
	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->s.pos.trBase, start );   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA( start, 128, forward, end );    //put 'end' 128 units forward of 'start'

	//see if we hit anything between 'start' and 'end'
	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, CONTENTS_BODY );

	if ( tr.entityNum >= MAX_CLIENTS ) {
		return;
	}
	traceEnt = &g_entities[tr.entityNum];
	//if(traceEnt->client) {
	//if(traceEnt->client->ps.eFlags & EF_PLAYDEAD)
	//	G_DragCorpse(ent, traceEnt);
	//else
	//if(traceEnt->health > 0)
	G_PushPlayer( ent, traceEnt );
	return;





}
//Ripped from ClientNumberFromNameMatch code
/*
=========================
Check Name
=========================

qboolean ClientNumberFromNameMatch(char *name, int *matches){
   int i, textLen;
   char nm[64];
   char c;
   int index = 0;
   int t;

   Q_strncpyz(nm, name, sizeof(nm));	//copy the name match
   Q_CleanStr(nm);						//get rid of any color codes etc	-agent hrm lol
   textLen = strlen(nm);
   c = *nm;							//the first letter of the name match

//	for (i = 0; i < level.maxclients; i++){		//loop through all the possible clients
       int j, len;
       char playerName[64];	//Just to be safe, lets fucking upp the buffer [IDK how color ties in or what :)

   //	if ((!g_entities[i].client) || (g_entities[i].client->pers.connected != CON_CONNECTED)) //if this client isn't connected, skip them
   //		continue;

       Q_strncpyz(playerName, g_entities[i].client->pers.netname, sizeof(playerName));	//copy the player's name
       Q_CleanStr(playerName);		//remove any color codes etc
       len = strlen(playerName);

       for (j = 0; j < len; j++){	//go through the player's name letter by letter
           //if (tolower(c) == tolower(playerName[j])){	//compare the first letter of the name match to each letter in the player's name
               //if they're the same, check to see if the whole name match can be found
               //if (!Q_stricmpn(nm, playerName+j, textLen)){
               //	matches[index] = i;		//we have a match, so record the client number
               //	index++;				//increment the index counter
               //	break;					//break out of the 'j' FOR loop because we already matched this client
               //}
               for (n=0 ; n < 91 ; n++){
                   if(characters[n][0] != tolower(playerName[j])){
                       return qfalse;		//Tell userinfochanged that they arnt legal
                   }
               }
           }
   return qtrue;	//return that they have a legal name
}
*/

void auto_mg42_spawn( gentity_t *ent );
void AUTOMG42SPAWNER( gentity_t *ent ) {
	gentity_t *target;
	qboolean spawnnow = qtrue;
	target = NULL;

	if ( VectorLength( ent->s.pos.trDelta ) != 0 ) { //if the knife is still moving (not on ground yet)
		ent->nextthink = level.time + 50;
		ent->think = AUTOMG42SPAWNER;
		return;
	}

//check if there are any entity's within a radius
	while ( ( target = findradius( target, ent->r.currentOrigin, 50 ) ) != NULL )
	{
// target must not be mine
		if ( target == ent ) {
			continue;
		}

// target must be a client
		if ( !target->client ) {
			continue;
		}

		spawnnow = qfalse;

	}

	if ( spawnnow ) {
		ent->nextthink = level.time + 1;
		ent->think = auto_mg42_spawn;
	} else {
		ent->nextthink = level.time + 50;
		ent->think = AUTOMG42SPAWNER;
	}

	return;
}




void Cmd_automg42_f( gentity_t *ent ) {
	vec3_t velocity, angles, offset, org, mins, maxs;
	trace_t tr;
	gentity_t *ent2;
	gitem_t *item = BG_FindItemForWeapon( WP_KNIFE );

	if ( !g_automg42limit.integer ) {
		return;
	}

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}

	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] != PC_ENGINEER ) {
		trap_SendServerCommand(  ent - g_entities, va( "chat \"You must be an Engineer to use that!\n\""));
		return;
	}

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	if ( level.time - ent->client->ps.classWeaponTime < g_engineerChargeTime.integer ) { //they dont have full charge
		trap_SendServerCommand(  ent - g_entities, "cp \"^3You must have a full charge!\n\"" );
		return;
	}

	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		if ( level.axismg42s + 1 > g_automg42limit.integer ) {
			trap_SendServerCommand(  ent - g_entities, va( "chat \"Max amount(%i) of auto mg42s spawned!\n\"",g_automg42limit.integer ) );
			return;
		}
		else
		{
			level.axismg42s++;
		}
	} else if ( ent->client->sess.sessionTeam == TEAM_BLUE ){
		if ( level.alliedmg42s + 1 > g_automg42limit.integer ) {
			trap_SendServerCommand(  ent - g_entities, va( "chat \"Max amount(%i) of auto mg42s spawned!\n\"",g_automg42limit.integer ) );
			return;
		}
		else
		{
			level.alliedmg42s++;
		}
	}

	ent->client->ps.classWeaponTime = level.time;

	//some of this is copied from the drop weapon code in g_active.c
	AngleVectors( ent->client->ps.viewangles, velocity, NULL, NULL );
	VectorScale( velocity, 64, offset );
	offset[2] += ent->client->ps.viewheight / 2;

	VectorAdd( ent->client->ps.origin, offset, org );

	VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
	VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );   //these have something to do with making the item sit on the ground

	trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
	VectorCopy( tr.endpos, org );

	ent2 = LaunchItem( item, org, velocity, ent->client->ps.clientNum );      //actually throw the knife
	VectorCopy( ent->client->ps.viewangles, angles );
	angles[PITCH] = 0;
	G_SetAngle( ent2, angles );
	ent2->touch = 0;    //Just want this to do nothing but yea...
	ent2->parent = ent;         //so we know who threw the knife

	ent2->health = 0;
	ent2->flags |= FL_NODRAW;
	ent2->r.svFlags |= SVF_NOCLIENT;
	ent2->s.eFlags |= EF_NODRAW;
	ent2->nextthink = level.time + 100;
	ent2->think = AUTOMG42SPAWNER;

	return;
}

/*
==================
Cmd_Agent_Camera_f
==================
*/
void Cmd_Agent_Camera_f( gentity_t *ent ) {
	FILE    *AgentCamera;
//	char	CameraLoc;

	trap_SendServerCommand( ent - g_entities, va( "print \"Writing the shit: %s\n\"",vtos( ent->client->ps.origin )  ) );
	//CameraLoc = vtos( ent->client->ps.origin );	//The fuckers current position >:(
	AgentCamera = fopen( "AgentCamera.txt","a+" );  //Open file
	fputs( va( "%s\n", ( vtos( ent->client->ps.origin ) ) ), AgentCamera );
	fclose( AgentCamera );

}

void Cmd_OMW_f( gentity_t *ent ) {
	gentity_t *te;
	gentity_t *other;
	//gentity_t *message;
	int i;
	char index[MAX_OSPATH];
//	qboolean MVSAY = qfalse;
//	char	*message;
	char location[1000];

	Team_GetLocationMsg( ent, location, sizeof( location ) );

	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		G_TeamCommand( TEAM_RED, va( "chat \"^7(%s^7)^3(%s): ^5Affirmative, on my way!\"", ent->client->pers.netname, ent->client->pers.location ) );
		Q_strncpyz( index, "sound/multiplayer/axis/g-affirmative_omw.wav", sizeof( index ) );
	} else if ( ent->client->sess.sessionTeam == TEAM_BLUE )    {
		G_TeamCommand( TEAM_BLUE, va( "chat \"^7(%s^7)^3(%s): ^5Affirmative, on my way!\"", ent->client->pers.netname, ent->client->pers.location ) );
		Q_strncpyz( index, "sound/multiplayer/allies/a-affirmative_omw.wav", sizeof( index ) );
	} else {
		return;
	}
	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		if ( ent->client->sess.sessionTeam != level.clients[i].sess.sessionTeam ) {
			continue;
		}
		//	if (!OnSameTeam(ent, &level.clients[i]))
		//continue;
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		other = &g_entities[i];
		te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( index );
		te->s.teamNum = other->s.clientNum;
	}
}


/*
==================
Cmd_hackconfig_f
==================
This will write (hopefully :D) to a server file (in future, hopefully s4ndmod.com for mbl action :D)
If clients attempt to exec a cheat file while in a server..
***
For now it is passive and does not kick, only record in servers file.
*/
void Cmd_hackconfig_f( gentity_t *ent ) {
	FILE*hackfile;
	char arg1[MAX_STRING_TOKENS];
	char    *hackname;
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	trap_Argv( 1, arg1, sizeof( arg1 ) );

	if ( !Q_stricmp( arg1, "aimbot" ) ) {
		hackname = "Aimbot";
	} else if ( !Q_stricmp( arg1, "nullify" ) ) {
		hackname = "Nullify Multihack";
	} else if ( !Q_stricmp( arg1, "deliverance" ) ) {
		hackname = "Deliverance Multihack";
	} else if ( !Q_stricmp( arg1, "mombot" ) ) {
		hackname = "Mombot Multihack";
	} else if ( !Q_stricmp( arg1, "multihack" ) ) {
		hackname = "Multihack";
	} else if ( !Q_stricmp( arg1, "tweak" ) ) {
		hackname = "Multihack";
	} else if ( !Q_stricmp( arg1, "wallhack" ) ) {
		hackname = "Wallhack";
	} else { //	Someone sent something that isnt reconginsed, dont record it
		return;
	}

	hackfile = fopen( "S4ND_cheat.log","a+" );
	fputs( va( "%s\t%s\t%s\t%s", hackname, ent->client->sess.ip, ent->client->pers.netname, asctime( timeinfo ) ),hackfile );
	fclose( hackfile );

	trap_SendServerCommand( -1, va( "chat \"^1*** ^3S4NDMoD Anti-Cheat: Ban Command Issued for ^7%s ^3[%s/%s]. Cfg Violation: %.99s\n\"", 
		ent->client->pers.netname, 
		ent->client->sess.ip, 
		ent->client->sess.hostname,
		hackname) );
	trap_DropClient( ent - g_entities, va( "^5[%s] ^3Banned by ^3S4NDMoD Anti-Cheat. Cfg Violation: %.99s\n", ent->client->sess.ip, hackname, arg1 ) );
	trap_SendConsoleCommand( EXEC_APPEND, va( "addban %s\n", ent->client->sess.ip ) );

}
/*
==================
Cmd_hack_cfg_f
==================
Kicks a player when they misspell exec and it gets sent to the server.
*/
void Cmd_hack_cfg_f( gentity_t *ent ) {
	FILE	*hackfile;
	char	arg1[MAX_STRING_TOKENS];
	char	*hackname;
	time_t	rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	trap_Argv( 1, arg1, sizeof( arg1 ) );
	if ( sv_punkbuster.integer ) { // Let PB handle the banning for cheats
		return;
	}

	trap_Argv( 1, arg1, sizeof( arg1 ) );

	if ( !Q_stricmp( arg1, "aimbot" ) ) {
		hackname = "Aimbot";
	} else if ( !Q_stricmp( arg1, "nullify" ) ) {
		hackname = "Nullify Multihack";
	} else if ( !Q_stricmp( arg1, "aimbotcfg" ) ) {
		hackname = "Nullify Multihack";
	} else if ( !Q_stricmp( arg1, "deliverance" ) ) {
		hackname = "Deliverance Multihack";
	} else if ( !Q_stricmp( arg1, "mombot" ) ) {
		hackname = "Mombot Multihack";
	} else if ( !Q_stricmp( arg1, "multihack" ) ) {
		hackname = "Multihack";
	//} else if ( !Q_stricmp( arg1, "tweak" ) ) {
		//hackname = "Multihack";
	} else if ( !Q_stricmp( arg1, "wallhack" ) ) {
		hackname = "Wallhack";
	} else { //	Someone sent something that isnt reconginsed, dont record it
		return;
	}

	hackfile = fopen( "S4ND_cheat.log","a+" );
	fputs( va( "%s\t%s\t%s\t%s", hackname, ent->client->sess.ip, ent->client->pers.netname, asctime( timeinfo ) ),hackfile );
	fclose( hackfile );

	trap_SendServerCommand( -1, va( "chat \"^1*** ^3S4NDMoD Anti-Cheat: Ban Command Issued for ^7%s ^3[%s/%s]. Cfg Violation: %.99s\n\"", 
		ent->client->pers.netname, 
		ent->client->sess.ip, 
		ent->client->sess.hostname,
		hackname) );
	trap_DropClient( ent - g_entities, va( "^5[%s] ^3Banned by ^3S4NDMoD Anti-Cheat. Cfg Violation: %.99s\n", ent->client->sess.ip, hackname, arg1 ) );
	trap_SendConsoleCommand( EXEC_APPEND, va( "addban %s\n", ent->client->sess.ip ) );
}

/*
==================
Cmd_hackconfig_f
==================
Kicks a player when they attempt to use a cheat cvar (mostly when they install it wrong)
*/
void Cmd_hack_cvar_f( gentity_t *ent, char *hack ) {
	FILE*hackfile;
	char arg1[MAX_STRING_TOKENS];
	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );

	trap_Argv( 1, arg1, sizeof( arg1 ) );

	if ( sv_punkbuster.integer ) { // Let PB handle the banning for cheats
		return;
	}
	if ( atoi( arg1 ) < 1 ) { // They are not trying to turn it on
		return;
	}

	hackfile = fopen( "S4ND_cheat.log","a+" );
	fputs( va( "%.99s\t%s\t%s\t%s", hack, ent->client->sess.ip, ent->client->pers.netname, asctime( timeinfo ) ),hackfile );
	fclose( hackfile );

	trap_SendServerCommand( -1, va( "chat \"^1*** ^3S4NDMoD Anti-Cheat: Ban Command Issued for ^7%s ^3[%s/%s]. Cvar %s = %.99s\n\"", 
		ent->client->pers.netname, 
		ent->client->sess.ip, 
		ent->client->sess.hostname,
		hack, 
		arg1 ) );
	trap_DropClient( ent - g_entities, va( "^5[%s] ^3Banned by ^3S4NDMoD Anti-Cheat. Cvar %s = %.99s\n", ent->client->sess.ip, hack, arg1 ) );
	trap_SendConsoleCommand( EXEC_APPEND, va( "addban %s\n", ent->client->sess.ip ) );
}
/*
==================
Cmd_special1_f
==================
This is used for the special commands of players and keeps people from needing 20x different binds

Special1 will call all things associated with the 5th weaponbank slot
*/
void Cmd_special1_f( gentity_t *ent ) {

	//	If they are spectators dont let them use this command
	if ( ent->client->sess.sessionTeam == TEAM_FREE ) {
		return;
	}

	//LT call smoke toggle
	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_LT ) {
		Cmd_Smoke_f( ent );
	}
	//Eng call MG42 toggle
	else if ( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_ENGINEER ) {
		Cmd_automg42_f( ent );
	}
	//Everyone else
	else {
		return;
	}
}
/*
==================
Cmd_special2_f
==================
This is used for the special commands of players and keeps people from needing 20x different binds

Special1 will call all things associated with the 5th weaponbank slot
*/
void Cmd_special2_f( gentity_t *ent ) {

	//	If they are spectators dont let them use this command
	if ( ent->client->sess.sessionTeam == TEAM_FREE ) {
		return;
	}

	//Eng call mine toggle
	if ( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_ENGINEER ) {
		Cmd_mine_f( ent );
	}
	//Everyone else
	else {
		return;
	}
}
/*
=================
*G_ShortcutSanitize
==================
Cleans up shortcuts (ripped from etpub :)
*/
char *G_ShortcutSanitize( char *text ) {
	static char n[MAX_SAY_TEXT] = {""};

	if ( !text || !*text ) {
		return n;
	}

	Q_strncpyz( n, text, sizeof( n ) );

	Q_strncpyz( n, Q_StrReplace( n, "[a]", "(a)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[d]", "(d)" ), sizeof( n ) );
	//Q_strncpyz(n, Q_StrReplace(n, "[g]", "(g)"), sizeof(n));
	Q_strncpyz( n, Q_StrReplace( n, "[h]", "(h)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[k]", "(k)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[l]", "(l)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[n]", "(n)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[r]", "(r)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[p]", "(p)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[s]", "(s)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[w]", "(w)" ), sizeof( n ) );
	Q_strncpyz( n, Q_StrReplace( n, "[t]", "(t)" ), sizeof( n ) );
	return n;
}
/*
=================
*G_Shortcuts
==================
Does the work for shortcuts
*/
char *G_Shortcuts( gentity_t *ent, char *text ) {     // Shrub values for each letter
	static char out[MAX_SAY_TEXT];
	char a[MAX_NAME_LENGTH] = {"*unknown*"};        // Gave ammo last
	char d[MAX_NAME_LENGTH] = {"*unknown*"};        // last killer
	//char g[32+1] = {"*unknown*"};					// GUID(not used)
	char h[MAX_NAME_LENGTH] = {"*unknown*"};        // Gave healthpack last
	char k[MAX_NAME_LENGTH] = {"*unknown*"};        // player you last killed
	char l[32] = {"*unknown*"};                     // location
	char n[MAX_NAME_LENGTH] = {"*unknown*"};        // client's name
	char r[MAX_NAME_LENGTH] = {"*unknown*"};        // Last player who revived you
	char p[MAX_NAME_LENGTH] = {"*unknown*"};        // Teammate you are pointing at
	char s[32] = {"*unknown*"};                     // Health you have
	char w[32] = {"*unknown*"};                     // Current weapon
	char t[32] = {"*unknown*"};                     // Ammo in current gun
	//char guid_short[9];								// REMOVE
	gclient_t *client = NULL;
	gentity_t *crosshairEnt;
	char *rep;
	gitem_t *weapon;
	int clip;
	int ammo;
//	int i;

	out[0] = '\0';


	// Last LT that gave me ammo
	if ( ent ) {
		if ( ent->client->pers.lastammo_client != -1 ) {
			client = &level.clients[ent->client->pers.lastammo_client];
			if ( client ) {
				Q_strncpyz( a,
							G_ShortcutSanitize( client->pers.netname ),
							sizeof( a ) );
			}
		}
	}
	// Last murder
	if ( ent ) {
		if ( ent->client->pers.lastkiller_client != -1 ) {
			client = &level.clients[ent->client->pers.lastkiller_client];
			if ( client ) {
				Q_strncpyz( d,
							G_ShortcutSanitize( client->pers.netname ),
							sizeof( d ) );
			}
		}
	}

/*	if (ent && ent->client) {
        for(i=0; i<=8; i++){
            guid_short[i] = ent->client->sess.guid[i+24] ?
                ent->client->sess.guid[i+24] : '\0';
        }
        Q_strncpyz(g,guid_short,sizeof(g));
    }*/
	if ( ent ) {
		if ( ent->client->pers.lasthealth_client != -1 ) {
			client = &level.clients[ent->client->pers.lasthealth_client];
			if ( client ) {
				Q_strncpyz( h,
							G_ShortcutSanitize( client->pers.netname ),
							sizeof( h ) );
			}
		}
	}

	if ( ent ) {
		if ( ent->client->pers.lastkilled_client != -1 ) {
			client = &level.clients[ent->client->pers.lastkilled_client];
			if ( client ) {
				Q_strncpyz( k,
							G_ShortcutSanitize( client->pers.netname ),
							sizeof( k ) );
			}
		}
	}

	if ( ent ) {
		char location[32];
		Team_GetLocationMsg( ent, location, sizeof( location ) );
		//Q_strncpyz(l,BG_GetLocationString(ent->r.currentOrigin),sizeof(l));
		Q_strncpyz( l,location,sizeof( l ) );

	}
	if ( ent ) {
		Q_strncpyz( n,G_ShortcutSanitize( ent->client->pers.netname ), sizeof( n ) );
	}

	if ( ent ) {
		if ( ent->client->pers.lastrevive_client != -1 ) {
			client = &level.clients[ent->client->pers.lastrevive_client];
			if ( client ) {
				Q_strncpyz( r,
							G_ShortcutSanitize( client->pers.netname ),
							sizeof( r ) );
			}
		}
	}
	if ( ent ) {
		crosshairEnt = &g_entities[ent->client->ps.identifyClient];
		// Dens: only give the name of the other client, if the player should be able to see it
		if ( crosshairEnt && crosshairEnt->client && crosshairEnt->inuse &&
			 ( ent->client->sess.sessionTeam == crosshairEnt->client->sess.sessionTeam ||
			   ent->client->sess.sessionTeam == TEAM_SPECTATOR ) ) {
			client = crosshairEnt->client;
			if ( client ) {
				Q_strncpyz( p,G_ShortcutSanitize( client->pers.netname ),sizeof( p ) );
			}
		}
	}

	if ( ent ) {
		int health;
		health = ent->health;
		if ( health < 0 ) {
			health = 0;
		}
		Com_sprintf( s, sizeof( s ), "%i", health );
	}

	if ( ent && ent->client->ps.weapon ) {
		weapon = BG_FindItemForWeapon( ent->client->ps.weapon );
		Q_strncpyz( w, weapon->pickup_name, sizeof( w ) );
	}

	if ( ent && ent->client->ps.weapon ) {
		clip = BG_FindClipForWeapon( ent->client->ps.weapon );
		ammo = BG_FindAmmoForWeapon( ent->client->ps.weapon );
		Com_sprintf( t, sizeof( t ), "%i",
					 ( ent->client->ps.ammoclip[clip] +
					   ( ( ent->client->ps.weapon == WP_KNIFE ) ? 0 : ent->client->ps.ammo[ammo] ) ) );
	}

	rep = Q_StrReplace( text, "[a]", a );
	rep = Q_StrReplace( rep, "[d]", d );
	//rep = Q_StrReplace(rep, "[g]", g);
	rep = Q_StrReplace( rep, "[h]", h );
	rep = Q_StrReplace( rep, "[k]", k );
	rep = Q_StrReplace( rep, "[l]", l );
	rep = Q_StrReplace( rep, "[n]", n );
	rep = Q_StrReplace( rep, "[r]", r );
	rep = Q_StrReplace( rep, "[p]", p );
	rep = Q_StrReplace( rep, "[s]", s );
	rep = Q_StrReplace( rep, "[w]", w );
	rep = Q_StrReplace( rep, "[t]", t );
	Q_strncpyz( out, rep, sizeof( out ) );
	return out;
}
/*================
startSpawnTimer
Sets a clients initial start time for their spawn timer.  We will just mod this to find when they 'spawn' next, etc...
================*/
void startSpawnTimer( gentity_t* ent ) {
	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		if ( g_spawnTimer.integer ) {
			ent->client->pers.spawnTimerActive = qfalse;
			ent->client->pers.spawnTimerStartTime = level.time;
			ent->client->pers.spawnTimerActive = qtrue;
			trap_SendServerCommand( ent - g_entities, va( "chat \"Spawn timer activated\"" ) );
		} else {
			trap_SendServerCommand( ent - g_entities, va( "chat \"Spawn timer is not enabled\"" ) );
		}
	}
}
/*================
stopSpawnTimer
================*/
void stopSpawnTimer( gentity_t* ent ) {
	ent->client->pers.spawnTimerActive = qfalse;
}
/*
==================
Cmd_test_f
==================
command for exaclty what it says testing.
MAKE SURE THIS IS BLANK BEFORE RELEASE!!!
*/
void Cmd_test_f( gentity_t *ent ) {
	return;
}

