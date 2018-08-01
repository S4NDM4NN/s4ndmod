//#include "cg_local.h"
//#include "..\ui\keycodes.h"
#include "ui_local.h"
#include "md5.h"
#include "GetHDDSerial.h"

//char * getHardDriveSerial();


void checkCDKey() {
	FILE* cdkeyFile;

	cdkeyFile = fopen( "main/rtcwkey", "r" );
	if ( cdkeyFile ) {
		char *serial;
		char hash[33];
		char buffer[33];

		fgets( buffer,33, cdkeyFile );
		fclose( cdkeyFile );

		serial = getHardDriveSerial();

		if ( !strlen( serial ) ) { //apparently they dont have a harddrive lol...
			serial = GetMAC();

			if ( !strlen( serial ) ) {
				Com_Error( ERR_FATAL, "VM_Create on UI failed: Code 1" );
			}
		}

		getmd5( serial, hash );

		if ( strcmp( hash,buffer ) ) {
			setCDKey();
			// note that if this one actually triggers, its because of a hardware
			// change or spoof.
			Com_Error( ERR_FATAL, "VM_Create on UI failed: Code 2" );
		}
	}
	else {
		setCDKey();
	}
}


void setCDKey() {
	char *serial;
	char hash[33];
	FILE* cdkeyFile;

	serial = getHardDriveSerial();

	if ( !strlen( serial ) ) { //apparently they dont have a harddrive lol...
		serial = GetMAC();

		if ( !strlen( serial ) ) {
			Com_Error( ERR_FATAL, "VM_Create on UI failed: Code 1" );
		}
	}

	getmd5( serial, hash );

	cdkeyFile = fopen( "main/rtcwkey", "w" );
	if ( cdkeyFile ) {
		fputs( hash, cdkeyFile );
		fclose( cdkeyFile );
	}
	else {
		// for whatever reason, they couldn't write a key file. most likely permissions
		Com_Error( ERR_FATAL, "VM_Create on UI failed: Code 3" );
	}

//		Com_Printf( "^2Writing CDKey\n" );
//		Com_Printf( "Hashed Serial: '%s'\n", hash );

}

