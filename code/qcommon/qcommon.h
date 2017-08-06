/*
===========================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Spearmint Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following
the terms and conditions of the GNU General Public License.  If not, please
request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional
terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
// qcommon.h -- definitions common between client and server, but not game.or ref modules
#ifndef _QCOMMON_H_
#define _QCOMMON_H_

#include "../qcommon/cm_public.h"
#include "../renderercommon/tr_public.h"

// Engine name
#define PRODUCT_NAME				"Spearmint"

// Keep this in-sync with VERSION in Makefile.
#ifndef PRODUCT_VERSION
	#define PRODUCT_VERSION			"0.4"
#endif

#define Q3_VERSION PRODUCT_NAME " " PRODUCT_VERSION

// Settings directory name
#define HOMEPATH_NAME_UNIX			".spearmint"
#define HOMEPATH_NAME_WIN			"Spearmint"
#define HOMEPATH_NAME_MACOSX		HOMEPATH_NAME_WIN

// Steam installation information
//#define STEAMPATH_NAME			"Quake 3 Arena"
//#define STEAMPATH_APPID			"2200"

// Separates games in server browser. Must NOT contain whitespace (dpmaster will reject the game servers).
// Change this if not compatible with Spearmint games aka cannot play them (such as if you break VM compatibility).
#define GAMENAME_FOR_MASTER			"Spearmint"

// Game's engine settings for compatibility and other information needed before loading CGame VM.
// Probably don't need to change this, but if you break compatiblity feel free to give it a less stupid name.
#define GAMESETTINGS				"mint-game.settings"

// Prefix for game and cgame virtual machines. Example: vm/PREFIXcgame.qvm, PREFIXcgame_x86.dll
// Change this if you break VM API compatibility with Spearmint.
// You'll also need to change VM_PREFIX in game code Makefile.
#define VM_PREFIX					"mint-"

// Prefix for renderer native libraries. Example: PREFIXopengl1_x86.dll
// Change this if you break renderer compatibility with Spearmint.
// You'll also need to change RENDERER_PREFIX in Makefile.
#define RENDERER_PREFIX				"mint-renderer-"

// Default game to load (default fs_game value).
// You can change this and it won't break network compatiblity.
#ifndef BASEGAME
	#define BASEGAME				"baseq3"
#endif

// File containing a list of games to check if are installed to use as default base game.
// This overrides BASEGAME if a game is found. If none are found, uses BASEGAME.
#define MINT_GAMELIST					"mint-gamelist.txt"

// In the future if the client-server protocol is modified, this may allow old and new engines to play together
//#define LEGACY_PROTOCOL

// Heartbeat for dpmaster protocol. You shouldn't change this unless you know what you're doing
#define HEARTBEAT_FOR_MASTER		"DarkPlaces"
#define FLATLINE_FOR_MASTER			HEARTBEAT_FOR_MASTER

#define MAX_MASTER_SERVERS      5	// number of supported master servers

#define DEMOEXT	"mintdemo"			// standard demo extension

//============================================================================

//Ignore __attribute__ on non-gcc platforms
#ifndef __GNUC__
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif

//============================================================================

//
// msg.c
//
typedef struct {
	qboolean	allowoverflow;	// if false, do a Com_Error
	qboolean	overflowed;		// set to true if the buffer size failed (with allowoverflow set)
	qboolean	oob;			// set to true if the buffer size failed (with allowoverflow set)
	byte	*data;
	int		maxsize;
	int		cursize;
	int		readcount;
	int		bit;				// for bitwise reads and writes
} msg_t;

void MSG_Init (msg_t *buf, byte *data, int length);
void MSG_InitOOB( msg_t *buf, byte *data, int length );
void MSG_Clear (msg_t *buf);
void MSG_WriteData (msg_t *buf, const void *data, int length);
void MSG_Bitstream( msg_t *buf );

// TTimo
// copy a msg_t in case we need to store it as is for a bit
// (as I needed this to keep an msg_t from a static var for later use)
// sets data buffer as MSG_Init does prior to do the copy
void MSG_Copy(msg_t *buf, byte *data, int length, msg_t *src);

struct usercmd_s;
struct entityState_s;
struct playerState_s;

void MSG_WriteBits( msg_t *msg, int value, int bits );

void MSG_WriteChar (msg_t *sb, int c);
void MSG_WriteByte (msg_t *sb, int c);
void MSG_WriteShort (msg_t *sb, int c);
void MSG_WriteLong (msg_t *sb, int c);
void MSG_WriteFloat (msg_t *sb, float f);
void MSG_WriteString (msg_t *sb, const char *s);
void MSG_WriteBigString (msg_t *sb, const char *s);
void MSG_WriteAngle16 (msg_t *sb, float f);
int MSG_HashKey(const char *string, int maxlen);

void	MSG_BeginReading (msg_t *sb);
void	MSG_BeginReadingOOB(msg_t *sb);

int		MSG_ReadBits( msg_t *msg, int bits );

int		MSG_ReadChar (msg_t *sb);
int		MSG_ReadByte (msg_t *sb);
int		MSG_ReadShort (msg_t *sb);
int		MSG_ReadLong (msg_t *sb);
float	MSG_ReadFloat (msg_t *sb);
char	*MSG_ReadString (msg_t *sb);
char	*MSG_ReadBigString (msg_t *sb);
char	*MSG_ReadStringLine (msg_t *sb);
float	MSG_ReadAngle16 (msg_t *sb);
void	MSG_ReadData (msg_t *sb, void *buffer, int size);
int		MSG_LookaheadByte (msg_t *msg);

void MSG_WriteDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );
void MSG_ReadDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to );

void MSG_SetNetFields( vmNetField_t *vmEntityFields, int numEntityFields, int entityStateSize, int entityNetworkSize,
					   vmNetField_t *vmPlayerFields, int numPlayerFields, int playerStateSize, int playerNetworkSize );
void MSG_ShutdownNetFields( void );

void MSG_WriteDeltaEntity( msg_t *msg, sharedEntityState_t *from, sharedEntityState_t *to,
						   qboolean force );
void MSG_ReadDeltaEntity( msg_t *msg, sharedEntityState_t *from, sharedEntityState_t *to, 
						 int number );

void MSG_WriteDeltaPlayerstate( msg_t *msg, sharedPlayerState_t *from, sharedPlayerState_t *to );
void MSG_ReadDeltaPlayerstate( msg_t *msg, sharedPlayerState_t *from, sharedPlayerState_t *to, int number );


void MSG_ReportChangeVectors_f( void );

//============================================================================

/*
==============================================================

NET

==============================================================
*/

#define NET_ENABLEV4            0x01
#define NET_ENABLEV6            0x02
// if this flag is set, always attempt ipv6 connections instead of ipv4 if a v6 address is found.
#define NET_PRIOV6              0x04
// disables ipv6 multicast support if set.
#define NET_DISABLEMCAST        0x08


#define	PACKET_BACKUP	32	// number of old messages that must be kept on client and
							// server for delta comrpession and ping estimation
#define	PACKET_MASK		(PACKET_BACKUP-1)

#define	MAX_PACKET_USERCMDS		32		// max number of usercmd_t in a packet

#define	MAX_SNAPSHOT_ENTITIES	512

#define	PORT_ANY			-1

#define	MAX_RELIABLE_COMMANDS	(64*MAX_SPLITVIEW) // max string commands buffered for restransmit

typedef enum {
	NA_BAD = 0,					// an address lookup failed
	NA_BOT,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IP6,
	NA_MULTICAST6,
	NA_UNSPEC
} netadrtype_t;

typedef enum {
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

#define NET_ADDRSTRMAXLEN 48	// maximum length of an IPv6 address string including trailing '\0'
typedef struct {
	netadrtype_t	type;

	byte	ip[4];
	byte	ip6[16];

	unsigned short	port;
	unsigned long	scope_id;	// Needed for IPv6 link-local addresses
} netadr_t;

void		NET_Init( void );
void		NET_Shutdown( void );
void		NET_Restart_f( void );
void		NET_Config( qboolean enableNetworking );
void		NET_FlushPacketQueue(void);
void		NET_SendPacket (netsrc_t sock, int length, const void *data, netadr_t to);
void		QDECL NET_OutOfBandPrint( netsrc_t net_socket, netadr_t adr, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
void		QDECL NET_OutOfBandData( netsrc_t sock, netadr_t adr, byte *format, int len );

qboolean	NET_CompareAdr (netadr_t a, netadr_t b);
qboolean	NET_CompareBaseAdrMask(netadr_t a, netadr_t b, int netmask);
qboolean	NET_CompareBaseAdr (netadr_t a, netadr_t b);
qboolean	NET_IsLocalAddress (netadr_t adr);
const char	*NET_AdrToString (netadr_t a);
const char	*NET_AdrToStringwPort (netadr_t a);
int		NET_StringToAdr ( const char *s, netadr_t *a, netadrtype_t family);
qboolean	NET_GetLoopPacket (netsrc_t sock, netadr_t *net_from, msg_t *net_message);
void		NET_JoinMulticast6(void);
void		NET_LeaveMulticast6(void);
void		NET_Sleep(int msec);


#define	MAX_MSGLEN				32768		// max length of a message, which may
											// be fragmented into multiple packets

#define MAX_DOWNLOAD_WINDOW		48	// ACK window of 48 download chunks. Cannot set this higher, or clients
						// will overflow the reliable commands buffer
#define MAX_DOWNLOAD_BLKSIZE		1024	// 896 byte block chunks

#define NETCHAN_GENCHECKSUM(challenge, sequence) ((challenge) ^ ((sequence) * (challenge)))

/*
Netchan handles packet fragmentation and out of order / duplicate suppression
*/

typedef struct {
	netsrc_t	sock;

	int			dropped;			// between last packet and previous

	netadr_t	remoteAddress;
	int			qport;				// qport value to write when transmitting

	// sequencing variables
	int			incomingSequence;
	int			outgoingSequence;

	// incoming fragment assembly buffer
	int			fragmentSequence;
	int			fragmentLength;	
	byte		fragmentBuffer[MAX_MSGLEN];

	// outgoing fragment buffer
	// we need to space out the sending of large fragmented messages
	qboolean	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];

	int			challenge;
	int		lastSentTime;
	int		lastSentSize;

#ifdef LEGACY_PROTOCOL
	qboolean	compat;
#endif
} netchan_t;

void Netchan_Init( int qport );
void Netchan_Setup(netsrc_t sock, netchan_t *chan, netadr_t adr, int qport, int challenge, qboolean compat);

void Netchan_Transmit( netchan_t *chan, int length, const byte *data );
void Netchan_TransmitNextFragment( netchan_t *chan );

qboolean Netchan_Process( netchan_t *chan, msg_t *msg );


/*
==============================================================

PROTOCOL

==============================================================
*/

#define	PROTOCOL_VERSION	10
#define PROTOCOL_LEGACY_VERSION	0

// maintain a list of compatible protocols for demo playing
// NOTE: that stuff only works with two digits protocols
extern int demo_protocols[];

//#define	UPDATE_SERVER_NAME	"update.quake3arena.com"
// override on command line, config files etc.
#ifndef MASTER_SERVER_NAME
#define MASTER_SERVER_NAME	"dpmaster.deathmask.net"
#endif

#define	PORT_MASTER			27950
#define	PORT_UPDATE			27951
#define	PORT_SERVER			27960
#define	NUM_SERVER_PORTS	4		// broadcast scan this many ports after
									// PORT_SERVER so a single machine can
									// run multiple servers


// the svc_strings[] array in cl_parse.c should mirror this
//
// server to client
//
enum svc_ops_e {
	svc_bad,
	svc_nop,
	svc_gamestate,
	svc_configstring,			// [short] [string] only in gamestate messages
	svc_baseline,
	svc_serverCommand,			// [string] to be executed by client game module
	svc_download,				// [short] size [size bytes]
	svc_snapshot,
	svc_EOF,

// new commands, supported only by ioquake3 protocol but not legacy
	svc_voipSpeex,     // not wrapped in USE_VOIP, so this value is reserved.
	svc_voipOpus,      //
};


//
// client to server
//
enum clc_ops_e {
	clc_bad,
	clc_nop, 		
	clc_move,				// [[usercmd_t]
	clc_moveNoDelta,		// [[usercmd_t]
	clc_clientCommand,		// [string] message
	clc_EOF,

// new commands, supported only by ioquake3 protocol but not legacy
	clc_voipSpeex,   // not wrapped in USE_VOIP, so this value is reserved.
	clc_voipOpus,    //
};

/*
==============================================================

VIRTUAL MACHINE

==============================================================
*/

typedef struct vm_s vm_t;

typedef enum {
	VMI_NATIVE,
	VMI_BYTECODE,
	VMI_COMPILED
} vmInterpret_t;

typedef enum {
	TRAP_MEMSET = 0,
	TRAP_MEMCPY,
	TRAP_STRNCPY,
	TRAP_SIN,
	TRAP_COS,
	TRAP_ATAN2,
	TRAP_SQRT,
	TRAP_FLOOR,
	TRAP_CEIL,
	TRAP_ACOS,
	TRAP_ASIN,
	TRAP_TAN,
	TRAP_ATAN,
	TRAP_POW,
	TRAP_EXP,
	TRAP_LOG,
	TRAP_LOG10,
	TRAP_SYSCALL
} qvmTraps_t;

void	VM_Init( void );
vm_t	*VM_Create( const char *module, intptr_t (*systemCalls)(intptr_t *), 
				   vmInterpret_t interpret, int zoneTag, int heapRequestedSize );
// module should be bare: "cgame", not "cgame.dll" or "vm/cgame.qvm"

void	VM_Free( vm_t *vm );
void	VM_Clear(void);
void	VM_Forced_Unload_Start(void);
void	VM_Forced_Unload_Done(void);
vm_t	*VM_Restart(vm_t *vm, qboolean unpure);

intptr_t		QDECL VM_Call( vm_t *vm, int callNum, ... );
intptr_t		QDECL VM_SafeCall( vm_t *vm, int callnum );

void	VM_GetVersion( vm_t *vm, int nameCallNum, int versionCallNum, char *apiName, int apiNameSize, int *major, int *minor );

void	VM_Debug( int level );

void	*VM_ArgPtr( intptr_t intValue );
void	*VM_ExplicitArgPtr( vm_t *vm, intptr_t intValue );

#define	VMA(x) VM_ArgPtr(args[x])
#define	VMF(x)	IntAsFloat((int)args[x])

intptr_t VM_HeapMalloc( int size );
int VM_HeapAvailable( void );
void VM_HeapFree( void *data );

/*
==============================================================

CMD

Command text buffering and command execution

==============================================================
*/

/*

Any number of commands can be added in a frame, from several different sources.
Most commands come from either keybindings or console line input, but entire text
files can be execed.

*/

void Cbuf_Init (void);
// allocates an initial text buffer that will grow as needed

void Cbuf_AddText( const char *text );
// Adds command text at the end of the buffer, does NOT add a final \n

void Cbuf_ExecuteText( int exec_when, const char *text );
// this can be used in place of either Cbuf_AddText or Cbuf_InsertText

void Cbuf_ExecuteTextSafe( int exec_when, const char *text );
// used by VMs with special handling for unsafe calls.

void Cbuf_Execute (void);
// Pulls off \n terminated lines of text from the command buffer and sends
// them through Cmd_ExecuteString.  Stops when the buffer is empty.
// Normally called once per frame, but may be explicitly invoked.
// Do not call inside a command function, or current args will be destroyed.

//===========================================================================

/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

*/

typedef void (*xcommand_t) (void);

void	Cmd_Init (void);

void	Cmd_AddCommand( const char *cmd_name, xcommand_t function );
// called by the init functions of other parts of the program to
// register commands and functions to call for them.
// The cmd_name is referenced later, so it should not be in temp memory
// if function is NULL, the command will be forwarded to the server
// as a clc_clientCommand instead of executed locally

void	Cmd_RemoveCommand( const char *cmd_name );
void	Cmd_RemoveCommandsByFunc( xcommand_t function );

typedef void (*completionFunc_t)( char *args, int argNum );

// don't allow VMs to remove system commands
void	Cmd_AddCommandSafe( const char *cmd_name, xcommand_t function );
void	Cmd_RemoveCommandSafe( const char *cmd_name, xcommand_t function );

void	Cmd_CommandCompletion( void(*callback)(const char *s) );
// callback with each valid string
void Cmd_SetCommandCompletionFunc( const char *command,
	completionFunc_t complete );
void Cmd_CompleteArgument( const char *command, char *args, int argNum );
void Cmd_CompleteCfgName( char *args, int argNum );

int		Cmd_Argc (void);
char	*Cmd_Argv (int arg);
void	Cmd_ArgvBuffer( int arg, char *buffer, int bufferLength );
char	*Cmd_Args (void);
char	*Cmd_ArgsFrom( int arg );
void	Cmd_ArgsBuffer( char *buffer, int bufferLength );
void	Cmd_LiteralArgsBuffer( char *buffer, int bufferLength );
char	*Cmd_Cmd (void);
void	Cmd_Args_Sanitize( void );
// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are allways safe.

void	Cmd_TokenizeString( const char *text );
void	Cmd_TokenizeStringIgnoreQuotes( const char *text_in );
// Takes a null terminated string.  Does not need to be /n terminated.
// breaks the string up into arg tokens.

void	Cmd_ExecuteString( const char *text );
// Parses a single line of text into arguments and tries to execute it
// as if it was typed at the console

void	Cmd_SaveCmdContext( void );
void	Cmd_RestoreCmdContext( void );

/*
==============================================================

CVAR

==============================================================
*/

/*

cvar_t variables are used to hold scalar or string variables that can be changed
or displayed at the console or prog code as well as accessed directly
in C code.

The user can access cvars from the console in three ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
set r_draworder 0	as above, but creates the cvar if not present

Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.

The are also occasionally used to communicated information between different
modules of the program.

*/

cvar_t *Cvar_Get( const char *var_name, const char *value, int flags );
// creates the variable if it doesn't exist, or returns the existing one
// if it exists, the value will not be changed, but flags will be ORed in
// that allows variables to be unarchived without needing bitflags
// if value is "", the value will not override a previously set value.

void	Cvar_Register( vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags );
// basically a slightly modified Cvar_Get for the interpreted modules

void	Cvar_Update( vmCvar_t *vmCvar );
// updates an interpreted modules' version of a cvar

cvar_t *Cvar_SetDefault( const char *var_name, const char *value );
// if cvar exists, change the default value of the cvar. Otherwise, create using Cvar_Get.

cvar_t *Cvar_Set2( const char *var_name, const char *value, int defaultFlags, qboolean force );
//

cvar_t	*Cvar_Set( const char *var_name, const char *value );
// will create the variable with no flags if it doesn't exist

cvar_t	*Cvar_SetSafe( const char *var_name, const char *value);
// same as Cvar_Set, but doesn't force setting the value (respects CVAR_ROM, etc)

cvar_t	*Cvar_User_Set( const char *var_name, const char *value );
// same as Cvar_SetSafe, but defaults to CVAR_USER_CREATED

void	Cvar_VM_Set( const char *var_name, const char *value, const char *vmName );
void	Cvar_Server_Set( const char *var_name, const char *value );
// sometimes we set variables from an untrusted source: fail if flags & CVAR_PROTECTED

cvar_t	*Cvar_SetValue( const char *var_name, float value );
void	Cvar_VM_SetValue( const char *var_name, float value, const char *vmName );
// expands value to a string and calls Cvar_Set/Cvar_VM_Set

cvar_t *Cvar_Unset(cvar_t *cv);
// remove a cvar, returns next cvar in linked list

float	Cvar_VariableValue( const char *var_name );
int		Cvar_VariableIntegerValue( const char *var_name );
// returns 0 if not defined or non numeric

char	*Cvar_VariableString( const char *var_name );
void	Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
// returns an empty string if not defined
void	Cvar_LatchedVariableStringBuffer( const char *var_name, char *buffer, int bufsize );
// returns the latched value if there is one, else the normal one, empty string if not defined

int	Cvar_Flags(const char *var_name);
// returns CVAR_NONEXISTENT if cvar doesn't exist or the flags of that particular CVAR.

void	Cvar_CommandCompletion( void(*callback)(const char *s) );
// callback with each valid string

void 	Cvar_Reset( const char *var_name );
void 	Cvar_ForceReset(const char *var_name);

void	Cvar_SetCheatState( void );
// reset all testing vars to a safe value

qboolean Cvar_Command( void );
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)

void 	Cvar_WriteVariables( fileHandle_t f );
// writes lines containing "set variable value" for all variables
// with the archive flag set to true.

void	Cvar_Init( void );

char	*Cvar_InfoString( int bit );
char	*Cvar_InfoString_Big( int bit );
// returns an info string containing all the cvars that have the given bit set
// in their flags ( CVAR_USERINFO, CVAR_SERVERINFO, CVAR_SYSTEMINFO, etc )
void	Cvar_InfoStringBuffer( int bit, char *buff, int buffsize );
void Cvar_CheckRange( cvar_t *cv, float minVal, float maxVal, qboolean shouldBeIntegral );
void Cvar_SetDescription( cvar_t *var, const char *var_description );

void Cvar_CheckRangeSafe( const char *varName, float min, float max, qboolean integral );
// basically a slightly modified Cvar_CheckRange for the interpreted modules

void	Cvar_Restart(qboolean unsetVM);
void	Cvar_Restart_f( void );

void	Cvar_ResetDefaultOverrides( void );

void Cvar_CompleteCvarName( char *args, int argNum );

extern	int			cvar_modifiedFlags;
// whenever a cvar is modifed, its flags will be OR'd into this, so
// a single check can determine if any CVAR_USERINFO, CVAR_SERVERINFO,
// etc, variables have been modified since the last check.  The bit
// can then be cleared to allow another change detection.

/*
==============================================================

FILESYSTEM

No stdio calls should be used by any part of the game, because
we need to deal with all sorts of directory and seperator char
issues.
==============================================================
*/

typedef enum {
	PAK_UNKNOWN,
	PAK_FREE,
	PAK_NO_DOWNLOAD,
	PAK_COMMERCIAL,
	PAK_MAX
} pakType_t;

#define	MAX_FILE_HANDLES	64

#ifdef DEDICATED
#	define Q3CONFIG_CFG "config_server.cfg"
#else
#	define Q3CONFIG_CFG "config.cfg"
#endif

qboolean FS_Initialized( void );

void	FS_InitFilesystem ( void );
void	FS_Shutdown( qboolean closemfp );

qboolean FS_ConditionalRestart(qboolean disconnect);
void	FS_Restart( qboolean gameDirChanged );
// shutdown and restart the filesystem so changes to fs_gamedir can take effect

void	FS_GameValid( void );
qboolean FS_TryLastValidGame( void );

void FS_AddGameDirectory( const char *path, const char *dir );

char	**FS_ListFiles( const char *directory, const char *extension, int *numfiles );
// directory should not have either a leading or trailing /
// if extension is "/", only subdirectories will be returned
// the returned files will not include any directories or /

void	FS_FreeFileList( char **list );

qboolean FS_FileExists( const char *file );
qboolean FS_FileInPathExists(const char *testpath);

qboolean FS_CreatePath (char *OSPath);

int FS_PathCmp( const char *s1, const char *s2 );

int FS_FindVM(void **startSearch, char *found, int foundlen, const char *name, int enableDll);

char   *FS_BuildOSPath( const char *base, const char *game, const char *qpath );
qboolean FS_CompareZipChecksum(const char *zipfile);

int		FS_LoadStack( void );

int		FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize );
int		FS_GetModList(  char *listbuf, int bufsize );

void	FS_GetModDescription( const char *modDir, char *description, int descriptionLen );

fileHandle_t	FS_FOpenFileWrite( const char *qpath );
fileHandle_t	FS_FOpenFileAppend( const char *filename );
fileHandle_t	FS_FCreateOpenPipeFile( const char *filename );
// will properly create any needed paths and deal with seperater character issues

qboolean	FS_SV_FileExists( const char *filename );
qboolean	FS_SV_RW_FileExists( const char *filename );
fileHandle_t FS_SV_FOpenFileWrite( const char *filename );
long		FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp );
void	FS_SV_Rename( const char *from, const char *to, qboolean safe );
long		FS_FOpenFileRead( const char *qpath, fileHandle_t *file, qboolean uniqueFILE );
// if uniqueFILE is true, then a new FILE will be fopened even if the file
// is found in an already open pak file.  If uniqueFILE is false, you must call
// FS_FCloseFile instead of fclose, otherwise the pak FILE would be improperly closed
// It is generally safe to always set uniqueFILE to true, because the majority of
// file IO goes through FS_ReadFile, which Does The Right Thing already.

long FS_System_FOpenFileRead(const char *ospath, fileHandle_t *fp);

int     FS_Delete( char *filename );    // only works inside the 'save' directory (for deleting savegames/images)

int		FS_Write( const void *buffer, int len, fileHandle_t f );

int		FS_Read( void *buffer, int len, fileHandle_t f );
// properly handles partial reads and reads from other dlls

void	FS_FCloseFile( fileHandle_t f );
// note: you can't just fclose from another DLL, due to MS libc issues

long	FS_ReadFileDir(const char *qpath, void *searchPath, qboolean unpure, void **buffer);
long	FS_ReadFile(const char *qpath, void **buffer);
// returns the length of the file
// a null buffer will just return the file length without loading
// as a quick check for existance. -1 length == not present
// A 0 byte will always be appended at the end, so string ops are safe.
// the buffer should be considered read-only, because it may be cached
// for other uses.

void	FS_ForceFlush( fileHandle_t f );
// forces flush on files we're writing to.

void	FS_FreeFile( void *buffer );
// frees the memory returned by FS_ReadFile

void	FS_WriteFile( const char *qpath, const void *buffer, int size );
// writes a complete file, creating any subdirectories needed

long FS_filelength(fileHandle_t f);
// doesn't work for files that are opened from a pack file

int		FS_FTell( fileHandle_t f );
// where are we?

void	FS_Flush( fileHandle_t f );

void 	QDECL FS_Printf( fileHandle_t f, const char *fmt, ... ) __attribute__ ((format (printf, 2, 3)));
// like fprintf

int		FS_FOpenFileByMode( const char *qpath, fileHandle_t *f, fsMode_t mode );
// opens a file for reading, writing, or appending depending on the value of mode

int		FS_Seek( fileHandle_t f, long offset, int origin );
// seek on a file

qboolean FS_FilenameCompare( const char *s1, const char *s2 );

const char *FS_LoadedPakNames( void );
const char *FS_LoadedPakChecksums( void );
// Returns a space separated string containing the checksums of all loaded pk3 files.
// Servers with sv_pure set will get this string and pass it to clients.

int			FS_ReferencedPakChecksum( int n );
const char *FS_ReferencedPakNames( void );
const char *FS_ReferencedPakChecksums( void );
// Returns a space separated string containing the checksums of all referenced pk3 files.
// The server will send this to the clients so they can check which files should be auto-downloaded.

void FS_PureServerSetReferencedPaks( const char *pakSums, const char *pakNames );
void FS_PureServerSetLoadedPaks( const char *pakSums, const char *pakNames );
// If the string is empty, all data sources will be allowed.
// If not empty, only pk3 files that match one of the space
// separated checksums will be checked for files, with the
// sole exception of .cfg files.

qboolean FS_CheckDirTraversal(const char *checkdir);
pakType_t FS_ReferencedPakType( const char *name, int checksum, qboolean *installed );
qboolean FS_ComparePaks( char *neededpaks, int len, qboolean dlstring );

qboolean FS_Rename( const char *from, const char *to );

int FS_Remove( const char *osPath );
int FS_HomeRemove( const char *homePath );

void	FS_FilenameCompletion( const char *dir, const char *ext,
		qboolean stripExt, void(*callback)(const char *s), qboolean allowNonPureFilesOnDisk );

const char *FS_GetCurrentGameDir(void);
qboolean FS_Which(const char *filename, void *searchPath);

/*
==============================================================

Game config, loaded from mint-game.settings (see GAMESETTINGS define)

==============================================================
*/

#define MAX_GAMEDIRS 16 // max gamedirs a mod can have
#define MAX_LOADINGSCREENS	200

typedef struct loadingScreen_s {
	char	shaderName[MAX_QPATH];
	float	aspect;
	vec3_t	color;
} loadingScreen_t;

typedef struct {
	char	gameDirs[MAX_GAMEDIRS][MAX_QPATH];
	int		numGameDirs;

#ifndef DEDICATED
	char	defaultSound[MAX_QPATH];

	loadingScreen_t	loadingScreens[MAX_LOADINGSCREENS];
	int			numLoadingScreens;
#endif
} gameConfig_t;

extern gameConfig_t com_gameConfig;

/*
==============================================================

Edit fields and command line history/completion

==============================================================
*/

#define	MAX_EDIT_LINE	256
typedef struct {
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
} field_t;

void Field_Clear( field_t *edit );
void Field_AutoComplete( field_t *edit );
void Field_CompleteKeyname( void );
void Field_CompleteFilename( const char *dir,
		const char *ext, qboolean stripExt, qboolean allowNonPureFilesOnDisk );
void Field_CompleteCommand( char *cmd,
		qboolean doCommands, qboolean doCvars );

/*
==============================================================

MISC

==============================================================
*/

// returned by Sys_GetProcessorFeatures
typedef enum
{
  CF_RDTSC      = 1 << 0,
  CF_MMX        = 1 << 1,
  CF_MMX_EXT    = 1 << 2,
  CF_3DNOW      = 1 << 3,
  CF_3DNOW_EXT  = 1 << 4,
  CF_SSE        = 1 << 5,
  CF_SSE2       = 1 << 6,
  CF_ALTIVEC    = 1 << 7
} cpuFeatures_t;

// centralized and cleaned, that's the max string you can send to a Com_Printf / Com_DPrintf (above gets truncated)
#define	MAXPRINTMSG	4096


typedef enum {
	// SE_NONE must be zero
	SE_NONE = 0,		// evTime is still valid
	SE_KEY,			// evValue is a key code, evValue2 is the down flag
	SE_CHAR,		// evValue is an ascii char
	SE_MOUSE,		// evValue and evValue2 are relative signed x / y moves
	SE_MOUSE_LAST = SE_MOUSE + MAX_SPLITVIEW - 1, // Reserve values for SE_MOUSE events for splitscreen players
	SE_JOYSTICK_AXIS,	// evValue is an axis number and evValue2 is the current state (-127 to 127)
	SE_JOYSTICK_AXIS_LAST = SE_JOYSTICK_AXIS + MAX_SPLITVIEW - 1, // Reserve values for SE_JOYSTICK_AXIS events for splitscreen players
	SE_JOYSTICK_BUTTON,	// evValue is a button number and evValue2 is the down flag
	SE_JOYSTICK_BUTTON_LAST = SE_JOYSTICK_BUTTON + MAX_SPLITVIEW - 1, // Reserve values for SE_JOYSTICK_BUTTON events for splitscreen players
	SE_JOYSTICK_HAT,	// evValue is a hat number and evValue2 is the state
	SE_JOYSTICK_HAT_LAST = SE_JOYSTICK_HAT + MAX_SPLITVIEW - 1, // Reserve values for SE_JOYSTICK_HAT events for splitscreen players
	SE_CONSOLE		// evPtr is a char*
} sysEventType_t;

typedef struct {
	int				evTime;
	sysEventType_t	evType;
	int				evValue, evValue2;
	int				evPtrLength;	// bytes of data pointed to by evPtr, for journaling
	void			*evPtr;			// this must be manually freed if not NULL
} sysEvent_t;

void		Com_QueueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr );
int			Com_EventLoop( void );
sysEvent_t	Com_GetSystemEvent( void );

char		*CopyString( const char *in );
void		Info_Print( const char *s );

void		Com_BeginRedirect (char *buffer, int buffersize, void (*flush)(char *));
void		Com_EndRedirect( void );
void 		QDECL Com_Printf( const char *fmt, ... ) __attribute__ ((format (printf, 1, 2)));
void 		QDECL Com_DPrintf( const char *fmt, ... ) __attribute__ ((format (printf, 1, 2)));
void 		QDECL Com_Error( int code, const char *fmt, ... ) __attribute__ ((noreturn, format(printf, 2, 3)));
void 		Com_Quit_f( void ) __attribute__ ((noreturn));
void		Com_GameRestart(qboolean disconnect);
void		Com_ExecuteCfg(void);

int			Com_Milliseconds( void );	// will be journaled properly
unsigned	Com_BlockChecksum( const void *buffer, int length );
char		*Com_MD5File(const char *filename, int length, const char *prefix, int prefix_len);
int			Com_Filter(char *filter, char *name, int casesensitive);
int			Com_FilterPath(char *filter, char *name, int casesensitive);
int			Com_RealTime(qtime_t *qtime);
qboolean	Com_SafeMode( void );
void		Com_RunAndTimeServerPacket(netadr_t *evFrom, msg_t *buf);

qboolean	Com_IsVoipTarget(uint8_t *voipTargets, int voipTargetsSize, int playerNum);

qboolean	Com_GameIsSinglePlayer(void);

void		Com_StartupVariable( const char *match );
// checks for and removes command line "+set var arg" constructs
// if match is NULL, all set commands will be executed, otherwise
// only a set with the exact name.  Only used during startup.


extern	cvar_t	*com_fs_pure;
extern	cvar_t	*com_developer;
extern	cvar_t	*com_dedicated;
extern	cvar_t	*com_speeds;
extern	cvar_t	*com_timescale;
extern	cvar_t	*com_sv_running;
extern	cvar_t	*com_cl_running;
extern	cvar_t	*com_version;
extern	cvar_t	*com_singlePlayerActive;
extern	cvar_t	*com_buildScript;		// for building release pak files
extern	cvar_t	*com_journal;
extern	cvar_t	*com_cameraMode;
extern	cvar_t	*com_ansiColor;
extern	cvar_t	*com_unfocused;
extern	cvar_t	*com_maxfpsUnfocused;
extern	cvar_t	*com_minimized;
extern	cvar_t	*com_maxfpsMinimized;
#if idppc_altivec
extern	cvar_t	*com_altivec;
#endif
extern	cvar_t	*com_homepath;

// both client and server must agree to pause
extern	cvar_t	*cl_paused;
extern	cvar_t	*sv_paused;

extern	cvar_t	*cl_packetdelay;
extern	cvar_t	*sv_packetdelay;

extern	cvar_t	*com_productName;
extern	cvar_t	*com_gamename;
extern	cvar_t	*com_protocol;
#ifdef LEGACY_PROTOCOL
extern	cvar_t	*com_legacyprotocol;
#endif
#ifndef DEDICATED
extern  cvar_t  *con_autochat;
#endif

#ifdef USE_RENDERER_DLOPEN
extern	cvar_t	*com_renderer;
#endif

#ifndef DEDICATED
extern	cvar_t	*con_autochat;
#endif

// com_speeds times
extern	int		time_game;
extern	int		time_frontend;
extern	int		time_backend;		// renderer backend time

extern	int		com_frameTime;

extern	int			com_errorEntered;
extern	qboolean	com_fullyInitialized;

extern	int		com_playVideo;

extern	fileHandle_t	com_journalFile;
extern	fileHandle_t	com_journalDataFile;

typedef enum {
	TAG_FREE,
	TAG_GENERAL,
	TAG_BOTLIB,
	TAG_RENDERER,
	TAG_SMALL,
	TAG_STATIC,
	TAG_GAME,
	TAG_CGAME
} memtag_t;

/*

--- low memory ----
server vm
server clipmap
---mark---
renderer initialization (shaders, etc)
cgame vm
renderer map
renderer models

---free---

temp file loading
--- high memory ---

*/

#ifdef ZONE_DEBUG
#define Z_TagMalloc(size, tag)			Z_TagMallocDebug(size, tag, #size, __FILE__, __LINE__)
#define Z_Malloc(size)					Z_MallocDebug(size, #size, __FILE__, __LINE__)
#define S_Malloc(size)					S_MallocDebug(size, #size, __FILE__, __LINE__)
#define Z_Free(ptr)						Z_FreeDebug(ptr, #ptr, __FILE__, __LINE__)
void *Z_TagMallocDebug( int size, int tag, char *label, char *file, int line );	// NOT 0 filled memory
void *Z_MallocDebug( int size, char *label, char *file, int line );			// returns 0 filled memory
void *S_MallocDebug( int size, char *label, char *file, int line );			// returns 0 filled memory
void Z_FreeDebug( void *ptr, char *label, char *file, int line  );
#else
void *Z_TagMalloc( int size, int tag );	// NOT 0 filled memory
void *Z_Malloc( int size );			// returns 0 filled memory
void *S_Malloc( int size );			// NOT 0 filled memory only for small allocations
void Z_Free( void *ptr );
#endif
int Z_FreeTags( int tag );
int Z_AvailableMemory( void );
void Z_LogHeap( void );

void Hunk_Clear( void );
void Hunk_ClearToMark( void );
void Hunk_SetMark( void );
qboolean Hunk_CheckMark( void );
void Hunk_ClearTempMemory( void );
void *Hunk_AllocateTempMemory( int size );
void Hunk_FreeTempMemory( void *buf );
int	Hunk_MemoryRemaining( void );
void Hunk_Log( void);

typedef struct {
	void *pointer;
	int maxElements;
	int elementLength;
	qboolean freeable;
} darray_t;

#ifdef ZONE_DEBUG
#define DA_Init(_darray,_maxElements,_elementLength,_freeable) DA_InitDebug(_darray,_maxElements,_elementLength,_freeable,__FILE__,__LINE__)
#define DA_Free( darray ) DA_FreeDebug( darray, __FILE__, __LINE__ )
void DA_InitDebug( darray_t *darray, int maxElements, int elementLength, qboolean freeable, char *file, int line );
void DA_FreeDebug( darray_t *darray, char *file, int line );
#else
void DA_Init( darray_t *darray, int maxElements, int elementLength, qboolean freeable );
void DA_Free( darray_t *darray );
#endif
void DA_Clear( darray_t *darray );
void DA_Copy( const darray_t in, darray_t *out );

void DA_ClearElement( darray_t *darray, int num );
void DA_SetElement( darray_t *darray, int num, const void *data );
void DA_GetElement( const darray_t darray, int num, void *data );
void *DA_ElementPointer( const darray_t darray, int num );

void Com_TouchMemory( void );

void Z_VM_InitHeap( int tag, void *preallocated, int size );
int Z_VM_HeapAvailable( int tag );
void Z_VM_ShutdownHeap( int tag );

// commandLine should not include the executable name (argv[0])
void Com_Init( char *commandLine );
void Com_Frame( void );
void Com_Shutdown( void );

/*
==============================================================

REFRESH DLL

==============================================================
*/

extern	refexport_t		re;		// interface to refresh .dll

void Com_ShutdownRef( void );
void Com_InitRef( refimport_t *ri );

/*
==============================================================

CLIENT / SERVER SYSTEMS

==============================================================
*/

//
// client interface
//
void CL_InitKeyCommands( void );
// the keyboard binding interface must be setup before execing
// config files, but the rest of client startup will happen later

void CL_InitJoyRemapCommands( void );

void CL_Init( void );
void CL_Disconnect( qboolean showMainMenu );
void CL_Shutdown(char *finalmsg, qboolean disconnect, qboolean quit);
void CL_Frame( int msec );
void CL_GameConsoleText( qboolean restoredText );
void CL_KeyEvent (int key, qboolean down, unsigned time);

void CL_CharEvent( int character );
// char events are for field typing, not game control

void CL_MouseEvent( int localPlayerNum, int dx, int dy, int time );

void CL_JoystickAxisEvent( int localPlayerNum, int axis, int value, unsigned time );
void CL_JoystickButtonEvent( int localPlayerNum, int button, qboolean down, unsigned time );
void CL_JoystickHatEvent( int localPlayerNum, int hat, int state, unsigned time );

void CL_PacketEvent( netadr_t from, msg_t *msg );

void CL_ConsolePrint( char *text );

void CL_MapLoading( void );
// do a screen update before starting to load a map
// when the server is going to load a new map, the entire hunk
// will be cleared, so the client must shutdown cgame, ui, and
// the renderer

void CL_FlushMemory( void );
// dump all memory on an error

void CL_ShutdownAll(qboolean shutdownRef);
// shutdown client

void CL_StartHunkUsers( qboolean rendererOnly );
// start all the client stuff using the hunk

qboolean CL_ConnectedToRemoteServer( void );
// returns qtrue if connected to a remote server

void CL_MissingDefaultCfg( const char *gamedir );
// connected to a remote server and is missing default.cfg for new fs_game

void Key_KeynameCompletion( void(*callback)(const char *s) );
// for keyname autocompletion

void Key_WriteBindings( fileHandle_t f );
// for writing the config files

void S_ClearSoundBuffer( void );
// call before filesystem access

void SCR_DebugGraph (float value);	// FIXME: move logging to common?

// AVI files have the start of pixel lines 4 byte-aligned
#define AVI_LINE_PADDING 4

//
// server interface
//
void SV_Init( void );
void SV_Shutdown( char *finalmsg );
void SV_Frame( int msec );
void SV_PacketEvent( netadr_t from, msg_t *msg );
int SV_FrameMsec(void);
int SV_SendQueuedPackets(void);

/*
==============================================================

NON-PORTABLE SYSTEM SERVICES

==============================================================
*/

void	Sys_Init (void);

// general development dll loading for virtual machine testing
void	* QDECL Sys_LoadGameDll( const char *name, intptr_t (QDECL **entryPoint)(int, ...),
				  intptr_t (QDECL *systemcalls)(intptr_t, ...) );
void	Sys_UnloadDll( void *dllHandle );

qboolean Sys_DllExtension( const char *name );

char	*Sys_GetCurrentUser( void );

void	QDECL Sys_Error( const char *error, ...) __attribute__ ((noreturn, format (printf, 1, 2)));
void	Sys_Quit (void) __attribute__ ((noreturn));
char	*Sys_GetClipboardData( void );	// note that this isn't journaled...
qboolean Sys_GetCapsLockMode( void );
qboolean Sys_GetNumLockMode( void );

void	Sys_Print( const char *msg );

// Sys_Milliseconds should only be used for profiling purposes,
// any game related timing information should come from event timestamps
int		Sys_Milliseconds (void);

qboolean Sys_RandomBytes( byte *string, int len );

// the system console is shown when a dedicated server is running
void	Sys_DisplaySystemConsole( qboolean show );

cpuFeatures_t Sys_GetProcessorFeatures( void );

void	Sys_SetErrorText( const char *text );

void	Sys_SendPacket( int length, const void *data, netadr_t to );

qboolean	Sys_StringToAdr( const char *s, netadr_t *a, netadrtype_t family );
//Does NOT parse port numbers, only base addresses.

qboolean	Sys_IsLANAddress (netadr_t adr);
void		Sys_ShowIP(void);

FILE	*Sys_FOpen( const char *ospath, const char *mode );
qboolean Sys_Mkdir( const char *path );
qboolean Sys_Rmdir( const char *path );
FILE	*Sys_Mkfifo( const char *ospath );
int		Sys_StatFile( char *ospath );
char	*Sys_Cwd( void );
void	Sys_SetDefaultInstallPath(const char *path);
char	*Sys_DefaultInstallPath(void);
char	*Sys_SteamPath(void);
char	*Sys_GogPath(void);

#ifdef __APPLE__
char    *Sys_DefaultAppPath(void);
#endif

void  Sys_SetDefaultHomePath(const char *path);
char	*Sys_DefaultHomePath(void);
const char *Sys_Dirname( char *path );
const char *Sys_Basename( char *path );
char *Sys_ConsoleInput(void);

char **Sys_ListFiles( const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs );
void	Sys_FreeFileList( char **list );
void	Sys_Sleep(int msec);

qboolean Sys_LowPhysicalMemory( void );

void Sys_SetEnv(const char *name, const char *value);

typedef enum
{
	DR_YES = 0,
	DR_NO = 1,
	DR_OK = 0,
	DR_CANCEL = 1
} dialogResult_t;

typedef enum
{
	DT_INFO,
	DT_WARNING,
	DT_ERROR,
	DT_YES_NO,
	DT_OK_CANCEL
} dialogType_t;

dialogResult_t Sys_Dialog( dialogType_t type, const char *message, const char *title );

void Sys_RemovePIDFile( const char *gamedir );
void Sys_InitPIDFile( const char *gamedir );

/*
==============================================================

CONSOLE LOG ACCESS

==============================================================
*/

unsigned int CON_LogRead( char *out, unsigned int outSize );
void CON_LogSaveReadPos( void );
void CON_LogRestoreReadPos( void );


/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */

#define NYT HMAX					/* NYT = Not Yet Transmitted */
#define INTERNAL_NODE (HMAX+1)

typedef struct nodetype {
	struct	nodetype *left, *right, *parent; /* tree structure */ 
	struct	nodetype *next, *prev; /* doubly-linked list */
	struct	nodetype **head; /* highest ranked node in block */
	int		weight;
	int		symbol;
} node_t;

#define HMAX 256 /* Maximum symbol */

typedef struct {
	int			blocNode;
	int			blocPtrs;

	node_t*		tree;
	node_t*		lhead;
	node_t*		ltail;
	node_t*		loc[HMAX+1];
	node_t**	freelist;

	node_t		nodeList[768];
	node_t*		nodePtrs[768];
} huff_t;

typedef struct {
	huff_t		compressor;
	huff_t		decompressor;
} huffman_t;

void	Huff_Compress(msg_t *buf, int offset);
void	Huff_Decompress(msg_t *buf, int offset);
void	Huff_Init(huffman_t *huff);
void	Huff_addRef(huff_t* huff, byte ch);
int		Huff_Receive (node_t *node, int *ch, byte *fin);
void	Huff_transmit (huff_t *huff, int ch, byte *fout, int maxoffset);
void	Huff_offsetReceive (node_t *node, int *ch, byte *fin, int *offset, int maxoffset);
void	Huff_offsetTransmit (huff_t *huff, int ch, byte *fout, int *offset, int maxoffset);
void	Huff_putBit( int bit, byte *fout, int *offset);
int		Huff_getBit( byte *fout, int *offset);

// don't use if you don't know what you're doing.
int		Huff_getBloc(void);
void	Huff_setBloc(int _bloc);


extern huffman_t clientHuffTables;

#define	SV_ENCODE_START		4
#define SV_DECODE_START		12
#define	CL_ENCODE_START		12
#define CL_DECODE_START		4

// flags for sv_allowDownload and cl_allowDownload
#define DLF_ENABLE 1
#define DLF_NO_REDIRECT 2
#define DLF_NO_UDP 4
#define DLF_NO_DISCONNECT 8

#endif // _QCOMMON_H_
