#include "g_local.h"
#include "S4ND_local.h"
#include <float.h>
/*
 * name:		S4ND_spawn.c
 *
 * desc:
 *
 * $Author: Martin crawford & sand :p $
 * $Revision: 7 $
 * $Modtime: 11/05/01 3:38p $
 * $Date: 11/05/01 3:38p $
*/

/*
===============
S4ND_Spawn

Custom spawn stuff for rtcw.
to clear all the shit out of g_callspawn
===============
*/
qboolean S4ND_Spawn( gentity_t *ent ) {
	char mapName[32];

//
//	General Shit - Mode specific later on
//
	//	Turn MED on map into regular MEDpacks for players to use
	if ( !Q_strncmp( ent->classname, "item_armor_", 11) || !Q_strncmp( ent->classname, "item_health_", 12) )  {
		ent->classname = "item_health";
		return qtrue;
	}

	//	Turn ammo on map into AMMOpacks for players to use
	if ( !Q_strncmp( ent->classname, "ammo_", 5) ) {
		ent->classname = "weapon_magicammo";
		// don't spawn nades in sniper mode
		return ( g_sniperWar.integer && !strcmp( "ammo_grenades", ent->classname ) ) ? qfalse : qtrue;
	}

//	BEGIN MAPS [General]
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );
	//All SP doors stay open forever once opened :D
	if ( ( !Q_stricmp( mapName, "assault" ) ) || ( !Q_stricmp( mapName, "baseout" ) ) || ( !Q_stricmp( mapName, "boss1" ) ) || ( !Q_stricmp( mapName, "boss2" ) )
		 || ( !Q_stricmp( mapName, "castle" ) ) || ( !Q_stricmp( mapName, "chateau" ) ) || ( !Q_stricmp( mapName, "church" ) ) || ( !Q_stricmp( mapName, "crypt1" ) )
		 || ( !Q_stricmp( mapName, "crypt2" ) ) || ( !Q_stricmp( mapName, "dam" ) ) || ( !Q_stricmp( mapName, "dig" ) ) || ( !Q_stricmp( mapName, "end" ) )
		 || ( !Q_stricmp( mapName, "escape1" ) ) || ( !Q_stricmp( mapName, "escape2" ) ) || ( !Q_stricmp( mapName, "factory" ) ) || ( !Q_stricmp( mapName, "forest" ) )
		 || ( !Q_stricmp( mapName, "norway" ) ) || ( !Q_stricmp( mapName, "rocket" ) ) || ( !Q_stricmp( mapName, "sfm" ) ) || ( !Q_stricmp( mapName, "swf" ) )
		 || ( !Q_stricmp( mapName, "trainyard" ) ) || ( !Q_stricmp( mapName, "tram" ) ) || ( !Q_stricmp( mapName, "village1" ) ) || ( !Q_stricmp( mapName, "village2" ) )
		 || ( !Q_stricmp( mapName, "xlabs" ) ) )
	{
		if  ( ( !strcmp( "func_door_rotating", ent->classname ) ) || ( !strcmp( "func_door", ent->classname ) ) ) {
			ent->wait = FLT_MAX;
		}
		//	Moved for TJ maps etc...
		if ( !strcmp( "trigger_aidoor", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "alarm_box", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "trigger_once", ent->classname ) ) {
			return qfalse;
		}
	}

	if ( !Q_stricmp( mapName, "assault" ) ) {
		if ( ( !strcmp( "func_door_rotating", ent->classname ) ) && ( ent->r.currentOrigin[0] == -4510 ) && ( ent->r.currentOrigin[1] == 4616 ) && ( ent->r.currentOrigin[2] == 664 ) ) {
			return qfalse;
		}
	}

	if ( !Q_stricmp( mapName, "baseout" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {

			if ( ( ent->r.currentOrigin[0] == -2064 ) && ( ent->r.currentOrigin[1] == 2286 ) && ( ent->r.currentOrigin[2] == 280 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 282 ) && ( ent->r.currentOrigin[1] == 672 ) && ( ent->r.currentOrigin[2] == 53 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 6 ) && ( ent->r.currentOrigin[1] == 672 ) && ( ent->r.currentOrigin[2] == 53 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "boss1" ) ) {
		if ( !strcmp( "func_door", ent->classname ) ) {
			return qfalse;
		}
		if ( ( !strcmp( "func_explosive", ent->classname ) ) && ( ent->spawnflags != 4 ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "boss2" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == 574 ) && ( ent->r.currentOrigin[1] == 968 ) && ( ent->r.currentOrigin[2] == 88 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 514 ) && ( ent->r.currentOrigin[1] == 1464 ) && ( ent->r.currentOrigin[2] == 88 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "castle" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == 1232 ) && ( ent->r.currentOrigin[1] == 1950 ) && ( ent->r.currentOrigin[2] == 336 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 1158 ) && ( ent->r.currentOrigin[1] == 1950 ) && ( ent->r.currentOrigin[2] == 336 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 1158 ) && ( ent->r.currentOrigin[1] == 2117 ) && ( ent->r.currentOrigin[2] == 100 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 1018 ) && ( ent->r.currentOrigin[1] == 2117 ) && ( ent->r.currentOrigin[2] == 100 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "chateau" ) ) {
		if ( !strcmp( "func_explosive", ent->classname ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "church" ) ) {
		if ( ( !strcmp( "func_explosive", ent->classname ) ) && ( ent->spawnflags == 20 ) ) {
			return qfalse;
		}
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == -480 ) && ( ent->r.currentOrigin[1] == -158 ) && ( ent->r.currentOrigin[2] == 704 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -384 ) && ( ent->r.currentOrigin[1] == -158 ) && ( ent->r.currentOrigin[2] == 1152 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -384 ) && ( ent->r.currentOrigin[1] == 734 ) && ( ent->r.currentOrigin[2] == 1152 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "crypt1" ) ) {
		if ( !strcmp( "func_explosive", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "trigger_hurt", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "func_door", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "func_invisible_user", ent->classname ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "crypt2" ) ) {
		if ( !strcmp( "func_explosive", ent->classname ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "dam" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == -1340 ) && ( ent->r.currentOrigin[1] == 5528 ) && ( ent->r.currentOrigin[2] == 2404 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -1576 ) && ( ent->r.currentOrigin[1] == 5528 ) && ( ent->r.currentOrigin[2] == 2404 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -1604 ) && ( ent->r.currentOrigin[1] == 5186 ) && ( ent->r.currentOrigin[2] == 2392 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "dark" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "escape1" ) ) {
		if ( !strcmp( "func_invisible_user", ent->classname ) ) {
			return qfalse;
		}
		if ( ( !strcmp( "func_door", ent->classname ) ) && ( ent->spawnflags == 7 ) ) {
			return qfalse;
		}
		if ( ( !strcmp( "func_explosive", ent->classname ) ) && ( ent->spawnflags == 4 ) ) {
			return qfalse;
		}
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == 264 ) && ( ent->r.currentOrigin[1] == 129 ) && ( ent->r.currentOrigin[2] == -504 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 432 ) && ( ent->r.currentOrigin[1] == 513 ) && ( ent->r.currentOrigin[2] == -504 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 600 ) && ( ent->r.currentOrigin[1] == 575 ) && ( ent->r.currentOrigin[2] == -504 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -190 ) && ( ent->r.currentOrigin[1] == 416 ) && ( ent->r.currentOrigin[2] == 536 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -190 ) && ( ent->r.currentOrigin[1] == 416 ) && ( ent->r.currentOrigin[2] == 752 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "factory" ) ) {
		if ( ( !strcmp( "func_door_rotating", ent->classname ) ) && ( ent->r.currentOrigin[0] == 1400 ) && ( ent->r.currentOrigin[1] == 162 ) && ( ent->r.currentOrigin[2] == 56 ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "forest" ) ) {
		if  ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == -3492 ) && ( ent->r.currentOrigin[1] == -434 ) && ( ent->r.currentOrigin[2] == 264 ) ) {
				return qtrue;
			} else {
				return qfalse;
			}
		}
		if ( !strcmp( "func_invisible_user", ent->classname ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "norway" ) ) {
		if ( !strcmp( "func_invisible_user", ent->classname ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "rocket" ) ) {
		if ( !strcmp( "func_invisible_user", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "sfm" ) ) {
		if ( !strcmp( "func_invisible_user", ent->classname ) ) {
			return qfalse;
		}
		if ( ( !strcmp( "func_door_rotating", ent->classname ) ) && ( ent->r.currentOrigin[0] == 478 ) && ( ent->r.currentOrigin[1] == -576 ) && ( ent->r.currentOrigin[2] == -64 ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "swf" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == 1986 ) && ( ent->r.currentOrigin[1] == -192 ) && ( ent->r.currentOrigin[2] == 544 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 2368 ) && ( ent->r.currentOrigin[1] == -514 ) && ( ent->r.currentOrigin[2] == 548 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 1440 ) && ( ent->r.currentOrigin[1] == -1090 ) && ( ent->r.currentOrigin[2] == 544 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 1440 ) && ( ent->r.currentOrigin[1] == -1214 ) && ( ent->r.currentOrigin[2] == 544 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 354 ) && ( ent->r.currentOrigin[1] == -1288 ) && ( ent->r.currentOrigin[2] == 544 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -98 ) && ( ent->r.currentOrigin[1] == 16 ) && ( ent->r.currentOrigin[2] == 544 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -222 ) && ( ent->r.currentOrigin[1] == 16 ) && ( ent->r.currentOrigin[2] == 544 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 384 ) && ( ent->r.currentOrigin[1] == -732 ) && ( ent->r.currentOrigin[2] == 484 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 384 ) && ( ent->r.currentOrigin[1] == -548 ) && ( ent->r.currentOrigin[2] == 484 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 786 ) && ( ent->r.currentOrigin[1] == 168 ) && ( ent->r.currentOrigin[2] == 376 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "trainyard" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == -1480 ) && ( ent->r.currentOrigin[1] == 606 ) && ( ent->r.currentOrigin[2] == 120 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 1904 ) && ( ent->r.currentOrigin[1] == 382 ) && ( ent->r.currentOrigin[2] == -192 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 1406 ) && ( ent->r.currentOrigin[1] == 80 ) && ( ent->r.currentOrigin[2] == -192 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 3522 ) && ( ent->r.currentOrigin[1] == 200 ) && ( ent->r.currentOrigin[2] == 72 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 2120 ) && ( ent->r.currentOrigin[1] == -610 ) && ( ent->r.currentOrigin[2] == 72 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "tram" ) ) {
		if ( !strcmp( "trigger_hurt", ent->classname ) ) {
			ent->classname = "trigger_push";
		}
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == -252 ) && ( ent->r.currentOrigin[1] == 318 ) && ( ent->r.currentOrigin[2] == 504 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -2718 ) && ( ent->r.currentOrigin[1] == 36 ) && ( ent->r.currentOrigin[2] == -264 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -3784 ) && ( ent->r.currentOrigin[1] == -1598 ) && ( ent->r.currentOrigin[2] == -576 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -3784 ) && ( ent->r.currentOrigin[1] == -1474 ) && ( ent->r.currentOrigin[2] == -576 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "village1" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == -1040 ) && ( ent->r.currentOrigin[1] == 130 ) && ( ent->r.currentOrigin[2] == -128 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -880 ) && ( ent->r.currentOrigin[1] == 130 ) && ( ent->r.currentOrigin[2] == -128 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 610 ) && ( ent->r.currentOrigin[1] == 1920 ) && ( ent->r.currentOrigin[2] == 68 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 236 ) && ( ent->r.currentOrigin[1] == 2338 ) && ( ent->r.currentOrigin[2] == -128 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 64 ) && ( ent->r.currentOrigin[1] == -670 ) && ( ent->r.currentOrigin[2] == -128 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 318 ) && ( ent->r.currentOrigin[1] == -324 ) && ( ent->r.currentOrigin[2] == -128 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "village2" ) ) {
		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( ( ent->r.currentOrigin[0] == 2808 ) && ( ent->r.currentOrigin[1] == -66 ) && ( ent->r.currentOrigin[2] == 8 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == 1024 ) && ( ent->r.currentOrigin[1] == -799 ) && ( ent->r.currentOrigin[2] == -64 ) ) {
				return qfalse;
			}
			if ( ( ent->r.currentOrigin[0] == -968 ) && ( ent->r.currentOrigin[1] == 705 ) && ( ent->r.currentOrigin[2] == 2 ) ) {
				return qfalse;
			}
		}
	}
	if ( !Q_stricmp( mapName, "xlabs" ) ) {
		if ( !strcmp( "trigger_hurt", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "func_invisible_user", ent->classname ) ) {
			return qfalse;
		}
		if ( ( !strcmp( "func_explosive", ent->classname ) ) && ( ent->spawnflags == 4 ) ) {
			return qfalse;
		}
		if ( ( !strcmp( "func_door_rotating", ent->classname ) ) && ( ent->r.currentOrigin[0] == 1500 ) && ( ent->r.currentOrigin[1] == 80 ) && ( ent->r.currentOrigin[2] == 48 ) ) {
			return qfalse;
		}
	}

//
//	AG3NT - Sniper spawn (actually more like DONT spawn)
//
	if ( g_sniperWar.integer ) {
		if ( !strcmp( "team_WOLF_checkpoint", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "misc_mg42", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "item_treasure", ent->classname ) ) {
			return qfalse;
		}
		//if ( !strcmp("ammo_panzerfaust", ent->classname) )
		//return qfalse;
		if ( !strcmp( "team_CTF_blueflag", ent->classname ) ) {
			return qfalse;
		}
		if ( !strcmp( "team_CTF_redflag", ent->classname ) ) {
			return qfalse;
		}

		// cs: lets check the class name first so every entity isnt checking against map names ...
		if ( !strcmp( "func_explosive", ent->classname ) ) {
			if ( ( Q_stricmp( mapName, "mp_beach" ) ) 
				&& ( Q_stricmp( mapName, "mp_ice" ) ) 
				&& ( Q_stricmp( mapName, "tt_trench" ) ) 
				&& ( Q_stricmp( mapName, "baseout" ) ) 
				&& ( Q_stricmp( mapName, "tram" ) ) 
				&& ( Q_stricmp( mapName, "fun_beach_2" ) ) 
				&& ( Q_stricmp( mapName, "te_ufo" ) ) 
				&& ( Q_stricmp( mapName, "mml_church_v1" ) ) 
				&& ( Q_stricmp( mapName, "factory" ) ) 
				&& ( Q_stricmp( mapName, "dark" ) ) 
				&& ( Q_stricmp( mapName, "forest" ) ) 
				&& ( Q_stricmp( mapName, "assault" ) ) 
				&& ( Q_stricmp( mapName, "dam" ) ) 
				&& ( Q_stricmp( mapName, "sfm" ) ) 
				&& ( Q_stricmp( mapName, "hs_sniperbeach" ) ) 
				&& ( Q_stricmp( mapName, "mp_frost" ) ) 
				&& ( Q_stricmp( mapName, "mp_assault" ) ) )
			{
				return qfalse;
			}
		}

		if ( !strcmp( "func_static", ent->classname ) ) {
			if ( ( Q_stricmp( mapName, "mp_beach" ) ) 
				&& ( Q_stricmp( mapName, "mp_ice" ) ) 
				&& ( Q_stricmp( mapName, "factory" ) ) 
				&& ( Q_stricmp( mapName, "mp_gta2" ) ) 
				&& ( Q_stricmp( mapName, "baseout" ) ) 
				&& ( Q_stricmp( mapName, "tram" ) ) 
				&& ( Q_stricmp( mapName, "mml_church_v1" ) ) 
				&& ( Q_stricmp( mapName, "dark" ) ) 
				&& ( Q_stricmp( mapName, "sfm" ) ) 
				&& ( Q_stricmp( mapName, "dam" ) ) 
				&& ( Q_stricmp( mapName, "mp_sub" ) ) 
				&& ( Q_stricmp( mapName, "mp_depot" ) ) )
			{
				return qfalse;
			}
		}

		if ( !strcmp( "func_door", ent->classname ) ) {
			if ( !Q_stricmp( mapName, "mp_destruction" )
				|| !Q_stricmp( mapName, "mp_gta2" ) )
			{
				return qfalse;
			}
		}

		if ( !strcmp( "func_door_rotating", ent->classname ) ) {
			if ( !Q_stricmp( mapName, "mp_destruction" )
				|| !Q_stricmp( mapName, "mp_assault" )
				|| !Q_stricmp( mapName, "mp_gta2" ) )
			{
				return qfalse;
			}
		}

		if ( !strcmp( "trigger_hurt", ent->classname ) ) {
			if ( ( Q_stricmp( mapName, "dam" ) ) 
				&& ( Q_stricmp( mapName, "border" ) ) 
				&& ( Q_stricmp( mapName, "te_ufo" ) ) 
				&& ( Q_stricmp( mapName, "train_siege" ) ) 
				&& ( Q_stricmp( mapName, "dark" ) ) )
			{
					return qfalse;
			}
		}

		if ( ( !strcmp( "func_explosive", ent->classname ) ) && ( ent->spawnflags == 32 ) ) {
			if ( !Q_stricmp( mapName, "hs_sniperbeach" ) ) {
					return qfalse;
			}
		}
	}

//S4NDM4NN- decide here what we dont want to spawn and such
	if ( ( g_deathmatch.integer ) || ( g_panzerWar.integer ) || ( g_SpamWar.integer ) ) {

		if ( !strcmp( "func_explosive", ent->classname ) ) {
			if ( !Q_stricmp( mapName, "mp_beach" ) || !Q_stricmp( mapName, "mp_castle" ) ) {
					return qfalse;
			}
		}

		if ( !strcmp( "team_WOLF_checkpoint", ent->classname ) ) {
			return qfalse;
		}

		if ( !strcmp( "trigger_hurt", ent->classname ) ) {
			return qfalse;
		}

		if ( !strcmp( "func_static", ent->classname ) ) {
			return qfalse;
		}

	}
	if ( g_instaGib.integer || g_panzerWar.integer ) {
		if ( !strcmp( "misc_mg42", ent->classname ) ) {
			return qfalse;
		}
	}


/*
//make radar models be solid, so noobs cant camp in them.
    if (!strcmp(ent->classname,"misc_model") && !strcmp(ent->model,"models/mapobjects/miltary_trim/radar_twr.md3")){
        ent->clipmask = CONTENTS_SOLID;
        ent->r.contents = CONTENTS_SOLID;
        return qtrue;//we still want to spawn so return true.<hope this isnt changing it too soon :0
    }
*/
// Martin -2/15/2011 - This doesnt work.
/*	if ( ( ent->model ) &&  ( !strcmp( "models/mapobjects/miltary_trim/radar_twr.md3", ent->model ) ) ) {
		ent->clipmask = CONTENTS_SOLID;
		ent->r.contents = CONTENTS_SOLID;
		VectorSet( ent->r.mins, -8, -8, 0 );
		VectorSet( ent->r.maxs, 1000, 1000, 128 );
		trap_LinkEntity( ent );
		return qfalse; //we still want to spawn so return true.<hope this isnt changing it too soon :0
	}
*/
	//make sandbags SOLID
//	if ((ent->model) && (!strcmp("models/mapobjects/miltary_trim/sandbag1_45.md3", ent->model) )){
	//	return qfalse;
	/*
	ent->spawnflags = 3;
	//ent->clipmask = MASK_SHOT;
	ent->clipmask = MASK_PLAYERSOLID | CONTENTS_SOLID | CONTENTS_MISSILECLIP;
	trap_LinkEntity (ent);
	//ent->r.contents = CONTENTS_SOLID;
	*/
	//}

	return qtrue;
}
