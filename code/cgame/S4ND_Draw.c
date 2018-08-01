#include "cg_local.h"
#include "..\ui\keycodes.h"

#define MAX_WINDOWS 8
#define MAX_WINDOW_ITEMS 64 //should be more then enough for now

#define X_LOC 0
#define Y_LOC 1
#define W_LOC 2
#define H_LOC 3

//for formatting purposes!
#define WINDOW_TITLEBAR_HEIGHT 10

typedef enum {
	DRAW_COMPASS,
	DRAW_AMMO,
	DRAW_AMMO_COUNT,
	DRAW_HEALTH,
	DRAW_LIFES,
	DRAW_CHARGE_TIME,
	DRAW_STAMINA,
	DRAW_KILLS,
	DRAW_CHATS
}windowItemTypes_t;

int compassPos[4];
int healthPos[4];
int lifesPos[4];
int ammoPos[4];
int ammocountPos[4];
int chargetimePos[4];
int staminaPos[4];
int chatPos[4];
int killPos[4];



typedef struct S4ND_windowItems_s
{
	int window; //the window number this is linked to
	windowItemTypes_t type;     //What type of item we are adding(button,checkbox,text,etc)
	int loc[4]; //this location is from (0,0)top left of window,also stores height and width
	vec4_t color; //will be used for text, maybe some other things.
	int inuse;
}S4ND_windowItems_t;


typedef struct S4ND_window_s {
	int loc[2]; //x, y
	int height;
	int width;
	int inuse;
	int priority;
	char title[32];
	int clickedTime;
	int active;
	int fillBody;
	int isHud;
} S4ND_window_t;

S4ND_window_t S4ND_windows[MAX_WINDOWS];
S4ND_windowItems_t S4ND_windowItems[MAX_WINDOW_ITEMS];
void CG_DrawCompassIcon( int x, int y, int w, int h, vec3_t origin, vec3_t dest, qhandle_t shader );


void S4ND_UI_setpriority( int win,int priority,qboolean newWin ) {
	int i;
	int shiftPriority = S4ND_windows[win].priority;

	for ( i = 0; i < MAX_WINDOWS; i++ )
	{
		if ( !S4ND_windows[i].inuse ) {
			continue;
		}
		if ( i == win ) {
			S4ND_windows[i].priority = priority;
		} else {
			if ( newWin ) {
				if ( S4ND_windows[i].priority >= priority ) {
					S4ND_windows[i].priority++;
				}
			} else {
				if ( S4ND_windows[i].priority < shiftPriority ) {
					S4ND_windows[i].priority++;
				}
			}
		}
	}
}

int S4ND_UI_newWindow( int x,int y,int height,int width,char*title,int isHud ) {
	int i;
	S4ND_window_t *tempWin;
	tempWin = NULL;

	for ( i = 0; i < MAX_WINDOWS; i++ )
	{
		if ( S4ND_windows[i].inuse ) {
			continue;
		}

		tempWin = &S4ND_windows[i];

		tempWin->inuse = 1;
		tempWin->loc[X_LOC] = x;
		tempWin->loc[Y_LOC] = y;
		if ( !isHud ) {
			tempWin->height = height + WINDOW_TITLEBAR_HEIGHT; //make height be + title bar size
			tempWin->width = width + 2;
		} else {
			tempWin->height = height; //make height be + title bar size
			tempWin->width = width + 2;
		}
		tempWin->isHud = isHud;

		strcpy( tempWin->title,title );

		S4ND_UI_setpriority( i,1,qtrue );

		break;
	}

	if ( !tempWin ) {
		CG_Printf( "^1Max number of windows(%i) are already in use!\n",MAX_WINDOWS );
		return -1;
	} else {
		return i; //return this so we can add 'contents'
	}
}

int S4ND_UI_activeWindowCount() {
	int count = 0;
	int i;
	//get an active window count
	for ( i = 0; i < MAX_WINDOWS; i++ )
	{
		if ( !S4ND_windows[i].inuse ) {
			continue;
		}
		count++;
	}
	return count;
}

void S4ND_UI_windowSizeFromHUD( int window ) {
	int i;
	windowItemTypes_t type = -1;
	S4ND_window_t * tWin;
	tWin = &S4ND_windows[window];

	for ( i = 0; i < MAX_WINDOW_ITEMS; i++ )
	{
		if ( S4ND_windowItems[i].window == window ) {
			type = S4ND_windowItems[i].type;
			break;
		}
	}

	switch ( type )
	{
	case DRAW_HEALTH:
		tWin->height = healthPos[H_LOC];
		tWin->width = healthPos[W_LOC];
		break;
	case DRAW_COMPASS:
		tWin->height = compassPos[H_LOC];
		tWin->width = compassPos[W_LOC];
		break;
	case DRAW_LIFES:
		tWin->height = lifesPos[H_LOC];
		tWin->width = lifesPos[W_LOC];
		break;
	case DRAW_STAMINA:
		tWin->height = staminaPos[H_LOC];
		tWin->width = staminaPos[W_LOC];
		break;
	case DRAW_CHARGE_TIME:
		tWin->height = chargetimePos[H_LOC];
		tWin->width = chargetimePos[W_LOC];
		break;
	}
}

void S4ND_UI_DrawWindow( int window ) {
	S4ND_window_t * tWin;
	int maxStrlen;

	//colors from windows xp default theme lol
	vec4_t backgroundColor = {.48,.59,.9,1};
	vec4_t focusColor = {0,.33,.9,1};

	vec4_t color = {0,0,0,0};
	tWin = &S4ND_windows[window];

	if ( tWin->priority == 1 ) {
		VectorFourCopy( focusColor,color );
	} else {
		VectorFourCopy( backgroundColor,color );
	}

	if ( !tWin->isHud ) {
		maxStrlen = tWin->width / TINYCHAR_WIDTH;
		//This will put a black border around the edges
		CG_DrawRect( tWin->loc[X_LOC] - 1,tWin->loc[Y_LOC] - 1,tWin->width + 2,tWin->height + 2,1,colorMdGrey );

		//actual body of window
		if ( tWin->fillBody ) {
			CG_FillRect( tWin->loc[X_LOC],tWin->loc[Y_LOC],tWin->width,tWin->height,colorWhite );
		}



		CG_FillRect( tWin->loc[X_LOC],tWin->loc[Y_LOC],tWin->width,WINDOW_TITLEBAR_HEIGHT,color );
		CG_DrawRect( tWin->loc[X_LOC],tWin->loc[Y_LOC],tWin->width,tWin->height,2,color );

		CG_DrawStringExt( tWin->loc[X_LOC], tWin->loc[Y_LOC],va( "%s",tWin->title ),colorWhite,qfalse,qfalse,TINYCHAR_WIDTH,TINYCHAR_HEIGHT,maxStrlen );

		//close box
		CG_FillRect( tWin->loc[X_LOC] + tWin->width - WINDOW_TITLEBAR_HEIGHT + 1,tWin->loc[Y_LOC] + 1,WINDOW_TITLEBAR_HEIGHT - 2,WINDOW_TITLEBAR_HEIGHT - 2,colorRed );
		CG_DrawRect( tWin->loc[X_LOC] + tWin->width - WINDOW_TITLEBAR_HEIGHT + 1,tWin->loc[Y_LOC] + 1,WINDOW_TITLEBAR_HEIGHT - 2,WINDOW_TITLEBAR_HEIGHT - 2,1,colorWhite );
		CG_DrawStringExt( tWin->loc[X_LOC] + tWin->width - WINDOW_TITLEBAR_HEIGHT + 2, tWin->loc[Y_LOC] + 2,"X",colorWhite,qfalse,qfalse,6,6,2 );

	} else { //it is a hud item, lets just draw a box around it.
		    //draw a box around the hud element
		S4ND_UI_windowSizeFromHUD( window );
		CG_DrawRect( tWin->loc[X_LOC],tWin->loc[Y_LOC],tWin->width,tWin->height,2,color );
	}
}

void S4ND_UI_UpdateLoc( int win,int old_x,int old_y ) {
	int i;
	S4ND_windows[win].loc[X_LOC] += cgs.cursorX - old_x;
	S4ND_windows[win].loc[Y_LOC] += cgs.cursorY - old_y;
	S4ND_windows[win].active = 0;

	//look through the "windowsitems" and see if hud elements need updated.
	for ( i = 0; i < MAX_WINDOW_ITEMS; i++ )
	{
		if ( !S4ND_windowItems[i].inuse ) {
			continue;
		}
		if ( S4ND_windowItems[i].window == win ) {
			switch ( S4ND_windowItems[i].type )
			{
			case DRAW_COMPASS:
				compassPos[X_LOC] += cgs.cursorX - old_x;
				compassPos[Y_LOC] += cgs.cursorY - old_y;
				break;
			case DRAW_HEALTH:
				healthPos[X_LOC] += cgs.cursorX - old_x;
				healthPos[Y_LOC] += cgs.cursorY - old_y;
				break;
			case DRAW_LIFES:
				lifesPos[X_LOC] += cgs.cursorX - old_x;
				lifesPos[Y_LOC] += cgs.cursorY - old_y;
				break;
			case DRAW_STAMINA:
				staminaPos[X_LOC] += cgs.cursorX - old_x;
				staminaPos[Y_LOC] += cgs.cursorY - old_y;
				break;
			case DRAW_CHARGE_TIME:
				chargetimePos[X_LOC] += cgs.cursorX - old_x;
				chargetimePos[Y_LOC] += cgs.cursorY - old_y;
				break;
			}
		}
	}

}

void S4ND_UI_DrawS4NDUI( int old_x,int old_y ) {
	int activeWindows = 0;
	int windowsDrawn = 0;
	int i;

	//get how many windows we have active
	activeWindows = S4ND_UI_activeWindowCount();
	//we want to find the "deepest" window first
	while ( windowsDrawn < activeWindows )
	{
		for ( i = 0; i < MAX_WINDOWS; i++ )
		{
			if ( !S4ND_windows[i].inuse ) {
				continue;
			}

			if ( S4ND_windows[i].priority != activeWindows - windowsDrawn ) {
				continue;
			}
			//check to make sure window dosent need to be updated
			if ( S4ND_windows[i].active ) {
				S4ND_UI_UpdateLoc( i,old_x,old_y );
			}

			S4ND_UI_DrawWindow( i );

			windowsDrawn++;
		}
	}

	//draw window very last
	CG_DrawPic( cgs.cursorX - 16,cgs.cursorY - 16,32,32,cgs.media.cursor );
}


//finds the window that the pointer is currently over. It also checks by priority.
int S4ND_UI_OverWindow() {
	int i;
	int curPriority = MAX_WINDOWS;
	int curWin = -1;
	S4ND_window_t * tWin = NULL;

	float x,y;

	x = cgs.cursorX;
	y = cgs.cursorY;

	for ( i = 0; i < MAX_WINDOWS; i++ )
	{
		if ( !S4ND_windows[i].inuse ) {
			continue;
		}

		tWin = &S4ND_windows[i];


		if ( x >= tWin->loc[X_LOC] && ( x <= ( tWin->loc[X_LOC] + tWin->width ) ) &&
			 y >= tWin->loc[Y_LOC] && ( y <= ( tWin->loc[Y_LOC] + tWin->height ) ) ) {

			if ( tWin->priority < curPriority ) {

				curPriority = tWin->priority;
				curWin = i;
			}
		}
	}

	return curWin;
}

void S4ND_UI_closeWindow( int win ) {
	int i;

	S4ND_windows[win].inuse = 0;

	//the closed window should have been up front, so move the rest up to fill in priority
	for ( i = 0; i < MAX_WINDOWS; i++ )
	{
		if ( !S4ND_windows[i].inuse ) {
			continue;
		}

		S4ND_windows[i].priority--; //slide them all down 1

	}

}

void S4ND_UI_onWindowLeftClicked( int win ) {
	S4ND_window_t * tWin = NULL;

	tWin = &S4ND_windows[win];

	//if the window isnt in front already send it to front
	if ( tWin->priority != 1 ) {
		S4ND_UI_setpriority( win,1,qfalse );
	}

	//check if they are clicking on the windows title bar, and if they are
	//set the window as active, this is used to let the user drag it around screen.
	//this could also end up closing the window
	if ( !tWin->isHud ) {
		if ( cgs.cursorX >= tWin->loc[X_LOC] && ( cgs.cursorX <= ( tWin->loc[X_LOC] + tWin->width ) ) &&
			 cgs.cursorY >= tWin->loc[Y_LOC] && ( cgs.cursorY <= ( tWin->loc[Y_LOC] + WINDOW_TITLEBAR_HEIGHT ) ) ) {
			//check and see if they clicked to close it
			if ( cgs.cursorX >= tWin->loc[X_LOC] + tWin->width - WINDOW_TITLEBAR_HEIGHT + 1 && cgs.cursorX <= ( tWin->loc[X_LOC] + tWin->width - WINDOW_TITLEBAR_HEIGHT + 1 ) + ( WINDOW_TITLEBAR_HEIGHT - 2 ) &&
				 cgs.cursorY >= tWin->loc[Y_LOC] + 1 && cgs.cursorY <= ( tWin->loc[Y_LOC] + 1 ) + ( WINDOW_TITLEBAR_HEIGHT - 2 ) ) {
				S4ND_UI_closeWindow( win );
				return;
			}
			tWin->active++;
		} else { //the window is being clicked on elsewhere(this will be used for doing things on the page
		}
	} else {
		if ( cgs.cursorX >= tWin->loc[X_LOC] && ( cgs.cursorX <= ( tWin->loc[X_LOC] + tWin->width ) ) &&
			 cgs.cursorY >= tWin->loc[Y_LOC] && ( cgs.cursorY <= ( tWin->loc[Y_LOC] + tWin->height ) ) ) {
			tWin->active++;
		}
	}
}



void S4ND_UI_CheckKeys() {

	int win;
	S4ND_window_t * tWin = NULL;

	win = S4ND_UI_OverWindow();

	if ( win == -1 ) {
		return;
	}

//	CG_DrawStringExt(0, 0,va("You are hovering over window %i!",win),colorRed,qfalse,qfalse,TINYCHAR_WIDTH,TINYCHAR_HEIGHT,32);

	tWin = &S4ND_windows[win];

	if ( trap_Key_IsDown( K_MOUSE1 ) ) {
		S4ND_UI_onWindowLeftClicked( win );
	} else if ( trap_Key_IsDown( K_MOUSE2 ) )      {
	}
}




int S4ND_UI_AddItem( int win,windowItemTypes_t type,int x,int y,int h,int w, vec4_t color ) {
	int i;

	for ( i = 0; i < MAX_WINDOW_ITEMS; i++ )
	{
		if ( S4ND_windowItems[i].inuse ) {
			continue;
		}

		S4ND_windowItems[i].inuse = 1;
		S4ND_windowItems[i].type = type;
		S4ND_windowItems[i].window = win;
		S4ND_windowItems[i].loc[X_LOC] = x;
		S4ND_windowItems[i].loc[Y_LOC] = y;
		S4ND_windowItems[i].loc[W_LOC] = w;
		S4ND_windowItems[i].loc[H_LOC] = h;

		switch ( type )
		{
		case DRAW_COMPASS:
			compassPos[X_LOC] = S4ND_windows[win].loc[X_LOC] + 1;
			compassPos[Y_LOC] = S4ND_windows[win].loc[Y_LOC] + 1;
			break;
		case DRAW_HEALTH:
			healthPos[X_LOC] = S4ND_windows[win].loc[X_LOC] + 1;
			healthPos[Y_LOC] = S4ND_windows[win].loc[Y_LOC] + 1;
			break;
		case DRAW_LIFES:
			lifesPos[X_LOC] = S4ND_windows[win].loc[X_LOC] + 1;
			lifesPos[Y_LOC] = S4ND_windows[win].loc[Y_LOC] + 1;
			break;
		case DRAW_STAMINA:
			staminaPos[X_LOC] = S4ND_windows[win].loc[X_LOC] + 1;
			staminaPos[Y_LOC] = S4ND_windows[win].loc[Y_LOC] + 1;
			break;
		case DRAW_CHARGE_TIME:
			chargetimePos[X_LOC] = S4ND_windows[win].loc[X_LOC] + 1;
			chargetimePos[Y_LOC] = S4ND_windows[win].loc[Y_LOC] + 1;
			break;
		}

		if ( color ) {
			VectorFourCopy( color,S4ND_windowItems[i].color );
		}

		break;
	}

	if ( S4ND_windowItems[i].inuse ) {
		return i;
	} else {
		return -1;
	}
}



void S4ND_INIT_UI() {
	int window;

	//add compass
	window = S4ND_UI_newWindow( 100,100,60,60,"Compass",qtrue );
	S4ND_UI_AddItem( window,DRAW_COMPASS,0,0,0,0,NULL );
	//add health
	window = S4ND_UI_newWindow( 150,100,50,25,"Health",qtrue );
	S4ND_UI_AddItem( window,DRAW_HEALTH,0,0,0,0,NULL );
	//add lifes
	window = S4ND_UI_newWindow( 150,100,50,50,"Lifes Left",qtrue );
	S4ND_UI_AddItem( window,DRAW_LIFES,0,0,0,0,NULL );
	//add stamina
	window = S4ND_UI_newWindow( 150,100,50,50,"Stamina",qtrue );
	S4ND_UI_AddItem( window,DRAW_STAMINA,0,0,0,0,NULL );
	//add recharge
	window = S4ND_UI_newWindow( 150,100,50,50,"Weapon Charge",qtrue );
	S4ND_UI_AddItem( window,DRAW_CHARGE_TIME,0,0,0,0,NULL );



}

//will be used to draw windows check mouse pointer etc
void S4ND_UI_doUI() {
	char buf[32];
	static int old_x,old_y;


	if ( !cg_drawS4NDUI.integer ) {
		if ( trap_Key_GetCatcher() & KEYCATCH_CGAME ) {
			trap_Key_SetCatcher( 0 );
		}
		return;
	}

	trap_Cvar_VariableStringBuffer( "ui_limboMode", buf, sizeof( buf ) );
	if ( atoi( buf ) ) {
		return;
	}



	trap_Key_SetCatcher( KEYCATCH_CGAME );

	S4ND_UI_DrawS4NDUI( old_x,old_y );

	S4ND_UI_CheckKeys();

	old_x = cgs.cursorX;
	old_y = cgs.cursorY;
}