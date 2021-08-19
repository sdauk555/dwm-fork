/* See LICENSE file for copyright and license details. */

#include "movestack.c"
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#141414";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#262938";
static const char col_gray4[]       = "#eeeeee";
static const char col_blue[]        = "#0a2a8a";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray4, col_gray3, col_blue },
	[SchemeSel]  = { col_gray4, col_gray1,  col_blue  },
};

/* tagging */
static const char *tags[] = { "⚪", "⚪", "⚪", "⚪", "⚪", "⚪", "⚪", "⚪", "⚪" };

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
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray3, "-nf", col_gray4, "-sb", col_blue, "-sf", col_gray4, NULL };

/* custom program cmds */
static const char *termcmd[]  = { "alacritty", NULL };
static const char *bravecmd[] = { "brave", NULL };
static const char *pcmanfmcmd[] = { "pcmanfm", NULL };
static const char *thunderbirdcmd[] = { "thunderbird", NULL };
static const char *spotifycmd[] = { "spotify", NULL };
static const char *vmplayercmd[] = { "vmplayer", NULL };
static const char *libreofficecmd[] = { "libreoffice", NULL };
static const char *bluemancmd[] = { "blueman-manager", NULL };
static const char *pavucontrolcmd[] = { "pavucontrol", NULL };

/* custom system control cmds */
static const char *lockscreencmd[] = { "betterlockscreen", "-l", "--time-format", "%l:%M %_p", NULL };
static const char *mutecmd[] = { "pactl", "set-sink-mute", "0", "toggle", NULL };
static const char *volupcmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%", NULL };
static const char *voldowncmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%", NULL };
static const char *audiosrc1cmd[] = { "pactl", "set-default-sink", "0", NULL };
static const char *audiosrc2cmd[] = { "pactl", "set-default-sink", "1", NULL };
static const char *audiosrc3cmd[] = { "pactl", "set-default-sink", "2", NULL };
static const char *audiosrc4cmd[] = { "pactl", "set-default-sink", "3", NULL };
static const char *brupcmd[] = { "xbacklight", "-inc", "10", NULL };
static const char *brdowncmd[] = { "xbacklight", "-dec", "10", NULL };
static const char *setdualscreen1cmd[] = { "/home/sam/.screenlayout/DualMonitor1.sh", NULL };
static const char *setdualscreen2cmd[] = { "/home/sam/.screenlayout/DualMonitor2.sh", NULL };
static const char *setsinglescreencmd[] = { "/home/sam/.screenlayout/SingleMonitor.sh", NULL }; 

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ SUPER,                        XK_space,  spawn,          {.v = dmenucmd } },
	{ MODKEY|ControlMask,           XK_t, 	   spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,		XK_j,	   movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,		XK_k,      movestack,      {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_h,      incrgaps,       {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_l,      incrgaps,       {.i = -1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ SUPER,             		XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ SUPER,			XK_w,	   spawn,	   {.v = bravecmd } },
	{ SUPER,			XK_f,	   spawn,	   {.v = pcmanfmcmd } },
	{ SUPER,			XK_l,	   spawn,	   {.v = lockscreencmd } },
	{ MODKEY|ControlMask,		XK_m,	   spawn,	   {.v = thunderbirdcmd } },
	{ MODKEY|ControlMask,		XK_s,	   spawn,	   {.v = spotifycmd } },
	{ MODKEY|ControlMask,		XK_v,	   spawn,	   {.v = vmplayercmd } },
	{ MODKEY|ControlMask,		XK_l,	   spawn,	   {.v = libreofficecmd } },
	{ MODKEY|ControlMask,		XK_b,	   spawn,	   {.v = bluemancmd } },
	{ MODKEY|ControlMask,		XK_p,	   spawn,	   {.v = pavucontrolcmd } },
	{ SUPER|ShiftMask|ControlMask,	XK_1,	   spawn,	   {.v = setdualscreen1cmd } },
	{ SUPER|ShiftMask|ControlMask,	XK_2,	   spawn,	   {.v = setdualscreen2cmd } },
	{ SUPER|ShiftMask|ControlMask,	XK_3,	   spawn,	   {.v = setsinglescreencmd } },
	{ 0, 				XF86XK_AudioMute, spawn,   {.v = mutecmd } },
	{ 0, 				XF86XK_AudioLowerVolume, spawn, {.v = voldowncmd } },
	{ 0, 				XF86XK_AudioRaiseVolume, spawn, {.v = volupcmd } },
	{ MODKEY|ShiftMask|ControlMask, XK_1,	   spawn,	   {.v = audiosrc1cmd } },
	{ MODKEY|ShiftMask|ControlMask, XK_2,	   spawn,	   {.v = audiosrc2cmd } },
	{ MODKEY|ShiftMask|ControlMask, XK_3,	   spawn,	   {.v = audiosrc3cmd } },
	{ MODKEY|ShiftMask|ControlMask, XK_4, 	   spawn,	   {.v = audiosrc4cmd } },
	{ 0,                            XF86XK_MonBrightnessUp,  spawn, {.v = brupcmd } },
	{ 0,                            XF86XK_MonBrightnessDown, spawn, {.v = brdowncmd } },
	{ MODKEY|ControlMask,		XK_k,	   viewtoright,	   {0} },
	{ MODKEY|ControlMask,		XK_j,	   viewtoleft,	   {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ControlMask,           XK_Delete, quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
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

