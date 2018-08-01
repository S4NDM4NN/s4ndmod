/*
Global Stats System

Please mark all comments in the format
// S4ND_stats Date Name Comment

Events list for statEntry struct:
    1 - Kill
        "1 <attackerCN> <attTeam> <attSpeed> <attWeapon> <attPosition> <attClass>
            <vicCN> <vicTeam> <vicSpeed> <vicWeapon> <vicPosition> <vicClass>
            ATTACKER of -1 means world killed them

    2 - Rename
        "2 <clientNum> <newName>"

    3 - Client Connect
        "3 <clientNum> <level.time> <ip> <name> <guid> "		//Don't send hostname...this can be calculated later

    4 - Client Disconnect
        "4 <clientNum> <level.time>"

    5 - Round Start											// Stores gametime, map, gametype, timelimit and swRound (if stopwatch mode)
        "5 <level.time> <map> <gametype> <timelimit> <swRound>

    6 - Round End
        "6 <level.time>"

    7 - Server settings										// Used to store server cvars
        TODO: Send this via encryption

    8 - Server stats username & PW
        "8 <serverIP> <serverPort> <serverName> <serverUsername> <serverPassword>"

    9 - Team Change
        "9 <clientNum> <newTeam> <level.time>"
            Teams:	1 - Axis, 2 - Allies, 3 - Spectator
            **NOTE:	Call this in client_begin also.

    10 - Send Client Data
        "10 <clientNum> <rest of data>"
*/
#ifdef GLOBALSTATS
#define TOTAL_STATS_WEAPONS 30

struct statEntry {
	char info[2048]; //used for additional info as needed.
	struct statEntry *next; //next node in list
};

struct statEntry *statHead, *statTail;

int statNumber;                         //This will just be a number we use to keep track of who is who(more so for the php side)

struct clientStatData_s
{
	int lastWeapSwitch;                         //last level.time player switched weapons
	int weaponTime[TOTAL_STATS_WEAPONS];        //total time with weapon
	int shotsFired[TOTAL_STATS_WEAPONS];        //total fired shots with weapon
	int shotsHit[TOTAL_STATS_WEAPONS];          //total shots that 'hit'
	int headshots[TOTAL_STATS_WEAPONS];         //total headshots with weapon
	int dmgGvn[TOTAL_STATS_WEAPONS];            //total damage given to players
	int dmgRcv[TOTAL_STATS_WEAPONS];            //total damage recieved from players
	int tDmgGvn[TOTAL_STATS_WEAPONS];           //total damage given to teammates
	int tDmgRcv[TOTAL_STATS_WEAPONS];           //total damage recieved from teammates
	int medPacksCollected;                      //total number of med packs picked up
	int medPacksDistributed;
	int ammoPacksCollected;                     //total number of ammo packs picked up
	int ammoPacksDistributed;
	int nadePacksCollected;                     //total number of nade packs picked up
	int nadePacksDistributed;
	int revivesReceived;                        //total number of revives given
	int objSteals;
	int objReturns;
	int objDestroyed;
	int spawnflagCaptures;                          //Martin - For dual maps (depot) don't count it as a reclaim, just a capture.
	int spawnflagReclaims;
	int dynaPlanted;                            //Only track ones at an objective
	int dynaDefused;
	int timeAxis;                               //Time on Axis team
	int timeAllies;                             //Time on Allied team
	int checkpointCaptures;
	int ping;
	int tGibs;                                  // Team Gibs
	int gibs;                                   // Enemy Gibs
};

typedef struct clientStatData_s clientStatData;

extern clientStatData statData[];


//All Weapons/MeansOfDeath stats will be tracked for
typedef enum {
	STATS_NONE, //unknown
	STATS_MP40,
	STATS_THOMPSON,
	STATS_STEN,
	STATS_MAUSER,
	STATS_SNIPERRIFLE,
	STATS_FLAMETHROWER,
	STATS_PANZERFAUST,
	STATS_VENOM,
	STATS_GRENADE,
	STATS_LUGER,
	STATS_COLT,
	STATS_MG42,
	STATS_DYNAMITE,
	STATS_KNIFE,
	STATS_THROWING_KNIFE,
	STATS_AIRSTRIKE,
	STATS_ARTILLERY,
	STATS_AIRSTRIKE_CANISTER,
	STATS_POISON,
	STATS_SYRINGE,   //I know why do we need this and poison?
	STATS_MINES,
	STATS_GOOMBA,
	STATS_FALLING,
	STATS_MORTAR,
	STATS_SUICIDE,
	STATS_DROWN,
	STATS_AMMO,
	STATS_MEDKIT,
	STATS_NADEPACK
}statWeapons;


statWeapons stat_mpMODtoStatsWeap( meansOfDeath_t mod );
statWeapons stat_mpWeapToStatsWeap( weapon_t weapon );
void stats_addEntry( char* entry );
void stats_event_clientConnect( gentity_t*ent );
void stats_event_clientDisconnect( gentity_t*ent );
void stats_event_damageTaken( gentity_t*attacker, gentity_t*target,int dmg,meansOfDeath_t mod );
void stats_event_die( gentity_t*ent );
void stats_event_dynamiteDefused( gentity_t *ent );
void stats_event_dynamitePlanted( gentity_t *ent );
void stats_event_kill( gentity_t*attacker, gentity_t*target,int MOD );
void stats_event_objectiveReturned( gentity_t *ent );
void stats_event_objectiveStolen( gentity_t *ent );
void stats_event_objectiveDestroyed( gentity_t *ent );
void stats_event_pickupPack( gentity_t*ent,gentity_t*giver,statWeapons weapon );
void stats_event_rename( gentity_t*ent );
void stats_event_respawn( gentity_t*ent );
void stats_event_revive( gentity_t*ent, gentity_t*target );
void stats_event_roundBegin();
void stats_event_roundEnd();
void stats_event_serverUser();
void stats_event_shotFired( gentity_t*attacker,int weapon );
void stats_event_shotHit( gentity_t*attacker, gentity_t*target,int weapon,int dmg,qboolean headshot );
void stats_event_spawnflagCaptured( gentity_t *ent );
void stats_event_spawnflagReclaimed( gentity_t *ent );
void stats_event_weapSwitch( gentity_t*ent,statWeapons weapon );
void stats_event_wonRound( gentity_t *ent );
void stats_event_sendClientData( gentity_t*ent );
void stats_event_checkpointCaptured( gentity_t *ent );
void stats_init();
void stats_shutdown();
void stats_dump();  // For Debugging
void stats_event_switchTeam( gentity_t*ent, int team );
#endif



