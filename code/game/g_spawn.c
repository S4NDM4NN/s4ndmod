/*
 * name:		g_spawn.c
 *
 * desc:
 *
 * $Author: Darin Mcneil $
 * $Revision: 7 $
 * $Modtime: 11/05/01 3:38p $
 * $Date: 11/05/01 3:38p $
*/

#include "g_local.h"
#include "S4ND_local.h"
qboolean    G_SpawnString( const char *key, const char *defaultString, char **out ) {
	int i;

	if ( !level.spawning ) {
		*out = (char *)defaultString;
//		G_Error( "G_SpawnString() called while not spawning" );
	}

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		if ( !strcmp( key, level.spawnVars[i][0] ) ) {
			*out = level.spawnVars[i][1];
			return qtrue;
		}
	}

	*out = (char *)defaultString;
	return qfalse;
}

qboolean    G_SpawnFloat( const char *key, const char *defaultString, float *out ) {
	char        *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atof( s );
	return present;
}

qboolean    G_SpawnInt( const char *key, const char *defaultString, int *out ) {
	char        *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atoi( s );
	return present;
}

qboolean    G_SpawnVector( const char *key, const char *defaultString, float *out ) {
	char        *s;
	qboolean present;

	present = G_SpawnString( key, defaultString, &s );
	sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] );
	return present;
}



//
// fields are needed for spawning from the entity string
//
typedef enum {
	F_INT,
	F_FLOAT,
	F_LSTRING,          // string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,          // string on disk, pointer in memory, TAG_GAME
	F_VECTOR,
	F_ANGLEHACK,
	F_ENTITY,           // index on disk, pointer in memory
	F_ITEM,             // index on disk, pointer in memory
	F_CLIENT,           // index on disk, pointer in memory
	F_IGNORE
} fieldtype_t;

typedef struct
{
	char    *name;
	int ofs;
	fieldtype_t type;
	int flags;
} field_t;

field_t fields[] = {
	{"classname",   FOFS( classname ),    F_LSTRING},
	{"origin",      FOFS( s.origin ),     F_VECTOR},
	{"model",       FOFS( model ),        F_LSTRING},
	{"model2",      FOFS( model2 ),       F_LSTRING},
	{"spawnflags",  FOFS( spawnflags ),   F_INT},
	{"speed",       FOFS( speed ),        F_FLOAT},
	{"closespeed",  FOFS( closespeed ),   F_FLOAT},   //----(SA)	added
	{"target",      FOFS( target ),       F_LSTRING},
	{"targetname",  FOFS( targetname ),   F_LSTRING},
	{"message",     FOFS( message ),      F_LSTRING},
	{"popup",       FOFS( message ),      F_LSTRING}, // (SA) mutually exclusive from 'message', but makes the ent more logical for the level designer
	{"book",        FOFS( message ),      F_LSTRING}, // (SA) mutually exclusive from 'message', but makes the ent more logical for the level designer
	{"team",        FOFS( team ),         F_LSTRING},
	{"wait",        FOFS( wait ),         F_FLOAT},
	{"random",      FOFS( random ),       F_FLOAT},
	{"count",       FOFS( count ),        F_INT},
	{"health",      FOFS( health ),       F_INT},
	{"light",       0,                  F_IGNORE},
	{"dmg",         FOFS( damage ),       F_INT},
	{"angles",      FOFS( s.angles ),     F_VECTOR},
	{"angle",       FOFS( s.angles ),     F_ANGLEHACK},
	// JOSEPH 9-27-99
	{"duration",    FOFS( duration ),     F_FLOAT},
	{"rotate",      FOFS( rotate ),       F_VECTOR},
	// END JOSEPH
	{"degrees",     FOFS( angle ),        F_FLOAT},
	{"time",        FOFS( speed ),        F_FLOAT},

	// Ridah, AI fields
	{"aiattributes", FOFS( aiAttributes ), F_LSTRING},
	{"ainame",      FOFS( aiName ),       F_LSTRING},
	{"aiteam",      FOFS( aiTeam ),       F_INT},
	// done.

	//----(SA) additional ai field
	{"skin",        FOFS( aiSkin ),       F_LSTRING},
	{"head",        FOFS( aihSkin ),      F_LSTRING},

	//----(SA) done

	// (SA) dlight lightstyles (made all these unique variables for testing)
	{"_color",      FOFS( dl_color ),     F_VECTOR},      // color of the light	(the underscore is inserted by the color picker in QER)
	{"color",       FOFS( dl_color ),     F_VECTOR},      // color of the light
	{"stylestring", FOFS( dl_stylestring ), F_LSTRING},   // user defined stylestring "fffndlsfaaaaaa" for example
	// done

	//----(SA)
	{"shader",      FOFS( dl_shader ), F_LSTRING},    // shader to use for a target_effect or dlight

	// (SA) for target_unlock
	{"key",         FOFS( key ),      F_INT},
	// done

	// (SA) for item placement
	{"stand",       FOFS( s.frame ),      F_INT},
	// (SA)	end

	// Rafael - mg42
	{"harc",        FOFS( harc ),         F_FLOAT},
	{"varc",        FOFS( varc ),         F_FLOAT},
	// done.

	// Rafael - sniper
	{"delay",   FOFS( delay ),        F_FLOAT},
	{"radius",  FOFS( radius ),       F_INT},

	// Ridah, for reloading savegames at correct mission spot
	{"missionlevel",    FOFS( missionLevel ), F_INT},

	// Rafel
	{"start_size", FOFS( start_size ), F_INT},
	{"end_size", FOFS( end_size ), F_INT},

	{"shard", FOFS( count ), F_INT},

	// Rafael
	{"spawnitem",       FOFS( spawnitem ),            F_LSTRING},

	{"track",           FOFS( track ),                F_LSTRING},

	{"scriptName",      FOFS( scriptName ),           F_LSTRING},

	{NULL}
};



void SP_info_player_start( gentity_t *ent );
void SP_info_player_checkpoint( gentity_t *ent );
void SP_info_player_deathmatch( gentity_t *ent );
void SP_info_player_intermission( gentity_t *ent );
void SP_info_firstplace( gentity_t *ent );
void SP_info_secondplace( gentity_t *ent );
void SP_info_thirdplace( gentity_t *ent );
void SP_info_podium( gentity_t *ent );

void SP_func_plat( gentity_t *ent );
void SP_func_static( gentity_t *ent );
void SP_func_leaky( gentity_t *ent ); //----(SA)	added
void SP_func_rotating( gentity_t *ent );
void SP_func_bobbing( gentity_t *ent );
void SP_func_pendulum( gentity_t *ent );
void SP_func_button( gentity_t *ent );
void SP_func_explosive( gentity_t *ent );
void SP_func_door( gentity_t *ent );
void SP_func_train( gentity_t *ent );
void SP_func_timer( gentity_t *self );
// JOSEPH 1-26-00
void SP_func_train_rotating( gentity_t *ent );
void SP_func_secret( gentity_t *ent );
// END JOSEPH
// Rafael
void SP_func_door_rotating( gentity_t *ent );
// RF
void SP_func_bats( gentity_t *self );

void SP_trigger_always( gentity_t *ent );
void SP_trigger_multiple( gentity_t *ent );
void SP_trigger_push( gentity_t *ent );
void SP_trigger_teleport( gentity_t *ent );
void SP_trigger_hurt( gentity_t *ent );

//---- (SA) Wolf triggers
void SP_trigger_concussive_dust( gentity_t *ent ); // JPW NERVE
void SP_trigger_once( gentity_t *ent );
//---- done

void SP_target_remove_powerups( gentity_t *ent );
void SP_target_give( gentity_t *ent );
void SP_target_delay( gentity_t *ent );
void SP_target_speaker( gentity_t *ent );
void SP_target_print( gentity_t *ent );
void SP_target_laser( gentity_t *self );
void SP_target_character( gentity_t *ent );
void SP_target_score( gentity_t *ent );
void SP_target_teleporter( gentity_t *ent );
void SP_target_relay( gentity_t *ent );
void SP_target_kill( gentity_t *ent );
void SP_target_position( gentity_t *ent );
void SP_target_location( gentity_t *ent );
void SP_target_push( gentity_t *ent );
void SP_target_script_trigger( gentity_t *ent );

//---- (SA) Wolf targets
// targets
void SP_target_alarm( gentity_t *ent );
void SP_target_counter( gentity_t *ent );
void SP_target_lock( gentity_t *ent );
void SP_target_effect( gentity_t *ent );
void SP_target_fog( gentity_t *ent );
void SP_target_autosave( gentity_t *ent );

// entity visibility dummy
void SP_misc_vis_dummy( gentity_t *ent );
void SP_misc_vis_dummy_multiple( gentity_t *ent );

//----(SA) done

void SP_light( gentity_t *self );
void SP_info_null( gentity_t *self );
void SP_info_notnull( gentity_t *self );
void SP_info_camp( gentity_t *self );
void SP_path_corner( gentity_t *self );

void SP_misc_teleporter_dest( gentity_t *self );
void SP_misc_model( gentity_t *ent );
void SP_misc_gamemodel( gentity_t *ent );
void SP_misc_portal_camera( gentity_t *ent );
void SP_misc_portal_surface( gentity_t *ent );
void SP_misc_light_surface( gentity_t *ent );
void SP_misc_grabber_trap( gentity_t *ent );
void SP_misc_spotlight( gentity_t *ent ); //----(SA)	added

void SP_shooter_rocket( gentity_t *ent );
void SP_shooter_grenade( gentity_t *ent );

void SP_team_CTF_redplayer( gentity_t *ent );
void SP_team_CTF_blueplayer( gentity_t *ent );

void SP_team_CTF_redspawn( gentity_t *ent );
void SP_team_CTF_bluespawn( gentity_t *ent );

// JPW NERVE for multiplayer spawnpoint selection
void SP_team_WOLF_objective( gentity_t *ent );
// jpw

void SP_team_WOLF_checkpoint( gentity_t *ent );     // DHM - Nerve

// JOSEPH 1-18-00
void SP_props_box_32( gentity_t *self );
void SP_props_box_48( gentity_t *self );
void SP_props_box_64( gentity_t *self );
// END JOSEPH

// Ridah
void SP_ai_soldier( gentity_t *ent );
void SP_ai_american( gentity_t *ent );
void SP_ai_zombie( gentity_t *ent );
void SP_ai_warzombie( gentity_t *ent );
void SP_ai_femzombie( gentity_t *ent );
void SP_ai_undead( gentity_t *ent );
void SP_ai_marker( gentity_t *ent );
void SP_ai_effect( gentity_t *ent );
void SP_ai_trigger( gentity_t *ent );
void SP_ai_venom( gentity_t *ent );
void SP_ai_loper( gentity_t *ent );
void SP_ai_sealoper( gentity_t *ent );
void SP_ai_boss_helga( gentity_t *ent );
void SP_ai_boss_heinrich( gentity_t *ent ); //----(SA)	added
void SP_ai_eliteguard( gentity_t *ent );
void SP_ai_stimsoldier_dual( gentity_t *ent );
void SP_ai_stimsoldier_rocket( gentity_t *ent );
void SP_ai_stimsoldier_tesla( gentity_t *ent );
void SP_ai_supersoldier( gentity_t *ent );
void SP_ai_blackguard( gentity_t *ent );
void SP_ai_protosoldier( gentity_t *ent );
void SP_ai_rejectxcreature( gentity_t *ent );
void SP_ai_frogman( gentity_t *ent );
void SP_ai_partisan( gentity_t *ent );
void SP_ai_civilian( gentity_t *ent );
void SP_ai_chimp( gentity_t *ent ); //----(SA)	added
// done.

// Rafael particles
void SP_Snow( gentity_t *ent );
void SP_target_smoke( gentity_t *ent );
void SP_Bubbles( gentity_t *ent );
// done.

// (SA) dlights
void SP_dlight( gentity_t *ent );
// done
void SP_corona( gentity_t *ent );

// Rafael mg42
void SP_mg42( gentity_t *ent );
// done.

// Rafael sniper
void SP_shooter_sniper( gentity_t *ent );
void SP_sniper_brush( gentity_t *ent );
// done

//----(SA)
void SP_shooter_zombiespit( gentity_t *ent );
void SP_shooter_mortar( gentity_t *ent );
void SP_shooter_tesla( gentity_t *ent );

// alarm
void SP_alarm_box( gentity_t *ent );
//----(SA)	end

void SP_trigger_flagonly( gentity_t *ent );     // DHM - Nerve
void SP_trigger_objective_info( gentity_t *ent );   // DHM - Nerve

void SP_gas( gentity_t *ent );
void SP_target_rumble( gentity_t *ent );
void SP_func_train_particles( gentity_t *ent );


// Rafael
void SP_trigger_aidoor( gentity_t *ent );
void SP_SmokeDust( gentity_t *ent );
void SP_Dust( gentity_t *ent );
void SP_props_sparks( gentity_t *ent );
void SP_props_gunsparks( gentity_t *ent );

// Props
void SP_Props_Bench( gentity_t *ent );
void SP_Props_Radio( gentity_t *ent );
void SP_Props_Chair( gentity_t *ent );
void SP_Props_ChairHiback( gentity_t *ent );
void SP_Props_ChairSide( gentity_t *ent );
void SP_Props_ChairChat( gentity_t *ent );
void SP_Props_ChairChatArm( gentity_t *ent );
void SP_Props_DamageInflictor( gentity_t *ent );
void SP_Props_Locker_Tall( gentity_t *ent );
void SP_Props_Desklamp( gentity_t *ent );
void SP_Props_Flamebarrel( gentity_t *ent );
void SP_crate_64( gentity_t *ent );
void SP_Props_Flipping_Table( gentity_t *ent );
void SP_crate_32( gentity_t *self );
void SP_Props_Crate32x64( gentity_t *ent );
void SP_Props_58x112tablew( gentity_t *ent );
void SP_props_castlebed( gentity_t *ent );
void SP_Props_RadioSEVEN( gentity_t *ent );
void SP_propsFireColumn( gentity_t *ent );
void SP_props_flamethrower( gentity_t *ent );

void SP_func_tramcar( gentity_t *ent );
void func_invisible_user( gentity_t *ent );

void SP_lightJunior( gentity_t *self );

void SP_props_me109( gentity_t *ent );
void SP_misc_flak( gentity_t *ent );
void SP_plane_waypoint( gentity_t *self );

void SP_props_snowGenerator( gentity_t *ent );
void SP_truck_cam( gentity_t *self );

void SP_screen_fade( gentity_t *ent );
void SP_camera_reset_player( gentity_t *ent );
void SP_camera_cam( gentity_t *ent );
void SP_props_decoration( gentity_t *ent );
void SP_props_decorBRUSH( gentity_t *ent );
void SP_props_statue( gentity_t *ent );
void SP_props_statueBRUSH( gentity_t *ent );
void SP_skyportal( gentity_t *ent );

// RF, scripting
void SP_script_model_med( gentity_t *ent );
void SP_script_mover( gentity_t *ent );
void SP_script_multiplayer( gentity_t *ent );         // DHM - Nerve

void SP_misc_mounted_gunner( gentity_t *ent );
void SP_props_footlocker( gentity_t *self );
void SP_misc_firetrails( gentity_t *ent );
void SP_trigger_deathCheck( gentity_t *ent );
void SP_misc_spawner( gentity_t *ent );
void SP_props_decor_Scale( gentity_t *ent );



spawn_t spawns[] = {
	// info entities don't do anything at all, but provide positional
	// information for things controlled by other processes
	{"info_player_start", SP_info_player_start},

	{"info_player_checkpoint", SP_info_player_checkpoint},

	{"info_player_deathmatch", SP_info_player_deathmatch},
	{"info_player_intermission", SP_info_player_intermission},
	{"info_null", SP_info_null},
	{"info_notnull", SP_info_notnull},      // use target_position instead
	{"info_camp", SP_info_camp},

	{"func_plat", SP_func_plat},
	{"func_button", SP_func_button},
	{"func_explosive", SP_func_explosive},
	{"func_door", SP_func_door},
	{"func_static", SP_func_static},
	{"func_leaky", SP_func_leaky},
	{"func_rotating", SP_func_rotating},
	{"func_bobbing", SP_func_bobbing},
	{"func_pendulum", SP_func_pendulum},
	{"func_train", SP_func_train},
	{"func_group", SP_info_null},
	// JOSEPH 1-26-00
	{"func_train_rotating", SP_func_train_rotating},
	{"func_secret", SP_func_secret},
	// END JOSEPH
	// Rafael
	{"func_door_rotating", SP_func_door_rotating},

	{"func_train_particles", SP_func_train_particles},

	{"func_timer", SP_func_timer},          // rename trigger_timer?

	{"func_tramcar", SP_func_tramcar},
	{"func_invisible_user", func_invisible_user},

	{"func_bats", SP_func_bats},

	// Triggers are brush objects that cause an effect when contacted
	// by a living player, usually involving firing targets.
	// While almost everything could be done with
	// a single trigger class and different targets, triggered effects
	// could not be client side predicted (push and teleport).
	{"trigger_always", SP_trigger_always},
	{"trigger_multiple", SP_trigger_multiple},
	{"trigger_push", SP_trigger_push},
	{"trigger_teleport", SP_trigger_teleport},
	{"trigger_hurt", SP_trigger_hurt},

	//---- (SA) Wolf triggers
	{"trigger_concussive_dust", SP_trigger_concussive_dust}, // JPW NERVE
	{"trigger_once",        SP_trigger_once},
	//---- done

	// Rafael
	{"trigger_aidoor", SP_trigger_aidoor},
	{"trigger_deathCheck",SP_trigger_deathCheck},

	// targets perform no action by themselves, but must be triggered
	// by another entity
	{"target_give", SP_target_give},
	{"target_remove_powerups", SP_target_remove_powerups},
	{"target_delay", SP_target_delay},
	{"target_speaker", SP_target_speaker},
	{"target_print", SP_target_print},
	{"target_laser", SP_target_laser},
	{"target_score", SP_target_score},
	{"target_teleporter", SP_target_teleporter},
	{"target_relay", SP_target_relay},
	{"target_kill", SP_target_kill},
	{"target_position", SP_target_position},
	{"target_location", SP_target_location},
	{"target_push", SP_target_push},
	{"target_script_trigger", SP_target_script_trigger},

	//---- (SA) Wolf targets
	{"target_alarm",        SP_target_alarm},
	{"target_counter",      SP_target_counter},
	{"target_lock",         SP_target_lock},
	{"target_effect",       SP_target_effect},
	{"target_fog",          SP_target_fog},
	{"target_autosave",     SP_target_autosave},    //----(SA)	added
	//---- done

	{"target_rumble", SP_target_rumble},


	{"light", SP_light},

	{"lightJunior", SP_lightJunior},

	{"path_corner", SP_path_corner},

	{"misc_teleporter_dest", SP_misc_teleporter_dest},
	{"misc_model", SP_misc_model},
	{"misc_gamemodel", SP_misc_gamemodel},
	{"misc_portal_surface", SP_misc_portal_surface},
	{"misc_portal_camera", SP_misc_portal_camera},

	//----(SA) Wolf misc
	{"misc_vis_dummy",      SP_misc_vis_dummy},
	{"misc_vis_dummy_multiple",     SP_misc_vis_dummy_multiple},
	{"misc_light_surface",  SP_misc_light_surface},
	{"misc_grabber_trap",   SP_misc_grabber_trap},
	{"misc_spotlight",      SP_misc_spotlight}, //----(SA)	added
	//----(SA) end


	// Rafael mg42
	{"misc_mg42", SP_mg42},
	// done.
	{"misc_flak", SP_misc_flak},
	{"misc_mounted_gunner",SP_misc_mounted_gunner},
	{"misc_firetrails", SP_misc_firetrails},

	{"shooter_rocket", SP_shooter_rocket},
	{"shooter_grenade", SP_shooter_grenade},

//----(SA)
	{"shooter_zombiespit", SP_shooter_zombiespit},
	{"shooter_mortar", SP_shooter_mortar},
	{"shooter_tesla", SP_shooter_tesla},

	// alarm
	{"alarm_box", SP_alarm_box},
//----(SA)	end

	// Rafael sniper
	{"shooter_sniper", SP_shooter_sniper},
	{"sniper_brush", SP_sniper_brush},
	// done

	{"team_CTF_redplayer", SP_team_CTF_redplayer},
	{"team_CTF_blueplayer", SP_team_CTF_blueplayer},

	{"team_CTF_redspawn", SP_team_CTF_redspawn},
	{"team_CTF_bluespawn", SP_team_CTF_bluespawn},

// JPW NERVE
	{"team_WOLF_objective", SP_team_WOLF_objective},
// jpw

	{"team_WOLF_checkpoint", SP_team_WOLF_checkpoint},      // DHM - Nerve

	//S4NDM4NN
	{"CTF_flag", SP_CTF_flag},
	{"cust_spawnflag", SP_cust_spawnflag},
	//END

	// Ridah
	//S4NDM4NN ai shit WAS here haha
	// done.

	// Rafael particles
	{"misc_snow256", SP_Snow},
	{"misc_snow128", SP_Snow},
	{"misc_snow64", SP_Snow},
	{"misc_snow32", SP_Snow},
	{"target_smoke", SP_target_smoke},

	{"misc_bubbles8", SP_Bubbles},
	{"misc_bubbles16", SP_Bubbles},
	{"misc_bubbles32", SP_Bubbles},
	{"misc_bubbles64", SP_Bubbles},
	// done.

	{"misc_spawner", SP_misc_spawner},

	// JOSEPH 1-18-00
	{"props_box_32", SP_props_box_32},
	{"props_box_48", SP_props_box_48},
	{"props_box_64", SP_props_box_64},
	// END JOSEPH

	// Rafael
	{"props_smokedust", SP_SmokeDust},
	{"props_dust", SP_Dust},
	{"props_sparks", SP_props_sparks},
	{"props_gunsparks", SP_props_gunsparks},

	{"plane_waypoint", SP_plane_waypoint},

	{"props_me109", SP_props_me109},

	// Rafael - props
	{"props_bench", SP_Props_Bench},
	{"props_radio", SP_Props_Radio},
	{"props_chair", SP_Props_Chair},
	{"props_chair_hiback", SP_Props_ChairHiback},
	{"props_chair_side", SP_Props_ChairSide},
	{"props_chair_chat", SP_Props_ChairChat},
	{"props_chair_chatarm", SP_Props_ChairChatArm},
	{"props_damageinflictor", SP_Props_DamageInflictor},
	{"props_locker_tall",SP_Props_Locker_Tall},
	{"props_desklamp", SP_Props_Desklamp},
	{"props_flamebarrel", SP_Props_Flamebarrel},
	{"props_crate_64", SP_crate_64},
	{"props_flippy_table", SP_Props_Flipping_Table},
	{"props_crate_32", SP_crate_32},
	{"props_crate_32x64", SP_Props_Crate32x64},
	{"props_58x112tablew", SP_Props_58x112tablew},
	{"props_castlebed", SP_props_castlebed},
	{"props_radioSEVEN", SP_Props_RadioSEVEN},
	{"props_snowGenerator", SP_props_snowGenerator},
	{"props_FireColumn", SP_propsFireColumn},
	{"props_decoration", SP_props_decoration},
	{"props_decorBRUSH", SP_props_decorBRUSH},
	{"props_statue", SP_props_statue},
	{"props_statueBRUSH", SP_props_statueBRUSH},
	{"props_skyportal", SP_skyportal},
	{"props_footlocker", SP_props_footlocker},
	{"props_flamethrower", SP_props_flamethrower},
	{"props_decoration_scale",SP_props_decor_Scale},

	{"truck_cam", SP_truck_cam},

	{"screen_fade", SP_screen_fade},
	{"camera_reset_player", SP_camera_reset_player},
	{"camera_cam",SP_camera_cam},

	// (SA) dlight and dlightstyles
	{"dlight",      SP_dlight},

	//----(SA)	light coronas
	{"corona",      SP_corona},

	{"test_gas", SP_gas},
	{"trigger_flagonly", SP_trigger_flagonly},              // DHM - Nerve
	{"trigger_objective_info", SP_trigger_objective_info},  // DHM - Nerve

	// RF, scripting
	{"script_model_med", SP_script_model_med},
	{"script_mover", SP_script_mover},
	{"script_multiplayer", SP_script_multiplayer},          // DHM - Nerve


	{0, 0}
};

/*
===============
G_CallSpawn

Finds the spawn function for the entity and calls it,
returning qfalse if not found
===============
*/
qboolean G_CallSpawn( gentity_t *ent ) {
	spawn_t *s;
	gitem_t *item;
	char mapName[32];
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	if ( !ent->classname ) {
		G_Printf( "G_CallSpawn: NULL classname\n" );
		return qfalse;
	}
	if ( !S4ND_Spawn( ent ) ) {
		return qfalse;
	}

//End
	// check item spawn functions
	for ( item = bg_itemlist + 1 ; item->classname ; item++ ) {
		if ( !strcmp( item->classname, ent->classname ) ) {
			// found it
			// DHM - Nerve :: allow flags in GTWOLF
			if ( item->giType == IT_TEAM && ( g_gametype.integer != GT_CTF && g_gametype.integer < GT_WOLF ) ) {
				return qfalse;
			}
			G_SpawnItem( ent, item );
			return qtrue;
		}
	}

	// check normal spawn functions
	for ( s = spawns ; s->name ; s++ ) {
		if ( !strcmp( s->name, ent->classname ) ) {
			// found it
			s->spawn( ent );

			// RF, entity scripting
			if ( ent->s.number >= MAX_CLIENTS && ent->scriptName ) {
				G_Script_ScriptParse( ent );
				G_Script_ScriptEvent( ent, "spawn", "" );
			}

			//if(!strcmp("target_location", ent->classname)){
			//G_LogPrintf("scriptname:%s\n",ent->classname);
			//G_LogPrintf("target name:%s\n",ent->message);
			//G_LogPrintf("tagname: %i\n",ent->health);

//	}
/*
    if(!strcmp("script_mover", ent->classname)){
        if(!strcmp("me109no1", ent->scriptName) || !strcmp("me109no2", ent->scriptName)){
            ent->nextthink =level.time+ (15000 + crandom()*15000);//make more random to cause a/s to be further apart
            ent->think = ForestPlanesThink;
        }
    }
*/                                                                                                                                                                                                                                                                                                    //make radar models be solid, so noobs cant camp in them.
			//		if (!strcmp("misc_model", ent->classname)){
			//	if (!strcmp("models/mapobjects/miltary_trim/radar_twr.md3", ent->model)){
/*			if ((ent->model2) &&  (!strcmp("models/mapobjects/miltary_trim/radar_twr.md3", ent->model2))){
                G_Printf("************************************\nchanedged radar to solid\n************************************\n");
        ent->clipmask = CONTENTS_SOLID;
        ent->r.contents = CONTENTS_SOLID;
        VectorSet (ent->r.mins, -8, -8, 0);
        VectorSet (ent->r.maxs, 100, 100, 128);
        trap_LinkEntity (ent);
        trap_SendServerCommand(-1, va("chat \"WE HAVE CALLED THE MODEL STUFF!\n\""));

        return qtrue;//we still want to spawn so return true.<hope this isnt changing it too soon :0
    //	}
    }*/

			//G_Printf("************************************\nchanedged radar to solid\n************************************\n");
			return qtrue;
		}

	}
	G_Printf( "%s doesn't have a spawn function\n", ent->classname );
	return qfalse;
}

/*
=============
G_NewString

Builds a copy of the string, translating \n to real linefeeds
so message texts can be multi-line
=============
*/
char *G_NewString( const char *string ) {
	char    *newb, *new_p;
	int i,l;

	l = strlen( string ) + 1;

	newb = G_Alloc( l );

	new_p = newb;

	// turn \n into a real linefeed
	for ( i = 0 ; i < l ; i++ ) {
		if ( string[i] == '\\' && i < l - 1 ) {
			i++;
			if ( string[i] == 'n' ) {
				*new_p++ = '\n';
			} else {
				*new_p++ = '\\';
			}
		} else {
			*new_p++ = string[i];
		}
	}

	return newb;
}




/*
===============
G_ParseField

Takes a key/value pair and sets the binary values
in a gentity
===============
*/
void G_ParseField( const char *key, const char *value, gentity_t *ent ) {
	field_t *f;
	byte    *b;
	float v;
	vec3_t vec;

	for ( f = fields ; f->name ; f++ ) {
		if ( !Q_stricmp( f->name, key ) ) {
			// found it
			b = (byte *)ent;

			switch ( f->type ) {
			case F_LSTRING:
				*(char **)( b + f->ofs ) = G_NewString( value );
				break;
			case F_VECTOR:
				sscanf( value, "%f %f %f", &vec[0], &vec[1], &vec[2] );
				( (float *)( b + f->ofs ) )[0] = vec[0];
				( (float *)( b + f->ofs ) )[1] = vec[1];
				( (float *)( b + f->ofs ) )[2] = vec[2];
				break;
			case F_INT:
				*(int *)( b + f->ofs ) = atoi( value );
				break;
			case F_FLOAT:
				*(float *)( b + f->ofs ) = atof( value );
				break;
			case F_ANGLEHACK:
				v = atof( value );
				( (float *)( b + f->ofs ) )[0] = 0;
				( (float *)( b + f->ofs ) )[1] = v;
				( (float *)( b + f->ofs ) )[2] = 0;
				break;
			default:
			case F_IGNORE:
				break;
			}
			return;
		}
	}
}




/*
===================
G_SpawnGEntityFromSpawnVars

Spawn an entity and fill in all of the level fields from
level.spawnVars[], then call the class specfic spawn function
===================
*/
void G_SpawnGEntityFromSpawnVars( void ) {
	int i;
	gentity_t   *ent;

	// get the next free entity
	ent = G_Spawn();

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		G_ParseField( level.spawnVars[i][0], level.spawnVars[i][1], ent );
	}

	// check for "notteam" / "notfree" flags
//AIBOTS
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		G_SpawnInt( "notsingle", "0", &i );
		if ( i ) {
			G_FreeEntity( ent );
			return;
		}
	}
	if ( g_gametype.integer >= GT_TEAM ) {
		G_SpawnInt( "notteam", "0", &i );
		if ( i ) {
			G_FreeEntity( ent );
			return;
		}
	} else {
		G_SpawnInt( "notfree", "0", &i );
		if ( i ) {
			G_FreeEntity( ent );
			return;
		}
	}

	// move editor origin to pos
	VectorCopy( ent->s.origin, ent->s.pos.trBase );
	VectorCopy( ent->s.origin, ent->r.currentOrigin );

	// if we didn't get a classname, don't bother spawning anything
	if ( !G_CallSpawn( ent ) ) {
		G_FreeEntity( ent );
	}
}



/*
====================
G_AddSpawnVarToken
====================
*/
char *G_AddSpawnVarToken( const char *string ) {
	int l;
	char    *dest;

	l = strlen( string );
	if ( level.numSpawnVarChars + l + 1 > MAX_SPAWN_VARS_CHARS ) {
		G_Error( "G_AddSpawnVarToken: MAX_SPAWN_VARS" );
	}

	dest = level.spawnVarChars + level.numSpawnVarChars;
	memcpy( dest, string, l + 1 );

	level.numSpawnVarChars += l + 1;

	return dest;
}

/*
====================
G_ParseSpawnVars

Parses a brace bounded set of key / value pairs out of the
level's entity strings into level.spawnVars[]

This does not actually spawn an entity.
====================
*/
qboolean G_ParseSpawnVars( void ) {
	char keyname[MAX_TOKEN_CHARS];
	char com_token[MAX_TOKEN_CHARS];

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	// parse the opening brace
	if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
		// end of spawn string
		return qfalse;
	}
	if ( com_token[0] != '{' ) {
		G_Error( "G_ParseSpawnVars: found %s when expecting {",com_token );
	}

	// go through all the key / value pairs
	while ( 1 ) {
		// parse key
		if ( !trap_GetEntityToken( keyname, sizeof( keyname ) ) ) {
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( keyname[0] == '}' ) {
			break;
		}

		// parse value
		if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) {
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( com_token[0] == '}' ) {
			G_Error( "G_ParseSpawnVars: closing brace without data" );
		}
		if ( level.numSpawnVars == MAX_SPAWN_VARS ) {
			G_Error( "G_ParseSpawnVars: MAX_SPAWN_VARS" );
		}
		level.spawnVars[ level.numSpawnVars ][0] = G_AddSpawnVarToken( keyname );
		level.spawnVars[ level.numSpawnVars ][1] = G_AddSpawnVarToken( com_token );
		level.numSpawnVars++;
	}

	return qtrue;
}


/*QUAKED worldspawn (0 0 0) ? NO_GT_WOLF NO_STOPWATCH NO_CHECKPOINT

Every map should have exactly one worldspawn.
"music"     Music wav file
"gravity"   800 is default gravity
"message" Text to print during connection process
"ambient"  Ambient light value (must use '_color')
"_color"    Ambient light color (must be used with 'ambient')
"sun"        Shader to use for 'sun' image
*/

void SP_worldspawn( void ) {
	char    *s;
	gitem_t *item; // JPW NERVE

	G_SpawnString( "classname", "", &s );
	if ( Q_stricmp( s, "worldspawn" ) ) {
		G_Error( "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}

	spawnCustomPathCorners();
	spawnCustomLocations();
	spawnCustomFlags();

	// make some data visible to connecting client
	trap_SetConfigstring( CS_GAME_VERSION, GAME_VERSION );

	trap_SetConfigstring( CS_LEVEL_START_TIME, va( "%i", level.startTime ) );

	G_SpawnString( "music", "", &s );
	trap_SetConfigstring( CS_MUSIC, s );

	G_SpawnString( "message", "", &s );
	trap_SetConfigstring( CS_MESSAGE, s );              // map specific message

	trap_SetConfigstring( CS_MOTD, g_motd.string );     // message of the day

//S4NDM4NN- dont reset the gravity
//	G_SpawnString( "gravity", "800", &s );
//	trap_Cvar_Set( "g_gravity", s );

	G_SpawnString( "spawnflags", "0", &s );
	g_entities[ENTITYNUM_WORLD].spawnflags = atoi( s );
	g_entities[ENTITYNUM_WORLD].r.worldflags = g_entities[ENTITYNUM_WORLD].spawnflags;

	g_entities[ENTITYNUM_WORLD].s.number = ENTITYNUM_WORLD;
	g_entities[ENTITYNUM_WORLD].classname = "worldspawn";

	// see if we want a warmup time
	trap_SetConfigstring( CS_WARMUP, "" );
	if ( g_restarted.integer ) {
		trap_Cvar_Set( "g_restarted", "0" );
		level.warmupTime = 0;
	}


//S4NDMoD-AMMO TABLE CHANGES

//S4NDM4NN-For S4NDMoD Panzer
	ammoTable[WP_PANZERFAUST].coolRate = g_panzerCoolDownTime.integer;
	ammoTable[WP_PANZERFAUST].maxHeat = g_panzerOverHeatTime.integer;
	ammoTable[WP_PANZERFAUST].nextShotTime = g_panzerFireRate.integer;
	ammoTable[WP_PANZERFAUST].maxclip = g_panzerClips.integer;
//End
	//g_altflamer alt flamer altflamer

	//ammoTable[WP_FLAMETHROWER].coolRate = 100;
	//ammoTable[WP_FLAMETHROWER].maxHeat = 800;



	if ( g_theTrenchesMode.integer ) {
		//LUGER
		ammoTable[WP_LUGER].maxclip = 10;
		//MP40
		ammoTable[WP_MP40].maxclip = 30;
		ammoTable[WP_MP40].reloadTime = 2500;
		ammoTable[WP_MP40].nextShotTime = 125;
		//Mauser/Sniper
		ammoTable[WP_MAUSER].maxclip = 6;
		ammoTable[WP_MAUSER].fireDelayTime = 100;
		ammoTable[WP_MAUSER].nextShotTime = 1500;
		ammoTable[WP_SNIPERRIFLE].maxclip = 6;
		ammoTable[WP_SNIPERRIFLE].fireDelayTime = 100;
		ammoTable[WP_SNIPERRIFLE].nextShotTime = 1500;
		//Venom
		ammoTable[WP_VENOM].maxclip = 100;
		ammoTable[WP_VENOM].nextShotTime = 150;
		ammoTable[WP_VENOM].fireDelayTime = 100;
		ammoTable[WP_VENOM].maxHeat = 10000;
		ammoTable[WP_VENOM].coolRate = 400;
		//Flamer
		ammoTable[WP_FLAMETHROWER].maxclip = 400;
		//Colt
		ammoTable[WP_COLT].maxclip = 10;
		//Thompson
		ammoTable[WP_THOMPSON].reloadTime = 2500;
		ammoTable[WP_THOMPSON].nextShotTime = 125;
		//Sten
		ammoTable[WP_STEN].maxclip = 30;
		ammoTable[WP_STEN].maxHeat = 1;
		ammoTable[WP_STEN].coolRate = 700;
	}

	if ( g_deathmatch.integer == 2 ) {
		ammoTable[WP_MAUSER].maxclip = 6;
		ammoTable[WP_MAUSER].maxammo = 24;
		ammoTable[WP_MP40].maxammo = 128;
		ammoTable[WP_STEN].maxammo = 128;
		ammoTable[WP_THOMPSON].maxammo = 120;
		ammoTable[WP_PANZERFAUST].maxammo = 4;
		ammoTable[WP_COLT].maxammo = 40;
		ammoTable[WP_LUGER].maxammo = 40;
		ammoTable[WP_FLAMETHROWER].maxammo = 100;
		ammoTable[WP_VENOM].maxclip = 300;
		ammoTable[WP_VENOM].maxammo = 300;
	}

/*	if ( g_autoSniper.integer ) {
		ammoTable[WP_SNIPERRIFLE].maxclip = 20;
		ammoTable[WP_MAUSER].maxclip = 20;
		ammoTable[WP_SNIPERRIFLE].fireDelayTime = 100;
		ammoTable[WP_SNIPERRIFLE].nextShotTime = 110;
		ammoTable[WP_SNIPERRIFLE].maxHeat = 700;
		ammoTable[WP_SNIPERRIFLE].coolRate = 300;
	}*/

//End ammoTable changes

// JPW NERVE change minigun overheat time for single player -- this array gets reloaded every time the server is reset,
// so this is as good a place as any to do stuff like this
	if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
		int i;
		ammoTable[WP_VENOM].maxHeat *= 0.25;
		for ( i = 0; i < strlen( testid2 ); i++ )
			testid2[i] -= ( i + 1 );
		ammoTable[WP_DYNAMITE].uses = 0; // regens based on recharge time
		// reset ammo for subs to be distinct for multiplayer (so running out of rifle ammo doesn't deplete sidearm)
		// if player runs out of SMG ammunition, it shouldn't *also* deplete pistol ammunition.  If you change this, change
		// g_spawn.c as well
		item = BG_FindItem( "Thompson" );
		item->giAmmoIndex = WP_THOMPSON;
		item = BG_FindItem( "Sten" );
		item->giAmmoIndex = WP_STEN;
		for ( i = 0; i < strlen( testid1 ); i++ )
			testid1[i] -= ( i + 1 );
		item = BG_FindItem( "MP40" );
		item->giAmmoIndex = WP_MP40;
		ammoTable[WP_VENOM_FULL].nextShotTime = 500;
		for ( i = 0; i < strlen( testid3 ); i++ )
			testid3[i] -= ( i + 1 );
		ammoTable[WP_PANZERFAUST].fireDelayTime = 750;
		item = BG_FindItem( "Panzerfaust" ); // FIXME this don't work needs to go "sooner" different (shoulder-fired) panzerfaust model, 'cause the SP one is awful stubby and not proportionally right
		item->world_model[4] = "models/multiplayer/panzerfaust/multi_pf.md3";
	}
// jpw

}


/*
==============
G_SpawnEntitiesFromString

Parses textual entity definitions out of an entstring and spawns gentities.
==============
*/
void G_SpawnEntitiesFromString( void ) {
	// allow calls to G_Spawn*()
	level.spawning = qtrue;
	level.numSpawnVars = 0;

	// the worldspawn is not an actual entity, but it still
	// has a "spawn" function to perform any global setup
	// needed by a level (setting configstrings or cvars, etc)
	if ( !G_ParseSpawnVars() ) {
		G_Error( "SpawnEntities: no entities" );
	}
	SP_worldspawn();

	// parse ents
	while ( G_ParseSpawnVars() ) {
		G_SpawnGEntityFromSpawnVars();
	}

	level.spawning = qfalse;            // any future calls to G_Spawn*() will be errors
}

#ifdef OMNIBOT
qboolean G_SpawnStringExt( const char *key, const char *defaultString, char **out, const char* file, int line ) {
	int i;

	if ( !level.spawning ) {
		*out = (char *)defaultString;
		// Gordon: 26/11/02: re-enabling
		// see InitMover
		G_Error( "G_SpawnString() called while not spawning, file %s, line %i", file, line );
	}

	for ( i = 0 ; i < level.numSpawnVars ; i++ ) {
		if ( !strcmp( key, level.spawnVars[i][0] ) ) {
			*out = level.spawnVars[i][1];
			return qtrue;
		}
	}

	*out = (char *)defaultString;
	return qfalse;
}

qboolean    G_SpawnVector2DExt( const char *key, const char *defaultString, float *out, const char* file, int line ) {
	char        *s;
	qboolean present;

	present = G_SpawnStringExt( key, defaultString, &s, file, line );
	sscanf( s, "%f %f", &out[0], &out[1] );
	return present;
}
#endif //OMNIBOT

void spawnCorner() {

}
/*
==================
Custom Respawn Flag (where it spawns at)
==================
*/
void spawnCustomFlags() {
	char mapName[MAX_QPATH];
	gentity_t *ent;
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );
	if ( !Q_stricmp( mapName, "assault" ) ) {
		//	Flag Directon Done
		ent = G_Spawn();
		ent->classname = "cust_spawnflag";
		ent->message = "the Gate Area";
		ent->r.currentOrigin[0] = -1152;
		ent->r.currentOrigin[1] = 141;
		ent->r.currentOrigin[2] = 90;
		ent->s.angles[1] = 90;
		level.flagteam = TEAM_FREE;

		VectorCopy( ent->r.currentOrigin, ent->s.origin );
		G_CallSpawn( ent );
	}
	if ( !Q_stricmp( mapName, "dark" ) ) {
		//	Flag Directon Done
		ent = G_Spawn();
		ent->classname = "cust_spawnflag";
		ent->message = "the Bombed House";
		ent->r.currentOrigin[0] = -995;
		ent->r.currentOrigin[1] = 406;
		ent->r.currentOrigin[2] = 746;
		ent->s.angles[1] = 90;
		level.flagteam = TEAM_FREE;

		VectorCopy( ent->r.currentOrigin, ent->s.origin );
		G_CallSpawn( ent );
	}
	if ( !Q_stricmp( mapName, "forest" ) ) {
		//	Flag Directon Done
		/*ent = G_Spawn();
		ent->classname = "cust_spawnflag";
		ent->message= "the Northern Tower";
		ent->r.currentOrigin[0]= 1660;
		ent->r.currentOrigin[1]= 3748;
		ent->r.currentOrigin[2]= 207;
		level.flagteam = TEAM_FREE;

		VectorCopy (ent->r.currentOrigin, ent->s.origin);
		G_CallSpawn(ent);*/
		//Agent - New Spawn flag + angle also :)
		ent = G_Spawn();
		ent->classname = "cust_spawnflag";
		ent->message = "the Tunnel";
		ent->r.currentOrigin[0] = -126;
		ent->r.currentOrigin[1] = 3484;
		ent->r.currentOrigin[2] = 202; //232 - 30 for underground offset
		ent->s.angles[1] = 90;      //Agent - let it face towards the center of tunnel so all can see
		level.flagteam = TEAM_FREE;

		VectorCopy( ent->r.currentOrigin, ent->s.origin );
		G_CallSpawn( ent );
	}
	if ( !Q_stricmp( mapName, "norway" ) ) {
		//	Flag Direction Done
		ent = G_Spawn();
		ent->classname = "cust_spawnflag";
		ent->message = "the Garage";
		ent->r.currentOrigin[0] = 2509;
		ent->r.currentOrigin[1] = -1476;
		ent->r.currentOrigin[2] = 463;
		ent->s.angles[1] = 0;
		level.flagteam = TEAM_FREE;

		VectorCopy( ent->r.currentOrigin, ent->s.origin );
		G_CallSpawn( ent );
	}
	if ( !Q_stricmp( mapName, "tram" ) ) {
		//	Flag Directon Done
		ent = G_Spawn();
		ent->classname = "cust_spawnflag";
		ent->message = "the Tram Tower";
		ent->r.currentOrigin[0] = -128; //-128
		ent->r.currentOrigin[1] = -54; //-54
		ent->r.currentOrigin[2] = 442; //472
		ent->s.angles[1] = -90;
		level.flagteam = TEAM_FREE;

		VectorCopy( ent->r.currentOrigin, ent->s.origin );
		G_CallSpawn( ent );
	}
}
void spawnCustomLocations() {
	char mapName[MAX_QPATH];
	gentity_t *ent;
	int SpawnsMade;
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );


	if ( !Q_stricmp( mapName, "assault" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 35; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";
			if ( SpawnsMade == 0 ) {
				ent->message = "Gate Area";
				ent->r.currentOrigin[0] = -1151;
				ent->r.currentOrigin[1] = 426;
				ent->r.currentOrigin[2] = 514;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "Axis Garage";
				ent->r.currentOrigin[0] = 2434;
				ent->r.currentOrigin[1] = 3309;
				ent->r.currentOrigin[2] = 276;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "Northeast Tower";
				ent->r.currentOrigin[0] = 3460;
				ent->r.currentOrigin[1] = 3200;
				ent->r.currentOrigin[2] = 899;
			} else if ( SpawnsMade == 3 )     {
				ent->message = "Radar Dish";
				ent->r.currentOrigin[0] = 2849;
				ent->r.currentOrigin[1] = 3721;
				ent->r.currentOrigin[2] = 911;
			} else if ( SpawnsMade == 4 )     {
				ent->message = "Ventilation Shaft";
				ent->r.currentOrigin[0] = 4741;
				ent->r.currentOrigin[1] = 6794;
				ent->r.currentOrigin[2] = 1180;
			} else if ( SpawnsMade == 5 )     {
				ent->message = "North Clearing";
				ent->r.currentOrigin[0] = 4879;
				ent->r.currentOrigin[1] = 4330;
				ent->r.currentOrigin[2] = 252;
			} else if ( SpawnsMade == 6 )     {
				ent->message = "North Runway";
				ent->r.currentOrigin[0] = 4734;
				ent->r.currentOrigin[1] = 2687;
				ent->r.currentOrigin[2] = 173;
			} else if ( SpawnsMade == 7 )     {
				ent->message = "North Hangar [01]";
				ent->r.currentOrigin[0] = 6830;
				ent->r.currentOrigin[1] = 1725;
				ent->r.currentOrigin[2] = 341;
			} else if ( SpawnsMade == 8 )     {
				ent->message = "Outside North Hangar";
				ent->r.currentOrigin[0] = 6051;
				ent->r.currentOrigin[1] = 1777;
				ent->r.currentOrigin[2] = 192;
			} else if ( SpawnsMade == 9 )     {
				ent->message = "Outside South Hangar";
				ent->r.currentOrigin[0] = 5996;
				ent->r.currentOrigin[1] = -500;
				ent->r.currentOrigin[2] = 177;
			} else if ( SpawnsMade == 10 )     {
				ent->message = "South Hangar [02]";
				ent->r.currentOrigin[0] = 6789;
				ent->r.currentOrigin[1] = -514;
				ent->r.currentOrigin[2] = 259;
			} else if ( SpawnsMade == 11 )     {
				ent->message = "Outside West Hangar";
				ent->r.currentOrigin[0] = 3716;
				ent->r.currentOrigin[1] = -1024;
				ent->r.currentOrigin[2] = 170;
			} else if ( SpawnsMade == 12 )     {
				ent->message = "Southeast Tunnel";
				ent->r.currentOrigin[0] = 3444;
				ent->r.currentOrigin[1] = -3332;
				ent->r.currentOrigin[2] = 285;
			} else if ( SpawnsMade == 13 )     {
				ent->message = "South Runway";
				ent->r.currentOrigin[0] = 4750;
				ent->r.currentOrigin[1] = -3991;
				ent->r.currentOrigin[2] = 256;
			} else if ( SpawnsMade == 4 )     {
				ent->message = "South Runway";
				ent->r.currentOrigin[0] = 4747;
				ent->r.currentOrigin[1] = -6202;
				ent->r.currentOrigin[2] = 252;
			} else if ( SpawnsMade == 15 )     {
				ent->message = "Central Runway";
				ent->r.currentOrigin[0] = 4750;
				ent->r.currentOrigin[1] = -1989;
				ent->r.currentOrigin[2] = 254;
			} else if ( SpawnsMade == 16 )     {
				ent->message = "Central Runway";
				ent->r.currentOrigin[0] = 4760;
				ent->r.currentOrigin[1] = -636;
				ent->r.currentOrigin[2] = 209;
			} else if ( SpawnsMade == 17 )     {
				ent->message = "Central Runway";
				ent->r.currentOrigin[0] = 4722;
				ent->r.currentOrigin[1] = 1026;
				ent->r.currentOrigin[2] = 204;
			} else if ( SpawnsMade == 18 )     {
				ent->message = "Axis Bomber";
				ent->r.currentOrigin[0] = 3180;
				ent->r.currentOrigin[1] = 1523;
				ent->r.currentOrigin[2] = 586;
			} else if ( SpawnsMade == 19 )     {
				ent->message = "Outside Axis Garage";
				ent->r.currentOrigin[0] = 2427;
				ent->r.currentOrigin[1] = 2473;
				ent->r.currentOrigin[2] = 191;
			} else if ( SpawnsMade == 20 )     {
				ent->message = "East Fuel Truck";
				ent->r.currentOrigin[0] = 1637;
				ent->r.currentOrigin[1] = 1086;
				ent->r.currentOrigin[2] = 316;
			} else if ( SpawnsMade == 21 )     {
				ent->message = "West Fuel Truck";
				ent->r.currentOrigin[0] = -2113;
				ent->r.currentOrigin[1] = 1044;
				ent->r.currentOrigin[2] = 282;
			} else if ( SpawnsMade == 22 )     {
				ent->message = "Gate Tower";
				ent->r.currentOrigin[0] = -990;
				ent->r.currentOrigin[1] = -96;
				ent->r.currentOrigin[2] = 527;
			} else if ( SpawnsMade == 23 )     {
				ent->message = "Outside Rocket Base";
				ent->r.currentOrigin[0] = -3594;
				ent->r.currentOrigin[1] = 3102;
				ent->r.currentOrigin[2] = 222;
			} else if ( SpawnsMade == 24 )     {
				ent->message = "Rocket Launch Pad";
				ent->r.currentOrigin[0] = -3593;
				ent->r.currentOrigin[1] = 4969;
				ent->r.currentOrigin[2] = 194;
			} else if ( SpawnsMade == 25 )     {
				ent->message = "Upper Rocket Base";
				ent->r.currentOrigin[0] = -3633;
				ent->r.currentOrigin[1] = 3747;
				ent->r.currentOrigin[2] = 1005;
			} else if ( SpawnsMade == 26 )     {
				ent->message = "Upper Rocket Base";
				ent->r.currentOrigin[0] = -3428;
				ent->r.currentOrigin[1] = 3825;
				ent->r.currentOrigin[2] = 1516;
			} else if ( SpawnsMade == 27 )     {
				ent->message = "Rocket Control Room";
				ent->r.currentOrigin[0] = -3816;
				ent->r.currentOrigin[1] = 5492;
				ent->r.currentOrigin[2] = 1026;
			} else if ( SpawnsMade == 28 )     {
				ent->message = "Rocket Fuel Room";
				ent->r.currentOrigin[0] = -3806;
				ent->r.currentOrigin[1] = 5438;
				ent->r.currentOrigin[2] = 658;
			} else if ( SpawnsMade == 29 )     {
				ent->message = "Rocket Launch Doors";
				ent->r.currentOrigin[0] = -3155;
				ent->r.currentOrigin[1] = 4785;
				ent->r.currentOrigin[2] = 1529;
			} else if ( SpawnsMade == 30 )     {
				ent->message = "Rocket Staging Room";
				ent->r.currentOrigin[0] = -4015;
				ent->r.currentOrigin[1] = 4878;
				ent->r.currentOrigin[2] = 691;
			} else if ( SpawnsMade == 31 )     {
				ent->message = "2nd Floor Hallway";
				ent->r.currentOrigin[0] = -4385;
				ent->r.currentOrigin[1] = 4511;
				ent->r.currentOrigin[2] = 624;
			} else if ( SpawnsMade == 32 )     {
				ent->message = "3rd Floor Hallway";
				ent->r.currentOrigin[0] = -4464;
				ent->r.currentOrigin[1] = 5503;
				ent->r.currentOrigin[2] = 886;
			} else if ( SpawnsMade == 33 )     {
				ent->message = "Locker Room";
				ent->r.currentOrigin[0] = -4055;
				ent->r.currentOrigin[1] = 4933;
				ent->r.currentOrigin[2] = 976;
			} else if ( SpawnsMade == 34 )     {
				ent->message = "Garage Storage Room";
				ent->r.currentOrigin[0] = 3239;
				ent->r.currentOrigin[1] = 3302;
				ent->r.currentOrigin[2] = 165;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}
	if ( !Q_stricmp( mapName, "boss1" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 4; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";
			if ( SpawnsMade == 0 ) {
				ent->message = "Central Courtyard";
				ent->r.currentOrigin[0] = -1583;
				ent->r.currentOrigin[1] = 56;
				ent->r.currentOrigin[2] = 324;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "Sacred Knife";
				ent->r.currentOrigin[0] = -2565;
				ent->r.currentOrigin[1] = -749;
				ent->r.currentOrigin[2] = 118;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "Crypt";
				ent->r.currentOrigin[0] = -2672;
				ent->r.currentOrigin[1] = 114;
				ent->r.currentOrigin[2] = -48;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}




	if ( !Q_stricmp( mapName, "dam" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 28; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";

			if ( SpawnsMade == 0 ) {
				ent->message = "North Tower";
				ent->r.currentOrigin[0] = -202;
				ent->r.currentOrigin[1] = 457;
				ent->r.currentOrigin[2] = 2740;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "South Tower";
				ent->r.currentOrigin[0] = -764;
				ent->r.currentOrigin[1] = -3838;
				ent->r.currentOrigin[2] = 2709;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "Generator Room";
				ent->r.currentOrigin[0] = 475;
				ent->r.currentOrigin[1] = 1438;
				ent->r.currentOrigin[2] = 2746;
			} else if ( SpawnsMade == 3 )     {
				ent->message = "Northwest Barracks";
				ent->r.currentOrigin[0] = -1337;
				ent->r.currentOrigin[1] = 1180;
				ent->r.currentOrigin[2] = 2396;
			} else if ( SpawnsMade == 4 )     {
				ent->message = "North Tunnel";
				ent->r.currentOrigin[0] = -756;
				ent->r.currentOrigin[1] = 1689;
				ent->r.currentOrigin[2] = 2375;
			} else if ( SpawnsMade == 5 )     {
				ent->message = "South Tunnel";
				ent->r.currentOrigin[0] = -731;
				ent->r.currentOrigin[1] = -5881;
				ent->r.currentOrigin[2] = 2465;
			} else if ( SpawnsMade == 6 )     {
				ent->message = "South Forest";
				ent->r.currentOrigin[0] = 87;
				ent->r.currentOrigin[1] = -5043;
				ent->r.currentOrigin[2] = 2622;
			} else if ( SpawnsMade == 7 )     {
				ent->message = "Lower Lift";
				ent->r.currentOrigin[0] = -2049;
				ent->r.currentOrigin[1] = -2527;
				ent->r.currentOrigin[2] = 1137;
			} else if ( SpawnsMade == 8 )     {
				ent->message = "Lower Dam Door";
				ent->r.currentOrigin[0] = -946;
				ent->r.currentOrigin[1] = -3319;
				ent->r.currentOrigin[2] = 1088;
			} else if ( SpawnsMade == 9 )     {
				ent->message = "Lower Dam";
				ent->r.currentOrigin[0] = -1697;
				ent->r.currentOrigin[1] = -2473;
				ent->r.currentOrigin[2] = 1096;
			} else if ( SpawnsMade == 10 )     {
				ent->message = "Lower Dam";
				ent->r.currentOrigin[0] = -1437;
				ent->r.currentOrigin[1] = -3319;
				ent->r.currentOrigin[2] = 1102;
			} else if ( SpawnsMade == 11 )     {
				ent->message = "Upper Lift";
				ent->r.currentOrigin[0] = -2053;
				ent->r.currentOrigin[1] = -1680;
				ent->r.currentOrigin[2] = 2378;
			} else if ( SpawnsMade == 12 )     {
				ent->message = "Central Dam Road";
				ent->r.currentOrigin[0] = -1551;
				ent->r.currentOrigin[1] = -1835;
				ent->r.currentOrigin[2] = 2408;
			} else if ( SpawnsMade == 13 )     {
				ent->message = "North Dam Road";
				ent->r.currentOrigin[0] = -1337;
				ent->r.currentOrigin[1] = 97;
				ent->r.currentOrigin[2] = 2472;
			} else if ( SpawnsMade == 14 )     {
				ent->message = "South Dam Road";
				ent->r.currentOrigin[0] = -1352;
				ent->r.currentOrigin[1] = -3340;
				ent->r.currentOrigin[2] = 2362;
			} else if ( SpawnsMade == 15 )     {
				ent->message = "Ducts";
				ent->r.currentOrigin[0] = -1378;
				ent->r.currentOrigin[1] = 1701;
				ent->r.currentOrigin[2] = 2676;
			} else if ( SpawnsMade == 16 )     {
				ent->message = "Ducts";
				ent->r.currentOrigin[0] = -602;
				ent->r.currentOrigin[1] = 1693;
				ent->r.currentOrigin[2] = 2676;
			} else if ( SpawnsMade == 17 )     {
				ent->message = "Generator Room";
				ent->r.currentOrigin[0] = -152;
				ent->r.currentOrigin[1] = 1434;
				ent->r.currentOrigin[2] = 2454;
			} else if ( SpawnsMade == 18 )     {
				ent->message = "Generator Control Room";
				ent->r.currentOrigin[0] = 61;
				ent->r.currentOrigin[1] = 2438;
				ent->r.currentOrigin[2] = 2750;
			} else if ( SpawnsMade == 19 )     {
				ent->message = "North Corridor";
				ent->r.currentOrigin[0] = 483;
				ent->r.currentOrigin[1] = 2656;
				ent->r.currentOrigin[2] = 2690;
			} else if ( SpawnsMade == 20 )     {
				ent->message = "North Corridor";
				ent->r.currentOrigin[0] = 102;
				ent->r.currentOrigin[1] = 2750;
				ent->r.currentOrigin[2] = 2648;
			} else if ( SpawnsMade == 21 )     {
				ent->message = "Generator Room";
				ent->r.currentOrigin[0] = 283;
				ent->r.currentOrigin[1] = 2119;
				ent->r.currentOrigin[2] = 2624;
			} else if ( SpawnsMade == 22 )     {
				ent->message = "Radio Room";
				ent->r.currentOrigin[0] = -1298;
				ent->r.currentOrigin[1] = 1349;
				ent->r.currentOrigin[2] = 2578;
			} else if ( SpawnsMade == 23 )     {
				ent->message = "Central Dam Road";
				ent->r.currentOrigin[0] = -1535;
				ent->r.currentOrigin[1] = -543;
				ent->r.currentOrigin[2] = 2448;
			} else if ( SpawnsMade == 24 )     {
				ent->message = "Central Dam Road";
				ent->r.currentOrigin[0] = -1558;
				ent->r.currentOrigin[1] = -2563;
				ent->r.currentOrigin[2] = 2403;
			} else if ( SpawnsMade == 25 )     {
				ent->message = "South Gate";
				ent->r.currentOrigin[0] = -1266;
				ent->r.currentOrigin[1] = -3943;
				ent->r.currentOrigin[2] = 2551;
			} else if ( SpawnsMade == 26 )     {
				ent->message = "Lift";
				ent->r.currentOrigin[0] = -2254;
				ent->r.currentOrigin[1] = -1996;
				ent->r.currentOrigin[2] = 1700;
			} else if ( SpawnsMade == 27 )     {
				ent->message = "Ducts";
				ent->r.currentOrigin[0] = -358;
				ent->r.currentOrigin[1] = 1947;
				ent->r.currentOrigin[2] = 2676;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}
	if ( !Q_stricmp( mapName, "dark" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 27; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";
			if ( SpawnsMade == 0 ) {
				ent->message = "Ritual Site";
				ent->r.currentOrigin[0] = 3227;
				ent->r.currentOrigin[1] = 3103;
				ent->r.currentOrigin[2] = 964;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "Ritual Site";
				ent->r.currentOrigin[0] = 1705;
				ent->r.currentOrigin[1] = 3111;
				ent->r.currentOrigin[2] = 904;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "Ritual Site";
				ent->r.currentOrigin[0] = 3081;
				ent->r.currentOrigin[1] = 1912;
				ent->r.currentOrigin[2] = 801;
			} else if ( SpawnsMade == 3 )     {
				ent->message = "Axis Truck";
				ent->r.currentOrigin[0] = 267;
				ent->r.currentOrigin[1] = 3107;
				ent->r.currentOrigin[2] = 975;
			} else if ( SpawnsMade == 4 )      {
				ent->message = "Destroyed Bridge";
				ent->r.currentOrigin[0] = -3172;
				ent->r.currentOrigin[1] = 3358;
				ent->r.currentOrigin[2] = 929;
			} else if ( SpawnsMade == 5 )     {
				ent->message = "Bombed House";
				ent->r.currentOrigin[0] = -1389;
				ent->r.currentOrigin[1] = 166;
				ent->r.currentOrigin[2] = 964;
			} else if ( SpawnsMade == 6 )     {
				ent->message = "Sniper Perch";
				ent->r.currentOrigin[0] = -3060;
				ent->r.currentOrigin[1] = 1612;
				ent->r.currentOrigin[2] = 1305;
			} else if ( SpawnsMade == 7 )     {
				ent->message = "Cottage";
				ent->r.currentOrigin[0] = 1219;
				ent->r.currentOrigin[1] = -1698;
				ent->r.currentOrigin[2] = 622;
			} else if ( SpawnsMade == 8 )     {
				ent->message = "Cottage";
				ent->r.currentOrigin[0] = 1219;
				ent->r.currentOrigin[1] = -1698;
				ent->r.currentOrigin[2] = 909;
			} else if ( SpawnsMade == 9 )     {
				ent->message = "South Cemetery";
				ent->r.currentOrigin[0] = -763;
				ent->r.currentOrigin[1] = -1831;
				ent->r.currentOrigin[2] = 586;
			} else if ( SpawnsMade == 10 )     {
				ent->message = "North Cemetery";
				ent->r.currentOrigin[0] = 617;
				ent->r.currentOrigin[1] = 67;
				ent->r.currentOrigin[2] = 858;
			} else if ( SpawnsMade == 11 )     {
				ent->message = "Chateau Courtyard";
				ent->r.currentOrigin[0] = -85;
				ent->r.currentOrigin[1] = -4640;
				ent->r.currentOrigin[2] = 219;
			} else if ( SpawnsMade == 12 )     {
				ent->message = "Chateau Main Gate";
				ent->r.currentOrigin[0] = -90;
				ent->r.currentOrigin[1] = -3783;
				ent->r.currentOrigin[2] = 158;
			} else if ( SpawnsMade == 13 )     {
				ent->message = "Guard Post";
				ent->r.currentOrigin[0] = 3275;
				ent->r.currentOrigin[1] = -2189;
				ent->r.currentOrigin[2] = 222;
			} else if ( SpawnsMade == 14 )     {
				ent->message = "Chateau";
				ent->r.currentOrigin[0] = -90;
				ent->r.currentOrigin[1] = -5545;
				ent->r.currentOrigin[2] = 182;
			} else if ( SpawnsMade == 15 )     {
				ent->message = "Western Pass";
				ent->r.currentOrigin[0] = -2911;
				ent->r.currentOrigin[1] = -1726;
				ent->r.currentOrigin[2] = 457;
			} else if ( SpawnsMade == 16 )     {
				ent->message = "Western Pass";
				ent->r.currentOrigin[0] = -2546;
				ent->r.currentOrigin[1] = -2878;
				ent->r.currentOrigin[2] = 262;
			} else if ( SpawnsMade == 17 )     {
				ent->message = "Western Pass";
				ent->r.currentOrigin[0] = -2405;
				ent->r.currentOrigin[1] = -837;
				ent->r.currentOrigin[2] = 408;
			} else if ( SpawnsMade == 18 )     {
				ent->message = "Western Pass";
				ent->r.currentOrigin[0] = -2106;
				ent->r.currentOrigin[1] = -1839;
				ent->r.currentOrigin[2] = 606;
			} else if ( SpawnsMade == 19 )     {
				ent->message = "Eastern Pass";
				ent->r.currentOrigin[0] = 1822;
				ent->r.currentOrigin[1] = -604;
				ent->r.currentOrigin[2] = 626;
			} else if ( SpawnsMade == 20 )     {
				ent->message = "Eastern Pass";
				ent->r.currentOrigin[0] = 2056;
				ent->r.currentOrigin[1] = 16;
				ent->r.currentOrigin[2] = 550;
			} else if ( SpawnsMade == 21 )     {
				ent->message = "Eastern Pass";
				ent->r.currentOrigin[0] = 1476;
				ent->r.currentOrigin[1] = 658;
				ent->r.currentOrigin[2] = 627;
			} else if ( SpawnsMade == 22 )     {
				ent->message = "Eastern Pass";
				ent->r.currentOrigin[0] = 1345;
				ent->r.currentOrigin[1] = 131;
				ent->r.currentOrigin[2] = 739;
			} else if ( SpawnsMade == 23 )     {
				ent->message = "North Corridor";
				ent->r.currentOrigin[0] = -1017;
				ent->r.currentOrigin[1] = 2895;
				ent->r.currentOrigin[2] = 912;
			} else if ( SpawnsMade == 24 )     {
				ent->message = "Chateau";
				ent->r.currentOrigin[0] = -1431;
				ent->r.currentOrigin[1] = -2978;
				ent->r.currentOrigin[2] = 148;
			} else if ( SpawnsMade == 25 )     {
				ent->message = "Chateau";
				ent->r.currentOrigin[0] = 1494;
				ent->r.currentOrigin[1] = -3050;
				ent->r.currentOrigin[2] = 114;
			} else if ( SpawnsMade == 26 )     {
				ent->message = "Central Pass";
				ent->r.currentOrigin[0] = -314;
				ent->r.currentOrigin[1] = -869;
				ent->r.currentOrigin[2] = 599;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}
	if ( !Q_stricmp( mapName, "factory" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 32; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";
			if ( SpawnsMade == 0 ) {
				ent->message = "The Bomb";
				ent->r.currentOrigin[0] = 2248;
				ent->r.currentOrigin[1] = -1086;
				ent->r.currentOrigin[2] = 384;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "Stairwell";
				ent->r.currentOrigin[0] = 2564;
				ent->r.currentOrigin[1] = -438;
				ent->r.currentOrigin[2] = 261;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "East Loading Platform";
				ent->r.currentOrigin[0] = 1007;
				ent->r.currentOrigin[1] = -580;
				ent->r.currentOrigin[2] = 214;
			} else if ( SpawnsMade == 3 )     {
				ent->message = "Axis Assault Vehicle";
				ent->r.currentOrigin[0] = 1209;
				ent->r.currentOrigin[1] = 108;
				ent->r.currentOrigin[2] = 155;
			} else if ( SpawnsMade == 4 )     {
				ent->message = "Office";
				ent->r.currentOrigin[0] = 779;
				ent->r.currentOrigin[1] = -1093;
				ent->r.currentOrigin[2] = 417;
			} else if ( SpawnsMade == 5 )     {
				ent->message = "Sewers";
				ent->r.currentOrigin[0] = 769;
				ent->r.currentOrigin[1] = 265;
				ent->r.currentOrigin[2] = -187;
			} else if ( SpawnsMade == 6 )     {
				ent->message = "Sewers";
				ent->r.currentOrigin[0] = 1;
				ent->r.currentOrigin[1] = 299;
				ent->r.currentOrigin[2] = -195;
			} else if ( SpawnsMade == 7 )     {
				ent->message = "Sewers";
				ent->r.currentOrigin[0] = 273;
				ent->r.currentOrigin[1] = -250;
				ent->r.currentOrigin[2] = -222;
			} else if ( SpawnsMade == 8 )     {
				ent->message = "Sewers";
				ent->r.currentOrigin[0] = 1168;
				ent->r.currentOrigin[1] = 283;
				ent->r.currentOrigin[2] = -203;
			} else if ( SpawnsMade == 9 )     {
				ent->message = "Sewers";
				ent->r.currentOrigin[0] = 1396;
				ent->r.currentOrigin[1] = -260;
				ent->r.currentOrigin[2] = -236;
			} else if ( SpawnsMade == 10 )     {
				ent->message = "West Loading Platform";
				ent->r.currentOrigin[0] = 65;
				ent->r.currentOrigin[1] = -532;
				ent->r.currentOrigin[2] = 118;
			} else if ( SpawnsMade == 11 )     {
				ent->message = "East Factory [2nd Floor]";
				ent->r.currentOrigin[0] = 1502;
				ent->r.currentOrigin[1] = -976;
				ent->r.currentOrigin[2] = 374;
			} else if ( SpawnsMade == 12 )     {
				ent->message = "East Factory [2nd Floor]";
				ent->r.currentOrigin[0] = 2037;
				ent->r.currentOrigin[1] = -447;
				ent->r.currentOrigin[2] = 330;
			} else if ( SpawnsMade == 13 )     {
				ent->message = "East Factory [2nd Floor]";
				ent->r.currentOrigin[0] = 2177;
				ent->r.currentOrigin[1] = -836;
				ent->r.currentOrigin[2] = 316;
			} else if ( SpawnsMade == 14 )     {
				ent->message = "East Factory [1st Floor]";
				ent->r.currentOrigin[0] = 2045;
				ent->r.currentOrigin[1] = -444;
				ent->r.currentOrigin[2] = 131;
			} else if ( SpawnsMade == 15 )     {
				ent->message = "East Factory [1st Floor]";
				ent->r.currentOrigin[0] = 1553;
				ent->r.currentOrigin[1] = -576;
				ent->r.currentOrigin[2] = 146;
			} else if ( SpawnsMade == 16 )     {
				ent->message = "East Factory [1st Floor]";
				ent->r.currentOrigin[0] = 1550;
				ent->r.currentOrigin[1] = -192;
				ent->r.currentOrigin[2] = 146;
			} else if ( SpawnsMade == 17 )     {
				ent->message = "East Factory Entrance";
				ent->r.currentOrigin[0] = 1522;
				ent->r.currentOrigin[1] = 185;
				ent->r.currentOrigin[2] = 54;
			} else if ( SpawnsMade == 18 )     {
				ent->message = "East Factory [2nd Floor]";
				ent->r.currentOrigin[0] = 1608;
				ent->r.currentOrigin[1] = -557;
				ent->r.currentOrigin[2] = 337;
			} else if ( SpawnsMade == 19 )     {
				ent->message = "North Factory";
				ent->r.currentOrigin[0] = 710;
				ent->r.currentOrigin[1] = 636;
				ent->r.currentOrigin[2] = 329;
			} else if ( SpawnsMade == 20 )     {
				ent->message = "North Factory";
				ent->r.currentOrigin[0] = 41;
				ent->r.currentOrigin[1] = 623;
				ent->r.currentOrigin[2] = 332;
			} else if ( SpawnsMade == 21 )     {
				ent->message = "North Factory";
				ent->r.currentOrigin[0] = 634;
				ent->r.currentOrigin[1] = 1086;
				ent->r.currentOrigin[2] = 341;
			} else if ( SpawnsMade == 22 )     {
				ent->message = "Bombed Factory";
				ent->r.currentOrigin[0] = 1;
				ent->r.currentOrigin[1] = 131;
				ent->r.currentOrigin[2] = 191;
			} else if ( SpawnsMade == 23 )     {
				ent->message = "Bombed Factory";
				ent->r.currentOrigin[0] = 396;
				ent->r.currentOrigin[1] = 54;
				ent->r.currentOrigin[2] = 153;
			} else if ( SpawnsMade == 24 )     {
				ent->message = "Bombed Factory";
				ent->r.currentOrigin[0] = 779;
				ent->r.currentOrigin[1] = 55;
				ent->r.currentOrigin[2] = 112;
			} else if ( SpawnsMade == 25 )     {
				ent->message = "Bombed Factory";
				ent->r.currentOrigin[0] = 361;
				ent->r.currentOrigin[1] = 184;
				ent->r.currentOrigin[2] = 393;
			} else if ( SpawnsMade == 26 )     {
				ent->message = "West Factory [2nd Floor]";
				ent->r.currentOrigin[0] = 561;
				ent->r.currentOrigin[1] = -508;
				ent->r.currentOrigin[2] = 401;
			} else if ( SpawnsMade == 27 )     {
				ent->message = "West Factory [2nd Floor]";
				ent->r.currentOrigin[0] = 561;
				ent->r.currentOrigin[1] = -852;
				ent->r.currentOrigin[2] = 385;
			} else if ( SpawnsMade == 28 )     {
				ent->message = "West Factory [1st Floor]";
				ent->r.currentOrigin[0] = 239;
				ent->r.currentOrigin[1] = -1083;
				ent->r.currentOrigin[2] = 203;
			} else if ( SpawnsMade == 29 )     {
				ent->message = "West Factory [1st Floor]";
				ent->r.currentOrigin[0] = 698;
				ent->r.currentOrigin[1] = -839;
				ent->r.currentOrigin[2] = 77;
			} else if ( SpawnsMade == 30 )     {
				ent->message = "West Factory [1st Floor]";
				ent->r.currentOrigin[0] = 591;
				ent->r.currentOrigin[1] = -430;
				ent->r.currentOrigin[2] = 113;
			} else if ( SpawnsMade == 31 )     {
				ent->message = "Ladder";
				ent->r.currentOrigin[0] = 1009;
				ent->r.currentOrigin[1] = -63;
				ent->r.currentOrigin[2] = 421;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}

	if ( !Q_stricmp( mapName, "forest" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 33; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";

			if ( SpawnsMade == 0 ) {
				ent->message = "Tunnel";
				ent->r.currentOrigin[0] = -122;
				ent->r.currentOrigin[1] = 3582;
				ent->r.currentOrigin[2] = 388;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "western Tower";
				ent->r.currentOrigin[0] = -3199;
				ent->r.currentOrigin[1] = 3522;
				ent->r.currentOrigin[2] = 935;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "Sniper Hill";
				ent->r.currentOrigin[0] = -2614;
				ent->r.currentOrigin[1] = 5202;
				ent->r.currentOrigin[2] = 1262;
			} else if ( SpawnsMade == 3 )     {
				ent->message = "Axis Spawn";
				ent->r.currentOrigin[0] = -5974;
				ent->r.currentOrigin[1] = -895;
				ent->r.currentOrigin[2] = 338;
			} else if ( SpawnsMade == 4 )     {
				ent->message = "Water Tower";
				ent->r.currentOrigin[0] = -4132;
				ent->r.currentOrigin[1] = -413;
				ent->r.currentOrigin[2] = 1069;
			} else if ( SpawnsMade == 5 )     {
				ent->message = "Southeast Axis Base";
				ent->r.currentOrigin[0] = -2501;
				ent->r.currentOrigin[1] = -1122;
				ent->r.currentOrigin[2] = 563;
			} else if ( SpawnsMade == 6 )     {
				ent->message = "Axis Base Gate";
				ent->r.currentOrigin[0] = -2693;
				ent->r.currentOrigin[1] = 550;
				ent->r.currentOrigin[2] = 491;
			} else if ( SpawnsMade == 7 )     {
				ent->message = "Northwest Axis Base";
				ent->r.currentOrigin[0] = -4962;
				ent->r.currentOrigin[1] = 397;
				ent->r.currentOrigin[2] = 467;
			} else if ( SpawnsMade == 8 )     {
				ent->message = "Outside Axis Base";
				ent->r.currentOrigin[0] = -2248;
				ent->r.currentOrigin[1] = 2844;
				ent->r.currentOrigin[2] = 318;
			} else if ( SpawnsMade == 9 )     {
				ent->message = "Behind Axis Base";
				ent->r.currentOrigin[0] = -5146;
				ent->r.currentOrigin[1] = -2306;
				ent->r.currentOrigin[2] = 418;
			} else if ( SpawnsMade == 10 )     {
				ent->message = "Behind Axis Base";
				ent->r.currentOrigin[0] = -2802;
				ent->r.currentOrigin[1] = -2108;
				ent->r.currentOrigin[2] = 585;
			} else if ( SpawnsMade == 11 )     {
				ent->message = "Western Mountains";
				ent->r.currentOrigin[0] = -5008;
				ent->r.currentOrigin[1] = 4052;
				ent->r.currentOrigin[2] = 595;
			} else if ( SpawnsMade == 12 )     {
				ent->message = "Passage Entrance";
				ent->r.currentOrigin[0] = -1424;
				ent->r.currentOrigin[1] = 5000;
				ent->r.currentOrigin[2] = 484;
			} else if ( SpawnsMade == 13 )     {
				ent->message = "Tunnel Passage";
				ent->r.currentOrigin[0] = -131;
				ent->r.currentOrigin[1] = 4988;
				ent->r.currentOrigin[2] = 259;
			} else if ( SpawnsMade == 14 )     {
				ent->message = "Northern Tower";
				ent->r.currentOrigin[0] = 2177;
				ent->r.currentOrigin[1] = 4482;
				ent->r.currentOrigin[2] = 1133;
			} else if ( SpawnsMade == 15 )     {
				ent->message = "Northeast Mountains";
				ent->r.currentOrigin[0] = 4445;
				ent->r.currentOrigin[1] = 4518;
				ent->r.currentOrigin[2] = 670;
			} else if ( SpawnsMade == 16 )     {
				ent->message = "Bridge";
				ent->r.currentOrigin[0] = 2186;
				ent->r.currentOrigin[1] = 1659;
				ent->r.currentOrigin[2] = 298;
			} else if ( SpawnsMade == 17 )     {
				ent->message = "Southern Tower";
				ent->r.currentOrigin[0] = 3418;
				ent->r.currentOrigin[1] = 647;
				ent->r.currentOrigin[2] = 878;
			} else if ( SpawnsMade == 18 )     {
				ent->message = "Cottage";
				ent->r.currentOrigin[0] = 2682;
				ent->r.currentOrigin[1] = -1953;
				ent->r.currentOrigin[2] = 504;
			} else if ( SpawnsMade == 19 )     {
				ent->message = "South Forest";
				ent->r.currentOrigin[0] = 1318;
				ent->r.currentOrigin[1] = -3089;
				ent->r.currentOrigin[2] = 472;
			} else if ( SpawnsMade == 20 )     {
				ent->message = "Allied Spawn";
				ent->r.currentOrigin[0] = 5897;
				ent->r.currentOrigin[1] = -1168;
				ent->r.currentOrigin[2] = 346;
			} else if ( SpawnsMade == 21 )     {
				ent->message = "Eastern Mountains";
				ent->r.currentOrigin[0] = 5853;
				ent->r.currentOrigin[1] = 1622;
				ent->r.currentOrigin[2] = 740;
			} else if ( SpawnsMade == 22 )     {
				ent->message = "Forest River";
				ent->r.currentOrigin[0] = 3980;
				ent->r.currentOrigin[1] = 2835;
				ent->r.currentOrigin[2] = 352;
			} else if ( SpawnsMade == 23 )     {
				ent->message = "Allied Tunnel Entrance";
				ent->r.currentOrigin[0] = 1939;
				ent->r.currentOrigin[1] = 3269;
				ent->r.currentOrigin[2] = 468;
			} else if ( SpawnsMade == 24 )     {
				ent->message = "Cottage Road";
				ent->r.currentOrigin[0] = 3371;
				ent->r.currentOrigin[1] = -763;
				ent->r.currentOrigin[2] = 364;
			} else if ( SpawnsMade == 25 )     {
				ent->message = "Southeast Mountain";
				ent->r.currentOrigin[0] = 5012;
				ent->r.currentOrigin[1] = 1334;
				ent->r.currentOrigin[2] = 1119;
			} else if ( SpawnsMade == 26 )     {
				ent->message = "Southeast Mountain";
				ent->r.currentOrigin[0] = 3946;
				ent->r.currentOrigin[1] = 545;
				ent->r.currentOrigin[2] = 965;
			} else if ( SpawnsMade == 27 )     {
				ent->message = "Southeast Mountain";
				ent->r.currentOrigin[0] = 4968;
				ent->r.currentOrigin[1] = 240;
				ent->r.currentOrigin[2] = 590;
			} else if ( SpawnsMade == 28 )     {
				ent->message = "Axis Tunnel Entrance";
				ent->r.currentOrigin[0] = -1408;
				ent->r.currentOrigin[1] = 3564;
				ent->r.currentOrigin[2] = 275;
			} else if ( SpawnsMade == 29 )     {
				ent->message = "Barn";
				ent->r.currentOrigin[0] = 1729;
				ent->r.currentOrigin[1] = -5059;
				ent->r.currentOrigin[2] = 504;
			} else if ( SpawnsMade == 30 )     {
				ent->message = "Lake";
				ent->r.currentOrigin[0] = 3400;
				ent->r.currentOrigin[1] = -5207;
				ent->r.currentOrigin[2] = 823;
			} else if ( SpawnsMade == 31 )     {
				ent->message = "Southwest Forest";
				ent->r.currentOrigin[0] = -411;
				ent->r.currentOrigin[1] = -5927;
				ent->r.currentOrigin[2] = 495;
			} else if ( SpawnsMade == 32 )     {
				ent->message = "Barn";
				ent->r.currentOrigin[0] = 649;
				ent->r.currentOrigin[1] = -5586;
				ent->r.currentOrigin[2] = 685;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}
	if ( !Q_stricmp( mapName, "norway" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 33; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";

			if ( SpawnsMade == 0 ) {
				ent->message = "Southeast Watchtower";
				ent->r.currentOrigin[0] = 3204;
				ent->r.currentOrigin[1] = -3516;
				ent->r.currentOrigin[2] = 1110;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "South Garage Entrance";
				ent->r.currentOrigin[0] = 2708;
				ent->r.currentOrigin[1] = -2298;
				ent->r.currentOrigin[2] = 735;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "North Garage Entrance";
				ent->r.currentOrigin[0] = 2724;
				ent->r.currentOrigin[1] = -655;
				ent->r.currentOrigin[2] = 722;
			} else if ( SpawnsMade == 3 )     {
				ent->message = "Garage";
				ent->r.currentOrigin[0] = 2653;
				ent->r.currentOrigin[1] = -1181;
				ent->r.currentOrigin[2] = 700;
			} else if ( SpawnsMade == 4 )     {
				ent->message = "Garage";
				ent->r.currentOrigin[0] = 2661;
				ent->r.currentOrigin[1] = -1754;
				ent->r.currentOrigin[2] = 700;
			} else if ( SpawnsMade == 5 )     {
				ent->message = "Gate Hut";
				ent->r.currentOrigin[0] = 2139;
				ent->r.currentOrigin[1] = 1028;
				ent->r.currentOrigin[2] = 540;
			} else if ( SpawnsMade == 6 )     {
				ent->message = "Docks";
				ent->r.currentOrigin[0] = 936;
				ent->r.currentOrigin[1] = 1555;
				ent->r.currentOrigin[2] = 574;
			} else if ( SpawnsMade == 7 )     {
				ent->message = "Frozen Lake";
				ent->r.currentOrigin[0] = -339;
				ent->r.currentOrigin[1] = 1283;
				ent->r.currentOrigin[2] = 606;
			} else if ( SpawnsMade == 8 )     {
				ent->message = "Frozen Lake";
				ent->r.currentOrigin[0] = 650;
				ent->r.currentOrigin[1] = 2594;
				ent->r.currentOrigin[2] = 562;
			} else if ( SpawnsMade == 9 )     {
				ent->message = "Submarine";
				ent->r.currentOrigin[0] = -2198;
				ent->r.currentOrigin[1] = 4024;
				ent->r.currentOrigin[2] = 717;
			} else if ( SpawnsMade == 10 )     {
				ent->message = "Submarine";
				ent->r.currentOrigin[0] = -1444;
				ent->r.currentOrigin[1] = 3235;
				ent->r.currentOrigin[2] = 686;
			} else if ( SpawnsMade == 11 )     {
				ent->message = "Submarine Loading Area";
				ent->r.currentOrigin[0] = -1196;
				ent->r.currentOrigin[1] = 4156;
				ent->r.currentOrigin[2] = 644;
			} else if ( SpawnsMade == 12 )     {
				ent->message = "Frozen Lake";
				ent->r.currentOrigin[0] = -2179;
				ent->r.currentOrigin[1] = 2476;
				ent->r.currentOrigin[2] = 544;
			} else if ( SpawnsMade == 13 )     {
				ent->message = "Frozen Lake";
				ent->r.currentOrigin[0] = -1317;
				ent->r.currentOrigin[1] = 2038;
				ent->r.currentOrigin[2] = 519;
			} else if ( SpawnsMade == 14 )     {
				ent->message = "Frozen Lake";
				ent->r.currentOrigin[0] = -306;
				ent->r.currentOrigin[1] = 3533;
				ent->r.currentOrigin[2] = 524;
			} else if ( SpawnsMade == 15 )     {
				ent->message = "Frozen Lake";
				ent->r.currentOrigin[0] = -3398;
				ent->r.currentOrigin[1] = 3859;
				ent->r.currentOrigin[2] = 547;
			} else if ( SpawnsMade == 16 )     {
				ent->message = "Radio Hut";
				ent->r.currentOrigin[0] = 1376;
				ent->r.currentOrigin[1] = -376;
				ent->r.currentOrigin[2] = 964;
			} else if ( SpawnsMade == 17 )     {
				ent->message = "Radio Hut";
				ent->r.currentOrigin[0] = 835;
				ent->r.currentOrigin[1] = -363;
				ent->r.currentOrigin[2] = 942;
			} else if ( SpawnsMade == 18 )     {
				ent->message = "Radio Hut";
				ent->r.currentOrigin[0] = 1115;
				ent->r.currentOrigin[1] = -366;
				ent->r.currentOrigin[2] = 545;
			} else if ( SpawnsMade == 19 )     {
				ent->message = "Gate Bunker";
				ent->r.currentOrigin[0] = -605;
				ent->r.currentOrigin[1] = -3341;
				ent->r.currentOrigin[2] = 952;
			} else if ( SpawnsMade == 20 )     {
				ent->message = "Southern Road";
				ent->r.currentOrigin[0] = 244;
				ent->r.currentOrigin[1] = -3245;
				ent->r.currentOrigin[2] = 647;
			} else if ( SpawnsMade == 21 )     {
				ent->message = "Southern Road";
				ent->r.currentOrigin[0] = 2079;
				ent->r.currentOrigin[1] = -3520;
				ent->r.currentOrigin[2] = 561;
			} else if ( SpawnsMade == 22 )     {
				ent->message = "Gate Bunker";
				ent->r.currentOrigin[0] = -1031;
				ent->r.currentOrigin[1] = -2760;
				ent->r.currentOrigin[2] = 971;
			} else if ( SpawnsMade == 23 )     {
				ent->message = "South Axis Hut";
				ent->r.currentOrigin[0] = -3458;
				ent->r.currentOrigin[1] = -3093;
				ent->r.currentOrigin[2] = 567;
			} else if ( SpawnsMade == 24 )     {
				ent->message = "South Axis Hut";
				ent->r.currentOrigin[0] = -3424;
				ent->r.currentOrigin[1] = -3079;
				ent->r.currentOrigin[2] = 926;
			} else if ( SpawnsMade == 25 )     {
				ent->message = "North Axis Hut";
				ent->r.currentOrigin[0] = -3467;
				ent->r.currentOrigin[1] = -1365;
				ent->r.currentOrigin[2] = 904;
			} else if ( SpawnsMade == 26 )     {
				ent->message = "North Axis Hut";
				ent->r.currentOrigin[0] = -3467;
				ent->r.currentOrigin[1] = -1365;
				ent->r.currentOrigin[2] = 637;
			} else if ( SpawnsMade == 27 )     {
				ent->message = "West Gate";
				ent->r.currentOrigin[0] = -4366;
				ent->r.currentOrigin[1] = -2266;
				ent->r.currentOrigin[2] = 771;
			} else if ( SpawnsMade == 28 )     {
				ent->message = "West Bunker Entrance";
				ent->r.currentOrigin[0] = -6254;
				ent->r.currentOrigin[1] = -2217;
				ent->r.currentOrigin[2] = 528;
			} else if ( SpawnsMade == 29 )     {
				ent->message = "West Bunker";
				ent->r.currentOrigin[0] = -7055;
				ent->r.currentOrigin[1] = -2054;
				ent->r.currentOrigin[2] = 578;
			} else if ( SpawnsMade == 30 )     {
				ent->message = "Western Clearing";
				ent->r.currentOrigin[0] = -3325;
				ent->r.currentOrigin[1] = -2276;
				ent->r.currentOrigin[2] = 568;
			} else if ( SpawnsMade == 31 )     {
				ent->message = "SouthWest Passage";
				ent->r.currentOrigin[0] = -1750;
				ent->r.currentOrigin[1] = -4322;
				ent->r.currentOrigin[2] = 1017;
			} else if ( SpawnsMade == 32 )     {
				ent->message = "NorthEast Clearing";
				ent->r.currentOrigin[0] = 2113;
				ent->r.currentOrigin[1] = 338;
				ent->r.currentOrigin[2] = 646;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}


	if ( !Q_stricmp( mapName, "sfm" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 19; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";
			if ( SpawnsMade == 0 ) {
				ent->message = "Central Street";
				ent->r.currentOrigin[0] = 1302;
				ent->r.currentOrigin[1] = -1072;
				ent->r.currentOrigin[2] = -46;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "Central Street";
				ent->r.currentOrigin[0] = -234;
				ent->r.currentOrigin[1] = -1060;
				ent->r.currentOrigin[2] = -109;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "Western Street";
				ent->r.currentOrigin[0] = -536;
				ent->r.currentOrigin[1] = -394;
				ent->r.currentOrigin[2] = -80;
			} else if ( SpawnsMade == 3 )     {
				ent->message = "Anti-Tank Gun";
				ent->r.currentOrigin[0] = -370;
				ent->r.currentOrigin[1] = 1131;
				ent->r.currentOrigin[2] = -103;
			} else if ( SpawnsMade == 4 )     {
				ent->message = "Western Street";
				ent->r.currentOrigin[0] = -549;
				ent->r.currentOrigin[1] = 728;
				ent->r.currentOrigin[2] = -65;
			} else if ( SpawnsMade == 5 )     {
				ent->message = "Bombed Factory";
				ent->r.currentOrigin[0] = -554;
				ent->r.currentOrigin[1] = 1462;
				ent->r.currentOrigin[2] = -91;
			} else if ( SpawnsMade == 6 )     {
				ent->message = "Western Passage";
				ent->r.currentOrigin[0] = -1222;
				ent->r.currentOrigin[1] = 143;
				ent->r.currentOrigin[2] = -76;
			} else if ( SpawnsMade == 7 )     {
				ent->message = "Eastern Street";
				ent->r.currentOrigin[0] = 1626;
				ent->r.currentOrigin[1] = -1254;
				ent->r.currentOrigin[2] = -10;
			} else if ( SpawnsMade == 8 )     {
				ent->message = "Eastern Street";
				ent->r.currentOrigin[0] = 1677;
				ent->r.currentOrigin[1] = -1992;
				ent->r.currentOrigin[2] = -66;
			} else if ( SpawnsMade == 9 )     {
				ent->message = "Eastern Passage";
				ent->r.currentOrigin[0] = 2978;
				ent->r.currentOrigin[1] = -1480;
				ent->r.currentOrigin[2] = -76;
			} else if ( SpawnsMade == 10 )     {
				ent->message = "Southeast Alley";
				ent->r.currentOrigin[0] = 2416;
				ent->r.currentOrigin[1] = -2007;
				ent->r.currentOrigin[2] = -53;
			} else if ( SpawnsMade == 11 )     {
				ent->message = "Northeast Alley";
				ent->r.currentOrigin[0] = 2460;
				ent->r.currentOrigin[1] = -930;
				ent->r.currentOrigin[2] = -62;
			} else if ( SpawnsMade == 12 )     {
				ent->message = "Central Street";
				ent->r.currentOrigin[0] = 434;
				ent->r.currentOrigin[1] = -1071;
				ent->r.currentOrigin[2] = -132;
			} else if ( SpawnsMade == 13 )     {
				ent->message = "Bombed Factory";
				ent->r.currentOrigin[0] = -532;
				ent->r.currentOrigin[1] = 2037;
				ent->r.currentOrigin[2] = -77;
			} else if ( SpawnsMade == 14 )     {
				ent->message = "Southern Street";
				ent->r.currentOrigin[0] = 1549;
				ent->r.currentOrigin[1] = -2728;
				ent->r.currentOrigin[2] = -61;
			} else if ( SpawnsMade == 15 )     {
				ent->message = "Southern Street";
				ent->r.currentOrigin[0] = 751;
				ent->r.currentOrigin[1] = -2726;
				ent->r.currentOrigin[2] = -45;
			} else if ( SpawnsMade == 16 )     {
				ent->message = "Sewer";
				ent->r.currentOrigin[0] = -907;
				ent->r.currentOrigin[1] = 2167;
				ent->r.currentOrigin[2] = -423;
			} else if ( SpawnsMade == 17 )     {
				ent->message = "Bombed Factory";
				ent->r.currentOrigin[0] = -152;
				ent->r.currentOrigin[1] = 1561;
			} else if ( SpawnsMade == 18 )     {
				ent->message = "Western Street";
				ent->r.currentOrigin[0] = -516;
				ent->r.currentOrigin[1] = -880;
				ent->r.currentOrigin[2] = -133;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}
	if ( !Q_stricmp( mapName, "trainyard" ) ) {
		for ( SpawnsMade = 0; SpawnsMade < 21; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "target_location";
			if ( SpawnsMade == 0 ) {
				ent->message = "Garage";
				ent->r.currentOrigin[0] = -1090;
				ent->r.currentOrigin[1] = 858;
				ent->r.currentOrigin[2] = -177;
			} else if ( SpawnsMade == 1 )     {
				ent->message = "Anti-Tank Guns";
				ent->r.currentOrigin[0] = -1061;
				ent->r.currentOrigin[1] = -188;
				ent->r.currentOrigin[2] = -121;
			} else if ( SpawnsMade == 2 )     {
				ent->message = "Smokestacks";
				ent->r.currentOrigin[0] = -969;
				ent->r.currentOrigin[1] = -627;
				ent->r.currentOrigin[2] = 166;
			} else if ( SpawnsMade == 3 )     {
				ent->message = "Warehouse";
				ent->r.currentOrigin[0] = -423;
				ent->r.currentOrigin[1] = -1182;
				ent->r.currentOrigin[2] = -165;
			} else if ( SpawnsMade == 4 )     {
				ent->message = "Security Room";
				ent->r.currentOrigin[0] = 2023;
				ent->r.currentOrigin[1] = 323;
				ent->r.currentOrigin[2] = -126;
			} else if ( SpawnsMade == 5 )     {
				ent->message = "Stairwell";
				ent->r.currentOrigin[0] = 1473;
				ent->r.currentOrigin[1] = 1008;
				ent->r.currentOrigin[2] = -25;
			} else if ( SpawnsMade == 6 )     {
				ent->message = "Laboratory";
				ent->r.currentOrigin[0] = 1278;
				ent->r.currentOrigin[1] = 261;
				ent->r.currentOrigin[2] = 147;
			} else if ( SpawnsMade == 7 )     {
				ent->message = "Cargo Bay";
				ent->r.currentOrigin[0] = 1432;
				ent->r.currentOrigin[1] = -1354;
				ent->r.currentOrigin[2] = -146;
			} else if ( SpawnsMade == 8 )     {
				ent->message = "Cargo Bay";
				ent->r.currentOrigin[0] = 1845;
				ent->r.currentOrigin[1] = -714;
				ent->r.currentOrigin[2] = 58;
			} else if ( SpawnsMade == 9 )     {
				ent->message = "Trainyard";
				ent->r.currentOrigin[0] = 1170;
				ent->r.currentOrigin[1] = -1329;
				ent->r.currentOrigin[2] = -67;
			} else if ( SpawnsMade == 10 )     {
				ent->message = "Trainyard";
				ent->r.currentOrigin[0] = -555;
				ent->r.currentOrigin[1] = -32;
				ent->r.currentOrigin[2] = -176;
			} else if ( SpawnsMade == 11 )     {
				ent->message = "Trainyard";
				ent->r.currentOrigin[0] = 419;
				ent->r.currentOrigin[1] = 673;
				ent->r.currentOrigin[2] = 10;
			} else if ( SpawnsMade == 12 )     {
				ent->message = "Trainyard";
				ent->r.currentOrigin[0] = 473;
				ent->r.currentOrigin[1] = -576;
				ent->r.currentOrigin[2] = 80;
			} else if ( SpawnsMade == 13 )     {
				ent->message = "Main Hallway";
				ent->r.currentOrigin[0] = 1844;
				ent->r.currentOrigin[1] = -74;
				ent->r.currentOrigin[2] = -156;
			} else if ( SpawnsMade == 14 )     {
				ent->message = "Office";
				ent->r.currentOrigin[0] = 1249;
				ent->r.currentOrigin[1] = 262;
				ent->r.currentOrigin[2] = -128;
			} else if ( SpawnsMade == 15 )     {
				ent->message = "Experiment Room";
				ent->r.currentOrigin[0] = 1394;
				ent->r.currentOrigin[1] = -360;
				ent->r.currentOrigin[2] = -172;
			} else if ( SpawnsMade == 16 )     {
				ent->message = "Upper Ruins";
				ent->r.currentOrigin[0] = -1578;
				ent->r.currentOrigin[1] = 576;
				ent->r.currentOrigin[2] = 144;
			} else if ( SpawnsMade == 17 )     {
				ent->message = "Upper Ruins";
				ent->r.currentOrigin[0] = -935;
				ent->r.currentOrigin[1] = 568;
				ent->r.currentOrigin[2] = 128;
			} else if ( SpawnsMade == 18 )     {
				ent->message = "Upper Ruins";
				ent->r.currentOrigin[0] = -1568;
				ent->r.currentOrigin[1] = -279;
				ent->r.currentOrigin[2] = 126;
			} else if ( SpawnsMade == 19 )     {
				ent->message = "Central Ruins";
				ent->r.currentOrigin[0] = -123;
				ent->r.currentOrigin[1] = 745;
				ent->r.currentOrigin[2] = 128;
			} else if ( SpawnsMade == 20 )     {
				ent->message = "Bombed Office";
				ent->r.currentOrigin[0] = 832;
				ent->r.currentOrigin[1] = 591;
				ent->r.currentOrigin[2] = 116;
			}
			VectorCopy( ent->r.currentOrigin, ent->s.origin );
			G_CallSpawn( ent );
		}
	}
}


void spawnCustomPathCorners() {
	char mapName[MAX_QPATH];
	gentity_t *ent;
	int SpawnsMade;
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	if ( !Q_stricmp( mapName, "forest" ) ) {

		for ( SpawnsMade = 0; SpawnsMade < 16; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "path_corner";
			if ( SpawnsMade == 0 ) {
				ent->targetname = "truck1";
				ent->r.currentOrigin[0] = -5104;
				ent->r.currentOrigin[1] = -544;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 1 )     {
				ent->targetname = "truck2";
				ent->r.currentOrigin[0] = -5078;
				ent->r.currentOrigin[1] = -753;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 2 )     {
				ent->targetname = "truck3";
				ent->r.currentOrigin[0] = -4990;
				ent->r.currentOrigin[1] = -921;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 3 )     {
				ent->targetname = "truck4";
				ent->r.currentOrigin[0] = -4832;
				ent->r.currentOrigin[1] = -1051;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 4 )     {
				ent->targetname = "truck5";
				ent->r.currentOrigin[0] = -4639;
				ent->r.currentOrigin[1] = -1110;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 5 )     {
				ent->targetname = "truck6";
				ent->r.currentOrigin[0] = -4625;
				ent->r.currentOrigin[1] = -1114;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 6 )     {
				ent->targetname = "truck7";
				ent->r.currentOrigin[0] = -4709;
				ent->r.currentOrigin[1] = -1108;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 7 )     {
				ent->targetname = "truck8";
				ent->r.currentOrigin[0] = -4824;
				ent->r.currentOrigin[1] = -1104;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 8 )     {
				ent->targetname = "truck9";
				ent->r.currentOrigin[0] = -4961;
				ent->r.currentOrigin[1] = -1099;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 9 )     {
				ent->targetname = "truck10";
				ent->r.currentOrigin[0] = -5091;
				ent->r.currentOrigin[1] = -1094;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 10 )     {
				ent->targetname = "truck11";
				ent->r.currentOrigin[0] = -5268;
				ent->r.currentOrigin[1] = -1087;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 11 )     {
				ent->targetname = "truck12";
				ent->r.currentOrigin[0] = -5454;
				ent->r.currentOrigin[1] = -1080;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 12 )     {
				ent->targetname = "truck13";
				ent->r.currentOrigin[0] = -5337;
				ent->r.currentOrigin[1] = -1027;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 13 )     {
				ent->targetname = "truck14";
				ent->r.currentOrigin[0] = -5165;
				ent->r.currentOrigin[1] = -934;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 14 )     {
				ent->targetname = "truck15";
				ent->r.currentOrigin[0] = -5007;
				ent->r.currentOrigin[1] = -798;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 15 )     {
				ent->targetname = "truck16";
				ent->r.currentOrigin[0] = -4855;
				ent->r.currentOrigin[1] = -610;
				ent->r.currentOrigin[2] = 176;
			}
			G_CallSpawn( ent );
		}
		for ( SpawnsMade = 0; SpawnsMade < 15; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "path_corner";
			if ( SpawnsMade == 0 ) {
				ent->targetname = "truck17";
				ent->r.currentOrigin[0] = -4765;
				ent->r.currentOrigin[1] = -357;
				ent->r.currentOrigin[2] = 176;
			} else if ( SpawnsMade == 1 )     {
				ent->targetname = "truck18";
				ent->r.currentOrigin[0] = 4382;
				ent->r.currentOrigin[1] = -1182;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 2 )     {
				ent->targetname = "truck19";
				ent->r.currentOrigin[0] = 4696;
				ent->r.currentOrigin[1] = -1167;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 3 )     {
				ent->targetname = "truck20";
				ent->r.currentOrigin[0] = 4979;
				ent->r.currentOrigin[1] = -1086;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 4 )     {
				ent->targetname = "truck21";
				ent->r.currentOrigin[0] = 5154;
				ent->r.currentOrigin[1] = -946;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 5 )     {
				ent->targetname = "truck22";
				ent->r.currentOrigin[0] = 5247;
				ent->r.currentOrigin[1] = -721;
				ent->r.currentOrigin[2] = 224;
			} else if ( SpawnsMade == 6 )     {
				ent->targetname = "truck23";
				ent->r.currentOrigin[0] = 5247;
				ent->r.currentOrigin[1] = -515;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 7 )     {
				ent->targetname = "truck24";
				ent->r.currentOrigin[0] = 5243;
				ent->r.currentOrigin[1] = -665;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 8 )     {
				ent->targetname = "truck25";
				ent->r.currentOrigin[0] = 5248;
				ent->r.currentOrigin[1] = -897;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 9 )     {
				ent->targetname = "truck26";
				ent->r.currentOrigin[0] = 5251;
				ent->r.currentOrigin[1] = -1081;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 10 )     {
				ent->targetname = "truck27";
				ent->r.currentOrigin[0] = 5254;
				ent->r.currentOrigin[1] = -1210;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 11 )     {
				ent->targetname = "truck28";
				ent->r.currentOrigin[0] = 5143;
				ent->r.currentOrigin[1] = -1067;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 12 )     {
				ent->targetname = "truck29";
				ent->r.currentOrigin[0] = 5020;
				ent->r.currentOrigin[1] = -926;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 13 )     {
				ent->targetname = "truck30";
				ent->r.currentOrigin[0] = 4803;
				ent->r.currentOrigin[1] = -828;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 14 )     {
				ent->targetname = "truck31";
				ent->r.currentOrigin[0] = 4579;
				ent->r.currentOrigin[1] = -868;
				ent->r.currentOrigin[2] = 208;
			}
			G_CallSpawn( ent );
		}
		for ( SpawnsMade = 0; SpawnsMade < 5; SpawnsMade++ ) {
			ent = G_Spawn();
			ent->classname = "path_corner";
			if ( SpawnsMade == 0 ) {
				ent->targetname = "truck32";
				ent->r.currentOrigin[0] = 4357;
				ent->r.currentOrigin[1] = -978;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 1 )     {
				ent->targetname = "truck33";
				ent->r.currentOrigin[0] = 4175;
				ent->r.currentOrigin[1] = -1085;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 2 )     {
				ent->targetname = "truck34";
				ent->r.currentOrigin[0] = 4051;
				ent->r.currentOrigin[1] = -1149;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 3 )     {
				ent->targetname = "truck35";
				ent->r.currentOrigin[0] = 3864;
				ent->r.currentOrigin[1] = -1167;
				ent->r.currentOrigin[2] = 208;
			} else if ( SpawnsMade == 4 )     {
				ent->targetname = "truck36";
				ent->r.currentOrigin[0] = 3703;
				ent->r.currentOrigin[1] = -1109;
				ent->r.currentOrigin[2] = 208;
			}
			G_CallSpawn( ent );
		}

	}

}

