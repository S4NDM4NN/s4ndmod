#include "g_local.h"
#include "S4ND_local.h"

void ParseStr( const char *strInput, char *strCmd, char *strArgs ) {
	int i = 0, j = 0;
	int foundspace = 0;

	while ( strInput[i] != 0 ) {
		if ( !foundspace ) {
			if ( strInput[i] == ' ' ) {
				foundspace = 1;
				strCmd[i] = 0;
			} else {
				strCmd[i] = strInput[i];
			}
			i++;
		} else { // We have found a space
			strArgs[j++] = strInput[i++];
		}
	}
	if ( !foundspace ) {
		strCmd[i] = 0;
	}

	strArgs[j] = 0;
}

void Parseip( const char *InputIP, char *ip1, char *ip2,char *ip3,char *ip4 ) {
	int i, j = 0;
	int foundperiod = 0;
	//clear them all
	ip1 = "";
	ip2 = "";
	ip3 = "";
	ip4 = "";

	for ( i = 0; i < strlen( InputIP ); i++ ) {
		if ( !foundperiod ) { //we havent found a period
			if ( InputIP[i] != '.' ) {
				ip1[j] = InputIP[i];
				j++;
			} else { //found us a period
				j = 0;
				foundperiod++;
			}
		} else if ( foundperiod == 1 )     { //after period 1
			if ( InputIP[i] != '.' ) {
				ip2[j] = InputIP[i];
				j++;
			} else { //found us a period
				j = 0;
				foundperiod++;
			}
		} else if ( foundperiod == 2 )     { //after period 2
			if ( InputIP[i] != '.' ) {
				ip3[j] = InputIP[i];
				j++;
			} else { //found us a period
				j = 0;
				foundperiod++;
			}
		} else if ( foundperiod == 3 )     { //after period 3
			ip4[j] = InputIP[i];
			j++;

		}
	}
}

void BreakRange( const char *range, char *from, char *to ) {

	int i = 0, j = 0;
	int founddash = 0;
	for ( i = 0; i < strlen( range ); i++ )
	{
		if ( range[i] == '|' ) {
			to[j++] = 0;
			if ( !founddash ) {
				from = "0.0.0.0";
				to = "0.0.0.0";
			}
			break;
		}

		if ( !founddash ) {
			if ( range[i] == '-' ) {
				founddash++;
				from[j++] = 0;
				j = 0;
			} else {
				from[j++] = range[i];
			}
		} else {
			to[j++] = range[i];
		}
	}
}


/*
====================================================================
this is actually for the new ban that i think wont crash :)
but kev gave me this so i put here and ban = admin.... kinda right

Maybe kev should be put on the developers page?
====================================================================
*/

void GetAdmin( const char *strCMD1, char *strCMD2, char *strCMD3 ) {

	int i = 0, j = 0;
	int foundcolon = 0;

	while ( strCMD1[i] != 0 )
	{
		if ( !foundcolon ) {
			if ( strCMD1[i] == '!' ) {
				foundcolon = 1;
				strCMD2[i] = 0;
			} else {
				strCMD2[i] = strCMD1[i];
			}
			i++;
		} else
		{
			strCMD3[j++] = strCMD1[i++];
		}
	}
	if ( !foundcolon ) {
		strCMD2[i] = 0;
	}
	strCMD3[j] = 0;
}


void easy_admin( gentity_t *ent ) {
	char *token, *cmd;
	char uneeded[128];
	char Command[128];
	int nums[MAX_CLIENTS];
	char message[MAX_STRING_CHARS];
	int count = 0;
	int i;
	gentity_t *te; // L0 - for "warning" sound
	qboolean usedCmd = qfalse;
//	int tempswitchtime = g_teamSwitchTime.integer;

	//this just takes the '!' off the admin cmd
	GetAdmin( ent->client->pers.Command1, uneeded, Command );

	if ( ent->client->sess.admin ) {
		Admin_Log( "ADMIN CMD: %s(%s) sent command %s %s %s",ent->client->pers.netname,ent->client->sess.ip,ent->client->pers.Command1,ent->client->pers.Command2,ent->client->pers.Command3 );
	}

	//Admin 5
	if ( ent->client->sess.admin == 5 ) {
		if ( strlen( a5_cmds.string ) ) { //if the cvar is enabled (it's a string, so it's enabled if it has any values)
			cmd = a5_cmds.string; //copy the pointer to the string
			while ( a5_cmds.string != 0 )
			{
				token = COM_Parse( &cmd ); //token will contain a single word, progressing 1 word each loop
				if ( !token || !token[0] ) {
					break;
				}

				if ( !strcmp( Command,"cn" ) ) {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					}
					strcpy( message, va( "chat \"" ) );
					for ( i = 0; i < count; i++ )
						strcat( message, va( "%s: %s%i%s\n", g_entities[nums[i]].client->pers.netname, S_COLOR_YELLOW, nums[i], S_COLOR_WHITE ) );

					strcat( message, "\"" );

					trap_SendServerCommand( ent - g_entities, message );
					usedCmd = qtrue;
					return;
//SPEC - SENDS PLAYER(S) TO SPEC
				} else if ( ( !strcmp( Command,"spec" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					if ( !strcmp( "999", ent->client->pers.Command2 ) ) {
						trap_SendServerCommand( -1, va( "chat \"console: ^3Forced connecting players to spectators.\n\"" ) );
						trap_Cvar_Set( "g_lockteams", "0" );
						for ( i = 0; i < level.maxclients; i++ ) {
							if ( !g_entities[i].client ) { //if no one i here...dont try to move them
								continue;
							}
							if ( g_entities[nums[i]].client->sess.sessionTeam == TEAM_SPECTATOR ) {
								continue;
							}
							if ( g_entities[i].client->pers.connected == CON_CONNECTING ) {
								trap_SendConsoleCommand( EXEC_APPEND, va( "forceteam %i s", nums[i] ) );
							}
						}
						usedCmd = qtrue;
						//		trap_Cvar_Set( "g_teamSwitchTime", tempswitchtime);
						return;
					} else {
						count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
						if ( count == 0 ) {
							trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
							usedCmd = qtrue;
							return;
						} else if ( count > 1 )    {
							trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
							usedCmd = qtrue;
							return;
						}
						for ( i = 0; i < count; i++ ) {
							trap_SendConsoleCommand( EXEC_APPEND, va( "forceteam %i s", nums[i] ) );
							trap_SendServerCommand( -1, va( "chat \"console: %s ^3forced to spectators.\n\"", g_entities[nums[i]].client->pers.netname ) );
						}
						usedCmd = qtrue;
						return;
					}
				} else if ( ( !strcmp( Command,"kick" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3kicked by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
						trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i 0 %s", nums[i]+1, ent->client->pers.Command3 ) );						
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"tempban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3Temporarily banned by admin.\n\"", g_entities[nums[i]].client->pers.netname ) );
						trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kick %i %s", nums[i]+1, ent->client->pers.Command3 ) );						
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"ban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3banned by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
						trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_ban %i", g_entities[nums[i]].client->ps.clientNum+1 ) );						
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"shuffle" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin shuffled the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( ( !strcmp( Command,"ignore" ) ) || ( !strcmp( Command,"unignore" ) ) || ( !strcmp( Command,"slap" ) ) || ( !strcmp( Command,"kill" ) ) || ( !strcmp( Command,"forceteam" ) ) ) && ( !Q_stricmp( Command, token ) ) )                               {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "%s %i %s\n", Command, nums[i], ent->client->pers.Command3 ) );
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You used ^1%s ^3on ^1%s\n\"", Command, g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"g_password" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"map" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed the map.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"swap_teams" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin swapped the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"reset_match" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin reset the match.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"say" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cp" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} // L0
				else if ( ( !strcmp( Command,"pause" ) ) && ( !Q_stricmp( Command, token ) ) ) {
					Cmd_Pause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"unpause" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_UnPause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cancelvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Cancelvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"passvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Passvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"warn" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "print \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
					te->s.eventParm = G_SoundIndex( "sound/player/buzzwarn.wav" );
					te->r.svFlags |= SVF_BROADCAST;
					usedCmd = qtrue; // L0 - end
				} else if ( !Q_stricmp( Command, token ) )      {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s %s", Command, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed %s to %s\n\"", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
					return;
				}
			}
		}
		if ( usedCmd == qfalse ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You are not allowed to use ^1%s ^3as level five admin\n\"", Command ) );
		}
	}

	//Admin 4
	if ( ent->client->sess.admin == 4 ) {
		if ( strlen( a4_cmds.string ) ) { //if the cvar is enabled (it's a string, so it's enabled if it has any values)
			cmd = a4_cmds.string; //copy the pointer to the string
			while ( a4_cmds.string != 0 )
			{
				token = COM_Parse( &cmd ); //token will contain a single word, progressing 1 word each loop
				if ( !token || !token[0] ) {
					break;
				}

				if ( !strcmp( Command,"cn" ) ) {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					}
					strcpy( message, va( "chat \"" ) );
					for ( i = 0; i < count; i++ )
						strcat( message, va( "%s: %s%i%s\n", g_entities[nums[i]].client->pers.netname, S_COLOR_YELLOW, nums[i], S_COLOR_WHITE ) );
					strcat( message, "\"" );

					trap_SendServerCommand( ent - g_entities, message );
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"kick" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_DropClient( nums[i], va( "kicked by admin. %s", ent->client->pers.Command3 ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3kicked by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"tempban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %s", nums[i], ent->client->pers.Command3 ) );
						trap_DropClient( nums[i], va( "temporarily banned by admin." ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3Temporarily banned by admin.\n\"", g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"ban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_DropClient( nums[i], va( "banned by admin. %s", ent->client->pers.Command3 ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3banned by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
						trap_SendConsoleCommand( EXEC_APPEND, va( "addban %s", g_entities[nums[i]].client->sess.ip ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"shuffle" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin shuffled the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( ( !strcmp( Command,"ignore" ) ) || ( !strcmp( Command,"unignore" ) ) || ( !strcmp( Command,"slap" ) ) || ( !strcmp( Command,"kill" ) ) || ( !strcmp( Command,"forceteam" ) ) ) && ( !Q_stricmp( Command, token ) ) )                               {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "%s %i %s\n", Command, nums[i], ent->client->pers.Command3 ) );
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You used ^1%s ^3on ^1%s\n\"", Command, g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"g_password" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"map" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed the map.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"swap_teams" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin swapped the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"reset_match" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin reset the match.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"say" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cp" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} // L0
				else if ( ( !strcmp( Command,"pause" ) ) && ( !Q_stricmp( Command, token ) ) ) {
					Cmd_Pause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"unpause" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_UnPause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cancelvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Cancelvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"passvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Passvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"warn" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "print \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
					te->s.eventParm = G_SoundIndex( "sound/player/buzzwarn.wav" );
					te->r.svFlags |= SVF_BROADCAST;
					usedCmd = qtrue; // L0 - end
				} else if ( !Q_stricmp( Command, token ) )      {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s %s", Command, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed %s to %s\n\"", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
					return;
				}
			}
		}
		if ( usedCmd == qfalse ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You are not allowed to use ^1%s ^3as level four admin\n\"", Command ) );
		}
	}

	//Admin 3
	if ( ent->client->sess.admin == 3 ) {
		if ( strlen( a3_cmds.string ) ) { //if the cvar is enabled (it's a string, so it's enabled if it has any values)
			cmd = a3_cmds.string; //copy the pointer to the string
			while ( a3_cmds.string != 0 )
			{
				token = COM_Parse( &cmd ); //token will contain a single word, progressing 1 word each loop
				if ( !token || !token[0] ) {
					break;
				}

				if ( !strcmp( Command,"cn" ) ) {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					}
					strcpy( message, va( "chat \"" ) );
					for ( i = 0; i < count; i++ )
						strcat( message, va( "%s: %s%i%s\n", g_entities[nums[i]].client->pers.netname, S_COLOR_YELLOW, nums[i], S_COLOR_WHITE ) );
					strcat( message, "\"" );
					trap_SendServerCommand( ent - g_entities, message );
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"kick" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_DropClient( nums[i], va( "kicked by admin. %s", ent->client->pers.Command3 ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3kicked by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"tempban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %s", nums[i], ent->client->pers.Command3 ) );
						trap_DropClient( nums[i], va( "temporarily banned by admin." ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3Temporarily banned by admin.\n\"", g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"ban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_DropClient( nums[i], va( "banned by admin. %s", ent->client->pers.Command3 ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3banned by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
						trap_SendConsoleCommand( EXEC_APPEND, va( "addban %s", g_entities[nums[i]].client->sess.ip ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"shuffle" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin shuffled the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( ( !strcmp( Command,"ignore" ) ) || ( !strcmp( Command,"unignore" ) ) || ( !strcmp( Command,"slap" ) ) || ( !strcmp( Command,"kill" ) ) || ( !strcmp( Command,"forceteam" ) ) ) && ( !Q_stricmp( Command, token ) ) )                               {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "%s %i %s\n", Command, nums[i], ent->client->pers.Command3 ) );
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You used ^1%s ^3on ^1%s\n\"", Command, g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"g_password" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"map" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed the map.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"swap_teams" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin swapped the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"reset_match" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin reset the match.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"say" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cp" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} // L0
				else if ( ( !strcmp( Command,"pause" ) ) && ( !Q_stricmp( Command, token ) ) ) {
					Cmd_Pause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"unpause" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_UnPause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cancelvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Cancelvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"passvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Passvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"warn" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "print \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
					te->s.eventParm = G_SoundIndex( "sound/player/buzzwarn.wav" );
					te->r.svFlags |= SVF_BROADCAST;
					usedCmd = qtrue; // L0 - end
				} else if ( !Q_stricmp( Command, token ) )      {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s %s", Command, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed %s to %s\n\"", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
					return;
				}
			}
		}
		if ( usedCmd == qfalse ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You are not allowed to use ^1%s ^3as level three admin\n\"", Command ) );
		}
	}

	//Admin 2
	if ( ent->client->sess.admin == 2 ) {
		if ( strlen( a2_cmds.string ) ) { //if the cvar is enabled (it's a string, so it's enabled if it has any values)
			cmd = a2_cmds.string; //copy the pointer to the string
			while ( a2_cmds.string != 0 )
			{
				token = COM_Parse( &cmd ); //token will contain a single word, progressing 1 word each loop
				if ( !token || !token[0] ) {
					break;
				}

				if ( !strcmp( Command,"cn" ) ) {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					}
					strcpy( message, va( "chat \"" ) );
					for ( i = 0; i < count; i++ )
						strcat( message, va( "%s: %s%i%s\n", g_entities[nums[i]].client->pers.netname, S_COLOR_YELLOW, nums[i], S_COLOR_WHITE ) );
					strcat( message, "\"" );
					trap_SendServerCommand( ent - g_entities, message );
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"kick" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_DropClient( nums[i], va( "kicked by admin. %s", ent->client->pers.Command3 ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3kicked by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"tempban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %s", nums[i], ent->client->pers.Command3 ) );
						trap_DropClient( nums[i], va( "temporarily banned by admin." ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3Temporarily banned by admin.\n\"", g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"ban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_DropClient( nums[i], va( "banned by admin. %s", ent->client->pers.Command3 ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3banned by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
						trap_SendConsoleCommand( EXEC_APPEND, va( "addban %s", g_entities[nums[i]].client->sess.ip ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"shuffle" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin shuffled the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( ( !strcmp( Command,"ignore" ) ) || ( !strcmp( Command,"unignore" ) ) || ( !strcmp( Command,"slap" ) ) || ( !strcmp( Command,"kill" ) ) || ( !strcmp( Command,"forceteam" ) ) ) && ( !Q_stricmp( Command, token ) ) )                               {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "%s %i %s\n", Command, nums[i], ent->client->pers.Command3 ) );
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You used ^1%s ^3on ^1%s\n\"", Command, g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"g_password" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"map" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed the map.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"swap_teams" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin swapped the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"reset_match" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin reset the match.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"say" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cp" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} // L0
				else if ( ( !strcmp( Command,"pause" ) ) && ( !Q_stricmp( Command, token ) ) ) {
					Cmd_Pause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"unpause" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_UnPause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cancelvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Cancelvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"passvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Passvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"warn" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "print \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
					te->s.eventParm = G_SoundIndex( "sound/player/buzzwarn.wav" );
					te->r.svFlags |= SVF_BROADCAST;
					usedCmd = qtrue; // L0 - end
				} else if ( !Q_stricmp( Command, token ) )      {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s %s", Command, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed %s to %s\n\"", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
					return;
				}
			}
		}
		if ( usedCmd == qfalse ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You are not allowed to use ^1%s ^3as level two admin\n\"", Command ) );
		}
	}

	//Admin 1
	if ( ent->client->sess.admin == 1 ) {
		if ( strlen( a1_cmds.string ) ) { //if the cvar is enabled (it's a string, so it's enabled if it has any values)
			cmd = a1_cmds.string; //copy the pointer to the string
			while ( a1_cmds.string != 0 )
			{
				token = COM_Parse( &cmd ); //token will contain a single word, progressing 1 word each loop
				if ( !token || !token[0] ) {
					break;
				}
				if ( !strcmp( Command,"cn" ) ) {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					}
					strcpy( message, va( "chat \"" ) );
					for ( i = 0; i < count; i++ )
						strcat( message, va( "%s: %s%i%s\n", g_entities[nums[i]].client->pers.netname, S_COLOR_YELLOW, nums[i], S_COLOR_WHITE ) );
					strcat( message, "\"" );
					trap_SendServerCommand( ent - g_entities, message );
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"kick" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_DropClient( nums[i], va( "kicked by admin. %s", ent->client->pers.Command3 ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3kicked by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"tempban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "tempban %i %s", nums[i], ent->client->pers.Command3 ) );
						trap_DropClient( nums[i], va( "temporarily banned by admin." ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3Temporarily banned by admin.\n\"", g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"ban" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_DropClient( nums[i], va( "banned by admin. %s", ent->client->pers.Command3 ) );
						trap_SendServerCommand( -1, va( "chat \"console: %s ^3banned by admin. %s\n\"", g_entities[nums[i]].client->pers.netname,ent->client->pers.Command3 ) );
						trap_SendConsoleCommand( EXEC_APPEND, va( "addban %s", g_entities[nums[i]].client->sess.ip ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"shuffle" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin shuffled the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( ( !strcmp( Command,"ignore" ) ) || ( !strcmp( Command,"unignore" ) ) || ( !strcmp( Command,"slap" ) ) || ( !strcmp( Command,"kill" ) ) || ( !strcmp( Command,"forceteam" ) ) ) && ( !Q_stricmp( Command, token ) ) )                               {
					count = ClientNumberFromNameMatch( ent->client->pers.Command2, nums );
					if ( count == 0 ) {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Client not on server\n\"" ) );
						usedCmd = qtrue;
						return;
					} else if ( count > 1 )    {
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3To many people with %s in their name\n\"", ent->client->pers.Command2 ) );
						usedCmd = qtrue;
						return;
					}
					for ( i = 0; i < count; i++ ) {
						trap_SendConsoleCommand( EXEC_APPEND, va( "%s %i %s\n", Command, nums[i], ent->client->pers.Command3 ) );
						trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You used ^1%s ^3on ^1%s\n\"", Command, g_entities[nums[i]].client->pers.netname ) );
					}
					usedCmd = qtrue;
					return;
				} else if ( ( !strcmp( Command,"g_password" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"map" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed the map.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"swap_teams" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin swapped the teams.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"reset_match" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin reset the match.\n\"" ) );
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s", Command ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"say" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "chat \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cp" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^%iADMIN: ^7%s %s\n\"",ent->client->sess.admin, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					usedCmd = qtrue;
				} // L0
				else if ( ( !strcmp( Command,"pause" ) ) && ( !Q_stricmp( Command, token ) ) ) {
					Cmd_Pause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"unpause" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_UnPause_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"cancelvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Cancelvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"passvote" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					Cmd_Passvote_f( ent );
					usedCmd = qtrue;
				} else if ( ( !strcmp( Command,"warn" ) ) && ( !Q_stricmp( Command, token ) ) )             {
					trap_SendServerCommand( -1, va( "cp \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "print \"^1WARNING^7! %s %s\n\"", ent->client->pers.Command2, ent->client->pers.Command3 ) );
					te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_SOUND );
					te->s.eventParm = G_SoundIndex( "sound/player/buzzwarn.wav" );
					te->r.svFlags |= SVF_BROADCAST;
					usedCmd = qtrue; // L0 - end
				} else if ( !Q_stricmp( Command, token ) )      {
					trap_SendConsoleCommand( EXEC_APPEND, va( "%s %s %s", Command, ent->client->pers.Command2, ent->client->pers.Command3 ) );
					trap_SendServerCommand( -1, va( "chat \"console: ^3Admin changed %s to %s\n\"", Command, ent->client->pers.Command2 ) );
					usedCmd = qtrue;
					return;
				}
			}
		}
		if ( usedCmd == qfalse ) {
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3You are not allowed to use ^1%s ^3as level one admin\n\"", Command ) );
		}
	}
}

