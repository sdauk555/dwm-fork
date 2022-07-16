/* See LICENSE file for copyright and license details. */
#include "movestack.c"
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;     /* 0 means no bar */
static const int topbar             = 1;     /* 0 means bottom bar */
static const char buttonbar[]	    = "X";
static const char *fonts[]          = { "Fira Mono:size=10", "Noto Emoji Nerd Font Complete Mono:size10" };
static const char dmenufont[]       = "Fira Mono:size=10";
static const char col_black[]       = "#1d2021";
static const char col_gray2[]       = "#32302f";
static const char col_gray3[]       = "#32302f";
static const char col_white[]       = "#d4be98";
// static const char col_blue[]        = "#7daea3";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_white, col_gray3, col_black },
	[SchemeSel]  = { col_white, col_black,  col_black  },
};

/* tagging */
static const char *tags[] = { "⚪", "⚪", "⚪", "⚪", "⚪" };

static const Rule rules[] = {
	{ NULL, NULL,	NULL,	0,	False,	-1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define SUPER Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray3, "-nf", col_white, "-sb", col_black, "-sf", col_white, NULL };

/* custom program cmds */
static const char *termcmd[]  = { "alacritty", NULL };
static const char *browsercmd[] = { "google-chrome-stable", NULL };
static const char *filecmd[] = { "pcmanfm", NULL };
static const char *spotifycmd[] = { "spotify", NULL };
static const char *officecmd[] = { "libreoffice", NULL };
static const char *emailcmd[] = { "thunderbird", NULL };
static const char *discordcmd[] = { "discord", NULL };
static const char *vscodecmd[] = { "code", NULL };
static const char *vmcmd[] = { "virt-manager", NULL };
static const char *screenshotcmd[] = { "flameshot", "gui", NULL };
static const char *bluemancmd[] = { "blueman-manager", NULL };
static const char *pavucontrolcmd[] = { "pavucontrol", NULL };

/* custom system control cmds */
static const char *lockscreencmd[] = { "betterlockscreen", "-l", NULL };
static const char *mutecmd[] = { "pactl", "set-sink-mute", "0", "toggle", NULL };
static const char *volupcmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+2%", NULL };
static const char *voldowncmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL };
static const char *brupcmd[] = { "xbacklight", "-inc", "10", NULL };
static const char *brdowncmd[] = { "xbacklight", "-dec", "10", NULL };
static const char *monitorcmd[] = { "/home/sam/scripts/monitor.sh", NULL };
static const char *laptopcmd[] = { "/home/sam/scripts/laptop.sh", NULL };
static const char *setoutputcmd[] = { "/home/sam/scripts/outputSwitch.sh", NULL };

static Key keys[] = {
	/* modifier                     key       			 function          argument */
	{ SUPER,                        XK_space, 			 spawn,            {.v = dmenucmd } },
	{ MODKEY|ControlMask,           XK_t, 	  			 spawn,            {.v = termcmd } },
	{ MODKEY,                       XK_b,     			 togglebar,        {0} },
	{ MODKEY,                       XK_j,     			 focusstack,       {.i = +1 } },
	{ MODKEY,                       XK_k,     			 focusstack,       {.i = -1 } },
	{ MODKEY,                       XK_i,     			 incnmaster,       {.i = +1 } },
	{ MODKEY,                       XK_d,     			 incnmaster,       {.i = -1 } },
	{ MODKEY,                       XK_h,     			 setmfact,         {.f = -0.05} },
	{ MODKEY,                       XK_l,     			 setmfact,         {.f = +0.05} },
        { MODKEY|Mod4Mask,              XK_0,      			 togglegaps,       {0} },
	{ MODKEY|ShiftMask,		XK_j,	  			 movestack,        {.i = +1 } },
	{ MODKEY|ShiftMask,		XK_k,     			 movestack,        {.i = -1 } },
	{ MODKEY,                       XK_Return,			 zoom,             {0} },
	{ MODKEY,                       XK_Tab,   			 view,             {0} },
	{ SUPER,             		XK_q,     			 killclient,       {0} },
	{ MODKEY,                       XK_t,     			 setlayout,        {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,     			 setlayout,        {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,     			 setlayout,        {.v = &layouts[2]} },
	{ MODKEY,                       XK_space, 			 setlayout,        {0} },
	{ MODKEY|ShiftMask,             XK_space, 			 togglefloating,   {0} },
	{ MODKEY,                       XK_0,     			 view,             {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,     			 tag,              {.ui = ~0 } },
	{ MODKEY,                       XK_comma, 			 focusmon,         {.i = -1 } },
	{ MODKEY,                       XK_period,			 focusmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma, 			 tagmon,           {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,			 tagmon,           {.i = +1 } },
	{ SUPER,			XK_w,	  			 spawn,	  	{.v = browsercmd } },
	{ SUPER,			XK_f,	  			 spawn,	  	{.v = filecmd } },
	{ SUPER,			XK_m,	  			 spawn,	  	{.v = emailcmd } },
	{ SUPER,			XK_l,	  			 spawn,	  	{.v = lockscreencmd } },
	{ MODKEY|ControlMask,		XK_s,	  			 spawn,	  	{.v = spotifycmd } },
	{ MODKEY|ControlMask,		XK_o,	  			 spawn,	  	{.v = officecmd } },
	{ MODKEY|ControlMask,		XK_d,				 spawn,		{.v = discordcmd } },
	{ MODKEY|ControlMask,		XK_c,	  			 spawn,	  	{.v = vscodecmd } },
	{ MODKEY|ControlMask,		XK_v,	  			 spawn,	  	{.v = vmcmd } },
	{ MODKEY|ControlMask,		XK_b,	  			 spawn,	  	{.v = bluemancmd } },
	{ MODKEY|ControlMask,		XK_p,	  			 spawn,	  	{.v = pavucontrolcmd } },
	{ SUPER,			XK_Print, 			 spawn,	  	{.v = screenshotcmd } },
	{ SUPER|ShiftMask|ControlMask,	XK_1,	  			 spawn,	  	{.v = monitorcmd } },
	{ SUPER|ShiftMask|ControlMask,	XK_2,	  			 spawn,	  	{.v = laptopcmd } },
	{ SUPER|ShiftMask|ControlMask,	XK_3,	  			 spawn,	  	{.v = setoutputcmd } },
	{ 0, 				XF86XK_AudioMute, 		 spawn,   	{.v = mutecmd } },
	{ 0, 				XF86XK_AudioLowerVolume,	 spawn, 	{.v = voldowncmd } },
	{ 0, 				XF86XK_AudioRaiseVolume,	 spawn, 	{.v = volupcmd } },
	{ 0,                            XF86XK_MonBrightnessUp, 	 spawn, 	{.v = brupcmd } },
	{ 0,                            XF86XK_MonBrightnessDown,	 spawn, 	{.v = brdowncmd } },
	{ MODKEY|ControlMask,		XK_k,	   			 viewtoright,   {0} },
	{ MODKEY|ControlMask,		XK_j,	   			 viewtoleft,	{0} },
	TAGKEYS(                        XK_1,                     	 0)
	TAGKEYS(                        XK_2,                     	 1)
	TAGKEYS(                        XK_3,                     	 2)
	TAGKEYS(                        XK_4,                     	 3)
	TAGKEYS(                        XK_5,                     	 4)
	{ MODKEY|ControlMask,           XK_Delete,     			 quit,             {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkButton,		0,		Button1,	killclient,	{0} },
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

