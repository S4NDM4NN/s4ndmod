OSP for Wolfenstein - Client README
===================================
Version: 0.9
Date:    30 Dec 03
Contact: rhea@OrangeSmoothie.org
Site:    http://www.OrangeSmoothie.org



Overview:
---------
This document describes all client-side settings.  For OSP-specific
commands, please see: osp-wolf-Commands.txt

Note: all of these settings can be viewed/modified in the in-game
UI, accessed through the ESC key.


Misc.
-----
There are 32 color escape codes in the OSP UI and cgame modules.  From
the normal ^0 - ^7, the following are also available:

	- ^8, ^9, ^<, ^>, ^:, ^;, ^=, ^?, ^@
	- ^A - ^O   (note, caps MUST be used to get the proper color)
	- For future compatibility with Main/, use ^p (lower case) for
        the color black.

+vstr support
	+vstr gives customizable "+" command support.  That is, with
      +vstr, a player has the ability to execute a command when a
      key is pressed, and another command when the same key is
      released.

	Format: bind <some_key> +vstr <var1> <var2>

	e.g.
		set some_var "say HEY!"
		set another_var "say There!!"
		bind o +vstr some_var another_var

		pressing o results in:  "HEY!"
		releasing o results in: "There!!"


Settings:
---------
cf_wstats <value> (default = 1.2)
      Specifies the font scale of the +wstats window.  The higher the
      value, the bigger the font (floating point value)

cf_wstopshots <value> (default = 0.8)
      Specifies the font scale of the +wtopshots window.  The higher
      the value, the bigger the font (floating point value)

cg_announcer <0|1> (default = 1)
      0 - Disables announcer voice messages
    * 1 - Enables announcer voice messages

cg_autoAction <bitmask> (default = 0)
      Specifies automatic actions to occur at key points in the
      game.  This setting is a bitmask:
           1 - Start a demo at the start of the round
           2 - Take a screenshot at the end of the round
           4 - Save game stats to a file at the end of the round

cg_autoReload <0|1> (default = 1)
      0 - Weapon is not automatically reloaded when clip is empty.
    * 1 - Weapon is automatically reloaded when clip is empty.

cg_bloodDamageBlend <0.0-1.0> (default = 1.0)
      Specifies the transparency level of blood "splatters" when hit
      by other players' bullets.

cg_bloodFlash <0.0-1.0> (default = 1.0)
      Specifies intensity of the red blood "flash" when taking damage.

cg_chatBeep <0|1|2> (default = 1)
      0 - No beeps made when chat is received
    * 1 - Beeps when normal and team chat is received (different sounds)
      2 - Beeps only when team chat is received

cg_complaintPopUp <0|1>  (default = 1)
      0 - Disables the auto-popup of the teammate complaint request for
            being killed by a teammate.
    * 1 - Auto-popup of teammate complaint request enabled.

cg_crosshairAlpha <0.0-1.0> (default = 1.0)
cg_crosshairAlphaAlt
      Specifies the transparency of the crosshair (Alt is for the
      secondary crosshair).  Note "cg_crosshairHealth" must be
      set to 0 for this option to be enabled.

cg_crosshairColor <color|0x-escape> (default = "White")
cg_crosshairColorAlt
      Specifies the color of the crosshair (Alt is for the secondary
      crosshair).  0x-escape is of the format 0xRRGGBB, where RRGGBB
      specify the Red/Green/Blue channels respecitively.  Each value
      is in hexidecimal format (00-FF).  For color naming, most common
      color names are available.  Note "cg_crosshairHealth" must be
      set to 0 for this option to be enabled.  String colors that
      are available for use:
		"white"
		"black"
		"red"
		"green"
		"blue"
		"yellow"
		"magenta"
		"cyan"
		"orange"
		"mdred"		(Medium Red)
		"mdgreen"		(Medium Green)
		"dkgreen"		(Dark Green)
		"mdcyan"		(Medium Cyan)
		"mdyellow"		(Medium Yellow)
		"mdorange"		(Medium Orange)
		"mdblue"		(Medium Blue)
		"ltgrey"		(Light Grey)
		"mdgrey"		(Medium Grey)
		"dkgrey"		(Dark Grey)

cg_crosshairPulse <0|1> (default = 1)
      0 - Crosshair does NOT expand with movement/firing.
    * 1 - Crosshair will grow to reflect current "spead" of fire due to
          movement/firing.

cg_drawCrosshairName <0|1|2> (default = 1)
      0 - Teammate names/status are not shown over crosshair
    * 1 - Displays teammate name/health bar/ammo count (only for weapons with
          real ammo) when crosshair is hovered over the player model.
      2 - Displays teammate name/health bar (no ammo) when crosshair is
          hovered over the player model.

cg_drawReinforcementTime <0|1> (default = 0)
    * 0 - Team reinforcement time not drawn on the active HUD.
      1 - Shows team reinforcement time on the active HUD.

cg_drawWeaponIconFlash <0|1> (default = 0)
    * 0 - Weapon icon flashing not enabled
      1 - Outline of weapon icon is colored based on weapon state:
		Red = Firing
		Yellow = Reload
		White = Switching

cg_muzzleFlash <0|1> (default = 1)
    * 0 - Local flash effect is NOT seen when firing current weapon.
      1 - Weapon flash effect is shown when weapon is fired.

cg_noAmmoAutoSwitch <0|1> (default = 1)
      0 - Weapon is NOT switched if completely out of ammo
    * 1 - Weapon is automatically switched if completely out of ammo

cg_printObjectiveInfo <0|1> (default = 1)
      0 - No printing of objective messages in the console (center-print only)
    * 1 - Objective messages show up in both console and center

cg_useScreenshotJPEG <0|1> (default = 1)
      0 - Use .tga format for autoScreenshot command
    * 1 - Use .jpg format for autoScreenshot command

ch_font <0|1|2> (default = 0)
    * 0 - Original RtCW font
      1 - Alternate font 1
      2 - Alternate font 2

demo_avifpsF1 <integer> (default = 0 [OFF])
demo_avifpsF2 <integer> (default = 10)
demo_avifpsF3 <integer> (default = 15)
demo_avifpsF4 <integer> (default = 20)
demo_avifpsF5 <integer> (default = 24)
      Specifies the cl_avidemo setting for the appropriate F1-F5 key
      that can be used during demo playback.

demo_drawTimeScale <0|1> (default = 1)
      0 - No text is shown when timescale is changed in demo playback
    * 1 - Briefly displays current timescale in lower left corner when
          its value is updated through a demo key.

mv_sensitivity <value> (default = 20.0)
      Specifies the sensitivity of the mouse when in multiview
      spectator mode.