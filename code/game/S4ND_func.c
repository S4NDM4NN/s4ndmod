#include "g_local.h"
#include "S4ND_local.h"
#include "md5.h"



/*
===========
isMvsay
===========
Determines if they are using an mvsay and if not returns false
*/
qboolean isMvsay( gentity_t *ent,char *arg1 ) {
	gentity_t *te;
	gentity_t *other;
	//gentity_t *message;
	int i;
	char index[MAX_OSPATH];
	qboolean MVSAY = qfalse;
	char    *message;
	char location[1000];

	if ( !g_mvsay.integer ) {
		return qfalse;
	}
	message = "";
//BEGIN SQUELCH
/*	ent->voiceChatSquelch -= (level.time - ent->voiceChatPreviousTime);
    ent->voiceChatPreviousTime = level.time;

    if (ent->voiceChatSquelch < 0)
        ent->voiceChatSquelch = 0;

    if (ent->voiceChatSquelch >= 30000) {
        trap_SendServerCommand(ent-g_entities, "print \"^1Spam Protection^7: VoiceChat ignored\n\"");
        return qfalse;
    }

    if (g_voiceChatsAllowed.integer)
        ent->voiceChatSquelch += (34000 / g_voiceChatsAllowed.integer);
    else
        return qfalse;
        */
	if ( ent->client->sess.ignored ) {
		trap_SendServerCommand( ent - g_entities, "print \"^3You are ignored.  Voice Chat cancelled\n\"" );
		return qfalse;
	}
	Team_GetLocationMsg( ent, location, sizeof( location ) );

//END SQULECH
	if ( !Q_stricmp( arg1, "alert" ) ) {
		message = "Alien Alert!";
		Q_strncpyz( index, "sound/player/alert.wav", sizeof( index ) );
		MVSAY = qtrue;
//B
	} else if ( !Q_stricmp( arg1, "bitches" ) )  {
		message = "Ungrateful little Bitches!";
		Q_strncpyz( index, "sound/player/bitches.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "busted" ) )  {
		message = "Your so busted!";
		Q_strncpyz( index, "sound/player/a-busted.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "backup" ) )  {
		message = "We Need Backup!";
		Q_strncpyz( index, "sound/player/backup.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "bogies" ) )  {
		message = "We Got Bogies!";
		Q_strncpyz( index, "sound/player/bogies.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "boner" ) )  {
		message = "I got a big fuckin' boner right now!";
		Q_strncpyz( index, "sound/player/bigboner.wav", sizeof( index ) );
		MVSAY = qtrue;
		//boomheadshottime
	} else if ( !Q_stricmp( arg1, "boomheadshot" ) )  {
		int n =  rand() % 5; //0-4 - oops nub mistake :) - the else is incase anything else returns, it doesnt crash :)
		if ( n == 0 ) {     //S4NDM4NN - t wont crash it would just make everyone look up at the sky and shit :D
			message = "Boom Headshot! Boom Headshot!";
			Q_strncpyz( index, "sound/player/boomheadshot1.wav", sizeof( index ) );
		} else if ( n == 1 )      {
			message = "Boom Headshot, yeah!";
			Q_strncpyz( index, "sound/player/boomheadshot2.wav", sizeof( index ) );
		} else if ( n == 2 )      {
			message = "Boom Headshot!";
			Q_strncpyz( index, "sound/player/boomheadshot3.wav", sizeof( index ) );
		} else if ( n == 3 )      {
			message = "Boom Headshot!";
			Q_strncpyz( index, "sound/player/boomheadshot4.wav", sizeof( index ) );
		} else {
			message = "Boom Headshot!";
			Q_strncpyz( index, "sound/player/boomheadshot5.wav", sizeof( index ) );
		}
		MVSAY = qtrue;
//C
	} else if ( !Q_stricmp( arg1, "charge" ) )  {
		message = "Do you have any idea who's in charge of this operation?";
		Q_strncpyz( index, "sound/player/charge.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "control" ) )  {
		message = "This whole operation is out of control!";
		Q_strncpyz( index, "sound/player/control.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "comeagain" ) )  {
		message = "Please come again!";
		Q_strncpyz( index, "sound/player/comeagain.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "coveringfire" ) )  {
		message = "Covering Fire!";
		Q_strncpyz( index, "sound/player/covering.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "checkrecon" ) )  {
		message = "Recon team, check in!";
		Q_strncpyz( index, "sound/player/checkrecon.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "clear" ) )  {
		message = "Clear this area!";
		Q_strncpyz( index, "sound/player/clear.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "check" ) )  {
		message = "Ok, check in!";
		Q_strncpyz( index, "sound/player/check.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "command" ) )  {
		message = "At your command, sir.";
		Q_strncpyz( index, "sound/player/command.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "current" ) )  {
		message = "The current situation is looking pretty grim.";
		Q_strncpyz( index, "sound/player/current.wav", sizeof( index ) );
		MVSAY = qtrue;
//D
	} else if ( !Q_stricmp( arg1, "dance" ) )  {
		int n =  rand() % 2; //0-2
		if ( n == 0 ) {
			message = "Its like I can dance all day!";
			Q_strncpyz( index, "sound/player/danceallday1.wav", sizeof( index ) );
		} else {
			message = "I can dance all day, I can dance all day...";
			Q_strncpyz( index, "sound/player/danceallday2.wav", sizeof( index ) );
		}
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "die" ) )  {
		message = "Die damnit!";
		Q_strncpyz( index, "sound/player/a-die.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "didntHurt" ) )   {
		message = "Heh, that didnt hurt!";
		Q_strncpyz( index, "sound/player/a-didnt-hurt.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "damage" ) )  {
		message = "Sure, we can do more damage that way!";
		Q_strncpyz( index, "sound/player/damage.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "disneyland" ) )  {
		message = "This place makes bootcamp seem like Disneyland!";
		Q_strncpyz( index, "sound/player/disney.wav", sizeof( index ) );
		MVSAY = qtrue;
//E

	} else if ( !Q_stricmp( arg1, "Eatshit" ) )  {
		message = "EAT MY SHIT!";
		Q_strncpyz( index, "sound/player/eatmy1.wav", sizeof( index ) );
		MVSAY = qtrue;
//F

	} else if ( !Q_stricmp( arg1, "fallback" ) )  {
		message = "Fallback!";
		Q_strncpyz( index, "sound/player/retreat.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "fags" ) )  {
		message = "Those guys are fags!";
		Q_strncpyz( index, "sound/player/fags.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "fatal" ) )  {
		message = "You just made a fatal mistake!";
		Q_strncpyz( index, "sound/player/fatalmis.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "fightclub" ) )  {
		message = "Gentlemen, welcome to fightclub!";
		Q_strncpyz( index, "sound/player/fight.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "freaks" ) )  {
		message = "Let's kill these freaks!";
		Q_strncpyz( index, "sound/player/freaks.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "friendlyfire" ) )  {
		message = "Friendly Fire!";
		Q_strncpyz( index, "sound/player/friendly.wav", sizeof( index ) );
		MVSAY = qtrue;
//G
	} else if ( !Q_stricmp( arg1, "gameover" ) )  {
		message = "That's it man, game over man, its game over.";
		Q_strncpyz( index, "sound/player/gameover.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "getsome" ) )  {
		message = "Get some!";
		Q_strncpyz( index, "sound/player/getsome.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "getout" ) )  {
		message = "How do I get out of this chickety sh*t outfit.";
		Q_strncpyz( index, "sound/player/howgetout.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "gotit" ) )  {
		message = "You got it Corporal I'll be right behind you.";
		Q_strncpyz( index, "sound/player/gotit.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "gogogo" ) )  {
		message = "GO GO GO!";
		Q_strncpyz( index, "sound/player/gogogo.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "guardduty" ) )  {
		message = "Guard duty.";
		Q_strncpyz( index, "sound/player/guardduty.wav", sizeof( index ) );
		MVSAY = qtrue;
//H

	} else if ( !Q_stricmp( arg1, "hello" ) )  {
		message = "Sir, hello sir!";
		Q_strncpyz( index, "sound/player/hellosir.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "hostiles" ) )  {
		message = "Squad we got hostiles!";
		Q_strncpyz( index, "sound/player/hostiles.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "howmuchlonger" ) )  {
		message = "Ok guys, uh how much longer are you going to play this?";
		Q_strncpyz( index, "sound/player/howmuchlonger.wav", sizeof( index ) );
		MVSAY = qtrue;

//I
	} else if ( !Q_stricmp( arg1, "iamownage" ) )  {
		message = "I am the ownage, bitches!";
		Q_strncpyz( index, "sound/player/purepwnage.wav", sizeof( index ) );
		MVSAY = qtrue;
//J
//K

	} else if ( !Q_stricmp( arg1, "kickass" ) )  {
		message = "Were gonna kick your ass!";
		Q_strncpyz( index, "sound/player/kickass.wav", sizeof( index ) );
		MVSAY = qtrue;
//L
	} else if ( !Q_stricmp( arg1, "lag" ) )  {
		int n =  rand() % 2; //0-2
		if ( n == 0 ) {
			message = "Oh yeah you fucking like that...";
			Q_strncpyz( index, "sound/player/lag1.wav", sizeof( index ) );
		} else {
			message = "Fucking lagging piece of shit...";
			Q_strncpyz( index, "sound/player/lag2.wav", sizeof( index ) );
		}
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "mletsgo" ) )  {
		message = "Lets go!";
		Q_strncpyz( index, "sound/player/go.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "letsrock" ) )  {
		message = "Lets rock!";
		Q_strncpyz( index, "sound/player/letsrock.wav", sizeof( index ) );
		MVSAY = qtrue;
//M

	} else if ( !Q_stricmp( arg1, "mosquitos" ) )  {
		message = "It's like I'm hearin mosquitos buzzin around my ears!";
		Q_strncpyz( index, "sound/player/a-mosquitos.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "MeantToDoThat" ) )   {
		message = "I aaaa... Meant to do that!";
		Q_strncpyz( index, "sound/player/a-meant.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "marines" ) )  {
		message = "Lets go Marines!";
		Q_strncpyz( index, "sound/player/marines.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "movein" ) )  {
		message = "Move in!";
		Q_strncpyz( index, "sound/player/move.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "movingout" ) )  {
		message = "Moving out corporal!";
		Q_strncpyz( index, "sound/player/moving.wav", sizeof( index ) );
		MVSAY = qtrue;
//N

	} else if ( !Q_stricmp( arg1, "nothing" ) )  {
		message = "You got nothin!";
		Q_strncpyz( index, "sound/player/nothing.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "needlight" ) )  {
		message = "Need a light?";
		Q_strncpyz( index, "sound/player/needlight.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "napalm" ) )  {
		message = "I love the smell of napalm.";
		Q_strncpyz( index, "sound/player/smellnapalm.wav", sizeof( index ) );
		MVSAY = qtrue;
//O
	} else if ( !Q_stricmp( arg1, "oneshot" ) )  {
		message = "One shot, one kill!";
		Q_strncpyz( index, "sound/player/oneshot.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "onlyone" ) )  {
		message = "I'm like the only one with skills.";
		Q_strncpyz( index, "sound/player/onlyskills.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "ownage" ) )  {
		message = "Ownage!";
		Q_strncpyz( index, "sound/player/a-ownage.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "outofhere" ) )  {
		message = "Man, if we make it out of here alive...";
		Q_strncpyz( index, "sound/player/outof.wav", sizeof( index ) );
		MVSAY = qtrue;
//p
//q
//r
//s

	} else if ( !Q_stricmp( arg1, "scream" ) )  {
		message = "Ahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh!";
		Q_strncpyz( index, "sound/player/scream.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "shazbot" ) )  {
		message = "Shazbot!";
		Q_strncpyz( index, "sound/player/a-shazbot.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "seensquad" ) )  {
		message = "Have you seen any other soldiers from my platoon?";
		Q_strncpyz( index, "sound/player/seensquad.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "suckdick" ) )  {
		message = "Hey, try not to suck any dick on the way from the parking lot!";
		Q_strncpyz( index, "sound/player/suck.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "suppressing" ) )  {
		message = "I need suppressing fire!";
		Q_strncpyz( index, "sound/player/suppressing.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "stfu" ) )  {
		int n =  rand() % 2; //0-2
		if ( n == 0 ) {
			message = "Shut the fuck up!";
			Q_strncpyz( index, "sound/player/stfu1.wav", sizeof( index ) );
			MVSAY = qtrue;
		} else {
			message = "Shut the fuck up!";
			Q_strncpyz( index, "sound/player/stfu2.wav", sizeof( index ) );
			MVSAY = qtrue;
		}
	} else if ( !Q_stricmp( arg1, "stillheadshot" ) )  {
		message = "I mean my hearts beating...";
		Q_strncpyz( index, "sound/player/stillheadshot.wav", sizeof( index ) );
		MVSAY = qtrue;
//T

	} else if ( !Q_stricmp( arg1, "tagem" ) )  {
		message = "I'll tag em, you bag em!";
		Q_strncpyz( index, "sound/player/tag.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "takethis" ) )  {
		message = "Take this!";
		Q_strncpyz( index, "sound/player/take.wav", sizeof( index ) );
		MVSAY = qtrue;
//u
//v
//W

	} else if ( !Q_stricmp( arg1, "wantsome" ) )  {
		message = "You want some of me?";
		Q_strncpyz( index, "sound/player/wantsome.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "watchfire" ) )  {
		message = "Watch your fire, sir!";
		Q_strncpyz( index, "sound/player/watchfire.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "watchit" ) )  {
		message = "Watch it corporal!";
		Q_strncpyz( index, "sound/player/watchit.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "whatithought" ) )  {
		message = "That's what I thought!";
		Q_strncpyz( index, "sound/player/thatswhatithought.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "wantapiece" ) )  {
		message = "You want a piece of me, boy?";
		Q_strncpyz( index, "sound/player/wantapiece.wav", sizeof( index ) );
		MVSAY = qtrue;
//z

	} else if ( !Q_stricmp( arg1, "zone" ) )  {
		message = "Check your zone!";
		Q_strncpyz( index, "sound/player/zone.wav", sizeof( index ) );
		MVSAY = qtrue;
/*///////-------------------------------------------------------------------------
Unreal Tournament/Quake 3 Sounds     ////////////////////\\\\\\\\\\\\\\\\\\\\\\\\|
*/                                                                                                                                                                         ///////-------------------------------------------------------------------------
/*
    }else if ( !Q_stricmp( arg1, "dkill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Double kill!");
    Q_strncpyz(index, "sound/player/doublekill.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "combo" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Combo Whore!");
    Q_strncpyz(index, "sound/player/combowhore.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "dominating") ){
    G_Say(ent, NULL, SAY_ALL, "^2DOMINATING");
    Q_strncpyz(index, "sound/player/dominating.wav",sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "excellent" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Excellent!");
    Q_strncpyz(index, "sound/player/excellent.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "fblood" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Firstblood!");
    Q_strncpyz(index, "sound/player/firstblood.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "flawless" ) ){
    trap_SendServerCommand( -1, va("chat \"console: ^2Flawless Victory^7!\n\""));
    Q_strncpyz(index, "sound/player/flawlessvic.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "god" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Godlike!");
    Q_strncpyz(index, "sound/player/godlike.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "hat" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Hat Trick!");
    Q_strncpyz(index, "sound/player/hattrick.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "hhunter" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Head Hunter!");
    Q_strncpyz(index, "sound/player/headhunter.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "hshot") ) {
    G_Say(ent, NULL, SAY_ALL, "^2Headshot!");
    Q_strncpyz(index, "sound/player/headshot.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "humiliation" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Humiliation!");
    Q_strncpyz(index, "sound/player/humiliation.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "humiliating" ) ){
    trap_SendServerCommand( -1, va("chat \"console: ^2Humiliating ^1DEFEAT^7!\n\""));
    Q_strncpyz(index, "sound/player/humiliation_defeat.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "hshit" ) ){
    G_Say(ent, NULL, SAY_ALL, "^1HOLY SHIT!!!");
    Q_strncpyz(index, "sound/player/holyshit.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "impressive" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Impressive!");
    Q_strncpyz(index, "sound/player/impressive.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "kspree" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Killing Spree!");
    Q_strncpyz(index, "sound/player/killingspree.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "ludicrous" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Llllludicrous KILL!");
    Q_strncpyz(index, "sound/player/ludicrouskill.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "mekill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Mega kill!");
    Q_strncpyz(index, "sound/player/megakill.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "mokill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2MMMMMONSTER KILL!");
    Q_strncpyz(index, "sound/player/monsterkill.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "mukill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Multikill!");
    Q_strncpyz(index, "sound/player/multikill.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "play" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Play!");
    Q_strncpyz(index, "sound/player/play.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "rampage" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Rampage!");
    Q_strncpyz(index, "sound/player/rampage.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "reset" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Reset!");
    Q_strncpyz(index, "sound/player/reset.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "score" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Score!");
    Q_strncpyz(index, "sound/player/score.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "skiller" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Spawnkiller!");
    Q_strncpyz(index, "sound/player/spawnkiller.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "tkiller" ) ){
    G_Say(ent, NULL, SAY_ALL, "^1Teamkiller!");
    Q_strncpyz(index, "sound/player/teamkiller.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "tkill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Tripple Kill!");
    Q_strncpyz(index, "sound/player/tripplekill.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "ukill" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Ultra Kill!");
    Q_strncpyz(index, "sound/player/ultrakill.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "unreal" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2UnReal!");
    Q_strncpyz(index, "sound/player/unreal.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "unstoppable" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Unstoppable!");
    Q_strncpyz(index, "sound/player/unstoppable.wav", sizeof(index));
    MVSAY = qtrue;
    }else if ( !Q_stricmp( arg1, "wicked" ) ){
    G_Say(ent, NULL, SAY_ALL, "^2Wicked Sick");
    Q_strncpyz(index, "sound/player/whickedsick.wav", sizeof(index));
    MVSAY = qtrue;
*/
/*////////--------------------------------------------------------------------
END UNREAL/QUAKE SOUNDS			THESE ARE NOW SERVER-ONLY!!!
///////---------------------------------------------------------------------
//////
BEGIN ADMIN ONLY SOUNDS
*/                                                                                                                                                                                                                                                        //////
	} else if ( !Q_stricmp( arg1, "adminmsg" ) )  {
		trap_SendServerCommand( -1, va( "chat \"console: Message from administration:\n\"" ) );
		Q_strncpyz( index, "sound/player/adminmessage.wav", sizeof( index ) );
		MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "authority" ) )  {
		message = "Don't confuse your rank with my authority, corporal!";
		Q_strncpyz( index, "sound/player/athority.wav", sizeof( index ) );
		MVSAY = qtrue;
		/*}else if ( !Q_stricmp( arg1, "bw" ) ){	//ONLY FOR TK WARNING
		G_Say(ent, NULL, SAY_ALL, "^2");
		Q_strncpyz(index, "sound/player/buzzwarn.wav", sizeof(index));
		*/MVSAY = qtrue;
	} else if ( !Q_stricmp( arg1, "tfc" ) )  {
		message = "";
		Q_strncpyz( index, "sound/player/(tfc) endgame.wav", sizeof( index ) );
		MVSAY = qtrue;

//WOLF MP SOUNDS!!
	} else if ( g_gamestate.integer != GS_PLAYING )    {
		if ( !Q_stricmp( arg1, "daloop1" ) ) {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop1.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "Matrix" ) )  {
			message = "";
			Q_strncpyz( index, "sound/player/Matrix.wav", sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "daloop2" ) )   {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop2.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "daloop3" ) )   {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop3.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "daloop4" ) )   {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop4.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "daloop5" ) )   {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop5.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "daloop6" ) )   {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop6.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "daloop7" ) )   {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop7.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "daloop8" ) )   {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop8.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "daloop9" ) )   {
			message = "Cool Tunes...";
			Q_strncpyz( index, "sound/player/daloop9.wav",sizeof( index ) );
			MVSAY = qtrue;
			//Songs:
		} else if ( !Q_stricmp( arg1, "lljon" ) )   {
			message = "Damn She's Fine!";
			Q_strncpyz( index, "sound/player/admin1.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "mwelcome" ) )   {
			message = "Welcome to the jungle";
			Q_strncpyz( index, "sound/player/admin2.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "woohoo" ) )   {
			message = "Woo Hoo!";
			Q_strncpyz( index, "sound/player/admin3.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "halo" ) )   {
			message = "Why do we have to listen to this old stuff sarge?";
			Q_strncpyz( index, "sound/player/admin4.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "thegame" ) )   {
			message = "Play again?";
			Q_strncpyz( index, "sound/player/admin5.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "hitground" ) )   {
			message = "Hit the Ground";
			Q_strncpyz( index, "sound/player/admin6.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "clickboom" ) )   {
			message = "Click Click BOOM!";
			Q_strncpyz( index, "sound/player/admin7.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "werdy" ) )   {
			message = "We Ready";
			Q_strncpyz( index, "sound/player/admin8.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "irnmn" ) )   {
			message = "";
			Q_strncpyz( index, "sound/player/admin9.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "lftbhnd" ) )   {
			message = "We all got left behind";
			Q_strncpyz( index, "sound/player/admin10.wav",sizeof( index ) );
			MVSAY = qtrue;
		} else if ( !Q_stricmp( arg1, "trnsfrm" ) )   {
			message = "Ya won't last that long.";
			Q_strncpyz( index, "sound/player/admin11.wav",sizeof( index ) );
			MVSAY = qtrue;
		}
	}
//END WARMUP-ONLY
	if ( MVSAY ) {
		if ( ent->client->sess.sessionTeam == TEAM_RED ) {
			G_TeamCommand( TEAM_RED, va( "chat \"%.99s ^3(%s): ^2%s\"", ent->client->pers.netname, ent->client->pers.location, message ) );
			G_TeamCommand( TEAM_BLUE, va( "chat \"%.99s ^3(unknown): ^2%s\"",  ent->client->pers.netname, message ) );
			G_TeamCommand( TEAM_SPECTATOR, va( "chat \"%.99s ^3(unknown): ^2%s\"",  ent->client->pers.netname, message ) );

		} else if ( ent->client->sess.sessionTeam == TEAM_BLUE )    {
			G_TeamCommand( TEAM_BLUE, va( "chat \"%.99s ^3(%s): ^2%s\"", ent->client->pers.netname, ent->client->pers.location, message ) );
			G_TeamCommand( TEAM_RED, va( "chat \"%.99s ^3(unknown): ^2%s\"",  ent->client->pers.netname, message ) );
			G_TeamCommand( TEAM_SPECTATOR, va( "chat \"%.99s ^3(unknown): ^2%s\"",  ent->client->pers.netname, message ) );

		} else {
			G_TeamCommand( TEAM_RED, va( "chat \"%.99s ^3(unknown): ^2%s\"",  ent->client->pers.netname, message ) );
			G_TeamCommand( TEAM_BLUE, va( "chat \"%.99s ^3(unknown): ^2%s\"",  ent->client->pers.netname, message ) );
			G_TeamCommand( TEAM_SPECTATOR, va( "chat \"%.99s ^3(unknown): ^2%s\"",  ent->client->pers.netname, message ) );
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
	}
	return qfalse;
}


int countmines( int team ) {
	gentity_t       *ent;
	int count = 0;
	int i;

	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		ent = &g_entities[i];

		if ( !ent->inuse ) {
			continue;
		}

		if ( !ent->clipmask || !ent->r.contents ) {
			continue;
		}

		if ( !ent->parent || !ent->parent->client || !ent ) {
			continue;
		}

		if ( ent->parent->client->sess.sessionTeam != team ) {
			continue;
		}

		if ( ent->methodOfDeath == MOD_LANDMINE ) {
			count++;
		}
	}
	return count;
}


/*
================
findradius
================
*/
gentity_t *findradius( gentity_t *ent, vec3_t org, float rad ) {

	vec3_t eorg;
	int j;

	if ( !ent ) {
		ent = g_entities;
	} else {
		ent++;
	}

	for (; ent < &g_entities[level.num_entities]; ent++ )
	{
		if ( !ent->inuse ) {
			continue;
		}

		for ( j = 0; j < 3; j++ )
			eorg[j] = org[j] - ( ent->r.currentOrigin[j] +
								 ( ent->r.mins[j] + ent->r.maxs[j] ) * 0.5 );
		if ( VectorLength( eorg ) > rad ) {
			continue;
		}
		return ent;
	}
	return NULL;
}

/*
================
CheckAmmo
================
*/
qboolean CheckAmmo( gentity_t *ent ) {
	int nades, weapon, i;
	int numClips;

	if ( !g_pickupAmmo.integer ) {
		return qtrue;
	}


	switch ( ent->client->ps.stats[STAT_PLAYER_CLASS] )
	{
	case PC_ENGINEER:
		nades = g_engNades.integer;
		break;
	case PC_SOLDIER:
		nades = g_soldNades.integer;
		break;
	case PC_LT:
		nades = g_ltNades.integer;
		break;
	case PC_MEDIC:
		nades = g_medNades.integer;
		break;
	default:
		nades = 1;
		break;
	}


	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		weapon = WP_GRENADE_LAUNCHER;
	} else {
		weapon = WP_GRENADE_PINEAPPLE;
	}

	if ( ent->client->ps.ammoclip[BG_FindClipForWeapon( weapon )] < nades ) {
		return qtrue;
	}


	if ( g_ammoGivesSyringe.integer ) {
		if ( ent->client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {
			if ( ent->client->ps.ammoclip[BG_FindClipForWeapon( WP_MEDIC_SYRINGE )] < 10 ) {
				return qtrue;
			}
		}
	}

	if ( g_throwableKnives.integer && g_ammoGivesKnife.integer ) {
		if ( ( ent->client->pers.throwingKnives < g_maxKnives.integer ) || ( g_maxKnives.integer == 0 ) ) {
			return qtrue;
		}
	}

	if ( g_ammoGivesBinocs.integer ) {
		ent->client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
	}


	if ( g_ammoGivesHelmet.integer ) {
		if ( ent->client->ps.eFlags & EF_HEADSHOT ) {
			return qtrue;
		}
	}

	//check pistols
	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		weapon = WP_LUGER;
	} else {
		weapon = WP_COLT;
	}




	if ( ent->client->ps.ammo[BG_FindAmmoForWeapon( weapon )] < ammoTable[weapon].maxclip * 4 ) {
		return qtrue;
	}


	for ( i = 0; i < MAX_WEAPS_IN_BANK_MP; i++ )
	{
		weapon = weapBanksMultiPlayer[3][i];

		if ( COM_BitCheck( ent->client->ps.weapons, weapon ) ) {
			if ( weapon == WP_FLAMETHROWER ) {
				if ( ent->client->ps.ammo[BG_FindAmmoForWeapon( weapon )] < ammoTable[weapon].maxammo ) {
					return qtrue;
				}
			}

			numClips = 3;

			if ( ent->client->ps.ammo[BG_FindAmmoForWeapon( weapon )] < ammoTable[weapon].maxclip * numClips ) {
				return qtrue;
			}
		}
	}

	return qfalse;
}

/*
=======================
SniperModeCustomSpawns
========================
*/
void SniperModeCustomSpawns( gentity_t *ent, vec3_t spawn_origin, vec3_t spawn_angles ) {
	vec3_t deathmatchangles, deathmatchspawn;
	char mapName[MAX_QPATH];

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );
	deathmatchangles[0] = 0;
	deathmatchangles[2] = 0; //i think this will fix the upsidedown heads

	//allies spawns!...who wrote this??
	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		level.RedSniperSpawnPoint++; //add 1 every time its called
		if ( !Q_stricmp( mapName, "baseout" ) ) {
			if ( level.RedSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = 1120;
				deathmatchspawn[1] = -307;
				deathmatchspawn[2] = 348;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = 1055;
				deathmatchspawn[1] = -307;
				deathmatchspawn[2] = 348;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = 994;
				deathmatchspawn[1] = -307;
				deathmatchspawn[2] = 348;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = 924;
				deathmatchspawn[1] = -307;
				deathmatchspawn[2] = 348;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = 864;
				deathmatchspawn[1] = -307;
				deathmatchspawn[2] = 348;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = 802;
				deathmatchspawn[1] = -307;
				deathmatchspawn[2] = 348;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = 741;
				deathmatchspawn[1] = -307;
				deathmatchspawn[2] = 348;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = 1120;
				deathmatchspawn[1] = -224;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = 1058;
				deathmatchspawn[1] = -224;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = 991;
				deathmatchspawn[1] = -224;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = 927;
				deathmatchspawn[1] = -225;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = 864;
				deathmatchspawn[1] = -225;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = 800;
				deathmatchspawn[1] = -226;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = 737;
				deathmatchspawn[1] = -226;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = 90;
				level.RedSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "boss1" ) ) {
			if ( level.RedSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -2563;
				deathmatchspawn[1] = -920;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -2498;
				deathmatchspawn[1] = -920;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = -2616;
				deathmatchspawn[1] = -920;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = -2616;
				deathmatchspawn[1] = -863;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -2563;
				deathmatchspawn[1] = -863;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -2496;
				deathmatchspawn[1] = -863;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = -2494;
				deathmatchspawn[1] = -793;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = -2564;
				deathmatchspawn[1] = -792;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -2617;
				deathmatchspawn[1] = -791;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = -2490;
				deathmatchspawn[1] = -737;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = -2620;
				deathmatchspawn[1] = -738;
				deathmatchspawn[2] = 48;
				deathmatchangles[1] = 90;
				level.RedSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "chateau" ) ) {
			if ( level.RedSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -1809;
				deathmatchspawn[1] = 2811;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -1870;
				deathmatchspawn[1] = 2811;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = 1648;
				deathmatchspawn[1] = 2228;
				deathmatchspawn[2] = -23;
				deathmatchangles[1] = -180;
			} else if ( level.RedSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = 1648;
				deathmatchspawn[1] = 2289;
				deathmatchspawn[2] = -23;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -1742;
				deathmatchspawn[1] = 2810;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -1682;
				deathmatchspawn[1] = 2810;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = 1647;
				deathmatchspawn[1] = 2673;
				deathmatchspawn[2] = -23;
				deathmatchangles[1] = -180;
			} else if ( level.RedSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = 1647;
				deathmatchspawn[1] = 2612;
				deathmatchspawn[2] = -23;
				deathmatchangles[1] = -180;
			} else if ( level.RedSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -1426;
				deathmatchspawn[1] = 2808;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = -1487;
				deathmatchspawn[1] = 2808;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = 1588;
				deathmatchspawn[1] = 2228;
				deathmatchspawn[2] = -23;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = 1587;
				deathmatchspawn[1] = 2673;
				deathmatchspawn[2] = -23;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = -1553;
				deathmatchspawn[1] = 2809;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = -1613;
				deathmatchspawn[1] = 2811;
				deathmatchspawn[2] = 344;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = 1587;
				deathmatchspawn[1] = 2289;
				deathmatchspawn[2] = -23;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = 1587;
				deathmatchspawn[1] = 2613;
				deathmatchspawn[2] = -23;
				deathmatchangles[1] = 180;
				level.RedSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "forest" ) ) {
			if ( level.flagteam == TEAM_RED ) { /*
				if(level.RedSniperSpawnPoint == 0){
				deathmatchspawn[0]= 2077;
				deathmatchspawn[1]= 5306;
				deathmatchspawn[2]= 601;
				deathmatchangles[1]= -90;
				}else if(level.RedSniperSpawnPoint == 1){
				deathmatchspawn[0]= 2158;
				deathmatchspawn[1]= 5306;
				deathmatchspawn[2]= 598;
				deathmatchangles[1]= -90;
				}else if(level.RedSniperSpawnPoint == 2){
				deathmatchspawn[0]= 2261;
				deathmatchspawn[1]= 5306;
				deathmatchspawn[2]= 589;
				deathmatchangles[1]= -90;
				}else if(level.RedSniperSpawnPoint == 3){
				deathmatchspawn[0]= 2361;
				deathmatchspawn[1]= 5307;
				deathmatchspawn[2]= 584;
				deathmatchangles[1]= -90;
				}else if(level.RedSniperSpawnPoint == 4){
				deathmatchspawn[0]= 2081;
				deathmatchspawn[1]= 5199;
				deathmatchspawn[2]= 592;
				deathmatchangles[1]= -91;
				}else if(level.RedSniperSpawnPoint == 5){
				deathmatchspawn[0]= 2162;
				deathmatchspawn[1]= 5196;
				deathmatchspawn[2]= 592;
				deathmatchangles[1]= -91;
				}else if(level.RedSniperSpawnPoint == 6){
				deathmatchspawn[0]= 2264;
				deathmatchspawn[1]= 5193;
				deathmatchspawn[2]= 589;
				deathmatchangles[1]= -89;
				}else if(level.RedSniperSpawnPoint == 7){
				deathmatchspawn[0]= 2082;
				deathmatchspawn[1]= 5111;
				deathmatchspawn[2]= 585;
				deathmatchangles[1]= -85;
				}else if(level.RedSniperSpawnPoint == 8){
				deathmatchspawn[0]= 2164;
				deathmatchspawn[1]= 5111;
				deathmatchspawn[2]= 585;
				deathmatchangles[1]= -88;
				}else if(level.RedSniperSpawnPoint == 9){
				deathmatchspawn[0]= 2261;
				deathmatchspawn[1]= 5111;
				deathmatchspawn[2]= 585;
				deathmatchangles[1]= -83;
				}else if(level.RedSniperSpawnPoint == 10){
				deathmatchspawn[0]= 2357;
				deathmatchspawn[1]= 5110;
				deathmatchspawn[2]= 583;
				deathmatchangles[1]= -81;
				}else if(level.RedSniperSpawnPoint == 11){
				deathmatchspawn[0]= 2357;
				deathmatchspawn[1]= 5207;
				deathmatchspawn[2]= 584;
				deathmatchangles[1]= -87;
				level.RedSniperSpawnPoint = 0;
				}
				*/
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -66;
					deathmatchspawn[1] = 3485;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = 3;
					deathmatchspawn[1] = 3485;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -178;
					deathmatchspawn[1] = 3484;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -248;
					deathmatchspawn[1] = 3484;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -29;
					deathmatchspawn[1] = 3675;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = 37;
					deathmatchspawn[1] = 3675;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -221;
					deathmatchspawn[1] = 3676;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -288;
					deathmatchspawn[1] = 3676;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = 74;
					deathmatchspawn[1] = 3485;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -310;
					deathmatchspawn[1] = 3485;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 0;
					level.RedSniperSpawnPoint = 0;
				}
			} else {
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -6480;
					deathmatchspawn[1] = -967;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 4;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -6391;
					deathmatchspawn[1] = -961;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 4;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -6322;
					deathmatchspawn[1] = -955;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 4;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -6248;
					deathmatchspawn[1] = -950;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 4;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -6192;
					deathmatchspawn[1] = -963;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 4;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -6121;
					deathmatchspawn[1] = -970;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 4;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -6042;
					deathmatchspawn[1] = -964;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 4;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -5974;
					deathmatchspawn[1] = -966;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = -6459;
					deathmatchspawn[1] = -893;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -6393;
					deathmatchspawn[1] = -885;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = -6313;
					deathmatchspawn[1] = -875;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = -6233;
					deathmatchspawn[1] = -874;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 12 )     {
					deathmatchspawn[0] = -6162;
					deathmatchspawn[1] = -886;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 13 )     {
					deathmatchspawn[0] = -6103;
					deathmatchspawn[1] = -879;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 14 )     {
					deathmatchspawn[0] = -6014;
					deathmatchspawn[1] = -868;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 15 )     {
					deathmatchspawn[0] = -5933;
					deathmatchspawn[1] = -858;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 16 )     {
					deathmatchspawn[0] = -6418;
					deathmatchspawn[1] = -811;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 17 )     {
					deathmatchspawn[0] = -6359;
					deathmatchspawn[1] = -807;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 18 )     {
					deathmatchspawn[0] = -6303;
					deathmatchspawn[1] = -808;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 19 )     {
					deathmatchspawn[0] = -6222;
					deathmatchspawn[1] = -804;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 20 )     {
					deathmatchspawn[0] = -6151;
					deathmatchspawn[1] = -810;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 21 )     {
					deathmatchspawn[0] = -6079;
					deathmatchspawn[1] = -802;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 22 )     {
					deathmatchspawn[0] = -5995;
					deathmatchspawn[1] = -792;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
				} else if ( level.RedSniperSpawnPoint == 23 )     {
					deathmatchspawn[0] = -5949;
					deathmatchspawn[1] = -796;
					deathmatchspawn[2] = 200;
					deathmatchangles[1] = 7;
					level.RedSniperSpawnPoint = 0;
				}
			}
		}

		if ( !Q_stricmp( mapName, "factory" ) ) {
			if ( level.RedSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = 2499;
				deathmatchspawn[1] = -961;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = 2500;
				deathmatchspawn[1] = -1003;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = 2499;
				deathmatchspawn[1] = -1111;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = 2501;
				deathmatchspawn[1] = -1052;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = 1977;
				deathmatchspawn[1] = -1061;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = 1977;
				deathmatchspawn[1] = -1116;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = 1977;
				deathmatchspawn[1] = -1177;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = 1977;
				deathmatchspawn[1] = -1230;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = 2445;
				deathmatchspawn[1] = -1238;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = 2435;
				deathmatchspawn[1] = -1108;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = 2435;
				deathmatchspawn[1] = -1164;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = 2433;
				deathmatchspawn[1] = -1048;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = 2434;
				deathmatchspawn[1] = -999;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = 2435;
				deathmatchspawn[1] = -955;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 180;
			} else if ( level.RedSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = 2047;
				deathmatchspawn[1] = -1175;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = 2044;
				deathmatchspawn[1] = -1231;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = 0;
				level.RedSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "norway" ) ) {
			if ( level.flagteam == TEAM_RED ) {
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1815;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = -1;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1756;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1701;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = -1;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1638;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1572;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = 2091;
					deathmatchspawn[1] = -1312;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = 2091;
					deathmatchspawn[1] = -1363;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = 2091;
					deathmatchspawn[1] = -1421;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = 2190;
					deathmatchspawn[1] = -1188;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = 2190;
					deathmatchspawn[1] = -1248;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = 2192;
					deathmatchspawn[1] = -1756;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = 2191;
					deathmatchspawn[1] = -1816;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
					level.RedSniperSpawnPoint = 0;
				}
			} else {
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -3677;
					deathmatchspawn[1] = -1225;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -3618;
					deathmatchspawn[1] = -1225;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -90;;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -3630;
					deathmatchspawn[1] = -3224;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 90;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -3581;
					deathmatchspawn[1] = -3224;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 90;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -3619;
					deathmatchspawn[1] = -1281;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -3677;
					deathmatchspawn[1] = -1280;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -3628;
					deathmatchspawn[1] = -3161;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 90;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -3581;
					deathmatchspawn[1] = -3161;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 90;
				} else if ( level.RedSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = -3341;
					deathmatchspawn[1] = -1226;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -3289;
					deathmatchspawn[1] = -1226;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = -3329;
					deathmatchspawn[1] = -3231;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 90;
				} else if ( level.RedSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = -3281;
					deathmatchspawn[1] = -3230;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 90;
				} else if ( level.RedSniperSpawnPoint == 12 )     {
					deathmatchspawn[0] = -3342;
					deathmatchspawn[1] = -1288;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 13 )     {
					deathmatchspawn[0] = -3287;
					deathmatchspawn[1] = -1287;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -90;;
				} else if ( level.RedSniperSpawnPoint == 14 )     {
					deathmatchspawn[0] = -3327;
					deathmatchspawn[1] = -3161;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 90;
				} else if ( level.RedSniperSpawnPoint == 15 )     {
					deathmatchspawn[0] = -3282;
					deathmatchspawn[1] = -3161;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 90;
				} else if ( level.RedSniperSpawnPoint == 16 )     {
					deathmatchspawn[0] = -3260;
					deathmatchspawn[1] = -1384;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -180;
				} else if ( level.RedSniperSpawnPoint == 17 )     {
					deathmatchspawn[0] = -3257;
					deathmatchspawn[1] = -1331;
					deathmatchspawn[2] = 528;
					deathmatchangles[1] = -180;
				} else if ( level.RedSniperSpawnPoint == 18 )     {
					deathmatchspawn[0] = -3669;
					deathmatchspawn[1] = -3058;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 19 )     {
					deathmatchspawn[0] = -3669;
					deathmatchspawn[1] = -3112;
					deathmatchspawn[2] = 519;
					deathmatchangles[1] = 0;
					level.RedSniperSpawnPoint = 0;
				}
			}
		}
		if ( !Q_stricmp( mapName, "rocket" ) ) {
			if ( level.RedSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -2367;
				deathmatchspawn[1] = 705;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -2367;
				deathmatchspawn[1] = 766;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = -2367;
				deathmatchspawn[1] = 831;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = -2367;
				deathmatchspawn[1] = 895;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -2368;
				deathmatchspawn[1] = 958;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -2368;
				deathmatchspawn[1] = 1021;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = -2287;
				deathmatchspawn[1] = 1022;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = -2287;
				deathmatchspawn[1] = 956;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -2287;
				deathmatchspawn[1] = 894;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = -2287;
				deathmatchspawn[1] = 829;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = -2287;
				deathmatchspawn[1] = 767;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = -2287;
				deathmatchspawn[1] = 709;
				deathmatchspawn[2] = 608;
				deathmatchangles[1] = 0;
				level.RedSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "sfm" ) ) {
			if ( level.RedSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = 681;
				deathmatchspawn[1] = -2570;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = 785;
				deathmatchspawn[1] = -2570;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = 879;
				deathmatchspawn[1] = -2571;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = 643;
				deathmatchspawn[1] = -2679;
				deathmatchspawn[2] = -111;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = 643;
				deathmatchspawn[1] = -2766;
				deathmatchspawn[2] = -111;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = 790;
				deathmatchspawn[1] = -2893;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = 886;
				deathmatchspawn[1] = -2896;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = 645;
				deathmatchspawn[1] = -2895;
				deathmatchspawn[2] = -84;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = 1090;
				deathmatchspawn[1] = -2911;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = 1050;
				deathmatchspawn[1] = -2458;
				deathmatchspawn[2] = -12;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = 988;
				deathmatchspawn[1] = -2459;
				deathmatchspawn[2] = -12;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = 987;
				deathmatchspawn[1] = -2567;
				deathmatchspawn[2] = -98;
				deathmatchangles[1] = 0;
				level.RedSniperSpawnPoint = 0;
			}
		}

		if ( !Q_stricmp( mapName, "dam" ) ) {
			if ( level.RedSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -1855;
				deathmatchspawn[1] = -3767;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -889;
				deathmatchspawn[1] = -6274;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = -1858;
				deathmatchspawn[1] = -3823;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = -807;
				deathmatchspawn[1] = -6274;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -1859;
				deathmatchspawn[1] = -3879;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -731;
				deathmatchspawn[1] = -6275;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = -1795;
				deathmatchspawn[1] = -3767;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = -642;
				deathmatchspawn[1] = -6279;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -1795;
				deathmatchspawn[1] = -3823;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = -887;
				deathmatchspawn[1] = -6211;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = -1730;
				deathmatchspawn[1] = -3767;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = -805;
				deathmatchspawn[1] = -6213;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = -1730;
				deathmatchspawn[1] = -3823;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = -731;
				deathmatchspawn[1] = -6217;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = -1607;
				deathmatchspawn[1] = -3882;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
			} else if ( level.RedSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = -641;
				deathmatchspawn[1] = -6215;
				deathmatchspawn[2] = 2328;
				deathmatchangles[1] = 90;
				level.RedSniperSpawnPoint = 0;
			}
		}

		if ( !Q_stricmp( mapName, "dark" ) ) {
			if ( level.flagteam == TEAM_RED ) {
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -3073;
					deathmatchspawn[1] = 1609;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -1707;
					deathmatchspawn[1] = 345;
					deathmatchspawn[2] = 792;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -1707;
					deathmatchspawn[1] = 282;
					deathmatchspawn[2] = 792;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -1708;
					deathmatchspawn[1] = 35;
					deathmatchspawn[2] = 792;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -1708;
					deathmatchspawn[1] = -34;
					deathmatchspawn[2] = 792;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					//Fireplace
					deathmatchspawn[0] = -1705;
					deathmatchspawn[1] = 157;
					deathmatchspawn[2] = 802;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -1706;
					deathmatchspawn[1] = 191;
					deathmatchspawn[2] = 896;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -1703;
					deathmatchspawn[1] = 129;
					deathmatchspawn[2] = 896;
					deathmatchangles[1] = 0;
					level.RedSniperSpawnPoint = 0;
				}
			} else {
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = 4393;
					deathmatchspawn[1] = 3103;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = 4323;
					deathmatchspawn[1] = 3104;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = 4253;
					deathmatchspawn[1] = 3107;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = 4391;
					deathmatchspawn[1] = 3158;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = 4320;
					deathmatchspawn[1] = 3158;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = 4248;
					deathmatchspawn[1] = 3160;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = 4390;
					deathmatchspawn[1] = 3049;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = 4319;
					deathmatchspawn[1] = 3049;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = 4252;
					deathmatchspawn[1] = 3050;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = 4180;
					deathmatchspawn[1] = 3051;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = 4180;
					deathmatchspawn[1] = 3107;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = 4179;
					deathmatchspawn[1] = 3161;
					deathmatchspawn[2] = 662;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 12 )     {
					deathmatchspawn[0] = 4316;
					deathmatchspawn[1] = 2654;
					deathmatchspawn[2] = 767;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 13 )     {
					deathmatchspawn[0] = 4311;
					deathmatchspawn[1] = 2576;
					deathmatchspawn[2] = 747;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 14 )     {
					deathmatchspawn[0] = 4313;
					deathmatchspawn[1] = 2491;
					deathmatchspawn[2] = 726;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 15 )     {
					deathmatchspawn[0] = 4315;
					deathmatchspawn[1] = 3553;
					deathmatchspawn[2] = 664;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 16 )     {
					deathmatchspawn[0] = 4312;
					deathmatchspawn[1] = 3654;
					deathmatchspawn[2] = 664;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 17 )     {
					deathmatchspawn[0] = 4312;
					deathmatchspawn[1] = 3760;
					deathmatchspawn[2] = 657;
					deathmatchangles[1] = 180;
					level.RedSniperSpawnPoint = 0;
				}
			}
		}
		if ( !Q_stricmp( mapName, "assault" ) ) {
			if ( level.flagteam == TEAM_RED ) {
/*	if(level.RedSniperSpawnPoint == 0){
deathmatchspawn[0]= 2962;
deathmatchspawn[1]= 3301;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.RedSniperSpawnPoint == 1){
deathmatchspawn[0]= 2877;
deathmatchspawn[1]= 3301;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.RedSniperSpawnPoint == 2){
deathmatchspawn[0]= 2790;
deathmatchspawn[1]= 3301;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.RedSniperSpawnPoint == 3){
deathmatchspawn[0]= 2706;
deathmatchspawn[1]= 3301;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.RedSniperSpawnPoint == 4){
deathmatchspawn[0]= 2965;
deathmatchspawn[1]= 3203;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.RedSniperSpawnPoint == 5){
deathmatchspawn[0]= 2878;
deathmatchspawn[1]= 3202;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.RedSniperSpawnPoint == 6){
deathmatchspawn[0]= 2788;
deathmatchspawn[1]= 3202;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.RedSniperSpawnPoint == 7){
deathmatchspawn[0]= 2704;
deathmatchspawn[1]= 3198;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
level.RedSniperSpawnPoint = 0;
}*/
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -1036;
					deathmatchspawn[1] = 913;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -966;
					deathmatchspawn[1] = 913;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -890;
					deathmatchspawn[1] = 912;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -818;
					deathmatchspawn[1] = 912;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -731;
					deathmatchspawn[1] = 910;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -651;
					deathmatchspawn[1] = 909;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -559;
					deathmatchspawn[1] = 906;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -559;
					deathmatchspawn[1] = 986;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
					level.RedSniperSpawnPoint = 0;
				}
			} else {
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = 3225;
					deathmatchspawn[1] = -3380;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = 7365;
					deathmatchspawn[1] = -976;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = 7473;
					deathmatchspawn[1] = 1170;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = 3224;
					deathmatchspawn[1] = -3280;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = 7368;
					deathmatchspawn[1] = -1149;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = 7467;
					deathmatchspawn[1] = 1283;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = -180;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = 3284;
					deathmatchspawn[1] = -3379;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = 7367;
					deathmatchspawn[1] = -1062;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = -180;
				} else if ( level.RedSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = 7459;
					deathmatchspawn[1] = 1389;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = 3284;
					deathmatchspawn[1] = -3278;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = 7266;
					deathmatchspawn[1] = -976;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = 7365;
					deathmatchspawn[1] = 1171;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = -180;
				} else if ( level.RedSniperSpawnPoint == 12 )     {
					deathmatchspawn[0] = 7268;
					deathmatchspawn[1] = -1060;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = -180;
				} else if ( level.RedSniperSpawnPoint == 13 )     {
					deathmatchspawn[0] = 7364;
					deathmatchspawn[1] = 1279;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 14 )     {
					deathmatchspawn[0] = 7270;
					deathmatchspawn[1] = -1151;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = 180;
				} else if ( level.RedSniperSpawnPoint == 15 )     {
					deathmatchspawn[0] = 7363;
					deathmatchspawn[1] = 1388;
					deathmatchspawn[2] = 90;
					deathmatchangles[1] = -180;
					level.RedSniperSpawnPoint = 0;
				}
			}
		}
		if ( !Q_stricmp( mapName, "trainyard" ) ) {
			if ( level.RedSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -1551;
				deathmatchspawn[1] = 682;
				deathmatchspawn[2] = 88;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -1608;
				deathmatchspawn[1] = 682;
				deathmatchspawn[2] = 88;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = -1408;
				deathmatchspawn[1] = 1048;
				deathmatchspawn[2] = -271;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = -1410;
				deathmatchspawn[1] = 970;
				deathmatchspawn[2] = -271;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -1550;
				deathmatchspawn[1] = 617;
				deathmatchspawn[2] = 88;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -1609;
				deathmatchspawn[1] = 617;
				deathmatchspawn[2] = 88;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = -1408;
				deathmatchspawn[1] = 822;
				deathmatchspawn[2] = -271;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = -1408;
				deathmatchspawn[1] = 898;
				deathmatchspawn[2] = -271;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -1611;
				deathmatchspawn[1] = 547;
				deathmatchspawn[2] = 88;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = -1341;
				deathmatchspawn[1] = 1048;
				deathmatchspawn[2] = -271;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = -1339;
				deathmatchspawn[1] = 970;
				deathmatchspawn[2] = -271;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = -1610;
				deathmatchspawn[1] = 484;
				deathmatchspawn[2] = 88;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = -1611;
				deathmatchspawn[1] = 416;
				deathmatchspawn[2] = 88;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = -1550;
				deathmatchspawn[1] = 484;
				deathmatchspawn[2] = 88;
				deathmatchangles[1] = -90;
			} else if ( level.RedSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = -1339;
				deathmatchspawn[1] = 896;
				deathmatchspawn[2] = -271;
				deathmatchangles[1] = 0;
			} else if ( level.RedSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = -1339;
				deathmatchspawn[1] = 821;
				deathmatchspawn[2] = -271;
				deathmatchangles[1] = 0;
				level.RedSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "tram" ) ) {
			if ( level.flagteam == TEAM_RED ) {
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -214;
					deathmatchspawn[1] = 348;
					deathmatchspawn[2] = 472;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -145;
					deathmatchspawn[1] = 348;
					deathmatchspawn[2] = 472;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -217;
					deathmatchspawn[1] = 185;
					deathmatchspawn[2] = 816;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -217;
					deathmatchspawn[1] = 17;
					deathmatchspawn[2] = 816;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -38;
					deathmatchspawn[1] = 14;
					deathmatchspawn[2] = 838;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -38;
					deathmatchspawn[1] = 182;
					deathmatchspawn[2] = 838;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -39;
					deathmatchspawn[1] = 91;
					deathmatchspawn[2] = 838;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -216;
					deathmatchspawn[1] = 96;
					deathmatchspawn[2] = 816;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = -125;
					deathmatchspawn[1] = 187;
					deathmatchspawn[2] = 830;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -123;
					deathmatchspawn[1] = -24;
					deathmatchspawn[2] = 821;
					deathmatchangles[1] = -90;
					level.RedSniperSpawnPoint = 0;
				}
			} else {
				if ( level.RedSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = 3372;
					deathmatchspawn[1] = 1818;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = 3487;
					deathmatchspawn[1] = 1816;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = 3614;
					deathmatchspawn[1] = 1816;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = 3733;
					deathmatchspawn[1] = 1815;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = 3382;
					deathmatchspawn[1] = 1440;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = 0;
				} else if ( level.RedSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = 3377;
					deathmatchspawn[1] = 1685;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = 3485;
					deathmatchspawn[1] = 1681;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = 3614;
					deathmatchspawn[1] = 1684;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = 3731;
					deathmatchspawn[1] = 1684;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -90;
				} else if ( level.RedSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = 3444;
					deathmatchspawn[1] = 1441;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = 0;
					level.RedSniperSpawnPoint = 0;
				}
			}
		}
///////////////////////////////////////////////////		ALLIES ///////////////////////////////////////////
	} else { //Allied Spawns!
		level.BlueSniperSpawnPoint++; //add 1 every time its called
		if ( !Q_stricmp( mapName, "baseout" ) ) {
			if ( level.BlueSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -2372;
				deathmatchspawn[1] = -670;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -2439;
				deathmatchspawn[1] = -670;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = -2503;
				deathmatchspawn[1] = -669;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = -2565;
				deathmatchspawn[1] = -670;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -2631;
				deathmatchspawn[1] = -670;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -2692;
				deathmatchspawn[1] = -670;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = -2374;
				deathmatchspawn[1] = -606;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = -2439;
				deathmatchspawn[1] = -606;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -2503;
				deathmatchspawn[1] = -606;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = -2564;
				deathmatchspawn[1] = -606;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = -2627;
				deathmatchspawn[1] = -606;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = -2693;
				deathmatchspawn[1] = -605;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = -2694;
				deathmatchspawn[1] = -541;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = -2628;
				deathmatchspawn[1] = -541;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = -2565;
				deathmatchspawn[1] = -541;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = -2503;
				deathmatchspawn[1] = -541;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 16 )     {
				deathmatchspawn[0] = -2436;
				deathmatchspawn[1] = -541;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 17 )     {
				deathmatchspawn[0] = -2374;
				deathmatchspawn[1] = -541;
				deathmatchspawn[2] = 56;
				deathmatchangles[1] = 90;
				level.BlueSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "boss1" ) ) {
			if ( level.BlueSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -2361;
				deathmatchspawn[1] = 217;
				deathmatchspawn[2] = -95;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -2342;
				deathmatchspawn[1] = -87;
				deathmatchspawn[2] = -95;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = -2586;
				deathmatchspawn[1] = -213;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = -2781;
				deathmatchspawn[1] = -157;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -2778;
				deathmatchspawn[1] = -83;
				deathmatchspawn[2] = -103;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -2776;
				deathmatchspawn[1] = -14;
				deathmatchspawn[2] = -99;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = -2597;
				deathmatchspawn[1] = 359;
				deathmatchspawn[2] = -71;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = -2307;
				deathmatchspawn[1] = 101;
				deathmatchspawn[2] = 80;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -2307;
				deathmatchspawn[1] = 41;
				deathmatchspawn[2] = 80;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = -2642;
				deathmatchspawn[1] = 98;
				deathmatchspawn[2] = 80;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = -2642;
				deathmatchspawn[1] = 25;
				deathmatchspawn[2] = 80;
				deathmatchangles[1] = 0;
				level.BlueSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "chateau" ) ) {
			if ( level.BlueSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = 2128;
				deathmatchspawn[1] = 352;
				deathmatchspawn[2] = -7;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = 2128;
				deathmatchspawn[1] = 401;
				deathmatchspawn[2] = -7;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = 2128;
				deathmatchspawn[1] = 304;
				deathmatchspawn[2] = -7;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = 1128;
				deathmatchspawn[1] = 1161;
				deathmatchspawn[2] = -215;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = 1128;
				deathmatchspawn[1] = 1219;
				deathmatchspawn[2] = -215;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = 1872;
				deathmatchspawn[1] = -1;
				deathmatchspawn[2] = -39;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = 1616;
				deathmatchspawn[1] = -1;
				deathmatchspawn[2] = -39;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = 1360;
				deathmatchspawn[1] = -2;
				deathmatchspawn[2] = -39;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = 1129;
				deathmatchspawn[1] = 997;
				deathmatchspawn[2] = -215;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = 1130;
				deathmatchspawn[1] = 943;
				deathmatchspawn[2] = -215;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = 1840;
				deathmatchspawn[1] = 608;
				deathmatchspawn[2] = -39;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = 1903;
				deathmatchspawn[1] = 608;
				deathmatchspawn[2] = -39;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = 1126;
				deathmatchspawn[1] = 1052;
				deathmatchspawn[2] = -215;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = 1129;
				deathmatchspawn[1] = 1103;
				deathmatchspawn[2] = -215;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = 1396;
				deathmatchspawn[1] = 608;
				deathmatchspawn[2] = -39;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = 1328;
				deathmatchspawn[1] = 608;
				deathmatchspawn[2] = -39;
				deathmatchangles[1] = -90;
				level.BlueSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "forest" ) ) {
			if ( level.flagteam == TEAM_BLUE ) { /*
				if(level.BlueSniperSpawnPoint == 0){
				deathmatchspawn[0]= 2077;
				deathmatchspawn[1]= 5306;
				deathmatchspawn[2]= 601;
				deathmatchangles[1]= -90;
				}else if(level.BlueSniperSpawnPoint == 1){
				deathmatchspawn[0]= 2158;
				deathmatchspawn[1]= 5306;
				deathmatchspawn[2]= 598;
				deathmatchangles[1]= -90;
				}else if(level.BlueSniperSpawnPoint == 2){
				deathmatchspawn[0]= 2261;
				deathmatchspawn[1]= 5306;
				deathmatchspawn[2]= 589;
				deathmatchangles[1]= -90;
				}else if(level.BlueSniperSpawnPoint == 3){
				deathmatchspawn[0]= 2361;
				deathmatchspawn[1]= 5307;
				deathmatchspawn[2]= 584;
				deathmatchangles[1]= -90;
				}else if(level.BlueSniperSpawnPoint == 4){
				deathmatchspawn[0]= 2081;
				deathmatchspawn[1]= 5199;
				deathmatchspawn[2]= 592;
				deathmatchangles[1]= -91;
				}else if(level.BlueSniperSpawnPoint == 5){
				deathmatchspawn[0]= 2162;
				deathmatchspawn[1]= 5196;
				deathmatchspawn[2]= 592;
				deathmatchangles[1]= -91;
				}else if(level.BlueSniperSpawnPoint == 6){
				deathmatchspawn[0]= 2264;
				deathmatchspawn[1]= 5193;
				deathmatchspawn[2]= 589;
				deathmatchangles[1]= -89;
				}else if(level.BlueSniperSpawnPoint == 7){
				deathmatchspawn[0]= 2082;
				deathmatchspawn[1]= 5111;
				deathmatchspawn[2]= 585;
				deathmatchangles[1]= -85;
				}else if(level.BlueSniperSpawnPoint == 8){
				deathmatchspawn[0]= 2164;
				deathmatchspawn[1]= 5111;
				deathmatchspawn[2]= 585;
				deathmatchangles[1]= -88;
				}else if(level.BlueSniperSpawnPoint == 9){
				deathmatchspawn[0]= 2261;
				deathmatchspawn[1]= 5111;
				deathmatchspawn[2]= 585;
				deathmatchangles[1]= -83;
				}else if(level.BlueSniperSpawnPoint == 10){
				deathmatchspawn[0]= 2357;
				deathmatchspawn[1]= 5110;
				deathmatchspawn[2]= 583;
				deathmatchangles[1]= -81;
				}else if(level.BlueSniperSpawnPoint == 11){
				deathmatchspawn[0]= 2357;
				deathmatchspawn[1]= 5207;
				deathmatchspawn[2]= 584;
				deathmatchangles[1]= -87;
				level.BlueSniperSpawnPoint = 0;
				}
				*/
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -66;
					deathmatchspawn[1] = 3485;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = 3;
					deathmatchspawn[1] = 3485;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -178;
					deathmatchspawn[1] = 3484;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -248;
					deathmatchspawn[1] = 3484;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -29;
					deathmatchspawn[1] = 3675;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = 37;
					deathmatchspawn[1] = 3675;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -221;
					deathmatchspawn[1] = 3676;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -288;
					deathmatchspawn[1] = 3676;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = 74;
					deathmatchspawn[1] = 3485;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -310;
					deathmatchspawn[1] = 3485;
					deathmatchspawn[2] = 232;
					deathmatchangles[1] = 180;
					level.BlueSniperSpawnPoint = 0;
				}
			} else {
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = 1860;
					deathmatchspawn[1] = -5317;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = 1860;
					deathmatchspawn[1] = -5164;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = 1861;
					deathmatchspawn[1] = -5099;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = 1594;
					deathmatchspawn[1] = -5097;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = 1864;
					deathmatchspawn[1] = -4908;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = 1593;
					deathmatchspawn[1] = -4911;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = 1596;
					deathmatchspawn[1] = -5037;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = 1594;
					deathmatchspawn[1] = -4973;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = 1750;
					deathmatchspawn[1] = -5275;
					deathmatchspawn[2] = 664;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = 1710;
					deathmatchspawn[1] = -5277;
					deathmatchspawn[2] = 664;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = 1710;
					deathmatchspawn[1] = -5200;
					deathmatchspawn[2] = 664;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = 1750;
					deathmatchspawn[1] = -5202;
					deathmatchspawn[2] = 664;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 12 )     {
					deathmatchspawn[0] = 1780;
					deathmatchspawn[1] = -5099;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 13 )     {
					deathmatchspawn[0] = 1779;
					deathmatchspawn[1] = -5164;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 14 )     {
					deathmatchspawn[0] = 1779;
					deathmatchspawn[1] = -5021;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 15 )     {
					deathmatchspawn[0] = 1687;
					deathmatchspawn[1] = -5020;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 16 )     {
					deathmatchspawn[0] = 1779;
					deathmatchspawn[1] = -4955;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 17 )     {
					deathmatchspawn[0] = 1686;
					deathmatchspawn[1] = -4955;
					deathmatchspawn[2] = 384;
					deathmatchangles[1] = 90;
					level.BlueSniperSpawnPoint = 0;
				}
			}
		} //Old Allies Spawn

/*if(level.BlueSniperSpawnPoint == 0){
deathmatchspawn[0]= 6599;
deathmatchspawn[1]= -1107;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 1){
deathmatchspawn[0]= 6526;
deathmatchspawn[1]= -1106;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 2){
deathmatchspawn[0]= 6464;
deathmatchspawn[1]= -1105;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 3){
deathmatchspawn[0]= 6402;
deathmatchspawn[1]= -1099;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 4){
deathmatchspawn[0]= 6332;
deathmatchspawn[1]= -1097;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 5){
deathmatchspawn[0]= 6264;
deathmatchspawn[1]= -1096;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 6){
deathmatchspawn[0]= 6193;
deathmatchspawn[1]= -1095;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 7){
deathmatchspawn[0]= 6106;
deathmatchspawn[1]= -1094;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 8){
deathmatchspawn[0]= 6033;
deathmatchspawn[1]= -1092;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 9){
deathmatchspawn[0]= 5954;
deathmatchspawn[1]= -1091;
deathmatchspawn[2]= 280;
deathmatchangles[1]= 179;
}else if(level.BlueSniperSpawnPoint == 10){
deathmatchspawn[0]= 6610;
deathmatchspawn[1]= -1171;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 11){
deathmatchspawn[0]= 6546;
deathmatchspawn[1]= -1172;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 12){
deathmatchspawn[0]= 6489;
deathmatchspawn[1]= -1173;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 13){
deathmatchspawn[0]= 6410;
deathmatchspawn[1]= -1175;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 14){
deathmatchspawn[0]= 6331;
deathmatchspawn[1]= -1177;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 15){
deathmatchspawn[0]= 6251;
deathmatchspawn[1]= -1178;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 16){
deathmatchspawn[0]= 6173;
deathmatchspawn[1]= -1180;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 17){
deathmatchspawn[0]= 6096;
deathmatchspawn[1]= -1180;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 18){
deathmatchspawn[0]= 6021;
deathmatchspawn[1]= -1176;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 19){
deathmatchspawn[0]= 5944;
deathmatchspawn[1]= -1177;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -179;
}else if(level.BlueSniperSpawnPoint == 20){
deathmatchspawn[0]= 6586;
deathmatchspawn[1]= -1240;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
}else if(level.BlueSniperSpawnPoint == 21){
deathmatchspawn[0]= 6530;
deathmatchspawn[1]= -1245;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
}else if(level.BlueSniperSpawnPoint == 22){
deathmatchspawn[0]= 6476;
deathmatchspawn[1]= -1248;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
}else if(level.BlueSniperSpawnPoint == 23){
deathmatchspawn[0]= 6410;
deathmatchspawn[1]= -1245;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
}else if(level.BlueSniperSpawnPoint == 24){
deathmatchspawn[0]= 6333;
deathmatchspawn[1]= -1241;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
}else if(level.BlueSniperSpawnPoint == 25){
deathmatchspawn[0]= 6231;
deathmatchspawn[1]= -1245;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
}else if(level.BlueSniperSpawnPoint == 26){
deathmatchspawn[0]= 6135;
deathmatchspawn[1]= -1242;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
}else if(level.BlueSniperSpawnPoint == 27){
deathmatchspawn[0]= 6047;
deathmatchspawn[1]= -1237;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
}else if(level.BlueSniperSpawnPoint == 28){
deathmatchspawn[0]= 5950;
deathmatchspawn[1]= -1246;
deathmatchspawn[2]= 280;
deathmatchangles[1]= -175;
level.BlueSniperSpawnPoint = 0;
}
*/
		if ( !Q_stricmp( mapName, "factory" ) ) {
			if ( level.BlueSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -16;
				deathmatchspawn[1] = -1166;
				deathmatchspawn[2] = 360;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = 908;
				deathmatchspawn[1] = -996;
				deathmatchspawn[2] = 360;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = 1184;
				deathmatchspawn[1] = 1429;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = 1245;
				deathmatchspawn[1] = 1429;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = 911;
				deathmatchspawn[1] = -1106;
				deathmatchspawn[2] = 360;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = 913;
				deathmatchspawn[1] = -1150;
				deathmatchspawn[2] = 360;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = 1243;
				deathmatchspawn[1] = 1349;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = 1184;
				deathmatchspawn[1] = 1349;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = 795;
				deathmatchspawn[1] = -995;
				deathmatchspawn[2] = 360;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = 795;
				deathmatchspawn[1] = -1051;
				deathmatchspawn[2] = 360;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = 1243;
				deathmatchspawn[1] = 1285;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = 1183;
				deathmatchspawn[1] = 1287;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = 1183;
				deathmatchspawn[1] = 1214;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = 1242;
				deathmatchspawn[1] = 1214;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = 1183;
				deathmatchspawn[1] = 1156;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = 1241;
				deathmatchspawn[1] = 1155;
				deathmatchspawn[2] = 296;
				deathmatchangles[1] = -90;
				level.BlueSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "sfm" ) ) {
			if ( level.BlueSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -666;
				deathmatchspawn[1] = 1661;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -591;
				deathmatchspawn[1] = 1660;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = -507;
				deathmatchspawn[1] = 1662;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = -422;
				deathmatchspawn[1] = 1662;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -672;
				deathmatchspawn[1] = 1563;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -672;
				deathmatchspawn[1] = 1481;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = -590;
				deathmatchspawn[1] = 1561;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = -506;
				deathmatchspawn[1] = 1562;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -414;
				deathmatchspawn[1] = 1563;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = -422;
				deathmatchspawn[1] = 1484;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = -23;
				deathmatchspawn[1] = 1632;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = -23;
				deathmatchspawn[1] = 1569;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = -23;
				deathmatchspawn[1] = 1500;
				deathmatchspawn[2] = -127;
				deathmatchangles[1] = -180;
				level.BlueSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "dam" ) ) {
			if ( level.BlueSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = -65;
				deathmatchspawn[1] = 2508;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = -65;
				deathmatchspawn[1] = 2440;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = -1397;
				deathmatchspawn[1] = 1218;
				deathmatchspawn[2] = 2520;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = -1397;
				deathmatchspawn[1] = 1345;
				deathmatchspawn[2] = 2520;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = -66;
				deathmatchspawn[1] = 2372;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = -2;
				deathmatchspawn[1] = 2508;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = -1;
				deathmatchspawn[1] = 2440;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = -1397;
				deathmatchspawn[1] = 1281;
				deathmatchspawn[2] = 2520;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = -3;
				deathmatchspawn[1] = 2372;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = 62;
				deathmatchspawn[1] = 2508;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = -1150;
				deathmatchspawn[1] = 1197;
				deathmatchspawn[2] = 2520;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = -1156;
				deathmatchspawn[1] = 1410;
				deathmatchspawn[2] = 2520;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = 61;
				deathmatchspawn[1] = 2440;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = 62;
				deathmatchspawn[1] = 2372;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = -1285;
				deathmatchspawn[1] = 1470;
				deathmatchspawn[2] = 2520;
				deathmatchangles[1] = -90;
			} else if ( level.BlueSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = 191;
				deathmatchspawn[1] = 2371;
				deathmatchspawn[2] = 2648;
				deathmatchangles[1] = 90;
				level.BlueSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "dark" ) ) {
			if ( level.flagteam == TEAM_BLUE ) {
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -3073;
					deathmatchspawn[1] = 1609;
					deathmatchspawn[2] = 1272;
					deathmatchangles[1] = -1;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -1707;
					deathmatchspawn[1] = 345;
					deathmatchspawn[2] = 792;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -1707;
					deathmatchspawn[1] = 282;
					deathmatchspawn[2] = 792;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -1708;
					deathmatchspawn[1] = 35;
					deathmatchspawn[2] = 792;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -1708;
					deathmatchspawn[1] = -34;
					deathmatchspawn[2] = 792;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -1705;
					deathmatchspawn[1] = 157;
					deathmatchspawn[2] = 802;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -1706;
					deathmatchspawn[1] = 191;
					deathmatchspawn[2] = 896;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -1703;
					deathmatchspawn[1] = 129;
					deathmatchspawn[2] = 896;
					deathmatchangles[1] = 0;
					level.BlueSniperSpawnPoint = 0;
				}
			} else {
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -32;
					deathmatchspawn[1] = -5715;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -94;
					deathmatchspawn[1] = -5715;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -158;
					deathmatchspawn[1] = -5715;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -158;
					deathmatchspawn[1] = -5651;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -95;
					deathmatchspawn[1] = -5651;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -31;
					deathmatchspawn[1] = -5651;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -31;
					deathmatchspawn[1] = -5586;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -94;
					deathmatchspawn[1] = -5586;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = -158;
					deathmatchspawn[1] = -5586;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -158;
					deathmatchspawn[1] = -5522;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = -99;
					deathmatchspawn[1] = -5522;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = -32;
					deathmatchspawn[1] = -5522;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 12 )     {
					deathmatchspawn[0] = -31;
					deathmatchspawn[1] = -5460;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 13 )     {
					deathmatchspawn[0] = -96;
					deathmatchspawn[1] = -5459;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 14 )     {
					deathmatchspawn[0] = -159;
					deathmatchspawn[1] = -5459;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 15 )     {
					deathmatchspawn[0] = -157;
					deathmatchspawn[1] = -5396;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 16 )     {
					deathmatchspawn[0] = -93;
					deathmatchspawn[1] = -5396;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 17 )     {
					deathmatchspawn[0] = -31;
					deathmatchspawn[1] = -5396;
					deathmatchspawn[2] = 88;
					deathmatchangles[1] = 90;
					level.BlueSniperSpawnPoint = 0;
				}
			}
		}
		if ( !Q_stricmp( mapName, "assault" ) ) {
			if ( level.flagteam == TEAM_BLUE ) {
/*		if(level.BlueSniperSpawnPoint == 0){
deathmatchspawn[0]= 2962;
deathmatchspawn[1]= 3301;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.BlueSniperSpawnPoint == 1){
deathmatchspawn[0]= 2877;
deathmatchspawn[1]= 3301;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.BlueSniperSpawnPoint == 2){
deathmatchspawn[0]= 2790;
deathmatchspawn[1]= 3301;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.BlueSniperSpawnPoint == 3){
deathmatchspawn[0]= 2706;
deathmatchspawn[1]= 3301;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.BlueSniperSpawnPoint == 4){
deathmatchspawn[0]= 2965;
deathmatchspawn[1]= 3203;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.BlueSniperSpawnPoint == 5){
deathmatchspawn[0]= 2878;
deathmatchspawn[1]= 3202;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.BlueSniperSpawnPoint == 6){
deathmatchspawn[0]= 2788;
deathmatchspawn[1]= 3202;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
}else if(level.BlueSniperSpawnPoint == 7){
deathmatchspawn[0]= 2704;
deathmatchspawn[1]= 3198;
deathmatchspawn[2]= 472;
deathmatchangles[1]= -90;
level.BlueSniperSpawnPoint = 0;
}*/             if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -1275;
					deathmatchspawn[1] = 912;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -1352;
					deathmatchspawn[1] = 912;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -1425;
					deathmatchspawn[1] = 912;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -1502;
					deathmatchspawn[1] = 912;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -1566;
					deathmatchspawn[1] = 912;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -88;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -1630;
					deathmatchspawn[1] = 912;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -1702;
					deathmatchspawn[1] = 914;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -1662;
					deathmatchspawn[1] = 992;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -90;
					level.BlueSniperSpawnPoint = 0;
				}
			} else {
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -3026;
					deathmatchspawn[1] = 3791;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -3020;
					deathmatchspawn[1] = 3894;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -3011;
					deathmatchspawn[1] = 4009;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -3019;
					deathmatchspawn[1] = 4095;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -3019;
					deathmatchspawn[1] = 4193;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -3019;
					deathmatchspawn[1] = 4293;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -3016;
					deathmatchspawn[1] = 4418;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 180;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -3017;
					deathmatchspawn[1] = 4538;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = -180;
				} else if ( level.BlueSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = -4278;
					deathmatchspawn[1] = 3801;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -4277;
					deathmatchspawn[1] = 3905;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = -4277;
					deathmatchspawn[1] = 4007;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = -4277;
					deathmatchspawn[1] = 4098;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 12 )     {
					deathmatchspawn[0] = -4276;
					deathmatchspawn[1] = 4200;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 13 )     {
					deathmatchspawn[0] = -4276;
					deathmatchspawn[1] = 4299;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 14 )     {
					deathmatchspawn[0] = -4276;
					deathmatchspawn[1] = 4432;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 15 )     {
					deathmatchspawn[0] = -4276;
					deathmatchspawn[1] = 4560;
					deathmatchspawn[2] = 120;
					deathmatchangles[1] = 0;
					level.BlueSniperSpawnPoint = 0;
				}
			}
		}
		if ( !Q_stricmp( mapName, "norway" ) ) {
			if ( level.flagteam == TEAM_BLUE ) {
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1815;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = -1;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1756;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1701;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = -1;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1638;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = 2092;
					deathmatchspawn[1] = -1572;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = 2091;
					deathmatchspawn[1] = -1312;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = 2091;
					deathmatchspawn[1] = -1363;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = 2091;
					deathmatchspawn[1] = -1421;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = 2190;
					deathmatchspawn[1] = -1188;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = 2190;
					deathmatchspawn[1] = -1248;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = 2192;
					deathmatchspawn[1] = -1756;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = 2191;
					deathmatchspawn[1] = -1816;
					deathmatchspawn[2] = 496;
					deathmatchangles[1] = 0;
					level.BlueSniperSpawnPoint = 0;
				}
			} else {
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -2269;
					deathmatchspawn[1] = 4125;
					deathmatchspawn[2] = 517;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -2308;
					deathmatchspawn[1] = 4077;
					deathmatchspawn[2] = 517;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -2205;
					deathmatchspawn[1] = 4055;
					deathmatchspawn[2] = 517;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -2249;
					deathmatchspawn[1] = 4012;
					deathmatchspawn[2] = 517;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -2123;
					deathmatchspawn[1] = 3976;
					deathmatchspawn[2] = 517;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -2170;
					deathmatchspawn[1] = 3932;
					deathmatchspawn[2] = 517;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -2071;
					deathmatchspawn[1] = 3921;
					deathmatchspawn[2] = 517;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -2115;
					deathmatchspawn[1] = 3879;
					deathmatchspawn[2] = 517;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = -2336;
					deathmatchspawn[1] = 4193;
					deathmatchspawn[2] = 513;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -2384;
					deathmatchspawn[1] = 4148;
					deathmatchspawn[2] = 513;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = -2442;
					deathmatchspawn[1] = 4210;
					deathmatchspawn[2] = 506;
					deathmatchangles[1] = -45;
				} else if ( level.BlueSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = -2394;
					deathmatchspawn[1] = 4261;
					deathmatchspawn[2] = 506;
					deathmatchangles[1] = -45;
					level.BlueSniperSpawnPoint = 0;
				}
			}
		}


		if ( !Q_stricmp( mapName, "rocket" ) ) {
			if ( level.BlueSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -960;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -895;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -829;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -766;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -702;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -639;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -61;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -128;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -190;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -254;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -322;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = 3024;
				deathmatchspawn[1] = -383;
				deathmatchspawn[2] = 24;
				deathmatchangles[1] = -180;
				level.BlueSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "trainyard" ) ) {
			if ( level.BlueSniperSpawnPoint == 0 ) {
				deathmatchspawn[0] = 2080;
				deathmatchspawn[1] = -1107;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 1 )     {
				deathmatchspawn[0] = 2080;
				deathmatchspawn[1] = -1050;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = -180;
			} else if ( level.BlueSniperSpawnPoint == 2 )     {
				deathmatchspawn[0] = 1056;
				deathmatchspawn[1] = 183;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 3 )     {
				deathmatchspawn[0] = 1278;
				deathmatchspawn[1] = 134;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 4 )     {
				deathmatchspawn[0] = 2078;
				deathmatchspawn[1] = -991;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 5 )     {
				deathmatchspawn[0] = 2078;
				deathmatchspawn[1] = -933;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 6 )     {
				deathmatchspawn[0] = 1343;
				deathmatchspawn[1] = 135;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 7 )     {
				deathmatchspawn[0] = 1490;
				deathmatchspawn[1] = 135;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 90;
			} else if ( level.BlueSniperSpawnPoint == 8 )     {
				deathmatchspawn[0] = 2078;
				deathmatchspawn[1] = -1431;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 9 )     {
				deathmatchspawn[0] = 2078;
				deathmatchspawn[1] = -1376;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 10 )     {
				deathmatchspawn[0] = 1124;
				deathmatchspawn[1] = 423;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 11 )     {
				deathmatchspawn[0] = 1188;
				deathmatchspawn[1] = 422;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 12 )     {
				deathmatchspawn[0] = 2077;
				deathmatchspawn[1] = -1315;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 13 )     {
				deathmatchspawn[0] = 2077;
				deathmatchspawn[1] = -1258;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 180;
			} else if ( level.BlueSniperSpawnPoint == 14 )     {
				deathmatchspawn[0] = 1254;
				deathmatchspawn[1] = 423;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 0;
			} else if ( level.BlueSniperSpawnPoint == 15 )     {
				deathmatchspawn[0] = 1308;
				deathmatchspawn[1] = 424;
				deathmatchspawn[2] = 40;
				deathmatchangles[1] = 0;
				level.BlueSniperSpawnPoint = 0;
			}
		}
		if ( !Q_stricmp( mapName, "tram" ) ) {
			if ( level.flagteam == TEAM_BLUE ) {
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -214;
					deathmatchspawn[1] = 348;
					deathmatchspawn[2] = 472;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -145;
					deathmatchspawn[1] = 348;
					deathmatchspawn[2] = 472;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -217;
					deathmatchspawn[1] = 185;
					deathmatchspawn[2] = 816;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -217;
					deathmatchspawn[1] = 17;
					deathmatchspawn[2] = 816;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -38;
					deathmatchspawn[1] = 14;
					deathmatchspawn[2] = 838;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -38;
					deathmatchspawn[1] = 182;
					deathmatchspawn[2] = 838;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -39;
					deathmatchspawn[1] = 91;
					deathmatchspawn[2] = 838;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -216;
					deathmatchspawn[1] = 96;
					deathmatchspawn[2] = 816;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = -125;
					deathmatchspawn[1] = 187;
					deathmatchspawn[2] = 830;
					deathmatchangles[1] = -90;
				} else if ( level.BlueSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -123;
					deathmatchspawn[1] = -24;
					deathmatchspawn[2] = 821;
					deathmatchangles[1] = -90;
					level.BlueSniperSpawnPoint = 0;
				}
			} else {
				if ( level.BlueSniperSpawnPoint == 0 ) {
					deathmatchspawn[0] = -3224;
					deathmatchspawn[1] = 167;
					deathmatchspawn[2] = -487;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 1 )     {
					deathmatchspawn[0] = -3224;
					deathmatchspawn[1] = 221;
					deathmatchspawn[2] = -487;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 2 )     {
					deathmatchspawn[0] = -3152;
					deathmatchspawn[1] = 221;
					deathmatchspawn[2] = -487;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 3 )     {
					deathmatchspawn[0] = -3152;
					deathmatchspawn[1] = 167;
					deathmatchspawn[2] = -487;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 4 )     {
					deathmatchspawn[0] = -3071;
					deathmatchspawn[1] = 167;
					deathmatchspawn[2] = -487;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 5 )     {
					deathmatchspawn[0] = -3071;
					deathmatchspawn[1] = 218;
					deathmatchspawn[2] = -430;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 6 )     {
					deathmatchspawn[0] = -3102;
					deathmatchspawn[1] = -287;
					deathmatchspawn[2] = -615;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 7 )     {
					deathmatchspawn[0] = -3041;
					deathmatchspawn[1] = -287;
					deathmatchspawn[2] = -615;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 8 )     {
					deathmatchspawn[0] = -3041;
					deathmatchspawn[1] = -208;
					deathmatchspawn[2] = -615;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 9 )     {
					deathmatchspawn[0] = -3098;
					deathmatchspawn[1] = -210;
					deathmatchspawn[2] = -615;
					deathmatchangles[1] = 90;
				} else if ( level.BlueSniperSpawnPoint == 10 )     {
					deathmatchspawn[0] = -3222;
					deathmatchspawn[1] = -110;
					deathmatchspawn[2] = -615;
					deathmatchangles[1] = 0;
				} else if ( level.BlueSniperSpawnPoint == 11 )     {
					deathmatchspawn[0] = -3166;
					deathmatchspawn[1] = -110;
					deathmatchspawn[2] = -615;
					deathmatchangles[1] = 0;
					level.BlueSniperSpawnPoint = 0;
				}
			}
		}
	}

	VectorCopy( deathmatchspawn, spawn_origin );
	VectorCopy( deathmatchangles, spawn_angles );
}

/*
=======================
FreeForAllSpawnpt

+Supported Maps+

    -Beach
    -Base
    -Assault
    -Sub
    -Castle
    -Destruction
    -Depot
    -Village
    -xlabs
    -boss2

=======================
*/

void FreeForAllSpawnpt( gentity_t *ent, vec3_t spawn_origin, vec3_t spawn_angles ) {
	vec3_t deathmatchangles, deathmatchspawn;
	char mapName[MAX_QPATH];

	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );
	deathmatchangles[0] = 0;
	deathmatchangles[2] = 0; //i think this will fix the upsidedown heads

	//if they arnt on the axis make them that way
	if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
		ent->client->sess.sessionTeam = TEAM_RED;
	}

	level.DMspawnpoint++; //add 1 every time its called
	ent->client->pers.spawnNum++; //add one here so we can turn off the telefrag

	if ( !Q_stricmp( mapName, "mp_beach" ) ) {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = 2797;
			deathmatchspawn[1] = 2640;
			deathmatchspawn[2] = 536;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 1 )      {
			deathmatchspawn[0] = 3723;
			deathmatchspawn[1] = 1903;
			deathmatchspawn[2] = 536;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 2 )      {
			deathmatchspawn[0] = 2582;
			deathmatchspawn[1] = 1641;
			deathmatchspawn[2] = 920;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 3 )      {
			deathmatchspawn[0] = 3468;
			deathmatchspawn[1] = 2047;
			deathmatchspawn[2] = 920;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 4 )      {
			deathmatchspawn[0] = 2956;
			deathmatchspawn[1] = 3161;
			deathmatchspawn[2] = 920;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 5 )      {
			deathmatchspawn[0] = 2681;
			deathmatchspawn[1] = 3584;
			deathmatchspawn[2] = 624;
			deathmatchangles[1] = -180;
		} else if ( level.DMspawnpoint == 6 )      {
			deathmatchspawn[0] = 2789;
			deathmatchspawn[1] = 4128;
			deathmatchspawn[2] = 624;
			deathmatchangles[1] = -137;
		} else if ( level.DMspawnpoint == 7 )      {
			deathmatchspawn[0] = 1434;
			deathmatchspawn[1] = 3880;
			deathmatchspawn[2] = 616;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 8 )      {
			deathmatchspawn[0] = 2135;
			deathmatchspawn[1] = 2185;
			deathmatchspawn[2] = 557;
			deathmatchangles[1] = -180;
		} else if ( level.DMspawnpoint == 9 )      {
			deathmatchspawn[0] = 2434;
			deathmatchspawn[1] = 2941;
			deathmatchspawn[2] = 664;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 10 )      {
			deathmatchspawn[0] = 2744;
			deathmatchspawn[1] = 3013;
			deathmatchspawn[2] = 920;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 11 )      {
			deathmatchspawn[0] = 2413;
			deathmatchspawn[1] = 2989;
			deathmatchspawn[2] = 1176;
			deathmatchangles[1] = 45;
		} else if ( level.DMspawnpoint == 12 )      {
			deathmatchspawn[0] = 3940;
			deathmatchspawn[1] = 3136;
			deathmatchspawn[2] = 536;
			deathmatchangles[1] = -180;
		} else if ( level.DMspawnpoint == 13 )      {
			deathmatchspawn[0] = 2603;
			deathmatchspawn[1] = 1951;
			deathmatchspawn[2] = 472;
			deathmatchangles[1] = -37;
		} else if ( level.DMspawnpoint == 14 )      {
			deathmatchspawn[0] = 3536;
			deathmatchspawn[1] = 1905;
			deathmatchspawn[2] = 536;
			deathmatchangles[1] = -180;
		} else if ( level.DMspawnpoint == 15 )      {
			deathmatchspawn[0] = 3703;
			deathmatchspawn[1] = 1619;
			deathmatchspawn[2] = 696;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 16 )      {
			deathmatchspawn[0] = 2440;
			deathmatchspawn[1] = 2842;
			deathmatchspawn[2] = 1176;
			deathmatchangles[1] = -45;
		} else if ( level.DMspawnpoint == 17 )      {
			deathmatchspawn[0] = 2385;
			deathmatchspawn[1] = 2293;
			deathmatchspawn[2] = 920;
			deathmatchangles[1] = -45;
		} else if ( level.DMspawnpoint == 18 )      {
			deathmatchspawn[0] = 3234;
			deathmatchspawn[1] = 1160;
			deathmatchspawn[2] = 1176;
			deathmatchangles[1] = -180;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "mp_base" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = 1023;
			deathmatchspawn[1] = -147;
			deathmatchspawn[2] = 32;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -380;
			deathmatchspawn[1] = -620;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = -2209;
			deathmatchspawn[1] = -1529;
			deathmatchspawn[2] = 329;
			deathmatchangles[1] = 44;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -960;
			deathmatchspawn[1] = -1244;
			deathmatchspawn[2] = 416;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = -549;
			deathmatchspawn[1] = -740;
			deathmatchspawn[2] = 320;
			deathmatchangles[1] = 45;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 1183;
			deathmatchspawn[1] = -190;
			deathmatchspawn[2] = 424;
			deathmatchangles[1] = 136;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 383;
			deathmatchspawn[1] = 160;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = 1447;
			deathmatchspawn[1] = 1962;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = -2626;
			deathmatchspawn[1] = -557;
			deathmatchspawn[2] = 310;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = -2687;
			deathmatchspawn[1] = -1373;
			deathmatchspawn[2] = 319;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -2395;
			deathmatchspawn[1] = 2396;
			deathmatchspawn[2] = 120;
			deathmatchangles[1] = -42;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = -2372;
			deathmatchspawn[1] = 2369;
			deathmatchspawn[2] = 248;
			deathmatchangles[1] = -43;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -1753;
			deathmatchspawn[1] = 1494;
			deathmatchspawn[2] = 120;
			deathmatchangles[1] = -51;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = -768;
			deathmatchspawn[1] = 2210;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = -93;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = 1554;
			deathmatchspawn[1] = 3010;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 178;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = 2437;
			deathmatchspawn[1] = 2437;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = 1;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = 3009;
			deathmatchspawn[1] = 2314;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 2461;
			deathmatchspawn[1] = 1566;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = 178;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = 111;
			deathmatchspawn[1] = 2745;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = -91;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -1807;
			deathmatchspawn[1] = -249;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 160;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "mp_assault" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = -2400;
			deathmatchspawn[1] = -148;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -2471;
			deathmatchspawn[1] = 921;
			deathmatchspawn[2] = 440;
			deathmatchangles[1] = -41;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = -1243;
			deathmatchspawn[1] = 925;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -119;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -905;
			deathmatchspawn[1] = -166;
			deathmatchspawn[2] = 440;
			deathmatchangles[1] = 137;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = 300;
			deathmatchspawn[1] = 2014;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -47;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 1266;
			deathmatchspawn[1] = 739;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 146;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 1472;
			deathmatchspawn[1] = 764;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = 3178;
			deathmatchspawn[1] = -187;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 87;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = 3796;
			deathmatchspawn[1] = -191;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 93;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = 4260;
			deathmatchspawn[1] = -208;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = 4243;
			deathmatchspawn[1] = -433;
			deathmatchspawn[2] = 536;
			deathmatchangles[1] = 136;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = 3772;
			deathmatchspawn[1] = -482;
			deathmatchspawn[2] = 472;
			deathmatchangles[1] = 125;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = 3762;
			deathmatchspawn[1] = 1602;
			deathmatchspawn[2] = 404;
			deathmatchangles[1] = 149;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = 3799;
			deathmatchspawn[1] = 2556;
			deathmatchspawn[2] = 404;
			deathmatchangles[1] = -135;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = 3764;
			deathmatchspawn[1] = 4945;
			deathmatchspawn[2] = 622;
			deathmatchangles[1] = -114;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = 2835;
			deathmatchspawn[1] = 3601;
			deathmatchspawn[2] = 792;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = 3540;
			deathmatchspawn[1] = 3038;
			deathmatchspawn[2] = 72;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 1560;
			deathmatchspawn[1] = 3192;
			deathmatchspawn[2] = -147;
			deathmatchangles[1] = -45;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = 1216;
			deathmatchspawn[1] = 2601;
			deathmatchspawn[2] = -535;
			deathmatchangles[1] = -87;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = 1968;
			deathmatchspawn[1] = 2270;
			deathmatchspawn[2] = -535;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = 815;
			deathmatchspawn[1] = 1793;
			deathmatchspawn[2] = -535;
			deathmatchangles[1] = 44;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = 957;
			deathmatchspawn[1] = 1782;
			deathmatchspawn[2] = -87;
			deathmatchangles[1] = 128;
		} else if ( level.DMspawnpoint == 22 )     {
			deathmatchspawn[0] = -640;
			deathmatchspawn[1] = 2749;
			deathmatchspawn[2] = -663;
			deathmatchangles[1] = -40;
		} else if ( level.DMspawnpoint == 23 )     {
			deathmatchspawn[0] = -1824;
			deathmatchspawn[1] = 2126;
			deathmatchspawn[2] = -535;
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 24 )     {
			deathmatchspawn[0] = -1055;
			deathmatchspawn[1] = 1167;
			deathmatchspawn[2] = -535;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 25 )     {
			deathmatchspawn[0] = -1614;
			deathmatchspawn[1] = 755;
			deathmatchspawn[2] = -241;
			deathmatchangles[1] = 136;
		} else if ( level.DMspawnpoint == 26 )     {
			deathmatchspawn[0] = 3531;
			deathmatchspawn[1] = 2765;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -135;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "mp_sub" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = -604;
			deathmatchspawn[1] = 2553;
			deathmatchspawn[2] = 504;
			deathmatchangles[1] = 3;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -603;
			deathmatchspawn[1] = 2042;
			deathmatchspawn[2] = 504;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = 1283;
			deathmatchspawn[1] = 3455;
			deathmatchspawn[2] = 504;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = 626;
			deathmatchspawn[1] = 3456;
			deathmatchspawn[2] = 504;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = 621;
			deathmatchspawn[1] = 3456;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 1277;
			deathmatchspawn[1] = 3454;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 1512;
			deathmatchspawn[1] = 2483;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 147;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = 1489;
			deathmatchspawn[1] = 2241;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = -177;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = 1475;
			deathmatchspawn[1] = 1729;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 178;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = 409;
			deathmatchspawn[1] = 1983;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -407;
			deathmatchspawn[1] = 1493;
			deathmatchspawn[2] = 408;
			deathmatchangles[1] = -50;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = -92;
			deathmatchspawn[1] = 481;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = -134;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -846;
			deathmatchspawn[1] = -45;
			deathmatchspawn[2] = 512;
			deathmatchangles[1] = -41;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = -481;
			deathmatchspawn[1] = -830;
			deathmatchspawn[2] = 512;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = 1030;
			deathmatchspawn[1] = 504;
			deathmatchspawn[2] = 512;
			deathmatchangles[1] = -177;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = 190;
			deathmatchspawn[1] = 574;
			deathmatchspawn[2] = 680;
			deathmatchangles[1] = 1;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = 1178;
			deathmatchspawn[1] = 81;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 563;
			deathmatchspawn[1] = -316;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = -50;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = -736;
			deathmatchspawn[1] = -641;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = 1242;
			deathmatchspawn[1] = -1241;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 132;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = 1968;
			deathmatchspawn[1] = -1573;
			deathmatchspawn[2] = 502;
			deathmatchangles[1] = 138;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = 2512;
			deathmatchspawn[1] = -824;
			deathmatchspawn[2] = 518;
			deathmatchangles[1] = -138;
		} else if ( level.DMspawnpoint == 22 )     {
			deathmatchspawn[0] = 2548;
			deathmatchspawn[1] = -647;
			deathmatchspawn[2] = 514;
			deathmatchangles[1] = 134;
		} else if ( level.DMspawnpoint == 23 )     {
			deathmatchspawn[0] = 1964;
			deathmatchspawn[1] = -9;
			deathmatchspawn[2] = 512;
			deathmatchangles[1] = -38;
		} else if ( level.DMspawnpoint == 24 )     {
			deathmatchspawn[0] = 960;
			deathmatchspawn[1] = 1805;
			deathmatchspawn[2] = 342;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 25 )     {
			deathmatchspawn[0] = 1332;
			deathmatchspawn[1] = 2483;
			deathmatchspawn[2] = 500;
			deathmatchangles[1] = 51;
		} else if ( level.DMspawnpoint == 26 )     {
			deathmatchspawn[0] = 1397;
			deathmatchspawn[1] = 602;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = -42;
		} else if ( level.DMspawnpoint == 27 )     {
			deathmatchspawn[0] = 1998;
			deathmatchspawn[1] = 49;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 136;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "mp_castle" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = 924;
			deathmatchspawn[1] = -81;
			deathmatchspawn[2] = -353;
			deathmatchangles[1] = 128;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -451;
			deathmatchspawn[1] = 476;
			deathmatchspawn[2] = -303;
			deathmatchangles[1] = -86;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = 191;
			deathmatchspawn[1] = 471;
			deathmatchspawn[2] = -303;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -522;
			deathmatchspawn[1] = 743;
			deathmatchspawn[2] = -239;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = -529;
			deathmatchspawn[1] = 1052;
			deathmatchspawn[2] = -239;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = -297;
			deathmatchspawn[1] = 1601;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -39;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = -132;
			deathmatchspawn[1] = 1226;
			deathmatchspawn[2] = 440;
			deathmatchangles[1] = -88;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = -535;
			deathmatchspawn[1] = -801;
			deathmatchspawn[2] = 440;
			deathmatchangles[1] = 56;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = 269;
			deathmatchspawn[1] = -334;
			deathmatchspawn[2] = 440;
			deathmatchangles[1] = 135;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = -538;
			deathmatchspawn[1] = 475;
			deathmatchspawn[2] = 440;
			deathmatchangles[1] = -42;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = 1545;
			deathmatchspawn[1] = 875;
			deathmatchspawn[2] = 496;
			deathmatchangles[1] = 134;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = 1091;
			deathmatchspawn[1] = 745;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = 632;
			deathmatchspawn[1] = 1656;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = -42;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = 668;
			deathmatchspawn[1] = 1641;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -91;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = 1519;
			deathmatchspawn[1] = 1639;
			deathmatchspawn[2] = 64;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = 1453;
			deathmatchspawn[1] = 321;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 46;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = 1599;
			deathmatchspawn[1] = -105;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 898;
			deathmatchspawn[1] = -155;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = 626;
			deathmatchspawn[1] = 1775;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = 45;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -371;
			deathmatchspawn[1] = 649;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 43;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = 120;
			deathmatchspawn[1] = 1141;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -135;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = 1291;
			deathmatchspawn[1] = 1775;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = 45;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "mp_destruction" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = 2260;
			deathmatchspawn[1] = -990;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = 152;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = 2204;
			deathmatchspawn[1] = -2048;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = 694;
			deathmatchspawn[1] = -2725;
			deathmatchspawn[2] = -111;
			deathmatchangles[1] = 3;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = 1378;
			deathmatchspawn[1] = -1548;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = 46;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = 923;
			deathmatchspawn[1] = -1302;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = -312;
			deathmatchspawn[1] = -1561;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 141;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = -226;
			deathmatchspawn[1] = -1240;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = -37;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = -290;
			deathmatchspawn[1] = -392;
			deathmatchspawn[2] = -175;
			deathmatchangles[1] = -178;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = -1340;
			deathmatchspawn[1] = 566;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = -123;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = -1326;
			deathmatchspawn[1] = 303;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 135;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -751;
			deathmatchspawn[1] = 372;
			deathmatchspawn[2] = 96;
			deathmatchangles[1] = 124;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = -1048;
			deathmatchspawn[1] = 808;
			deathmatchspawn[2] = -175;
			deathmatchangles[1] = -140;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = 104;
			deathmatchspawn[1] = 324;
			deathmatchspawn[2] = -175;
			deathmatchangles[1] = 88;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = 341;
			deathmatchspawn[1] = 905;
			deathmatchspawn[2] = -143;
			deathmatchangles[1] = -5;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = 339;
			deathmatchspawn[1] = 562;
			deathmatchspawn[2] = -143;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = 1011;
			deathmatchspawn[1] = 948;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = 1807;
			deathmatchspawn[1] = 1124;
			deathmatchspawn[2] = 216;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 573;
			deathmatchspawn[1] = 291;
			deathmatchspawn[2] = 31;
			deathmatchangles[1] = 86;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = 1120;
			deathmatchspawn[1] = 397;
			deathmatchspawn[2] = 104;
			deathmatchangles[1] = -122;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = 1400;
			deathmatchspawn[1] = 185;
			deathmatchspawn[2] = 200;
			deathmatchangles[1] = 36;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = 1718;
			deathmatchspawn[1] = 333;
			deathmatchspawn[2] = 96;
			deathmatchangles[1] = -39;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = 3145;
			deathmatchspawn[1] = 991;
			deathmatchspawn[2] = -175;
			deathmatchangles[1] = -41;
		} else if ( level.DMspawnpoint == 22 )     {
			deathmatchspawn[0] = 4069;
			deathmatchspawn[1] = 682;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 134;
		} else if ( level.DMspawnpoint == 23 )     {
			deathmatchspawn[0] = 3941;
			deathmatchspawn[1] = 566;
			deathmatchspawn[2] = -119;
			deathmatchangles[1] = 47;
		} else if ( level.DMspawnpoint == 24 )     {
			deathmatchspawn[0] = 4808;
			deathmatchspawn[1] = 1646;
			deathmatchspawn[2] = -31;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 25 )     {
			deathmatchspawn[0] = 4189;
			deathmatchspawn[1] = 2250;
			deathmatchspawn[2] = -21;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 26 )     {
			deathmatchspawn[0] = 3854;
			deathmatchspawn[1] = 2800;
			deathmatchspawn[2] = 125;
			deathmatchangles[1] = 134;
		} else if ( level.DMspawnpoint == 27 )     {
			deathmatchspawn[0] = 2302;
			deathmatchspawn[1] = 3032;
			deathmatchspawn[2] = -183;
			deathmatchangles[1] = -84;
		} else if ( level.DMspawnpoint == 28 )     {
			deathmatchspawn[0] = 1176;
			deathmatchspawn[1] = 2250;
			deathmatchspawn[2] = -303;
			deathmatchangles[1] = 82;
		} else if ( level.DMspawnpoint == 29 )     {
			deathmatchspawn[0] = 2223;
			deathmatchspawn[1] = 4426;
			deathmatchspawn[2] = -223;
			deathmatchangles[1] = -22;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "mp_depot" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = -12144;
			deathmatchspawn[1] = 5307;
			deathmatchspawn[2] = 616;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -12923;
			deathmatchspawn[1] = 6129;
			deathmatchspawn[2] = 8;
			deathmatchangles[1] = -45;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = -12542;
			deathmatchspawn[1] = 5822;
			deathmatchspawn[2] = 904;
			deathmatchangles[1] = 38;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -11443;
			deathmatchspawn[1] = 5291;
			deathmatchspawn[2] = 8;
			deathmatchangles[1] = 135;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = -12146;
			deathmatchspawn[1] = 5429;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = 15;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = -11456;
			deathmatchspawn[1] = 5834;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = -138;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = -10004;
			deathmatchspawn[1] = 3663;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = -38;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = -8945;
			deathmatchspawn[1] = 4531;
			deathmatchspawn[2] = -207;
			deathmatchangles[1] = -176;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = -10229;
			deathmatchspawn[1] = 4469;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = 169;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = -8832;
			deathmatchspawn[1] = 3471;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = 92;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -8781;
			deathmatchspawn[1] = 2999;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = 177;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = -7999;
			deathmatchspawn[1] = 2840;
			deathmatchspawn[2] = -235;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -8669;
			deathmatchspawn[1] = 3674;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = -50;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = -8055;
			deathmatchspawn[1] = 4027;
			deathmatchspawn[2] = 312;
			deathmatchangles[1] = -47;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = -7284;
			deathmatchspawn[1] = 3442;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 52;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = -6582;
			deathmatchspawn[1] = 4082;
			deathmatchspawn[2] = -295;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = -8046;
			deathmatchspawn[1] = 4132;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = -5304;
			deathmatchspawn[1] = 1656;
			deathmatchspawn[2] = -223;
			deathmatchangles[1] = 177;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = -5478;
			deathmatchspawn[1] = 1379;
			deathmatchspawn[2] = -23;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -7647;
			deathmatchspawn[1] = 2063;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = -7648;
			deathmatchspawn[1] = 1240;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = -178;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = -8644;
			deathmatchspawn[1] = 1943;
			deathmatchspawn[2] = -359;
			deathmatchangles[1] = -2;
		} else if ( level.DMspawnpoint == 22 )     {
			deathmatchspawn[0] = -8022;
			deathmatchspawn[1] = 2201;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = 61;
		} else if ( level.DMspawnpoint == 23 )     {
			deathmatchspawn[0] = -8553;
			deathmatchspawn[1] = 2453;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = -55;
		} else if ( level.DMspawnpoint == 24 )     {
			deathmatchspawn[0] = -9080;
			deathmatchspawn[1] = 1481;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = -177;
		} else if ( level.DMspawnpoint == 25 )     {
			deathmatchspawn[0] = -9438;
			deathmatchspawn[1] = 1367;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = 125;
		} else if ( level.DMspawnpoint == 26 )     {
			deathmatchspawn[0] = -10318;
			deathmatchspawn[1] = 2489;
			deathmatchspawn[2] = -223;
			deathmatchangles[1] = -72;
		} else if ( level.DMspawnpoint == 27 )     {
			deathmatchspawn[0] = -9658;
			deathmatchspawn[1] = 1895;
			deathmatchspawn[2] = 40;
			deathmatchangles[1] = 146;
		} else if ( level.DMspawnpoint == 28 )     {
			deathmatchspawn[0] = -9893;
			deathmatchspawn[1] = 1698;
			deathmatchspawn[2] = -797;
			deathmatchangles[1] = 140;
		} else if ( level.DMspawnpoint == 29 )      {
			deathmatchspawn[0] = -12313;
			deathmatchspawn[1] = 6051;
			deathmatchspawn[2] = -735;
			deathmatchangles[1] = -126;
		} else if ( level.DMspawnpoint == 30 )     {
			deathmatchspawn[0] = -12920;
			deathmatchspawn[1] = 6135;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = -52;
		} else if ( level.DMspawnpoint == 31 )     {
			deathmatchspawn[0] = -11845;
			deathmatchspawn[1] = 3292;
			deathmatchspawn[2] = -271;
			deathmatchangles[1] = 100;
			level.DMspawnpoint = -1;
		}

	} else if ( !Q_stricmp( mapName, "mp_village" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = -675;
			deathmatchspawn[1] = -523;
			deathmatchspawn[2] = -167;
			deathmatchangles[1] = 88;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = 191;
			deathmatchspawn[1] = 4318;
			deathmatchspawn[2] = -131;
			deathmatchangles[1] = -132;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = -886;
			deathmatchspawn[1] = 3421;
			deathmatchspawn[2] = -43;
			deathmatchangles[1] = 88;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -678;
			deathmatchspawn[1] = 4911;
			deathmatchspawn[2] = -50;
			deathmatchangles[1] = -47;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = -252;
			deathmatchspawn[1] = -1195;
			deathmatchspawn[2] = -199;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = -1177;
			deathmatchspawn[1] = 4071;
			deathmatchspawn[2] = -287;
			deathmatchangles[1] = 40;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 227;
			deathmatchspawn[1] = -1185;
			deathmatchspawn[2] = -119;
			deathmatchangles[1] = -137;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = -1063;
			deathmatchspawn[1] = 3034;
			deathmatchspawn[2] = -287;
			deathmatchangles[1] = 121;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = 199;
			deathmatchspawn[1] = -1940;
			deathmatchspawn[2] = -151;
			deathmatchangles[1] = 39;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = -2468;
			deathmatchspawn[1] = 2517;
			deathmatchspawn[2] = -135;
			deathmatchangles[1] = -41;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -1709;
			deathmatchspawn[1] = 2239;
			deathmatchspawn[2] = -135;
			deathmatchangles[1] = 130;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = -1276;
			deathmatchspawn[1] = 3078;
			deathmatchspawn[2] = -135;
			deathmatchangles[1] = -146;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -768;
			deathmatchspawn[1] = 491;
			deathmatchspawn[2] = -159;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = -1488;
			deathmatchspawn[1] = 640;
			deathmatchspawn[2] = -159;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = -2789;
			deathmatchspawn[1] = 1539;
			deathmatchspawn[2] = 96;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = -2777;
			deathmatchspawn[1] = 898;
			deathmatchspawn[2] = 96;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = -1118;
			deathmatchspawn[1] = -358;
			deathmatchspawn[2] = -151;
			deathmatchangles[1] = -88;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = -799;
			deathmatchspawn[1] = -348;
			deathmatchspawn[2] = -119;
			deathmatchangles[1] = -92;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = -1753;
			deathmatchspawn[1] = -192;
			deathmatchspawn[2] = -159;
			deathmatchangles[1] = -177;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -1127;
			deathmatchspawn[1] = 488;
			deathmatchspawn[2] = -159;
			deathmatchangles[1] = -92;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = -1638;
			deathmatchspawn[1] = -1152;
			deathmatchspawn[2] = -159;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = 265;
			deathmatchspawn[1] = 438;
			deathmatchspawn[2] = -175;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 22 )     {
			deathmatchspawn[0] = 705;
			deathmatchspawn[1] = 545;
			deathmatchspawn[2] = -94;
			deathmatchangles[1] = -92;
		} else if ( level.DMspawnpoint == 23 )     {
			deathmatchspawn[0] = 357;
			deathmatchspawn[1] = 1708;
			deathmatchspawn[2] = 218;
			deathmatchangles[1] = -44;
		} else if ( level.DMspawnpoint == 24 )     {
			deathmatchspawn[0] = 571; //agent fix. players spawn in a wall lol
			deathmatchspawn[1] = 2200;
			deathmatchspawn[2] = 32;
			deathmatchangles[1] = -39;
		} else if ( level.DMspawnpoint == 25 )     {
			deathmatchspawn[0] = -373;
			deathmatchspawn[1] = 2697;
			deathmatchspawn[2] = 129;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 26 )     {
			deathmatchspawn[0] = 1453;
			deathmatchspawn[1] = 2312;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = -178;
		} else if ( level.DMspawnpoint == 27 )     {
			deathmatchspawn[0] = 1436;
			deathmatchspawn[1] = 1858;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = -178;
		} else if ( level.DMspawnpoint == 28 )     {
			deathmatchspawn[0] = -768;
			deathmatchspawn[1] = 27;
			deathmatchspawn[2] = -159;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 29 )     {
			deathmatchspawn[0] = -2336;
			deathmatchspawn[1] = -1126;
			deathmatchspawn[2] = -159;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 30 )     {
			deathmatchspawn[0] = -2375;
			deathmatchspawn[1] = 678;
			deathmatchspawn[2] = -151;
			deathmatchangles[1] = 87;
		} else if ( level.DMspawnpoint == 31 )     {
			deathmatchspawn[0] = -2115;
			deathmatchspawn[1] = 679;
			deathmatchspawn[2] = -151;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 32 )     {
			deathmatchspawn[0] = -150;
			deathmatchspawn[1] = 2111;
			deathmatchspawn[2] = -139;
			deathmatchangles[1] = 9;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "xlabs" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = 1664;
			deathmatchspawn[1] = -1680;
			deathmatchspawn[2] = -15;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = 1505;
			deathmatchspawn[1] = -1683;
			deathmatchspawn[2] = -15;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = 1344;
			deathmatchspawn[1] = -1667;
			deathmatchspawn[2] = -15;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = 1191;
			deathmatchspawn[1] = -496;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = -1;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = 1252;
			deathmatchspawn[1] = 190;
			deathmatchspawn[2] = -15;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 1181;
			deathmatchspawn[1] = -290;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = 49;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 707;
			deathmatchspawn[1] = -572;
			deathmatchspawn[2] = 16;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = 730;
			deathmatchspawn[1] = -127;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = 289;
			deathmatchspawn[1] = -603;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = 63;
			deathmatchspawn[1] = -2273;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = 88;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -43;
			deathmatchspawn[1] = -1694;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = -1;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = 173;
			deathmatchspawn[1] = -1380;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = 176;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -865;
			deathmatchspawn[1] = -802;
			deathmatchspawn[2] = 16;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = 63;
			deathmatchspawn[1] = -1696;
			deathmatchspawn[2] = 16;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = -151;
			deathmatchspawn[1] = -738;
			deathmatchspawn[2] = 16;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = -284;
			deathmatchspawn[1] = 930;
			deathmatchspawn[2] = 16;
			deathmatchangles[1] = -134;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = -542;
			deathmatchspawn[1] = -1152;
			deathmatchspawn[2] = 16;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = -1387;
			deathmatchspawn[1] = -1763;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = -645;
			deathmatchspawn[1] = -2829;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -396;
			deathmatchspawn[1] = 51;
			deathmatchspawn[2] = 16;
			deathmatchangles[1] = 178;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = 867;
			deathmatchspawn[1] = -1188;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = 138;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = 1221;
			deathmatchspawn[1] = -762;
			deathmatchspawn[2] = 224;
			deathmatchangles[1] = 0;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "boss2" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = 602;
			deathmatchspawn[1] = 735;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = 138;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -611;
			deathmatchspawn[1] = 1701;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = -38;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = 606;
			deathmatchspawn[1] = 1003;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -279;
			deathmatchspawn[1] = 2607;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = 281;
			deathmatchspawn[1] = 2613;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = -88;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 610;
			deathmatchspawn[1] = 1698;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -135;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 227;
			deathmatchspawn[1] = -1251;
			deathmatchspawn[2] = -55;
			deathmatchangles[1] = 135;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = -222;
			deathmatchspawn[1] = -802;
			deathmatchspawn[2] = -55;
			deathmatchangles[1] = -46;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = 30;
			deathmatchspawn[1] = -1724;
			deathmatchspawn[2] = 231;
			deathmatchangles[1] = 1;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = 733;
			deathmatchspawn[1] = -1727;
			deathmatchspawn[2] = 230;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = 848;
			deathmatchspawn[1] = -2246;
			deathmatchspawn[2] = -39;
			deathmatchangles[1] = 93;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = 186;
			deathmatchspawn[1] = 1913;
			deathmatchspawn[2] = -19;
			deathmatchangles[1] = 41;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -613;
			deathmatchspawn[1] = 731;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = 46;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = 606;
			deathmatchspawn[1] = 1694;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = -141;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = 1269;
			deathmatchspawn[1] = 1214;
			deathmatchspawn[2] = 568;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = 841;
			deathmatchspawn[1] = 1216;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = 1;
			deathmatchspawn[1] = 2656;
			deathmatchspawn[2] = -103;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 609;
			deathmatchspawn[1] = 732;
			deathmatchspawn[2] = 56;
			deathmatchangles[1] = 142;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = 807;
			deathmatchspawn[1] = -1726;
			deathmatchspawn[2] = 241;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -38;
			deathmatchspawn[1] = -1728;
			deathmatchspawn[2] = 242;
			deathmatchangles[1] = -179;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "church" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = -1570;
			deathmatchspawn[1] = 418;
			deathmatchspawn[2] = 192;
			deathmatchangles[1] = -87;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -2175;
			deathmatchspawn[1] = 242;
			deathmatchspawn[2] = 152;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = -1451;
			deathmatchspawn[1] = 43;
			deathmatchspawn[2] = 152;
			deathmatchangles[1] = 134;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -98;
			deathmatchspawn[1] = 588;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 176;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = -88;
			deathmatchspawn[1] = -7;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = -178;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 421;
			deathmatchspawn[1] = -6;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = -177;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 24;
			deathmatchspawn[1] = -6;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = 420;
			deathmatchspawn[1] = 583;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = 1766;
			deathmatchspawn[1] = 89;
			deathmatchspawn[2] = 312;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = 581;
			deathmatchspawn[1] = -278;
			deathmatchspawn[2] = 608;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -544;
			deathmatchspawn[1] = 775;
			deathmatchspawn[2] = 664;
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = -707;
			deathmatchspawn[1] = 771;
			deathmatchspawn[2] = 1120;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -707;
			deathmatchspawn[1] = -198;
			deathmatchspawn[2] = 1120;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = 527;
			deathmatchspawn[1] = 432;
			deathmatchspawn[2] = 600;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = 677;
			deathmatchspawn[1] = 1200;
			deathmatchspawn[2] = 600;
			deathmatchangles[1] = -123;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = 2242;
			deathmatchspawn[1] = 287;
			deathmatchspawn[2] = 664;
			deathmatchangles[1] = 179;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = 1424;
			deathmatchspawn[1] = 549;
			deathmatchspawn[2] = 632;
			deathmatchangles[1] = -91;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 1420;
			deathmatchspawn[1] = 30;
			deathmatchspawn[2] = 632;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = 421;
			deathmatchspawn[1] = 101;
			deathmatchspawn[2] = 1112;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -543;
			deathmatchspawn[1] = -198;
			deathmatchspawn[2] = 664;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = -714;
			deathmatchspawn[1] = 91;
			deathmatchspawn[2] = 88;
			deathmatchangles[1] = 90;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "rocket" ) )      {
//This spawn point below spawns in the cealing
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = 2676;
			deathmatchspawn[1] = -752;
			deathmatchspawn[2] = 72; //was 68
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = 3038;
			deathmatchspawn[1] = -34;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = -133;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = 1818;
			deathmatchspawn[1] = -903;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = 27;
//This spawn point below spawns in the cealing
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = 1279;
			deathmatchspawn[1] = 866;
			deathmatchspawn[2] = 220; //was 230
			deathmatchangles[1] = 90;
//This spawn point below spawns in the cealing
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = 1278;
			deathmatchspawn[1] = 726;
			deathmatchspawn[2] = 220; //was 230
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 1890;
			deathmatchspawn[1] = -1058;
			deathmatchspawn[2] = -359;
			deathmatchangles[1] = 135;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 1117;
			deathmatchspawn[1] = -30;
			deathmatchspawn[2] = -359;
			deathmatchangles[1] = -45;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = 264;
			deathmatchspawn[1] = 177;
			deathmatchspawn[2] = -356;
			deathmatchangles[1] = -115;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = -2150;
			deathmatchspawn[1] = 1202;
			deathmatchspawn[2] = -359;
			deathmatchangles[1] = -65;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = -1555;
			deathmatchspawn[1] = 948;
			deathmatchspawn[2] = -359;
			deathmatchangles[1] = 178;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -1852;
			deathmatchspawn[1] = 601;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = -2179;
			deathmatchspawn[1] = 609;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -1980;
			deathmatchspawn[1] = 1173;
			deathmatchspawn[2] = 608;
			deathmatchangles[1] = 97;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = -2367;
			deathmatchspawn[1] = 1178;
			deathmatchspawn[2] = 608;
			deathmatchangles[1] = 90;
//This spawn point below spawns in the cealing
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = -1644;
			deathmatchspawn[1] = 1317;
			deathmatchspawn[2] = 608;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = -1556;
			deathmatchspawn[1] = 1674;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 143;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = -160;
			deathmatchspawn[1] = 1439;
			deathmatchspawn[2] = 336;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 194;
			deathmatchspawn[1] = 1596;
			deathmatchspawn[2] = 152;
			deathmatchangles[1] = -127;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = -1724;
			deathmatchspawn[1] = 490;
			deathmatchspawn[2] = 152;
			deathmatchangles[1] = -6;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = 1172;
			deathmatchspawn[1] = -899;
			deathmatchspawn[2] = 152;
			deathmatchangles[1] = 1;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = 861;
			deathmatchspawn[1] = -316;
			deathmatchspawn[2] = 152;
			deathmatchangles[1] = -42;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = 1449;
			deathmatchspawn[1] = -278;
			deathmatchspawn[2] = 152;
			deathmatchangles[1] = -134;
		} else if ( level.DMspawnpoint == 22 )     {
			deathmatchspawn[0] = 1441;
			deathmatchspawn[1] = -288;
			deathmatchspawn[2] = 320;
			deathmatchangles[1] = -126;
		} else if ( level.DMspawnpoint == 23 )     {
			deathmatchspawn[0] = 3044;
			deathmatchspawn[1] = -987;
			deathmatchspawn[2] = 320;
			deathmatchangles[1] = 179;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "crypt2" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = 321;
			deathmatchspawn[1] = -1001;
			deathmatchspawn[2] = 160;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -57;
			deathmatchspawn[1] = -185;
			deathmatchspawn[2] = 160;
			deathmatchangles[1] = -134;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = -455;
			deathmatchspawn[1] = -583;
			deathmatchspawn[2] = 160;
			deathmatchangles[1] = 44;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -490;
			deathmatchspawn[1] = -379;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = 254;
			deathmatchspawn[1] = 41;
			deathmatchspawn[2] = 392;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 512;
			deathmatchspawn[1] = 87;
			deathmatchspawn[2] = 376;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = 63;
			deathmatchspawn[1] = 38;
			deathmatchspawn[2] = 392;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = -342;
			deathmatchspawn[1] = 218;
			deathmatchspawn[2] = 175;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = -896;
			deathmatchspawn[1] = 281;
			deathmatchspawn[2] = 96;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = -481;
			deathmatchspawn[1] = -939;
			deathmatchspawn[2] = 385;
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = -1311;
			deathmatchspawn[1] = -221;
			deathmatchspawn[2] = -55;
			deathmatchangles[1] = 178;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = -1637;
			deathmatchspawn[1] = -933;
			deathmatchspawn[2] = -55;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = -1114;
			deathmatchspawn[1] = -1956;
			deathmatchspawn[2] = -55;
			deathmatchangles[1] = 134;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = -2441;
			deathmatchspawn[1] = -1605;
			deathmatchspawn[2] = -19;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = -1803;
			deathmatchspawn[1] = -1248;
			deathmatchspawn[2] = -55;
			deathmatchangles[1] = -47;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = -1472;
			deathmatchspawn[1] = -2540;
			deathmatchspawn[2] = -55;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = -798;
			deathmatchspawn[1] = -1595;
			deathmatchspawn[2] = -55;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = -1470;
			deathmatchspawn[1] = -754;
			deathmatchspawn[2] = 288;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = -1471;
			deathmatchspawn[1] = -2531;
			deathmatchspawn[2] = 252;
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -1147;
			deathmatchspawn[1] = -2494;
			deathmatchspawn[2] = 264;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = -1814;
			deathmatchspawn[1] = -2495;
			deathmatchspawn[2] = 264;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = -24;
			deathmatchspawn[1] = -383;
			deathmatchspawn[2] = 344;
			deathmatchangles[1] = 179;
			level.DMspawnpoint = -1;
		}
	} else if ( !Q_stricmp( mapName, "factory" ) )      {
		if ( level.DMspawnpoint == 0 ) {
			deathmatchspawn[0] = -478;
			deathmatchspawn[1] = -954;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 1 )     {
			deathmatchspawn[0] = -129;
			deathmatchspawn[1] = -1109;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = 89;
		} else if ( level.DMspawnpoint == 2 )     {
			deathmatchspawn[0] = -15;
			deathmatchspawn[1] = -1054;
			deathmatchspawn[2] = 360;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 3 )     {
			deathmatchspawn[0] = -25;
			deathmatchspawn[1] = -874;
			deathmatchspawn[2] = 170;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 4 )     {
			deathmatchspawn[0] = 1211;
			deathmatchspawn[1] = 1442;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = -89;
		} else if ( level.DMspawnpoint == 5 )     {
			deathmatchspawn[0] = 94;
			deathmatchspawn[1] = 33;
			deathmatchspawn[2] = 42;
			deathmatchangles[1] = 91;
		} else if ( level.DMspawnpoint == 6 )     {
			deathmatchspawn[0] = -89;
			deathmatchspawn[1] = 288;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 7 )     {
			deathmatchspawn[0] = 864;
			deathmatchspawn[1] = 161;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 8 )     {
			deathmatchspawn[0] = 1187;
			deathmatchspawn[1] = 546;
			deathmatchspawn[2] = -231;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 9 )     {
			deathmatchspawn[0] = 611;
			deathmatchspawn[1] = 340;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 10 )     {
			deathmatchspawn[0] = 544;
			deathmatchspawn[1] = 343;
			deathmatchspawn[2] = 103;
			deathmatchangles[1] = -90;
		} else if ( level.DMspawnpoint == 11 )     {
			deathmatchspawn[0] = 917;
			deathmatchspawn[1] = -990;
			deathmatchspawn[2] = 360;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 12 )     {
			deathmatchspawn[0] = 2463;
			deathmatchspawn[1] = -531;
			deathmatchspawn[2] = 120;
			deathmatchangles[1] = -135;
		} else if ( level.DMspawnpoint == 13 )     {
			deathmatchspawn[0] = 1817;
			deathmatchspawn[1] = -245;
			deathmatchspawn[2] = 120;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 14 )     {
			deathmatchspawn[0] = 1878;
			deathmatchspawn[1] = -477;
			deathmatchspawn[2] = 120;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 15 )     {
			deathmatchspawn[0] = 2514;
			deathmatchspawn[1] = -940;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 16 )     {
			deathmatchspawn[0] = 1966;
			deathmatchspawn[1] = -810;
			deathmatchspawn[2] = 120;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 17 )     {
			deathmatchspawn[0] = 1435;
			deathmatchspawn[1] = 91;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = 90;
		} else if ( level.DMspawnpoint == 18 )     {
			deathmatchspawn[0] = 1748;
			deathmatchspawn[1] = -811;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = -179;
		} else if ( level.DMspawnpoint == 19 )     {
			deathmatchspawn[0] = -167;
			deathmatchspawn[1] = 427;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 20 )     {
			deathmatchspawn[0] = 1448;
			deathmatchspawn[1] = -420;
			deathmatchspawn[2] = 296;
			deathmatchangles[1] = 0;
		} else if ( level.DMspawnpoint == 21 )     {
			deathmatchspawn[0] = 1021;
			deathmatchspawn[1] = -1190;
			deathmatchspawn[2] = 24;
			deathmatchangles[1] = 89;
			level.DMspawnpoint = -1;
		}
	}

	VectorCopy( deathmatchspawn, spawn_origin );
	VectorCopy( deathmatchangles, spawn_angles );
}


/*
======================
StatsCheck
======================
*/
//This only gets called if g_stats is on
//This is sort of an AMX MOD X feature :) (www.amxmodx.org)
//also from the old hs sniper days
// this is per round :) (might change if it just becomes one msg after another.
void StatsCheck( gentity_t *attacker ) {
	gentity_t *te;
	gentity_t *other;
	char index[MAX_OSPATH];
	int i;
	qboolean playsound = qfalse; //this is to play sound so no extra ents created :)
/*
    if (attacker->client->pers.kills == 5){
        Q_strncpyz(index, "sound/player/multikill.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1MULTI KILL! (5 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4MULTI KILL!\n\""));//		FIXME for all
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 10){
        Q_strncpyz(index, "sound/player/killingspree.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1KILLING SPREE! (10 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4KILLING SPREE!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 15){
        Q_strncpyz(index, "sound/player/megakill.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1MEGA KILL! (15 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4MEGA KILL!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 20){
        Q_strncpyz(index, "sound/player/rampage.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1RAMPAGE! (20 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4RAMPAGE!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 25){
        Q_strncpyz(index, "sound/player/impressive.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1IMPRESSIVE! (25 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4IMPRESSIVE!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 30){
        Q_strncpyz(index, "sound/player/ultrakill.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1ULTRA KILL! (30 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4ULTRA KILL!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 35){
        Q_strncpyz(index, "sound/player/unstoppable.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1UNSTOPPABLE! (35 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4UNSTOPPABLE!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 40){
        Q_strncpyz(index, "sound/player/dominating.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1DOMINATING! (40 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4DOMINATING!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 45){
        Q_strncpyz(index, "sound/player/excellent.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1EXCELLENT! (45 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4EXCELLENT!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 50){
        Q_strncpyz(index, "sound/player/whickedsick.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1WICKED SICK! (50 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4WICKED SICK!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 55){
        Q_strncpyz(index, "sound/player/unreal.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1UNREAL! (55 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4UNREAL!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 60){
        Q_strncpyz(index, "sound/player/ludicrouskill.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1LLLLUDICROUS KILL! (60 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4LUDICROUS KILL!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 65){
        Q_strncpyz(index, "sound/player/monsterkill.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1MMMMONSTER KILL! (65 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4MONSTER KILL!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 70){
        Q_strncpyz(index, "sound/player/godlike.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1GODLIKE! (70 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^4GODLIKE!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 75){
        Q_strncpyz(index, "sound/player/holyshit.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1HOLY SHIT! (75 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 80){
        Q_strncpyz(index, "sound/player/holyshit.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1HOLY SHIT! (80 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 85){
        Q_strncpyz(index, "sound/player/holyshit.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1HOLY SHIT! (85 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 90){
        Q_strncpyz(index, "sound/player/holyshit.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1HOLY SHIT! (90 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 95){
        Q_strncpyz(index, "sound/player/holyshit.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1HOLY SHIT! (95 Kills) ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
        playsound = qtrue;
    }
    if (attacker->client->pers.kills == 100){
        Q_strncpyz(index, "sound/player/admin6.wav", sizeof(index));
        trap_SendServerCommand(-1, va("chat \"^1100 KILLS THIS ROUND! ^7- %.99s^7\n\"", attacker->client->pers.netname));
        trap_SendServerCommand( attacker-g_entities, va("cp \"^1100 KILLS!\n\""));
        playsound = qtrue;
    }
    */

	if ( attacker->client->pers.kills == 5 ) {
		Q_strncpyz( index, "sound/player/multikill.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5MULTI KILL! (5k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4MULTI KILL!\n\""));//		FIXME for all
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 10 ) {
		Q_strncpyz( index, "sound/player/killingspree.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5KILLING SPREE! (10k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4KILLING SPREE!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 15 ) {
		Q_strncpyz( index, "sound/player/megakill.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5MEGA KILL! (15k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4MEGA KILL!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 20 ) {
		Q_strncpyz( index, "sound/player/rampage.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5RAMPAGE! (20k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4RAMPAGE!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 25 ) {
		Q_strncpyz( index, "sound/player/impressive.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5IMPRESSIVE! (25k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4IMPRESSIVE!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 30 ) {
		Q_strncpyz( index, "sound/player/ultrakill.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5ULTRA KILL! (30k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4ULTRA KILL!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 35 ) {
		Q_strncpyz( index, "sound/player/unstoppable.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5UNSTOPPABLE! (35k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4UNSTOPPABLE!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 40 ) {
		Q_strncpyz( index, "sound/player/dominating.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5DOMINATING! (40k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4DOMINATING!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 45 ) {
		Q_strncpyz( index, "sound/player/excellent.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5EXCELLENT! (45k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4EXCELLENT!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 50 ) {
		Q_strncpyz( index, "sound/player/whickedsick.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5WICKED SICK! (50k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		trap_SendServerCommand( attacker - g_entities, va( "cp \"^4WICKED SICK!\n\"" ) );
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 55 ) {
		Q_strncpyz( index, "sound/player/unreal.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5UNREAL! (55k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4UNREAL!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 60 ) {
		Q_strncpyz( index, "sound/player/ludicrouskill.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5LLLLUDICROUS KILL! (60k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4LUDICROUS KILL!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 65 ) {
		Q_strncpyz( index, "sound/player/monsterkill.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5MMMMONSTER KILL! (65k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4MONSTER KILL!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 70 ) {
		Q_strncpyz( index, "sound/player/godlike.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5GODLIKE! (70k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^4GODLIKE!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 75 ) {
		Q_strncpyz( index, "sound/player/holyshit.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5HOLY SHIT! (75k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 80 ) {
		Q_strncpyz( index, "sound/player/holyshit.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5HOLY SHIT! (80k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 85 ) {
		Q_strncpyz( index, "sound/player/holyshit.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5HOLY SHIT! (85k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 90 ) {
		Q_strncpyz( index, "sound/player/holyshit.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5HOLY SHIT! (90k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 95 ) {
		Q_strncpyz( index, "sound/player/holyshit.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5HOLY SHIT! (95k, %ihs)^7:%.99s^7\n\"", attacker->client->pers.headshots, attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^1HOLY SHIT!\n\""));
		playsound = qtrue;
	}
	if ( attacker->client->pers.kills == 100 ) {
		Q_strncpyz( index, "sound/player/admin6.wav", sizeof( index ) );
		trap_SendServerCommand( -1, va( "chat \"^5100 KILLS THIS ROUND! ^7- %.99s^7\n\"", attacker->client->pers.netname ) );
		//trap_SendServerCommand( attacker-g_entities, va("cp \"^1100 KILLS!\n\""));
		playsound = qtrue;
	}
	if ( playsound ) {
		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			//they dont want to hear it so shut it off.
			if ( !level.clients[i].pers.multikill ) {
				continue;
			}

			other = &g_entities[i];
			te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( index );
			te->s.teamNum = other->s.clientNum;
		}
	}
}
/*
=========
checkSpys
=========
*/
void checkSpys( gentity_t *ent ) {

	gentity_t *target;
	trace_t tr;
	vec3_t start, end, forward;

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->s.pos.trBase, start );   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA( start, 512, forward, end );    //put 'end' 512 units forward of 'start'

	//see if we hit anything between 'start' and 'end'
	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, ( CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_CORPSE | CONTENTS_TRIGGER ) );

	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	if ( tr.entityNum == ENTITYNUM_WORLD ) {
		return;
	}

	//if we didn't hit a player, return
	if ( tr.entityNum >= MAX_CLIENTS ) {
		return;
	}

	target = &g_entities[ tr.entityNum ];

	if ( ( !target->inuse ) || ( !target->client ) ) { //if the player is lagged/disconnected/etc
		return;
	}

	if ( target->client->ps.stats[STAT_HEALTH] <= 0 ) { //if they're dead
		return;
	}

	if ( !OnSameTeam( target,ent ) ) { //dont need to know otherteams info
		return;
	}

	if ( !target->client->ps.isSpy ) { //they arnt a spy so return
		return;
	}

	ent->client->lastLTinfotime = level.time;

	//July 27 2007 - agent.
	//make the spy messages print with level 1 priority so
	//people know that he is a spy.
	if ( target->client->sess.sessionTeam == TEAM_RED ) {
		trap_SendServerCommand( ent - g_entities, va( "cp \"^1AXIS ^3spy!\n\"1" ) );
	}

	if ( target->client->sess.sessionTeam == TEAM_BLUE ) {
		trap_SendServerCommand( ent - g_entities, va( "cp \"^4ALLIED ^3spy!\n\"1" ) );
	}

}

//WeaponUsesCharge
//returns 1 if it is a weapon that uses charge and ammo(panzer)
//returns 2 if it is totally dependant on charge
// everything else it returns 0 for
int weaponUsesCharge( weapon_t weapon ) {
	switch ( weapon ) {
	case WP_PANZERFAUST:
		return 1;
	case WP_ARTY:
	case WP_SMOKE_GRENADE:
	case WP_DYNAMITE:
	case WP_DYNAMITE2:
	case WP_AMMO:
	case WP_MEDKIT:
		return 2;
	default:
		return 0;
	}
}


char * weaponToString( weapon_t weapon ) {
	switch ( weapon ) {
	case WP_MP40:
		return "MP40";
	case WP_THOMPSON:
		return "Thompson";
	case WP_STEN:
		return "Sten";
	case WP_MAUSER:
		return "Mauser";
	case WP_SNIPERRIFLE:
		return "Sniper Rifle";
	case WP_SHOTGUN:
		return "Shotgun";
	case WP_FG42:
	case WP_FG42SCOPE:
		return "FG42";
	case WP_FLAMETHROWER:
		return "Flamethrower";
	case WP_PANZERFAUST:
		return "Panzerfaust";
	case WP_VENOM:
		return "Venom";
	case WP_GRENADE_LAUNCHER:
	case WP_GRENADE_PINEAPPLE:
		return "Grenade";
	case WP_LUGER:
		return "Luger";
	case WP_COLT:
		return "Colt";
	case WP_KNIFE:
	case WP_KNIFE2:
		return "Knife";
	case WP_ARTY:
		return "Artillery";
	case WP_SMOKE_GRENADE:
		return "Air strike";
	case WP_MEDIC_SYRINGE:
		return "Syringe";
	case WP_DYNAMITE:
	case WP_DYNAMITE2:
		return "Dynamite";
	case WP_AMMO:
		return "Ammo Pack";
	case WP_MEDKIT:
		return "Medkit";
	default:
		return "";
	}
}


/*
============
GetLTinfo
============
*/
void GetLTinfo( gentity_t *ent ) {
	int mainweapres = 0;
	int mainweapclip = 0;
	int pistweapres = 0;
	int pistweapclip = 0;
	int nadeammo = 0;
	int usesCharge;
	float chargeTime;
	int percentCharged;
	weapon_t weapon;

	gentity_t *target;
	trace_t tr;
	vec3_t start, end, forward;

	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}

	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}


	AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );

	VectorCopy( ent->s.pos.trBase, start );   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA( start, 512, forward, end );    //put 'end' 512 units forward of 'start'

	//see if we hit anything between 'start' and 'end'
	trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, ( CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_CORPSE | CONTENTS_TRIGGER ) );

	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		return;
	}

	if ( tr.entityNum == ENTITYNUM_WORLD ) {
		return;
	}

	//if we didn't hit a player, return
	if ( tr.entityNum >= MAX_CLIENTS ) {
		return;
	}

	target = &g_entities[ tr.entityNum ];


	if ( ( !target->inuse ) || ( !target->client ) ) { //if the player is lagged/disconnected/etc
		return;
	}

	if ( target->client->ps.stats[STAT_HEALTH] <= 0 ) { //if they're dead
		return;
	}

	if ( !OnSameTeam( target,ent ) ) { //dont need to know otherteams info
		return;
	}

	ent->client->lastLTinfotime = level.time;

	weapon = target->client->ps.weapon;

	usesCharge = weaponUsesCharge( weapon );


	if ( target->client->sess.sessionTeam == TEAM_BLUE ) {
		pistweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon( WP_COLT )];
		pistweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon( WP_COLT )];
		//add this to it also (for packing pistols
		pistweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon( WP_LUGER )];
	} else {
		pistweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon( WP_LUGER )];
		pistweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon( WP_LUGER )];
		//add this to it also (for packing pistols
		pistweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon( WP_COLT )];
	}

	//add up total of grenades
	nadeammo += target->client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_PINEAPPLE )];
	nadeammo += target->client->ps.ammoclip[BG_FindClipForWeapon( WP_GRENADE_LAUNCHER )];

	//get charge time
	if ( target->client->ps.stats[STAT_PLAYER_CLASS] == PC_MEDIC ) {
		chargeTime = g_medicChargeTime.value;
	} else if ( target->client->ps.stats[STAT_PLAYER_CLASS] == PC_LT ) {
		chargeTime = g_engineerChargeTime.value;
	} else if ( target->client->ps.stats[STAT_PLAYER_CLASS] == PC_ENGINEER ) {
		chargeTime = g_LTChargeTime.value;
	} else {
		chargeTime = g_soldierChargeTime.value;
	}

	percentCharged = ( ( level.time - target->client->ps.classWeaponTime ) / chargeTime ) * 100;

	if ( percentCharged > 100 ) {
		percentCharged = 100;
	}

	if ( percentCharged < 0 ) { //dont think this can happen
		percentCharged = 0;
	}

	mainweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon( weapon )];
	mainweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon( weapon )];

	if ( !usesCharge ) {
		if ( weapon == WP_FLAMETHROWER || weapon == WP_MEDIC_SYRINGE ) {
			trap_SendServerCommand( ent - g_entities, va( "cp \"^3%s:^5%i ^3Pistol:^5%i/%i ^3Nades:^5%i ^3Charge:^5%i\n\"",weaponToString( weapon ),mainweapclip,pistweapclip,pistweapres,nadeammo,percentCharged ) );
		} else {
			trap_SendServerCommand( ent - g_entities, va( "cp \"^3%s:^5%i/%i ^3Pistol:^5%i/%i ^3Nades:^5%i ^3Charge:^5%i\n\"",weaponToString( weapon ),mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo,percentCharged ) );
		}
	} else if ( usesCharge == 1 )       {
		trap_SendServerCommand( ent - g_entities, va( "cp \"^3%s:^5%i/%i ^3Pistol:^5%i/%i ^3Nades:^5%i ^3Charge:^5%i\n\"",weaponToString( weapon ),mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo,percentCharged ) );
	} else if ( usesCharge == 2 )       {
		trap_SendServerCommand( ent - g_entities, va( "cp \"^3%s:^5%i ^3Pistol:^5%i/%i ^3Nades:^5%i ^3Charge:^5%i\n\"",weaponToString( weapon ),percentCharged,pistweapclip,pistweapres,nadeammo,percentCharged ) );
	}

	//main weapons
	//S4NDM4NN - what in the hell was I drinking when I did this....
/*	mainweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon(25)];
    mainweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon(25)];
    mainweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon(4)];
    mainweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon(4)];
    mainweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon(3)];
    mainweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon(3)];
    mainweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon(7)];
    mainweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon(7)];
    mainweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon(8)];
    mainweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon(8)];
    mainweapres += target->client->ps.ammo[ BG_FindAmmoForWeapon(14)];
    mainweapclip += target->client->ps.ammoclip[BG_FindClipForWeapon(14)];
    mainweapclip += target->client->ps.ammoclip[BG_FindAmmoForWeapon(9)];
    //Pistols


    //Nades
    nadeammo += target->client->ps.ammoclip[BG_FindClipForWeapon(WP_GRENADE_PINEAPPLE)];
    nadeammo += target->client->ps.ammoclip[BG_FindClipForWeapon(WP_GRENADE_LAUNCHER)];


    if (COM_BitCheck( target->client->ps.weapons, WP_MP40)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Mp40:^5%i/%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Mp40:^5%i/%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
    }else if(COM_BitCheck( target->client->ps.weapons, WP_THOMPSON)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Thompson:^5%i/%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Thompson:^5%i/%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
    }else if(COM_BitCheck( target->client->ps.weapons, WP_STEN)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Sten:^5%i/%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Sten:^5%i/%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
    }else if(COM_BitCheck( target->client->ps.weapons, WP_PANZERFAUST)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Panzer:^5%i/%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Panzer:^5%i/%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
    }else if(COM_BitCheck( target->client->ps.weapons, WP_VENOM)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Venom:^5%i/%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Venom:^5%i/%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
    }else if(COM_BitCheck( target->client->ps.weapons, WP_FLAMETHROWER)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Flamethrower:^5%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Flamethrower:^5%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,pistweapclip,pistweapres,nadeammo));
    }else if(COM_BitCheck( target->client->ps.weapons, WP_MAUSER)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Mauser:^5%i/%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres, pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Mauser:^5%i/%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
    }else if(COM_BitCheck( target->client->ps.weapons, WP_SHOTGUN)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Shotgun:^5%i/%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres, pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Shotgun:^5%i/%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
    }else if(COM_BitCheck( target->client->ps.weapons, WP_FG42)){
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3FG42:^5%i/%i ^3Colt:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres, pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3FG42:^5%i/%i ^3Luger:^5%i/%i ^3Nades:^5%i \n\"",mainweapclip,mainweapres,pistweapclip,pistweapres,nadeammo));
    }else{
        if(target->client->sess.sessionTeam == TEAM_BLUE)
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Colt:^5%i/%i ^3Nades:^5%i \n\"",pistweapclip,pistweapres,nadeammo));
        else
        trap_SendServerCommand(ent-g_entities, va("cp \"^3Luger:^5%i/%i ^3Nades:^5%i \n\"",pistweapclip,pistweapres,nadeammo));
    }
    */
}

char *GetLevelTime() {
	int Objseconds,Objmins,Objtens;

	Objseconds = ( ( ( g_timelimit.value * 60 * 1000 ) - ( ( level.time - level.startTime ) ) ) / 1000 ); //begin martin - this line was a bitch :-)
	Objmins = Objseconds / 60;
	Objseconds -= Objmins * 60;
	Objtens = Objseconds / 10;
	Objseconds -= Objtens * 10;

	if ( Objseconds < 0 ) {
		Objseconds = 0;
	}
	if ( Objtens < 0 ) {
		Objtens = 0;
	}
	if ( Objmins < 0 ) {
		Objmins = 0;
	}

	return va( "%i:%i%i",Objmins,Objtens,Objseconds );  //end martin

}


/*
==================
ForestPlanesThink
==================
*/
//attempting to give the planes on forest a brain
//(maybe not a smart one but its still abrain function)
void ForestPlanesThink( gentity_t *ent ) {
	//Set the next time we are going to think
	ent->nextthink = level.time + ( 15000 + crandom() * 10000 );
	//set what we want to do next time we think
	ent->think = ForestPlanesThink;
	//yea lets spawn some bombs :D
	SpawnPlaneBombs( ent );
}

/*
==================
SpawnPlaneBombs
==================
*/
//this will do the act of the whole bomb thing from the planes
void SpawnPlaneBombs( gentity_t *ent ) {
	int i;
	gentity_t *bomb;
	vec3_t tmpdir;


	SnapVector( ent->s.pos.trBase );

	for ( i = 0; i < 5; i++ ) {
		bomb = G_Spawn();
		bomb->classname = "rocket";
		bomb->s.eType       = ET_MISSILE;
		bomb->r.svFlags = SVF_USE_CURRENT_ORIGIN | SVF_BROADCAST;
		bomb->r.ownerNum    = ent->s.number;
		bomb->parent        = ent;
		bomb->s.weapon = WP_PANZERFAUST;
		bomb->damage = G_GetWeaponDamage( WP_ROCKET_LAUNCHER ); // JPW NERVE
		bomb->splashDamage = G_GetWeaponDamage( WP_ROCKET_LAUNCHER ); // JPW NERVE
		bomb->splashRadius = G_GetWeaponDamage( WP_ROCKET_LAUNCHER );
		bomb->clipmask = MASK_MISSILESHOT;
		bomb->s.pos.trType = TR_GRAVITY;
		bomb->s.pos.trTime = level.time;        // move a bit on the very first frame
		FindPlaneDir( ent, tmpdir );
		tmpdir[0] += crandom() * 45;
		tmpdir[1] += crandom() * 45;
		tmpdir[2] = 0; //-10
		VectorNormalize( tmpdir );
		VectorScale( tmpdir,750,tmpdir );
		VectorCopy( tmpdir,bomb->s.pos.trDelta );
		SnapVector( bomb->s.pos.trDelta );          // save net bandwidth
		VectorCopy( ent->s.pos.trBase,bomb->s.pos.trBase );
		//VectorCopy(ent->r.currentAngles,bomb->r.currentAngles);
		VectorCopy( ent->s.pos.trBase,bomb->r.currentOrigin );

	}
	//G_LogPrintf("angles TEST: %s\n",vtos( ent-> ) );

}

void FindPlaneDir( gentity_t *ent, vec3_t dir ) {
/*	//n
    if(ent->r.currentAngles[1] > 65 &&
    //ne
    //e
    //se
    //s
    //sw
    //w
    //nw
*/
}


/*
=======================
DeathmatchCheckLocation
=======================
*/
void DeathmatchCheckLocation( gentity_t *ent ) {
	gentity_t *te;


	char location[1000];
	// skip if not connected
	if ( ent->client->pers.connected != CON_CONNECTED ) {
		return;
	}
	// skip if not on team
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}
	//lets skip if they are dead no need to kill dead people right
	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}
	if ( ent->client->ps.pm_flags & PMF_LIMBO ) {
		return;
	}


	Team_GetLocationMsg( ent, location, sizeof( location ) );


	if ( ( !Q_stricmp( ent->client->pers.location, "beach" ) && ent->client->ps.origin[0] < 1325 ) || ( !Q_stricmp( ent->client->pers.location, "outside axis base" ) ) ) {
		G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
		trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is off limits during ^5Free for All!\"",ent->client->pers.location ) );
		te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );   //Agent, in case they didnt see the message, wake them up
		te->s.teamNum = ent->s.clientNum;
	}

}
/*
======================
SniperCheckLocation
    -Basically this is a dump of the work that u did before ^^^^^^

+Supported Maps+
    -Assault
    -Base
    -Beach
    -Castle
    -Depot
    -Destruction
    -Sub
    -Village
    -Forest <-single player map :)
    *** S4ND, check if these are good ( i know beach is... [HS]SN!P!NG :)
======================
*/
void SniperCheckLocation( gentity_t *ent ) {
	gentity_t *te;
	char location[1000];
	char mapName[1000];
	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );
	//Dump
	// skip if not connected
	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}

	if ( ent->client->pers.connected != CON_CONNECTED ) {
		return;
	}
	// skip if not on team
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return;
	}
	//lets skip if they are dead no need to kill dead people right
	if ( ent->client->ps.stats[STAT_HEALTH] <= 0 ) {
		return;
	}
	if ( ent->client->ps.pm_flags & PMF_LIMBO ) {
		return;
	}


	Team_GetLocationMsg( ent, location, sizeof( location ) );

	//Alphabetacial order, FOLLOW IT!
	//ALLIES CAN'T GO HERE:
	if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
		if ( ( !Q_stricmp( mapName, "border" ) ) && ( ent->client->ps.origin[0] < -1886 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Axis Base is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "boss1" ) ) && ( ent->client->ps.origin[1] < -350 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "factory" ) ) && ( ent->client->ps.origin[0] > 1430 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "ffsniper" ) ) && ( ent->client->ps.origin[1] > 840 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Axis Side ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "fun_beach_2" ) ) && ( ent->client->ps.origin[2] > 1000 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "hs_bridge" ) ) && ( ent->client->ps.origin[1] < -1714 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Axis Side ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_assault" ) ) && ( ( !Q_stricmp( ent->client->pers.location, "axis hangar" ) )
		                                                  /*||(!Q_stricmp(ent->client->pers.location, "main warehouse")) || (!Q_stricmp(ent->client->pers.location, "back of warehouse")*/ ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_base" ) ) && ( ent->client->ps.origin[0] > 2515 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_beach" ) && ( !Q_stricmp( ent->client->pers.location, "trenches" ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_castle" ) ) && ( ( !Q_stricmp( ent->client->pers.location, "west axis spawn" ) )
														 || ( !Q_stricmp( ent->client->pers.location, "east axis spawn" ) ) || ( !Q_stricmp( ent->client->pers.location, "courtyard connection" ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_depot" ) ) && ( ( !Q_stricmp( ent->client->pers.location, "lower tunnels" ) )
														|| ( !Q_stricmp( ent->client->pers.location, "coal depot" ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_destruction" ) ) && ( !Q_stricmp( ent->client->pers.location, "axis reinforcement" ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		//		if (!Q_stricmp(mapName, "forest") &&(ent->client->ps.origin[1] < -3509)){
		if ( ( !Q_stricmp( mapName, "forest" ) ) && ( ( !Q_stricmp( ent->client->pers.location, "southwest forest" ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( !Q_stricmp( mapName, "mp_frost" ) && ( ent->client->ps.origin[0] > 3666 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_ice" ) ) && ( ( ent->client->ps.origin[0] > -4557 ) || ( ent->client->ps.origin[0] < -6942 ) || ( ent->client->ps.origin[1] < 800 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_tank" ) ) && ( ( ent->client->ps.origin[0] < -7127 ) || ( ent->client->ps.origin[2] < -380 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_sub" ) ) && ( !Q_stricmp( ent->client->pers.location, "barracks" ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "norway" ) ) && ( ent->client->ps.origin[0] < -4502 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "ogg_snipers" ) ) && ( ent->client->ps.origin[1] < -1583 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Axis Spawn ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "rwfsniper2" ) ) && ( ( ent->client->ps.origin[0] > 3340 ) ||
														  ( ent->client->ps.origin[0] < -3432 ) || ( ent->client->ps.origin[1] > 2800 ) || ( ent->client->ps.origin[1] < -3300 ) ||
														  ( ent->client->ps.origin[0] > 305 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "sfm" ) ) && ( ent->client->ps.origin[1] < -2530 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Axis Spawn ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "tt_trench" ) ) && ( ent->client->ps.origin[1] < 434 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "dam" ) ) && ( ent->client->ps.origin[1] > 3076 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "trainyard" ) ) && ( ent->client->ps.origin[0] > 2400 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		// we are already skipping specs, so this else wont apply to them
		//axis cant go here

	} else {

		if ( ( !Q_stricmp( mapName, "assault" ) ) && ( ( ent->client->ps.origin[1] > 3700 ) && ( ent->client->ps.origin[0] < -2980 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "border" ) ) && ( ent->client->ps.origin[0] > 2846 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3Allied Base is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "boss1" ) ) && ( ( ent->client->ps.origin[1] > 475 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "ffsniper" ) ) && ( ent->client->ps.origin[1] < -1706 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Allied Side ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "fun_beach_2" ) ) && ( ( ent->client->ps.origin[2] < 728 ) || ( ent->client->ps.origin[0] > 4000 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "hs_bridge" ) ) && ( ent->client->ps.origin[1] > 1850 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Allied Side ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		//allied hangar
		if ( ( !Q_stricmp( mapName, "mp_assault" ) ) && ( ( ent->client->ps.origin[0] > 3924 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_base" ) ) && ( !Q_stricmp( ent->client->pers.location, "allied deployment" ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_beach" ) ) && ( ( !Q_stricmp( ent->client->pers.location, "radio stairway" ) )
														|| ( !Q_stricmp( ent->client->pers.location, "back stairway" ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}

		if ( !Q_stricmp( mapName, "mp_beach" ) && ent->client->ps.origin[0] < 1100 ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Beach ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		//if ((!Q_stricmp(mapName, "mp_castle")) && ((!Q_stricmp(ent->client->pers.location, "sw ladder room"))	||(!Q_stricmp(ent->client->pers.location, "north stairwell"))))
		if ( ( !Q_stricmp( mapName, "mp_castle" ) ) && ( ( ent->client->ps.origin[2] < 0 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_depot" ) ) && ( ( !Q_stricmp( ent->client->pers.location, "lower tunnels" ) )
														|| ( !Q_stricmp( ent->client->pers.location, "garage" ) ) || ( !Q_stricmp( ent->client->pers.location, "cargo bay" ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_destruction" ) ) && ( ( !Q_stricmp( ent->client->pers.location, "allied reinforcement" ) )
															  || ( !Q_stricmp( ent->client->pers.location, "southern road" ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		//	if (!Q_stricmp(mapName, "forest") &&(ent->client->ps.origin[1] < -3509)){
		if ( ( !Q_stricmp( mapName, "forest" ) ) && ( ( !Q_stricmp( ent->client->pers.location, "southwest forest" ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( !Q_stricmp( mapName, "mp_frost" ) && ( ent->client->ps.origin[0] > 3666 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_ice" ) ) && ( ( ent->client->ps.origin[0] > -4557 ) || ( ( ent->client->ps.origin[0] > -6942 ) && ( ent->client->ps.origin[1] > 800 ) ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_sub" ) ) && ( !Q_stricmp( ent->client->pers.location, "generator room" ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_tank" ) ) && ( ( ent->client->ps.origin[0] < -7127 ) || ( ent->client->ps.origin[2] < -380 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "mp_village" ) ) && ( !Q_stricmp( ent->client->pers.location, "southeast city wall" ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "norway" ) ) && ( ent->client->ps.origin[0] < -4502 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5%s ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "ogg_snipers" ) ) && ( ent->client->ps.origin[1] > 600 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Allied Spawn ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "sfm" ) ) && ( ent->client->ps.origin[1] > 1416 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^5Allied Spawn ^3is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "rwfsniper2" ) ) && ( ( ent->client->ps.origin[0] > 3340 ) ||
														  ( ent->client->ps.origin[0] < -3432 ) || ( ent->client->ps.origin[1] > 2800 ) || ( ent->client->ps.origin[1] < -3300 ) ||
														  ( ent->client->ps.origin[0] < -786 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "tt_trench" ) ) && ( ent->client->ps.origin[1] > 434 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "dam" ) ) && ( ent->client->ps.origin[1] > 3076 ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
		if ( ( !Q_stricmp( mapName, "trainyard" ) ) && ( ( ent->client->ps.origin[0] > 2400 ) || ( ent->client->ps.origin[0] > 1350 ) ) ) {
			G_Damage( ent, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, 1000 );
			trap_SendServerCommand( ent - g_entities, va( "chat \"console: ^3This Area is out of bounds during ^5Sniper Mode^7.\"",ent->client->pers.location ) );
			te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			te->s.eventParm = G_SoundIndex( "sound/player/denied.wav" );
			te->s.teamNum = ent->s.clientNum;
		}
	}
}
/*
=======================
clearCleanIp
=======================
This is the next 2 functions, has to be split up to prevent it from crashing
*/
void clearCleanIp2( void ) {
	FILE        *bannedfile;
	FILE        *tempfile;
	char tempbannedips[1000];


	bannedfile = fopen( "cleanclients.txt","w" );
	if ( bannedfile ) {
		tempfile = fopen( "cleanclientstemp.txt","r" );

		while ( fgets( tempbannedips,20,tempfile ) != NULL )
		{
			fputs( tempbannedips,bannedfile );

		}
		fclose( bannedfile );
		fclose( tempfile );

		remove( "cleanclientstemp.txt" );
	}
}

void clearCleanIp( gentity_t *ent ) {

	FILE        *bannedfile;
	FILE        *tempfile;
	char bannedips[18];

	bannedfile = fopen( "cleanclients.txt","r" );
	if ( bannedfile ) {
		tempfile = fopen( "cleanclientstemp.txt","w" );
		while ( fgets( bannedips,20,bannedfile ) != NULL )
		{
			if ( Q_stricmp( bannedips, va( "%s\n",ent->client->sess.ip ) ) ) {
				fputs( bannedips,tempfile );
			}
		}
		fclose( bannedfile );
		fclose( tempfile );

		clearCleanIp2();
	}
}



/*
============
WinFree4All
============
*/
void WinFree4All() {
	gentity_t   *other;
	gentity_t   *te;
	int i;


	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		other = &g_entities[i];

		if ( other == level.highkiller ) {
			continue;
		}
		other->client->ps.weapons[0] = 0;
		other->client->ps.weapons[1] = 0;
		other->client->ps.weapon = 0;
		other->client->ps.endDeathmatch++;
		trap_SendServerCommand( other - g_entities, va( "chat \"console: %s ^3has hit the fraglimit!\n\"", level.highkiller->client->pers.netname ) );
	}
	trap_SendServerCommand( -1, va( "cp \"%s ^7has won the match!\n\"3", level.highkiller->client->pers.netname ) );

//tell him he won
	trap_SendServerCommand( level.highkiller - g_entities, va( "chat \"console: ^3You have hit the fraglimit!\n\"", level.highkiller->client->pers.netname ) );
//give him some god he deserves it
	level.highkiller->flags |= FL_GODMODE;
//no fatige is always nice
	level.highkiller->flags |= FL_NOFATIGUE;
//Well i couldnt make the round end so i made up a better idea :)
//clear wepons he has
	level.highkiller->client->ps.weapons[0] = 0;
	level.highkiller->client->ps.weapons[1] = 0;
//clear weapon he has out
	level.highkiller->client->ps.weapon = 0;
//make panzer = ownage
	ammoTable[WP_PANZERFAUST].maxammo = 1000000;
	ammoTable[WP_PANZERFAUST].maxclip = 1000000;
	ammoTable[WP_PANZERFAUST].fireDelayTime = 0;
	ammoTable[WP_PANZERFAUST].nextShotTime = 100;
//clear ammo
	memset( level.highkiller->client->ps.ammo,MAX_WEAPONS,sizeof( int ) );
//play a sound
	te = G_TempEntity( level.highkiller->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
	te->s.eventParm = G_SoundIndex( "sound/player/finishhim.wav" );
	te->s.teamNum = level.highkiller->s.clientNum;
//give him a panzer
	COM_BitSet( level.highkiller->client->ps.weapons, WP_PANZERFAUST );
//give him ammo and lots of it
	level.highkiller->client->ps.ammoclip[BG_FindClipForWeapon( WP_PANZERFAUST )] = 1000000;
	level.highkiller->client->ps.ammo[BG_FindAmmoForWeapon( WP_PANZERFAUST )] = 1000000;
	level.highkiller->client->ps.powerups[PW_REDFLAG] = INT_MAX;
	level.highkiller->client->ps.powerups[PW_QUAD] = INT_MAX;
	level.highkiller->client->ps.powerups[PW_FLIGHT] = INT_MAX;
	level.endDeathmatch++; //just make this not 0 so g_main can reconize it :)
	level.highkiller->client->ps.endDeathmatch++;
	level.nextDeathmatchendTime = level.time + 3000;
//I think this will make him switch to the panzer
	level.highkiller->client->ps.weapon = WP_PANZERFAUST;


}


/*
================
CheckDeathmatch
================
*/
void CheckDeathmatch( gentity_t *attacker, gentity_t *self ) {
	gentity_t *te;

	//return if attacker is not a client
	if ( !( attacker && attacker->client ) ) {
		return;
	}

	//he killed himself so return
	if ( attacker == self ) {
		return;
	}
	//return if they dont have enough kills
	if ( attacker->client->pers.kills < level.highkill ) {
		return;
	}

	if ( attacker->client->pers.kills == level.highkill ) {
		trap_SendServerCommand( -1, va( "chat \"console: %s ^3has tied with the lead!\n",attacker->client->pers.netname ) );
		te = G_TempEntity( attacker->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/player/TiedLead.wav" );
		te->s.teamNum = attacker->s.clientNum;
	}

	if ( ( attacker->client->pers.kills > level.highkill ) && ( attacker != level.highkiller ) ) {
		trap_SendServerCommand( -1, va( "chat \"console: %s ^3has taken the lead!\n", attacker->client->pers.netname ) );

		te = G_TempEntity( attacker->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		te->s.eventParm = G_SoundIndex( "sound/player/TakenLead.wav" );
		te->s.teamNum = attacker->s.clientNum;
		level.highkiller = attacker;
	}
	if ( level.highkiller == attacker ) {
		level.highkill = level.highkiller->client->pers.kills;
	}

	//lets check for our fraglimit
	if ( level.highkill >= g_fraglimit.integer ) {
		//hmmmm... if they hit it i think ill just make another function for this above^^^
		WinFree4All();
	}
}


/*
=============
checkSpeclock
=============
*/
void checkSpeclock( gentity_t *ent ) {
	gclient_t           *client;
	int clientNum;
	int specClient;
	client = ent->client;
	clientNum = client - level.clients;

	if ( ent->client->pers.connected != CON_CONNECTED ) { //hes not connected return
		return;
	}

	if ( ent->client->sess.sessionTeam == TEAM_RED || ent->client->sess.sessionTeam == TEAM_BLUE ) {
		return; //hes allowed to spec his team when hes dead

	}
	//if they are speclocked already lets see if they want unlocked
	if ( ent->client->sess.isspeclocked ) {

		if ( ent->client->sess.admin ) { //hes an admin unlock him
			spec_unlock( ent );
			return;
		}

		if ( ent->client->sess.speclock == 3 ) { //he can watch whoever he wants let him go
			spec_unlock( ent );
			return;
		}

		if ( g_speclock.integer == 0 ) {
			spec_unlock( ent );
			return;
		}

	} else
	{ //they arnt speclocked should they be

		if ( ent->client->sess.admin ) { //an admin just return
			return;
		}

		if ( ent->client->sess.speclock == 3 ) { //hes been allowed to spec both teams so return
			return;
		}



		//Making myself a guid here because for some reason i keep mixing this up

		//for sess.speclock
		//0 = cant spec
		//1 = can spec axis
		//2 = can spec allies
		//3 = can spec whoever they want

		//for g_speclock
		//0 = off
		//1 = cant spec axis
		//2 = cant spec allies
		//3 = cant spec

		if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {
			specClient = ent->client->sess.spectatorClient;

			if ( g_speclock.integer == 1 ) { //axis team is locked
				if ( level.clients[specClient].sess.sessionTeam == TEAM_RED ) {
					if ( ent->client->sess.speclock == 0 || ent->client->sess.speclock == 2 ) {
						spec_lock( ent );
						return;
					}
				}
			} else if ( g_speclock.integer == 2 )     {
				if ( level.clients[specClient].sess.sessionTeam == TEAM_BLUE ) {
					if ( ent->client->sess.speclock == 0 || ent->client->sess.speclock == 1 ) {
						spec_lock( ent );
						return;
					}
				}
			} else if ( g_speclock.integer == 3 )     {
				if ( ent->client->sess.speclock == 0 ) {
					spec_lock( ent );
					return;
				} else if ( ent->client->sess.speclock == 1 )     {
					if ( level.clients[specClient].sess.sessionTeam == TEAM_BLUE ) {
						spec_lock( ent );
						return;
					}
				} else if ( ent->client->sess.speclock == 2 )     {
					if ( level.clients[specClient].sess.sessionTeam == TEAM_RED ) {
						spec_lock( ent );
						return;
					}
				}
			}
		}
		if ( ent->client->sess.spectatorState == SPECTATOR_FREE ) {
			spec_lock( ent );
			return;
		}


	}

}

/*
=============
spec_lock
=============
*/
void spec_lock( gentity_t *ent ) {
	gclient_t           *client;
	int clientNum;
	vec3_t origin;
	vec3_t angles;
	client = ent->client;
	clientNum = client - level.clients;


	if ( !g_speclock.integer ) {
		return;
	}

	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		return;
	}


	origin[0] = -9999;
	origin[1] = -9999;
	origin[2] = -9999;
	angles[0] = 87;
	angles[1] = 180;
	angles[2] = 0;
	ent->client->sess.spectatorState = SPECTATOR_FREE;
	TeleportPlayer( ent, origin, angles );
	ent->client->sess.spectatorClient = clientNum;
	ent->client->sess.isspeclocked = 1;
}

/*
=============
spec_unlock
=============
*/
void spec_unlock( gentity_t *ent ) {
	gclient_t           *client;
	int clientNum;
	client = ent->client;
	clientNum = client - level.clients;


	if ( ent->client->sess.sessionTeam != TEAM_SPECTATOR ) {
		return;
	}

	ent->client->sess.isspeclocked = 0;
//	ent->client->sess.spectatorState = SPECTATOR_FREE;
	ent->client->ps.pm_flags &= ~PMF_TIME_LOCKPLAYER;
//	SetTeam( ent, "spectator",qfalse );
	ClientBegin( clientNum );
//	trap_SendServerCommand( -1, "chat \" spec_unlock was called - TAKE THIS OUT (agent)\n\"");
}

/*
================
print_mod
================
*/
//makes the messages of death server side
void print_mod( gentity_t *attacker, gentity_t *self, int meansOfDeath ) {
	gentity_t   *te;
	gentity_t *other;
	char        *message;
	char        *message2;
	char index[MAX_OSPATH];
	int i;

	message = 0;
	message2 = 0;
	//just do this to fix the crap that prints at the end of mod sometimes :)
	//Q_strncpyz ( message, "", sizeof( message ) );
	//Q_strncpyz ( message2, "", sizeof( message2 ) );

	if ( g_deathmatch.integer < 2 ) {
		if ( !( attacker == self ) ) {
			if ( OnSameTeam( attacker, self ) ) {
				trap_SendServerCommand( attacker - g_entities, va( "cp \"You killed ^1TEAMMATE ^7%s\n\"", self->client->pers.netname ) );
			} else {
				trap_SendServerCommand( attacker - g_entities, va( "cp \"You killed %s", self->client->pers.netname ) );
			}
		}
	}
	// For deathmatch 2, everyone is an enemy
	if ( g_deathmatch.integer == 2 ) {
		if ( !( attacker == self ) ) {
			trap_SendServerCommand( attacker - g_entities, va( "cp \"You killed %s", self->client->pers.netname ) );
		}
	}

	if ( !attacker->client ) {
		if ( meansOfDeath == MOD_FALLING ) {
			if ( self->client->sess.gender == 0 ) {
				message = "fell to his death";
			} else {
				message = "fell to her death";
			}
		} else if ( meansOfDeath == MOD_ADMIN_SLAP )     { //for slap			  WAS 1006
			message = "was slapped to death by the admin";
		} else if ( meansOfDeath == 1000 )     { //for deathmatch out of bounds
			message = "went out of bounds";
		} else if ( meansOfDeath == MOD_ADMIN_KILL )     { //for  admin kill		was 1007
			message = "was killed by the admin";
		} else if ( meansOfDeath == 1010 )     { //for  CV kill
			message = "was killed ^3for centerviewing";
		} else if ( meansOfDeath == MOD_CRUSH )     {
			message = "was crushed";
		} else if ( meansOfDeath == MOD_WATER )     {
			message = "drowned";
		} else if ( meansOfDeath == MOD_SLIME )     {
			message = "died by toxic materials";
		} else if ( meansOfDeath == MOD_TRIGGER_HURT )     {
			message = "was killed";
		} else if ( meansOfDeath == MOD_MORTAR || meansOfDeath == MOD_MORTAR_SPLASH )     {
			message = "was shelled";
		} else if ( meansOfDeath == MOD_FLAMETHROWER )     {
			message = "was cooked";
		} else if ( meansOfDeath == MOD_LOPER_LEAP )    {
			message = "dove on a landmine";
		} else {
			message = "died";
		}
		if ( message ) {
			trap_SendServerCommand( -1, va( "print \"^7%s ^7%s\n", self->client->pers.netname, message ) );
			return;
		}
	}
	if ( attacker == self ) {
		if ( meansOfDeath == MOD_DYNAMITE_SPLASH || meansOfDeath == MOD_DYNAMITE ) {
			if ( self->client->sess.gender == 0 ) {
				message = "dynamited himself to pieces";
			} else {
				message = "dynamited herself to pieces";
			}
		} else if ( meansOfDeath == MOD_SUICIDE_KILL )     { //for /kill
			if ( self->client->sess.gender == 0 ) {
				message = "slit his own throat";
			} else {
				message = "slit her own throat";
			}
		} else if ( meansOfDeath == MOD_SUICIDE_GIB )     { //for /kill
			if ( self->client->sess.gender == 0 ) {
				message = "killed himself";
			} else {
				message = "killed herself";
			}
		} else if ( meansOfDeath == MOD_GRENADE_PINEAPPLE || meansOfDeath == MOD_GRENADE_SPLASH || meansOfDeath == MOD_GRENADE_LAUNCHER )      {
			if ( self->client->sess.gender == 0 ) {
				message = "dove on his own grenade";
			} else {
				message = "dove on her own grenade";
			}
		} else if ( meansOfDeath == MOD_ROCKET_SPLASH )     {
			if ( self->client->sess.gender == 0 ) {
				message = "vaporized himself";
			} else {
				message = "vaporized herself";
			}
		} else if ( meansOfDeath == MOD_AIRSTRIKE )     {
			if ( self->client->sess.gender == 0 ) {
				message = "obliterated himself";
			} else {
				message = "obliterated herself";
			}
		} else if ( meansOfDeath == MOD_ARTILLERY )     {
			if ( self->client->sess.gender == 0 ) {
				message = "fired for effect on himself";
			} else {
				message = "fired for effect on herself";
			}
		} else if ( meansOfDeath == MOD_EXPLOSIVE )     {
			if ( self->client->sess.gender == 0 ) {
				message = "died in his own explosion";
			} else {
				message = "died in her own explosion";
			}
		}
//yes it is possiable to kill yourself on this
		else if ( meansOfDeath == MOD_MACHINEGUN ) {
			if ( self->client->sess.gender == 0 ) {
				message = "perforated himself";
			} else {
				message = "perforated herself";
			}
		}
//noob who ran into fire
		else if ( meansOfDeath == MOD_FLAMETHROWER ) {
			if ( self->client->sess.gender == 0 ) {
				message = "cooked himself";
			} else {
				message = "cooked herself";
			}
		} else if ( meansOfDeath == MOD_THROWING_KNIFE )     {
			if ( self->client->sess.gender == 0 ) {
				message = "failed to juggle his own knives";
			} else {
				message = "failed to juggle her own knives";
			}
		} else if ( meansOfDeath == MOD_LANDMINE )     {
			if ( self->client->sess.gender == 0 ) {
				message = "was to close to his own mine";
			} else {
				message = "was to close to her own mine";
			}

		} else {
			if ( self->client->sess.gender == 0 ) {
				message = "killed himself";
			} else {
				message = "killed herself";
			}
		}


		if ( message ) {
			trap_SendServerCommand( -1, va( "print \"^7%s ^7%s\n", self->client->pers.netname, message ) );
			return;
		}
	}


	if ( attacker && attacker->client ) {
		if ( meansOfDeath == MOD_KNIFE2 || meansOfDeath == MOD_KNIFE || meansOfDeath == MOD_KNIFE_STEALTH ) {
			message = "was stabbed by";
			message2 = "'s knife";

			if ( !OnSameTeam( attacker,self ) ) {
				Q_strncpyz( index, "sound/player/humiliation.wav", sizeof( index ) );
				for ( i = 0; i < MAX_CLIENTS; i++ ) {
					if ( level.clients[i].pers.connected != CON_CONNECTED ) {
						continue;
					}
					other = &g_entities[i];
					te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
					te->s.eventParm = G_SoundIndex( index );
					te->s.teamNum = other->s.clientNum;
				}
				G_AddEvent( attacker, EV_GENERAL_SOUND, G_SoundIndex( "sound/player/goat.wav" ) );
			}
		} else if ( meansOfDeath == MOD_LUGER )     {
			message = "was killed by";
			message2 = "'s Luger 9mm";
		} else if ( meansOfDeath == MOD_COLT )     {
			message = "was killed by";
			message2 = "'s .45ACP 1911";
		} else if ( meansOfDeath == MOD_MP40 )     {
			message = "was killed by";
			message2 = "'s MP40";
		} else if ( meansOfDeath == MOD_THOMPSON )     {
			message = "was killed by";
			message2 = "'s Thompson";
		} else if ( meansOfDeath == MOD_STEN )     {
			message = "was killed by";
			message2 = "'s Sten";
		} else if ( meansOfDeath == MOD_MAUSER )     {
			message = "was killed by";
			message2 = "'s Mauser";
			if ( g_deathmatch.integer == 2 ) {
				message2 = "'s Shotgun";
			}
		} else if ( meansOfDeath == MOD_SNIPERRIFLE )     {
			if ( ( g_sniperWar.integer ) && ( self->client->ps.eFlags & EF_HEADSHOT ) ) {
				message = "was decapitated by";
				message2 = "'s sniper rifle";
				/*	if(!OnSameTeam(attacker,self)){
				        Q_strncpyz(index, "sound/player/headshot.wav", sizeof(index));
				        for(i=0; i<MAX_CLIENTS; i++){
				            if (level.clients[i].pers.connected != CON_CONNECTED)
				            continue;
				            other = &g_entities[i];
				            te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
				            te->s.eventParm = G_SoundIndex (index);
				            te->s.teamNum = other->s.clientNum;
				        }
				*/
			} else {
				message = "was killed by";
				message2 = "'s sniper rifle";
			}
		}
		/*else if (meansOfDeath == MOD_SNIPERRIFLE){
		    message = "was killed by";
		    message2 = "'s sniper rifle";
		}*/
		else if ( meansOfDeath == MOD_DYNAMITE_SPLASH || meansOfDeath == MOD_DYNAMITE ) {
			message = "was blasted by";
			message2 = "'s dynamite";
		} else if ( meansOfDeath == MOD_FG42 || meansOfDeath == MOD_FG42SCOPE )     {
			message = "was killed by";
			message2 = "'s FG42";
		} else if ( meansOfDeath == MOD_ROCKET_SPLASH || meansOfDeath == MOD_ROCKET_LAUNCHER || meansOfDeath == MOD_ROCKET )     {
			message = "was blasted by";
			message2 = "'s Panzerfaust";
		} else if ( meansOfDeath == MOD_GRENADE_PINEAPPLE || meansOfDeath == MOD_GRENADE_SPLASH || meansOfDeath == MOD_GRENADE || meansOfDeath == MOD_GRENADE_LAUNCHER )        {
			message = "was exploded by";
			message2 = "'s grenade";
		} else if ( meansOfDeath == MOD_VENOM )     {
			message = "was ventilated by";
			message2 = "'s Venom";
		} else if ( meansOfDeath == MOD_FLAMETHROWER )     {
			message = "was cooked by";
			message2 = "'s flamethrower";
		} else if ( meansOfDeath == MOD_MACHINEGUN )     {
			message = "was perforated by";
			message2 = "'s crew-served MG42";
		} else if ( meansOfDeath == MOD_AIRSTRIKE )     {
			message = "was blasted by";
			message2 = "'s support fire"; // JPW NERVE changed since it gets called for both air strikes and artillery
		} else if ( meansOfDeath == MOD_ARTILLERY )     {
			message = "was shelled by";
			message2 = "'s artillery support"; // JPW NERVE changed since it gets called for both air strikes and artillery
		} else if ( meansOfDeath == MOD_POISON_SYRINGE )     {  //WAS means of death == 1001
			message = "was poisoned by";
			attacker->client->pers.poisons++;
			if ( g_gamestate.integer == GS_PLAYING ) {
				if ( attacker->client->pers.poisons > level.Hapoisonnum ) {
					Q_strncpyz( level.Hahighpoisoners, "", sizeof( level.HAhighkillers ) );
					strcat( level.Hahighpoisoners, attacker->client->pers.netname );
					level.Hapoisonnum = attacker->client->pers.poisons;
				} else if ( attacker->client->pers.poisons == level.Hapoisonnum )     {
					strcat( level.Hahighpoisoners, "^7, " ); //Add a comma after the other person
					strcat( level.Hahighpoisoners, attacker->client->pers.netname ); //and add their name
				}
			}

		} else if ( meansOfDeath == MOD_AIRSTRIKE_CANISTER )     {
			message = "stood on";
			message2 = "'s smoke canister";
		} else if ( meansOfDeath == MOD_GOOMBA )     {                  //WAS meansOfDeath == 1002
			message = "experienced death from above by";
			attacker->client->pers.goomba++;
			if ( g_gamestate.integer == GS_PLAYING ) {
				if ( attacker->client->pers.goomba > level.HAgoombanum ) {
					Q_strncpyz( level.HAhighgoombaers, "", sizeof( level.HAhighgoombaers ) );
					strcat( level.HAhighgoombaers, attacker->client->pers.netname );
					level.HAgoombanum = attacker->client->pers.goomba;
				} else if ( attacker->client->pers.goomba == level.HAgoombanum )     {
					strcat( level.HAhighgoombaers, "^7, " ); //Add a comma after the other person
					strcat( level.HAhighgoombaers, attacker->client->pers.netname ); //and add their name
				}
			}
		} else if ( meansOfDeath == MOD_THROWING_KNIFE )     {  // WAS meansOfDeath == 1003
			message = "was impaled by";
			message2 = "'s throwing knife";
		} else if ( meansOfDeath == MOD_LANDMINE )     {        // WAS meansOfDeath == 1008
			message = "tripped on";
			message2 = "'s mine";
		} else if ( meansOfDeath == MOD_FALLING )     {
			message = "was pushed by";
			message2 = "";
		} else if ( meansOfDeath == MOD_TELEFRAG )     {
			message = "was telefragged by";

		} else {
			message = "was killed by";
		}

// JPW NERVE if attacker != target but on same team
		if ( g_deathmatch.integer < 2 ) {
			if ( ( OnSameTeam( attacker, self ) ) && ( !( attacker == self ) ) ) {
				if ( meansOfDeath == MOD_FALLING ) {
					message = "^1WAS PUSHED BY TEAMMATE^7";
					message2 = "";
				} else {
					message = "^1WAS KILLED BY TEAMMATE^7";
					message2 = "";
				}
			}
		}
		if ( !message2 ) {
			trap_SendServerCommand( -1, va( "print \"^7%s ^7%s ^7%s\n", self->client->pers.netname, message, attacker->client->pers.netname ) );
			return;
		} else {
			trap_SendServerCommand( -1, va( "print \"^7%s ^7%s ^7%s^7%s\n", self->client->pers.netname, message, attacker->client->pers.netname, message2 ) );
			return;
		}
	}
}

/*
==========
HitSounds
==========
*/
void HitSounds( gentity_t *ent,gentity_t *targ,qboolean isHeadshot ) {
	gentity_t *te;

	if ( !ent->client->pers.hitsounds  && !S4ND_IsDemo() ) {
		return;
	}

	te = G_TempEntity( ent->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );

	if (S4ND_IsDemo() && g_deathmatch.integer != 2 )
	{
		if ( OnSameTeam( ent,targ ) ) 
		{
				te->s.eventParm = G_SoundIndex( "/sound/multiplayer/hurt_barbwire.wav" );
		} 
		else if ( !OnSameTeam( ent,targ ) )       
		{
			if ( isHeadshot ) 
			{
				te->s.eventParm = G_SoundIndex( "/sound/weapons/bullethit_metal3.wav" );
			} 
			else 
			{
				te->s.eventParm = G_SoundIndex( "/sound/working/bullethitflesh1.wav" );
			}
		} 
		else 
		{
			return;
		}
	}
	else
	{
	//	Regular hitsounds for Freeforall
		if ( g_deathmatch.integer == 2 ) {
			if ( isHeadshot ) {
				te->s.eventParm = G_SoundIndex( "sound/player/hithead.wav" );
			} else {
				te->s.eventParm = G_SoundIndex( "sound/player/hit.wav" );
			}
	//	Non FreeForAll
		} else {
			if ( OnSameTeam( ent,targ ) ) {
				te->s.eventParm = G_SoundIndex( "sound/player/hitteam.wav" );
			} else if ( !OnSameTeam( ent,targ ) )       {
				if ( isHeadshot ) {
					te->s.eventParm = G_SoundIndex( "sound/player/hithead.wav" );
				} else {
					te->s.eventParm = G_SoundIndex( "sound/player/hit.wav" );
				}
			} else {
				return;
			}
		}
	}
	te->s.teamNum = ent->s.clientNum;

}

#define WCP_ANIM_NOFLAG             0
#define WCP_ANIM_RAISE_AXIS         1
#define WCP_ANIM_RAISE_AMERICAN     2
#define WCP_ANIM_AXIS_RAISED        3
#define WCP_ANIM_AMERICAN_RAISED    4
#define WCP_ANIM_AXIS_TO_AMERICAN   5
#define WCP_ANIM_AMERICAN_TO_AXIS   6
#define WCP_ANIM_AXIS_FALLING       7
#define WCP_ANIM_AMERICAN_FALLING   8

void CTF_think( gentity_t * self ) {
	switch ( self->s.frame ) {

	case WCP_ANIM_NOFLAG:
		break;
	case WCP_ANIM_RAISE_AXIS:
		self->s.frame = WCP_ANIM_AXIS_RAISED;
		break;
	case WCP_ANIM_RAISE_AMERICAN:
		self->s.frame = WCP_ANIM_AMERICAN_RAISED;
		break;
	case WCP_ANIM_AXIS_RAISED:
		break;
	case WCP_ANIM_AMERICAN_RAISED:
		break;
	case WCP_ANIM_AXIS_TO_AMERICAN:
		self->s.frame = WCP_ANIM_AMERICAN_RAISED;
		break;
	case WCP_ANIM_AMERICAN_TO_AXIS:
		self->s.frame = WCP_ANIM_AXIS_RAISED;
		break;
	case WCP_ANIM_AXIS_FALLING:
		self->s.frame = WCP_ANIM_NOFLAG;
		break;
	case WCP_ANIM_AMERICAN_FALLING:
		self->s.frame = WCP_ANIM_NOFLAG;
		break;
	default:
		break;

	}

	self->touch = CTF_touch;
	self->nextthink = 0;

}


void CTF_touch( gentity_t *self, gentity_t *other, trace_t *trace ) {

	if ( self->count2 == other->client->sess.sessionTeam ) {
		return;
	}


	// Set controlling team
	self->count = other->client->sess.sessionTeam;

	// Set animation
	if ( self->count == TEAM_RED ) {
		self->dl_color[0] = 1;
		self->dl_color[1] = 0;
		self->dl_color[2] = 0;
		SP_dlight( self );
		if ( self->s.frame == WCP_ANIM_NOFLAG ) {
			self->s.frame = WCP_ANIM_RAISE_AXIS;
		} else if ( self->s.frame == WCP_ANIM_AMERICAN_RAISED ) {
			self->s.frame = WCP_ANIM_AMERICAN_TO_AXIS;
		} else {
			self->s.frame = WCP_ANIM_AXIS_RAISED;
		}

	} else {
		self->dl_color[0] = 0;
		self->dl_color[1] = 0;
		self->dl_color[2] = 1;
		SP_dlight( self );
		if ( self->s.frame == WCP_ANIM_NOFLAG ) {
			self->s.frame = WCP_ANIM_RAISE_AMERICAN;
		} else if ( self->s.frame == WCP_ANIM_AXIS_RAISED ) {
			self->s.frame = WCP_ANIM_AXIS_TO_AMERICAN;
		} else {
			self->s.frame = WCP_ANIM_AMERICAN_RAISED;
		}
	}

	// Play a sound
	G_AddEvent( self, EV_GENERAL_SOUND, self->soundPos1 );

	// Don't allow touch again until animation is finished
	self->touch = NULL;

	self->think = CTF_think;
	self->nextthink = level.time + 1000;
}

void SP_CTF_flag( gentity_t *ent ) {
	char *capture_sound;

	// Make sure the ET_TRAP entity type stays valid
	ent->s.eType        = ET_TRAP;


	// Model is user assignable, but it will always try and use the animations for flagpole.md3
	if ( ent->model ) {
		ent->s.modelindex   = G_ModelIndex( ent->model );
	} else {
		ent->s.modelindex   = G_ModelIndex( "models/multiplayer/flagpole/flagpole.md3" );
	}

	G_SpawnString( "noise", "sound/movers/doors/door6_open.wav", &capture_sound );
	ent->soundPos1  = G_SoundIndex( capture_sound );

	ent->clipmask   = CONTENTS_SOLID;
	ent->r.contents = CONTENTS_SOLID;

	VectorSet( ent->r.mins, -8, -8, 0 );
	VectorSet( ent->r.maxs, 8, 8, 128 );

	G_SetOrigin( ent, ent->s.origin );
	G_SetAngle( ent, ent->s.angles );

	// s.frame is the animation number
	if ( ent->count == 1 ) { //axis
		ent->s.frame    = WCP_ANIM_AXIS_RAISED;
		//trap_SendServerCommand(-1, va("chat \"console:Axis capture fb\n\""));
		ent->dl_color[0] = 1;
		ent->dl_color[1] = 0;
		ent->dl_color[2] = 0;
		SP_dlight( ent );
	} else {              //allies
		ent->s.frame    = WCP_ANIM_AMERICAN_RAISED;
		ent->dl_color[0] = 0;
		ent->dl_color[1] = 0;
		ent->dl_color[2] = 1;
		SP_dlight( ent );
	}

	// s.teamNum is which set of animations to use ( only 1 right now )
	ent->s.teamNum  = 1;

	//when its touched call CTF_touch, located in S4ND_func
	ent->touch      = CTF_touch;


	trap_LinkEntity( ent );
}
/*
=============================
SPAWN FLAG
=============================
*/
// Shoves the shit into the spawn flag
void SP_cust_spawnflag( gentity_t *ent ) {
	char *capture_sound;

	// Make sure the ET_TRAP entity type stays valid
	ent->s.eType        = ET_TRAP;


	// Model is user assignable, but it will always try and use the animations for flagpole.md3
	if ( ent->model ) {
		ent->s.modelindex   = G_ModelIndex( ent->model );
	} else {
		ent->s.modelindex   = G_ModelIndex( "models/multiplayer/flagpole/flagpole_reinforce.md3" );
	}

	G_SpawnString( "noise", "sound/movers/doors/door6_open.wav", &capture_sound );
	ent->soundPos1  = G_SoundIndex( capture_sound );

	ent->clipmask   = CONTENTS_SOLID;
	ent->r.contents = CONTENTS_SOLID;

	VectorSet( ent->r.mins, -8, -8, 0 );
	VectorSet( ent->r.maxs, 8, 8, 128 );

	G_SetOrigin( ent, ent->s.origin );
	G_SetAngle( ent, ent->s.angles );

	// s.frame is the animation number
	if ( ent->count == 1 ) { //axis
		ent->s.frame    = WCP_ANIM_NOFLAG;
	} else {              //allies
		ent->s.frame    = WCP_ANIM_NOFLAG;
	}

	// s.teamNum is which set of animations to use ( only 1 right now )
	ent->s.teamNum  = 1;

	//when its touched call CTF_touch, located in S4ND_func
	ent->touch      = cust_spawnflag;
	trap_LinkEntity( ent );
}

/*
=============================
SPAWN FLAG
=============================
*/
// Shoves the shit into the spawn flag
void SP_binocs( gentity_t *ent ) {
//	char *capture_sound;

	// Make it an item
	ent->s.eType        = ET_ITEM;

	//set the model
	ent->s.modelindex   = G_ModelIndex( "models/powerups/keys/binoculars.md3" );

	// more stuff to make it an item
	ent->clipmask   = CONTENTS_ITEM;
	ent->r.contents = CONTENTS_ITEM;

	//uhhhhh gonna have to play with these some more, leave them for now
	VectorSet( ent->r.mins, -8, -8, 0 );
	VectorSet( ent->r.maxs, 8, 8, 128 );

	G_SetOrigin( ent, ent->s.origin );
	G_SetAngle( ent, ent->s.angles );

	//when its touched call CTF_touch, located in S4ND_func
	ent->touch      = cust_spawnflag;
	trap_LinkEntity( ent );
}



// Runs the thinking code for the Spawn flag
void Spawn_think( gentity_t * self ) {
	switch ( self->s.frame ) {

	case WCP_ANIM_NOFLAG:
		break;
	case WCP_ANIM_RAISE_AXIS:
		self->s.frame = WCP_ANIM_AXIS_RAISED;
		break;
	case WCP_ANIM_RAISE_AMERICAN:
		self->s.frame = WCP_ANIM_AMERICAN_RAISED;
		break;
	case WCP_ANIM_AXIS_RAISED:
		break;
	case WCP_ANIM_AMERICAN_RAISED:
		break;
	case WCP_ANIM_AXIS_TO_AMERICAN:
		self->s.frame = WCP_ANIM_AMERICAN_RAISED;
		break;
	case WCP_ANIM_AMERICAN_TO_AXIS:
		self->s.frame = WCP_ANIM_AXIS_RAISED;
		break;
	case WCP_ANIM_AXIS_FALLING:
		self->s.frame = WCP_ANIM_NOFLAG;
		break;
	case WCP_ANIM_AMERICAN_FALLING:
		self->s.frame = WCP_ANIM_NOFLAG;
		break;
	default:
		break;

		//later...
		//	te = G_TempEntity( ent2->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
		//	te->s.eventParm = G_SoundIndex (index);
		//	te->s.teamNum = ent2->s.clientNum;
	}

	self->touch = cust_spawnflag;
	self->nextthink = 0;

}
//=========================
// Runs the "touch" code for the Spawn flag
void cust_spawnflag( gentity_t *self, gentity_t *other, trace_t *trace ) {
	if ( other->client->sess.sessionTeam == level.flagteam ) { //if the player is on same team as flag... dont do shit
		return;
	}

	if ( self->count2 == other->client->sess.sessionTeam ) {
		return;
	}


	// Set controlling team
	self->count = other->client->sess.sessionTeam;

	level.BlueSniperSpawnPoint = -1;
	level.RedSniperSpawnPoint = -1;
	// Set animation
	if ( self->count == TEAM_RED ) {
		level.flagteam = TEAM_RED;
		//	Agent - NO, waste of entity for a flag people can see...
		//self->dl_color[0] = 1;
		//self->dl_color[1] = 0;
		//self->dl_color[2] = 0;
		//SP_dlight(self);
		trap_SendServerCommand( -1, va( "cp \"Axis capture %s!\n\" 2", self->message ) );
		trap_SendServerCommand( -1, va( "print \"^3%s: ^5Axis capture %s!\n\" 2",GetLevelTime(), self->message ) );
		G_AddEvent( self, EV_GLOBAL_SOUND, G_SoundIndex( "sound/player/axis_fbcapt.wav" ) );
		if ( self->s.frame == WCP_ANIM_NOFLAG ) {
			self->s.frame = WCP_ANIM_RAISE_AXIS;
		} else if ( self->s.frame == WCP_ANIM_AMERICAN_RAISED ) {
			self->s.frame = WCP_ANIM_AMERICAN_TO_AXIS;
		} else {
			self->s.frame = WCP_ANIM_AXIS_RAISED;
		}
	} else {
		level.flagteam = TEAM_BLUE;
		//self->dl_color[0] = 0;
		//self->dl_color[1] = 0;
		//self->dl_color[2] = 1;
		//SP_dlight(self);
		trap_SendServerCommand( -1, va( "cp \"Allies capture %s!\n\" 2", self->message ) );
		trap_SendServerCommand( -1, va( "print \"^3%s: ^5Allies capture %s!\n\" 2",GetLevelTime(), self->message ) );
		G_AddEvent( self, EV_GLOBAL_SOUND, G_SoundIndex( "sound/player/allies_fbcapt.wav" ) );
		if ( self->s.frame == WCP_ANIM_NOFLAG ) {
			self->s.frame = WCP_ANIM_RAISE_AMERICAN;
		} else if ( self->s.frame == WCP_ANIM_AXIS_RAISED ) {
			self->s.frame = WCP_ANIM_AXIS_TO_AMERICAN;
		} else {
			self->s.frame = WCP_ANIM_AMERICAN_RAISED;
		}
	}

	// Play a sound
	G_AddEvent( self, EV_GENERAL_SOUND, self->soundPos1 );

	// Don't allow touch again until animation is finished
	self->touch = NULL;

	self->think = Spawn_think;
	self->nextthink = level.time + 1000;
}




/*////////////////////////////////////////////
MyInfront
my modified version of the built-in 'infront' function
I changed it so that we can have the bots field of vision 'fov'
change based on its skill
returns true if 'other' is in the bot's field of vision
*/                                                                                                                                                                                                                                                                         ////////////////////////////////////////////
qboolean MyInfront( gentity_t *self, gentity_t *other ) {
	vec3_t vec;
	float dot;
	vec3_t forward;

	float dotConst = 0.0;   //180 --DEFAULT - like cg_fov 90

	//get the direction in front of the bot
	AngleVectors( self->client->ps.viewangles, forward, NULL, NULL );
	//get the direction to the 'other'
	VectorSubtract( other->r.currentOrigin, self->r.currentOrigin, vec );
	//make 'vec' (which points from the bot to the 'other') have length 1 (necessary for math...)
	VectorNormalize( vec );
	//essentially get the angle between the bot and the 'other'
	dot = DotProduct( vec, forward );
	//if the 'other' is in the bot's fov, return true (yes, the 'other' is in fron of the bot)
	if ( dot > dotConst ) {
		return qtrue;
	}

	//otherwise return false
	return qfalse;
}


/*////////////////////////////////////////////
MyVisible
This is just a copy of the built-in 'visible' function
but accounting for the players' viewheight.  This lets the
bots see players slightly more often and in a more realistic way
*/                                                                                                                                                                                                                                                  ////////////////////////////////////////////
qboolean MyVisible( gentity_t *self, gentity_t *other ) {
	vec3_t selfView, otherView;     //basically stores the locations of the players' eyes
	trace_t tr;                     //used in performing the collision info
	gentity_t   *traceEnt;          //holds the entity returned by the trace function

	VectorCopy( self->r.currentOrigin, selfView );        //copy the bot's origin to this variable
	selfView[2] += self->client->ps.viewheight;         //add the bot's viewheight
	VectorCopy( other->r.currentOrigin, otherView );      //copy the target's origin
	otherView[2] += other->client->ps.viewheight;       //add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );

	traceEnt = &g_entities[tr.entityNum];       //set traceEnt to the entity the shot would hit

	if ( traceEnt == other ) {    //if it would hit the player, return true
		return qtrue;
	}

	return qfalse;              //it would hit something other than the player (or nothing) so return false

}


/*
==========
FindEnemy
==========
*/
qboolean FindEnemy( gentity_t *ent ) {
	int i;
	gentity_t       *targ;
	gentity_t       *bestenemy = NULL;
	float bestweight = -1;
	float weight;
	vec3_t dist;
	int viewDist;
	int reactionTime;


	viewDist = g_autoAim.integer;
	reactionTime = 50;

	//loop through all the clients to see if the bot can see any enemies
	for ( i = 0; i < level.maxclients; i++ )
	{
		targ = &g_entities[i];

		//make sure the client is a valid player (and in use)
		if ( !targ->inuse || !targ->client ) {
			continue;
		}

		// No autoaim for bots
		if ( ent->r.svFlags & SVF_BOT ) {
			continue;
		}

		if ( targ->client->ps.powerups[PW_INVULNERABLE] ) {
			continue;
		}

		//make sure the player isn't the bot or anyone on his team
		if ( ( targ == ent ) || ( targ->client->sess.sessionTeam == ent->client->sess.sessionTeam ) ) {
			continue;
		}

		//make sure the player isn't dead or spectating
		if ( ( ( targ->health <= 0 ) || ( targ->client->ps.pm_flags & PMF_LIMBO ) ) || targ->client->sess.sessionTeam == TEAM_SPECTATOR ) { //not dead or spectating...
			continue;
		}

		// Check if the bot can see the enemy from this position
		if ( MyVisible( ent, targ ) && MyInfront( ent, targ ) ) {
			// Find the distance between bot and enemy
			VectorSubtract( ent->r.currentOrigin, targ->r.currentOrigin, dist );

			//store it so we can compare
			weight = VectorLength( dist );

			//if this enemy is too far away to be seen, skip him
			if ( weight > viewDist ) {
				continue;
			}

			//if the new distance is the shortest we have found so far
			if ( ( weight < bestweight ) || ( bestweight == -1 ) ) {
				//set the new shortest distance and the new 'best' enemy
				bestweight = weight;
				bestenemy = targ;   //this loop (and this line) set 'bestenemy' to the closest visible enemy
			}
		}
	}

	//if the bot found an enemy
	if ( bestenemy ) {
		ent->enemy = bestenemy;     //set the enemy to best ememy
		return qtrue;               //return yes, he can see an enemy
	}

	return qfalse;      //no, the bot did not find any enemies
}

void DoAutoAim( gentity_t *ent ) {
	vec3_t vec, enemyvec,angles;

#ifdef OMNIBOT
	if ( ent->r.svFlags & SVF_BOT ) {
		return;
	}
#endif

	if ( ( ent->s.weapon != WP_LUGER ) &&
		 ( ent->s.weapon != WP_COLT ) &&
		 ( ent->s.weapon != WP_MAUSER ) &&
		 ( ent->s.weapon != WP_SNIPERRIFLE ) &&
		 ( ent->s.weapon != WP_VENOM ) &&
		 ( ent->s.weapon != WP_MP40 ) &&
		 ( ent->s.weapon != WP_STEN ) &&
		 ( ent->s.weapon != WP_PANZERFAUST ) &&
		 ( ent->s.weapon != WP_THOMPSON ) &&
		 ( ent->s.weapon != WP_FG42 ) &&
		 ( ent->s.weapon != WP_FG42SCOPE ) ) {
		return;
	}

	if ( !FindEnemy( ent ) ) { //check for an enemy :D
		return;
	}

	//Get Enemy Posistion
	VectorCopy( ent->enemy->r.currentOrigin, enemyvec );

	//Enemy crouched, aimer standing
	if ( ent->enemy->client->ps.pm_flags & PMF_DUCKED ) {
		enemyvec[2] -= ( ent->enemy->client->ps.viewheight - ent->enemy->client->ps.crouchViewHeight );
//			enemyvec[2] -= ent->enemy->client->ps.crouchViewHeight;
	}
	// Both Crouching
	else if ( ( ent->enemy->client->ps.pm_flags & PMF_DUCKED ) && ( ent->client->ps.pm_flags & PMF_DUCKED ) ) {
		enemyvec[2] -= ent->enemy->client->ps.crouchViewHeight;
	}
	// Enemy standing, human crouching
	else if ( ent->client->ps.pm_flags & PMF_DUCKED ) {
		//enemyvec[2] += ent->enemy->client->ps.crouchViewHeight;	//old
		//enemyvec[2] += ent->enemy->client->ps.viewheight;	// SUCKS
		enemyvec[2] += ( ent->enemy->client->ps.viewheight - ent->enemy->client->ps.crouchViewHeight );

	}
	// Both Standing
	else
	{
		//Works GREAT
	}

	//find the direction to the enemy
	VectorSubtract( enemyvec, ent->r.currentOrigin, vec );
	//turn to face the enemy- and own him :)
	vectoangles( vec, angles );

	SetClientViewAngle( ent, angles );

}
/*
====================
CheckRoundStart
====================
*/
void CheckRoundStart( gentity_t *ent ) {
//	int i=0;
//	gentity_t *te;
//	gentity_t *other;
//	char index[MAX_OSPATH];



	//Q_strncpyz(index, "sound/player/fight_rb.wav", sizeof(index));	// Martin - Old Way
	trap_SendServerCommand( ent - g_entities, va( "cp \"^1FIGHT!\n\"3" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"\n\"" ) );
	trap_SendServerCommand( ent - g_entities, va( "print \"^1FIGHT!\n\"" ) );
	G_AddEvent( ent, EV_GLOBAL_CLIENT_SOUND, G_SoundIndex( "sound/player/fight_rb.wav" ) );

/*	other = &g_entities[i];
    te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
    te->s.eventParm = G_SoundIndex (index);
    te->s.teamNum = other->s.clientNum;*/


}
/*
=====================
CheckFraglimitScores
=====================
*/

void CheckFraglimitScores() {

//	char index[MAX_OSPATH];
//	gentity_t *other;
//	gentity_t *te;
	if ( g_deathmatch.integer == 2 ) {
		int killLeader = 0;         // Sanity check
		int i;
		int killsNeeded;

		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			if ( level.clients[i].sess.sessionTeam != TEAM_RED ) {
				continue;
			}
			//	Here is the math.  We step through each guy checkin against the previous high killer.
			//	If there is a new one, set him as high killer.
			//	If there are 2 people with same kills... well fuck them lol.
			if ( level.clients[i].pers.kills > level.clients[killLeader].pers.kills ) {
				killLeader = i;
			}
		}
		//	Now that we found out who wtfPWNS, tell everyone, even specs/teamb people
		for ( i = 0; i < MAX_CLIENTS; i++ ) {
			if ( level.clients[i].pers.connected != CON_CONNECTED ) {
				continue;
			}
			//	Different message for people on axis so they know how far behind they are.
			if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
				//	Tell a player their score and the leaders
				//				Agent [27]
				//			 wolfplayer [5]
				//agent - let the winner know that he is winning the match
				if ( level.clients[i].ps.clientNum == level.clients[killLeader].ps.clientNum ) {
					killsNeeded = g_fraglimit.integer - level.clients[i].pers.kills;
					trap_SendServerCommand( level.clients[i].ps.clientNum, va( "cp \"You are winning with %i kills needed\n\"2",
																			   killsNeeded ) );
				} else {
					trap_SendServerCommand( level.clients[i].ps.clientNum, va( "cp \"%s ^7[%i]\n %s ^7[%i]\n\"2", level.clients[killLeader].pers.netname,
																			   level.clients[killLeader].pers.kills, level.clients[i].pers.netname, level.clients[i].pers.kills ) );
				}
				trap_SendServerCommand( level.clients[i].ps.clientNum, va( "print \"^3%s: ^7%s ^5is winning with %i of %i kills\n\"", GetLevelTime(),
																		   level.clients[killLeader].pers.netname, level.clients[killLeader].pers.kills, g_fraglimit.integer ) );
			} else {
				//	Just send a print message to everyone else incase they are makin a movie or something.
				trap_SendServerCommand( level.clients[i].ps.clientNum, va( "print \"^3%s: ^7%s ^5is winning with %i of %i kills\n\"", GetLevelTime(),
																		   level.clients[killLeader].pers.netname, level.clients[killLeader].pers.kills, g_fraglimit.integer ) );
			}
		}


	} else {
		if ( g_fraglimit.integer ) {
			if ( level.teamRedKills > level.teamBlueKills ) {
				trap_SendServerCommand( -1, va( "print \"^3%s: ^5Axis lead Allies %i to %i. Fraglimit is %i\n\"", GetLevelTime(),level.teamRedKills, level.teamBlueKills, g_fraglimit.integer ) );
				trap_SendServerCommand( -1, va( "cp \"^3SCORE[%i]: ^1Axis^7: %i ^4Allies^7: %i\n\"2", g_fraglimit.integer, level.teamRedKills, level.teamBlueKills ) );
				/*  for(i=0; i<MAX_CLIENTS; i++){
				    if (level.clients[i].pers.connected != CON_CONNECTED)
				        continue;
				    if(level.clients[i].sess.sessionTeam == TEAM_RED)
				        Q_strncpyz(index, "sound/player/takenlead.wav", sizeof(index));
				     if(level.clients[i].sess.sessionTeam == TEAM_BLUE)
				        Q_strncpyz(index, "sound/player/lostlead.wav", sizeof(index));
				    }*/
			}
			if ( level.teamBlueKills > level.teamRedKills ) {
				trap_SendServerCommand( -1, va( "print \"^3%s: ^5Allies lead Axis %i to %i. Fraglimit is %i\n\"",GetLevelTime(), level.teamBlueKills, level.teamRedKills, g_fraglimit.integer ) );
				trap_SendServerCommand( -1, va( "cp \"^3SCORE[%i]: ^4Allies^7: %i ^1Axis^7: %i\n\"2", g_fraglimit.integer, level.teamBlueKills, level.teamRedKills ) );
				/* for(i=0; i<MAX_CLIENTS; i++){
				    if (level.clients[i].pers.connected != CON_CONNECTED)
				        continue;
				    if(level.clients[i].sess.sessionTeam == TEAM_BLUE)
				        Q_strncpyz(index, "sound/player/takenlead.wav", sizeof(index));
				    if(level.clients[i].sess.sessionTeam == TEAM_RED)
				        Q_strncpyz(index, "sound/player/lostlead.wav", sizeof(index));
				    }
				    */
			}
			if ( level.teamRedKills == level.teamBlueKills ) {
				trap_SendServerCommand( -1, va( "print \"^3%s: ^5Teams are tied at %i. Fraglimit is %i\n\"",GetLevelTime(), level.teamRedKills, g_fraglimit.integer ) );
				trap_SendServerCommand( -1, va( "cp \"^3SCORE[%i]: ^5Teams are tied at %i\n\"2", g_fraglimit.integer, level.teamRedKills ) );
				//	 Q_strncpyz(index, "sound/player/tiedlead.wav", sizeof(index));

			}
			/*
			for(i=0; i<MAX_CLIENTS; i++){
			   if (level.clients[i].pers.connected != CON_CONNECTED)
			   continue;
			   other = &g_entities[i];
			   te = G_TempEntity( other->s.pos.trBase, EV_GLOBAL_CLIENT_SOUND );
			   te->s.eventParm = G_SoundIndex (index);
			   te->s.teamNum = other->s.clientNum;
			}*/
		}
		if ( g_maxlives.integer ) {
			level.AxisLeft      = level.numteamVotingClients[0] - level.numFinalDead[0];
			level.AlliesLeft    = level.numteamVotingClients[1] - level.numFinalDead[1];
			trap_SendServerCommand( -1, va( "print \"^3%s: ^5Allies left: %i Axis left: %i\n\"",GetLevelTime(), level.AlliesLeft, level.AxisLeft ) );
			trap_SendServerCommand( -1, va( "cp \"^4Allies Left^7: %i ^1Axis Left^7: %i\n\"2",level.AlliesLeft, level.AxisLeft ) );
		}

/*
    for(i=0; i<MAX_CLIENTS; i++){
            if (level.clients[i].pers.connected != CON_CONNECTED)
                continue;
            if ((level.clients[i].sess.sessionTeam == TEAM_BLUE) && ((level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT] > 0) ||(level.clients[i].pers.gibbed == qfalse )))
                level.AlliesLeft++;
            if ((level.clients[i].sess.sessionTeam == TEAM_RED) && ((level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT] > 0) ||(level.clients[i].pers.gibbed == qfalse )))
                level.AxisLeft++;
            }
        trap_SendServerCommand( -1, va("print \"^3%s: ^5Allies left: %i Axis left: %i\n\"",GetLevelTime(), level.AlliesLeft, level.AxisLeft) );
        trap_SendServerCommand(-1, va("cp \"^4Allies Left^7: %i ^1Axis Left^7: %i\n\"2",level.AlliesLeft, level.AxisLeft));
    }
    */
		if ( g_goldrush.integer ) {
			if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] ) {
				trap_SendServerCommand( -1, va( "print \"^3%s: ^5Axis lead Allies %i to %i\n\"", GetLevelTime(),level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] ) );
				trap_SendServerCommand( -1, va( "cp \"^3SCORE: ^1Axis^7: %i ^4Allies^7: %i\n\"2",level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] ) );
			} else if ( level.teamScores[TEAM_RED] < level.teamScores[TEAM_BLUE] )     {
				trap_SendServerCommand( -1, va( "print \"^3%s: ^5Allies lead Axis %i to %i\n\"",GetLevelTime(), level.teamScores[TEAM_BLUE], level.teamScores[TEAM_RED] ) );
				trap_SendServerCommand( -1, va( "cp \"^3SCORE: ^4Allies^7: %i ^1Axis^7: %i\n\"2",level.teamScores[TEAM_BLUE], level.teamScores[TEAM_RED] ) );
			} else if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] )     {
				trap_SendServerCommand( -1, va( "print \"^3%s: ^5Teams are tied at: %i\n\"",GetLevelTime(), level.teamScores[TEAM_RED] ) );
				trap_SendServerCommand( -1, va( "cp \"^3SCORE: ^5Teams are tied at: %i\n\"2", level.teamScores[TEAM_RED] ) );
			}
		}

	}

	level.LastFraglimitScore = level.time; //record that we did something...so we dont just return with nothing for our timestamp
}
/*==================
Checks to see if its time to tap out players for respawn
====================
*/
void CheckAutoTap( gentity_t *ent ) {
	int axisTime = 0;
	int alliedTime = 0;
//	int i;

	//level.LastAutoTapCheckTime = level.time;
	ent->lastAutoTapCheckTime = level.time;

	if ( g_deathmatch.integer ) { // Any DM mode = no checking
		return;
	}
	if ( ent->client->pers.gibbed == qtrue ) {
		return;
	}

	/*axisTime = level.time % g_redlimbotime.integer;
	axisTime = g_redlimbotime.integer - axisTime;
	alliedTime = level.time % g_bluelimbotime.integer;
	alliedTime = g_bluelimbotime.integer - alliedTime;*/

	axisTime = g_redlimbotime.integer - ( ( level.dwRedReinfOffset + level.time - level.startTime ) % g_redlimbotime.integer);
	alliedTime = g_bluelimbotime.integer - ( ( level.dwBlueReinfOffset + level.time - level.startTime ) % g_bluelimbotime.integer);

	/*if ( ( g_redlimbotime.integer - ( ( level.dwRedReinfOffset + level.time - level.startTime ) % g_redlimbotime.integer ) ) < 2000 ) {
					Cmd_Kill_f( ent );
					if ( !ent->client->sess.botSuicidePersist || g_gamestate.integer != GS_PLAYING ) {
						ent->client->sess.botSuicide = qfalse;
					}

					return qtrue;
				}
			} else if ( ent->client->sess.sessionTeam == TEAM_BLUE && !ent->client->ps.powerups[PW_REDFLAG] )      {
				if ( ( g_bluelimbotime.integer - ( ( level.dwBlueReinfOffset + level.time - level.startTime ) % g_bluelimbotime.integer ) ) < 2000 ) {
*/
	//Tap em if its .5sec till spawn
	if ( ( axisTime < 500 ) || ( alliedTime < 500 ) ) {
		if ( ent->client->pers.connected != CON_CONNECTED ) {
			return;
		}
		if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
			return;
		}
		if ( ( ent->client->sess.sessionTeam == TEAM_RED ) && ( axisTime > 500 ) ) {
			return;
		}
		if ( ( ent->client->sess.sessionTeam == TEAM_BLUE ) && ( alliedTime > 500 ) ) {
			return;
		}
		if ( ent->client->pers.isTapped == qfalse ) {
			return;
		}
		// Smarter...if a nearby medic can revive, dont tap them...
		// if they have missed spawn twice, send them regardless (may be a glitch or something)
		if ( ( isRevivingMedic( ent ) ) && ( ent->client->pers.spawnCyclesSkipped < 3 ) ) {
			ent->client->pers.spawnCyclesSkipped++;
			trap_SendServerCommand( ent - g_entities, va( "chat \"Skipping deployment because a medic is coming to revive you.\"" ) );
			return;
		}
		if ( g_tapReports.integer == 1 ) {
			trap_SendServerCommand( -1, va( "print \"%s ^7tapped out\n\"", ent->client->pers.netname ) );
		}
		if ( g_tapReports.integer > 1 ) {
			if ( ent->client->sess.sessionTeam == TEAM_RED ) {
				G_TeamCommand( TEAM_RED, va( "print \"%s ^7tapped out\n\"", ent->client->pers.netname ) );
			}
			if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
				G_TeamCommand( TEAM_BLUE, va( "print \"%s ^7tapped out\n\"",  ent->client->pers.netname ) );
			}
		}
		ent->client->pers.isTapped = qfalse;
		ent->client->pers.spawnCyclesSkipped = 0;
		limbo( ent, qtrue );
	}

}
/*
==========
isAirstrikeAvailable

Determines if the player can throw an air/arty. only called upon attempted use
==========
*/
qboolean isAirstrikeAvailable( gentity_t* ent ) {
//	int i;


	if ( ( g_maxAirstrikes.integer < 3 ) && ( g_maxAirstrikes.integer >= 0 ) ) {
		trap_Cvar_Set( "g_maxAirstrikes", "3" );
	}


	// Take off as many seconds as its been since the last check.
	// No need to do this in runframe like ET does since it isnt used every frame...
	level.axisAirstrikeCounter = level.axisAirstrikeCounter - ( level.time - level.LastAirstrikeCheck );
	level.alliedAirstrikeCounter = level.alliedAirstrikeCounter - ( level.time - level.LastAirstrikeCheck );
	if ( level.axisAirstrikeCounter < 0 ) {
		level.axisAirstrikeCounter = 0;
	}
	if ( level.alliedAirstrikeCounter < 0 ) {
		level.alliedAirstrikeCounter = 0;
	}

	level.LastAirstrikeCheck = level.time;
/*
    // If its > 60000 [or something else in the future]... "insufficient fire support"
    if( ent->client->sess.sessionTeam == TEAM_RED ) {
        if( level.axisAirstrikeCounter >= 60000 ) {
            return qfalse;
        }
    }
    if( ent->client->sess.sessionTeam == TEAM_BLUE ) {
        if( level.alliedAirstrikeCounter >= 60000 ) {
            return qfalse;
        }
    }*/

	// New way...adds the current 'cost' of this throw to the check to see if they really can throw it
	if ( ent->client->sess.sessionTeam == TEAM_RED ) {
		int cost = 60000 / g_maxAirstrikes.integer;
		if ( level.axisAirstrikeCounter + cost >= 60000 ) {
			//trap_SendServerCommand(-1, va("chat \"Level: %i  Cost: %i\n\"",level.axisAirstrikeCounter, cost));
			return qfalse;
		}
	}
	if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
		int cost = 60000 / g_maxAirstrikes.integer;
		if ( level.alliedAirstrikeCounter + cost >= 60000 ) {
			return qfalse;
		}
	}

	return qtrue;
}
/*
==========
calculateStartLives

For a g_maxlives situation, this calculates how many lives a new connecting client starts with.
==========
*/
int calculateStartLives( gentity_t* ent ) {
	int i;
	int teamPlayers = 0;
	int teamLives = 0;

	for ( i = 0; i < level.maxclients; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}
		if ( OnSameTeam( ent,&g_entities[level.clients[i].ps.clientNum] ) ) {
			teamPlayers++;
			teamLives = teamLives + level.clients[i].ps.persistant[PERS_RESPAWNS_LEFT];
		}
	}
	if ( teamPlayers > 0 ) {
		return teamLives / teamPlayers; //assign the new player his lives (always rounded down cause he joined late...
	} else {
		return 0;
	}
}
/*
==========
showHitBoxes

Shows bounding boxes of all players upon being called.
==========
*/
void showHitBoxes( gentity_t *ent ) {
	int i;
	gentity_t *bboxEnt;
	vec3_t b1, b2;

	for ( i = 0; i < level.maxclients; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_SPECTATOR ) {
			continue;
		}


		VectorCopy( ent->r.currentOrigin, b1 );
		VectorCopy( ent->r.currentOrigin, b2 );
		VectorAdd( b1, ent->r.mins, b1 );
		VectorAdd( b2, ent->r.maxs, b2 );
		bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
		VectorCopy( b2, bboxEnt->s.origin2 );
		bboxEnt->s.dmgFlags = g_test1.integer;  // ("type")
	}
}
/*
==========
CheckMapRotation

Checks to see if 1. server is empty, 2. it has been empty a long time and we should go to the next map
==========
*/
// THIS FUNCTION IS NEVER CALLED BECAUSE IT IS WORTHLESS :)
void CheckMapRotation() {
/*	if ((g_sniperWar.integer <1) || (g_deathmatch.integer <1)){
    //Ok retards... g_timelimit is in mins... mins->secs->millasecs  [Roundtime - (currenttime - starttime) = 0 --> change]<<<THIS IS WRONG!!!
        if((level.numConnectedClients == 0) && (((g_timelimit.integer * 60 * 1000) - (level.time - level.startTime)) <= 0 )){
            trap_SendServerCommand(-1, va("chat \"Changing maps due to empty server.  ^1(%i)!!!\n\"",level.numConnectedClients));
            trap_SendServerCommand(-1, va("print \"Clients: ^1(%i)!!! Time: %i\n\"",level.numConnectedClients,((g_timelimit.value * 60 * 1000) - (level.time - level.startTime))));
            trap_SendConsoleCommand( EXEC_APPEND, va( "vstr nextmap\n") );
        }
    }else{
    */
	//	if ((g_sniperWar.integer <1) || (g_deathmatch.integer <1)){
	if ( ( level.numConnectedClients == 0 ) && ( level.time >= 600000 ) ) {
		trap_SendServerCommand( -1, va( "chat \"Changing maps due to empty server.  ^1(%i)!!!\n\"",level.numConnectedClients ) );
		trap_SendServerCommand( -1, va( "print \"Clients: ^1(%i)!!! Time: %i\n\"",level.numConnectedClients,( ( g_timelimit.value * 60 * 1000 ) - ( level.time - level.startTime ) ) ) );
		trap_SendConsoleCommand( EXEC_APPEND, va( "vstr nextmap\n" ) );
		//}
//	}
	}
	level.LastMapRotationCheck = level.time;
}
/*=========================
demoHeartbeat
Heartbeats to the master server so demo servers can stay on list
//=======================*/
void demoHeartbeat() {
	trap_Cvar_Set("sv_master1", "wolfmaster.idsoftware.com");
	trap_Cvar_Set("sv_master2", "etmaster.idsoftware.com");
	trap_Cvar_Set("sv_master3", "master3.idsoftware.com");
	trap_Cvar_Set("sv_master4", "dpmaster.deathmask.net");
	trap_SendConsoleCommand( EXEC_APPEND, va( "heartbeat\n" ) );
}


void stripcvars( const char *buffer, char *checkcvar, char* cvarfrom, char* cvarto ) {

	int i = 0, a = 0, b = 0, c = 0;
	int foundspace = 0;


	while ( buffer[i] != 0 ) {
		if ( !foundspace ) { //havent found any periods
			if ( buffer[i] == ' ' ) {
				foundspace = 1;
				checkcvar[a] = 0;
			} else {
				checkcvar[a] = buffer[i];
			}
			i++;
			a++;
		} else if ( foundspace == 1 )    { // We have found 1 period
			if ( buffer[i] == ' ' ) {
				foundspace = 2;
				cvarfrom[b] = 0;
			} else {
				cvarfrom[b] = buffer[i];
			}
			i++;
			b++;
		} else if ( foundspace == 2 )    { // We have found 2 periods
			if ( buffer[i] == ' ' ) {
				foundspace = 3;
				cvarto[c] = 0;
			} else {
				cvarto[c] = buffer[i];
			}
			i++;
			c++;
		}
	}


}
/*=========================
Auto Respawn seter
//=======================*/
void AutoSpawnTimes( gentity_t* ent ) {
	int i;
	int RedPlayers = 0;
	int BluePlayers = 0;
	char mapName[32];
	//ya this doesnt do shit yet :)

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
			BluePlayers++;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
			RedPlayers++;
		}
	}
	if ( !Q_stricmp( mapName, "mp_assault" ) ) {    //	40/30
		if ( ( RedPlayers ) <= 10 ) {
			trap_Cvar_Set( "g_redlimbotime", "10000" );
		} else if ( ( RedPlayers ) <= 20 ) {
			trap_Cvar_Set( "g_redlimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_RedDefaultSpawnTime ) );
		}

		//	Allies
		if ( ( BluePlayers ) <= 10 ) {
			trap_Cvar_Set( "g_bluelimbotime", "20000" );
		} else if ( ( BluePlayers ) <= 20 ) {
			trap_Cvar_Set( "g_bluelimbotime", "30000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_BlueDefaultSpawnTime ) );
		}
	}
	if ( !Q_stricmp( mapName, "mp_base" ) ) {       //	20/30
		if ( ( RedPlayers ) <= 10 ) {
			trap_Cvar_Set( "g_redlimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_RedDefaultSpawnTime ) );
		}

		//	Allies
		if ( ( BluePlayers ) <= 10 ) {
			trap_Cvar_Set( "g_bluelimbotime", "10000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_BlueDefaultSpawnTime ) );
		}
	}
	if ( !Q_stricmp( mapName, "mp_beach" ) ) {      //	30/40
		if ( ( RedPlayers ) <= 10 ) {
			trap_Cvar_Set( "g_redlimbotime", "20000" );
		} else if ( ( RedPlayers ) <= 20 ) {
			trap_Cvar_Set( "g_redlimbotime", "30000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_RedDefaultSpawnTime ) );
		}

		//	Allies
		if ( ( BluePlayers ) <= 10 ) {
			trap_Cvar_Set( "g_bluelimbotime", "10000" );
		} else if ( ( BluePlayers ) <= 20 ) {
			trap_Cvar_Set( "g_bluelimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_BlueDefaultSpawnTime ) );
		}
	}
	if ( !Q_stricmp( mapName, "mp_castle" ) ) {     //	15/20 :S
		if ( ( RedPlayers ) <= 10 ) {
			trap_Cvar_Set( "g_redlimbotime", "15000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_RedDefaultSpawnTime ) );
		}

		//	Allies
		if ( ( BluePlayers ) <= 10 ) {
			trap_Cvar_Set( "g_bluelimbotime", "10000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_BlueDefaultSpawnTime ) );
		}
	}
	if ( !Q_stricmp( mapName, "mp_depot" ) ) {      //	30/30
		if ( ( RedPlayers ) <= 10 ) {
			trap_Cvar_Set( "g_redlimbotime", "10000" );
		} else if ( ( RedPlayers ) <= 20 ) {
			trap_Cvar_Set( "g_redlimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_RedDefaultSpawnTime ) );
		}

		//	Allies
		if ( ( BluePlayers ) <= 10 ) {
			trap_Cvar_Set( "g_bluelimbotime", "10000" );
		} else if ( ( BluePlayers ) <= 20 ) {
			trap_Cvar_Set( "g_bluelimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_BlueDefaultSpawnTime ) );
		}
	}
	if ( !Q_stricmp( mapName, "mp_destruction" ) ) {    //	30/30
		if ( ( RedPlayers ) <= 10 ) {
			trap_Cvar_Set( "g_redlimbotime", "10000" );
		} else if ( ( RedPlayers ) <= 20 ) {
			trap_Cvar_Set( "g_redlimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_RedDefaultSpawnTime ) );
		}

		//	Allies
		if ( ( BluePlayers ) <= 10 ) {
			trap_Cvar_Set( "g_bluelimbotime", "10000" );
		} else if ( ( BluePlayers ) <= 20 ) {
			trap_Cvar_Set( "g_bluelimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_BlueDefaultSpawnTime ) );
		}
	}
	if ( !Q_stricmp( mapName, "mp_sub" ) ) {    //	20/35
		if ( ( RedPlayers ) <= 10 ) {
			trap_Cvar_Set( "g_redlimbotime", "15000" );
		} else if ( ( RedPlayers ) <= 20 ) {
			trap_Cvar_Set( "g_redlimbotime", "25000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_RedDefaultSpawnTime ) );
		}

		//	Allies
		if ( ( BluePlayers ) <= 10 ) {
			trap_Cvar_Set( "g_bluelimbotime", "10000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_BlueDefaultSpawnTime ) );
		}
	}
	if ( !Q_stricmp( mapName, "mp_village" ) ) {    //	30/30
		if ( ( RedPlayers ) <= 10 ) {
			trap_Cvar_Set( "g_redlimbotime", "10000" );
		} else if ( ( RedPlayers ) <= 20 ) {
			trap_Cvar_Set( "g_redlimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_RedDefaultSpawnTime ) );
		}

		//	Allies
		if ( ( BluePlayers ) <= 10 ) {
			trap_Cvar_Set( "g_bluelimbotime", "10000" );
		} else if ( ( BluePlayers ) <= 20 ) {
			trap_Cvar_Set( "g_bluelimbotime", "20000" );
		} else {
			trap_Cvar_Set( "g_redlimbotime", va( "%i000", g_BlueDefaultSpawnTime ) );
		}
	}

}
/*
==========
checkEvenTeams

Checks to see if teams are unbalanced
==========
*/
void checkEvenTeams() {
	if ( !g_autoTeamBalance.integer ) {
		return;
	}
	if ( g_gamestate.integer != GS_PLAYING ) {
		return;
	}
	if ( g_deathmatch.integer == 2 ) {
		return;
	}
	if ( g_lockteams.integer ) {
		return;
	}
	if ( g_needBalance.integer ) { //we are already going to check, dont reset timer
		return;
	}

	countPlayers();

	if ( ( ( level.axisPlayers - level.alliedPlayers ) > 1 ) || ( ( level.alliedPlayers - level.axisPlayers ) > 1 ) ) {
		trap_Cvar_Set( "g_needBalance", "1" );
		trap_SendServerCommand( -1, va( "chat \"console: ^5Teams will be balanced in ^315 ^5seconds\n\"" ) );
		level.balanceTime = level.time + 15000;
	}
}
/*
==========
countPlayers

Records how many people are on allies + axis
==========
*/
void countPlayers() {
	int i;
	level.axisPlayers = 0;
	level.alliedPlayers = 0;

	for ( i = 0; i < level.maxclients; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
			level.axisPlayers++;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
			level.alliedPlayers++;
		}
	}
}
/*
==========
stripGuid

Returns an 8char GUID instead of full 32
==========
*/
void stripGuid( char *in, char *out ) {
	int len = strlen( in );
	int i = 0, j = 0;

	for ( i = len - 8; i < len; i++ )
		out[j++] = in[i];

	out[j++] = '\0';

}

/*
==========
balanceTeams

Balances teams.  Do this regardless of gamestate
==========
*/
void balanceTeams() {
	int lowScoreClient = -1;
	int lowScore = 0;
	int i;

	while ( ( ( level.axisPlayers - level.alliedPlayers ) > 1 ) || ( ( level.alliedPlayers - level.axisPlayers ) > 1 ) ) {
		countPlayers();
		lowScoreClient = -1;
		if ( ( level.axisPlayers - level.alliedPlayers ) > 1 ) {
			for ( i = 0; i < level.maxclients; i++ ) {
				if ( level.clients[i].pers.connected != CON_CONNECTED ) {
					continue;
				}
				if ( level.clients[i].sess.sessionTeam != TEAM_RED ) { // only want to move axis players
					continue;
				}
				if ( lowScoreClient == -1 ) {
					lowScoreClient = i;
					lowScore = level.clients[i].ps.persistant[PERS_SCORE];
				} else if ( level.clients[i].ps.persistant[PERS_SCORE] < lowScore )    {
					lowScoreClient = i;
					lowScore = level.clients[i].ps.persistant[PERS_SCORE];
				}
			}

			SetTeam( &g_entities[lowScoreClient], "b",qtrue );
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7was forced to ^4Allies ^7to balance teams.\n\"",level.clients[lowScoreClient].pers.netname ) );
		}
		if ( ( level.alliedPlayers - level.axisPlayers ) > 1 ) {
			for ( i = 0; i < level.maxclients; i++ ) {
				if ( level.clients[i].pers.connected != CON_CONNECTED ) {
					continue;
				}
				if ( level.clients[i].sess.sessionTeam != TEAM_BLUE ) { // only want to move allied players
					continue;
				}
				if ( lowScoreClient == -1 ) {
					lowScoreClient = i;
					lowScore = level.clients[i].ps.persistant[PERS_SCORE];
				} else if ( level.clients[i].ps.persistant[PERS_SCORE] < lowScore )    {
					lowScoreClient = i;
					lowScore = level.clients[i].ps.persistant[PERS_SCORE];
				}
			}
			SetTeam( &g_entities[lowScoreClient], "r",qtrue );
			trap_SendServerCommand( -1, va( "chat \"console: %s ^7was forced to ^1Axis ^7to balance teams.\n\"",level.clients[lowScoreClient].pers.netname ) );
		}
	}
	trap_Cvar_Set( "g_needBalance", "0" );
}

/*
Aimbot decection
*/

int zbc_jittermax = 4;
int zbc_jittertime = 10;
int zbc_jittermove = 500;

qboolean zbc_ZbotCheck( gentity_t *ent, usercmd_t *ucmd ) {
	int tog0, tog1;


	if ( ent->client->pers.connected != CON_CONNECTED ) {
		return qfalse;      // unconnected clients may have weird mouse movements
	}
	if ( ent->client->sess.sessionTeam == TEAM_SPECTATOR ) {
		return qfalse;      // Spectators can aimbot all they want

	}
	zbc_jittermax = g_test1.integer;
	zbc_jittertime = g_test2.integer;
	zbc_jittermove = g_test3.integer;


	tog0 = ent->zbc_tog;
	ent->zbc_tog ^= 1;
	tog1 = ent->zbc_tog;

	if ( ucmd->angles[0] == ent->zbc_angles[tog1][0] &&
		 ucmd->angles[1] == ent->zbc_angles[tog1][1] &&
		 ucmd->angles[0] != ent->zbc_angles[tog0][0] &&
		 ucmd->angles[1] != ent->zbc_angles[tog0][1] &&
		 abs( ucmd->angles[0] - ent->zbc_angles[tog0][0] ) +
		 abs( ucmd->angles[1] - ent->zbc_angles[tog0][1] ) >= zbc_jittermove ) {

		if ( level.time <= ent->zbc_jitter_last + 0.1 ) {
			if ( !ent->zbc_jitter ) {
				ent->zbc_jitter_time = level.time;
			}
			if ( ++ent->zbc_jitter >= zbc_jittermax ) {
				return qtrue;
			}
		}

		ent->zbc_jitter_last = level.time;
	}

	ent->zbc_angles[tog1][0] = ucmd->angles[0];
	ent->zbc_angles[tog1][1] = ucmd->angles[1];

	if ( level.time > ent->zbc_jitter_time + zbc_jittertime ) {
		ent->zbc_jitter = 0;
	}

	return qfalse;
}
/*=================
canRetake
    returns true if:  X objectives have not yet been destroyed
    returns false if: X objectives have been destroyed
=================*/
qboolean canRetake() {
	char mapName[MAX_QPATH];

	trap_Cvar_VariableStringBuffer( "mapname", mapName, sizeof( mapName ) );

	if ( !Q_stricmp( mapName, "mp_assault" ) ) { // Hatch: 369	Warehouse: 511
		if ( ( ( !g_entities[369].inuse ) && ( Q_stricmp( g_entities[369].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[511].inuse ) && ( Q_stricmp( g_entities[511].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "mp_beach" ) ) {  // Door: 108	Breach: 100
		if ( ( ( !g_entities[108].inuse ) && ( Q_stricmp( g_entities[108].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[100].inuse ) && ( Q_stricmp( g_entities[100].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "mp_sub" ) ) { // Door: 89	Filt: 86
		if ( ( ( !g_entities[89].inuse ) && ( Q_stricmp( g_entities[89].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[86].inuse ) && ( Q_stricmp( g_entities[86].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "mp_village" ) ) { // crypt: 77
		if ( ( !g_entities[77].inuse ) && ( Q_stricmp( g_entities[77].classname, "func_explosive" ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "mp_ice" ) ) { // service: 481	side wall: 75
		if ( ( ( !g_entities[481].inuse ) && ( Q_stricmp( g_entities[481].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[75].inuse ) && ( Q_stricmp( g_entities[75].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "mp_rocket" ) ) { // rubble: 117
		if ( ( !g_entities[117].inuse ) && ( Q_stricmp( g_entities[117].classname, "func_explosive" ) ) ) {
			return qfalse;
		}
	}
	if ( ( !Q_stricmp( mapName, "mp_dam" ) ) || ( !Q_stricmp( mapName, "mp_dam_small" ) ) ) { // Upper front: 641 Lower front: 395 Main: 638
		if ( ( ( !g_entities[641].inuse ) && ( Q_stricmp( g_entities[641].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[395].inuse ) && ( Q_stricmp( g_entities[395].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[638].inuse ) && ( Q_stricmp( g_entities[638].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	if ( ( !Q_stricmp( mapName, "mp_tram" ) ) || ( !Q_stricmp( mapName, "mp_tram_small" ) ) ) {   // lower door: 546
		if ( ( !g_entities[546].inuse ) && ( Q_stricmp( g_entities[546].classname, "func_explosive" ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "tundra_rush_beta" ) ) {  // Gate: 177
		if ( ( !g_entities[177].inuse ) && ( Q_stricmp( g_entities[177].classname, "func_explosive" ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "te_ufo" ) ) { // south: 172 main: 301
		if ( ( ( !g_entities[172].inuse ) && ( Q_stricmp( g_entities[172].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[301].inuse ) && ( Q_stricmp( g_entities[301].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "te_frostbite" ) ) {  // main: 254 service: 225
		if ( ( ( !g_entities[254].inuse ) && ( Q_stricmp( g_entities[254].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[225].inuse ) && ( Q_stricmp( g_entities[225].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "axis_complex" ) ) {  // Flag door: 103 Main: 102
		if ( ( ( !g_entities[103].inuse ) && ( Q_stricmp( g_entities[103].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[102].inuse ) && ( Q_stricmp( g_entities[102].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	if ( !Q_stricmp( mapName, "te_escape2" ) ) { // bridge: 428 lower: 418
		if ( ( ( !g_entities[428].inuse ) && ( Q_stricmp( g_entities[428].classname, "func_explosive" ) ) )
			 && ( ( !g_entities[418].inuse ) && ( Q_stricmp( g_entities[418].classname, "func_explosive" ) ) ) ) {
			return qfalse;
		}
	}
	return qtrue;
}
qboolean S4ND_IsDemo()
{
	char version[64];
	char protocol[3];
	char paksHash[33];
	char pakNameHash[33];
	
	// Martin - 9/13/2012
	return qtrue;

	trap_Cvar_VariableStringBuffer( "protocol", protocol, sizeof( protocol ) );	
	trap_Cvar_VariableStringBuffer( "version", version, sizeof( version ) );

	getmd5(sv_referencedPaks.string, paksHash);
	getmd5(sv_referencedPakNames.string, pakNameHash);

	if ((!Q_stricmp( paksHash, "9c38722a6e368db14f0162be3b7da3e2" )) 
		&& (!Q_stricmp( pakNameHash, "73b7fcb69c4c994480eea9bb722234ba" )) 
		&& (atoi(protocol) == 50)
		&& (strstr( version, "Wolf 1.1")))
	{
		return qtrue;
	}
	return qfalse;
}
void S4ND_InitGame() {
	char pakNames[4096];
	char serverInfo[16];
	char version[64];
	char buf[32];
	char substring[32];
	char bufHash[33];
	int i;
	float j;
	int shift = 7;
	int zero = 0;
	int num = 20;

	trap_Cvar_VariableStringBuffer( "protocol", serverInfo, sizeof( serverInfo ) );
	trap_Cvar_VariableStringBuffer( "version", version, sizeof( version ) );

//	trap_Cvar_Set( "mod_url", "http://www.s4ndmod.com" );

	// RtCW Demo
	if (S4ND_IsDemo())
		return;

	if ( !Q_stricmp( serverInfo, "57" ) ) {
		/*if ( strstr( version, "Wolf 1.3" ) == NULL ) {
			Com_Error( ERR_FATAL, "Servers MUST run executable version 1.3 available at www.s4ndmod.com\n" );
		}*/


		// Does not work right on linux
/*		trap_Cvar_VariableStringBuffer( "fs_game", serverInfo, sizeof( serverInfo ) );
		// Demo will not work correctly with fs_game set
		if ( !Q_stricmp( serverInfo, "s4ndmod" ) ) {
			trap_SendServerCommand( -1, va( "print \"\n\n\n'%s'\n\n\n\"", serverInfo ) );
			Com_Error( ERR_FATAL, "Wrong mod directory, should be 's4ndmod'. Features break when installed incorrectly.\n" );
		}*/
		
		trap_Cvar_VariableStringBuffer( "sv_pakNames", pakNames, sizeof( pakNames ) );
		// IRATA: unfortunately pakNames is emtpy if mod is not init
		// Martin - pakNames is empty if pure is disabled!

		if ( pakNames[0] ) {

			// We need these pk3s...
			if ( strstr( pakNames, "s4ndmod_bin_2010_01_14" ) != NULL ) {
				Com_Error( ERR_FATAL, "No s4ndmod_bin_2010_01_14.pk3 found on server! Missing essential files!\n" );
			}

			// These are old pk3s...
			if ( strstr( pakNames, "s4ndmod_bin_2009_11_12" ) != NULL ) {
				Com_Error( ERR_FATAL, "Old binary s4ndmod_bin_2009_11_12.pk3 found on server! Remove it.\n" );
			}
			if ( strstr( pakNames, "s4ndmod_bin_2009_11_17" ) != NULL ) {
				Com_Error( ERR_FATAL, "Old binary s4ndmod_bin_2009_11_17.pk3 found on server! Remove it.\n" );
			}
			if ( strstr( pakNames, "S4NDMoD_Scripts_01" ) != NULL ) {
				Com_Error( ERR_FATAL, "Old binary S4NDMoD_Scripts_01.pk3 found on server! Remove it.\n" );
			}
			if ( strstr( pakNames, "S4NDMoD_Scripts_02" ) != NULL ) {
				Com_Error( ERR_FATAL, "Old binary S4NDMoD_Scripts_02.pk3 found on server! Remove it.\n" );
			}
		/*	if ( !strstr( pakNames, "hs sounds2" ) ) {
				Com_Error( ERR_FATAL, "Old binary hs sounds2.pk3 found on server! Remove it.\n" );
			}
			if ( !strstr( pakNames, "hs_sounds2" ) ) {
				Com_Error( ERR_FATAL, "Old binary hs_sounds2.pk3 found on server! Remove it.\n" );
			}
			if ( !strstr( pakNames, "S4NDMoD_Hitsounds_00" ) ) {
				Com_Error( ERR_FATAL, "Old binary S4NDMoD_Hitsounds_00.pk3 found on server! Remove it.\n" );
			}
			if ( !strstr( pakNames, "S4NDMod-pak1" ) ) {
				Com_Error( ERR_FATAL, "Old binary S4NDMod-pak1.pk3 found on server! Remove it.\n" );
			}*/
		}
	}
	// gamename check
	strcpy (substring, gamename.string);

	j = 12;
	for ( i = 0; substring[i]; i++ ) 
	{
		buf[i] = substring[i] + shift;
	}
	buf[i+1] = '\0';

	getmd5(buf, bufHash);
	getmd5(bufHash, bufHash);
	/*if (Q_stricmp( bufHash, "3741cddffa7c2015198987e428bfc5f2")) 
	{
		j = j / zero;
		num = num / zero;
		Com_Error( ERR_FATAL, "" );
	}

	// Clean Memory
	memset(&substring, '\0', sizeof(substring));
	memset(&buf, '\0', sizeof(buf));
	memset(&bufHash, '\0', sizeof(bufHash));
	// ** URL CHECK **
	shift = -3; // Change the shift
	strcpy (substring, mod_url.string);

	for ( i = 0; substring[i]; i++ ) 
	{
		buf[i] = substring[i] + shift;
	}
	buf[i+1] = '\0';

	getmd5(buf, bufHash);
	getmd5(bufHash, bufHash);

	if (Q_stricmp( bufHash, "001d797e0fe69c344fe94e5a1bb999cf" )) 
	{
		j = j / zero;
		Com_Error( ERR_FATAL, "" );
	}*/

}
/*=========================
setupCvars
    Sets cvars that need to be used by the game.
//=======================*/
void setupCvars() {
	char maxrate[16];

	trap_Cvar_Set( "sv_master5", "wolfmaster.s4ndmod.com" );

#ifndef _DEBUG
	//trap_Cvar_Set( "sv_punkbuster", "1" );	//Martin - This should be checked to ensure that PB is on, instead of forcing.
#endif

	// Reset airstrike check stuff so that someone can throw it the first time.
	level.axisAirstrikeCounter = 0;
	level.alliedAirstrikeCounter = 0;
	level.LastAirstrikeCheck = 0;
	level.axisMinesAvailable = g_mines.integer;
	level.alliedMinesAvailable = g_mines.integer;


	trap_Cvar_VariableStringBuffer( "sv_maxrate", maxrate, sizeof( maxrate ) );

	if ( atoi( maxrate ) < 50000 ) {
		trap_Cvar_Set( "sv_maxrate", "50000" );
	}
	trap_SendConsoleCommand( EXEC_APPEND, va( "pb_sv_kicklen %i", AB_TempBanTime.integer ) );
	trap_Cvar_Set( "sv_minping", "0" );
	trap_Cvar_Set( "sv_maxping", "0" );
	trap_Cvar_Set( "r_vertexlight", "1" );      // Martin - I think this will fix the vertexlight prob
	trap_Cvar_Set( "r_primitives", "2" );		// Martin - 7/6/09 - Fix video driver issues with modern hardware

	updateUptime();     // set our time server has been running for in systeminfo

	// Prevent too much ammo in gun bug (where dropping causes more ammo & leads to server crash)
	if ( g_medExtraClips.integer > 9 ) {
		trap_Cvar_Set( "g_medExtraClips", "9" );
	}
	if ( g_engExtraClips.integer > 9 ) {
		trap_Cvar_Set( "g_engExtraClips", "9" );
	}
	if ( g_ltExtraClips.integer > 9 ) {
		trap_Cvar_Set( "g_ltExtraClips", "9" );
	}
	if ( g_soldExtraClips.integer > 9 ) {
		trap_Cvar_Set( "g_soldExtraClips", "9" );
	}
	if ( g_venomExtraClips.integer > 3 ) {
		trap_Cvar_Set( "g_venomExtraClips", "3" );
	}
	if ( g_mauserExtraClips.integer > 3 ) {
		trap_Cvar_Set( "g_mauserExtraClips", "3" );
	}

	// So we dont have super unbalanced teams after warmup is done (players d/c or dont have map...)
	if ( g_gamestate.integer != GS_WARMUP ) {
		checkEvenTeams();
	}
}

/*=========================
isRevivingMedic
    returns true if the medic appears like they will revive:
        1. Is within the usual radius, alive, etc
        2. **Has a syringe OUT (wp # 26)
    They will skip the current spawn cycle if both are met.
//=======================*/
qboolean isRevivingMedic( gentity_t *self ) {
	int i, medic = -1;
	gclient_t   *cl;
	vec3_t start, end;
	vec3_t temp;
	trace_t tr;
	float bestdist = 1024, dist;

	//self->client->ps.viewlocked_entNum = 0;
	//self->client->ps.viewlocked = 0;
	//self->client->ps.stats[STAT_DEAD_YAW] = 999;

	VectorCopy( self->s.pos.trBase, start );
	start[2] += self->client->ps.viewheight;

	for ( i = 0; i < level.numConnectedClients; i++ ) {
		cl = &level.clients[ level.sortedClients[i] ];

		if ( level.sortedClients[i] == self->client->ps.clientNum ) {
			continue;
		}
		if ( cl->sess.sessionTeam != self->client->sess.sessionTeam ) {
			continue;
		}
		if ( cl->ps.pm_type == PM_DEAD ) {
			continue;
		}
		// zinx - limbo'd players are not PM_DEAD or STAT_HEALTH <= 0.
		// and we certainly don't want to lock to them
		// fix for bug #345
		if ( cl->ps.pm_flags & PMF_LIMBO ) {
			continue;
		}
		if ( cl->ps.stats[ STAT_HEALTH ] <= 0 ) {
			continue;
		}
		// tjw: ps.stats updated before spawn?
		//if( cl->ps.stats[ STAT_PLAYER_CLASS ] != PC_MEDIC ) {
		if ( cl->sess.playerType != PC_MEDIC ) {
			continue;
		}
		VectorCopy( g_entities[level.sortedClients[i]].s.pos.trBase, end );
		end[2] += cl->ps.viewheight;

		trap_Trace( &tr, start, NULL, NULL, end, self->s.number, CONTENTS_SOLID );
		if ( tr.fraction < 0.95 ) {
			continue;
		}

		VectorSubtract( end, start, end );
		dist = VectorNormalize( end );

		if ( dist < bestdist ) {
			medic = cl->ps.clientNum;
			vectoangles( end, temp );
			self->client->ps.stats[STAT_DEAD_YAW] = temp[YAW];
			bestdist = dist;
		}
	}
	if ( medic >= 0 ) {
		//self->client->ps.viewlocked_entNum = medic;
		//self->client->ps.viewlocked = 7;
		if ( level.clients[medic].ps.weapon == 26 ) {     // more than likley going to revive now..
			return qtrue;
		}
	}
	return qfalse;
}

/*=========================
ETPUB Binoc War
//=======================*/
int QDECL G_SortPlayersByBinocs( const void* a, const void* b ) {
	gclient_t* cla = &level.clients[ *( (int*)a ) ];
	gclient_t* clb = &level.clients[ *( (int*)b ) ];
	int aBinocs, bBinocs;

	aBinocs = cla->pers.numBinocs;
	bBinocs = clb->pers.numBinocs;

	if ( aBinocs > bBinocs ) {
		return -1;
	}
	if ( bBinocs > aBinocs ) {
		return 1;
	}
	return 0;
}
/*=========================
ETPUB Binoc War Part 2
//=======================*/
void G_BinocMasters() {
	int binocMasters[MAX_CLIENTS];
	int i, countConn = 0;
	//char msg[MAX_STRING_CHARS];
	gclient_t *cl;

	if ( g_goldrush.integer ) {
		return;
	}

	if ( g_sniperWar.integer ) {
		return;
	}

	level.LastBinocWarTime = level.time;

	for ( i = 0; i < level.numConnectedClients; i++ ) {
		cl = level.clients + level.sortedClients[i];
		if ( cl->sess.sessionTeam != TEAM_RED && cl->sess.sessionTeam != TEAM_BLUE ) {
			continue;
		}
		binocMasters[countConn++] = level.sortedClients[i];
	}

	qsort( binocMasters, countConn, sizeof( int ), G_SortPlayersByBinocs );

	//msg[0] = '\0';
	if ( countConn > 0 ) {
		cl = level.clients + binocMasters[0];
		if ( cl->pers.numBinocs > 0 ) {
			trap_SendServerCommand( -1, va( "chat \"^3*** ^4Binocular War Stats:\"" ) );
			trap_SendServerCommand( -1, va( "chat \"^3***  ^41. ^7%s ^4(^7%i^4)\"", cl->pers.netname, cl->pers.numBinocs ) );
		}
	}
	if ( countConn > 1 ) {
		cl = level.clients + binocMasters[1];
		if ( cl->pers.numBinocs > 0 ) {
			trap_SendServerCommand( -1, va( "chat \"^3***  ^42. ^7%s ^4(^7%i^4)\"", cl->pers.netname, cl->pers.numBinocs ) );
		}
	}
	if ( countConn > 2 ) {
		cl = level.clients + binocMasters[2];
		if ( cl->pers.numBinocs > 0 ) {
			trap_SendServerCommand( -1, va( "chat \"^3***  ^43. ^7%s ^4(^7%i^4)\"", cl->pers.netname, cl->pers.numBinocs ) );
		}
	}
	/*if(msg[0]) {
	    if (!endOfMatch)
	        AP(va("cpm \"%s\n\"",msg));
	    else
	        AP(va("chat \"%s\" -1",msg));
	}*/

	//return;
}
/*=========================
updateUptime
Sets a cvar in serverinfo letting users know how long its been up..
mostly useful for admins
//=======================*/
void updateUptime() {
	//if(!g_uptime.integer)
	//	return;
	int secs = level.time / 1000;
	int mins = ( secs / 60 ) % 60;
	int hours = ( secs / 3600 ) % 24;
	int days = ( secs / ( 3600 * 24 ) );

	//string uptime = ("%d D, %d H, %d M", days
/*	SPC( va("%s ^/uptime: ^2%d ^7day%s ^2%d ^7hours ^2%d ^7minutes",
        sv_hostname.string, days,
        (days != 1 ? "s" : ""),
        hours, mins) );*/
	trap_Cvar_Set( "Uptime", va( "%i Days, %i Hours, %i Minutes", days, hours, mins ) );
	//return qtrue;
}
/*=========================
Same-Team timer
//=======================*/
void SpawnTimer( gentity_t* ent ) {
	int testtime;

	if ( g_spawnTimer.integer == 1 ) {
		if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
			testtime = ( level.time - ent->client->pers.spawnTimerStartTime ) % g_redlimbotime.integer;
			if ( g_redlimbotime.integer - testtime == g_redlimbotime.integer ) {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				trap_SendServerCommand( ent - g_entities, va( "chat \"(%s^7) (%s^7): ^1AXIS ^3SPAWN!\"", ent->client->pers.netname, location ) );
			} else if ( g_redlimbotime.integer - testtime == 5000 )   {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				trap_SendServerCommand( ent - g_entities, va( "chat \"(%s^7) (%s^7): ^1Axis ^7spawn in 5 seconds\"", ent->client->pers.netname, location ) );
			} else if ( g_redlimbotime.integer - testtime == 3000 )   {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				trap_SendServerCommand( ent - g_entities, va( "chat \"(%s^7) (%s^7): ^1Axis ^7spawn in 3 seconds\"", ent->client->pers.netname, location ) );
			}
		} else if ( ent->client->sess.sessionTeam == TEAM_RED )    {
			testtime = ( level.time - ent->client->pers.spawnTimerStartTime ) % g_bluelimbotime.integer;
			if ( g_bluelimbotime.integer - testtime == g_bluelimbotime.integer ) {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				trap_SendServerCommand( ent - g_entities, va( "chat \"(%s^7) (%s^7): ^4ALLIES ^3SPAWN!\"", ent->client->pers.netname, location ) );
			} else if ( g_bluelimbotime.integer - testtime == 5000 )   {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				trap_SendServerCommand( ent - g_entities, va( "chat \"(%s^7) (%s^7): ^4Allies ^7spawn in 5 seconds\"", ent->client->pers.netname, location ) );
			} else if ( g_bluelimbotime.integer - testtime == 3000 )   {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				trap_SendServerCommand( ent - g_entities, va( "chat \"(%s^7) (%s^7): ^4Allies ^7spawn in 3 seconds\"", ent->client->pers.netname, location ) );
			}
		}
	} else if ( g_spawnTimer.integer == 2 )    {
		if ( ent->client->sess.sessionTeam == TEAM_BLUE ) {
			testtime = ( level.time - ent->client->pers.spawnTimerStartTime ) % g_redlimbotime.integer;
			if ( g_redlimbotime.integer - testtime == g_redlimbotime.integer ) {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				G_TeamCommand( TEAM_BLUE, va( "chat \"(%s^7) (%s^7): ^1AXIS ^3SPAWN!\"", ent->client->pers.netname, location ) );
			} else if ( g_redlimbotime.integer - testtime == 5000 )   {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				G_TeamCommand( TEAM_BLUE, va( "chat \"(%s^7) (%s^7): ^1Axis ^7spawn in 5 seconds\"", ent->client->pers.netname, location ) );
			} else if ( g_redlimbotime.integer - testtime == 3000 )   {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				G_TeamCommand( TEAM_BLUE, va( "chat \"(%s^7) (%s^7): ^1Axis ^7spawn in 3 seconds\"", ent->client->pers.netname, location ) );
			}
		} else if ( ent->client->sess.sessionTeam == TEAM_RED )    {
			testtime = ( level.time - ent->client->pers.spawnTimerStartTime ) % g_bluelimbotime.integer;
			if ( g_bluelimbotime.integer - testtime == g_bluelimbotime.integer ) {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				G_TeamCommand( TEAM_RED, va( "chat \"(%s^7) (%s^7): ^4ALLIES ^3SPAWN!\"", ent->client->pers.netname, location ) );
			} else if ( g_bluelimbotime.integer - testtime == 5000 )   {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				G_TeamCommand( TEAM_RED, va( "chat \"(%s^7) (%s^7): ^4Allies ^7spawn in 5 seconds\"", ent->client->pers.netname, location ) );
			} else if ( g_bluelimbotime.integer - testtime == 3000 )   {
				char location[64];
				Team_GetLocationMsg( ent, location, sizeof( location ) );
				G_TeamCommand( TEAM_RED, va( "chat \"(%s^7) (%s^7): ^4Allies ^7spawn in 3 seconds\"", ent->client->pers.netname, location ) );
			}
		}
	}
	ent->client->pers.lastSpawnTimerCheck = level.time;
}
/*================
setSeedPings
Sets random number for a bot's ping.
The seed will give it an initial 'ping' and then will fluxuatee +/-  from it (hopefully :D)
================*/
void setSeedPings( gentity_t* ent ) {
	ent->client->pers.seedPing = ( ( rand() % 70 ) + 30 );  // 30 to 100 for pings

}

/*=========================
Auto Even teams + Heavy weapons
//=======================*/
void AutoWeaponsConfig( gentity_t* ent ) {
	int i;
	int RedPlayers = 0;
	int BluePlayers = 0;

	//Call this out here, cause if we got this far we are doing one or the other, no need to do it twice... (morons :D)
	for ( i = 0 ; i < level.maxclients ; i++ ) {     //for(i=0; i<MAX_CLIENTS; i++){		//AGent - Whats the diff?? :S
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_BLUE ) {
			BluePlayers++;
		}
		if ( level.clients[i].sess.sessionTeam == TEAM_RED ) {
			RedPlayers++;
		}
	}

	if ( g_AutoHeavyWeapons.integer ) {
		//Panz Loopies
		if ( ( RedPlayers || BluePlayers ) > 6 ) {
			trap_Cvar_Set( "g_maxPf", "1" );
		} else if ( ( RedPlayers || BluePlayers ) > 16 ) {
			trap_Cvar_Set( "g_maxPf", "2" );
		} else if ( ( RedPlayers || BluePlayers ) > 24 ) {
			trap_Cvar_Set( "g_maxPf", "3" );
		} else if ( ( RedPlayers || BluePlayers ) > 30 ) {
			trap_Cvar_Set( "g_maxPf", "4" );
		} else { //
			trap_Cvar_Set( "g_maxPf", "0" );
		}

		//Flame Loopies [Yea i know you hate em, ...]	We should fix what happens when a flamer gets kicked && attacking lol
		//												Maybe check to see if the attacker is con_connected, otherwise do 0 dmg
		if ( ( RedPlayers || BluePlayers ) > 10 ) {
			trap_Cvar_Set( "g_maxFlamer", "1" );
		} else if ( ( RedPlayers || BluePlayers ) > 20 ) {
			trap_Cvar_Set( "g_maxFlamer", "2" );
		} else if ( ( RedPlayers || BluePlayers ) > 30 ) {
			trap_Cvar_Set( "g_maxFlamer", "3" );
		} else {
			trap_Cvar_Set( "g_maxFlamer", "0" );
		}
	}
/*
    if (g_AutoTeams.integer){
        if ((RedPlayers - BluePlayers) >= 2){	//They are 2 different :(
                for ( i = 0 ; i < level.maxclients ; i++ ){
                    if (level.clients[i].pers.connected != CON_CONNECTED)
                        continue;
                    if (level.clients[i].sess.sessionTeam == TEAM_RED){						//We found someone who can be switched
//						trap_SendConsoleCommand(EXEC_APPEND, va("forceteam %i b\n", level.clients[i]));
                        trap_SendServerCommand(-1, va("print \"Server forced %s ^7to Allies to balance teams\n\"", level.clients[i].pers.netname));
                        return;		//Does this end the loop???? (yes im retarted)
                    }
                }
        }
        if ((BluePlayers - RedPlayers) >= 2){
                for ( i = 0 ; i < level.maxclients ; i++ ){
                    if (level.clients[i].pers.connected != CON_CONNECTED)
                        continue;
                    if (level.clients[i].sess.sessionTeam == TEAM_BLUE){
//						trap_SendConsoleCommand(EXEC_APPEND, va("forceteam %i r\n", level.clients[i]));
                        trap_SendServerCommand(-1, va("print \"Server forced %s ^7to Axis to balance teams\n\"", level.clients[i].pers.netname));
                        return;
                    }
                }
        }*/
}

//END==============
void Checkcvars( gentity_t *ent,char *cvar, int cvaris ) {
	char * checkcvar;
	char * cvarto;
	char * cvarfrom;
	char * buffer;
	FILE * cvars;
	qboolean badcvars = qfalse;
	cvars = fopen( "cvars.txt","r" );

	checkcvar = "";
	cvarto = "";
	cvarfrom = "";
	buffer = "";

	if ( cvars ) {
		while ( fgets( buffer,1024,cvars ) != NULL ) {
			stripcvars( buffer, checkcvar,cvarfrom, cvarto );
			if ( !strcmp( checkcvar,cvar ) ) {

				if ( atoi( va( "%s",cvaris ) ) < atoi( va( "%s",cvarfrom ) ) || atoi( va( "%s",cvaris ) ) > atoi( va( "%s",cvarto ) ) ) {
					trap_SendServerCommand( ent - g_entities, va( "print \" ^1WARNING^7: ^3%s must be between or equal to %s and %s, your setting is %i\n",cvar, cvarfrom, cvarto, cvaris ) );
					badcvars = qtrue;
				}

			}
		}
		fclose( cvars );
	}

	if ( badcvars ) {
		trap_DropClient( ent - g_entities, "cvar violations: check your console for more info." );
	}

}


int muzzleflashmodel;
void mg42_use( gentity_t * ent, gentity_t * other, gentity_t * activator ) ;
void mg42_die( gentity_t * self, gentity_t * inflictor, gentity_t * attacker, int damage, int mod ) ;
void mg42_think( gentity_t * self ) ;
void Flak_Animate( gentity_t * ent ) ;
void mg42_track( gentity_t * self, gentity_t * other ) ;
void mg42_touch( gentity_t * self, gentity_t * other, trace_t * trace ) ;

qboolean MyMG42Visible( gentity_t *self, gentity_t *other ) {
	vec3_t selfView, otherView;     //basically stores the locations of the players' eyes
	trace_t tr;                     //used in performing the collision info
	gentity_t   *traceEnt;          //holds the entity returned by the trace function

	VectorCopy( self->r.currentOrigin, selfView );        //copy the bot's origin to this variable
	VectorCopy( other->r.currentOrigin, otherView );      //copy the target's origin
	otherView[2] += other->client->ps.viewheight;       //add the target's viewheight

	//check if a shot from the bot's viewheight to the player's viewheight would hit
	trap_Trace( &tr, selfView, NULL, NULL, otherView, self->s.number, MASK_SHOT );

	traceEnt = &g_entities[tr.entityNum];       //set traceEnt to the entity the shot would hit

	if ( traceEnt == other ) {    //if it would hit the player, return true
		return qtrue;
	}

	return qfalse;              //it would hit something other than the player (or nothing) so return false

}

/*
==========
FindEnemy
==========
*/
qboolean MG42FindEnemy( gentity_t *ent ) {
	int i;
	gentity_t       *targ;
	gentity_t       *bestenemy = NULL;
	float bestweight = -1;
	float weight;
	vec3_t dist;
	int viewDist;
//	int				reactionTime;


	viewDist = 99999;


	//loop through all the clients to see if the MG42 can see any enemies
	for ( i = 0; i < level.maxclients; i++ )
	{
		targ = &g_entities[i];

		//make sure the client is a valid player (and in use)
		if ( !targ->inuse || !targ->client ) {
			continue;
		}

		//make sure the player isn't the bot or anyone on his team
		if ( targ->client->sess.sessionTeam == ent->parent->client->sess.sessionTeam ) {
			continue;
		}

		//make sure the player isn't dead or spectating
		if ( ( ( targ->health <= 0 ) || ( targ->client->ps.pm_flags & PMF_LIMBO ) ) || targ->client->sess.sessionTeam == TEAM_SPECTATOR ) { //not dead or spectating...
			continue;
		}

		// Check if the bot can see the enemy from this position
		if ( MyMG42Visible( ent, targ ) ) {
			// Find the distance between bot and enemy
			VectorSubtract( ent->r.currentOrigin, targ->r.currentOrigin, dist );

			//store it so we can compare
			weight = VectorLength( dist );

			//if this enemy is too far away to be seen, skip him
			if ( weight > viewDist ) {
				continue;
			}

			//if the new distance is the shortest we have found so far
			if ( ( weight < bestweight ) || ( bestweight == -1 ) ) {
				//set the new shortest distance and the new 'best' enemy
				bestweight = weight;
				bestenemy = targ;   //this loop (and this line) set 'bestenemy' to the closest visible enemy
			}
		}
	}

	//if the bot found an enemy
	if ( bestenemy ) {
		ent->enemy = bestenemy;     //set the enemy to best ememy
		return qtrue;               //return yes, he can see an enemy
	}

	return qfalse;      //no, the bot did not find any enemies
}


qboolean DoMG42Aim( gentity_t *ent ) {
	vec3_t vec, enemyvec,angles;


	if ( !MG42FindEnemy( ent ) ) { //check for an enemy :D
		return qfalse;
	}

	//Get Enemy Posistion
	VectorCopy( ent->enemy->r.currentOrigin, enemyvec );

	//if enemy is crouched aim lower
	if ( ent->enemy->client->ps.pm_flags & PMF_DUCKED ) {
		enemyvec[2] -= ent->enemy->client->ps.crouchViewHeight;
	}

	//find the direction to the enemy
	VectorSubtract( enemyvec, ent->r.currentOrigin, vec );
	//turn to face the enemy- and own him :)
	vectoangles( vec, angles );
	G_SetAngle( ent, angles );
	return qtrue;
}

static vec3_t forward, right, up;
static vec3_t muzzle;

/*
==============
Fire_Lead
==============
*/
void mg42_muzzleflash( gentity_t *ent );
//----(SA)	added 'activator' so the bits that used to expect 'ent' to be the gun still work
void AUTOMG42_Fire_Lead( gentity_t *ent, gentity_t *activator, float spread, int damage ) {
	trace_t tr;
	vec3_t end;
	float r;
	float u;
	gentity_t       *tent;
	gentity_t       *traceEnt;
	//qboolean	isflak = qfalse;

	r = crandom() * spread;
	u = crandom() * spread;

	ent->s.eFlags |= EF_MG42_ACTIVE;
	//activator->s.eFlags |= EF_MG42_ACTIVE;

	VectorMA( muzzle, 8192, forward, end );
	VectorMA( end, r, right, end );
	VectorMA( end, u, up, end );

	trap_Trace( &tr, muzzle, NULL, NULL, end, ent->s.number, MASK_SHOT );


	if ( tr.surfaceFlags & SURF_NOIMPACT ) {
		mg42_muzzleflash( ent );
		G_AddEvent( ent, EV_FIRE_WEAPON_MG42, 0 );

// JPW NERVE added this event so tracers work if you're shooting mg42 into skybox, otherwise shouldn't ever see the event double-up
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_WALL );
		tent->s.otherEntityNum = ent->s.number;
		tent->s.otherEntityNum2 = ent->s.number;
// jpw

		return;
	}

	traceEnt = &g_entities[ tr.entityNum ];

	// snap the endpos to integers, but nudged towards the line
	SnapVectorTowards( tr.endpos, muzzle );

	// send bullet impact
	if ( traceEnt->takedamage && traceEnt->client ) {
		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_FLESH );
		tent->s.eventParm = traceEnt->s.number;
		tent->s.otherEntityNum = ent->s.number;

		if ( LogAccuracyHit( traceEnt, ent ) ) {
			ent->client->ps.persistant[PERS_ACCURACY_HITS]++;
		}
	} else {
		// Ridah, bullet impact should reflect off surface
		vec3_t reflect;
		float dot;

		tent = G_TempEntity( tr.endpos, EV_BULLET_HIT_WALL );

		dot = DotProduct( forward, tr.plane.normal );
		VectorMA( forward, -2 * dot, tr.plane.normal, reflect );
		VectorNormalize( reflect );

		tent->s.eventParm = DirToByte( reflect );
		tent->s.otherEntityNum = ent->s.number;
		tent->s.otherEntityNum2 = ent->s.number;    // (SA) store the user id, so the client can position the tracer
	}

	if ( traceEnt->takedamage ) {
		G_Damage( traceEnt, ent, activator, forward, tr.endpos,
				  damage, 0, MOD_MACHINEGUN );
	}

	mg42_muzzleflash( ent );
	G_AddEvent( ent, EV_FIRE_WEAPON_MG42, 0 );
}



void auto_mg42_think( gentity_t * self ) {
//vec3_t		forward, right, up, muzzle;
	int spread, firerate;
	vec3_t newviewangles;
	spread = 150;
	firerate = 150;

	self->nextthink = level.time + 50;
	self->think = auto_mg42_think;


	if ( self->s.frame != 0 ) {
		return;
	}

	if ( g_entities[self->mg42BaseEnt].health <= 200 ) { //once mg42 drops below 100 health lets make it show some stress
		self->s.eFlags |= EF_SMOKING; // emit black smoke
		spread = 200; //make the spread get worse
		firerate = 200; //slow this down
	}

	if ( g_entities[self->mg42BaseEnt].health <= 100 ) { //once mg42 drops below 75 health lets make it show some stress
		self->s.eFlags &= ~EF_SMOKING;
		self->s.eFlags |= EF_SMOKINGBLACK; // emit thick black smoke
		spread = 300; //make the spread get worse
		firerate = 250; ////slow this down
	}


	if ( DoMG42Aim( self ) ) {

		if ( ( level.time - self->last_move_time ) >= firerate ) {
			self->last_move_time = level.time;

			AngleVectors( self->s.apos.trBase, forward, right, up );
			VectorCopy( self->s.pos.trBase, muzzle );

			VectorMA( muzzle, 16, up, muzzle );

			self->s.eFlags |= EF_MG42_ACTIVE;

			// snap to integer coordinates for more efficient network bandwidth usage
			SnapVector( muzzle );

			AUTOMG42_Fire_Lead( self, self->parent, spread, 18 );
		}
		self->count = level.time;
	} else { //no target so... lets just do somethin for the hell of it

		if ( level.time - self->count > 3000 || self->count == 0 ) {
			VectorCopy( self->r.currentAngles,newviewangles );

			//this will stop it from being so damn choppy.

			if ( level.time - self->count2 > 10000 ) {
				if ( self->count3 == 1 ) {
					self->count3 = 0;
				} else {
					self->count3 = 1;
				}

				self->count2 = level.time;
			}

			if ( self->count3 == 1 ) {
				newviewangles[YAW] -= 2; //lets spin it around :D
			} else {
				newviewangles[YAW] += 2;
			}

			if ( self->r.currentAngles[PITCH] != 0 && self->r.currentAngles[PITCH] != -360 ) {
				//newviewangles[PITCH] = g_test1.integer;
				if ( self->r.currentAngles[PITCH] >= -90 && self->r.currentAngles[PITCH] <= 0 ) {
					newviewangles[PITCH] += 1; //make gun move down
				} else if ( self->r.currentAngles[PITCH] <= -270 && self->r.currentAngles[PITCH] >= -360 )                                    {
					newviewangles[PITCH] -= 1; //make gun move up
				}
			}

			G_SetAngle( self,newviewangles );
			self->count = 0;

		}
	}
}

void propExplosionLarge( gentity_t *ent );
void auto_mg42_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod ) {
	gentity_t   *gun;
	//play a brizake sound
	G_Sound( self, G_SoundIndex( "sound/weapons/mg42/mg42_death.wav" ) );
	//tell the world it got brizoke
	gun = self->chain;
	if ( attacker->client && ( gun->parent->client != attacker->client ) ) {
		trap_SendServerCommand( -1, va( "print \"%s ^7disabled %s^7's mobile mg42\n",
										attacker->client->pers.netname, gun->parent->client->pers.netname ) );
	} else {
		trap_SendServerCommand( -1, va( "print \"%s^7's mobile mg42 was disabled\n",
										gun->parent->client->pers.netname ) );
	}

	if ( gun->parent->client->sess.sessionTeam == TEAM_BLUE ){
		level.alliedmg42s++;
	}
	else
	{
		level.axismg42s--;
	}
	self->freeAfterEvent = qtrue;
	self->flags |= FL_NODRAW;
	self->r.svFlags |= SVF_NOCLIENT;
	self->s.eFlags |= EF_NODRAW;
	self->r.contents = 0;
	self->nextthink = 0;
	self->think = 0;
	gun->freeAfterEvent = qtrue;
	gun->flags |= FL_NODRAW;
	gun->r.svFlags |= SVF_NOCLIENT;
	gun->s.eFlags |= EF_NODRAW;
	gun->r.contents = 0;
	gun->nextthink = 0;
	gun->think = 0;



}

void auto_mg42_spawn( gentity_t *ent ) {
	gentity_t *base, *gun;
	vec3_t offset;


	base = G_Spawn();
	base->clipmask = CONTENTS_SOLID;
	base->r.contents = CONTENTS_SOLID;
	base->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	base->s.eType = ET_GENERAL;
	base->takedamage = qfalse;
	base->die = auto_mg42_die;

	base->s.modelindex = G_ModelIndex( "models/mapobjects/weapons/mg42b.md3" );


	VectorSet( base->r.mins, -8, -8, -8 );
	VectorSet( base->r.maxs, 8, 8, 48 );
	VectorCopy( ent->r.currentOrigin, offset );
	offset[2] -= 24;
	offset[2] += 20;
	G_SetOrigin( base, offset );
	base->s.apos.trType = TR_STATIONARY;
	base->s.apos.trTime = 0;
	base->s.apos.trDuration = 0;
	base->s.dmgFlags = HINT_MG42;       // identify this for cursorhints
	VectorCopy( ent->r.currentAngles, base->s.angles );
	VectorCopy( base->s.angles, base->s.apos.trBase );
	VectorCopy( base->s.angles, base->s.apos.trDelta );
	base->health = ent->health;
	base->target = ent->target;     //----(SA)	added so mounting mg42 can trigger targets
	trap_LinkEntity( base );


	gun = G_Spawn();
	gun->classname = "automg42";
	gun->clipmask = CONTENTS_SOLID;
	gun->r.contents = CONTENTS_TRIGGER;
	gun->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	gun->s.eType = ET_GENERAL;
	gun->health = base->health; // JPW NERVE

	// DHM - Don't need to specify here, handled in G_CheckForCursorHints
	//gun->s.dmgFlags = HINT_MG42;	// identify this for cursorhints

	gun->touch = 0;
	gun->last_move_time = 0;


	// DHM - Nerve :: Use a different model in multiplayer
	if ( g_gametype.integer >= GT_WOLF ) {
		gun->s.modelindex = G_ModelIndex( "models/multiplayer/mg42/mg42.md3" );
	} else {
		gun->s.modelindex = G_ModelIndex( "models/mapobjects/weapons/mg42a.md3" );
	}

	VectorCopy( ent->r.currentOrigin, offset );
	offset[2] += 24;
	offset[2] += 20;
	G_SetOrigin( gun, offset );
	VectorSet( gun->r.mins, -24, -24, -8 );
	VectorSet( gun->r.maxs, 24, 24, 48 );
	gun->s.apos.trTime = 0;
	gun->s.apos.trDuration = 0;
	VectorCopy( ent->r.currentAngles, gun->s.angles );
	VectorCopy( gun->s.angles, gun->s.apos.trBase );
	VectorCopy( gun->s.angles, gun->s.apos.trDelta );

	VectorCopy( ent->s.angles, gun->s.angles2 );

	gun->think = 0;
	gun->nextthink = 0;
	gun->timestamp = 0;
	gun->s.number = gun - g_entities;
	gun->harc = ent->harc;
	gun->varc = ent->varc;
	gun->s.apos.trType = TR_LINEAR_STOP;    // interpolate the angles
	gun->takedamage = qfalse;
	gun->targetname = ent->targetname;      // need this for scripting
	gun->damage = ent->damage;
	gun->accuracy = ent->accuracy;
	gun->target = ent->target;  //----(SA)	added so mounting mg42 can trigger targets
	gun->use = 0;
	//gun->die = auto_mg42_die; // JPW NERVE we want it to be called for non-tripod machineguns too (for mp_beach etc)
	gun->parent = ent->parent;
	gun->s.frame = 2;

	gun->mg42BaseEnt = base->s.number;


	gun->spawnflags = ent->spawnflags;

	trap_LinkEntity( gun );

	base->chain = gun;

	G_FreeEntity( ent );


	muzzleflashmodel = G_ModelIndex( "models/weapons2/machinegun/mg42_flash.md3" );
	//auto_mg42_die(ent);


}

//Model of what i think stats should be like
/*
===============================================================================
                                AXIS TEAM
===============================================================================
| CN NAME     |   K   D    KR Tk |   G   R   H   A |    DG    DR |  HS    ACC |
-------------------------------------------------------------------------------
| 10 namename | 100 100 23.00  0 | 100 100 100 100 | 10000 10000 | 100 100.00 |
-------------------------------------------------------------------------------

===============================================================================
                               ALLIED TEAM
===============================================================================
| CN NAME     |   K   D    KR Tk |   G   R   H   A |    DG    DR |  HS    ACC |
-------------------------------------------------------------------------------
| 10 namename | 100 100 23.00  0 | 100 100 100 100 | 10000 10000 | 100 100.00 |
-------------------------------------------------------------------------------
*/

/*
===========
printstats
===========
*/
void printstats() {
	int i;
	int j;

	trap_SendServerCommand( -1, "print \"^5Stats key:\n" );
	trap_SendServerCommand( -1, "print \"^3CN  = Client Number    K = Kills         D  = Deaths\n" );
	trap_SendServerCommand( -1, "print \"^3Tk  = Teamkills        G = Gibs          A  = Ammo given\n" );
	trap_SendServerCommand( -1, "print \"^3DR  = Damage received  R = Revives       KR = Kill Ratio\n" );
	trap_SendServerCommand( -1, "print \"^3DG  = Damage given     H = Health given  HS = Head shots\n" );
	trap_SendServerCommand( -1, "print \"^3ACC = Accuracy\n" );
	trap_SendServerCommand( -1, "print \"\n" );
	trap_SendServerCommand( -1, "print \"^3-----------------------------------------------------------------------------\n" );
	trap_SendServerCommand( -1, "print \"^3|                                 ^1AXIS TEAM                                 ^3|\n" );
	trap_SendServerCommand( -1, "print \"^3-----------------------------------------------------------------------------\n" );
	trap_SendServerCommand( -1, "print \"^3CN NAME     |   K   D    KR Tk |   G   R   H   A |    DG    DR |  HS    ACC |\n" );
	trap_SendServerCommand( -1, "print \"^3-----------------------------------------------------------------------------\n" );
	//go through all clients
	for ( i = 0; i < g_maxclients.integer; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}

		for ( j = 0; j < g_maxclients.integer; j++ ) { //go through all clients again lol
			if ( level.clients[j].pers.connected != CON_CONNECTED ) {
				continue;
			}

			if ( level.clients[j].sess.sessionTeam != TEAM_RED ) {
				continue;
			}

			if ( j == i ) {
				trap_SendServerCommand( i, va( "print \"%s",level.clients[j].pers.ownendroundstats ) );
			} else {
				trap_SendServerCommand( i, va( "print \"%s",level.clients[j].pers.endroundstats ) );
			}
		}
	}
	trap_SendServerCommand( -1, "print \"^3-----------------------------------------------------------------------------\n" );
	trap_SendServerCommand( -1, "print \"\n" );
	trap_SendServerCommand( -1, "print \"^3-----------------------------------------------------------------------------\n" );
	trap_SendServerCommand( -1, "print \"^3|                               ^4ALLIED TEAM                                 ^3|\n" );
	trap_SendServerCommand( -1, "print \"^3-----------------------------------------------------------------------------\n" );
	trap_SendServerCommand( -1, "print \"^3CN NAME     |   K   D    KR Tk |   G   R   H   A |    DG    DR |  HS    ACC |\n" );
	trap_SendServerCommand( -1, "print \"^3-----------------------------------------------------------------------------\n" );
	//go through all clients
	for ( i = 0; i < g_maxclients.integer; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED ) {
			continue;
		}

		for ( j = 0; j < g_maxclients.integer; j++ ) { //go through all clients again lol
			if ( level.clients[j].pers.connected != CON_CONNECTED ) {
				continue;
			}

			if ( level.clients[j].sess.sessionTeam != TEAM_BLUE ) {
				continue;
			}

			if ( j == i ) {
				trap_SendServerCommand( i, va( "print \"%s",level.clients[j].pers.ownendroundstats ) );
			} else {
				trap_SendServerCommand( i, va( "print \"%s",level.clients[j].pers.endroundstats ) );
			}
		}
	}
	trap_SendServerCommand( -1, "print \"^3-----------------------------------------------------------------------------\n" );
// removed this stupid line :p
//trap_SendServerCommand(-1, "print \"^5Need help understanding? Scroll up to the key at the top\n");
}

void parseleftovercolorthings( char name[32] ) {
	int i = 0;
	int j = 0;
	char afterparse[32];

	afterparse[0] = '\0';

	for ( i = 0 ; i < 32; i++ )
	{
		if ( name[i] == '^' ) {
			continue;
		}
		if ( name[i] == '\0' ) {
			break;
		}

		afterparse[j] = name[i];
		j++;
	}

	strcpy( name,afterparse );
}


/*
===========
BuildStats
===========
*/

//Builds each clients line for the stats

void BuildStats() {

	int i, j, cnt;
	int tKill, tDeaths, tKillratio, tTk, tGibs, tRevies, tHealth, tAmmo, tDG, tDR, tHS, tHits,tShots;
	gclient_t *cl;
	float KillRatio = 0.00f;
	float acc       = 0.00f;
	float avgKR     = 0.00f;
	float avgAcc    = 0.00f;

	char name[MAX_STRING_CHARS];
	char*col;

	for ( i = TEAM_RED; i <= TEAM_BLUE; i++ ) {

		if ( i == TEAM_RED ) {
			trap_SendServerCommand( -1, "print \"                                  ^1AXIS TEAM                                  \n^3-----------------------------------------------------------------------------\n^3    NAME    |   K   D    KR Tk |   G   R   H   A |    DG    DR |  HS    ACC |\n^3-----------------------------------------------------------------------------\n" );
		} else {
			trap_SendServerCommand( -1, "print \"                                 ^4ALLIED TEAM                                 \n^3-----------------------------------------------------------------------------\n^3    NAME    |   K   D    KR Tk |   G   R   H   A |    DG    DR |  HS    ACC |\n^3-----------------------------------------------------------------------------\n" );
		}

		tKill = 0;
		tDeaths = 0;
		tKillratio = 0;
		tTk = 0;
		tGibs = 0;
		tRevies = 0;
		tHealth = 0;
		tAmmo = 0;
		tDG = 0;
		tDR = 0;
		tHS = 0;
		tShots = 0;
		tHits = 0;
		cnt = 0;


		for ( j = 0; j < level.numPlayingClients; j++ ) {
			cl = level.clients + level.sortedClients[j];

			if ( cl->pers.connected != CON_CONNECTED || cl->sess.sessionTeam != i ) {
				continue;
			}

			CleanseString( cl->pers.netname, name, qfalse );
			name[11] = 0;

			//keep track of totals!
			tKill += cl->pers.kills;
			tDeaths += cl->pers.deaths;
			tTk += cl->pers.teamKills;
			tGibs += cl->pers.teamGibs;
			tRevies += cl->pers.revives;
			tHealth += cl->pers.medPacks;
			tAmmo += cl->pers.ammoPacks;
			tDG += cl->pers.dmgGiven;
			tDR += cl->pers.dmgreceived;
			tHS += cl->pers.headshots;
			tHits += cl->pers.acc_hits;
			tShots += cl->pers.acc_shots;





			if ( cl->pers.acc_shots > 0 ) {
				acc = ( (float)cl->pers.acc_hits / (float)cl->pers.acc_shots ) * 100.00f;
			}


			if ( cl->pers.deaths < 1 ) {
				KillRatio = (float)cl->pers.kills / (float)1;
			} else {
				KillRatio = (float)cl->pers.kills / (float)cl->pers.deaths;
			}



			trap_SendServerCommand( cl - level.clients, "print \"^5-----------------------------------------------------------------------------\n" );

			trap_SendServerCommand( -1, va( "print \"^7%-11s ^3|^7 %3d %3d %5.2f %2d ^3|^7 %3d %3d %3d %3d ^3|^7 %5d %5d ^3|^7 %3d %6.2f ^3|\n\"",
											name,
											cl->pers.kills,
											cl->pers.deaths,
											KillRatio,
											cl->pers.teamKills,
											cl->pers.gibs,
											cl->pers.revives,
											cl->pers.medPacks,
											cl->pers.ammoPacks,
											cl->pers.dmgGiven,
											cl->pers.dmgreceived,
											cl->pers.headshots,
											acc ) );

			trap_SendServerCommand( cl - level.clients, "print \"^5-----------------------------------------------------------------------------\n" );

			//count up to see if we actually printed stats for players
			cnt++;
		} //end of individual stats


		if ( cnt ) { //check if we actually had stats for this team
			if ( tShots > 0 ) {
				acc = ( (float)tHits / (float)tShots ) * 100.00f;
			}

			if ( tShots > 0 ) {
				avgAcc = ( (float)( tHits / cnt ) / (float)( tShots / cnt ) ) * 100.00f;
			}


			if ( tDeaths < 1 ) {
				KillRatio = (float)tKill / (float)1;
			} else {
				KillRatio = (float)tKill / (float)tDeaths;
			}

			if ( tDeaths < 1 ) {
				avgKR = ( (float)( tKill / cnt ) ) / 1;
			} else {
				avgKR = ( (float)( tKill / cnt ) ) / ( (float)( tDeaths / cnt ) );
			}

			if ( i == TEAM_RED ) {
				col = "^1";
			} else {
				col = "^4";
			}


			trap_SendServerCommand( -1, va( "print \"^3-----------------------------------------------------------------------------\n  %sTOTALS    ^3|^2 %3d %3d %5.2f %2d ^3|^2 %3d %3d %3d %3d ^3|^2 %5d %5d ^3|^2 %3d %6.2f ^3|\n\"",
											col,
											tKill,
											tDeaths,
											KillRatio,
											tTk,
											tGibs,
											tRevies,
											tHealth,
											tAmmo,
											tDG,
											tDR,
											tHS,
											acc ) );

			trap_SendServerCommand( -1, va( "print \"  %sTEAM AVG  ^3|^2 %3d %3d %5.2f %2d ^3|^2 %3d %3d %3d %3d ^3|^2 %5d %5d ^3|^2 %3d %6.2f ^3|\n^3-----------------------------------------------------------------------------\n\n\"",
											col,
											tKill / cnt,
											tDeaths / cnt,
											avgKR,
											tTk / cnt,
											tGibs / cnt,
											tRevies / cnt,
											tHealth / cnt,
											tAmmo / cnt,
											tDG / cnt,
											tDR / cnt,
											tHS / cnt,
											avgAcc ) );

		} else { // if we didnt have any stats for this team
			trap_SendServerCommand( -1, va( "print \"^3No stats to report!\n" ) );
		}

	} //end of team stats
}



void forceDLinit() {
	char *variable, *token, *text;

	if ( !strlen( g_forceDLpaknames.string ) || !strlen( g_forceDLchecksums.string ) ) {
		return;
	}

	//first check paknames
	text = g_forceDLpaknames.string;
	variable = sv_referencedPakNames.string;

	do {
		token = COM_Parse( &text );   //token will contain a single word, progressing 1 word each loop

		if ( strstr( variable,token ) == NULL ) {
			strcat( variable, va( "%s ",token ) );
		}

	} while ( token && token[0] );

	trap_Cvar_Set( "sv_referencedPakNames",  va( "%s",variable ) );

	//second check checksums
	text = g_forceDLchecksums.string;
	variable = sv_referencedPaks.string;

	do {
		token = COM_Parse( &text );   //token will contain a single word, progressing 1 word each loop

		if ( strstr( variable,token ) == NULL ) {
			strcat( variable, va( "%s ",token ) );
		}

	} while ( token && token[0] );
	trap_Cvar_Set( "sv_referencedPaks", va( "%s",variable ) );

	//S4ND_Log("Force Download Started!\n");
}
void QDECL Admin_Log( const char *fmt, ... ) {
	va_list argptr;
	char string[1024];
	int min, tens, sec;
	time_t rawtime;
	time( &rawtime );
	if ( !level.S4NDLog ) {
		return;
	}
	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;



	Com_sprintf( string, sizeof( string ), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	vsprintf( string + 7, fmt,argptr );
	va_end( argptr );

	Q_CleanStr( string );

	fputs( va( "%s\n",string ),level.S4NDLog );
	G_Printf( string );
}

void QDECL S4ND_Log( const char *fmt, ... ) {
	va_list argptr;
	char string[1024];
	int min, tens, sec;
	time_t rawtime;
	time( &rawtime );
	if ( !level.S4NDLog ) {
		return;
	}
	sec = level.time / 1000;

	min = sec / 60;
	sec -= min * 60;
	tens = sec / 10;
	sec -= tens * 10;



	Com_sprintf( string, sizeof( string ), "%3i:%i%i ", min, tens, sec );

	va_start( argptr, fmt );
	vsprintf( string + 7, fmt,argptr );
	va_end( argptr );

	Q_CleanStr( string );

	fputs( va( "%s\n",string ),level.S4NDLog );
	G_Printf( "%s\n",string );
}


void appendNode( char* data1,char* data2,char* data3,int type ) {
	struct list *node;

//allocate memory space for this
	node = (struct list *)malloc( sizeof( struct list ) );

	if ( data1 != NULL ) {
		strcpy( node->data1,data1 );
	} else {
		node->data1[0] = '\0';
	}

	if ( data2 != NULL ) {
		strcpy( node->data2,data2 );
	} else {
		node->data2[0] = '\0';
	}

	if ( data3 != NULL ) {
		strcpy( node->data3,data3 );
	} else {
		node->data3[0] = '\0';
	}

	node->type = type;

	if ( head == NULL ) {
		head = node;
		node->prev = NULL;
	} else {
		tail->next = node;
		node->prev = tail;
	}
	tail = node;
	node->next = NULL;
}


void removeNode( struct list *node ) {


	if ( node->prev == NULL ) {
		head = node->next;
	} else {
		node->prev->next = node->next;
	}

	if ( node->next == NULL ) {
		tail = node->prev;
	} else {
		node->next->prev = node->prev;
	}

	free( node );

}

void findAndRemoveNode( char*find,int list ) {
	struct list *node;
	int bansFound = 0;

	for ( node = head; node != NULL; node = node->next ) {
		if ( node->type == list ) {
			if ( strstr( node->data1,find ) ) {
				bansFound++;
				if ( list == 3 ) {
					G_Printf( "Deleted ban: %s\n",node->data1 );
				}
				if ( list == 4 ) {
					G_Printf( "Deleted bypass: %s %s\n",node->data1,node->data2 );
				}
				removeNode( node );
				if ( list == 6 ) {
					G_Printf( "Deleted tempban: %s\n",node->data1 );
				}
			}
		}
	}
	G_Printf( "Deleted %i entries!\n",bansFound );
}


//Heh, sorry people the ban list is being redone again...
//luckily I think this will allow for easier management...
//and eventually this will mean more commands for admins to
//use such as add ban/remove bans/search bans
void parseBans() {
	FILE * banfile;
	char buf[70];
	char range[70];
	char pass[70];
	banfile = fopen( "banned.txt","r" );
	if ( banfile ) {
		while ( fgets( buf,1024,banfile ) != NULL )
		{
			sscanf( buf, "%s ", range );

			if ( !strcmp( range,"bypasses" ) ) {
				break;
			}

			appendNode( range,NULL,NULL,3 ); //3 is server ban
		}

		while ( fgets( buf,1024,banfile ) != NULL )
		{
			sscanf( buf, "%s %s ", range,pass );
			appendNode( range,pass,NULL,4 ); //4 is server bypass
		}

		fclose( banfile );
	}
}

void parseMBL() {
	FILE * mbl;
	char buf[70];
	char range[70];
	char pass[70];


	mbl = fopen( "mbl.txt","r" );
	if ( mbl ) {
		while ( fgets( buf,36,mbl ) != NULL )
		{
			sscanf( buf, "%s ", range );

			if ( !strcmp( range,"bypasses" ) ) {
				break;
			}

			appendNode( range,NULL,NULL,1 ); //1 is MBL ban

		}

		while ( fgets( buf,70,mbl ) != NULL )
		{
			sscanf( buf, "%s %s ", range,pass );
			appendNode( range,pass,NULL,2 ); //2 is MBL bypass
		}

		fclose( mbl );
	}
	remove( "mbl.txt" );
}

void ParseTempBans() {
	char data[64];
	char data2[64];
	FILE * tempbanfile;
	char buf[128];
	char ip[16];

	tempbanfile = fopen( "tempbans.txt","r" );
	if ( tempbanfile ) {
		while ( fgets( buf,1024,tempbanfile ) != NULL )
		{
			sscanf( buf, "%s %s %[^\n]s",ip, data,data2 );
			appendNode( ip,data,data2,5 ); //5 is tempban!
		}
		//close file down
		fclose( tempbanfile );
	}
}

void parseBanFiles() {
	parseBans();
	ParseTempBans();
}

//print list only shows server bans and bypasses.
void printList( int list ) {
	struct list *node;

	for ( node = head; node != NULL; node = node->next ) {
		if ( node->type == 3 && list == 3 ) {
			G_Printf( "Ban: %s\n",node->data1 );
		}

		if ( node->type == 4 && list == 4 ) {
			G_Printf( "Bypass: %s %s\n",node->data1,node->data2 );
		}
	}
}

//this is called anytime an addban/bypass / removeban/bypass command is called.
void rewriteBans() {
	struct list *node;
	FILE*banfile;

	banfile = fopen( "banned.txt","w+" );

	//scan for bans first
	for ( node = head; node != NULL; node = node->next )
	{
		if ( node->type != 3 ) {
			continue;              //if it is not a ban continue

		}
		fputs( va( "%s \n",node->data1 ),banfile );
	}
	fputs( "bypasses\n",banfile );

	//Now scan for bypasses
	for ( node = head; node != NULL; node = node->next )
	{
		if ( node->type != 4 ) {
			continue;              //if it is not a bypass continue

		}
		fputs( va( "%s %s \n",node->data1, node->data2 ),banfile );
	}
	fclose( banfile );
}

void rewriteTempbans() {
	struct list *node;
	FILE*tempbanfile;

	tempbanfile = fopen( "tempbans.txt","w+" );

	//scan for bans first
	for ( node = head; node != NULL; node = node->next )
	{
		if ( node->type != 5 ) {
			continue;              //if it is not a ban continue
		}
		fputs( va( "%s %s %s \n",node->data1,node->data2,node->data3 ),tempbanfile ); //write to file
	}

	fclose( tempbanfile );
}


char* checkBanned( char*checkip,char*password ) {
	struct list *node;
	int ip[4],ipf[4],ipt[4];
	int isMBL = 0, isBanned = 0, isTempbanned = 0;
	int bypassBan = 0, bypassMBL = 0;
	time_t seconds;
	int bantill,mins,tens,second;
	time( &seconds );

	//go ahead and break down the ip address
	sscanf( checkip,"%i.%i.%i.%i",&ip[0],&ip[1],&ip[2],&ip[3] );
	for ( node = head; node != NULL; node = node->next )
	{
		switch ( node->type )
		{
		case 1:
		{
			sscanf( node->data1,"%i.%i.%i.%i-%i.%i.%i.%i", &ipf[0],&ipf[1],&ipf[2],&ipf[3],&ipt[0],&ipt[1],&ipt[2],&ipt[3] );
			if ( ip[0] >= ipf[0] && ip[0] <= ipt[0] && ip[1] >= ipf[1] && ip[1] <= ipt[1] && ip[2] >= ipf[2] && ip[2] <= ipt[2] && ip[3] >= ipf[3] && ip[3] <= ipt[3] ) {
				isMBL = 1;
			}
			break;
		}
		case 2:
		{
			sscanf( node->data1,"%i.%i.%i.%i-%i.%i.%i.%i", &ipf[0],&ipf[1],&ipf[2],&ipf[3],&ipt[0],&ipt[1],&ipt[2],&ipt[3] );
			if ( ip[0] >= ipf[0] && ip[0] <= ipt[0] && ip[1] >= ipf[1] && ip[1] <= ipt[1] && ip[2] >= ipf[2] && ip[2] <= ipt[2] && ip[3] >= ipf[3] && ip[3] <= ipt[3] ) {
				if ( !strcmp( password,node->data2 ) ) {
					bypassMBL = 1;
				}
			}
			break;
		}
		case 3:
		{
			sscanf( node->data1,"%i.%i.%i.%i-%i.%i.%i.%i", &ipf[0],&ipf[1],&ipf[2],&ipf[3],&ipt[0],&ipt[1],&ipt[2],&ipt[3] );
			if ( ip[0] >= ipf[0] && ip[0] <= ipt[0] && ip[1] >= ipf[1] && ip[1] <= ipt[1] && ip[2] >= ipf[2] && ip[2] <= ipt[2] && ip[3] >= ipf[3] && ip[3] <= ipt[3] ) {
				isBanned = 1;
			}
			break;
		}
		case 4:
		{
			sscanf( node->data1,"%i.%i.%i.%i-%i.%i.%i.%i", &ipf[0],&ipf[1],&ipf[2],&ipf[3],&ipt[0],&ipt[1],&ipt[2],&ipt[3] );
			if ( ip[0] >= ipf[0] && ip[0] <= ipt[0] && ip[1] >= ipf[1] && ip[1] <= ipt[1] && ip[2] >= ipf[2] && ip[2] <= ipt[2] && ip[3] >= ipf[3] && ip[3] <= ipt[3] ) {
				if ( !strcmp( password,node->data2 ) ) {
					bypassBan = 1;
				}
			}
			break;
		}
		case 5:
		{
			if ( !strcmp( node->data1,checkip ) ) {
				bantill = atoi( node->data2 );

				if ( bantill > (int)seconds ) {
					isTempbanned = bantill;
				}


			}

			break;
		}
		}
	}


	if ( isMBL && !bypassMBL ) {
		return "^3Banned from S4NDMoD. Protest at S4NDMoD.com.";
	}
	if ( isBanned && !bypassBan ) {
		return "^3You Have Been Banned From This Server!";
	}

	if ( isTempbanned ) {
		second = isTempbanned - (int)seconds;
		mins = second / 60;
		second -= mins * 60;
		tens = second / 10;
		second -= tens * 10;
		return va( "^3You are banned for: %i:%i%i",mins,tens,second );
	}

	return NULL;


}


void cleanTempBans() {
	struct list *node;
	time_t seconds;
	time( &seconds );

	for ( node = head; node != NULL; node = node->next )
	{
		if ( node->type != 5 ) {
			continue;
		}

		if ( seconds > atoi( node->data2 ) ) {
			removeNode( node );
		}
	}
	rewriteTempbans();
}

void listTempBans() {
	struct list *node;
	time_t seconds;
	int bantill,mins,tens,second;
	char* timeleft;

	time( &seconds );

	for ( node = head; node != NULL; node = node->next )
	{
		if ( node->type != 5 ) {
			continue;
		}

		bantill = atoi( node->data2 );

		if ( bantill > (int)seconds ) {
			second = bantill - (int)seconds;
			mins = second / 60;
			second -= mins * 60;
			tens = second / 10;
			second -= tens * 10;
			timeleft = va( "%i:%i%i",mins,tens,second );
			G_Printf( "%s %s %s\n",node->data1,node->data3,timeleft );
		}
	}
}

//move these to .h later


/*================================================================================
startWorkThread

this thread will start and loop to allow us to do things over sockets that would
normally cause hitch warnings. For starters will be used for resolving hostnames,
later it can be used to retrieve mbl, and update the global stats.
==================================================================================*/



void LockData() {
#ifdef WIN32
	WaitForSingleObject( workThreadLock, INFINITE ); //wait for it to be unlocked then lock..
#else
	pthread_mutex_lock( &workThreadLock );
#endif
	return;
}

void ReleaseLock() {
#ifdef WIN32
	ReleaseMutex( workThreadLock ); //release the lock
#else
	pthread_mutex_unlock( &workThreadLock );
#endif
	return;
}


void startWorkThread() {
	exitThread = 0;

#ifdef WIN32 //Windows code
	workThreadLock = CreateMutex( NULL, FALSE,NULL );
	workThread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) doWork, NULL, 0, &workThreadID );
#else //linux code
	pthread_mutex_init( &workThreadLock,NULL );
	pthread_create( &workThread, NULL, doWork, NULL );
#endif
	//G_Printf( "workThread: Worker thread has been started!\n" );
	return;
}

void endWorkThread() {
	LockData();
	exitThread++;
	ReleaseLock();
#ifdef WIN32 //wait for thread to end!
	WaitForSingleObject( workThread, INFINITE );
#else
	pthread_join( workThread, NULL );
#endif

	//G_Printf( "workThread: Worker thread was terminated!\n" );
	return;
}


/*=============================================================
doWork

This will be the function all of the threaded work is done in
===============================================================*/
void doWork( void*dummy ) {
	//first job at hand /get mbl
	gettingMBL = 1;
	if ( httpGet( "http://mbl.s4ndmod.com/getlist.php","mbl.txt" ) ) {
		LockData();
		addMessage( "rcvd",NULL,1 );
		ReleaseLock();
	} else {
		gettingMBL = 0;
	}
	while ( 1 ) {

		checkThreadInput();

		LockData();
		if ( exitThread ) { //if exit gets set close down now!
			ReleaseLock();
			return;
		} else {
			ReleaseLock();
		}
		#ifdef WIN32
		Sleep( 1000 ); //only do work every second
		#else
		sleep( 1 );
		#endif
	}
}

void checkThreadOutput() {
	struct messagelist *node;
	int cn;
	char hostname[256];
	level.lastThreadCheck = level.time;
	LockData();
	for ( node = mhead; node != NULL; node = node->next )
	{
		if ( node->type == 0 ) {
			continue;                //skip output

		}
		if ( !strcmp( node->command,"hostname" ) ) { //right now this is the only thing the thread does
			cn = -1;
			sscanf( node->data,"%i %s",&cn,hostname );
			strcpy( level.clients[cn].sess.hostname,hostname );
			//G_Printf( "Found hostname for client(%i): %s\n",cn,hostname );
			removeMessage( node );
			ReleaseLock();
			checkThreadOutput();
			return;
		} else if ( !strcmp( node->command,"rcvd" ) )       {
			parseMBL();
			//G_Printf( "Master ban list was received!\n" );
			gettingMBL = 0;
			removeMessage( node );
			ReleaseLock();
			checkThreadOutput();
			return;
		}
	}
	ReleaseLock();
}

void checkThreadInput() {
	struct messagelist *node;
	char ip[16];
	char*hostname;
	int cn;
	LockData();

	for ( node = mhead; node != NULL; node = node->next )
	{
		if ( node->type == 1 ) {
			continue;                //skip output

		}
		ReleaseLock();
		if ( !strcmp( node->command,"resolve" ) ) { //right now this is the only thing the thread does

			sscanf( node->data,"%i %s",&cn,ip );
			LockData();
			removeMessage( node );
			ReleaseLock();
			hostname = iptohostname( ip );
			LockData();
			addMessage( "hostname",va( "%i %s",cn,hostname ),1 );
			ReleaseLock();

			checkThreadInput();
			return;
		}
		LockData();
	}
	ReleaseLock();
}

void addMessage( char* command,char*data, int type ) {
	struct messagelist *node;

//allocate memory space for this
	node = (struct messagelist *)malloc( sizeof( struct messagelist ) );

	if ( command != NULL ) {
		strcpy( node->command,command );
	} else {
		node->command[0] = '\0';
	}

	if ( data != NULL ) {
		strcpy( node->data,data );
	} else {
		node->data[0] = '\0';
	}


	node->type = type;

	if ( mhead == NULL ) {
		mhead = node;
		node->prev = NULL;
	} else {
		mtail->next = node;
		node->prev = mtail;
	}
	mtail = node;
	node->next = NULL;
}


void removeMessage( struct messagelist *node ) {

	if ( node->prev == NULL ) {
		mhead = node->next;
	} else {
		node->prev->next = node->next;
	}

	if ( node->next == NULL ) {
		mtail = node->prev;
	} else {
		node->next->prev = node->prev;
	}

	free( node );
}



