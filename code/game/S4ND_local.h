#ifdef WIN32
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#ifdef GLOBALSTATS
#include "S4ND_stats.h"
#endif

#ifdef TEAMSPEAK
int startTeamspeakServer();
int stopTeamspeakServer();
#endif

#define USE_MBL

#define CROUCH_FASTSTAMINA	0x01
#define CROUCH_QUIET		0x02

//char S4ND_VERSION[];
//char S4ND_URL[];

static qboolean firstblood;
// L0
static qboolean firstheadshot;
// L0 - end

/*
===============
RTCW functions
===============
*/
//functions s4ndmod used but did not make themselves :)
void G_Say( gentity_t * ent, gentity_t * target, int mode, const char * chatText );
void MagicSink( gentity_t *self );
void SP_dlight( gentity_t * ent );
void limbo( gentity_t *ent, qboolean makeCorpse );
int G_GetWeaponDamage( int weapon );
/*
=================
S4NDMoD Functions
=================
*/
void CheckMapRotation();
void AutoWeaponsConfig( gentity_t* ent );
void SpawnTimer( gentity_t* ent );
void startSpawnTimer( gentity_t* ent );
void stopSpawnTimer( gentity_t* ent );
void AutoSpawnTimes( gentity_t* ent );
void CheckServer();
void demoHeartbeat();
void CheckFraglimitScores();
void CheckRoundStart( gentity_t* ent );
void DoAutoAim( gentity_t *ent );
void SP_CTF_flag( gentity_t* ent );
void CTF_touch( gentity_t *self, gentity_t *other, trace_t *trace );
void Cmd_MakeCustomLocation_f( gentity_t *ent );
void spawnCustomPathCorners();
void spawnCustomLocations();
void spawnCustomFlags(); //respawn flag
void SP_cust_spawnflag( gentity_t* ent );
void cust_spawnflag( gentity_t *self, gentity_t *other, trace_t *trace ); //end respawn
void HitSounds( gentity_t *ent,gentity_t *targ,qboolean isHeadshot );
void FindPlaneDir( gentity_t *ent, vec3_t dir );
void SpawnPlaneBombs( gentity_t *ent );
gentity_t *findradius( gentity_t *ent, vec3_t org, float rad );
int countmines( int team );
void Cmd_Spy_f( gentity_t *ent );
void easy_admin( gentity_t *ent );
void ParseStr( const char *strInput, char *strCmd, char *strArgs );
int ClientNumberFromNameMatch( char *name, int *matches );
void checkSpeclock( gentity_t *ent );
void DeathmatchCheckLocation( gentity_t *ent );
void SniperCheckLocation( gentity_t *ent );       //Agent
void StatsCheck( gentity_t *attacker );           //Agent -THIS IS THE NEW LINE
void Cmd_dropObj_f( gentity_t *ent );
void Cmd_ThrowKnife_f( gentity_t *ent );
//void Cmd_Push_f( gentity_t *ent );
void Cmd_Drag_f( gentity_t *ent );
void Cmd_KickAS_f( gentity_t *ent );
void Cmd_KickGrenade_f( gentity_t *ent );
void Cmd_giveAmmo_f( gentity_t* ent );
void G_CanisterKick( gentity_t *ent );
void FreeForAllSpawnpt( gentity_t *ent, vec3_t spawn_origin, vec3_t spawn_angles );
void SniperModeCustomSpawns( gentity_t *ent, vec3_t spawn_origin, vec3_t spawn_angles );
void clearCleanIp( gentity_t *ent );
void print_mod( gentity_t *attacker, gentity_t *self, int meansOfDeath );
void check_adminbot( gentity_t * attacker, gentity_t * self, int meansOfDeath ) ;
void CheckDeathmatch( gentity_t *attacker, gentity_t *self );
qboolean CheckAmmo( gentity_t *ent );
void GetLTinfo( gentity_t *ent );
void checkSpys( gentity_t *ent );
char *GetLevelTime();
void ForestPlanesThink( gentity_t *ent );
void spec_lock( gentity_t *ent );
void spec_unlock( gentity_t *ent );
void Kick_Cheats( gentity_t * ent );
void S4ND_nuke( gentity_t * ent );
void S4ND_spam( gentity_t * ent );
qboolean isMvsay( gentity_t * ent, char * arg1 );
qboolean S4ND_Spawn( gentity_t *ent );  //Agent's first file :D
void BuildStats();
char * parseStats( char * stats ) ;
void CheckAutoTap( gentity_t *ent );
qboolean isAirstrikeAvailable( gentity_t* ent );
qboolean canRetake();
int calculateStartLives( gentity_t* ent );
void showHitBoxes( gentity_t *ent );
qboolean isNameShort( gentity_t *ent );
void checkEvenTeams();
void countPlayers();
void balanceTeams();
void forceDLinit();
char *G_Shortcuts( gentity_t *ent, char *text );
void S4ND_InitGame();
qboolean S4ND_IsDemo();
void G_BinocMasters();
int QDECL G_SortPlayersByBinocs( const void* a, const void* b );
void updateUptime();
void setupCvars();
qboolean zbc_ZbotCheck( gentity_t *ent, usercmd_t *ucmd );    // anticheat
qboolean badUserinfo( int clientNum );      // Etpub RIP
qboolean hostnameBanCheck( int clientNum );
void setSeedPings( gentity_t* ent );
void modifyName( gentity_t *ent );
void stripGuid( char *in, char *out );


//void showHitBoxes();

//Commands
void Cmd_automg42_f( gentity_t * ent );
void Cmd_checkcvar_f( gentity_t *ent );
void Cmd_checkpure_f( gentity_t *ent );
void Cmd_Pmsg_f( gentity_t * ent );
void Cmd_Mvsay_f( gentity_t *ent );
void Cmd_MakeDmWeap_f( gentity_t *ent );
void Cmd_MakeDmPoint_f( gentity_t *ent );
void Cmd_Makepathcorner_f( gentity_t *ent );
void Cmd_Gib_f( gentity_t *ent );
void Cmd_Clientnum_f( gentity_t *ent );
void Cmd_NadePack_f( gentity_t *ent );
void Cmd_FindDMpoints_f( gentity_t *ent );
void Cmd_Playdead_f( gentity_t *ent );
void Cmd_Login_f( gentity_t *ent );
void Cmd_Logout_f( gentity_t *ent );
void Cmd_Thompson_f( gentity_t *ent );
void Cmd_MP40_f( gentity_t *ent );
void Cmd_Sten_f( gentity_t *ent );
void Cmd_Ready_f( gentity_t *ent );
void Cmd_NotReady_f( gentity_t *ent );
void Cmd_Smoke_f( gentity_t *ent );
void Cmd_mine_f( gentity_t *ent );
void Cmd_test_f( gentity_t *ent );
void Cmd_HighAcheivers_f( gentity_t *ent );
void Cmd_status_f( gentity_t *ent );
void Cmd_Accuracy_f( gentity_t *ent );
void Cmd_speclock_f( gentity_t *ent );
void Cmd_specunlock_f( gentity_t *ent );
void Cmd_allowspec_f( gentity_t *ent );
void Cmd_unallowspec_f( gentity_t *ent );
void Cmd_checkSpecs_f( gentity_t *ent );
void Cmd_ABHelp_f( gentity_t *ent );
void Cmd_Stats_f( gentity_t *ent );
void Cmd_Rules_f( gentity_t *ent );
void Cmd_Classes_f( gentity_t *ent );
void Cmd_Agent_Camera_f( gentity_t *ent );
void Cmd_OMW_f( gentity_t *ent );
qboolean G_PushPlayer( gentity_t *ent, gentity_t *victim );
void Cmd_Activate2_f( gentity_t *ent );
qboolean isRevivingMedic( gentity_t *self );
// AntiCheat
void Cmd_hackconfig_f( gentity_t *ent );
void Cmd_hack_cvar_f( gentity_t *ent, char *hack );
void Cmd_hack_cfg_f( gentity_t *ent );
// End
void Cmd_special1_f( gentity_t *ent );
void Cmd_special2_f( gentity_t *ent );
void QDECL S4ND_Log( const char *fmt, ... );
void QDECL Admin_Log( const char *fmt, ... );


//Server Commands
void Checkcvars( gentity_t *ent,char *cvar, int cvaris );
void Svcmd_Ignore_f();
void Svcmd_Test_f();
void Svcmd_Headshots_f();
void Svcmd_Unignore_f();
void Cmd_Lock_f();
void Cmd_Unlock_f();
void Svcmd_Kill_f();
void Svcmd_Slap_f();
void Svcmd_Shuffle_f();
void Svcmd_SortClan_f();
void Svcmd_ForceClan_f();
void Svcmd_banip_f();
void Svcmd_addbypass_f();
void Svcmd_Unbanip_f();
void Svcmd_CheckPure_f();

//some functions for a linked list
struct list {
	char data1[64];     //ipaddresses/hostnames will be stored here
	char data2[64];     //data2 will now be holding passwords and junk
	char data3[1024]; //this one holds the most info
	int type;       //type of data stored in data(ip/hostname)
	struct list *next;
	struct list *prev;
};

struct list *head, *tail;

//going to but all ban stuff here

int checkMBL( char*ip,char*password );
int checkBan( char*checkip,char*password );
void findAndRemoveNode( char*find,int list );
void rewriteBans();
void rewriteTempbans();
char* checkTempBans( char*ip );
void appendNode( char *data1,char *data2,char*data3,int type );
void removeNode( struct list *node );
void parseBanFiles();
void cleanTempBans();
void Svcmd_removeban_f();
void Svcmd_removebypass_f();
void Svcmd_removetempban_f();
void printList();
void listTempBans();
void Svcmd_tempban_f();
char* checkBanned( char*checkip,char*password );
/*
==============
S4NDMoD Cvars
==============
*/

extern vmCvar_t g_speclock;
//extern vmCvar_t		g_checkguid;
extern vmCvar_t g_medPacks;
extern vmCvar_t g_ammoPacks;
extern vmCvar_t g_nadePacks;
//extern vmCvar_t	    g_banGuids;
extern vmCvar_t g_goomba;
extern vmCvar_t g_poison;
extern vmCvar_t g_dropReload;
extern vmCvar_t g_reviveReports;
extern vmCvar_t g_tapReports;
extern vmCvar_t g_nameChangeReports;
extern vmCvar_t g_ltNades;
extern vmCvar_t g_medNades;
extern vmCvar_t g_engNades;
extern vmCvar_t g_soldNades;
extern vmCvar_t g_privateMsgs;
extern vmCvar_t g_minMsgChars;
extern vmCvar_t g_shove;
//extern vmCvar_t g_shoveAmount;
extern vmCvar_t g_shoveNoZ;
extern vmCvar_t g_kickableNades;
extern vmCvar_t g_kickableAS;
extern vmCvar_t g_kickerGetsOwner;
extern vmCvar_t g_gibReports;
extern vmCvar_t g_warmupDamage;
extern vmCvar_t g_unlockWeapons;
extern vmCvar_t g_pickupAmmo;
extern vmCvar_t g_ammoGivesSyringe;
extern vmCvar_t g_ammoGivesHelmet;
extern vmCvar_t g_ammoGivesBinocs;
extern vmCvar_t g_medExtraClips;
extern vmCvar_t g_ltExtraClips;
extern vmCvar_t g_engExtraClips;
extern vmCvar_t g_soldExtraClips;
extern vmCvar_t g_venomExtraClips;
extern vmCvar_t g_mauserExtraClips;
extern vmCvar_t g_pistolExtraClips;
extern vmCvar_t g_headshotDmg;
extern vmCvar_t g_helmetDmg;
extern vmCvar_t g_helmetProtects;
extern vmCvar_t g_teamSwitchTime;
extern vmCvar_t g_connectMsg;
extern vmCvar_t g_deathmatch;
extern vmCvar_t g_lateJoinLives;
extern vmCvar_t g_lateJoinTime;
extern vmCvar_t g_allowLateJoiners;
extern vmCvar_t g_maxPf;
extern vmCvar_t g_maxFlamer;
extern vmCvar_t g_maxMauser;
extern vmCvar_t g_maxVenom;
extern vmCvar_t g_alliedmaxlives;
extern vmCvar_t g_axismaxlives;
extern vmCvar_t g_ignoreSpecs;
extern vmCvar_t g_panzerArc;
extern vmCvar_t g_rocketSpeed;
extern vmCvar_t g_ASBlock;
extern vmCvar_t g_ASBlockEasy;
extern vmCvar_t g_disallowedVotes;
extern vmCvar_t g_lockteams;
extern vmCvar_t g_dropObj;
extern vmCvar_t g_dropNadepacks;
extern vmCvar_t g_throwableKnives;
extern vmCvar_t g_ammoGivesKnife;
extern vmCvar_t g_maxKnives;
extern vmCvar_t g_knifeDamage;
extern vmCvar_t g_wolfKick;
extern vmCvar_t g_dropClips;
extern vmCvar_t g_playDead;
extern vmCvar_t g_headshotsOnly;
extern vmCvar_t g_voteCredits;
extern vmCvar_t g_checkBan;
extern vmCvar_t g_chooseMg;
extern vmCvar_t g_mvsay;
extern vmCvar_t g_dragCorpse;
extern vmCvar_t g_underwaterMg;
extern vmCvar_t g_underwaterRevive;
extern vmCvar_t g_underwaterPistols;
extern vmCvar_t g_tournymode;
extern vmCvar_t g_forceWarmupSpawn;
extern vmCvar_t g_smokeGrenades;
extern vmCvar_t g_smokeGrenadeLimit;
extern vmCvar_t g_homingRockets;
extern vmCvar_t g_homingRocketsSens;
extern vmCvar_t g_panzerOverHeatTime;
extern vmCvar_t g_panzerClips;
extern vmCvar_t g_panzerCoolDownTime;
extern vmCvar_t g_panzerFireRate;
extern vmCvar_t g_extraPanzerClips;
extern vmCvar_t g_DmSpawnPoints;
extern vmCvar_t g_mines;
extern vmCvar_t g_smokingPlayers;
extern vmCvar_t g_goldrush;
extern vmCvar_t g_goldWeight;
extern vmCvar_t g_instaGib;
extern vmCvar_t g_nodrowning;
extern vmCvar_t g_homingrocketdetects;
extern vmCvar_t g_privateclients;
extern vmCvar_t g_autoShuffle;
extern vmCvar_t g_panzerWar;
extern vmCvar_t g_checkpureclient;
extern vmCvar_t g_makeDMspawns;
extern vmCvar_t g_DoHighAcheivers;
extern vmCvar_t g_LTinfomsg;
extern vmCvar_t g_allowSpys;
extern vmCvar_t g_reportSpys;
extern vmCvar_t g_antilag;
//extern vmCvar_t		g_antilagVersion;
extern vmCvar_t g_allowhitsounds;
extern vmCvar_t g_CaptureTheFlag;
extern vmCvar_t g_autoAim;
extern vmCvar_t g_jumpAccel;
extern vmCvar_t g_moonPhysics;
extern vmCvar_t g_automg42limit;
extern vmCvar_t g_allowCenterview;
extern vmCvar_t g_allowGiveAmmo;
extern vmCvar_t g_dropBinocs;
extern vmCvar_t g_packingPistols;
extern vmCvar_t g_antiwarp;
extern vmCvar_t g_shootableRocket;
extern vmCvar_t g_shootableGrenade;
extern vmCvar_t g_shootableMortar;
extern vmCvar_t g_shootableAS;
extern vmCvar_t g_statsUsername;        // S4ND_stats
extern vmCvar_t g_statsPassword;        // S4ND_stats
extern vmCvar_t g_statsEnable;          // S4ND_stats
extern vmCvar_t g_autoRestart;
extern vmCvar_t sv_punkbuster;
extern vmCvar_t sv_fps;
extern vmCvar_t sv_allowDownload;
extern vmCvar_t	sv_master5;
extern vmCvar_t	sv_maxPing;
extern vmCvar_t	sv_minPing;
extern vmCvar_t g_restartScript;
extern vmCvar_t	g_isDemoVersion;

#ifdef TEAMSPEAK
extern vmCvar_t ts_ip;
extern vmCvar_t ts_port;
#endif


extern vmCvar_t updatebanlist;
extern vmCvar_t g_SpamWar;

//for testing
extern vmCvar_t g_test1;
extern vmCvar_t g_test2;
extern vmCvar_t g_test3;
//For msgs
extern vmCvar_t g_msg1;
extern vmCvar_t g_msg2;
extern vmCvar_t g_msg3;
extern vmCvar_t g_msg4;
extern vmCvar_t g_msg5;
extern vmCvar_t g_msg6;
extern vmCvar_t g_msg7;
extern vmCvar_t g_msg8;
extern vmCvar_t g_msg9;
extern vmCvar_t g_msg10;
extern vmCvar_t g_msgs;
extern vmCvar_t g_msgtime;
extern vmCvar_t msgNum;
//Admin
extern vmCvar_t a1_pass;
extern vmCvar_t a2_pass;
extern vmCvar_t a3_pass;
extern vmCvar_t a4_pass;
extern vmCvar_t a5_pass;

extern vmCvar_t a1_cmds;
extern vmCvar_t a2_cmds;
extern vmCvar_t a3_cmds;
extern vmCvar_t a4_cmds;
extern vmCvar_t a5_cmds;
extern vmCvar_t a5_allPowers;

//The trenches
extern vmCvar_t g_theTrenchesMode;
//End

//End my cvars
//begin agent
extern vmCvar_t AB_Enable;
extern vmCvar_t AB_MinScore;
extern vmCvar_t AB_Tks;
//extern	vmCvar_t		AB_CheatsOK;
//extern	vmCvar_t		AB_CvarCheck;
extern vmCvar_t AB_Test;
extern vmCvar_t AB_AutoMaps;
extern vmCvar_t AB_MinNameChars;
extern vmCvar_t AB_MinHits;
extern vmCvar_t AB_MinHitsPercent;
//S4NDM4NN- just put them here so they will be with the rest
extern vmCvar_t AB_KnifePoints;
extern vmCvar_t AB_MgPoints;
extern vmCvar_t AB_MauserPoints;
extern vmCvar_t AB_Mg42Points;
extern vmCvar_t AB_ThrowKnivesPoints;
extern vmCvar_t AB_NadePoints;
extern vmCvar_t AB_AsPoints;
extern vmCvar_t AB_FlamerPoints;
extern vmCvar_t AB_VenomPoints;
extern vmCvar_t AB_PanzerPoints;
extern vmCvar_t AB_DynamitePoints;
extern vmCvar_t AB_PistolPoints;
extern vmCvar_t AB_SniperPoints;
extern vmCvar_t AB_MaxTkPoints;
extern vmCvar_t AB_AllowNameSpam;
//extern	vmCvar_t		AB_MaxPing;
//extern  vmCvar_t		AB_MaxPingTime;
//extern  vmCvar_t		AB_KickHighPingers;
extern vmCvar_t AB_TempBanTime;
//extern vmCvar_t g_RetakeLimit;          //fb retakes (not cp flags)
extern vmCvar_t g_sniperWar;
//extern vmCvar_t g_autoSniper;
extern vmCvar_t g_ShowHighlights;
extern vmCvar_t g_AutoSpawnTimes;
extern vmCvar_t g_AutoHeavyWeapons;
extern vmCvar_t g_autoTeamBalance;
extern vmCvar_t g_needBalance;
extern vmCvar_t g_RedDefaultSpawnTime;
extern vmCvar_t g_BlueDefaultSpawnTime;
extern vmCvar_t cl_allowdownload;
extern vmCvar_t g_ltExtraAirstrike;
extern vmCvar_t g_gibPoints;
extern vmCvar_t g_autoTap;
extern vmCvar_t g_bloodFlash;
//extern	vmCvar_t		Players_Allies;
//extern	vmCvar_t		Players_Axis;
//extern	vmCvar_t		Players_Spec;
extern vmCvar_t g_maxAirstrikes;
extern vmCvar_t g_showHitBoxes;
extern vmCvar_t g_fixedphysics;
extern vmCvar_t g_forceDLpaknames;
extern vmCvar_t g_forceDLchecksums;
extern vmCvar_t sv_referencedPaks;
extern vmCvar_t sv_referencedPakNames;
extern vmCvar_t g_tableStats;
extern vmCvar_t g_crouch;
extern vmCvar_t g_noSameTeamDefuse;
extern vmCvar_t g_truePing;
//extern	vmCvar_t		g_anticheat;
extern vmCvar_t g_shortcuts;
extern vmCvar_t g_binocWar;
extern vmCvar_t g_S4NDLog;
extern vmCvar_t g_S4NDdebug;
extern vmCvar_t g_allowspeclock;
extern vmCvar_t g_mineHealth;
//extern	vmCvar_t		g_mineArmAmt;
//extern	vmCvar_t		g_mineSplashRadius;
//extern	vmCvar_t		g_mineDetectDist;
extern vmCvar_t g_uptime;
extern vmCvar_t mod_url;
extern vmCvar_t gamename;
extern vmCvar_t g_fakePing;
extern vmCvar_t g_friendlyFireKnockback;
extern vmCvar_t g_spawnTimer;
extern vmCvar_t g_showLifeStats;
extern vmCvar_t g_undeadRevives;
extern vmCvar_t g_maxConnsPerIP;
extern vmCvar_t g_reducedScreenshake;
extern vmCvar_t g_multikillTime;
extern vmCvar_t g_announcer;

//extern	vmCvar_t		g_fixedphysicsfps;
//END S4NDM4NN
//end agent

// L0
extern vmCvar_t g_FirstBlood;        // 0 = off, 1 = spams only, 2 = in case if game is tied it gives the victory to team that made the 1st kill in game...
extern vmCvar_t g_redDrawFirst;      // fixed..
extern vmCvar_t g_blueDrawFirst;     // fixed..
extern vmCvar_t g_doubleKills;       // 0 = off, 1 = on
extern vmCvar_t g_showFlags;     // mcwf's GeoIP
// L0 - end


//S4NDM4NN - moved from g_spawn for agent
typedef struct {
	char    *name;
	void ( *spawn )( gentity_t *ent );
} spawn_t;

extern spawn_t spawns[]; // stop the gay errors...

int httpGet( char*url,char*filename );

// L0
void FirstBlood( gentity_t *self, gentity_t *attacker );
void FirstHeadshot( gentity_t *attacker, gentity_t *targ );
void DoubleKills( gentity_t *ent, int meansOfDeath );
void deathSpree( gentity_t *ent );
void Cmd_Pause_f( gentity_t *ent );
void Cmd_UnPause_f( gentity_t *ent );
void CountDown( void );
void PauseGame( void );
void Svcmd_Pause();
void Svcmd_UnPause();
void Cmd_incognito( gentity_t *ent );
void Cmd_Cancelvote_f( gentity_t *ent );
void Cmd_Passvote_f( gentity_t *ent );


//S4NDM4NN - Antiwarp
qboolean G_DoAntiwarp( gentity_t *ent );
void etpro_AddUsercmd( int clientNum, usercmd_t *cmd );
static float G_CmdScale( gentity_t *ent, usercmd_t *cmd );
void DoClientThinks( gentity_t *ent );

//for thread "messages"
struct messagelist {
	char command[16];   //adds what we need to do to the list.
	char data[256];                 //used for additional info as needed.
	int type;       //what "type" of message this is (in or out)
	struct messagelist *next; //next node in list
	struct messagelist *prev; //prev node in list
};


struct messagelist *mhead, *mtail;

void startWorkThread();
void endWorkThread();
char*iptohostname( char*ip );
void checkThreadInput();
void checkThreadOutput();
void removeMessage( struct messagelist *node );
void addMessage( char*command,char*data,int type );
void ReleaseLock();
void LockData();
void doWork( void*dummy );
#ifdef _WIN32
HANDLE workThreadLock;
HANDLE workThread;
DWORD workThreadID;
#else
pthread_mutex_t workThreadLock;
pthread_t workThread;
#endif

int exitThread;
int gettingMBL;


//g_crash.h
void EnableStackTrace();
void DisableStackTrace();

// L0 -GeoIP
typedef struct GeoIPTag {
	fileHandle_t GeoIPDatabase;
	unsigned char * cache;
	unsigned int memsize;
} GeoIP;

unsigned long GeoIP_addr_to_num( const char *addr );
unsigned int GeoIP_seek_record( GeoIP *gi, unsigned long ipnum );
void GeoIP_open( void );
void GeoIP_close( void );

extern GeoIP * gidb;
// end


//S4NDM4NN - detours shit
#ifdef _WIN32
extern unsigned ( WINAPIV *orig_Com_Print )( const char *fmt, ... );
extern unsigned Com_Print_Hook( const char *fmt, ... );
#endif