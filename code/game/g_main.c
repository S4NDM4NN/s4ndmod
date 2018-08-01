#include "g_local.h"
#include "S4ND_local.h"
#ifdef OMNIBOT
	#include  "g_rtcwbot_interface.h"
#endif

//S4NDM4NN - testing detours
#ifdef _WIN32
#include <windows.h>
#include <detours.h>
#endif
//TEST
level_locals_t level;

//mg42_t	automg42s;

typedef struct {
	vmCvar_t    *vmCvar;
	char        *cvarName;
	char        *defaultString;
	int cvarFlags;
	int modificationCount;          // for tracking changes
	qboolean trackChange;           // track this variable, and announce if changed
	qboolean teamShader;      // track and if changed, update shader state
} cvarTable_t;

gentity_t g_entities[MAX_GENTITIES];
gclient_t g_clients[MAX_CLIENTS];

gentity_t       *g_camEnt = NULL;   //----(SA)	script camera

// Rafael gameskill
extern int bg_pmove_gameskill_integer;
// done

vmCvar_t g_gametype;

// Rafael gameskill
vmCvar_t g_gameskill;
// done
//ADDED
vmCvar_t g_ippool;
vmCvar_t g_ipbadpool;


vmCvar_t g_dmflags;
vmCvar_t g_fraglimit;
vmCvar_t g_timelimit;
vmCvar_t g_capturelimit;
vmCvar_t g_friendlyFire;
vmCvar_t g_password;
vmCvar_t g_maxclients;
vmCvar_t g_maxGameClients;
vmCvar_t g_minGameClients;          // NERVE - SMF
vmCvar_t g_dedicated;
vmCvar_t g_speed;
vmCvar_t g_gravity;
vmCvar_t g_cheats;
vmCvar_t g_knockback;
vmCvar_t g_quadfactor;
vmCvar_t g_forcerespawn;
vmCvar_t g_inactivity;
vmCvar_t g_debugMove;
vmCvar_t g_debugDamage;
vmCvar_t g_debugAlloc;
vmCvar_t g_debugBullets;    //----(SA)	added
vmCvar_t g_debugSpawn;      //Martin
vmCvar_t g_weaponRespawn;
vmCvar_t g_motd;
vmCvar_t g_syncronousClients;
vmCvar_t g_warmup;

// NERVE - SMF
vmCvar_t g_warmupLatch;
vmCvar_t g_nextTimeLimit;
vmCvar_t g_showHeadshotRatio;
vmCvar_t g_userTimeLimit;
vmCvar_t g_userAlliedRespawnTime;
vmCvar_t g_userAxisRespawnTime;
vmCvar_t g_currentRound;
vmCvar_t g_noTeamSwitching;
vmCvar_t g_altStopwatchMode;
vmCvar_t g_gamestate;
vmCvar_t g_swapteams;
// -NERVE - SMF

vmCvar_t g_restarted;
vmCvar_t g_log;
vmCvar_t g_logSync;
vmCvar_t g_podiumDist;
vmCvar_t g_podiumDrop;
vmCvar_t g_allowVote;
vmCvar_t g_complaintlimit;          // DHM - Nerve
vmCvar_t g_maxlives;                // DHM - Nerve
vmCvar_t g_voiceChatsAllowed;       // DHM - Nerve

vmCvar_t g_needpass;
vmCvar_t g_weaponTeamRespawn;
vmCvar_t g_doWarmup;
vmCvar_t g_teamAutoJoin;
vmCvar_t g_teamForceBalance;
vmCvar_t g_listEntity;
vmCvar_t g_banIPs;
vmCvar_t g_banmsg;
vmCvar_t g_tempbanmsg;
vmCvar_t g_filterBan;
vmCvar_t g_rankings;
vmCvar_t g_enableBreath;
vmCvar_t g_smoothClients;
vmCvar_t pmove_fixed;
vmCvar_t pmove_msec;

// Rafael
vmCvar_t g_autoactivate;
vmCvar_t g_testPain;

vmCvar_t g_missionStats;
vmCvar_t ai_scriptName;         // name of AI script file to run (instead of default for that map)
vmCvar_t g_scriptName;          // name of script file to run (instead of default for that map)

vmCvar_t g_developer;

vmCvar_t g_userAim;

vmCvar_t g_forceModel;

vmCvar_t g_mg42arc;

vmCvar_t g_footstepAudibleRange;
// JPW NERVE multiplayer reinforcement times
vmCvar_t g_redlimbotime;
vmCvar_t g_bluelimbotime;
// charge times for character class special weapons
vmCvar_t g_medicChargeTime;
vmCvar_t g_engineerChargeTime;
vmCvar_t g_LTChargeTime;
vmCvar_t g_soldierChargeTime;
// screen shakey magnitude multiplier
vmCvar_t sv_screenshake;
// jpw
//S4NDM4NN- cvars
vmCvar_t g_speclock;
vmCvar_t g_checkguid;
vmCvar_t g_ammoPacks;
vmCvar_t g_medPacks;
vmCvar_t g_nadePacks;
vmCvar_t g_banGuids;
vmCvar_t g_goomba;
vmCvar_t g_poison;
vmCvar_t g_dropReload;
vmCvar_t g_reviveReports;
vmCvar_t g_tapReports;
vmCvar_t g_nameChangeReports;
vmCvar_t g_ltNades;
vmCvar_t g_medNades;
vmCvar_t g_engNades;
vmCvar_t g_soldNades;
vmCvar_t g_privateMsgs;
vmCvar_t g_minMsgChars;
vmCvar_t g_shove;
//vmCvar_t g_shoveAmount;
vmCvar_t g_shoveNoZ;
vmCvar_t g_kickableNades;
vmCvar_t g_kickableAS;
vmCvar_t g_kickerGetsOwner;
vmCvar_t g_gibReports;
vmCvar_t g_warmupDamage;
vmCvar_t g_unlockWeapons;
vmCvar_t g_pickupAmmo;
vmCvar_t g_ammoGivesSyringe;
vmCvar_t g_ammoGivesHelmet;
vmCvar_t g_ammoGivesBinocs;
vmCvar_t g_medExtraClips;
vmCvar_t g_ltExtraClips;
vmCvar_t g_engExtraClips;
vmCvar_t g_soldExtraClips;
vmCvar_t g_venomExtraClips;
vmCvar_t g_mauserExtraClips;
vmCvar_t g_pistolExtraClips;
vmCvar_t g_headshotDmg;
vmCvar_t g_helmetDmg;
vmCvar_t g_helmetProtects;
vmCvar_t g_teamSwitchTime;
vmCvar_t g_connectMsg;
vmCvar_t g_deathmatch;
vmCvar_t g_lateJoinLives;
vmCvar_t g_lateJoinTime;
vmCvar_t g_allowLateJoiners;
vmCvar_t g_maxPf;
vmCvar_t g_maxFlamer;
vmCvar_t g_maxMauser;
vmCvar_t g_maxVenom;
vmCvar_t g_alliedmaxlives;
vmCvar_t g_axismaxlives;
vmCvar_t g_ignoreSpecs;
vmCvar_t g_panzerArc;
vmCvar_t g_rocketSpeed;
vmCvar_t g_ASBlock;
vmCvar_t g_ASBlockEasy;
vmCvar_t g_disallowedVotes;
vmCvar_t g_lockteams;
vmCvar_t g_dropObj;
vmCvar_t g_dropNadepacks;
vmCvar_t g_throwableKnives;
vmCvar_t g_ammoGivesKnife;
vmCvar_t g_maxKnives;
vmCvar_t g_knifeDamage;
vmCvar_t g_wolfKick;
vmCvar_t g_dropClips;
vmCvar_t g_playDead;
vmCvar_t g_headshotsOnly;
vmCvar_t g_voteCredits;
vmCvar_t g_checkBan;
vmCvar_t g_chooseMg;
vmCvar_t g_mvsay;
vmCvar_t g_dragCorpse;
vmCvar_t g_underwaterMg;
vmCvar_t g_underwaterRevive;
vmCvar_t g_underwaterPistols;
vmCvar_t g_tournymode;
vmCvar_t g_forceWarmupSpawn;
vmCvar_t g_smokeGrenades;
vmCvar_t g_smokeGrenadeLimit;
vmCvar_t g_homingRockets;
vmCvar_t g_homingRocketsSens;
vmCvar_t g_panzerOverHeatTime;
vmCvar_t g_panzerClips;
vmCvar_t g_panzerCoolDownTime;
vmCvar_t g_panzerFireRate;
vmCvar_t g_extraPanzerClips;
vmCvar_t g_DmSpawnPoints;
vmCvar_t g_homingrocketdetects;
vmCvar_t g_test1;
vmCvar_t g_test2;
vmCvar_t g_test3;
vmCvar_t g_mines;
vmCvar_t g_smokingPlayers;
vmCvar_t g_goldrush;
vmCvar_t g_goldWeight;
vmCvar_t g_instaGib;
vmCvar_t g_assassanation;
vmCvar_t g_nodrowning;
vmCvar_t g_privateclients;
vmCvar_t g_autoShuffle;
vmCvar_t g_panzerWar;
vmCvar_t g_checkpureclient;
vmCvar_t g_makeDMspawns;
vmCvar_t g_DoHighAcheivers;
vmCvar_t g_LTinfomsg;
vmCvar_t g_allowSpys;
vmCvar_t g_reportSpys;
vmCvar_t g_antilag;
//vmCvar_t		g_antilagVersion;
vmCvar_t g_allowhitsounds;
vmCvar_t g_CaptureTheFlag;
vmCvar_t g_autoAim;
vmCvar_t g_jumpAccel;
vmCvar_t g_moonPhysics;
vmCvar_t updatebanlist;
vmCvar_t g_SpamWar;
vmCvar_t g_automg42limit;
vmCvar_t g_allowCenterview;
vmCvar_t g_dropBinocs;
vmCvar_t g_packingPistols;
vmCvar_t g_forceDLpaknames;
vmCvar_t g_forceDLchecksums;
vmCvar_t g_tableStats;
vmCvar_t g_S4NDLog;
vmCvar_t g_S4NDdebug;
vmCvar_t g_allowspeclock;
vmCvar_t g_allowGiveAmmo;
vmCvar_t g_bloodFlash;
//For msgs
vmCvar_t g_msg1;
vmCvar_t g_msg2;
vmCvar_t g_msg3;
vmCvar_t g_msg4;
vmCvar_t g_msg5;
vmCvar_t g_msg6;
vmCvar_t g_msg7;
vmCvar_t g_msg8;
vmCvar_t g_msg9;
vmCvar_t g_msg10;
vmCvar_t g_msgs;
vmCvar_t g_msgtime;

vmCvar_t msgNum;
//Admin
vmCvar_t a1_pass;
vmCvar_t a2_pass;
vmCvar_t a3_pass;
vmCvar_t a4_pass;
vmCvar_t a5_pass;

vmCvar_t a1_cmds;
vmCvar_t a2_cmds;
vmCvar_t a3_cmds;
vmCvar_t a4_cmds;
vmCvar_t a5_cmds;
vmCvar_t a5_allPowers;


//The trenches WW1 mod
vmCvar_t g_theTrenchesMode;

//agentbegun
vmCvar_t AB_Enable;
vmCvar_t AB_MinScore;
vmCvar_t AB_Tks;
//vmCvar_t		AB_CheatsOK;
vmCvar_t AB_Test;
vmCvar_t AB_AutoMaps;
vmCvar_t AB_MinNameChars;
vmCvar_t AB_MinHits;
vmCvar_t AB_MinHitsPercent;
//vmCvar_t		AB_CvarCheck;
vmCvar_t AB_KnifePoints;
vmCvar_t AB_MgPoints;
vmCvar_t AB_MauserPoints;
vmCvar_t AB_Mg42Points;
vmCvar_t AB_ThrowKnivesPoints;
vmCvar_t AB_NadePoints;
vmCvar_t AB_AsPoints;
vmCvar_t AB_FlamerPoints;
vmCvar_t AB_VenomPoints;
vmCvar_t AB_PanzerPoints;
vmCvar_t AB_DynamitePoints;
vmCvar_t AB_PistolPoints;
vmCvar_t AB_SniperPoints;
vmCvar_t AB_MaxTkPoints;
//vmCvar_t		AB_MaxPing;
//vmCvar_t		AB_MaxPingTime;
//vmCvar_t		AB_KickHighPingers;
vmCvar_t AB_TempBanTime;
vmCvar_t AB_AllowNameSpam;
//vmCvar_t g_RetakeLimit;
vmCvar_t g_sniperWar;
//vmCvar_t g_autoSniper;
vmCvar_t g_ShowHighlights;
vmCvar_t g_AutoSpawnTimes;
vmCvar_t g_AutoHeavyWeapons;
vmCvar_t g_autoTeamBalance;
vmCvar_t g_needBalance;
vmCvar_t g_RedDefaultSpawnTime;
vmCvar_t g_BlueDefaultSpawnTime;
vmCvar_t cl_allowdownload;
vmCvar_t s_khz;
//vmCvar_t		r_znear;
vmCvar_t r_vertexlight;
vmCvar_t cl_anonymous;
vmCvar_t r_depthbits;
vmCvar_t cl_timenudge;
vmCvar_t r_primitives;          // Martin - 7/6/09 - Fix video driver issues with modern hardware
vmCvar_t g_ltExtraAirstrike;
vmCvar_t g_gibPoints;
//vmCvar_t		Players_Allies;
//vmCvar_t		Players_Axis;
//vmCvar_t		Players_Spec;
vmCvar_t g_autoTap;
vmCvar_t g_maxAirstrikes;
vmCvar_t g_showHitBoxes;
// martin - fixed physics
vmCvar_t g_fixedphysics;
vmCvar_t g_crouch;
vmCvar_t g_noSameTeamDefuse;
vmCvar_t g_truePing;
//vmCvar_t		g_anticheat;
vmCvar_t g_shortcuts;
vmCvar_t g_binocWar;
vmCvar_t g_mineHealth;
vmCvar_t g_shootableRocket;
vmCvar_t g_shootableGrenade;
vmCvar_t g_shootableMortar;
vmCvar_t g_shootableAS;

//vmCvar_t		g_mineArmAmt;
//vmCvar_t		g_mineSplashRadius;
//vmCvar_t		g_mineDetectDist;
vmCvar_t Uptime;
//vmCvar_t		g_fixedphysicsfps;
vmCvar_t g_fakePing;
vmCvar_t g_friendlyFireKnockback;
vmCvar_t g_spawnTimer;
vmCvar_t g_showLifeStats;
vmCvar_t g_undeadRevives;
vmCvar_t g_maxConnsPerIP;
vmCvar_t g_reducedScreenshake;
vmCvar_t g_statsUsername;           // S4ND_stats
vmCvar_t g_statsPassword;           // S4ND_stats
vmCvar_t g_statsEnable;             // S4ND_stats
vmCvar_t g_autoRestart;
vmCvar_t sv_punkbuster;
vmCvar_t sv_fps;
vmCvar_t sv_allowDownload;
vmCvar_t sv_master5;
vmCvar_t sv_maxPing;
vmCvar_t sv_minPing;
vmCvar_t g_restartScript;
vmCvar_t g_isDemoVersion;
vmCvar_t g_multikillTime;
vmCvar_t g_announcer;
//eng martin

vmCvar_t g_antiwarp;

//S4NDM4NN

#ifdef TEAMSPEAK
vmCvar_t ts_ip;
vmCvar_t ts_port;
#endif


//S4NDM4NN - test
vmCvar_t sv_referencedPaks;
vmCvar_t sv_referencedPakNames;
//added this to use to print s4ndmod and its version
vmCvar_t gamename;
vmCvar_t mod_url;

//end of my cvars

#ifdef OMNIBOT
vmCvar_t g_OmniBotEnable;
vmCvar_t g_OmniBotPath;
vmCvar_t g_OmniBotFlags;
vmCvar_t g_OmniBotPlaying;
vmCvar_t g_OmniBotGib;
vmCvar_t g_botTeam;
#endif

// L0 - my cvars
vmCvar_t g_FirstBlood;
vmCvar_t g_redDrawFirst;
vmCvar_t g_blueDrawFirst;
vmCvar_t g_doubleKills;
vmCvar_t g_showFlags;    // L0 - mcwf's GeoIP
// L0 - end

cvarTable_t gameCvarTable[] = {
	// don't override the cheat state set by the system
	{ &g_cheats, "g_cheats", "0", 0, qfalse },

	// noset vars
	{ &gamename, "gamename", GAMEVERSION, CVAR_SERVERINFO | CVAR_ROM | CVAR_CHEAT, 0, qfalse  },
	{ &mod_url, "mod_url", MODURL, CVAR_SERVERINFO | CVAR_ROM | CVAR_CHEAT, 0, qfalse  },
	{ NULL, "gamedate", __DATE__, CVAR_ROM, 0, qfalse  },
	{ NULL, "Compile Date", __DATE__, CVAR_ROM | CVAR_CHEAT | CVAR_SERVERINFO, 0, qfalse  },
	{ &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  },
	{ NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },

	// latched vars
	// DHM - Nerve :: default to GT_WOLF
	{ &g_gametype, "g_gametype", "5", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse  },

	//ADDED
	{ &g_ippool, "g_ippool", "", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_ipbadpool, "g_ipbadpool", "", CVAR_ARCHIVE, 0, qfalse  },


	// Rafael gameskill
	{ &g_gameskill, "g_gameskill", "3", CVAR_LATCH, 0, qfalse  },
	// done

// JPW NERVE multiplayer stuffs
	{ &sv_screenshake, "sv_screenshake", "5", CVAR_ARCHIVE, 0, qfalse},
	{ &g_redlimbotime, "g_redlimbotime", "30000", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ROM, 0, qfalse },
	{ &g_bluelimbotime, "g_bluelimbotime", "30000", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ROM, 0, qfalse },
	{ &g_medicChargeTime, "g_medicChargeTime", "45000", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{ &g_engineerChargeTime, "g_engineerChargeTime", "30000", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{ &g_LTChargeTime, "g_LTChargeTime", "40000", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{ &g_soldierChargeTime, "g_soldierChargeTime", "20000", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
// jpw

	{ &g_maxclients, "sv_maxclients", "32", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },          // NERVE - SMF - made 32 from 8
	{ &g_maxGameClients, "g_maxGameClients", "0", CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
	{ &g_minGameClients, "g_minGameClients", "2", 0, 0, qfalse  },                               // NERVE - SMF

	// change anytime vars
	{ &g_dmflags, "dmflags", "0", CVAR_ARCHIVE, 0, qtrue  },
	{ &g_fraglimit, "fraglimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
	{ &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_capturelimit, "capturelimit", "8", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },

	{ &g_syncronousClients, "g_syncronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse  },

	{ &g_friendlyFire, "g_friendlyFire", "1", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qtrue  },

	{ &g_teamForceBalance, "g_teamForceBalance", "1", CVAR_ARCHIVE  },                          // NERVE - SMF - merge from team arena

	{ &g_warmup, "g_warmup", "30", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_doWarmup, "g_doWarmup", "0", 0, 0, qtrue  },

	// NERVE - SMF
	{ &g_warmupLatch, "g_warmupLatch", "1", 0, 0, qfalse },

	{ &g_nextTimeLimit, "g_nextTimeLimit", "0", CVAR_WOLFINFO, 0, qfalse  },
	{ &g_currentRound, "g_currentRound", "0", CVAR_WOLFINFO, 0, qfalse  },
	{ &g_altStopwatchMode, "g_altStopwatchMode", "0", CVAR_ARCHIVE, 0, qtrue  },
	{ &g_gamestate, "gamestate", "-1", CVAR_WOLFINFO | CVAR_ROM, 0, qfalse  },

	{ &g_noTeamSwitching, "g_noTeamSwitching", "0", CVAR_ARCHIVE, 0, qtrue  },

	{ &g_showHeadshotRatio, "g_showHeadshotRatio", "0", 0, 0, qfalse  },

	{ &g_userTimeLimit, "g_userTimeLimit", "0", 0, 0, qfalse  },
	{ &g_userAlliedRespawnTime, "g_userAlliedRespawnTime", "0", 0, 0, qfalse  },
	{ &g_userAxisRespawnTime, "g_userAxisRespawnTime", "0", 0, 0, qfalse  },

	{ &g_swapteams, "g_swapteams", "0", CVAR_ROM, 0, qfalse },
	// -NERVE - SMF

	{ &g_log, "g_log", "", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_logSync, "g_logSync", "0", CVAR_ARCHIVE, 0, qfalse  },

	{ &g_password, "g_password", "", CVAR_USERINFO, 0, qfalse  },
	{ &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0, qfalse  },
	{ &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0, qfalse },
	{ &g_dedicated, "dedicated", "0", 0, 0, qfalse  },

	{ &g_speed, "g_speed", "320", 0, 0, qtrue  },
	{ &g_gravity, "g_gravity", "800", 0, 0, qtrue  },
	{ &g_knockback, "g_knockback", "1000", 0, 0, qtrue  },
	{ &g_quadfactor, "g_quadfactor", "3", 0, 0, qtrue  },
	{ &g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue  },
	{ &g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", 0, 0, qtrue },
	{ &g_forcerespawn, "g_forcerespawn", "0", 0, 0, qtrue },
	{ &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
	{ &g_debugMove, "g_debugMove", "0", 0, 0, qfalse },
	{ &g_debugDamage, "g_debugDamage", "0", CVAR_CHEAT, 0, qfalse },
	{ &g_debugAlloc, "g_debugAlloc", "0", 0, 0, qfalse },
	{ &g_debugBullets, "g_debugBullets", "0", 0, 0, qfalse},    //----(SA)	added
	{ &g_debugSpawn, "g_debugSpawn", "0", CVAR_CHEAT, 0, qtrue},
	{ &g_motd, "g_motd", "", CVAR_ARCHIVE, 0, qfalse },

	{ &g_podiumDist, "g_podiumDist", "80", 0, 0, qfalse },
	{ &g_podiumDrop, "g_podiumDrop", "70", 0, 0, qfalse },

	{ &g_allowVote, "g_allowVote", "1", 0, 0, qfalse },
	{ &g_listEntity, "g_listEntity", "0", 0, 0, qfalse },

	{ &g_complaintlimit, "g_complaintlimit", "0", CVAR_ARCHIVE, 0, qtrue},                      // DHM - Nerve
	{ &g_maxlives, "g_maxlives", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_SERVERINFO, 0, qtrue},       // DHM - Nerve
	{ &g_voiceChatsAllowed, "g_voiceChatsAllowed", "4", CVAR_ARCHIVE, 0, qfalse},               // DHM - Nerve

	{ &g_enableBreath, "g_enableBreath", "1", 0, 0, qtrue},     //agent removed
	{ &g_testPain, "g_testPain", "0", CVAR_CHEAT, 0, qfalse },
	{ &g_missionStats, "g_missionStats", "0", CVAR_ROM, 0, qfalse },
	{ &g_developer, "developer", "0", CVAR_TEMP, 0, qfalse },
	{ &g_rankings, "g_rankings", "0", 0, 0, qfalse},
	{ &g_userAim, "g_userAim", "1", CVAR_CHEAT, 0, qfalse },

	{ &g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse},
	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0, qfalse},
	{ &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0, qfalse},

	{&g_mg42arc, "g_mg42arc", "0", CVAR_TEMP, 0, qfalse},

	{&g_footstepAudibleRange, "g_footstepAudibleRange", "256", CVAR_CHEAT, 0, qfalse},

	{&g_scriptName, "g_scriptName", "", CVAR_ROM, 0, qfalse},
	{&ai_scriptName, "ai_scriptName", "", CVAR_ROM, 0, qfalse},
	//S4NDM4NN cvars
	{&g_speclock, "g_speclock", "0", /*CVAR_SERVERINFO | CVAR_LATCH*/ 0, 0, qtrue},
	//{&g_checkguid, "g_checkguid", "0", 0, 0, qtrue},
	{&g_medPacks, "g_medPacks", "2", 0, 0, qtrue},
	{&g_ammoPacks, "g_ammoPacks", "1", 0, 0, qtrue},
	{&g_nadePacks, "g_nadePacks", "1", 0, 0, qtrue},
	//{&g_banGuids, "g_banGuids", "", 0, 0, qfalse},
	{&g_goomba, "g_goomba", "1", 0, 0, qtrue},
	{&g_poison, "g_poison", "15", 0, 0, qtrue},
	{&g_dropReload, "g_dropReload", "0", 0, 0, qtrue},
	{&g_reviveReports, "g_reviveReports", "1", 0, 0, qtrue},
	{&g_tapReports, "g_tapReports", "1", 0, 0, qtrue},
	{&g_nameChangeReports, "g_nameChangeReports", "1", 0, 0, qtrue},
	{&g_ltNades, "g_ltNades", "1", 0, 0, qtrue},
	{&g_engNades, "g_engNades", "8", 0, 0, qtrue},
	{&g_medNades, "g_medNades", "1", 0, 0, qtrue},
	{&g_soldNades, "g_soldNades", "4", 0, 0, qtrue},
	{&g_minMsgChars, "g_minMsgChars", "3", 0, 0, qtrue},
	{&g_privateMsgs, "g_privateMsgs", "1", 0, 0, qtrue},
	{&g_shove, "g_shove", "150", 0, 0, qtrue},
	//{&g_shoveAmount, "g_shoveAmount", "1", 0, 0, qtrue},
	{&g_shoveNoZ, "g_shoveNoZ", "1", 0, 0, qtrue},
	{&g_kickableNades, "g_kickableNades", "1", 0, 0, qtrue},
	{&g_kickableAS, "g_kickableAS", "1", 0, 0, qtrue},
	{&g_kickerGetsOwner, "g_kickerGetsOwner", "0",0,0,qtrue},
	{&g_gibReports, "g_gibReports", "1", 0, 0, qtrue},
	{&g_warmupDamage, "g_warmupDamage", "1", 0, 0, qtrue},
	{&g_unlockWeapons, "g_unlockWeapons", "1", CVAR_SERVERINFO | CVAR_LATCH, 0, qtrue},
	{&g_pickupAmmo, "g_pickupAmmo", "1", 0, 0, qtrue},
	{&g_ammoGivesSyringe, "g_ammoGivesSyringe", "1", 0, 0, qtrue},
	{&g_ammoGivesHelmet, "g_ammoGivesHelmet", "1", 0, 0, qtrue},
	{&g_ammoGivesBinocs, "g_ammoGivesBinocs", "1", 0, 0, qtrue},
	{&g_soldExtraClips, "g_soldExtraClips", "2", 0, 0, qfalse},
	{&g_medExtraClips, "g_medExtraClips", "0", 0, 0, qfalse},
	{&g_ltExtraClips, "g_ltExtraClips", "1", 0, 0, qfalse},
	{&g_engExtraClips, "g_engExtraClips", "1", 0, 0, qfalse},
	{&g_pistolExtraClips, "g_pistolExtraClips", "3", 0, 0, qfalse},
	{&g_mauserExtraClips, "g_mauserExtraClips", "1", 0, 0, qfalse},
	{&g_venomExtraClips, "g_venomExtraClips", "0", 0, 0, qfalse},
	{&g_headshotDmg, "g_headshotDmg", "50", 0, 0, qtrue},
	{&g_helmetDmg, "g_helmetDmg", "10", 0, 0, qtrue},
	{&g_helmetProtects, "g_helmetProtects", "0", 0, 0, qtrue},
	{&g_teamSwitchTime, "g_teamSwitchTime", "30", 0, 0, qtrue},
	{&g_connectMsg, "g_connectMsg", "", 0, 0, qfalse},
	{&g_deathmatch, "g_deathmatch", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qfalse },
	{&g_lateJoinTime, "g_lateJoinTime", "15", 0, 0, qtrue},
	{&g_allowLateJoiners, "g_allowlateJoiners", "0", 0, 0, qtrue},
	{&g_lateJoinLives, "g_lateJoinLives", "0", 0, 0, qtrue},
	{&g_maxPf, "g_maxPf", "-1", 0, 0, qfalse},
	{&g_maxFlamer, "g_maxFlamer", "-1", 0, 0, qfalse},
	{&g_maxMauser, "g_maxMauser", "-1", 0, 0, qtrue},
	{&g_maxVenom, "g_maxVenom", "-1", 0, 0, qtrue},
	{&g_axismaxlives, "g_axismaxlives", "0", CVAR_ARCHIVE | CVAR_LATCH, 0, qtrue},
	{&g_alliedmaxlives, "g_alliedmaxlives", "0", CVAR_ARCHIVE | CVAR_LATCH, 0, qtrue},
	{&g_ignoreSpecs, "g_ignoreSpecs", "0", 0, 0, qtrue},
	{&g_panzerArc, "g_panzerArc", "0", 0, 0, qtrue},
	{&g_rocketSpeed, "g_rocketSpeed", "2500", 0, 0, qtrue},
	{&g_ASBlock, "g_ASBlock", "1", 0, 0, qtrue},
	{&g_ASBlockEasy, "g_ASBlockEasy", "1", 0, 0, qtrue},
	{&g_disallowedVotes, "g_disallowedVotes", "", 0, 0, qfalse},
	{&g_lockteams, "g_lockteams", "0", 0, 0, qfalse},
	{&g_dropObj, "g_dropObj", "0", 0, 0, qtrue},
	{&g_dropNadepacks, "g_dropNadepacks", "1", 0, 0, qtrue},
	{&g_throwableKnives, "g_throwableKnives", "5", 0, 0, qtrue},
	{&g_ammoGivesKnife, "g_ammoGivesKnife", "1", 0, 0, qtrue},
	{&g_maxKnives, "g_maxKnives", "5", 0, 0, qtrue},
	{&g_knifeDamage, "g_knifeDamage", "35", 0, 0, qtrue},
	{&g_wolfKick, "g_wolfKick", "1", 0, 0, qtrue},
	{&g_dropClips, "g_dropClips", "1", 0, 0, qtrue},
	{&g_playDead, "g_playDead", "0", 0, 0, qtrue},
	{&g_headshotsOnly, "g_headshotsOnly", "0", 0, 0, qfalse},
	{&g_voteCredits, "g_voteCredits", "-1", CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse},
	{&g_checkBan, "g_checkBan", "1", 0, 0, qfalse},
	{&g_chooseMg, "g_chooseMg", "1", 0, 0, qtrue},
	{&g_mvsay, "g_mvsay", "1", 0, 0, qtrue},
	{&g_dragCorpse, "g_dragCorpse", "1", 0, 0, qtrue},
	{&g_underwaterRevive, "g_underwaterRevive", "1", CVAR_SERVERINFO | CVAR_LATCH, 0, qtrue},
	{&g_underwaterMg, "g_underwaterMg", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qtrue},
	{&g_underwaterPistols, "g_underwaterPistols", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qtrue},
	{&g_tournymode, "g_tournymode", "0", CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse},
	{&g_forceWarmupSpawn, "g_forceWarmupSpawn", "1", 0, 0, qtrue},
	{&g_smokeGrenades, "g_smokeGrenades", "15", 0, 0, qtrue},
	{&g_smokeGrenadeLimit, "g_smokeGrenadeLimit", "0", 0, 0, qtrue},
	{&g_homingRockets, "g_homingRockets", "0", 0, 0, qtrue},
	{&g_homingRocketsSens, "g_homingRocketsSens", ".9", 0, 0, qtrue},
	{&g_panzerClips, "g_panzerClips", "1", CVAR_LATCH, 0, qtrue},
	{&g_panzerOverHeatTime, "g_panzerOverHeatTime", "0", CVAR_LATCH, 0, qtrue},
	{&g_panzerCoolDownTime, "g_panzerCoolDownTime", "0", CVAR_LATCH, 0, qtrue},
	{&g_panzerFireRate, "g_panzerFireRate", "2000", CVAR_LATCH, 0, qtrue},
	{&g_extraPanzerClips, "g_extraPanzerClips", "4", 0, 0, qtrue},
	{&g_DmSpawnPoints, "g_DmSpawnpoints", "", 0, 0, qfalse},
	{&g_mines, "g_mines", "1", CVAR_LATCH, 0, qtrue},
	{&g_smokingPlayers, "g_smokingPlayers", "0", 0, 0, qtrue},
	{&g_goldrush, "g_goldrush", "0", 0, 0, qtrue},
	{&g_goldWeight, "g_goldWeight", "5", 0, 0, qtrue},
	{&g_instaGib, "g_instaGib", "0", 0, 0, qtrue},
	{&g_nodrowning, "g_nodrowning", "0", 0, 0, qtrue},
	{&g_homingrocketdetects, "g_homingrocketdetects", "600", 0, 0, qtrue},
	{&g_privateclients, "sv_privateClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },
	{&g_autoShuffle, "g_autoShuffle", "0", 0, 0, qfalse},
	{&g_panzerWar, "g_panzerWar", "0", 0, 0, qtrue},
	{&g_checkpureclient, "g_checkpureclient", "0", 0, 0, qtrue},
	{&g_makeDMspawns, "g_makeDMspawns", "0", 0, 0, qtrue},
	{&g_DoHighAcheivers, "g_DoHighAcheivers", "1", 0, 0, qtrue},
	{&g_LTinfomsg, "g_LTinfomsg", "1", 0, 0, qtrue},
	{&g_allowSpys, "g_allowSpys", "0", 0, 0, qtrue},
	{&g_reportSpys, "g_reportSpys", "1", 0, 0, qtrue},
	{&g_allowhitsounds, "g_allowhitsounds", "1", 0, 0, qtrue},
	{&g_CaptureTheFlag, "g_CaptureTheFlag", "0", CVAR_LATCH, 0, qtrue},
	{&g_autoAim, "g_autoAim", "0", 0, 0, qtrue},
	{&g_jumpAccel, "g_jumpAccel", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qtrue},
	{&g_moonPhysics, "g_moonPhysics", "0", CVAR_SERVERINFO | CVAR_LATCH, 0, qtrue},
	{&updatebanlist, "updatebanlist", "", CVAR_ROM, 0, qfalse},
	{&g_SpamWar, "g_SpamWar", "0", CVAR_LATCH, 0, qfalse},
	{&g_automg42limit, "g_automg42limit", "0", CVAR_LATCH, 0, qtrue},
	{&g_allowCenterview, "g_allowCenterview", "0", CVAR_LATCH, 0, qtrue},
	{&g_dropBinocs, "g_dropBinocs", "1", 0, 0, qtrue},
	{&g_packingPistols, "g_packingPistols", "1", CVAR_SERVERINFO | CVAR_LATCH, 0, qtrue},
	{&g_forceDLpaknames, "g_forceDLpaknames", "", 0, 0, qfalse},
	{&g_forceDLchecksums, "g_forceDLchecksums", "", 0, 0, qfalse},
	{&sv_referencedPaks, "sv_referencedPaks", "", CVAR_ROM | CVAR_CHEAT, 0, qfalse},
	{&sv_referencedPakNames, "sv_referencedPakNames", "", CVAR_ROM | CVAR_CHEAT, 0, qfalse},
	{&g_tableStats, "g_tableStats", "1", 0, 0, qtrue},
	{&g_S4NDLog, "g_S4NDLog", "0", CVAR_LATCH, 0, qtrue},
	{&g_S4NDdebug, "g_S4NDdebug", "0", CVAR_LATCH, 0, qtrue},
	{&g_allowspeclock, "g_allowspeclock", "0", 0, 0, qtrue},
	{&g_allowGiveAmmo, "g_allowGiveAmmo", "1", 0, 0, qtrue},
	{&g_mineHealth, "g_mineHealth", "150", 0, 0, qtrue},
	{&g_bloodFlash, "g_bloodFlash", "150", CVAR_ROM | CVAR_CHEAT, 0, qtrue},
	//{&g_mineArmAmt, "g_mineArmAmt", "99", 0, 0, qtrue},
	//{&g_mineDetectDist, "g_mineDetectDist", "200", 0, 0, qtrue},
	//{&g_mineSplashRadius, "g_mineSplashRadius", "400", 0, 0, qtrue},
	//adding this just to test stuff
	{&g_test1, "g_test1", "", 0, 0, qtrue},
	{&g_test2, "g_test2", "", 0, 0, qtrue},
	{&g_test3, "g_test3", "", 0, 0, qtrue},
	//For msgs
	{&g_msg1, "g_msg1", "", 0, 0, qfalse},
	{&g_msg2, "g_msg2", "", 0, 0, qfalse},
	{&g_msg3, "g_msg3", "", 0, 0, qfalse},
	{&g_msg4, "g_msg4", "", 0, 0, qfalse},
	{&g_msg5, "g_msg5", "", 0, 0, qfalse},
	{&g_msg6, "g_msg6", "", 0, 0, qfalse},
	{&g_msg7, "g_msg7", "", 0, 0, qfalse},
	{&g_msg8, "g_msg8", "", 0, 0, qfalse},
	{&g_msg9, "g_msg9", "", 0, 0, qfalse},
	{&g_msg10, "g_msg10", "", 0, 0, qfalse},
	{&g_msgs, "g_msgs", "0", 0, 0, qtrue},
	{&g_msgtime, "g_msgtime", "45", 0, 0, qtrue},
	{&msgNum, "msgNum", "1", 0, 0, qfalse},
	//Admin
	{&a1_pass, "a1_pass", "none", 0, 0, qfalse},
	{&a2_pass, "a2_pass", "none", 0, 0, qfalse},
	{&a3_pass, "a3_pass", "none", 0, 0, qfalse},
	{&a4_pass, "a4_pass", "none", 0, 0, qfalse},
	{&a5_pass, "a5_pass", "none", 0, 0, qfalse},

	{&a1_cmds, "a1_cmds", "", 0, 0, qfalse},
	{&a2_cmds, "a2_cmds", "", 0, 0, qfalse},
	{&a3_cmds, "a3_cmds", "", 0, 0, qfalse},
	{&a4_cmds, "a4_cmds", "", 0, 0, qfalse},
	{&a5_cmds, "a5_cmds", "", 0, 0, qfalse},
	{&a5_allPowers, "a5_allPowers", "0", 0, 0, qfalse},
	//End S4NDM4NN

	//The trenches
	{&g_theTrenchesMode, "g_theTrenchesMode", "0", CVAR_LATCH, 0, qfalse},
	//End
	{&AB_Enable, "AB_Enable", "1", CVAR_SERVERINFO, 0, qfalse},
	{&AB_MinScore, "AB_MinScore", "0", CVAR_LATCH, 0, qfalse},
	{&AB_Tks, "AB_Tks", "8", CVAR_LATCH, 0, qfalse},
	{&AB_AutoMaps, "AB_AutoMaps", "1", CVAR_LATCH, 0, qfalse},
	{&AB_AllowNameSpam, "AB_AllowNameSpam", "1", 0, 0, qfalse},
	{&AB_MinNameChars, "AB_MinNameChars", "3", CVAR_LATCH, 0, qfalse},
	{&AB_MinHits, "AB_MinHits", "50", 0, qtrue},
	{&AB_MinHitsPercent, "AB_MinHitsPercent", "0", 0, qtrue},

	{&AB_KnifePoints, "AB_KnifePoints", "50", 0, qfalse},
	{&AB_MgPoints, "AB_MgPoints", "30", 0, qfalse},
	{&AB_MauserPoints, "AB_MauserPoints", "50", 0, qfalse},
	{&AB_Mg42Points, "AB_Mg42Points", "20", 0, qfalse},
	{&AB_ThrowKnivesPoints, "AB_ThrowKnivesPoints", "40", 0, qfalse},
	{&AB_NadePoints, "AB_NadePoints", "25", 0, qfalse},
	{&AB_AsPoints, "AB_AsPoints", "25", 0, qfalse},
	{&AB_FlamerPoints, "AB_FlamerPoints", "50", 0, qfalse},
	{&AB_VenomPoints, "AB_VenomPoints", "30", 0, qfalse},
	{&AB_PanzerPoints, "AB_PanzerPoints", "25", 0, qfalse},
	{&AB_DynamitePoints, "AB_DynamitePoints", "20", 0, qfalse},
	{&AB_PistolPoints, "AB_PistolPoints", "50", 0, qfalse},
	{&AB_SniperPoints, "AB_SniperPoints", "50", 0, qfalse},
	{&AB_MaxTkPoints, "AB_MaxTkPoints", "200", 0, qfalse},
	{&AB_TempBanTime, "AB_TempBanTime", "5", 0, qfalse},
	{&g_sniperWar, "g_sniperWar", "0", CVAR_LATCH, 0, qtrue},
	{&g_ShowHighlights, "g_ShowHighlights", "0", 0, qtrue},
	{&g_AutoSpawnTimes, "g_AutoSpawnTimes", "0", 0, qtrue},
	{&g_AutoHeavyWeapons, "g_AutoHeavyWeapons", "1", 0, qfalse},
	{&g_autoTeamBalance, "g_autoTeamBalance", "1", 0, qtrue},
	{&g_needBalance, "g_needBalance", "0", CVAR_CHEAT, qfalse},
	{&g_RedDefaultSpawnTime, "g_RedDefaultSpawnTime", "", CVAR_ROM | CVAR_CHEAT, qfalse},
	{&g_BlueDefaultSpawnTime, "g_BlueDefaultSpawnTime", "", CVAR_ROM | CVAR_CHEAT, qfalse},
	{&g_ltExtraAirstrike, "g_ltExtraAirstrike", "1", CVAR_LATCH, qfalse},
	{&g_gibPoints, "g_gibPoints", "1", CVAR_LATCH, qfalse},
	{&g_autoTap, "g_autoTap", "0", CVAR_LATCH, qfalse},
	{&g_maxAirstrikes, "g_maxAirstrikes", "-1", CVAR_LATCH, qfalse},
	{&g_showHitBoxes, "g_showHitBoxes", "0", CVAR_CHEAT, qtrue},
	{&g_fixedphysics, "g_fixedphysics", "1", CVAR_SERVERINFO, qfalse },
	{&g_crouch, "g_crouch", "3", 0, qfalse},
	{&g_noSameTeamDefuse, "g_noSameTeamDefuse", "1", CVAR_LATCH, qfalse},
	{&g_truePing, "g_truePing", "1", CVAR_LATCH, qfalse},
	{&g_shortcuts, "g_shortcuts", "1", 0, 0, qtrue},
	{&g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_CHEAT, qfalse},
	{&g_binocWar, "g_binocWar", "3", 0, 0, qtrue},
	{&Uptime, "Uptime", "", CVAR_SERVERINFO | CVAR_CHEAT, qfalse},
	{&g_fakePing, "g_fakePing", "1", CVAR_ARCHIVE | CVAR_LATCH, qfalse},
	{&g_friendlyFireKnockback, "g_friendlyFireKnockback", "1", 0, qtrue},
	{&g_spawnTimer, "g_spawnTimer", "1", 0, qtrue},
	{&g_showLifeStats, "g_showLifeStats", "1", 0, qtrue},
	{&g_undeadRevives, "g_undeadRevives", "0", 0, qtrue},
	{&g_maxConnsPerIP, "g_maxConnsPerIP", "2", CVAR_LATCH, qtrue},
	{&g_antiwarp, "g_antiwarp", "1", CVAR_LATCH, qtrue},
	{&g_autoRestart, "g_autoRestart", "1", 0, qtrue},
	{&g_restartScript, "g_restartScript", "restart.cfg", 0, qtrue},
	{&g_reducedScreenshake, "g_reducedScreenshake", "1", 0, qtrue},	

	{ &g_multikillTime, "g_multikillTime", "1000", 0 },
	{ &g_announcer, "g_announcer", "7", 0 },
	//Stats
	{&g_statsUsername, "g_statsUsername", "", CVAR_ROM | CVAR_CHEAT, qfalse}, // S4ND_stats 9/30/09 Martin - A username to authenticate the server
	{&g_statsPassword, "g_statsPassword", "", CVAR_ROM | CVAR_CHEAT, qfalse}, // S4ND_stats 9/30/09 Martin - A password to authenticate the server
	{&g_statsEnable, "g_statsEnable", "0", CVAR_ROM | CVAR_CHEAT, qfalse},    // S4ND_stats
	
//#ifndef _DEBUG
	{&sv_punkbuster, "sv_punkbuster", "", CVAR_ROM | CVAR_CHEAT, qfalse},
//#endif
	{&sv_fps, "sv_minPing", "0", CVAR_ROM | CVAR_CHEAT, qfalse},
	{&sv_fps, "sv_maxPing", "0", CVAR_ROM | CVAR_CHEAT, qfalse},
	{&sv_master5, "sv_master5", "wolfmaster.s4ndmod.com", CVAR_ROM | CVAR_CHEAT, qfalse}, // A failsafe for when wolfmaster goes down.
	{&g_isDemoVersion, "g_isDemoVersion", "1", CVAR_ROM | CVAR_CHEAT, qfalse},

	{&g_shootableRocket, "g_shootableRocket", "0", 0,0, qtrue},
	{&g_shootableGrenade, "g_ShootableGrenade", "0", 0,0, qtrue},
	{&g_shootableMortar, "g_ShootableMortar", "0", 0,0, qtrue},
	{&g_shootableAS, "g_ShootableAS", "0", 0,0, qtrue},
	{&g_antilag, "g_antilag", "1", CVAR_SERVERINFO, 0, qtrue  },
	
	//set these cvars on the connecting clients
	{0, "r_vertexlight", "1", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},     // Fix problem on tundra rush, others. 3/20/10 - This is still _not_ fixed in engine 1.3, so this needs to be here.
	{0, "cl_allowdownload", "1", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},
	{0, "cg_predictitems", "0", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},	// People wont see themselves pick up extra ammopacks now
	//{0, "cl_anonymous", "0", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},
	{0, "r_primitives", "2", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},		// Martin - Fix video driver issues with modern hardware
	{0, "s_khz", "22", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},
	{0, "r_znear", "1", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},
	{0, "rate",	"50000", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},
	{0, "snaps", "1000", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},
	{0, "cl_timenudge", "0", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},
	{0, "cg_errordecay", "100", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},
	{0, "cl_maxpackets", "100", CVAR_SYSTEMINFO | CVAR_ROM | CVAR_CHEAT, qfalse},

#ifdef OMNIBOT
	{ &g_OmniBotEnable, "omnibot_enable", "1", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_OmniBotPath, "omnibot_path", "", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },
	{ &g_OmniBotPlaying, "omnibot_playing", "0", CVAR_ROM | CVAR_SERVERINFO, 0, qfalse },   // Martin - added serverinfo to be able to see how many bots are playing.
	{ &g_OmniBotFlags, "omnibot_flags", "32", CVAR_ARCHIVE | CVAR_NORESTART, 0, qfalse },	// Don't play sprees for bots by default
	{ &g_OmniBotGib, "g_botGib", "1", CVAR_ARCHIVE, 0, qtrue},
	{ &g_botTeam, "g_botTeam", "0", CVAR_SERVERINFO, qfalse},
#endif

#ifdef TEAMSPEAK
	{&ts_ip, "ts_ip", "0.0.0.0", 0, 0, qfalse},
	{&ts_port, "ts_port", "9987", 0, 0, qfalse},
#endif
	// L0
	{ &g_FirstBlood, "g_FirstBlood", "0", CVAR_ARCHIVE | CVAR_LATCH, 0, qfalse },
	{ &g_redDrawFirst, "g_redDrawFirst", "0", CVAR_ROM, 0, qfalse, qtrue },
	{ &g_blueDrawFirst, "g_blueDrawFirst", "0", CVAR_ROM, 0, qfalse, qtrue },
	{ &g_doubleKills, "g_doubleKills", "0", CVAR_ARCHIVE, 0, qfalse },
	{ &g_showFlags, "g_showFlags", "0", CVAR_LATCH, 0, qtrue} // L0 - mcwf's GeoIP
	// L0 - end
};

// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );


void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );

#ifdef OMNIBOT
void G_HandleMessage( int _clientfrom, const char *_buffer, int _messagesize, int _commandtime );
#endif

// Ridah, Cast AI
qboolean AICast_VisibleFromPos( vec3_t srcpos, int srcnum,
								vec3_t destpos, int destnum, qboolean updateVisPos );
qboolean AICast_CheckAttackAtPos( int entnum, int enemy, vec3_t pos, qboolean ducking, qboolean allowHitWorld );
void AICast_Init( void );
// done.

void G_RetrieveMoveSpeedsFromClient( int entnum, char *text );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
qboolean detoured;

#if defined( __MACOS__ )
#pragma export on
#endif
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 ) {
#if defined( __MACOS__ )
#pragma export off
#endif
//	if(!detoured){
//	orig_SV_BeginDownload =  DetourFunction( (LPBYTE)0x00433C80 , (LPBYTE)SV_BeginDownload_Hook );
//	detoured=qtrue;
//	}
	switch ( command ) {
	case GAME_INIT:
		EnableStackTrace();
#ifdef OMNIBOT
		Bot_Interface_InitHandles();
#endif
		G_InitGame( arg0, arg1, arg2 );
#ifdef OMNIBOT
		if ( !Bot_Interface_Init() ) {
			G_Printf( S_COLOR_RED "Unable to Initialize Omni-Bot.\n" );
		}
#endif
		return 0;
	case GAME_SHUTDOWN:
#ifdef OMNIBOT
		if ( !Bot_Interface_Shutdown() ) {
			G_Printf( S_COLOR_RED "Error shutting down Omni-Bot.\n" );
		}
#endif
	
		G_ShutdownGame( arg0 );
		//remove detours
		#ifdef _WIN32
		DetourRemove( (PBYTE)orig_Com_Print,(PBYTE)Com_Print_Hook );
		#endif
		DisableStackTrace();
		return 0;
	case GAME_CLIENT_CONNECT:
		return (int)ClientConnect( arg0, arg1, arg2 );
	case GAME_CLIENT_THINK:
		ClientThink( arg0 );
		return 0;
	case GAME_CLIENT_USERINFO_CHANGED:
		ClientUserinfoChanged( arg0 );
		return 0;
	case GAME_CLIENT_DISCONNECT:
		ClientDisconnect( arg0 );
		return 0;
	case GAME_CLIENT_BEGIN:
		ClientBegin( arg0 );
		return 0;
	case GAME_CLIENT_COMMAND:
		ClientCommand( arg0 );
		return 0;
#ifdef OMNIBOT
	case GAME_MESSAGERECEIVED:
	{
		int iClientFrom = arg0;
		const char *pBuffer = (const char *)arg1;
		int iBufferLength = arg2;
		int iCommandTime = arg3;
		G_HandleMessage( iClientFrom, pBuffer, iBufferLength, iCommandTime );
		return -1;
	}
#endif //OMNIBOT
	case GAME_RUN_FRAME:
		G_RunFrame( arg0 );
#ifdef OMNIBOT
		Bot_Interface_Update();
#endif
		return 0;
	case GAME_CONSOLE_COMMAND:
		return ConsoleCommand();

	case GAME_RETRIEVE_MOVESPEEDS_FROM_CLIENT:
		G_RetrieveMoveSpeedsFromClient( arg0, (char *)arg1 );
		return 0;
	}

	return -1;
}

void QDECL G_Printf( const char *fmt, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_Printf( text );
}

void QDECL G_DPrintf( const char *fmt, ... ) {
	va_list argptr;
	char text[1024];

	if ( !g_developer.integer ) {
		return;
	}

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_Printf( text );
}

void QDECL G_Error( const char *fmt, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );
	S4ND_Log( "!!!!!ERROR!!!!!: %s",text );

	// Martin - Experimental autorestart functionality
	// Without this cfg on the server, it still dies
	if ( g_autoRestart.integer ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "exec %s\n", g_restartScript.string ) );
	} else {
		trap_Error( text );
	}
}


#define CH_KNIFE_DIST       48  // from g_weapon.c
#define CH_LADDER_DIST      100
#define CH_WATER_DIST       100
#define CH_BREAKABLE_DIST   64
#define CH_DOOR_DIST        96
#define CH_ACTIVATE_DIST    96
#define CH_EXIT_DIST        256

#define CH_MAX_DIST         256     // use the largest value from above
#define CH_MAX_DIST_ZOOM    8192    // max dist for zooming hints

#ifdef OMNIBOT
qboolean G_EmplacedGunIsMountable( gentity_t* ent, gentity_t* other ) {
	if ( Q_stricmp( ent->classname, "misc_mg42" ) && Q_stricmp( ent->classname, "misc_aagun" ) ) {
		return qfalse;
	}

	if ( !other->client ) {
		return qfalse;
	}

	if ( BG_IsScopedWeapon( other->client->ps.weapon ) ) {
		return qfalse;
	}

	if ( other->client->ps.pm_flags & PMF_DUCKED ) {
		return qfalse;
	}

	if ( other->client->ps.persistant[PERS_HWEAPON_USE] ) {
		return qfalse;
	}

	if ( ent->r.currentOrigin[2] - other->r.currentOrigin[2] >= 40 ) {
		return qfalse;
	}

	if ( ent->r.currentOrigin[2] - other->r.currentOrigin[2] < 0 ) {
		return qfalse;
	}

	if ( ent->s.frame != 0 ) {
		return qfalse;
	}

	if ( ent->active ) {
		return qfalse;
	}

	if ( other->client->ps.grenadeTimeLeft ) {
		return qfalse;
	}

	if ( infront( ent, other ) ) {
		return qfalse;
	}

	return qtrue;
}

qboolean G_EmplacedGunIsRepairable( gentity_t* ent, gentity_t* other ) {
	if ( Q_stricmp( ent->classname, "misc_mg42" ) && Q_stricmp( ent->classname, "misc_aagun" ) ) {
		return qfalse;
	}

	if ( !other->client ) {
		return qfalse;
	}

	if ( BG_IsScopedWeapon( other->client->ps.weapon ) ) {
		return qfalse;
	}

	if ( other->client->ps.persistant[PERS_HWEAPON_USE] ) {
		return qfalse;
	}

	if ( ent->s.frame == 0 ) {
		return qfalse;
	}

	return qtrue;
}
#endif //OMNIBOT

/*
==============
G_CheckForCursorHints
    non-AI's check for cursor hint contacts

    server-side because there's info we want to show that the client
    just doesn't know about.  (health or other info of an explosive,invisible_users,items,etc.)

    traceEnt is the ent hit by the trace, checkEnt is the ent that is being
    checked against (in case the traceent was an invisible_user or something)

==============
*/
void G_CheckForCursorHints( gentity_t *ent ) {
	vec3_t forward, right, up, offset, end;
	trace_t     *tr;
	float dist;
	gentity_t   *checkEnt, *traceEnt = 0;
	//gentity_t *traceEnt2 = 0; // JPW NERVE // TTimo unused
	playerState_t *ps;
	int hintType, hintDist, hintVal;
	qboolean zooming, indirectHit;      // indirectHit means the checkent was not the ent hit by the trace (checkEnt!=traceEnt)
	int trace_contents;                 // DHM - Nerve

	// FIXME:	no need at all to do this trace/string comparison every frame.
	//			stagger it at least a little bit

//	if(!servercursorhints)
//		return;

#ifndef OMNIBOT
	if ( !ent->client ) {
		return;
	}
#else
	if ( !ent->client || ent->r.svFlags & SVF_BOT ) {
		return;
	}
#endif
	ps = &ent->client->ps;

	if ( ps->aiChar != AICHAR_NONE ) {
		return;
	}

	indirectHit = qfalse;

	zooming = (qboolean)( ps->eFlags & EF_ZOOMING );

	AngleVectors( ps->viewangles, forward, right, up );

	VectorCopy( ps->origin, offset );
	offset[2] += ps->viewheight;

	// lean
	if ( ps->leanf ) {
		VectorMA( offset, ps->leanf, right, offset );
	}

//	SnapVector( offset );

	if ( zooming ) {
		VectorMA( offset, CH_MAX_DIST_ZOOM, forward, end );
	} else {
		VectorMA( offset, CH_MAX_DIST, forward, end );
	}

	tr = &ps->serverCursorHintTrace;

	// DHM - Nerve
	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		trace_contents = ( CONTENTS_TRIGGER | CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_BODY );
	} else {
		trace_contents = ( CONTENTS_TRIGGER | CONTENTS_SOLID | CONTENTS_PLAYERCLIP | CONTENTS_BODY | CONTENTS_CORPSE );
	}

	trap_Trace( tr, offset, NULL, NULL, end, ps->clientNum, trace_contents );
	// dhm - end

	// reset all
	hintType    = ps->serverCursorHint      = HINT_NONE;
	hintVal     = ps->serverCursorHintVal   = 0;

	if ( zooming ) {
		dist        = tr->fraction * CH_MAX_DIST_ZOOM;
		hintDist    = CH_MAX_DIST_ZOOM;
	} else {
		dist        = tr->fraction * CH_MAX_DIST;
		hintDist    = CH_MAX_DIST;
	}

	if ( tr->fraction == 1 ) {
		return;
	}

	traceEnt = &g_entities[tr->entityNum];

	// DHM - Nerve :: Ignore trigger_objective_info
	if ( ( ps->stats[ STAT_PLAYER_CLASS ] != PC_MEDIC && traceEnt->client )
		 || !( strcmp( traceEnt->classname, "trigger_objective_info" ) )
		 || !( strcmp( traceEnt->classname, "trigger_multiple" ) ) ) {

		trap_Trace( tr, offset, NULL, NULL, end, traceEnt->s.number, trace_contents );
		if ( zooming ) {
			dist        = tr->fraction * CH_MAX_DIST_ZOOM;
			hintDist    = CH_MAX_DIST_ZOOM;
		} else {
			dist        = tr->fraction * CH_MAX_DIST;
			hintDist    = CH_MAX_DIST;
		}
		if ( tr->fraction == 1 ) {
			return;
		}
		traceEnt = &g_entities[tr->entityNum];
	}

	//
	// WORLD
	//
	if ( tr->entityNum == ENTITYNUM_WORLD ) {
		if ( ( tr->contents & CONTENTS_WATER ) && !( ps->powerups[PW_BREATHER] ) ) {
			hintDist = CH_WATER_DIST;
			hintType = HINT_WATER;
		}
		// ladder
		else if ( ( tr->surfaceFlags & SURF_LADDER ) && !( ps->pm_flags & PMF_LADDER ) ) {
			hintDist = CH_LADDER_DIST;
			hintType = HINT_LADDER;
		}
	}
	//
	// PEOPLE
	//
	else if ( tr->entityNum < MAX_CLIENTS ) {

		// DHM - Nerve
		if ( g_gametype.integer >= GT_WOLF ) {
			// Show medics a syringe if they can revive someone
			if ( traceEnt->client && traceEnt->client->sess.sessionTeam == ent->client->sess.sessionTeam
				 && ps->stats[ STAT_PLAYER_CLASS ] == PC_MEDIC
				 && traceEnt->client->ps.pm_type == PM_DEAD
				 && !( traceEnt->client->ps.pm_flags & PMF_LIMBO ) ) {
				hintDist    = 48;     // JPW NERVE matches weapon_syringe in g_weapon.c
				hintType    = HINT_REVIVE;
			}
		}
		// dhm - Nerve

	}
	//
	// OTHER ENTITIES
	//
	else {
		checkEnt = traceEnt;

		// check invisible_users first since you don't want to draw a hint based
		// on that ent, but rather on what they are targeting.
		// so find the target and set checkEnt to that to show the proper hint.
		if ( traceEnt->s.eType == ET_GENERAL ) {

			// ignore trigger_aidoor.  can't just not trace for triggers, since I need invisible_users...
			// damn, I would like to ignore some of these triggers though.

			if ( !Q_stricmp( traceEnt->classname, "trigger_aidoor" ) ) {
				return;
			}

			if ( !Q_stricmp( traceEnt->classname, "func_invisible_user" ) ) {
				indirectHit = qtrue;

				// DHM - Nerve :: Put this back in only in multiplayer
				if ( g_gametype.integer >= GT_WOLF && traceEnt->s.dmgFlags ) { // hint icon specified in entity
					hintType = traceEnt->s.dmgFlags;
					hintDist = CH_ACTIVATE_DIST;
					checkEnt = 0;
				} else {                      // use target for hint icon
					checkEnt = G_Find( NULL, FOFS( targetname ), traceEnt->target );
					if ( !checkEnt ) {     // no target found
						hintType = HINT_BAD_USER;
						hintDist = CH_MAX_DIST_ZOOM;    // show this one from super far for debugging
					}
				}
			}
		}


		if ( checkEnt ) {
			if ( checkEnt->s.eType == ET_GENERAL ) {

				// this is effectively an 'exit' brush.  they should be created with:
				//
				// classname = 'ai_trigger'
				// ainame = 'player'
				// target = 'endmap'
				//
				if ( !Q_stricmp( traceEnt->classname, "ai_trigger" ) ) {
					if ( ( !Q_stricmp( traceEnt->aiName, "player" ) ) &&
						 ( !Q_stricmp( traceEnt->target, "endmap" ) ) ) {
						hintDist = CH_EXIT_DIST;
						hintType = HINT_EXIT;

						// show distance in the cursorhint bar
						hintVal = (int)dist;        // range for this hint is 256, so it happens to translate nicely
					}
				}

				if ( ( !Q_stricmp( traceEnt->classname, "misc_mg42" ) ) && ( ps->weapon != WP_SNIPERRIFLE ) && ( ps->weapon != WP_FG42SCOPE ) ) { // JPW NERVE no hint if you're scoped in sniperwise
					hintDist = CH_ACTIVATE_DIST;
					hintType = HINT_MG42;

// JPW NERVE -- busted MG42 shouldn't show a use hint by default
					if ( traceEnt->health <= 0 ) {
						hintDist = 0;
						hintType = HINT_FORCENONE;
						ps->serverCursorHint = HINT_FORCENONE;
					}
// jpw

					// DHM - Nerve :: Engineers can repair turrets
					if ( g_gametype.integer >= GT_WOLF ) {
						if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
							if ( !traceEnt->takedamage ) {
								hintType = HINT_BUILD;
								hintDist = CH_BREAKABLE_DIST;
								hintVal = traceEnt->health;
								if ( hintVal > 255 ) {
									hintVal = 255;
								}
							}
						}
					}
					// dhm - end
				}
			} else if ( checkEnt->s.eType == ET_EXPLOSIVE )      {
//				if(traceEnt->s.dmgFlags) {	 // override flag		// hint icon specified in entity, this overrides type
//					hintType = traceEnt->s.dmgFlags;
//				} else {
				if ( ( checkEnt->health > 0 ) || ( checkEnt->spawnflags & 64 ) ) {  // JPW NERVE they are now if it's dynamite			// 0 health explosives are not breakable
					hintDist    = CH_BREAKABLE_DIST;
					hintType    = HINT_BREAKABLE;

					// DHM - Nerve :: Show different hint if it can only be destroyed with dynamite
					if ( g_gametype.integer >= GT_WOLF && ( checkEnt->spawnflags & 64 ) ) {
// JPW NERVE only show hint for players who can blow it up
						vec3_t mins,maxs,range = { 40, 40, 52 };
						int i,num;
						//int defendingTeam=0; // TTimo unused
						int touch[MAX_GENTITIES];
						gentity_t *hit = NULL;

						VectorSubtract( ent->client->ps.origin, range, mins );
						VectorAdd( ent->client->ps.origin, range, maxs );
						num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
						for ( i = 0 ; i < num ; i++ ) {
							hit = &g_entities[touch[i]];
							if ( !( hit->r.contents & CONTENTS_TRIGGER ) ) {
								continue;
							}
							if ( !strcmp( hit->classname,"trigger_objective_info" ) ) {
								if ( !( hit->spawnflags & ( AXIS_OBJECTIVE | ALLIED_OBJECTIVE ) ) ) {
									continue;
								}
// we're in a trigger_objective_info field with at least one owner, so use this one and bail
								break;
							}
						}
						if ( ( hit ) &&
							 ( ( ( ent->client->sess.sessionTeam == TEAM_RED ) && ( hit->spawnflags & ALLIED_OBJECTIVE ) ) ||
							   ( ( ent->client->sess.sessionTeam == TEAM_BLUE ) && ( hit->spawnflags & AXIS_OBJECTIVE ) ) )
							 ) {
							hintDist = CH_BREAKABLE_DIST * 2;
							hintType = HINT_BREAKABLE_DYNAMITE;
						} else {
							hintDist = 0;
							hintType = ps->serverCursorHint     = HINT_FORCENONE;
							hintVal     = ps->serverCursorHintVal   = 0;
							return;
						}
// jpw
					}
					// dhm - end

					hintVal     = checkEnt->health;         // also send health to client for visualization
				}
//				}
			} else if ( checkEnt->s.eType == ET_ALARMBOX )      {
				if ( checkEnt->health > 0 ) {
//					hintDist	= CH_BREAKABLE_DIST;
					hintType    = HINT_ACTIVATE;
				}
			} else if ( checkEnt->s.eType == ET_ITEM )      {
				gitem_t *it;
				it = &bg_itemlist[checkEnt->item - bg_itemlist];

				hintDist = CH_ACTIVATE_DIST;

				switch ( it->giType ) {
				case IT_HEALTH:
					hintType = HINT_HEALTH;
					break;
				case IT_TREASURE:
					hintType = HINT_TREASURE;
					break;
				case IT_CLIPBOARD:
					hintType = HINT_CLIPBOARD;
					break;
				case IT_WEAPON:
// JPW NERVE changed this to be more specific
					if ( it->giTag != WP_LUGER && it->giTag != WP_COLT ) {
						if ( ps->stats[STAT_PLAYER_CLASS] == PC_SOLDIER ) {   // soldier can pick up all weapons
							hintType = HINT_WEAPON;
							break;
						}
						if ( ps->stats[STAT_PLAYER_CLASS] != PC_LT ) { // medics & engrs can't pick up squat
							break;
						}
						if ( it->giTag == WP_THOMPSON || it->giTag == WP_MP40 || it->giTag == WP_STEN ) {
							hintType = HINT_WEAPON;
						}
					}
// jpw
					break;
				case IT_AMMO:
					hintType = HINT_AMMO;
					break;
				case IT_ARMOR:
					hintType = HINT_ARMOR;
					break;
				case IT_POWERUP:
					hintType = HINT_POWERUP;
					break;
				case IT_HOLDABLE:
					hintType = HINT_HOLDABLE;
					break;
				case IT_KEY:
					hintType = HINT_INVENTORY;
					break;
				case IT_TEAM:
					if ( !Q_stricmp( traceEnt->classname, "team_CTF_redflag" ) && ent->client->sess.sessionTeam == TEAM_BLUE ) {
						hintType = HINT_POWERUP;
					} else if ( !Q_stricmp( traceEnt->classname, "team_CTF_blueflag" ) && ent->client->sess.sessionTeam == TEAM_RED ) {
						hintType = HINT_POWERUP;
					}
					break;
				case IT_BAD:
				default:
					break;
				}
			} else if ( checkEnt->s.eType == ET_MOVER )     {
				if ( !Q_stricmp( checkEnt->classname, "func_door_rotating" ) ) {
					if ( checkEnt->moverState == MOVER_POS1ROTATE ) {  // stationary/closed
						hintDist = CH_DOOR_DIST;
						hintType = HINT_DOOR_ROTATING;

						if ( checkEnt->key == -1 ) {   // locked
							//						hintType = HINT_DOOR_ROTATING_LOCKED;
						}
					}
				} else if ( !Q_stricmp( checkEnt->classname, "func_door" ) )        {
					if ( checkEnt->moverState == MOVER_POS1 ) {    // stationary/closed
						hintDist = CH_DOOR_DIST;
						hintType = HINT_DOOR;

						if ( checkEnt->key == -1 ) {   // locked
							//						hintType = HINT_DOOR_LOCKED;
						}
					}
				} else if ( !Q_stricmp( checkEnt->classname, "func_button" ) )        {
					hintDist = CH_ACTIVATE_DIST;
					hintType = HINT_BUTTON;
				} /*
				else if(checkEnt->s.dmgFlags == HINT_CHAIR) {
				    hintDist = CH_ACTIVATE_DIST;
				    hintType = HINT_CHAIR;
				}*/
			}

			// DHM - Nerve :: Handle wolf multiplayer hints
			if ( g_gametype.integer >= GT_WOLF ) {

				if ( checkEnt->s.eType == ET_MISSILE ) {
					if ( ps->stats[ STAT_PLAYER_CLASS ] == PC_ENGINEER ) {
						hintDist    = CH_BREAKABLE_DIST;
						hintType    = HINT_DISARM;
						hintVal     = checkEnt->health;     // also send health to client for visualization
						if ( hintVal > 255 ) {
							hintVal = 255;
						}
					}
				}

			}
			// dhm - end

			// hint icon specified in entity (and proper contact was made, so hintType was set)
			// first try the checkent...
			if ( checkEnt->s.dmgFlags && hintType ) {
				hintType = checkEnt->s.dmgFlags;
			}
		}

		// then the traceent
		if ( traceEnt->s.dmgFlags && hintType ) {
			hintType = traceEnt->s.dmgFlags;
		}

	}

	if ( zooming ) {

		hintDist = CH_MAX_DIST_ZOOM;

		// zooming can eat a lot of potential hints
		switch ( hintType ) {

		// allow while zooming
		case HINT_PLAYER:
		case HINT_TREASURE:
		case HINT_LADDER:
		case HINT_EXIT:
		case HINT_PLYR_FRIEND:
		case HINT_PLYR_NEUTRAL:
		case HINT_PLYR_ENEMY:
		case HINT_PLYR_UNKNOWN:
			break;

		default:
			return;
		}
	}

	if ( dist <= hintDist ) {
		ps->serverCursorHint = hintType;
		ps->serverCursorHintVal = hintVal;
	}


//	Com_Printf("hint: %d\n", ps->serverCursorHint);
}


/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
	gentity_t   *e, *e2;
	int i, j;
	int c, c2;

	c = 0;
	c2 = 0;
	for ( i = 1, e = g_entities + i ; i < level.num_entities ; i++,e++ ) {
		if ( !e->inuse ) {
			continue;
		}

		if ( !e->team ) {
			continue;
		}

		if ( e->flags & FL_TEAMSLAVE ) {
			continue;
		}

		if ( !Q_stricmp( e->classname, "func_tramcar" ) ) {
			if ( e->spawnflags & 8 ) { // leader
				e->teammaster = e;
			} else
			{
				continue;
			}
		}

		c++;
		c2++;
		for ( j = i + 1, e2 = e + 1 ; j < level.num_entities ; j++,e2++ )
		{
			if ( !e2->inuse ) {
				continue;
			}
			if ( !e2->team ) {
				continue;
			}
			if ( e2->flags & FL_TEAMSLAVE ) {
				continue;
			}
			if ( !strcmp( e->team, e2->team ) ) {
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
//				e2->key = e->key;	// (SA) I can't set the key here since the master door hasn't finished spawning yet and therefore has a key of -1
				e2->flags |= FL_TEAMSLAVE;

				if ( !Q_stricmp( e2->classname, "func_tramcar" ) ) {
					trap_UnlinkEntity( e2 );
				}

				// make sure that targets only point at the master
				if ( e2->targetname ) {
					e->targetname = e2->targetname;

					// Rafael
					// note to self: added this because of problems
					// pertaining to keys and double doors
					if ( Q_stricmp( e2->classname, "func_door_rotating" ) ) {
						e2->targetname = NULL;
					}
				}
			}
		}
	}

	if ( trap_Cvar_VariableIntegerValue( "g_gametype" ) != GT_SINGLE_PLAYER ) {
		G_Printf( "%i teams with %i entities\n", c, c2 );
	}
}


/*
==============
G_RemapTeamShaders
==============
*/
void G_RemapTeamShaders() {
#ifdef MISSIONPACK
	char string[1024];
	float f = level.time * 0.001;
	Com_sprintf( string, sizeof( string ), "team_icon/%s_red", g_redteam.string );
	AddRemap( "textures/ctf2/redteam01", string, f );
	AddRemap( "textures/ctf2/redteam02", string, f );
	Com_sprintf( string, sizeof( string ), "team_icon/%s_blue", g_blueteam.string );
	AddRemap( "textures/ctf2/blueteam01", string, f );
	AddRemap( "textures/ctf2/blueteam02", string, f );
	trap_SetConfigstring( CS_SHADERSTATE, BuildShaderStateConfig() );
#endif
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void ) {
	int i;
	cvarTable_t *cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName,
							cv->defaultString, cv->cvarFlags );
		if ( cv->vmCvar ) {
			cv->modificationCount = cv->vmCvar->modificationCount;
		}

		if ( cv->teamShader ) {
			remapped = qtrue;
		}
	}

	if ( remapped ) {
		G_RemapTeamShaders();
	}

	// check some things
	// DHM - Gametype is currently restricted to supported types only
	if ( g_gametype.integer < GT_WOLF || g_gametype.integer > GT_WOLF_CPH ) { // JPW NERVE WOLF_CPH now highest type
		G_Printf( "g_gametype %i is out of range, defaulting to GT_WOLF(5)\n", g_gametype.integer );
		trap_Cvar_Set( "g_gametype", "5" );
		trap_Cvar_Update( &g_gametype );
	}

	// Rafael gameskill
	if ( g_gameskill.integer < GSKILL_EASY || g_gameskill.integer > GSKILL_VERYHARD ) {
		G_Printf( "g_gameskill %i is out of range, default to medium\n", g_gameskill.integer );
		trap_Cvar_Set( "g_gameskill", "3" ); // default to medium
	}

	bg_pmove_gameskill_integer = g_gameskill.integer;
	// done

	level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void ) {
	int i;
	cvarTable_t *cv;
	qboolean remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ ) {
		if ( cv->vmCvar ) {
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount ) {
				cv->modificationCount = cv->vmCvar->modificationCount;

				if ( cv->trackChange ) {
					trap_SendServerCommand( -1, va( "print \"Server: %s ^7changed to %s\n\"",
													cv->cvarName, cv->vmCvar->string ) );
				}

				if ( cv->teamShader ) {
					remapped = qtrue;
				}
			}
		}
	}

	if ( remapped ) {
		G_RemapTeamShaders();
	}
}



/*
==============
G_SpawnScriptCamera
    create the game entity that's used for camera<->script communication and portal location for camera view
==============
*/
void G_SpawnScriptCamera( void ) {
	if ( g_camEnt ) {
		G_FreeEntity( g_camEnt );
	}

	g_camEnt = G_Spawn();

	g_camEnt->scriptName = "scriptcamera";

	g_camEnt->s.eType = ET_CAMERA;
	g_camEnt->s.apos.trType = TR_STATIONARY;
	g_camEnt->s.apos.trTime = 0;
	g_camEnt->s.apos.trDuration = 0;
	VectorCopy( g_camEnt->s.angles, g_camEnt->s.apos.trBase );
	VectorClear( g_camEnt->s.apos.trDelta );

	g_camEnt->s.frame = 0;

	g_camEnt->r.svFlags |= SVF_NOCLIENT;        // only broadcast when in use

	if ( g_camEnt->s.number >= MAX_CLIENTS && g_camEnt->scriptName ) {
		G_Script_ScriptParse( g_camEnt );
		G_Script_ScriptEvent( g_camEnt, "spawn", "" );
	}

}




/*
============
G_InitGame
============
*/
void G_InitGame( int levelTime, int randomSeed, int restart ) {
	int i;
	char cs[MAX_INFO_STRING];
//	char		hostname[MAX_INFO_STRING];
	char serverIP[MAX_INFO_STRING];
	char mapName[64];
	char protocol[16];
	char ip1[10];
	char port[15];
	char ip2[10];
	char ip3[10];
	char ip4[10];
	unsigned int dwBlueOffset, dwRedOffset;
	unsigned int aRandomValues[MAX_REINFSEEDS];
	char strReinfSeeds[MAX_STRING_CHARS];
	FILE*config;

//randomly set a password to join the teamspeak
#ifdef TEAMSPEAK
	int tspassword;
#endif
//	FILE*Ice;
//	FILE*TrenchToast;
//	FILE*soundpack;
	time_t rawtime;
//	qboolean goodip;
//	char *ipcheck;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );


	trap_Cvar_VariableStringBuffer( "protocol", protocol, sizeof( protocol ) );
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	if ( trap_Cvar_VariableIntegerValue( "g_gametype" ) != GT_SINGLE_PLAYER ) {
		G_Printf( "------- Game Initialization -------\n" );
		G_Printf( "gamename: %s\n", GAMEVERSION );
		G_Printf( "gamedate: %s\n", __DATE__ );
	}

	//trap_Cvar_VariableStringBuffer( "sv_hostname", hostname, sizeof(hostname) );	// Martin - Unused
	trap_Cvar_VariableStringBuffer( "net_ip", serverIP, sizeof( serverIP ) );
	trap_Cvar_VariableStringBuffer( "net_port", port, sizeof( port ) );
	//trap_Cvar_Set( "sv_hostname", parseStats(hostname) );

	Q_strncpyz( ip1, "", sizeof( ip1 ) );
	Q_strncpyz( ip2, "", sizeof( ip2 ) );
	Q_strncpyz( ip3, "", sizeof( ip3 ) );
	Q_strncpyz( ip4, "", sizeof( ip4 ) );
	Q_strncpyz( port, "", sizeof( port ) );


	//Crash server
	//BreakIP(serverIP, ip1, ip2, ip3, ip4);
	//Dallas Box Break 74.52.131.50
/*	if(atoi(ip1)==74 && atoi(ip2)==52 && atoi(ip3)==131 && atoi(ip4)>=50 && atoi(ip4)<=60){
    //	    BEACH Break	70.84.181.105
    //if(atoi(ip1)==70 && atoi(ip2)==84 && atoi(ip3)==181 && atoi(ip4)==105){
    //yay
    }else{
        Com_Error(ERR_FATAL,"");
    }*/


	srand( randomSeed );

	G_RegisterCvars();

	G_ProcessIPBans();

	G_InitMemory();



	// NERVE - SMF - intialize gamestate
	if ( g_gamestate.integer == GS_INITIALIZE ) {
		if ( g_noTeamSwitching.integer ) {
			trap_Cvar_Set( "gamestate", va( "%i", GS_WAITING_FOR_PLAYERS ) );
		} else {
			trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP ) );
		}
	}


	//memset(&automg42s, 0, sizeof(automg42s));

	// set some level globals
	memset( &level, 0, sizeof( level ) );
	level.time = levelTime;
	level.startTime = levelTime;
	level.maxautomg42s = g_automg42limit.integer * 2;

	level.snd_fry = G_SoundIndex( "sound/player/fry.wav" );   // FIXME standing in lava / slime
	level.bulletRicochetSound = G_SoundIndex( "bulletRicochet" );
	level.snipersound = G_SoundIndex( "sound/weapons/mauser/mauserf1.wav" );
	level.knifeSound[0] = G_SoundIndex( "sound/weapons/knife/knife_hitwall1.wav" );

	// RF, init the anim scripting
	level.animScriptData.soundIndex = G_SoundIndex;
	level.animScriptData.playSound = G_AnimScriptSound;

	if ( g_gametype.integer != GT_SINGLE_PLAYER && g_log.string[0] ) {
		if ( g_logSync.integer ) {
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_SYNC );
		} else {
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND );
		}
		if ( !level.logFile ) {
			G_Printf( "WARNING: Couldn't open logfile: %s\n", g_log.string );
		} else {
			char serverinfo[MAX_INFO_STRING];

			trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );
#ifdef OMNIBOT
			Q_strncpyz( level.rawmapname, Info_ValueForKey( serverinfo, "mapname" ), sizeof( level.rawmapname ) );
#endif
			G_LogPrintf( "------------------------------------------------------------\n" );
			G_LogPrintf( "InitGame: %s\n", serverinfo );
		}
	} else {
		if ( trap_Cvar_VariableIntegerValue( "g_gametype" ) != GT_SINGLE_PLAYER ) {
			G_Printf( "Not logging to disk.\n" );
		}
	}

	if ( g_S4NDLog.integer ) {
		level.S4NDLog = fopen( "s4ndmod.log","a+" ); // Linux friendly name
		S4ND_Log( "------------------------------------------------------------" );
		S4ND_Log( "InitGame" );
		S4ND_Log( "Server time is: %s", asctime( timeinfo ) );
	}

	//level.AdminLog = fopen("Admin.log","a+");
	// start the MBL stuff

	level.exitlevel = 0;    //make sure this is 0 :)

	parseBanFiles();

#ifdef USE_MBL // S4ND_local.h
	//start worker thread, this will be used to resolve hostnames and eventually grab mbl.
	startWorkThread();
#endif

	G_InitWorldSession();

	// DHM - Nerve :: Clear out spawn target config strings
	if ( g_gametype.integer >= GT_WOLF ) {
		trap_GetConfigstring( CS_MULTI_INFO, cs, sizeof( cs ) );
		Info_SetValueForKey( cs, "numspawntargets", "0" );
		trap_SetConfigstring( CS_MULTI_INFO, cs );

		for ( i = CS_MULTI_SPAWNTARGETS; i < CS_MULTI_SPAWNTARGETS + MAX_MULTI_SPAWNTARGETS; i++ ) {
			trap_SetConfigstring( i, "" );
		}
	}
	
	// L0 - FirstBlood
	if ( g_gametype.integer == GT_WOLF && g_FirstBlood.integer == 2 ) {
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
		Info_SetValueForKey( cs, "winner", "-1" );
		trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );

		level.firstbloodTeam = -1;

		if ( g_currentRound.integer == 0 ) {
			trap_Cvar_Set( "g_redDrawFirst", "0" );
			trap_Cvar_Set( "g_blueDrawFirst", "0" );

			trap_Cvar_Update( &g_redDrawFirst );
			trap_Cvar_Update( &g_blueDrawFirst );
		}

		trap_SetConfigstring( CS_REDWON, va( "%i", g_redDrawFirst.integer ) );
		trap_SetConfigstring( CS_BLUEWON, va( "%i", g_blueDrawFirst.integer ) );
	} // L0 - end

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof( g_entities[0] ) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof( g_clients[0] ) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ),
						 &level.clients[0].ps, sizeof( level.clients[0] ) );

	// load level script


	G_Script_ScriptLoad();

	// reserve some spots for dead player bodies
	InitBodyQue();

	ClearRegisteredItems();

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString();

	// create the camera entity that will communicate with the scripts
	G_SpawnScriptCamera();

	// general initialization
	G_FindTeams();

	// make sure we have flags for CTF, etc
	if ( g_gametype.integer >= GT_TEAM ) {
		G_CheckTeamItems();
	}

	SaveRegisteredItems();

	S4ND_InitGame();        // Check to see if they hexed out sandmod, etc

	if ( trap_Cvar_VariableIntegerValue( "g_gametype" ) != GT_SINGLE_PLAYER ) {
		G_Printf( "-----------------------------------\n" );
	}
//AIBOTS

	if ( AB_AutoMaps.integer ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "exec mapconfigs/%s \n", mapName ) );
	}

	setupCvars();       // This sets server cvars to acceptable limits

	if ( g_dedicated.integer > 0 ) {  // Dont delete for localhost servers, thats just mean :)
		config = fopen( "s4ndmod/wolfconfig_mp.cfg","w+" );
		config = fopen( "main/wolfconfig_mp.cfg","w+" );
		if ( config ) {
			fclose( config );
		}
	}
#ifdef GLOBALSTATS
	if ( g_gamestate.integer == GS_PLAYING ) {
		stats_init();
	}
#endif

#ifdef TEAMSPEAK
	startTeamspeakServer();
	G_LogPrintf( "The teamspeak server has been started!\n" );
#endif

	// Set up the random reinforcement seeds for both teams and send to clients
	dwBlueOffset = rand() % MAX_REINFSEEDS;
	dwRedOffset = rand() % MAX_REINFSEEDS;
	strcpy( strReinfSeeds, va( "%d %d", ( dwBlueOffset << REINF_BLUEDELT ) + ( rand() % ( 1 << REINF_BLUEDELT ) ),
							   ( dwRedOffset << REINF_REDDELT )  + ( rand() % ( 1 << REINF_REDDELT ) ) ) );

	for ( i = 0; i < MAX_REINFSEEDS; i++ ) {
		aRandomValues[i] = ( rand() % REINF_RANGE ) * aReinfSeeds[i];
		strcat( strReinfSeeds, va( " %d", aRandomValues[i] ) );
	}

	level.dwBlueReinfOffset = 1000 * aRandomValues[dwBlueOffset] / aReinfSeeds[dwBlueOffset];
	level.dwRedReinfOffset  = 1000 * aRandomValues[dwRedOffset] / aReinfSeeds[dwRedOffset];
	trap_SetConfigstring( CS_REINFSEEDS, strReinfSeeds );

	// L0  - geoIP
	GeoIP_open();     //open/update
	// end

	//Set pb_sv_msgprefix for console scanning
	trap_SendConsoleCommand(EXEC_APPEND, "pb_sv_MsgPrefix \"^3PunkBuster Server\"\n");

	G_RemapTeamShaders(); //DO NOT DO ANYTHING INVOLVING CLIENTS AFTER THIS!!!

//	if (g_deathmatch.integer == 1){
	//trap_SendConsoleCommand(EXEC_APPEND, "g_DmSpawnpoints -1\n");
//	trap_SendConsoleCommand(EXEC_APPEND, va("exec %s\n", mapname.string));
//	}
	//So admins can have a custom config loaded each map :)

}


/*void (WINAPIV *orig_SV_BeginDownload)  ( client_t *cl  );
void SV_BeginDownload_Hook( client_t *cl ) {

    // Kill any existing download
//	SV_CloseDownload( cl );

    // cl->downloadName is non-zero now, SV_WriteDownloadToClient will see this and open
    // the file itself
//	Q_strncpyz( cl->downloadName, Cmd_Argv(1), sizeof(cl->downloadName) );

    G_LogPrintf("%s",cl->downloadName);

    orig_SV_BeginDownload(cl);
}
*/

/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart ) {
#ifndef OMNIBOT
	int i;
#endif //OMNIBOT

	// Ridah, kill AI cast's


	#ifdef TEAMSPEAK
	stopTeamspeakServer();
	G_LogPrintf( "The teamspeak server has been stopped!\n" );
	#endif


	if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
		G_Printf( "==== ShutdownGame ====\n" );
	}

	if ( level.logFile ) {
		G_LogPrintf( "ShutdownGame:\n" );
		G_LogPrintf( "------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
	}

	if ( level.S4NDLog ) {
		S4ND_Log( "ShutdownGame" );
		S4ND_Log( "------------------------------------------------------------" );
		fclose( level.S4NDLog );
	}


	//S4NDM4NN - clean out tempban file.
	// Martin - Removed on 2/2, was causing crashes. Using PB instead.
	//cleanTempBans();

#ifdef USE_MBL // S4ND_local.h
	//end our working thread
	endWorkThread();

	//S4NDM4NN - clear linklists
	while ( head != NULL )
		removeNode( head );

	while ( mhead != NULL )
		removeMessage( mhead );
#endif

	// Ridah, shutdown the Botlib, so weapons and things get reset upon doing a "map xxx" command
//	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {



	// done.
//	}
	// done.
#ifndef OMNIBOT
	for ( i = 0; i < g_maxclients.integer; i++ )
	{
		if ( g_entities[i].isFake ) {
			trap_BotFreeClient( i );                  //kick all the bots
		}
	}
#endif //OMNIBOT


	// write all the client session data so we can get it back
	G_WriteSessionData();

	// clean up GeoIP
	GeoIP_close();

#ifndef NO_BOT_SUPPORT
	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) ) {
//		BotAIShutdown( restart );
	}
#endif
}

//===================================================================

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link

void QDECL Com_Error( int level, const char *error, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, error );
	vsprintf( text, error, argptr );
	va_end( argptr );

	G_Error( "%s", text );
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, msg );
	vsprintf( text, msg, argptr );
	va_end( argptr );

	G_Printf( "%s", text );
}

#endif

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/

/*
=============
AddTournamentPlayer

If there are less than two tournament players, put a
spectator in the game and restart
=============
*/
void AddTournamentPlayer( void ) {
	int i;
	gclient_t   *client;
	gclient_t   *nextInLine;

	if ( level.numPlayingClients >= 2 ) {
		return;
	}

	// never change during intermission
	if ( level.intermissiontime ) {
		return;
	}

	nextInLine = NULL;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = &level.clients[i];
		if ( client->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( client->sess.sessionTeam != TEAM_SPECTATOR ) {
			continue;
		}
		// never select the dedicated follow or scoreboard clients
		if ( client->sess.spectatorState == SPECTATOR_SCOREBOARD ||
			 client->sess.spectatorClient < 0  ) {
			continue;
		}

		if ( !nextInLine || client->sess.spectatorTime < nextInLine->sess.spectatorTime ) {
			nextInLine = client;
		}
	}

	if ( !nextInLine ) {
		return;
	}

	level.warmupTime = -1;

	// set them to free-for-all team
	SetTeam( &g_entities[ nextInLine - level.clients ], "f",qfalse );
}

/*
=======================
RemoveTournamentLoser

Make the loser a spectator at the back of the line
=======================
*/
void RemoveTournamentLoser( void ) {
	int clientNum;

	if ( level.numPlayingClients != 2 ) {
		return;
	}

	clientNum = level.sortedClients[1];

	if ( level.clients[ clientNum ].pers.connected != CON_CONNECTED ) {
		return;
	}

	// make them a spectator
	SetTeam( &g_entities[ clientNum ], "s",qfalse );
}


/*
=======================
AdjustTournamentScores

=======================
*/
void AdjustTournamentScores( void ) {
	int clientNum;

	clientNum = level.sortedClients[0];
	if ( level.clients[ clientNum ].pers.connected == CON_CONNECTED ) {
		level.clients[ clientNum ].sess.wins++;
		ClientUserinfoChanged( clientNum );
	}

	clientNum = level.sortedClients[1];
	if ( level.clients[ clientNum ].pers.connected == CON_CONNECTED ) {
		level.clients[ clientNum ].sess.losses++;
		ClientUserinfoChanged( clientNum );
	}

}

/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b ) {
	gclient_t   *ca, *cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD || ca->sess.spectatorClient < 0 ) {
		return 1;
	}
	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD || cb->sess.spectatorClient < 0  ) {
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING ) {
		return 1;
	}
	if ( cb->pers.connected == CON_CONNECTING ) {
		return -1;
	}


	// then spectators
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR && cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		if ( ca->sess.spectatorTime < cb->sess.spectatorTime ) {
			return -1;
		}
		if ( ca->sess.spectatorTime > cb->sess.spectatorTime ) {
			return 1;
		}
		return 0;
	}
	if ( ca->sess.sessionTeam == TEAM_SPECTATOR ) {
		return 1;
	}
	if ( cb->sess.sessionTeam == TEAM_SPECTATOR ) {
		return -1;
	}

	// then sort by score
	if ( ca->ps.persistant[PERS_SCORE]
		 > cb->ps.persistant[PERS_SCORE] ) {
		return -1;
	}
	if ( ca->ps.persistant[PERS_SCORE]
		 < cb->ps.persistant[PERS_SCORE] ) {
		return 1;
	}
	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void ) {
	int i;
	int rank;
	int score;
	int newScore;
	gclient_t   *cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.numVotingClients = 0;     // don't count bots

	level.numFinalDead[0] = 0;      // NERVE - SMF
	level.numFinalDead[1] = 0;      // NERVE - SMF

	for ( i = 0; i < 2; i++ ) {
		level.numteamVotingClients[i] = 0;
	}
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_DISCONNECTED ) {
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			if ( level.clients[i].sess.sessionTeam != TEAM_SPECTATOR ) {
				level.numNonSpectatorClients++;

				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED ) {
					level.numPlayingClients++;
					if ( !( g_entities[i].r.svFlags & SVF_BOT ) ) {
						level.numVotingClients++;
					}

					// CS: don't exclude bots from these since maxlives games rely on them
					if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
						// NERVE - SMF
						if ( level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT] == 0
							 && g_entities[i].health <= 0 ) {
							level.numFinalDead[0]++;
						}

						level.numteamVotingClients[0]++;
					} else if ( level.clients[i].sess.sessionTeam == TEAM_BLUE )   {
						// NERVE - SMF
						if ( level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT] == 0
							 && g_entities[i].health <= 0 ) {
							level.numFinalDead[1]++;
						}

						level.numteamVotingClients[1]++;
					}

					if ( level.follow1 == -1 ) {
						level.follow1 = i;
					} else if ( level.follow2 == -1 ) {
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort( level.sortedClients, level.numConnectedClients,
		   sizeof( level.sortedClients[0] ), SortRanks );

	// set the rank value for all clients that are connected and not spectators
	if ( g_gametype.integer >= GT_TEAM ) {
		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0; i < level.numConnectedClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 2;
			} else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				cl->ps.persistant[PERS_RANK] = 0;
			} else {
				cl->ps.persistant[PERS_RANK] = 1;
			}
		}
	} else {
		rank = -1;
		score = 0;
		for ( i = 0; i < level.numPlayingClients; i++ ) {
			cl = &level.clients[ level.sortedClients[i] ];
			newScore = cl->ps.persistant[PERS_SCORE];
			if ( i == 0 || newScore != score ) {
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			} else {
				// we are tied with the previous client
				level.clients[ level.sortedClients[i - 1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
			if ( g_gametype.integer == GT_SINGLE_PLAYER && level.numPlayingClients == 1 ) {
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
		}
	}

	// set the CS_SCORES1/2 configstrings, which will be visible to everyone
	if ( g_gametype.integer >= GT_TEAM ) {
		trap_SetConfigstring( CS_SCORES1, va( "%i", level.teamScores[TEAM_RED] ) );
		trap_SetConfigstring( CS_SCORES2, va( "%i", level.teamScores[TEAM_BLUE] ) );
	} else {
		if ( level.numConnectedClients == 0 ) {
			trap_SetConfigstring( CS_SCORES1, va( "%i", SCORE_NOT_PRESENT ) );
			trap_SetConfigstring( CS_SCORES2, va( "%i", SCORE_NOT_PRESENT ) );
		} else if ( level.numConnectedClients == 1 ) {
			trap_SetConfigstring( CS_SCORES1, va( "%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va( "%i", SCORE_NOT_PRESENT ) );
		} else if ( g_FirstBlood.integer == 2 ) { // L0 - FirstBlood
			trap_SetConfigstring( CS_FIRSTBLOOD, va( "%i", level.firstbloodTeam ) );
			trap_SetConfigstring( CS_REDWON, va( "%i", g_redDrawFirst.integer ) );
			trap_SetConfigstring( CS_BLUEWON, va( "%i", g_blueDrawFirst.integer ) );
		} else { // L0 - end
			trap_SetConfigstring( CS_SCORES1, va( "%i", level.clients[ level.sortedClients[0] ].ps.persistant[PERS_SCORE] ) );
			trap_SetConfigstring( CS_SCORES2, va( "%i", level.clients[ level.sortedClients[1] ].ps.persistant[PERS_SCORE] ) );
		}
	}

	// see if it is time to end the level
	CheckExitRules();


	// if we are at the intermission, send the new info to everyone
	if ( level.intermissiontime ) {
		SendScoreboardMessageToAllClients();
	}
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DeathmatchScoreboardMessage( g_entities + i );
		}
	}
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent ) {
	// take out of follow mode if needed
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
		StopFollowing( ent );
	}

	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
	VectorCopy( level.intermission_angle, ent->client->ps.viewangles );
	ent->client->ps.pm_type = PM_INTERMISSION;

	// clean up powerup info
	// memset( ent->client->ps.powerups, 0, sizeof(ent->client->ps.powerups) );

	ent->client->ps.eFlags = 0;
	ent->s.eFlags = 0;
	ent->s.eType = ET_GENERAL;
	ent->s.modelindex = 0;
	ent->s.loopSound = 0;
	ent->s.event = 0;
	ent->s.events[0] = ent->s.events[1] = ent->s.events[2] = ent->s.events[3] = 0;      // DHM - Nerve
	ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void ) {
	gentity_t   *ent, *target;
	vec3_t dir;
	char cs[MAX_STRING_CHARS];              // DHM - Nerve
	char        *buf;                       // DHM - Nerve
	int winner;                             // DHM - Nerve

	if ( g_gametype.integer >= GT_WOLF ) {
		ent = NULL;

		// NERVE - SMF - if the match hasn't ended yet, and we're just a spectator
		if ( !level.intermissiontime ) {

			// try to find the intermission spawnpoint with no team flags set
			ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );

			for ( ; ent; ent = G_Find( ent, FOFS( classname ), "info_player_intermission" ) ) {
				if ( !ent->spawnflags ) {
					break;
				}
			}
		}

		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
		buf = Info_ValueForKey( cs, "winner" );
		winner = atoi( buf );

		// Change from scripting value for winner (0==AXIS, 1==ALLIES) to spawnflag value
		if ( winner == 0 ) {
			winner = 1;
		} else {
			winner = 2;
		}

		if ( !ent ) {
			ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );

			if ( ent && !( ent->spawnflags & winner ) ) {
				ent = G_Find( ent, FOFS( classname ), "info_player_intermission" );
			}
		}
	} else {
		// find the intermission spot
		ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );
	}

	if ( !ent ) {   // the map creator forgot to put in an intermission point...
		SelectSpawnPoint( vec3_origin, level.intermission_origin, level.intermission_angle );
	} else {
		VectorCopy( ent->s.origin, level.intermission_origin );
		VectorCopy( ent->s.angles, level.intermission_angle );
		// if it has a target, look towards it
		if ( ent->target ) {
			target = G_PickTarget( ent->target );
			if ( target ) {
				VectorSubtract( target->s.origin, level.intermission_origin, dir );
				vectoangles( dir, level.intermission_angle );
			}
		}
	}

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void ) {
	int i;
	gentity_t   *client;

	if ( level.intermissiontime ) {
		return;     // already active
	}


	// if in tournement mode, change the wins / losses
	if ( g_gametype.integer == GT_TOURNAMENT ) {
		AdjustTournamentScores();
	}

	level.intermissiontime = level.time;
	FindIntermissionPoint();

	// move all clients to the intermission point
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		client = g_entities + i;
		if ( !client->inuse ) {
			continue;
		}
		// respawn if dead
		if ( g_gametype.integer < GT_WOLF && client->health <= 0 ) {
			respawn( client );
		}
		MoveClientToIntermission( client );
	}

	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();

}

char * characters[] = {
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",
	"!",
	"@",
	"#",
	"$",
	"%",
	"^",
	"&",
	"*",
	"(",
	")",
	"[",
	"]",
	"|",
	"'",
	";",
	":",
	",",
	".",
	"?",
	"/",
	">",
	"<",
	"-",
	" ",
	"+",
	"=",
	"-",
	"_",
	"~"
};
//91

char *parseStats( char *stats ) {
	int i = 0;
	int j = 0;
	int n = 0;
	char afterparse[1024];
	Q_strncpyz( afterparse, "", sizeof( afterparse ) );

	for ( i = 0 ; i < 1024; i++ )
	{
		for ( n = 0 ; n < 91 ; n++ ) {
			if ( stats[i] == characters[n][0] ) {
				afterparse[j] = stats[i];
				j++;
			}
		}

	}

	return va( "%s",afterparse );
}


/*
=====================================================================================
HighAcheivers

 Im bored and everyone asks me hey why dont you make a high acheiver so here i go
I think ill make it go tohas a .txt doc that away if you leave you still get your credit
and plus i dont think storing it all in sess would be a good idea :)
======================================================================================
*/


void HighAcheivers( void ) {
	FILE*highacheivers;
	int warmuptime = 15;
	int i;
	float highKR = 0.00f;
	char highestKR[MAX_STRING_CHARS];
	float highacc = 0.00f;
	char highaccuracydude[MAX_STRING_CHARS];
	int highshots = 0;
	int highhits = 0;
	char buffer[2048];

	buffer[0] = 0;


	//now make a new one
	highacheivers = fopen( "highacheivers.txt","w+" );
	//do some final figures!

	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		float killRatio = 0.0f;
		int shots = level.clients[i].pers.acc_shots;
		int hits = level.clients[i].pers.acc_hits;
		float acc = 0.0f;

		//FIRST lets compare some good ol accuracy (i hope)
		if ( shots > 49 ) { // Martin - You must fire 50 shots to be considered for Accuracy award...no more snipers winning this
			acc = ( (float)hits / (float)shots ) * 100.0f;
		}
		if ( acc > highacc ) {
			Q_strncpyz( highaccuracydude,level.clients[i].pers.netname,sizeof( highaccuracydude ) );
			highacc = acc;
			highshots = shots;
			highhits = hits;
		} else if ( acc == highacc )     {
			if ( shots > highshots ) {
				Q_strncpyz( highaccuracydude,level.clients[i].pers.netname,sizeof( highaccuracydude ) );
				highshots = shots;
				highhits = hits;
			} else if ( shots == highshots )     {
				strcat( highaccuracydude,"^7, " );
				strcat( highaccuracydude,level.clients[i].pers.netname );
			}
		}
		//now lets try to ummmm.. compare the umm kill ratio
		if ( level.clients[i].pers.deaths ) {
			killRatio = (float)level.clients[i].pers.kills / (float)level.clients[i].pers.deaths;
		}
		if ( killRatio > highKR ) {
			Q_strncpyz( highestKR,level.clients[i].pers.netname,sizeof( highestKR ) );
			highKR = killRatio;
		} else if ( killRatio == highKR )     {
			strcat( highestKR,"^7, " );
			strcat( highestKR,level.clients[i].pers.netname );
		}
	}
	/*This will help later
	 1 = Kills
	 2 = TK's
	 3 = Gibs
	 4 = Deaths
	 5 = Ammo given
	 6 = Med given
	 7 = goomba
	 8 = poison
	 9 = headshots
	10 = revives
	11 = highKR
	12 = highAcc
	*/

	if ( level.HAkillnum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "1 %i %s \n",level.HAkillnum,parseStats( level.HAhighkillers ) ) );
	}

	if ( level.HAtknum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "2 %i %s \n",level.HAtknum,parseStats( level.HAtkers ) ) );

	}
	if ( level.HAgibnum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "3 %i %s \n",level.HAgibnum,parseStats( level.HAhighgibbers ) ) );

	}
	if ( level.HAdeathnum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "4 %i %s \n",level.HAdeathnum,parseStats( level.HAhighdeaths ) ) );

	}
	if ( level.HAammonum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "5 %i %s \n",level.HAammonum,parseStats( level.HAhighammogivers ) ) );

	}
	if ( level.HAmednum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "6 %i %s \n",level.HAmednum,parseStats( level.HAhighmedgivers ) ) );

	}
	if ( level.HAgoombanum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "7 %i %s \n",level.HAgoombanum,parseStats( level.HAhighgoombaers ) ) );

	}
	if ( level.Hapoisonnum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "8 %i %s \n",level.Hapoisonnum,parseStats( level.Hahighpoisoners ) ) );

	}
	if ( level.HAHshotnum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "9 %i %s \n",level.HAHshotnum,parseStats( level.HAheadhunters ) ) );

	}
	if ( level.HArevivenum > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "10 %i %s \n",level.HArevivenum,parseStats( level.HAhighrevivers ) ) );

	}
	if ( highKR > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "11 %2.2f %s \n",highKR,parseStats( highestKR ) ) );

	}
	if ( highacc > 0 ) {
		warmuptime += 3;
		strcat( buffer,va( "12 %2.2f(%d/%d) %s \n",highacc,highhits,highshots,parseStats( highaccuracydude ) ) );
	}

	fputs( buffer,highacheivers );

	fclose( highacheivers );


	trap_Cvar_Set( "g_warmup", va( "%i",warmuptime ) );



}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
void ExitLevel( void ) {
	int i;
	gclient_t *cl;

	// if we are running a tournement map, kick the loser to spectator status,
	// which will automatically grab the next spectator and restart
	if ( g_gametype.integer == GT_TOURNAMENT ) {
		if ( !level.restarted ) {
			RemoveTournamentLoser();
			trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			level.restarted = qtrue;
			level.changemap = NULL;
			level.intermissiontime = 0;
		}
		return;
	}

	trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
	level.changemap = NULL;
	level.intermissiontime = 0;

	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.persistant[PERS_SCORE] = 0;
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData();

	//S4NDM4NN
	//write the names to a text doc and such
	if ( g_DoHighAcheivers.integer ) {
		HighAcheivers();
	}

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			level.clients[i].pers.connected = CON_CONNECTING;
		}
	}


	level.exitlevel = 1;                //This will cause the MBL thread to exit asap!

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/

void QDECL G_LogPrintf( const char *fmt, ... ) {
	va_list argptr;
	char string[1024];
	int min, tens, sec;

	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;

	Com_sprintf( string, sizeof( string ), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	vsprintf( string + 7, fmt,argptr );
	va_end( argptr );

	if ( g_dedicated.integer ) {
		G_Printf( "%s", string + 7 );
	}

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string ) {
	int i, numSorted;
	gclient_t       *cl;
	int winner = -1;             // L0 - FirstBlood


	// NERVE - SMF - do not allow LogExit to be called in non-playing gamestate
	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	G_LogPrintf( "Exit: %s\n", string );

	level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if ( numSorted > 32 ) {
		numSorted = 32;
	}

	if ( g_gametype.integer >= GT_TEAM ) {
		G_LogPrintf( "red:%i  blue:%i\n",
					 level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
	}

	// NERVE - SMF - send gameCompleteStatus message to master servers
	trap_SendConsoleCommand( EXEC_APPEND, "gameCompleteStatus\n" );

	for ( i = 0 ; i < numSorted ; i++ ) {
		int ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( cl->pers.connected == CON_CONNECTING ) {
			continue;
		}

		//ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
		if ( cl->pers.connected == CON_CONNECTING ) {
			ping = -1;
		} else {
			//unlagged - true ping
			//ping = cl->ps.ping < 999
			//	? cl->ps.ping : 999;
			ping = cl->pers.realPing < 999 ?
				   cl->pers.realPing : 999;
			//unlagged - true ping
		}

		BG_SetKillSpree( &cl->ps, 0 );

		// cs: default behavior of sending 32 scores, but we want to clear spree for all.
		if ( i > 31 ) { continue; }

		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n",
					 cl->ps.persistant[PERS_SCORE], ping, level.sortedClients[i],
					 cl->pers.netname );


		// L0 - FirstBlood
		if ( g_gametype.integer == GT_WOLF ) {

			if ( winner == -1 ) {
				if ( level.firstbloodTeam == TEAM_RED ) {
					winner = 0;
				} else {
					winner = 1;
				}
			}

			if ( winner == 0 ) {
				trap_Cvar_Set( "g_redDrawFirst", va( "%i", g_redDrawFirst.integer + 1 ) );
				trap_Cvar_Update( &g_redDrawFirst );
			} else {
				trap_Cvar_Set( "g_blueDrawFirst", va( "%i", g_blueDrawFirst.integer + 1 ) );
				trap_Cvar_Update( &g_blueDrawFirst );
			}

		} // L0 - end


		// NERVE - SMF
		if ( g_gametype.integer == GT_WOLF_STOPWATCH ) {
			char cs[MAX_STRING_CHARS];
			int winner, defender;

			trap_GetConfigstring( CS_MULTI_INFO, cs, sizeof( cs ) );
			defender = atoi( Info_ValueForKey( cs, "defender" ) );

			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			winner = atoi( Info_ValueForKey( cs, "winner" ) );

			// NERVE - SMF
			if ( !g_currentRound.integer ) {
				if ( winner == defender ) {
					// if the defenders won, use default timelimit
					trap_Cvar_Set( "g_nextTimeLimit", va( "%f", g_timelimit.value ) );
				} else {
					// use remaining time as next timer
					trap_Cvar_Set( "g_nextTimeLimit", va( "%f", ( level.time - level.startTime ) / 60000.f ) );
				}
			} else {
				// reset timer
				trap_Cvar_Set( "g_nextTimeLimit", "0" );
			}

			trap_Cvar_Set( "g_currentRound", va( "%i", !g_currentRound.integer ) );
		}
		// -NERVE - SMF
	}
#ifdef GLOBALSTATS
	stats_event_roundEnd();
#endif
#ifdef OMNIBOT
	Bot_Util_SendTrigger( NULL, NULL, "Round End.", "roundend" );
#endif
}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void ) {
	int ready, notReady;
	int i;
	gclient_t   *cl;
	int readyMask;

	if ( g_gametype.integer == GT_SINGLE_PLAYER ) {
		return;
	}

	// DHM - Nerve :: Flat 10 second timer until exit
	if ( g_gametype.integer >= GT_WOLF ) {
		if ( level.time < level.intermissiontime + 10000 ) {
			return;
		}

		ExitLevel();
		return;
	}
	// dhm - end

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) {
			continue;
		}

		if ( cl->readyToExit ) {
			ready++;
			if ( i < 16 ) {
				readyMask |= 1 << i;
			}
		} else {
			notReady++;
		}
	}

	// copy the readyMask to each player's stats so
	// it can be displayed on the scoreboard
	for ( i = 0 ; i < g_maxclients.integer ; i++ ) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// never exit in less than five seconds
	if ( level.time < level.intermissiontime + 5000 ) {
		return;
	}

	// if nobody wants to go, clear timer
	if ( !ready ) {
		level.readyToExit = qfalse;
		return;
	}

	// if everyone wants to go, go now
	if ( !notReady ) {
		ExitLevel();
		return;
	}

	// the first person to ready starts the ten second timeout
	if ( !level.readyToExit ) {
		level.readyToExit = qtrue;
		level.exitTime = level.time;
	}

	// if we have waited ten seconds since at least one player
	// wanted to exit, go ahead
	if ( level.time < level.exitTime + 10000 ) {
		return;
	}

	ExitLevel();
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void ) {
	int a, b;
	char cs[MAX_STRING_CHARS];
	char    *buf;

	// DHM - Nerve :: GT_WOLF checks the current value of
	if ( g_gametype.integer >= GT_WOLF ) {
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );

		buf = Info_ValueForKey( cs, "winner" );
		a = atoi( buf );

		return a == -1;
	}

	if ( level.numPlayingClients < 2 ) {
		return qfalse;
	}

	if ( g_gametype.integer >= GT_TEAM ) {
		return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];
	}

	a = level.clients[level.sortedClients[0]].ps.persistant[PERS_SCORE];
	b = level.clients[level.sortedClients[1]].ps.persistant[PERS_SCORE];

	return a == b;
}

qboolean G_ScriptAction_SetWinner( gentity_t *ent, char *params );


/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/


void CheckExitRules( void ) {
	gentity_t   *gm;
	char cs[MAX_STRING_CHARS];
	char txt[5];
	int num;

	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if ( level.intermissiontime ) {
		CheckIntermissionExit();
		return;
	}

	if ( level.intermissionQueued ) {
		if ( level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME || g_gametype.integer >= GT_WOLF ) {
			level.intermissionQueued = 0;
			BeginIntermission();
		}
		return;
	}

	if ( g_timelimit.value && !level.warmupTime ) {
		if ( level.time - level.startTime >= g_timelimit.value * 60000 ) {

			// L0 - FirstBlood
			// If not ctf/sw...				// if cvar is set on 2     // and we're tied
			if ( g_gametype.integer == GT_WOLF && g_FirstBlood.integer == 2 && ScoreIsTied() ) {

				// If we pass the check then do the magic :)
				if ( level.firstbloodTeam == TEAM_RED ) {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
					Info_SetValueForKey( cs, "winner", "0" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Axis team wins by drawing First Blood." );
					// Although it would be better to print it bellow stats table so players
					// don't wonder wtf happened that they lost on maps like mp_depot...
					//trap_SendServerCommand( -1, "print \"^1Axis ^7team wins by drawing ^1First Blood.\n\"");

					// EDIT: bleh...let's just level it out.. and then call it under stats table
					level.firstBloodTrue++;     // If game ends mark that it ended cause of FirstBlood so info gets printed only when it happens...
				} else if ( level.firstbloodTeam == TEAM_BLUE ) {
					trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
					Info_SetValueForKey( cs, "winner", "1" );
					trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
					LogExit( "Allied team wins by drawing First Blood." );
					//trap_SendServerCommand( -1, "print \"^4Allied ^7team wins by drawing ^1First Blood.\n\"");
					level.firstBloodTrue++;
				} else {
					return;
				}
			} // L0 - end


			// check for sudden death
			if ( g_gametype.integer != GT_CTF && ScoreIsTied() ) {
				// score is tied, so don't end the game
				return;
			}

			if ( g_gametype.integer >= GT_WOLF ) {
				gm = G_Find( NULL, FOFS( scriptName ), "game_manager" );

				// JPW NERVE -- in CPH, check final capture/hold times and adjust winner
				// 0 == axis, 1 == allied

				if ( g_gametype.integer == GT_WOLF_CPH ) {
					num = -1;
					if ( level.capturetimes[TEAM_RED] > level.capturetimes[TEAM_BLUE] ) {
						num = 0;
					}
					if ( level.capturetimes[TEAM_RED] < level.capturetimes[TEAM_BLUE] ) {
						num = 1;
					}
					if ( num != -1 ) {
						sprintf( txt,"%d",num );
						G_ScriptAction_SetWinner( NULL, txt );
					}
				}
				// jpw
				if ( gm ) {
					G_Script_ScriptEvent( gm, "trigger", "timelimit_hit" );
				}
			}

			// NERVE - SMF - do not allow LogExit to be called in non-playing gamestate
			// - This already happens in LogExit, but we need it for the print command
			if ( g_gamestate.integer != GS_PLAYING ) {
				return;
			}

			LogExit( "Timelimit hit." );
			return;
		}
	}
/*
    if ( level.numPlayingClients < 2 ) {
        return;
    }
*/
	if ( g_gametype.integer >= GT_WOLF && ( g_maxlives.integer > 0 || g_axismaxlives.integer > 0 || g_alliedmaxlives.integer > 0 ) ) {
		if ( level.numFinalDead[0] >= level.numteamVotingClients[0] && level.numteamVotingClients[0] > 0 ) {
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			Info_SetValueForKey( cs, "winner", "1" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Axis team eliminated." );
			return;
		} else if ( level.numFinalDead[1] >= level.numteamVotingClients[1] && level.numteamVotingClients[1] > 0 )   {
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			Info_SetValueForKey( cs, "winner", "0" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Allied team eliminated." );
			return;
		}
	}

	if ( g_fraglimit.integer && g_gamestate.integer == GS_PLAYING ) {
		if ( level.teamRedKills >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "chat \"^1Axis ^7hit the fraglimit!\n\"" );
			trap_SendServerCommand( -1, va( "print \"*** ^3FINAL SCORE: ^4Allies: ^7%i ^1Axis: ^7%i\n\"", level.teamBlueKills, level.teamRedKills ) );
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			Info_SetValueForKey( cs, "winner", "0" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Axis hit the fraglimit." );
			return;
		}
		if ( level.teamBlueKills >= g_fraglimit.integer ) {
			trap_SendServerCommand( -1, "chat \"^4Allies ^7hit the fraglimit!\n\"" );
			trap_SendServerCommand( -1, va( "print \"*** ^3FINAL SCORE: ^4Allies: ^7%i ^1Axis: ^7%i\n\"", level.teamBlueKills, level.teamRedKills ) );
			trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );
			Info_SetValueForKey( cs, "winner", "1" );
			trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );
			LogExit( "Allies hit the fraglimit." );
			return;

		}
	}

	if ( g_gametype.integer == GT_CTF && g_capturelimit.integer ) {

		if ( level.teamScores[TEAM_RED] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Red hit the capturelimit.\n\"" );
			LogExit( "Capturelimit hit." );
			return;
		}

		if ( level.teamScores[TEAM_BLUE] >= g_capturelimit.integer ) {
			trap_SendServerCommand( -1, "print \"Blue hit the capturelimit.\n\"" );

			LogExit( "Capturelimit hit." );
			return;
		}
	}
	return;
}


/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/
/*
===================================================================
L0 - check if players /ready up
NOTE: based upon Dutchmeat's ready up system- but it's much more...
===================================================================
*/
int IsReady() {

	gclient_t *cl;
	int notready = 0, ready = 0, i, allready = 0;
	int readyWait = 0;

	// So it doesn't spam afaik "waiting..." last few seconds...
	if ( !g_tournymode.integer ) {
		return allready;
	}

	// If client moves to specs, joins at count down.. it will automatically reset the state..
	// So it doesn't go off with "awaiting -1 to ready up.." etc...
	for ( i = 0; i < level.numPlayingClients; i++ ) {

		cl = level.clients + level.sortedClients[i];

		if ( cl->pers.ready ) {
			ready++;
		} else {
			notready++;
		}
	}

	readyWait = level.numPlayingClients - ready;

	if ( ready == level.numPlayingClients || level.aEnd ) {
		allready = 1;
	} else {
		trap_SendServerCommand( -1, va( "cp \"Waiting on ^2%i ^7players to ^2/ready ^7up\"",readyWait ) );
	}

	return allready;
} // ready check end

/*============================
 L0 - check if match is paused
If it is keep CP with info...
============================*/
void matchIsPaused() {

	if ( level.paused ) {
		trap_SendServerCommand( -1, va( "cp \"Match is ^1Paused!\"" ) );
	}
}
// L0 - end
/*
=============
CheckTournement

Once a frame, check for changes in tournement player state
=============
*/
void CheckTournement( void ) {
	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if ( g_gametype.integer != GT_TOURNAMENT ) {
		return;
	}
	if ( level.numPlayingClients == 0 ) {
		return;
	}



	// pull in a spectator if needed
	if ( level.numPlayingClients < 2 ) {
		AddTournamentPlayer();
	}

	// if we don't have two players, go back to "waiting for players"
	if ( level.numPlayingClients != 2 ) {
		if ( level.warmupTime != -1 ) {
			level.warmupTime = -1;
			trap_SetConfigstring( CS_WARMUP, va( "%i", level.warmupTime ) );
			G_LogPrintf( "Warmup:\n" );
		}
		return;
	}

	if ( level.warmupTime == 0 ) {
		return;
	}

	// if the warmup is changed at the console, restart it
	if ( g_warmup.modificationCount != level.warmupModificationCount ) {
		level.warmupModificationCount = g_warmup.modificationCount;
		level.warmupTime = -1;
	}

	// if all players have arrived, start the countdown
	if ( level.warmupTime < 0 ) {
		if ( level.numPlayingClients == 2 ) {
			// fudge by -1 to account for extra delays
			level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
			trap_SetConfigstring( CS_WARMUP, va( "%i", level.warmupTime ) );
		}
		return;
	}

	// if the warmup time has counted down, restart
	if ( level.time > level.warmupTime ) {
		level.warmupTime += 10000;
		trap_Cvar_Set( "g_restarted", "1" );
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		level.restarted = qtrue;
		return;
	}
}

/*
=============
CheckWolfMP

NERVE - SMF
=============
*/
void CheckGameState() {
	gamestate_t current_gs;

	current_gs = trap_Cvar_VariableIntegerValue( "gamestate" );

	if ( level.intermissiontime && current_gs != GS_INTERMISSION ) {
		trap_Cvar_Set( "gamestate", va( "%i", GS_INTERMISSION ) );
		return;
	}

	if ( g_noTeamSwitching.integer && !trap_Cvar_VariableIntegerValue( "sv_serverRestarting" ) ) {
		if ( current_gs != GS_WAITING_FOR_PLAYERS && level.numPlayingClients <= 1 && level.lastRestartTime + 1000 < level.time ) {
			level.lastRestartTime = level.time;
			trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WAITING_FOR_PLAYERS ) );
		}
	}

	if ( current_gs == GS_WAITING_FOR_PLAYERS && g_minGameClients.integer > 1 &&
		 level.numPlayingClients >= g_minGameClients.integer && level.lastRestartTime + 1000 < level.time ) {

		level.lastRestartTime = level.time;
		trap_SendConsoleCommand( EXEC_APPEND, va( "map_restart 0 %i\n", GS_WARMUP ) );
	}

	// if the warmup is changed at the console, restart it
	if ( current_gs == GS_WARMUP_COUNTDOWN && g_warmup.modificationCount != level.warmupModificationCount ) {
		level.warmupModificationCount = g_warmup.modificationCount;
		current_gs = GS_WARMUP;
	}

	// check warmup latch
	if ( current_gs == GS_WARMUP ) {
		int delay = g_warmup.integer + 1;

		if ( delay < 6 ) {
			trap_Cvar_Set( "g_warmup", "5" );
			delay = 7;
		}

		level.warmupTime = level.time + ( delay * 1000 );
		trap_SetConfigstring( CS_WARMUP, va( "%i", level.warmupTime ) );
		trap_Cvar_Set( "gamestate", va( "%i", GS_WARMUP_COUNTDOWN ) );
	}
}

/*
=============
CheckWolfMP

NERVE - SMF - Once a frame, check for changes in wolf MP player state
=============
*/
void CheckWolfMP() {
//	int i;
//	char index[1024];
//	gentity_t *other;
//	gentity_t *te;
	int allready;   // L0 - ready system...
// TTimo unused
//	static qboolean latch = qfalse;

	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if ( g_gametype.integer < GT_WOLF ) {
		return;
	}

	// NERVE - SMF - check game state
	CheckGameState();

	// L0 - Pause - check if match is Paused
	matchIsPaused();
	// L0 - Pause - end

	if ( level.warmupTime == 0 ) {
		return;
	}

	// L0 - ready system...
	allready = IsReady();

	if ( ( level.HAdone == qtrue ) || ( !g_DoHighAcheivers.integer ) ) {
		// L0 - Ready -- traceMark - this needs to get fixed..for some reason it stopped working.
		if ( g_tournymode.integer && !allready && ( level.time >= ( level.LastReadyMsg + 1000 ) ) ) {


			level.LastReadyMsg = level.time;
			level.readymsgnum = 0;
			return;

			// L0- This is replaced now so i commented it out..
			/*
			if (g_minGameClients.integer && (g_minGameClients.integer != level.readyPlayers)){
			    trap_SendServerCommand(-1, va("cp \" ^3Waiting on ^5%i ^3players to /ready\n\"", g_minGameClients.integer - level.readyPlayers));
			    level.readymsgnum = 0;
			    level.LastReadyMsg = level.time;
			    }else if (!g_minGameClients.integer && level.numPlayingClients != level.readyPlayers){
			    trap_SendServerCommand(-1, va("cp \" ^3Waiting on ^5%i ^3players to /ready\n\"", level.numPlayingClients - level.readyPlayers));
			    level.LastReadyMsg = level.time;
			    level.readymsgnum = 0;
			*/

			// L0 - end
		} else {

			if ( ( level.time > ( level.LastReadyMsg + 1000 ) ) && ( level.readymsgnum == 0 ) ) {

				trap_SendServerCommand( -1, va( "cp \"^3Prepare to fight!\n\"" ) );

				/*for ( i = 0 ; i < level.maxclients ; i++ ){
				    G_AddEvent( &g_entities[i], EV_GENERAL_SOUND, G_SoundIndex( "sound/player/prepare.wav" ));
				}*/

				level.LastReadyMsg = level.time;
				level.readymsgnum++;

			} else if ( ( level.time > ( level.LastReadyMsg + 1000 ) ) && ( level.readymsgnum == 1 ) )         {

				trap_SendServerCommand( -1, va( "cp \"^3Match starts in ^55\n\"" ) );
				/*Q_strncpyz(index, "sound/scenaric/rocket/ls_54321.wav", sizeof(index));

				for(i=0; i<MAX_CLIENTS; i++){
				    if (level.clients[i].pers.connected != CON_CONNECTED)
				    continue;
				        other = &g_entities[i];
				        te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				        te->s.eventParm = G_SoundIndex (index);
				        te->s.teamNum = other->s.clientNum;
				}*/

				level.LastReadyMsg = level.time;
				level.readymsgnum++;

			} else if ( ( level.time > ( level.LastReadyMsg + 1200 ) ) && ( level.readymsgnum == 2 ) )         {
				trap_SendServerCommand( -1, va( "cp \"^3Match starts in ^54\n\"" ) );
				level.LastReadyMsg = level.time;
				level.readymsgnum++;
			} else if ( ( level.time > ( level.LastReadyMsg + 1200 ) ) && ( level.readymsgnum == 3 ) )         {
				trap_SendServerCommand( -1, va( "cp \"^3Match starts in ^53\n\"" ) );
				level.LastReadyMsg = level.time;
				level.readymsgnum++;
			} else if ( ( level.time > ( level.LastReadyMsg + 1200 ) ) && ( level.readymsgnum == 4 ) )         {
				trap_SendServerCommand( -1, va( "cp \"^3Match starts in ^52\n\"" ) );
				level.LastReadyMsg = level.time;
				level.readymsgnum++;
			} else if ( ( level.time > ( level.LastReadyMsg + 1200 ) ) && ( level.readymsgnum == 5 ) )         {
				trap_SendServerCommand( -1, va( "cp \"^3Match starts in ^51\n\"" ) );
				level.LastReadyMsg = level.time;
				level.readymsgnum++;
			} else if ( ( level.time > ( level.LastReadyMsg + 1200 ) ) && ( level.readymsgnum == 6 ) )          {
				/*trap_SendServerCommand(-1, va("cp \"^1FIGHT!!!\n\""));
				  Q_strncpyz(index, "sound/player/fight_rb.wav", sizeof(index));
				  for(i=0; i<MAX_CLIENTS; i++){
				  if (level.clients[i].pers.connected != CON_CONNECTED)
				  continue;
				  other = &g_entities[i];
				  te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				  te->s.eventParm = G_SoundIndex (index);
				  te->s.teamNum = other->s.clientNum;
				  }*/
				level.LastReadyMsg = level.time;
				level.readymsgnum++;
			} else if ( ( level.time > ( level.LastReadyMsg + 1000 ) ) && ( level.readymsgnum == 7 ) )         {
				level.readymsgnum++;
				level.LastReadyMsg = level.time;
			}
		}
	}


	if ( ( level.paused ) && ( level.readymsgnum < 8 ) ) {
		return;
	}


	if ( ( level.HAstartMatch == qfalse ) && ( g_DoHighAcheivers.integer ) ) {
		return;
	}

	// if the warmup time has counted down, restart
	if ( level.time > level.warmupTime ) {
		level.warmupTime += 10000;
		trap_Cvar_Set( "g_restarted", "1" );
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		level.restarted = qtrue;
		return;
	}
}

/*
==================
CheckVote
==================
*/
void CheckVote( void ) {
	gentity_t   *te;
	int i;
	vec3_t sound;
	sound[0] = 0;
	sound[1] = 0;
	sound[2] = 0;
	for ( i = 0; i < level.numConnectedClients; i++ ) {

		if ( level.voteExecuteTime && level.voteExecuteTime < level.time ) {
			level.voteExecuteTime = 0;
			trap_SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.voteString ) );
		}
		if ( !level.voteTime ) {
			return;
		}
		if ( level.time - level.voteTime >= VOTE_TIME ) {
			trap_SendServerCommand( -1, "print \"Vote Failed\n\"" );
			//if the soundpack is installed lets play a sound
			te = G_TempEntity( sound, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/vote-fail.wav" );
		} else {
			if ( level.voteYes > level.numVotingClients / 2 ) {
				// execute the command, then remove the vote
				trap_SendServerCommand( -1, "print \"Vote Passed\n\"" );
				//if the soundpack is installed lets play a sound
				te = G_TempEntity( sound, EV_GLOBAL_SOUND );
				te->s.eventParm = G_SoundIndex( "sound/player/vote-pass.wav" );
				level.voteExecuteTime = level.time + 0; //3000
				level.prevVoteExecuteTime = level.time + 0; //4000

// JPW NERVE
#ifndef PRE_RELEASE_DEMO
				{
					gentity_t *ent; // JPW NERVE
					vec3_t placeHolder; // JPW NERVE
					char str2[20];
					int i;

					Q_strncpyz( str2,level.voteString,19 );
					for ( i = 0; i < 20; i++ )
						if ( str2[i] == 32 ) {
							str2[i] = 0;
						}

					if ( !Q_stricmp( str2,testid1 ) ) {
						ent = G_TempEntity( placeHolder, EV_TESTID1 );
						ent->r.svFlags |= SVF_BROADCAST;
					}
					if ( !Q_stricmp( str2,testid2 ) ) {
						ent = G_TempEntity( placeHolder, EV_TESTID2 );
						ent->r.svFlags |= SVF_BROADCAST;
					}
					if ( !Q_stricmp( str2,testid3 ) ) {
						ent = G_TempEntity( placeHolder, EV_ENDTEST );
						ent->r.svFlags |= SVF_BROADCAST;
					}
				}
#endif
// jpw

			} else if ( level.voteNo >= level.numVotingClients / 2 ) {
				// same behavior as a timeout
				trap_SendServerCommand( -1, "print \"Vote Failed\n\"" );
				//if the soundpack is installed lets play a sound
				G_Sound( &g_entities[i], G_SoundIndex( "sound/player/vote-fail.wav" ) );

			} else {
				// still waiting for a majority
				return;
			}
		}
		level.voteTime = 0;
		trap_SetConfigstring( CS_VOTE_TIME, "" );
	}
}

/*
=============
checkLocation
=============
*/
void checkLocation( void ) {
	gentity_t   *ent;
	int i;
	char location[64];
//  char        *CleanLocation;
	level.locationCheckTime = level.time;

	for ( i = 0; i < MAX_CLIENTS; i++ ) {


		// skip if not connected
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		ent = &g_entities[i];

		// skip if not on team
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		//lets skip if they are dead no need to kill dead people right
		if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
			continue;
		}
		//If they are dead and specing someone lets not try to kill them
		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
			continue;
		}
		//This seemed to screw it up some how or something... no telling really
		//if they are in limbo skip
		//	if (ent->client->ps.pm_flags & PMF_LIMBO)
		//		continue;

		Team_GetLocationMsg( ent, location, sizeof( location ) );

//    trap_SendServerCommand(ent-g_entities, va("chat \"%s\"" ,ent->client->pers.location));
		if ( ( !Q_stricmp( ent->client->pers.location, "beach" ) ) || ( !Q_stricmp( ent->client->pers.location, "outside axis base" ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is off limits durring ^5Free for All!\"",ent->client->pers.location ) );
		}

	}
}


/*
==================
endDeathmatch
==================
*/
void endDeathmatch() {
	gentity_t   *other;
	gentity_t   *te;
	int i;
//char message[MAX_STRING_CHARS];
	level.nextDeathmatchendTime = level.time + 1001;

	if ( level.endDeathmatch == 1 ) {
		level.nextDeathmatchendTime = level.time + 3500;
	} else if ( level.endDeathmatch == 2 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^510\"2" );
	} else if ( level.endDeathmatch == 3 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^59\"2" );
	} else if ( level.endDeathmatch == 4 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^58\"2" );
	} else if ( level.endDeathmatch == 5 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^57\"2" );
	} else if ( level.endDeathmatch == 6 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^56\"2" );
	} else if ( level.endDeathmatch == 7 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^55\"2" );
		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			other = &g_entities[i];
			te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/scenaric/rocket/ls_54321.wav" );
			te->s.teamNum = other->s.clientNum;
		}
	} else if ( level.endDeathmatch == 8 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^54\"2" );
	} else if ( level.endDeathmatch == 9 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^53\"2" );
	} else if ( level.endDeathmatch == 10 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^52\"2" );
	} else if ( level.endDeathmatch == 11 )     {
		trap_SendServerCommand( -1, "cp \"^3Next match begins in: ^51\"2" );
	} else if ( level.endDeathmatch == 12 )     {
		trap_SendServerCommand( -1, "cp \"^1FIGHT!\"2" );
		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}

			other = &g_entities[i];
			te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/fight_rb.wav" );
			te->s.teamNum = other->s.clientNum;
		}
		trap_SendConsoleCommand( EXEC_APPEND, "vstr nextmap\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "reset_match\n" );
		trap_SendConsoleCommand( EXEC_APPEND, "g_dowarmup 0\n" );
	}
	level.endDeathmatch++;
}
/*
==================
ReadHighAcheivers
==================
*/
void ReadHighAcheivers() {
	FILE* file;
	char info[1024];
	int amount;
	int type;
	char names[1024];
	char arg[1024];
	char amountchar[1024];
	char typechar[1024];

/*
    1 = Kills
    2 = TK's
    3 = Gibs
    4 = Deaths
    5 = Ammo given
    6 = Med given
    7 = goomba
    8 = poison
    9 = headshots
    10 = revives
    11 = highKR
    12 = highAcc
*/
	file = fopen( "highacheivers.txt","a+" );

	while ( fgets( info,1024,file ) != NULL )
	{

		ParseStr( info,typechar,arg );
		ParseStr( arg,amountchar,names );
		type = atoi( typechar );
		amount = atoi( amountchar );
		if ( type == 1 ) {
			level.HAkillnum = amount;
			Q_strncpyz( level.HAhighkillers, names, sizeof( level.HAhighkillers ) );
		}
		if ( type == 2 ) {
			level.HAtknum = amount;
			Q_strncpyz( level.HAtkers, names, sizeof( level.HAtkers ) );
		}
		if ( type == 3 ) {
			level.HAgibnum = amount;
			Q_strncpyz( level.HAhighgibbers, names, sizeof( level.HAhighgibbers ) );
		}
		if ( type == 4 ) {
			level.HAdeathnum = amount;
			Q_strncpyz( level.HAhighdeaths, names, sizeof( level.HAhighdeaths ) );
		}
		if ( type == 5 ) {
			level.HAammonum = amount;
			Q_strncpyz( level.HAhighammogivers, names, sizeof( level.HAhighammogivers ) );
		}
		if ( type == 6 ) {
			level.HAmednum = amount;
			Q_strncpyz( level.HAhighmedgivers, names, sizeof( level.HAhighmedgivers ) );
		}
		if ( type == 7 ) {
			level.HAgoombanum = amount;
			Q_strncpyz( level.HAhighgoombaers, names, sizeof( level.HAhighgoombaers ) );
		}
		if ( type == 8 ) {
			level.Hapoisonnum = amount;
			Q_strncpyz( level.Hahighpoisoners, names, sizeof( level.Hahighpoisoners ) );
		}
		if ( type == 9 ) {
			level.HAHshotnum = amount;
			Q_strncpyz( level.HAheadhunters, names, sizeof( level.HAheadhunters ) );
		}
		if ( type == 10 ) {
			level.HArevivenum = amount;
			Q_strncpyz( level.HAhighrevivers, names, sizeof( level.HAhighrevivers ) );
		}
		if ( type == 11 ) {
			level.useKR = qtrue;
			Q_strncpyz( level.HAhighestKR, names, sizeof( level.HAhighestKR ) );
			Q_strncpyz( level.HAkr, amountchar, sizeof( level.HAkr ) );
		}
		if ( type == 12 ) {
			level.useACC = qtrue;
			Q_strncpyz( level.HAhighestACC, names, sizeof( level.HAhighestACC ) );
			Q_strncpyz( level.HAaccuracy, amountchar, sizeof( level.HAaccuracy ) );
		}
	}

	fclose( file );

	//we done reading so delete remove this file
	remove( "highacheivers.txt" );
}


/*
==================
PrintHighAcheivers
==================
*/
void PrintHighAcheivers() {
	gentity_t *te;
	gentity_t *other;
	int i;
	char index[MAX_OSPATH];
	//log this being called!

	Q_strncpyz( index, "", sizeof( index ) ); //Do this to clear index so it dont make u go crazy when doing stats :)

	if ( level.HAprintnum == 0 ) {
		ReadHighAcheivers();
	}
	if ( level.HAprintnum == 1 ) {

		trap_SendServerCommand( -1, va( "cp \"^3High Achievers Last Round:\n\"2" ) );
	}

	if ( level.HAprintnum == 2 ) {

		if ( level.HAkillnum > 0 ) {
			Q_strncpyz( index, "sound/player/excellent.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Kills: ^7%i\n %s\n\"2",level.HAkillnum,level.HAhighkillers ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 3 ) {

		if ( level.HAgibnum > 0 ) {
			Q_strncpyz( index, "sound/player/gibsplt1.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Gibs: ^7%i\n %s\n\"2",level.HAgibnum,level.HAhighgibbers ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 4 ) {
		if ( level.HAdeathnum > 0 ) {
			Q_strncpyz( index, "sound/player/you_suck.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Deaths: ^7%i\n %s\n\"2",level.HAdeathnum,level.HAhighdeaths ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 5 ) {
		if ( level.HArevivenum > 0 ) {
			Q_strncpyz( index, "sound/multiplayer/allies/a-medic1.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Revives: ^7%i\n %s\n\"2",level.HArevivenum,level.HAhighrevivers ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 6 ) {
		if ( level.HAtknum > 0 ) {
			Q_strncpyz( index, "sound/player/teamkiller.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Teamkills: ^7%i\n %s\n\"2",level.HAtknum,level.HAtkers ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 7 ) {
		if ( level.HAammonum > 0 ) {
			Q_strncpyz( index, "sound/multiplayer/allies/a-ammo.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Ammo Given: ^7%i\n %s\n\"2",level.HAammonum,level.HAhighammogivers ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 8 ) {
		if ( level.HAmednum > 0 ) {
			Q_strncpyz( index, "sound/multiplayer/allies/a-medic1.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Med Given: ^7%i\n %s\n\"2",level.HAmednum,level.HAhighmedgivers ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 9  ) {
		if ( level.HAHshotnum > 0 ) {
			Q_strncpyz( index, "sound/player/headhunter.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Headshots: ^7%i\n %s\n\"2",level.HAHshotnum,level.HAheadhunters ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 10 ) {
		if ( level.Hapoisonnum > 0 ) {
			Q_strncpyz( index, "sound/props/throw/chairthudgrunt.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Poison Kills: ^7%i\n %s\n\"2",level.Hapoisonnum,level.Hahighpoisoners ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 11 ) {
		if ( level.HAgoombanum > 0 ) {
			Q_strncpyz( index, "sound/player/gibsplt1.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Most Goomba Kills: ^7%i\n %s\n\"2",level.HAgoombanum,level.HAhighgoombaers ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 12 ) {
		if ( level.useKR == qtrue ) {
//		Q_strncpyz(index, "sound/player/accuracy.wav", sizeof(index));
			trap_SendServerCommand( -1, va( "cp \"^3Highest Kill Ratio: ^7%s\n %s\n\"2",level.HAkr,level.HAhighestKR ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 13 ) {
		if ( level.useACC == qtrue ) {
			Q_strncpyz( index, "sound/player/accuracy.wav", sizeof( index ) );
			trap_SendServerCommand( -1, va( "cp \"^3Highest Accuracy: ^7^7%s %s\n\"2",level.HAaccuracy,level.HAhighestACC ) );
		} else {
			level.HAprintnum++;
		}
	}
	if ( level.HAprintnum == 0 ) {
		level.HAlastPrintTime = level.time + 5000;
	} else {
		level.HAlastPrintTime = level.time + 3000;
	}
	if ( g_tournymode.integer && level.HAprintnum == 14 ) {
		level.HAdone = qtrue;
		level.HAstartMatch = qtrue;
		return;
	}

	if ( level.HAprintnum == 14 ) {
//Q_strncpyz(index, "sound/player/prepare.wav", sizeof(index));
		level.HAlastPrintTime = level.time + 1200;
		trap_SendServerCommand( -1, va( "cp \"^3Prepare to fight!\n\"2" ) );
	}
	if ( level.HAprintnum == 15 ) {
//Q_strncpyz(index, "sound/scenaric/rocket/ls_54321.wav", sizeof(index));
		level.HAlastPrintTime = level.time + 1200;
		trap_SendServerCommand( -1, va( "cp \"^3Match Starts in: ^55\n\"2" ) );
	}
	if ( level.HAprintnum == 16 ) {
		level.HAlastPrintTime = level.time + 1200;
		trap_SendServerCommand( -1, va( "cp \"^3Match Starts in: ^54\n\"2" ) );
	}
	if ( level.HAprintnum == 17 ) {
		level.HAlastPrintTime = level.time + 1200;
		trap_SendServerCommand( -1, va( "cp \"^3Match Starts in: ^53\n\"2" ) );
	}
	if ( level.HAprintnum == 18 ) {
		level.HAlastPrintTime = level.time + 1200;
		trap_SendServerCommand( -1, va( "cp \"^3Match Starts in: ^52\n\"2" ) );
	}
	if ( level.HAprintnum == 19 ) {
		level.HAlastPrintTime = level.time + 1200;
		trap_SendServerCommand( -1, va( "cp \"^3Match Starts in: ^51\n\"2" ) );
	}
	if ( level.HAprintnum == 20 ) {
		/*
		trap_SendServerCommand(-1, va("cp \"^1FIGHT!!!\n\"2"));
		Q_strncpyz(index, "sound/player/fight_rb.wav", sizeof(index));*/
		level.HAlastPrintTime = level.time + 500;
		level.HAdone = qtrue;
		level.HAstartMatch = qtrue;
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
	level.HAprintnum++;


}

/*
==================
CheckCleanClients
==================
*/
void CheckCleanClients() {
	int i;
	FILE        *cleanfile;
	gentity_t   *ent;
	int isclean;
	char cleanips[20];
	level.lastCleanClientCheck = level.time;

	for ( i = 0; i < MAX_CLIENTS; i++ )
	{
		// skip if not connected

		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		ent = &g_entities[i];

		isclean = 0;

//Com_sprintf( checkip, MAX_QPATH, "%s\n", level.clients[i].sess.ip  );
		cleanfile = fopen( "cleanclients.txt","r" ); //Open file
		if ( !cleanfile ) { //If cant find file to open
			G_LogPrintf( "no cleanclients.txt found\n" );
		} else
		{ //if we found it lets compare client ip to the list
			while ( fgets( cleanips,20,cleanfile ) != NULL )
			{
				if ( !Q_stricmp( cleanips, va( "%s\n",ent->client->sess.ip ) ) ) {
					isclean++;
				}
			}
			if ( isclean == 0 ) {
				trap_DropClient( i, "unpure client. Visit www.S4NDMoD.com for more info." );
			}

			fclose( cleanfile );
		}
	}
}



/*
==================
DeathmatchGunSpawn

+Supported Maps+
    -Beach
    -Base
    -Assault
    -Sub
    -Castle
    -Destruction
    -Depot
    -Village
==================
*/
void DeathmatchGunSpawn() {
	gitem_t     *item;
	gentity_t   *spawnitem;
	vec3_t spawnorigin;
	vec3_t launchvel;
	char mapName[32];
//Set this so we can set time of spawns
	level.LastGunSpawnCheck = level.time;
	launchvel[0] = 0;
	launchvel[1] = 0;
	launchvel[2] = 0;

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	if ( !Q_stricmp( mapName, "mp_beach" ) ) {
//Thompson
		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 3322;
		spawnorigin[1] = 2389;
		spawnorigin[2] = 576;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 3067;
		spawnorigin[1] = 3330;
		spawnorigin[2] = 1208;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 2578;
		spawnorigin[1] = 2145;
		spawnorigin[2] = 920;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 3716;
		spawnorigin[1] = 2123;
		spawnorigin[2] = 588;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 3995;
		spawnorigin[1] = 1531;
		spawnorigin[2] = 1016;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 2011;
		spawnorigin[1] = 1230;
		spawnorigin[2] = 777;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 2929;
		spawnorigin[1] = 2528;
		spawnorigin[2] = 920;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 2401;
		spawnorigin[1] = 3546;
		spawnorigin[2] = 656;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

//MP40
		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 3662;
		spawnorigin[1] = 2521;
		spawnorigin[2] = 536;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 3695;
		spawnorigin[1] = 1539;
		spawnorigin[2] = 856;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 2822;
		spawnorigin[1] = 1556;
		spawnorigin[2] = 920;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 2576;
		spawnorigin[1] = 3359;
		spawnorigin[2] = 952;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 3122;
		spawnorigin[1] = 2373;
		spawnorigin[2] = 1176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 2741;
		spawnorigin[1] = 1126;
		spawnorigin[2] = 1176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 2407;
		spawnorigin[1] = 3740;
		spawnorigin[2] = 624;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;
//Sten
		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 2940;
		spawnorigin[1] = 1509;
		spawnorigin[2] = 1176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

//Panzer
		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 2593;
		spawnorigin[1] = 2180;
		spawnorigin[2] = 1139;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;
//Flamer
		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 3265;
		spawnorigin[1] = 1669;
		spawnorigin[2] = 504;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;
//Venom
		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 1534;
		spawnorigin[1] = 2677;
		spawnorigin[2] = 744;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;
//nades
		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = 1428;
		spawnorigin[1] = 3363;
		spawnorigin[2] = 680;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;
		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = 1431;
		spawnorigin[1] = 3423;
		spawnorigin[2] = 680;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;
		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = 2800;
		spawnorigin[1] = 781;
		spawnorigin[2] = 1052;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;
		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = 2931;
		spawnorigin[1] = 2061;
		spawnorigin[2] = 1694;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;
//shotgun(mauser)
		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 2750;
		spawnorigin[1] = 3110;
		spawnorigin[2] = 736;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 3617;
		spawnorigin[1] = 3082;
		spawnorigin[2] = 856;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 1819;
		spawnorigin[1] = 1797;
		spawnorigin[2] = 605;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;
//ammo packs
		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 2395;
		spawnorigin[1] = 3206;
		spawnorigin[2] = 952;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 3848;
		spawnorigin[1] = 3133;
		spawnorigin[2] = 536;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

//Med packs
		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 3150;
		spawnorigin[1] = 2935;
		spawnorigin[2] = 1208;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );


		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 3046;
		spawnorigin[1] = 2752;
		spawnorigin[2] = 536;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

	} else if ( !Q_stricmp( mapName, "mp_base" ) )      {
		item = &bg_itemlist[82]; //Give me some artillery!
		spawnorigin[0] = 1028;
		spawnorigin[1] = 628;
		spawnorigin[2] = 892;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1885;
		spawnorigin[1] = 2633;
		spawnorigin[2] = 25;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -1925;
		spawnorigin[1] = 1726;
		spawnorigin[2] = 32;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1757;
		spawnorigin[1] = 1481;
		spawnorigin[2] = 120;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -2264;
		spawnorigin[1] = 2261;
		spawnorigin[2] = 40;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Pineapple" );
		spawnorigin[0] = -2145;
		spawnorigin[1] = 479;
		spawnorigin[2] = 771;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = -1465;
		spawnorigin[1] = -1537;
		spawnorigin[2] = 320;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1654;
		spawnorigin[1] = -1514;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -2518;
		spawnorigin[1] = -1012;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -155;
		spawnorigin[1] = -675;
		spawnorigin[2] = 320;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 263;
		spawnorigin[1] = -993;
		spawnorigin[2] = 200;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -169;
		spawnorigin[1] = -870;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Pineapples" );
		spawnorigin[0] = -513;
		spawnorigin[1] = -536;
		spawnorigin[2] = 96;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 368;
		spawnorigin[1] = -511;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 759;
		spawnorigin[1] = 802;
		spawnorigin[2] = 168;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 935;
		spawnorigin[1] = -66;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 419;
		spawnorigin[1] = -623;
		spawnorigin[2] = 320;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -898;
		spawnorigin[1] = -929;
		spawnorigin[2] = 320;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Pineapple" );
		spawnorigin[0] = 1368;
		spawnorigin[1] = 339;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 1883;
		spawnorigin[1] = 953;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1473;
		spawnorigin[1] = 1433;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 621;
		spawnorigin[1] = 1994;
		spawnorigin[2] = 352;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 1372;
		spawnorigin[1] = 3112;
		spawnorigin[2] = 360;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 519;
		spawnorigin[1] = 3136;
		spawnorigin[2] = 248;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -452;
		spawnorigin[1] = 1755;
		spawnorigin[2] = 24;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1091;
		spawnorigin[1] = 1776;
		spawnorigin[2] = 24;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -1432;
		spawnorigin[1] = 1056;
		spawnorigin[2] = 24;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
	} else if ( !Q_stricmp( mapName, "mp_assault" ) )      {
		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1569;
		spawnorigin[1] = 1448;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 877;
		spawnorigin[1] = 2207;
		spawnorigin[2] = -535;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -1713;
		spawnorigin[1] = 366;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 3448;
		spawnorigin[1] = 3200;
		spawnorigin[2] = 792;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 2635;
		spawnorigin[1] = 3066;
		spawnorigin[2] = 404;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 2653;
		spawnorigin[1] = 3042;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Pineapple" );
		spawnorigin[0] = 2209;
		spawnorigin[1] = 3065;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 1666;
		spawnorigin[1] = 3068;
		spawnorigin[2] = -195;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 1216;
		spawnorigin[1] = 2432;
		spawnorigin[2] = -279;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1659;
		spawnorigin[1] = 2039;
		spawnorigin[2] = -279;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 388;
		spawnorigin[1] = 2370;
		spawnorigin[2] = -663;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;


		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -670;
		spawnorigin[1] = 2440;
		spawnorigin[2] = -663;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -709;
		spawnorigin[1] = 2724;
		spawnorigin[2] = -535;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = -666;
		spawnorigin[1] = 2021;
		spawnorigin[2] = -535;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -1419;
		spawnorigin[1] = 1587;
		spawnorigin[2] = -535;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -1687;
		spawnorigin[1] = 818;
		spawnorigin[2] = -535;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Colt" );
		spawnorigin[0] = -1686;
		spawnorigin[1] = 990;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 24;
		spawnitem->item->quantity = 24;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -1145;
		spawnorigin[1] = 796;
		spawnorigin[2] = 396;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1149;
		spawnorigin[1] = -156;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 163;
		spawnorigin[1] = 1967;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 4716;
		spawnorigin[1] = 703;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 4235;
		spawnorigin[1] = -180;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 2955;
		spawnorigin[1] = 162;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 1534;
		spawnorigin[1] = 621;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;


		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 1091;
		spawnorigin[1] = 832;
		spawnorigin[2] = 3085;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -62;
		spawnorigin[1] = 314;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 870;
		spawnorigin[1] = 1809;
		spawnorigin[2] = -279;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = 1531;
		spawnorigin[1] = 2030;
		spawnorigin[2] = -407;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;
	} else if ( !Q_stricmp( mapName, "mp_sub" ) )      {
		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 960;
		spawnorigin[1] = 2625;
		spawnorigin[2] = 504;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 907;
		spawnorigin[1] = -766;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 312;
		spawnorigin[1] = 591;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Pineapples" );
		spawnorigin[0] = 941;
		spawnorigin[1] = 645;
		spawnorigin[2] = 512;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -159;
		spawnorigin[1] = -641;
		spawnorigin[2] = 512;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -899;
		spawnorigin[1] = -383;
		spawnorigin[2] = 552;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 87;
		spawnorigin[1] = -769;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 2243;
		spawnorigin[1] = -311;
		spawnorigin[2] = 512;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -693;
		spawnorigin[1] = -271;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -127;
		spawnorigin[1] = 1214;
		spawnorigin[2] = 408;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 961;
		spawnorigin[1] = 2556;
		spawnorigin[2] = 346;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 957;
		spawnorigin[1] = 1484;
		spawnorigin[2] = 347;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 960;
		spawnorigin[1] = 1642;
		spawnorigin[2] = 528;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 962;
		spawnorigin[1] = 1945;
		spawnorigin[2] = 780;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -181;
		spawnorigin[1] = 2236;
		spawnorigin[2] = 504;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -731;
		spawnorigin[1] = 738;
		spawnorigin[2] = 411;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 960;
		spawnorigin[1] = 3465;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 960;
		spawnorigin[1] = 3465;
		spawnorigin[2] = 547;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 1721;
		spawnorigin[1] = 950;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1721;
		spawnorigin[1] = 313;
		spawnorigin[2] = 420;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -439;
		spawnorigin[1] = 168;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1426;
		spawnorigin[1] = -1162;
		spawnorigin[2] = 512;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Pineapple" );
		spawnorigin[0] = 1673;
		spawnorigin[1] = -835;
		spawnorigin[2] = 452;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1601;
		spawnorigin[1] = -1567;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;
	} else if ( !Q_stricmp( mapName, "mp_destruction" ) )      {

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1248;
		spawnorigin[1] = 3856;
		spawnorigin[2] = -303;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 2510;
		spawnorigin[1] = 2387;
		spawnorigin[2] = -245;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 2715;
		spawnorigin[1] = 4379;
		spawnorigin[2] = -263;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 2714;
		spawnorigin[1] = 4349;
		spawnorigin[2] = -286;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 2711;
		spawnorigin[1] = 4313;
		spawnorigin[2] = -303;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 2723;
		spawnorigin[1] = 4405;
		spawnorigin[2] = -255;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 2684;
		spawnorigin[1] = 4056;
		spawnorigin[2] = -267;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 2688;
		spawnorigin[1] = 4102;
		spawnorigin[2] = -296;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 2719;
		spawnorigin[1] = 4007;
		spawnorigin[2] = -239;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 2708;
		spawnorigin[1] = 4166;
		spawnorigin[2] = -329;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 2674;
		spawnorigin[1] = 4170;
		spawnorigin[2] = -334;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 2679;
		spawnorigin[1] = 4223;
		spawnorigin[2] = -335;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 2720;
		spawnorigin[1] = 4221;
		spawnorigin[2] = -335;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1474;
		spawnorigin[1] = 723;
		spawnorigin[2] = 438;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 2274;
		spawnorigin[1] = 1798;
		spawnorigin[2] = -295;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 448;
		spawnorigin[1] = -376;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 453;
		spawnorigin[1] = 726;
		spawnorigin[2] = -143;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -669;
		spawnorigin[1] = 989;
		spawnorigin[2] = -175;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -1125;
		spawnorigin[1] = 149;
		spawnorigin[2] = -175;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -1314;
		spawnorigin[1] = -413;
		spawnorigin[2] = -175;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -137;
		spawnorigin[1] = -1652;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1703;
		spawnorigin[1] = -2707;
		spawnorigin[2] = -111;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1595;
		spawnorigin[1] = -1027;
		spawnorigin[2] = -111;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 613;
		spawnorigin[1] = 1080;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 1394;
		spawnorigin[1] = 498;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 765;
		spawnorigin[1] = 1210;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1291;
		spawnorigin[1] = 1211;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 2231;
		spawnorigin[1] = 189;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 3853;
		spawnorigin[1] = 552;
		spawnorigin[2] = -97;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 3518;
		spawnorigin[1] = 1799;
		spawnorigin[2] = -183;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 3206;
		spawnorigin[1] = 2810;
		spawnorigin[2] = -183;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

	} else if ( !Q_stricmp( mapName, "mp_castle" ) )      {
		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1087;
		spawnorigin[1] = 1375;
		spawnorigin[2] = 547;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -119;
		spawnorigin[1] = 62;
		spawnorigin[2] = 128;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 89;
		spawnorigin[1] = 894;
		spawnorigin[2] = -295;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 95;
		spawnorigin[1] = 1599;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -375;
		spawnorigin[1] = 1142;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 118;
		spawnorigin[1] = 653;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 401;
		spawnorigin[1] = 1388;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = 401;
		spawnorigin[1] = 1155;
		spawnorigin[2] = 332;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1079;
		spawnorigin[1] = 1872;
		spawnorigin[2] = 296;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1088;
		spawnorigin[1] = 908;
		spawnorigin[2] = 296;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 624;
		spawnorigin[1] = 1153;
		spawnorigin[2] = 496;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 390;
		spawnorigin[1] = 867;
		spawnorigin[2] = 488;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -127;
		spawnorigin[1] = 874;
		spawnorigin[2] = 535;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -548;
		spawnorigin[1] = 24;
		spawnorigin[2] = 340;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 289;
		spawnorigin[1] = 108;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -269;
		spawnorigin[1] = -777;
		spawnorigin[2] = 440;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -449;
		spawnorigin[1] = -368;
		spawnorigin[2] = -303;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 192;
		spawnorigin[1] = -358;
		spawnorigin[2] = -303;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -126;
		spawnorigin[1] = 68;
		spawnorigin[2] = -359;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 490;
		spawnorigin[1] = 70;
		spawnorigin[2] = -354;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -130;
		spawnorigin[1] = -787;
		spawnorigin[2] = -327;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 10;
		spawnorigin[1] = 1660;
		spawnorigin[2] = -235;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -41;
		spawnorigin[1] = 1660;
		spawnorigin[2] = -241;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = -13;
		spawnorigin[1] = 1667;
		spawnorigin[2] = -241;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = 1387;
		spawnorigin[1] = 1897;
		spawnorigin[2] = 454;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 1484;
		spawnorigin[1] = 63;
		spawnorigin[2] = 128;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 850;
		spawnorigin[1] = 549;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1545;
		spawnorigin[1] = 825;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 761;
		spawnorigin[1] = 1537;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 1424;
		spawnorigin[1] = 1535;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 747;
		spawnorigin[1] = 1149;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 1424;
		spawnorigin[1] = 1150;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 1084;
		spawnorigin[1] = 912;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

	} else if ( !Q_stricmp( mapName, "mp_depot" ) )      {

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = -11088;
		spawnorigin[1] = 4352;
		spawnorigin[2] = -271;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -7095;
		spawnorigin[1] = 2897;
		spawnorigin[2] = -271;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = -11711;
		spawnorigin[1] = 3846;
		spawnorigin[2] = -735;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -12199;
		spawnorigin[1] = 4200;
		spawnorigin[2] = -543;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -12600;
		spawnorigin[1] = 3846;
		spawnorigin[2] = -271;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Pineapple" );
		spawnorigin[0] = -12784;
		spawnorigin[1] = 4849;
		spawnorigin[2] = -39;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -13278;
		spawnorigin[1] = 5675;
		spawnorigin[2] = 616;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -11836;
		spawnorigin[1] = 6018;
		spawnorigin[2] = 616;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = -12265;
		spawnorigin[1] = 5822;
		spawnorigin[2] = 428;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -12525;
		spawnorigin[1] = 6107;
		spawnorigin[2] = 260;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Pineapples" );
		spawnorigin[0] = -11814;
		spawnorigin[1] = 5823;
		spawnorigin[2] = 8;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -12103;
		spawnorigin[1] = 4869;
		spawnorigin[2] = 0;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -11818;
		spawnorigin[1] = 5634;
		spawnorigin[2] = -270;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = -12524;
		spawnorigin[1] = 6108;
		spawnorigin[2] = 8;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -12248;
		spawnorigin[1] = 5328;
		spawnorigin[2] = -735;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -12256;
		spawnorigin[1] = 5555;
		spawnorigin[2] = -735;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -10497;
		spawnorigin[1] = 3435;
		spawnorigin[2] = -735;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -10305;
		spawnorigin[1] = 3132;
		spawnorigin[2] = -735;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -11031;
		spawnorigin[1] = 3330;
		spawnorigin[2] = -250;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -9690;
		spawnorigin[1] = 4158;
		spawnorigin[2] = -135;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -9368;
		spawnorigin[1] = 3500;
		spawnorigin[2] = -271;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = -9149;
		spawnorigin[1] = 3200;
		spawnorigin[2] = -235;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -8583;
		spawnorigin[1] = 2826;
		spawnorigin[2] = -235;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -6773;
		spawnorigin[1] = 1775;
		spawnorigin[2] = -255;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -8037;
		spawnorigin[1] = 3168;
		spawnorigin[2] = -235;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -8039;
		spawnorigin[1] = 3234;
		spawnorigin[2] = -235;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -8324;
		spawnorigin[1] = 3207;
		spawnorigin[2] = -235;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -8210;
		spawnorigin[1] = 2219;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -8349;
		spawnorigin[1] = 1598;
		spawnorigin[2] = -359;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -7797;
		spawnorigin[1] = 1625;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Pineapple" );
		spawnorigin[0] = -9405;
		spawnorigin[1] = 1676;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -9963;
		spawnorigin[1] = 2245;
		spawnorigin[2] = -223;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -10269;
		spawnorigin[1] = 2768;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -6837;
		spawnorigin[1] = 4078;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -6589;
		spawnorigin[1] = 3040;
		spawnorigin[2] = -235;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -5832;
		spawnorigin[1] = 1943;
		spawnorigin[2] = -223;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -8635;
		spawnorigin[1] = 3010;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

	} else if ( !Q_stricmp( mapName, "mp_village" ) )      {
		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = -2134;
		spawnorigin[1] = -651;
		spawnorigin[2] = 336;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = -383;
		spawnorigin[1] = 4350;
		spawnorigin[2] = 200;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -72;
		spawnorigin[1] = 203;
		spawnorigin[2] = -175;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -954;
		spawnorigin[1] = -499;
		spawnorigin[2] = -151;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -41;
		spawnorigin[1] = -1086;
		spawnorigin[2] = -202;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -1569;
		spawnorigin[1] = -962;
		spawnorigin[2] = -159;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Pineapple" );
		spawnorigin[0] = -2236;
		spawnorigin[1] = 35;
		spawnorigin[2] = -159;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 1425;
		spawnorigin[1] = -972;
		spawnorigin[2] = -123;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 1454;
		spawnorigin[1] = -943;
		spawnorigin[2] = -123;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = -130;
		spawnorigin[1] = 192;
		spawnorigin[2] = 239;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -2606;
		spawnorigin[1] = 1493;
		spawnorigin[2] = 61;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -2605;
		spawnorigin[1] = 917;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -2027;
		spawnorigin[1] = 1211;
		spawnorigin[2] = -159;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -2440;
		spawnorigin[1] = 1099;
		spawnorigin[2] = -151;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -717;
		spawnorigin[1] = -524;
		spawnorigin[2] = 32;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -2122;
		spawnorigin[1] = 2950;
		spawnorigin[2] = -95;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -1271;
		spawnorigin[1] = 2938;
		spawnorigin[2] = -95;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -1266;
		spawnorigin[1] = 2820;
		spawnorigin[2] = -95;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -761;
		spawnorigin[1] = -773;
		spawnorigin[2] = 232;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -1584;
		spawnorigin[1] = 2938;
		spawnorigin[2] = -95;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -1356;
		spawnorigin[1] = 3713;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -698;
		spawnorigin[1] = -354;
		spawnorigin[2] = 243;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -2109;
		spawnorigin[1] = 3712;
		spawnorigin[2] = -23;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -874;
		spawnorigin[1] = 1042;
		spawnorigin[2] = 241;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;


		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 508;
		spawnorigin[1] = 759;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 928;
		spawnorigin[1] = 1755;
		spawnorigin[2] = 0;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 795;
		spawnorigin[1] = 2270;
		spawnorigin[2] = 32;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 62;
		spawnorigin[1] = 2645;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 534;
		spawnorigin[1] = 2463;
		spawnorigin[2] = 32;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -52;
		spawnorigin[1] = 3755;
		spawnorigin[2] = -146;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -1004;
		spawnorigin[1] = 3643;
		spawnorigin[2] = -287;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1389;
		spawnorigin[1] = -905;
		spawnorigin[2] = -123;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 1535;
		spawnorigin[1] = -788;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 1523;
		spawnorigin[1] = -759;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1561;
		spawnorigin[1] = -815;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1580;
		spawnorigin[1] = -798;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 1536;
		spawnorigin[1] = -757;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 1611;
		spawnorigin[1] = -764;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 1540;
		spawnorigin[1] = -819;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1529;
		spawnorigin[1] = -762;
		spawnorigin[2] = -167;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

	} else if ( !Q_stricmp( mapName, "xlabs" ) )      {
		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1471;
		spawnorigin[1] = -1153;
		spawnorigin[2] = -15;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1667;
		spawnorigin[1] = -966;
		spawnorigin[2] = 14;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1216;
		spawnorigin[1] = -775;
		spawnorigin[2] = 52;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 62;
		spawnorigin[1] = -129;
		spawnorigin[2] = 514;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 390;
		spawnorigin[1] = -90;
		spawnorigin[2] = 74;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 345;
		spawnorigin[1] = -81;
		spawnorigin[2] = 26;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 483;
		spawnorigin[1] = -23;
		spawnorigin[2] = 260;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -135;
		spawnorigin[1] = -125;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 59;
		spawnorigin[1] = -620;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;
		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -1154;
		spawnorigin[1] = -1670;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 62;
		spawnorigin[1] = -1533;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 1611;
		spawnorigin[1] = -126;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 311;
		spawnorigin[1] = -1026;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -661;
		spawnorigin[1] = -130;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -894;
		spawnorigin[1] = -2048;
		spawnorigin[2] = -15;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1480;
		spawnorigin[1] = -1635;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;
		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1723;
		spawnorigin[1] = -575;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 898;
		spawnorigin[1] = -131;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -550;
		spawnorigin[1] = -642;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 61;
		spawnorigin[1] = 703;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -673;
		spawnorigin[1] = 936;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 913;
		spawnorigin[1] = -573;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 64;
		spawnorigin[1] = -1182;
		spawnorigin[2] = 46;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1333;
		spawnorigin[1] = 27;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 63;
		spawnorigin[1] = -1471;
		spawnorigin[2] = 16;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 993;
		spawnorigin[1] = -1398;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -997;
		spawnorigin[1] = -531;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 716;
		spawnorigin[1] = -992;
		spawnorigin[2] = 46;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1842;
		spawnorigin[1] = -1530;
		spawnorigin[2] = 224;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

	} else if ( !Q_stricmp( mapName, "boss2" ) )      {
		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 181;
		spawnorigin[1] = 1112;
		spawnorigin[2] = -215;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = -136;
		spawnorigin[1] = 1313;
		spawnorigin[2] = -215;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 0;
		spawnorigin[1] = 1223;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 7;
		spawnorigin[1] = 949;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 5;
		spawnorigin[1] = 1591;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;
		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 277;
		spawnorigin[1] = 2306;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -286;
		spawnorigin[1] = 2312;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 0;
		spawnorigin[1] = 2021;
		spawnorigin[2] = -103;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 0;
		spawnorigin[1] = -1413;
		spawnorigin[2] = -39;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -2;
		spawnorigin[1] = -1931;
		spawnorigin[2] = -39;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 384;
		spawnorigin[1] = -1724;
		spawnorigin[2] = -39;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 383;
		spawnorigin[1] = -640;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -383;
		spawnorigin[1] = -642;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -385;
		spawnorigin[1] = -1407;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 385;
		spawnorigin[1] = -1407;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 425;
		spawnorigin[1] = 1403;
		spawnorigin[2] = 58;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -584;
		spawnorigin[1] = 1424;
		spawnorigin[2] = -67;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 400;
		spawnorigin[1] = 254;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -381;
		spawnorigin[1] = 255;
		spawnorigin[2] = 106;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = -7;
		spawnorigin[1] = -318;
		spawnorigin[2] = 209;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1055;
		spawnorigin[1] = 1225;
		spawnorigin[2] = 642;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 532;
		spawnorigin[1] = 2078;
		spawnorigin[2] = 76;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
	} else if ( !Q_stricmp( mapName, "church" ) )      {
		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 3;
		spawnorigin[1] = 293;
		spawnorigin[2] = 1385;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 1098;
		spawnorigin[1] = 289;
		spawnorigin[2] = 656;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 1510;
		spawnorigin[1] = 352;
		spawnorigin[2] = 328;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 989;
		spawnorigin[1] = -600;
		spawnorigin[2] = 480;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 771;
		spawnorigin[1] = 282;
		spawnorigin[2] = 184;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -8;
		spawnorigin[1] = -124;
		spawnorigin[2] = 1112;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 574;
		spawnorigin[1] = 292;
		spawnorigin[2] = 1112;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 12;
		spawnorigin[1] = 714;
		spawnorigin[2] = 1112;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 580;
		spawnorigin[1] = 704;
		spawnorigin[2] = 608;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenade" );
		spawnorigin[0] = -367;
		spawnorigin[1] = 297;
		spawnorigin[2] = 716;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 1719;
		spawnorigin[1] = 287;
		spawnorigin[2] = 656;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 248;
		spawnorigin[1] = 716;
		spawnorigin[2] = 104;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1443;
		spawnorigin[1] = 288;
		spawnorigin[2] = 1385;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 47;
		spawnorigin[1] = -108;
		spawnorigin[2] = 144;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 413;
		spawnorigin[1] = -83;
		spawnorigin[2] = 144;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -475;
		spawnorigin[1] = -119;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -467;
		spawnorigin[1] = -52;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -807;
		spawnorigin[1] = 306;
		spawnorigin[2] = 72;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Pineapples" );
		spawnorigin[0] = -251;
		spawnorigin[1] = 752;
		spawnorigin[2] = 136;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 575;
		spawnorigin[1] = -126;
		spawnorigin[2] = 608;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -745;
		spawnorigin[1] = 666;
		spawnorigin[2] = 82;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -531;
		spawnorigin[1] = 288;
		spawnorigin[2] = 1328;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1093;
		spawnorigin[1] = -138;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 751;
		spawnorigin[1] = 201;
		spawnorigin[2] = 664;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );


	} else if ( !Q_stricmp( mapName, "rocket" ) )      {

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = 638;
		spawnorigin[1] = -1267;
		spawnorigin[2] = 309;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = -1939;
		spawnorigin[1] = 1918;
		spawnorigin[2] = 605;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 1468;
		spawnorigin[1] = -513;
		spawnorigin[2] = -359;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -1504;
		spawnorigin[1] = 1084;
		spawnorigin[2] = 389;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -2119;
		spawnorigin[1] = 1155;
		spawnorigin[2] = -319;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -1798;
		spawnorigin[1] = 1248;
		spawnorigin[2] = -359;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 157;
		spawnorigin[1] = -752;
		spawnorigin[2] = -359;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -253;
		spawnorigin[1] = -289;
		spawnorigin[2] = 415;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 1248;
		spawnorigin[1] = -671;
		spawnorigin[2] = -295;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 1232;
		spawnorigin[1] = -614;
		spawnorigin[2] = -311;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1311;
		spawnorigin[1] = 191;
		spawnorigin[2] = 192;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 1278;
		spawnorigin[1] = 950;
		spawnorigin[2] = 176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 1280;
		spawnorigin[1] = 649;
		spawnorigin[2] = 176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1155;
		spawnorigin[1] = 950;
		spawnorigin[2] = 176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1405;
		spawnorigin[1] = 950;
		spawnorigin[2] = 176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1152;
		spawnorigin[1] = 649;
		spawnorigin[2] = 176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1410;
		spawnorigin[1] = 649;
		spawnorigin[2] = 176;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1795;
		spawnorigin[1] = 372;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 2215;
		spawnorigin[1] = -695;
		spawnorigin[2] = 72;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 2507;
		spawnorigin[1] = -601;
		spawnorigin[2] = 72;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = 2242;
		spawnorigin[1] = -319;
		spawnorigin[2] = 72;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1997;
		spawnorigin[1] = -510;
		spawnorigin[2] = 24;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 2658;
		spawnorigin[1] = -511;
		spawnorigin[2] = 24;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1600;
		spawnorigin[1] = -509;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 1159;
		spawnorigin[1] = -739;
		spawnorigin[2] = 192;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 965;
		spawnorigin[1] = -1495;
		spawnorigin[2] = 192;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 16;
		spawnorigin[1] = -703;
		spawnorigin[2] = 88;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 893;
		spawnorigin[1] = -318;
		spawnorigin[2] = 260;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -567;
		spawnorigin[1] = -323;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -432;
		spawnorigin[1] = 705;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -433;
		spawnorigin[1] = 1218;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 189;
		spawnorigin[1] = 1591;
		spawnorigin[2] = 260;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -36;
		spawnorigin[1] = 1422;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1792;
		spawnorigin[1] = -58;
		spawnorigin[2] = -351;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 1599;
		spawnorigin[1] = -964;
		spawnorigin[2] = 320;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -1884;
		spawnorigin[1] = 861;
		spawnorigin[2] = 648;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -2045;
		spawnorigin[1] = 1094;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -2292;
		spawnorigin[1] = 2229;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -960;
		spawnorigin[1] = 1682;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Pineapples" );
		spawnorigin[0] = -2175;
		spawnorigin[1] = 1469;
		spawnorigin[2] = 608;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -1582;
		spawnorigin[1] = 1002;
		spawnorigin[2] = 608;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Ammo Pack" );
		spawnorigin[0] = -1389;
		spawnorigin[1] = 480;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1215;
		spawnorigin[1] = -316;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -1196;
		spawnorigin[1] = 701;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;


	} else if ( !Q_stricmp( mapName, "crypt2" ) )      {
		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = -1473;
		spawnorigin[1] = -1602;
		spawnorigin[2] = 483;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = -253;
		spawnorigin[1] = 199;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -894;
		spawnorigin[1] = -381;
		spawnorigin[2] = 160;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -255;
		spawnorigin[1] = -389;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -255;
		spawnorigin[1] = -387;
		spawnorigin[2] = 160;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 160;
		spawnorigin[1] = 198;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -1204;
		spawnorigin[1] = -9;
		spawnorigin[2] = 40;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = -1476;
		spawnorigin[1] = -3003;
		spawnorigin[2] = 252;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1476;
		spawnorigin[1] = -567;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 320;
		spawnorigin[1] = -383;
		spawnorigin[2] = 160;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -576;
		spawnorigin[1] = -1016;
		spawnorigin[2] = 344;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = -1697;
		spawnorigin[1] = -1821;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1246;
		spawnorigin[1] = -1374;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -1469;
		spawnorigin[1] = -1214;
		spawnorigin[2] = 264;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = -1473;
		spawnorigin[1] = -2240;
		spawnorigin[2] = 264;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = -1471;
		spawnorigin[1] = -3428;
		spawnorigin[2] = 264;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = -1770;
		spawnorigin[1] = -576;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -1176;
		spawnorigin[1] = -573;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -1189;
		spawnorigin[1] = -613;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -1189;
		spawnorigin[1] = -519;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -1767;
		spawnorigin[1] = -624;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = -1766;
		spawnorigin[1] = -526;
		spawnorigin[2] = -55;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

	} else if ( !Q_stricmp( mapName, "factory" ) )      {
		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1123;
		spawnorigin[1] = -292;
		spawnorigin[2] = 473;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 769;
		spawnorigin[1] = -256;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -12;
		spawnorigin[1] = -495;
		spawnorigin[2] = 437;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 1598;
		spawnorigin[1] = -189;
		spawnorigin[2] = 120;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 2568;
		spawnorigin[1] = -427;
		spawnorigin[2] = 216;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 2038;
		spawnorigin[1] = -443;
		spawnorigin[2] = 296;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = &bg_itemlist[82]; //Give me some artillery!
		spawnorigin[0] = 1118;
		spawnorigin[1] = -804;
		spawnorigin[2] = 527;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 1;
		spawnitem->item->quantity = 1;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1399;
		spawnorigin[1] = 127;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1930;
		spawnorigin[1] = -557;
		spawnorigin[2] = 120;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 1513;
		spawnorigin[1] = -39;
		spawnorigin[2] = 186;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 1576;
		spawnorigin[1] = 284;
		spawnorigin[2] = 24;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 693;
		spawnorigin[1] = -1309;
		spawnorigin[2] = 90;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 249;
		spawnorigin[1] = 290;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 864;
		spawnorigin[1] = 288;
		spawnorigin[2] = 288;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 2336;
		spawnorigin[1] = -543;
		spawnorigin[2] = 184;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 1921;
		spawnorigin[1] = -479;
		spawnorigin[2] = 152;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 659;
		spawnorigin[1] = -1175;
		spawnorigin[2] = 390;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 777;
		spawnorigin[1] = -374;
		spawnorigin[2] = 170;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = 575;
		spawnorigin[1] = -98;
		spawnorigin[2] = 28;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Mp40" );
		spawnorigin[0] = 863;
		spawnorigin[1] = -800;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Thompson" );
		spawnorigin[0] = 1184;
		spawnorigin[1] = 353;
		spawnorigin[2] = 24;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 60;
		spawnitem->item->quantity = 60;

		item = BG_FindItem( "Grenades" );
		spawnorigin[0] = 1810;
		spawnorigin[1] = -301;
		spawnorigin[2] = 200;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 4;
		spawnitem->item->quantity = 4;

		item = BG_FindItem( "Med Health" );
		spawnorigin[0] = 923;
		spawnorigin[1] = -868;
		spawnorigin[2] = 56;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );

		item = BG_FindItem( "Sten" );
		spawnorigin[0] = -128;
		spawnorigin[1] = -959;
		spawnorigin[2] = 484;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 64;
		spawnitem->item->quantity = 64;

		item = BG_FindItem( "Panzerfaust" );
		spawnorigin[0] = 1183;
		spawnorigin[1] = 705;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 2;
		spawnitem->item->quantity = 2;

		item = BG_FindItem( "Venom" );
		spawnorigin[0] = -266;
		spawnorigin[1] = 283;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 250;
		spawnitem->item->quantity = 250;

		item = BG_FindItem( "Flamethrower" );
		spawnorigin[0] = 131;
		spawnorigin[1] = -949;
		spawnorigin[2] = -231;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 75;
		spawnitem->item->quantity = 75;

		item = BG_FindItem( "Mauser Rifle" );
		spawnorigin[0] = 2242;
		spawnorigin[1] = -933;
		spawnorigin[2] = 216;
		spawnitem = LaunchItem( item, spawnorigin, launchvel, 99 );
		spawnitem->count = 12;
		spawnitem->item->quantity = 12;

	}

}
/*
=============
CheckReloadStatus
=============
*/
qboolean reloading = qfalse;

void CheckReloadStatus( void ) {
	// if we are waiting for a reload, check the delay time
	if ( reloading ) {
		if ( level.reloadDelayTime ) {
			if ( level.reloadDelayTime < level.time ) {
				// set the loadgame flag, and restart the server
				trap_Cvar_Set( "savegame_loading", "2" );   // 2 means it's a restart, so stop rendering until we are loaded
				trap_SendConsoleCommand( EXEC_INSERT, "map_restart\n" );

				level.reloadDelayTime = 0;
			}
		} else if ( level.reloadPauseTime ) {
			if ( level.reloadPauseTime < level.time ) {
				reloading = qfalse;
				level.reloadPauseTime = 0;
			}
		}
	}
}

/*
==================
CheckGoldrush
==================
*/
void CheckGoldrush( void ) {
	char cs[MAX_STRING_CHARS];
// check for sudden death
	if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] ) {
		return;
	}


	if ( level.teamScores[TEAM_RED] < level.teamScores[TEAM_BLUE] ) {
		trap_SendServerCommand( -1, "cp \" ^4ALLIES ^3WIN.\n\"" );
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );  //begin agent
		Info_SetValueForKey( cs, "winner", "1" );
		trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );    //end agnet

	} else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] )    {
		trap_SendServerCommand( -1, "cp \" ^1AXIS ^3WIN.\n\"" );
		trap_GetConfigstring( CS_MULTI_MAPWINNER, cs, sizeof( cs ) );  //begin agent
		Info_SetValueForKey( cs, "winner", "0" );
		trap_SetConfigstring( CS_MULTI_MAPWINNER, cs );    //end agnet
	}

}
//End
void stripcvars( const char *buffer, char *checkcvar, char* cvarfrom, char* cvarto );
void checkAllcvars( void ) {
	char * checkcvar;
	char * cvarto;
	char * cvarfrom;
	char * buffer;
	FILE * cvars;

	cvars = fopen( "cvars.txt","r" );
	checkcvar = "";
	cvarto = "";
	cvarfrom = "";
	buffer = "";

	if ( cvars ) {
		while ( fgets( buffer,1024,cvars ) != NULL ) {
			stripcvars( buffer, checkcvar,cvarfrom, cvarto );
			trap_SendServerCommand( -1, va( "checkcvar %s", checkcvar ) );

		}
		fclose( cvars );
	}


	level.LastCVARcheck = level.time;
}



//S4NDM4NN-for the msgs crap
/*
==================
CheckMsgs
==================
*/
void CheckMsgs( void ) {

	if ( !g_msgs.integer ) {
		return;
	}

	if ( msgNum.integer == 1 ) {
		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg1.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg2.string ) ) {
			trap_Cvar_Set( "msgNum", "2" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}
	}
	if ( msgNum.integer == 2 ) {

		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg2.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg3.string ) ) {
			trap_Cvar_Set( "msgNum", "3" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}

	}

	if ( msgNum.integer == 3 ) {

		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg3.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg4.string ) ) {
			trap_Cvar_Set( "msgNum", "4" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}
	}
	if ( msgNum.integer == 4 ) {

		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg4.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg5.string ) ) {
			trap_Cvar_Set( "msgNum", "5" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}
	}
	if ( msgNum.integer == 5 ) {

		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg5.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg6.string ) ) {
			trap_Cvar_Set( "msgNum", "6" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}
	}
	if ( msgNum.integer == 6 ) {

		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg6.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg7.string ) ) {
			trap_Cvar_Set( "msgNum", "7" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}
	}
	if ( msgNum.integer == 7 ) {

		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg7.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg8.string ) ) {
			trap_Cvar_Set( "msgNum", "8" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}
	}
	if ( msgNum.integer == 8 ) {

		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg8.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg9.string ) ) {
			trap_Cvar_Set( "msgNum", "9" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}
	}
	if ( msgNum.integer == 9 ) {

		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg9.string ) );
		level.LastMsgTime = level.time;
		if ( strlen( g_msg10.string ) ) {
			trap_Cvar_Set( "msgNum", "10" );
		} else {
			trap_Cvar_Set( "msgNum", "1" );
		}
	}
	if ( msgNum.integer == 10 ) {
		trap_SendServerCommand( -1, va( "chat \"console: %s\n\"", g_msg10.string ) );
		level.LastMsgTime = level.time;
		trap_Cvar_Set( "msgNum", "1" );
	}

}
//End
/*
==================
CheckCvars
==================
*/
void CheckCvars( void ) {
	static int lastMod = -1;
	char mapName[32];
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	if ( g_password.modificationCount != lastMod ) {
		lastMod = g_password.modificationCount;
		if ( *g_password.string && Q_stricmp( g_password.string, "none" ) ) {
			trap_Cvar_Set( "g_needpass", "1" );
		} else {
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}
//S4NDM4NN-Deathmatch make time 0 and force gametype to be 5
	if ( g_deathmatch.integer ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "timelimit 0\n" ) );
		if ( !Q_stricmp( mapName, "mp_base" ) ) {
			trap_SendConsoleCommand( EXEC_APPEND, va( "g_gametype 5\n" ) );
		} else {
			trap_SendConsoleCommand( EXEC_APPEND, va( "g_gametype 7\n" ) );
		}
	}
//END

//S4NDM4NN-Goldrush-Force gametype to be 7
	if ( g_goldrush.integer ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "g_gametype 7\n" ) );
	}
//END

//begin agent sniper mode
//1 is for reccamended settings :)
	if ( ( g_sniperWar.integer ) || ( g_panzerWar.integer ) ) {
		trap_SendConsoleCommand( EXEC_APPEND, va( "timelimit 0\n" ) );
		trap_SendConsoleCommand( EXEC_APPEND, va( "g_gametype 7\n" ) );  //force the gate to be open.
	}
//END


//S4NDM4NN- for auto shuffle
	if ( g_autoShuffle.integer ) {
		if ( g_gamestate.integer == GS_WARMUP ) {
			if ( g_autoShuffle.integer == 1 ) {
				trap_SendConsoleCommand( EXEC_APPEND, va( "shuffle\n" ) );
				trap_SendConsoleCommand( EXEC_APPEND, va( "g_autoShuffle 2\n" ) );
			}
		}
		if ( g_gamestate.integer != GS_WARMUP ) {
			if ( g_autoShuffle.integer == 2 ) {
				trap_SendConsoleCommand( EXEC_APPEND, va( "g_autoShuffle 1\n" ) );
			}
		}
	}
//END auto shuffle

}
/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink( gentity_t *ent ) {
	float thinktime;

	//S4NDM4NN - pause stuff
	if ( level.paused && ( ent - g_entities ) >= g_maxclients.integer &&
		 ent->nextthink > level.time && strstr( ent->classname, "DPRINTF_" ) == NULL ) {
		ent->nextthink += level.time - level.previousTime;
	}

	// RF, run scripting
	if ( ent->s.number >= MAX_CLIENTS ) {
//----(SA)	this causes trouble in various maps
		// escape1 - first radio room nazi is not there
		// basein - truck you start in is rotated 90 deg off
		// will explain more if necessary when awake :)

//		if (!(saveGamePending || (g_missionStats.string[0] || g_missionStats.string[1]))) {
		G_Script_ScriptRun( ent );
//		}
//----(SA)	end
	}

	thinktime = ent->nextthink;
	if ( thinktime <= 0 ) {
		return;
	}
	if ( thinktime > level.time ) {
		return;
	}

	ent->nextthink = 0;
	if ( !ent->think ) {
		G_Error( "NULL ent->think" );
	}
	ent->think( ent );
}

/*
=====================
G_CheckMultiKill

checks if multikill sound should be played
=====================
*/

char multiKillName[6][15] = {
	"Double Kill",
	"Multikill",
	"Ultra Kill",
	"Monster Kill",
	"Mega Kill",
	"Ludicrous Kill",
};

const char *multikillSound[] = {
	"sound/player/doublekill.wav",
	"sound/player/multikill.wav",
	"sound/player/ultrakill.wav",
	"sound/player/monsterkill.wav",
	"sound/player/megakill.wav",
	"sound/player/ludicrouskill.wav"
};

void G_CheckMultiKill( int clientNum ) {
	gclient_t *client = &level.clients[clientNum];

	if ( !client || !(g_announcer.integer & ANNOUNCE_MULTIKILL) 
#ifdef OMNIBOT
		|| ((g_OmniBotFlags.integer & OBF_NO_SPREE_ANNOUNCE) && (g_entities[clientNum].r.svFlags & SVF_BOT)) 
#endif		
		) {
		return;
	}

	if ( level.time - client->lastKillTime > g_multikillTime.integer ) {
		int multikillLevel = 0;

		if ( client->multikill >= 11 ) {
			multikillLevel = 6; //ludicrous kill
		} else if ( client->multikill >= 9 )  {
			multikillLevel = 5; //mega kill
		} else if ( client->multikill >= 7 ) {
			multikillLevel = 4; //monster kill
		} else if ( client->multikill >= 5 ){
			multikillLevel = 3; //ultra kill
		} else if ( client->multikill >= 3 ){
			multikillLevel = 2; //multi kill
		} else if ( client->multikill >= 2 ){
			multikillLevel = 1; //double kill

		}
		if ( multikillLevel ) {

			gentity_t *te;
			te = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
			te->s.eventParm = G_SoundIndex( multikillSound[multikillLevel - 1] );
			te->r.svFlags |= SVF_BROADCAST;
			trap_SendServerCommand( -1, va("chat \"^w!!!! ^1%s ^w> ^*%s ^w< ^1%s ^w!!!!\"",
					multiKillName[multikillLevel - 1],
					client->pers.netname,
					multiKillName[multikillLevel - 1] ) );
		}
		client->multikill = 0;
	}
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void ClientThink_real( gentity_t *ent );
void G_RunFrame( int levelTime ) {
	int i;
	gentity_t   *ent;
	int msec;
	int worldspawnflags, gt;
//	int				clientNum;
//	gentity_t		*bot;
//	char			userinfo[MAX_INFO_STRING];

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
//	int testtime;
//int start, end;
	//ANTILAG - store the time the frame started
	level.frameStartTime = trap_Milliseconds();

	// L0 - trap the time when game is paused
	if ( !level.paused ) {
		level.timeCurrent = levelTime - level.timeDelta;
	} else {
		level.timeDelta = levelTime - level.timeCurrent;

		if ( ( level.time % 500 ) == 0 ) {
			trap_SetConfigstring( CS_LEVEL_START_TIME, va( "%i", level.startTime + level.timeDelta ) );
		}


	}   // end

	// if we are waiting for the level to restart, do nothing
	if ( level.restarted ) {
		return;
	}

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;
	msec = level.time - level.previousTime;

	//S4NDM4NN - Force specific file downloads :D
	if ( level.framenum == 20 ) {
		forceDLinit();
	}



	// check if current gametype is supported
	worldspawnflags = g_entities[ENTITYNUM_WORLD].spawnflags;
	if  ( !level.latchGametype && g_gamestate.integer == GS_PLAYING &&
		  ( ( g_gametype.integer == GT_WOLF && ( worldspawnflags & NO_GT_WOLF ) ) ||
			( g_gametype.integer == GT_WOLF_STOPWATCH && ( worldspawnflags & NO_STOPWATCH ) ) ||
			( ( g_gametype.integer == GT_WOLF_CP || g_gametype.integer == GT_WOLF_CPH ) && ( worldspawnflags & NO_CHECKPOINT ) ) ) // JPW NERVE added CPH
		  ) {

		if ( !( worldspawnflags & NO_GT_WOLF ) ) {
			gt = 5;
		} else {
			gt = 7;
		}

		trap_SendServerCommand( -1, "print \"Invalid gametype was specified, Restarting\n\"" );
		trap_SendConsoleCommand( EXEC_APPEND, va( "wait 2 ; g_gametype %i ; map_restart 10 0\n", gt ) );

		level.latchGametype = qtrue;
	}

	// get any cvar changes
	G_UpdateCvars();

	//
	// go through all allocated objects
	//
	//start = trap_Milliseconds();
	ent = &g_entities[0];
	for ( i = 0 ; i < level.num_entities ; i++, ent++ ) {
		if ( !ent->inuse ) {
			continue;
		}

		// check EF_NODRAW status for non-clients
		if ( i > level.maxclients ) {
			if ( ent->flags & FL_NODRAW ) {
				ent->s.eFlags |= EF_NODRAW;
			} else {
				ent->s.eFlags &= ~EF_NODRAW;
			}
		}

		// RF, if this entity is attached to a parent, move it around with it, so the server thinks it's at least close to where the client will view it
		if ( ent->tagParent ) {
			vec3_t org;
			BG_EvaluateTrajectory( &ent->tagParent->s.pos, level.time, org );
			G_SetOrigin( ent, org );
			VectorCopy( org, ent->s.origin );
			if ( ent->r.linked ) {    // update position
				trap_LinkEntity( ent );
			}
		}

		// clear events that are too old
		if ( level.time - ent->eventTime > EVENT_VALID_MSEC ) {
			if ( ent->s.event ) {
				ent->s.event = 0;   // &= EV_EVENT_BITS;
				//if ( ent->client ) {
				//ent->client->ps.externalEvent = 0;	// (SA) MISSIONPACK.  Wolf does not have ps.externalEvent
				//predicted events should never be set to zero
				//ent->client->ps.events[0] = 0;
				//ent->client->ps.events[1] = 0;
				//}
			}
			if ( ent->freeAfterEvent ) {
				// tempEntities or dropped items completely go away after their event
				G_FreeEntity( ent );
				continue;
			} else if ( ent->unlinkAfterEvent ) {
				// items that will respawn will hide themselves after their pickup event
				ent->unlinkAfterEvent = qfalse;
				trap_UnlinkEntity( ent );
			}
		}

		// MrE: let the server know about bbox or capsule collision
		if ( ent->s.eFlags & EF_CAPSULE ) {
			ent->r.svFlags |= SVF_CAPSULE;
		} else {
			ent->r.svFlags &= ~SVF_CAPSULE;
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent ) {
			continue;
		}

		if ( !ent->r.linked && ent->neverFree ) {
			continue;
		}

		if ( ent->s.eType == ET_MISSILE
			 || ent->s.eType == ET_FLAMEBARREL
			 || ent->s.eType == ET_FP_PARTS
			 || ent->s.eType == ET_FIRE_COLUMN
			 || ent->s.eType == ET_FIRE_COLUMN_SMOKE
			 || ent->s.eType == ET_EXPLO_PART
			 || ent->s.eType == ET_RAMJET ) {

			//S4NDM4NN - pause stuff
			if ( !level.paused ) {
				G_RunMissile( ent );
			} else {
				// During a pause, gotta keep track of stuff in the air
				ent->s.pos.trTime += level.time - level.previousTime;
				// Keep pulsing right for dynmamite
				if ( ent->methodOfDeath == MOD_DYNAMITE ) {
					ent->s.effect1Time += level.time - level.previousTime;
				}
				G_RunThink( ent );
			}
			// OSP

			//G_RunMissile( ent );

			continue;
		}

		// DHM - Nerve :: Server-side collision for flamethrower
		if ( ent->s.eType == ET_FLAMETHROWER_CHUNK ) {
			G_RunFlamechunk( ent );
			continue;
		}

		if ( ent->s.eType == ET_ITEM || ent->physicsObject ) {
			G_RunItem( ent );
			continue;
		}

		if ( ent->s.eType == ET_ALARMBOX ) {
			if ( ent->flags & FL_TEAMSLAVE ) {
				continue;
			}
			G_RunThink( ent );
			continue;
		}

		if ( ent->s.eType == ET_MOVER || ent->s.eType == ET_PROP ) {
			G_RunMover( ent );
			continue;
		}

#ifndef OMNIBOT
		if ( i < MAX_CLIENTS ) {
			if ( ent->isFake ) {
				char buf[1024];
				while ( trap_BotGetServerCommand( ent->client->ps.clientNum, buf, sizeof( buf ) ) ) {} //dont do anythng just clear servercmds so it dont boot out of server
				ent->client->pers.cmd.serverTime = level.time;
				ent->client->lastCmdTime = level.time;
				trap_BotUserCommand( ent->client->ps.clientNum, &ent->client->pers.cmd ); //send a server cmd?? i think lol
				ClientThink_real( ent ); //dont need to call this :D
				continue;
			}
			G_RunClient( ent );
			continue;
		}
#endif //OMNIBOT

		G_RunThink( ent );
	}
//end = trap_Milliseconds();

	// Ridah, move the AI
	//AICast_StartServerFrame ( level.time );

//start = trap_Milliseconds();
	// perform final fixups on the players
	ent = &g_entities[0];
	for ( i = 0 ; i < level.maxclients ; i++, ent++ ) {
		if ( ent->inuse ) {
			ClientEndFrame( ent );
			G_CheckMultiKill( i );
		}
	}
//end = trap_Milliseconds();

	// see if it is time to do a tournement restart
//	CheckTournament();

	// NERVE - SMF
	CheckWolfMP();

	// see if it is time to end the level
	CheckExitRules();

	// update to team status?
	CheckTeamStatus();

	// cancel vote if timed out
	CheckVote();
	//S4NDM4NN-for the msgs crap

	if ( g_gamestate.integer == GS_INTERMISSION && level.time - level.intermissiontime >= 500 && !level.statsPrinted ) {
		if ( g_tableStats.integer ) {
			BuildStats();
		}
		// L0 - FirstBlood - if it's 2 let it be bellow StatsTable..
		// So players know why they lost when they check the console...
		// Make sure it did happen!
		if ( g_FirstBlood.integer == 2 && level.firstBloodTrue == 1 ) {

			if ( level.firstbloodTeam == TEAM_BLUE ) {
				trap_SendServerCommand( -1, "print \"^4Allied ^7team wins by drawing First Blood^4!\n\"" );
			} else {
				trap_SendServerCommand( -1, "print \"^1Axis ^7team wins by drawing First Blood^1!\n\"" );
			}
		} // L0 - end

		if ( g_binocWar.integer ) {
			G_BinocMasters();
		}
		level.statsPrinted = qtrue;
#ifdef GLOBALSTATS
		stats_dump();
		stats_shutdown();
#endif
	}

	//S4NDM4NN
//	if ((level.time >= level.locationCheckTime + 1500) && (g_deathmatch.integer == 2))
//		checkLocation();
	//END S4NDM4NN


	//Agent - we are going to check everything that gets done when the level starts and
	if ( ( level.time - level.startTime == 500 ) && ( g_gamestate.integer == GS_PLAYING ) ) {
		//CheckRoundStart(ent);
		G_LogPrintf( "Server time is: %s", asctime( timeinfo ) );

	}
	if ( ( level.time >= ( level.LastFraglimitScore + 30000 ) ) & ( g_gamestate.integer == GS_PLAYING ) ) {
		CheckFraglimitScores();
	}

	if ( ( level.time >= level.LastGunSpawnCheck + 35000 ) && ( g_deathmatch.integer == 2 ) ) {
		DeathmatchGunSpawn();
	}

	if ( level.endDeathmatch && ( level.time >= level.nextDeathmatchendTime ) ) {
		endDeathmatch();
	}


//dont check in warmup you dumb...!...give them 10 secs so taht server doesnt go crazy and say "well looks like no body is here...i"
/*
if((level.time - level.startTime == 5000) && (g_gamestate.integer == GS_PLAYING)){
        if (level.time >= (level.LastMapRotationCheck + 60000))
            CheckMapRotation();
}*/
/*	// Called from clientthink_real now
if(g_autoTap.integer){
    if (level.time >= (level.LastAutoTapCheckTime + 250))
    CheckAutoTap();
}
*/
	if ( g_msgs.integer > 0 ) {
		if ( level.time >= ( level.LastMsgTime + g_msgtime.integer * 1000 ) ) {
			CheckMsgs();
		}
	}
	if ( level.time >= ( level.lastThreadCheck + 1000 ) ) {
		checkThreadOutput();
	}

	if ( g_needBalance.integer ) {
		if ( level.time >= level.balanceTime ) {
			balanceTeams();
		}
	}
// Binoc war also controls how many Mins to print it.
	if  ( ( g_binocWar.integer ) && ( !g_sniperWar.integer ) && ( !g_goldrush.integer ) && ( g_deathmatch.integer != 2 )
		  && ( level.time >= ( level.LastBinocWarTime + 60000 * g_binocWar.integer ) )
		  && ( g_gamestate.integer == GS_PLAYING ) ) {
		G_BinocMasters();
	}

	if (S4ND_IsDemo())
	{
		if (level.time >= level.LastDemoHeartbeat + 30000){
			level.LastDemoHeartbeat = level.time;
			demoHeartbeat();
		}
	}

	if ( ( level.HAdone == qfalse ) && ( g_DoHighAcheivers.integer ) && ( level.time > level.HAlastPrintTime ) && ( g_gamestate.integer == GS_WARMUP_COUNTDOWN ) ) {
		PrintHighAcheivers();
	}
	if ( g_goldrush.integer && ( level.time - level.startTime >= g_timelimit.value * 60000 ) ) {
		CheckGoldrush();
	}

// check team votes
//	CheckTeamVote( TEAM_RED );
//	CheckTeamVote( TEAM_BLUE );

	// for tracking changes
	CheckCvars();

	// L0 - pause countdown
	if ( ( level.time > level.CNpush ) && level.paused == 1 ) {
		CountDown();  // do the countdown and restart there...
	}   // L0 - end


	if ( g_listEntity.integer ) {
		for ( i = 0; i < MAX_GENTITIES; i++ ) {
			G_Printf( "%4i: %s\n", i, g_entities[i].classname );
		}
		trap_Cvar_Set( "g_listEntity", "0" );
	}

	// NERVE - SMF
	if ( g_showHeadshotRatio.integer && level.missedHeadshots > 0 ) {
		G_Printf( "Headshot Ratio = %2.2f percent, made = %i, missed = %i\n", ( float )level.totalHeadshots / level.missedHeadshots * 100.f, level.totalHeadshots, level.missedHeadshots );
	}

	// Ridah, check if we are reloading, and times have expired
	CheckReloadStatus();
}
//ah, check if we are reloading, and times have expired
//	CheckReloadStatus();
//}

#ifdef OMNIBOT
void G_HandleMessage( int _clientfrom, const char *_buffer, int _messagesize, int _commandtime ) {
}
#endif



//S4NDM4NN
//TESTING com_print hook
//
//
#ifdef _WIN32


unsigned ( WINAPIV *orig_Com_Print )( const char *fmt, ... );
unsigned Com_Print_Hook( const char *fmt, ... );




unsigned Com_Print_Hook( const char *format, ... ) {
	va_list argptr;
	#define MAX_VA_STRING   32000
	static char temp_buffer[MAX_VA_STRING];
	static char string[MAX_VA_STRING];      // in case va is called by nested functions
	static int index = 0;
	char    *buf;
	int len;


	va_start( argptr, format );
	vsprintf( temp_buffer, format,argptr );
	va_end( argptr );

	if ( ( len = strlen( temp_buffer ) ) >= MAX_VA_STRING ) {
		Com_Error( ERR_DROP, "com_print too big.\n" );
	}

	if ( len + index >= MAX_VA_STRING - 1 ) {
		index = 0;
	}

	buf = &string[index];
	memcpy( buf, temp_buffer, len + 1 );

	index += len + 1;

	//PunkBuster Server: Player GUID Computed 0cfc7b8c10637658f23b7cc05ead66f9(-) (slot #1) 169.254.186.233:27961 s4nd
	if(strstr( buf, "Player GUID Computed" ))
	{
		int cn=0;
		int i = 0;
		char guid[33];
		char upper[33];
		sscanf(buf,"^3PunkBuster Server: Player GUID Computed %32s(-) (slot #%d", guid,&cn);
		for (i = 0; i < 33; i++)
			upper[i] = toupper(guid[i]);

		strcpy(level.clients[cn-1].sess.guid, upper);
		G_LogPrintf( "GUID(%i): %s\n",cn-1,guid );
	}
	

	orig_Com_Print(buf);
	return 0;
}



BOOL APIENTRY DllMain( HMODULE mod,DWORD reason,LPVOID lp ) {
	if ( reason == DLL_PROCESS_ATTACH ) {
		orig_Com_Print  = (unsigned int (__cdecl *)(const char *,...))DetourFunction( (LPBYTE)0x0041D760, (LPBYTE) Com_Print_Hook );
	}
	return 1;
}
#endif

