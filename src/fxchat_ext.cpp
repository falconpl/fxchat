/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_ext.cpp

   Falcon script Xchat plugin
   Interface extension functions
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin
   Interface extension functions
*/

#include <falcon/engine.h>
#include <falcon/transcoding.h>
#include <falcon/rosstream.h>
#include <falcon/lineardict.h>

#include <string.h>
#include <ctype.h>

#include "fxchat.h"
#include "fxchat_script.h"
#include "fxchat_errhand.h"
#include "fxchat_hook.h"
#include "fxchat_ext.h"
#include "fxchat_events.h"
#include "fxchat_vm.h"

#include "version.h"

namespace Falcon {

namespace Ext {

static char *stringize_dyn( VMachine *vm, const Item &itm )
{
   String *str;
   String tempStr;

   if ( itm.isString() )
   {
      str = itm.asString();
   }
   else {
      str = &tempStr;
      vm->itemToString( tempStr, &itm );
   }

   uint32 len = str->length() * 4 + 4;
   char *ret = new char[ len ];
   return ret;
}

/*#
   @object XChat
   @brief Main interface to XChat scripting.

   This object gives access to the XChat API. It provides
   informations, callback hook subscription and means to send direct commands
   to the host XChat applications.
*/

/*#
   @method command XChat
   @brief Sends a command to XChat.
   @param cmd A Complete command as entered on the XChat command line.

   This method sends a command to the XChat application engine as if it
   was digited by the user on the currently active context.

   The if the @b cmd parameter is not a string, it will be automatically
   converted by the Virtual Machine. This means that the toString FBOM method
   will be applied.

   @note Do not use a leading "/" before the command. The string is already
   interpreted as a command, and it doesn't require an extra "/".

   @note Through the @b command method it is possible to send menu and other GUI
   commands to the XChat engine. Please, refer to the XChat plugin programming manual
   for a list of GUI commands that are avaliable to plugins, and also to scripts.
*/
FALCON_FUNC  XChat_command( ::Falcon::VMachine *vm )
{
   Item *i_param = vm->param( 0 );

   if ( i_param == 0 )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "X" ) );
      return;
   }

   AutoCString ret( vm, *i_param );
   xchat_command( the_plugin, ret );
}

/*#
   @method message XChat
   @brief Sends a message to a channel or a nick name.
   @param target Target channel or nick.
   @param msg Message to be sent to the target.

   This method sends a message for the specified target, which may
   be any valid IRC target. If the @b target parameter is prefixed with
   "#" or "&", the, then the message will be directed to the named channel,
   otheriwse it will be directed to the given nick.

   Both @b target and @b msg parameters must be strings.

   @note Actually, this command is just a shortcut to the "PRIVMSG $target :$msg"
   command.
*/
FALCON_FUNC  XChat_message( ::Falcon::VMachine *vm )
{
   char tempc[2048];

   Item *i_channel = vm->param( 0 );
   Item *i_message = vm->param( 1 );

   if ( i_channel == 0 || ! i_channel->isString() ||
      i_message == 0 || ! i_message->isString() )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "S,S" ) );
      return;
   }

   String temp = "PRIVMSG " + *i_channel->asString() + " : " + *i_message->asString();
   AutoCString ret( vm, &temp );
   xchat_command( the_plugin, ret.c_str() );
}

/*#
   @method emit XChat
   @brief Generates an XChat print event.
   @param evt A string containing a print event.
   @optparam ... Print event parameters (max 10).

   Generates a print event.
   This can be any event found in the Preferences > Lists > Text Events window.

   The FXchat plugin generates a list of print event through the command
   @code
   /FALCON HELP EVENTS
   @endcode

   Each event has zero or more parameters; the parameters passed to this method after the
   required print event name should be in the same number and order as explained in the
   text events window or as returned by the HELP EVENTS falcon command.

   If the given parameters are not strings, they will be converted to strings by the
   virtual machine through the @b toString() basic object method.
*/
FALCON_FUNC  XChat_emit( ::Falcon::VMachine *vm )
{
   if ( vm->paramCount() == 0 )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "X, ..." ) );
      return;
   }

   char *args[11];
   int count = vm->paramCount() >= 10 ? 10 : vm->paramCount();

   for( int i = 0; i < count; i++ )
   {
      args[i] = stringize_dyn( vm, *vm->param( i ) );

      // oops, conversion error?
      if ( args[i] == 0 )
      {
         for( int j = 0; j < i; j++ )
         {
            delete[] args[j];
         }
         return;
      }

   }
   args[ count ] = 0;

   bool val = xchat_emit_print( the_plugin,
      args[0], args[1], args[2], args[3], args[4],
      args[5], args[6], args[7], args[8], args[9],
      args[10] ) == 1;

   for( int i = 0; i < count; i++ )
   {
      delete[] args[i];
   }

   vm->retval( (int64) (val ? 1: 0) );
}

/*#
   @method sendModes XChat
   @brief Send a mode change request to the current channel.
   @param targets A list of target nicks (or a single target).
   @param sign A mode sign: "+" to add, "-" to remove.
   @param mode The mode to be added or removed.
   @optparam modes_per_line Maximum modes to send per line.

   Sends a number of channel mode changes to the current channel.
   For example, you can Op a whole group of people in one go.
   It may send multiple MODE lines if the request doesn't fit on one.
   Pass 0 for modes_per_line to use the current server's maximum possible.

   @note This function should only be called while in a channel context.
*/
FALCON_FUNC  XChat_sendModes( ::Falcon::VMachine *vm )
{
   Item *i_targets = vm->param( 0 );
   Item *i_sign = vm->param( 1 );
   Item *i_mode = vm->param( 2 );
   Item *i_modesPerLine = vm->param( 3 );

   if ( i_targets == 0 ||
      i_sign == 0 || ! i_sign->isString()||
      i_mode == 0 || ! i_mode->isString()||
      ( i_modesPerLine != 0 && ! i_modesPerLine->isOrdinal() )
      )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "X, S, S, [N]" ) );
      return;
   }

   char **targets;
   int count;

   // if i_targets is an array, then create an array of targets.
   if ( i_targets->isArray() )
   {
      CoreArray *ca = i_targets->asArray();
      count = ca->length();
      targets = (char**) memAlloc( count * sizeof( char* ) );
      for ( int i = 0; i < count; i ++ )
      {
         targets[i] = stringize_dyn( vm, ca->at( i ) );

         // oops, conversion error?
         if ( targets[i] == 0 )
         {
            for( int j = 0; j < i; j++ )
            {
               delete[] targets[j];
            }
            return;
         }
      }
   }
   else {
      count = 1;
      targets = (char**) memAlloc( sizeof( char* ) );
      targets[0] = stringize_dyn( vm, *i_targets );
      if( targets[0] == 0 )
         return;
   }

   int mpl = i_modesPerLine == 0 ? 0 : i_modesPerLine->forceInteger();

   AutoCString sign( vm, *i_sign );
   AutoCString mode( vm, *i_mode );
   xchat_send_modes( the_plugin, (const char**) targets, count, mpl, *sign , *mode );

   for( int i = 0; i < count; i++ )
   {
      delete[] targets[i];
   }
   memFree( targets );
}


static void internal_create_context( VMachine *vm, xchat_context *ptr, const char *server, const char *channel )
{
   // Access the script data for faster retrival of the live-module
   XChatVM *xvm = static_cast<XChatVM *>( vm );

   // create an instance of the private class XChatContext
   Item *clitem = xvm->scriptData()->m_liveModule->findModuleItem( "XChatContext" );
   fassert( clitem != 0 );

   // Create the core object
   CoreObject *object = clitem->asClass()->createInstance();
   object->setUserData( ptr );

   // fill the infos
   if ( server == 0 ) {
      server = xchat_get_info( the_plugin, "server" );
      if ( server == 0 )
         server = "";
   }

   if ( channel == 0 ) {
      channel = xchat_get_info( the_plugin, "channel" );
      if ( channel == 0 )
         channel = "";
   }

   object->setProperty( "server", UTF8String( server ) );
   object->setProperty( "channel", UTF8String( channel ) );

   vm->retval( object );

}

/*#
   @method findContext XChat
   @brief Finds an xchat context matching the given name.
   @optparam channel A channel name to search for (or nil).
   @optparam server A server name to search for (or nil).
   @return The @a XChatContext handler for the searched context if found, or nil if not found.

   This method searches for a given context among the open panels in an XChat application.
   Both @b channel and @b server parameters are optional.
   - If both @b channel and @b server are not given or nil, an @a XChatContext handler to the
     currently open context is returned.
   - If @b server is not given or nil, the channel is searched among those opened in the currently
     active server context.
   - If @b channel is nil and @b server is given, the server context with the matching name is
     searched.

   In case a panel with the required criterion is not found, nil is returned, otherwise a new
   @a XChatContext containing the informations of the required panel is returned. That object
   can be used to send messages or otherwise manipulate the given context without the need to
   switch to it.
*/
FALCON_FUNC  XChat_findContext( ::Falcon::VMachine *vm )
{
   Item *i_channel = vm->param( 0 );
   Item *i_server = vm->param( 1 );

   if ( ! ( i_channel == 0 || i_channel->isNil() || i_channel->isString() ) ||
      ! ( i_server == 0 || i_server->isNil() || i_server->isString() )
      )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).
         extra( "[S|nil],[S|nil]" ) );
      return;
   }

   const char *server = 0;
   const char *channel = 0;
   char b_server[256];
   char b_channel[256];

   if ( i_channel != 0 && i_channel->isString() )
   {
      if ( i_channel->asString()->toCString( b_channel, 256 ) )
         channel = b_channel;
   }

   if ( i_server != 0 && i_server->isString() )
   {
      if ( i_server->asString()->toCString( b_server, 256 ) )
         server = b_server;
   }

   xchat_context *ptr = xchat_find_context( the_plugin, server, channel );
   if ( ptr == 0 )
   {
      vm->retnil();
      return;
   }

   internal_create_context( vm, ptr, server, channel );
}


/*#
   @method getContext XChat
   @brief Returns a context handler to the currently active context.
   @return A @a XChatContext that can be used to manipulate this context.

   This method returns the currently open context (XChat window or tab) in a
   @a XChatContext instance.

   @note Possibly, record this value for later usage; avoid recreating it too often,
   if not necessary, as creating instances requires extra memory allocation.
*/
FALCON_FUNC  XChat_getContext( ::Falcon::VMachine *vm )
{
   xchat_context *ptr = xchat_get_context( the_plugin );
   if ( ptr == 0 )
   {
      vm->retnil();
      return;
   }

   internal_create_context( vm, ptr, 0, 0 );
}

/*#
   @method getInfo XChat
   @brief Returns textual informations relative to the currently opened context.
   @param info The info to be queried (string).
   @return The value for the required info or @b nil if the required info is not available.

   This method queries the current context for one of the following informations (to be passed
   as the @b info parameter):
      - "away": your current away reason or @b nil if you are not away.
      - "channel": current channel name.
      - "host": real hostname of the server you connected to.
      - "inputbox": the input-box contents, what the user has typed (since 2.4.1).
      - "libdirfs": library directory. e.g. /usr/lib/xchat. The same directory used for auto-loading plugins (since 2.4.0).This string isn't necessarily UTF-8, but local file system encoding.
      - "network": current network name or NULL.
      - "nick": your current nick name.
      - "server": current server name (what the server claims to be). NULL if you are not connected.
      - "topic": current channel topic.
      - "version": xchat version number.
      - "win_status": window status: "active", "hidden" or "normal" (since 2.0.9).
      - "xchatdir": xchat config directory, e.g.: "/home/user/.xchat2" (since 2.0.9).
      - "xchatdirfs": xchat config directory, e.g.: "/home/user/.xchat2" (since 2.0.9).

   @see XChatContext.getInfo
*/
FALCON_FUNC  XChat_getInfo( ::Falcon::VMachine *vm )
{
   if ( vm->paramCount() == 0 )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "X" ) );
      return;
   }

   Item *i_param = vm->param( 0 );
   AutoCString ret( vm, *i_param );

   const char *info = xchat_get_info( the_plugin,  ret );
   if( info == 0 )
      vm->retnil();
   else {
      vm->retval( UTF8String( info ) );
   }
}

/*#
   @method getPrefs XChat
   @brief ovides xchat's setting information (that which is available through the /set command).
   @param setting The setting to be queried.
   @return The required setting (may be nil, a string, an integer or a boolean).

   Provides xchat's setting information (that which is available through the /set command).

   Depending on the setting type the returned value may be a string, an integer or a boolean
   value; in case the setting cannot be found, the method returns nil.

   @note The preferences can be set via @a XChat.command, using the "set" command.
*/

FALCON_FUNC  XChat_getPrefs( ::Falcon::VMachine *vm )
{
   if ( vm->paramCount() == 0 )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "X" ) );
      return;
   }

   Item *i_param = vm->param( 0 );

   AutoCString option( vm, *i_param );

   const char *string;
   int value;
   int result = xchat_get_prefs( the_plugin, option, &string, &value );
   switch( result )
   {
      case 0: vm->retnil(); break;
      case 1: vm->retval( UTF8String( string ) ); break;
      case 2: vm->retval( (int64) value ); break;
      case 3: vm->regA().setBoolean( value != 0 ); break;
   }

}

/*#
   @method nickcmp XChat
   @brief Performs a nick name string-wise comparison.
   @param nick1 The first nickname to be compared.
   @param nick2 The second nickname to be compared.
   @return -1, 0 or 1 if nick1 is respectively lexicographically smaller, the same or greather than nick2.

   Performs a nick name comparison, based on the current server connection.
   This might be a RFC1459 compliant string compare, or plain ascii (in the case of DALNet).
   Use this to compare channels and nicknames. The function works the same way as strcasecmp.

   Because of IRC's scandanavian origin, the characters {}| are considered to be the lower
   case equivalents of the characters []\, respectively. This is a critical issue when
   determining the equivalence of two nicknames.

   @note If the parameters are not strings, they are converted to strings by the Virtual Machine
   using the @b toString() basic object method.
*/

FALCON_FUNC  XChat_nickcmp( ::Falcon::VMachine *vm )
{
   Item *i_s1 = vm->param( 0 );
   Item *i_s2 = vm->param( 1 );
   if( i_s1 == 0 || i_s2 == 0 )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "X,X" ) );
      return;
   }

   AutoCString s1( vm, *i_s1 );
   AutoCString s2( vm, *i_s2 );

   vm->retval( (int64) xchat_nickcmp( the_plugin, s1, s2) );
}

/*#
   @method strip XChat
   @brief Strips mIRC color codes and/or text attributes (bold, underlined etc) from the given string
   @param string The string to be stripped.
   @optparam flags 1 to strip mIRC colors, 2 to strip text attributes, to strip both.
   @return The string with the color codes removed.

   Strips mIRC color codes and/or text attributes (bold, underlined etc) from the given string.

   If not given, @b flags parameter defaults to 3 (strip all).

   @note If the @b string parameter is not a string, it is converted to a string by the Virtual Machine
   using the @b toString() basic object method.
*/

FALCON_FUNC  XChat_strip( ::Falcon::VMachine *vm )
{
   Item *i_s1 = vm->param( 0 );
   Item *i_flags = vm->param( 1 );
   if( i_s1 == 0 || i_flags != 0 && ! i_flags->isOrdinal() )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "X, [N]" ) );
      return;
   }

   AutoCString stripped( vm, *i_s1 );

   int flags = 3;
   if( i_flags != 0 )
   {
      flags = (int) i_flags->forceInteger();
   }

   char *returned = xchat_strip( the_plugin, stripped, strlen( stripped ), flags );
   if ( returned == 0 )
   {
      vm->retnil();
   }
   else {
      vm->retval( UTF8String( returned ) );
      xchat_free( the_plugin, returned );
   }
}


static void internal_crate_timestamp( VMachine *vm, time_t t )
{
   Item *ts_class = vm->findGlobalItem( "TimeStamp" );
   fassert( ts_class != 0);
   CoreObject *tsobj = ts_class->asClass()->createInstance();

   // convert T
   struct tm the_time;
   localtime_r( &t, &the_time );

   tsobj->setProperty( "year", (int64) the_time.tm_year +1900 );
   tsobj->setProperty( "month", (int64) the_time.tm_mon );
   tsobj->setProperty( "day", (int64) the_time.tm_mday );
   tsobj->setProperty( "hour", (int64) the_time.tm_hour );
   tsobj->setProperty( "minute", (int64) the_time.tm_min );
   tsobj->setProperty( "second", (int64) the_time.tm_sec );

   vm->retval( tsobj );
}


static void internal_list( VMachine *vm, const char *name )
{
   const char *const *fields = xchat_list_fields( the_plugin, name );
   xchat_list *list = xchat_list_get( the_plugin, name );

   if( fields == 0 || list == 0 )
   {
      //TODO: Raise an exception?
      vm->retnil();
      return;
   }

   CoreArray *array = new CoreArray;

   while( xchat_list_next(the_plugin, list) )
   {
      LinearDict *dict = new LinearDict;
      const char *server = 0;
      const char *channel = 0;
      xchat_context *ctx = 0;

      for ( int i = 0; fields[i]; i++ )
      {
         char type = fields[i][0];
         const char *fld = fields[i]+1;
         Item value;
         const char *vstr;
         Item *timeStampClass;
         CoreObject *tstamp;

         int iattr;
         switch( type )
         {
         case 's':
            vstr = xchat_list_str( the_plugin, list, (char*)fld );
            value = UTF8String( vstr != 0 ? vstr : "" );
            // is this the server or the channel? -- we must save it for the context
            if( strcmp( fld, "server" ) == 0 )
               server = vstr;
            else if ( strcmp( fld, "channel" ) == 0 )
               channel = vstr;
            break;

         case 'i':
            value = (int64) xchat_list_int( the_plugin, list, (char*)fld );
            break;

         case 'p':
            vstr = xchat_list_str( the_plugin, list, (char*)fld);
            if (strcmp(fld, "context") == 0)
            {
               ctx = (xchat_context*)vstr;
            }
            break;

         case 't':
            // create a timestamp object
            internal_crate_timestamp( vm, xchat_list_time( the_plugin, list, (char*)fld) );
            value = vm->regA();
            break;

         default: /* ignore unknown (newly added?) types */
            value.setNil();
         }

         // had we a context in this loop?
         if ( ctx != 0 )
         {
            // server and channel may be zero if not found
            try
            {
               internal_create_context( vm, ctx, server, channel );
            }
            catch( Falcon::Error* e )
            {
               xchat_list_free( the_plugin, list);
               // We'll return it anyhow to have the GC get rid of it.
               array->append( new CoreDict( dict ) );
               vm->retval( array );
               throw;
            }

            // no need to bufferize.
            dict->put( new CoreString( "context" ), vm->regA()  );
            ctx = 0;
         }
         else {
            dict->put( UTF8String( fld ), value );
         }
      }

      array->append( new CoreDict( dict ) );
   }

   xchat_list_free( the_plugin, list);
   vm->retval( array );
}


/*#
   @method listChannels XChat
   @brief Returns the list of tab contexts currently opened (channels, queries and servers).
   @return An array containing all the informations on the open channels.

   Each entry in the returned array is a dictionary containing the following fields:
   - "channel":  Channel or query name
   - "chantypes":   Channel types e.g. "#!&"
   - "flags": Server/Channel Bits: 0=Connected 1=Connecting 2=Away 3=EndOfMotd (Login complete)
             4=Has WHOX 5=Has IDMSG (FreeNode)
             6=Hide Join/Parts 7=Color Paste 8=Beep on Message 9=Blink Tray on Message
   - "id": Unique server ID, as an integer
   - "lag": Lag in milliseconds
   - "maxmodes": Maximum modes per line
   - "network":  Network name to which this channel belongs
   - "nickprefixes": Nickname prefixes e.g. "@+"
   - "nickmodes": Nickname mode chars e.g. "ov"
   - "queue": Number of bytes in the send-queue
   - "server": Server name to which this channel belongs string
   - "type":  Type of context this is: 1-Server 2-Channel 3-Dialog
   - "users" Number of users in this channel

   @note Please, refer to the XChat plugin manual for updated informations about
      availability of the dictionary fields under different XChat versions.
*/

FALCON_FUNC  XChat_listChannels( ::Falcon::VMachine *vm )
{
   internal_list( vm, "channels" );
}

/*#
   @method listDcc XChat
   @brief Returns the list of DCC transfers currently active.
   @return An array containing all the informations on the currently active DCCs.

   Each entry in the returned array is a dictionary containing the following fields:
   - "address32":Address of the remote user (ipv4 address)
   - "cps": Bytes per second (speed)
   - "destfile": Destination full pathname
   - "file": File name
   - "nick": Nickname of person who the file is from/to
   - "port": TCP port number
   - "pos": Bytes sent/received
   - "resume": Point at which this file was resumed (or zero if it was not resumed)
   - "size":  File size in bytes, low order 32 bits (cast it to unsigned)
   - "sizehigh": File size in bytes, high order 32 bits
   - "status": DCC Status: 0-Queued 1-Active 2-Failed 3-Done 4-Connecting 5-Aborted
   - "type":  DCC Type: 0-Send 1-Receive 2-ChatRecv 3-ChatSend

   @note Please, refer to the XChat plugin manual for updated informations about
      availability of the dictionary fields under different XChat versions.
*/

FALCON_FUNC  XChat_listDcc( ::Falcon::VMachine *vm )
{
   internal_list( vm, "dcc" );
}


/*#
   @method listUsers XChat
   @brief Returns the list of users in the current channel.
   @return An array containing all the informations about the users in the current channel.

   Each entry in the returned array is a dictionary containing the following fields:

   - "away": Away status 0=not away, 1=away
   - "lasttalk": Last time the user was seen talking (as a TimeStamp instance)
   - "nick": Nick name
   - "host": Host name in the form: user\@host (or nil if not known).
   - "prefix": Prefix character, .e.g: \@ or +. Points to a single char.
   - "selected": Selected status in the user list, only works for retrieving
                 the user list of the focused tab.

   @note Please, refer to the XChat plugin manual for updated informations about
      availability of the dictionary fields under different XChat versions.

   @see XChatContext.listUsers
*/
FALCON_FUNC  XChat_listUsers( ::Falcon::VMachine *vm )
{
   internal_list( vm, "users" );
}

/*#
   @method listNotify XChat
   @brief Returns the list of users in the notify list for the current server.
   @return An array containing all the informations about the users in the notify list.

   Each entry in the returned array is a dictionary containing the following fields:

   - "networks": Networks to which this nick applies. Comma separated. May be nil.
   - "nick":  Nickname string
   - "flags" Bit field of flags. 0=Is online.
   - "on": Time when user came online (As a TimeStamp instance)
   - "off": Time when user went offline (As a TimeStamp instance)
   - "seen": Time when user the user was last verified still online (As a TimeStamp instance)

   @note Please, refer to the XChat plugin manual for updated informations about
      availability of the dictionary fields under different XChat versions.

   @see XChatContext.listNotify
*/
FALCON_FUNC  XChat_listNotify( ::Falcon::VMachine *vm )
{
   internal_list( vm, "notify" );
}

/*#
   @method listIgnore XChat
   @brief Returns the list of users in the global ignore list .
   @return An array containing all the informations about the users in the ignore list.

   Each entry in the returned array is a dictionary containing the following fields:

   - "mask": Ignore mask. .e.g: *!*@*.aol.com string
   - "flags": Bit field of flags. 0=Private 1=Notice 2=Channel 3=Ctcp
               4=Invite 5=UnIgnore 6=NoSave 7=DCC

   @note Please, refer to the XChat plugin manual for updated informations about
      availability of the dictionary fields under different XChat versions.
*/
FALCON_FUNC  XChat_listIgnore( ::Falcon::VMachine *vm )
{
   internal_list( vm, "ignore" );
}

/*#
   @method list XChat
   @brief Returns the one of the XChat lists.
   @param list The type of list to be returned.
   @return An array containing all the informations in the given list, or NIL if the list is not available.

   The method allows to call one of the list functions dynamically. The @b list parameter can
   be one of the following values:
   - "channels": see @a XChat.listChannels
   - "dcc": see @a XChat.listDcc
   - "ignore": see @a XChat.listIgnore
   - "notify": see @a XChat.listNotify
   - "users": see @a XChat.listUsers

   @note Please, refer to the XChat plugin manual for updated informations about
      availability of the various list requests and the filed they return
      under different XChat versions.
*/
FALCON_FUNC  XChat_list( ::Falcon::VMachine *vm )
{
   Item *i_list = vm->param( 0 );

   if( i_list == 0 )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "X" ) );
      return;
   }

   AutoCString list( vm, *i_list );
   if( list == 0 )
      return;

   internal_list( vm, list );
}


//=============================================================
// Hook utilities
//=============================================================


static void create_wordlist( VMachine *vm, LinearDict *eventInfo, char *word[], int cmdPos )
{
   // if we're parsing a print event, we must find a print definition.
   CoreArray *theArray = new CoreArray;

   while( word[ cmdPos ][0] != 0 )
   {
      theArray->append( UTF8String( word[ cmdPos ] ) );
      cmdPos ++;
   }

   eventInfo->put( new CoreString( "wordlist" ), theArray  );
}


// Preforms the real call to the VM item performing the callback
// Also appends to the already prepared parameters the parameterse passed by the
// hook caller (at script level).
static int internal_call_cb( XChatVM *vm, CoreObject *handler, const Item &i_callback, int paramCount )
{
   // the real call.
   try {
      vm->callItem( i_callback, paramCount );
   }
   catch( Falcon::Error* err )
   {
      XChatErrHand::handleError( err, vm->scriptData() );
      return XCHAT_EAT_NONE;
   }
   
   if ( vm->scriptData()->isSleeping() )
   {
      return XCHAT_EAT_NONE;
   }

   int retval = (int) vm->regA().forceInteger();

   // was this our last dance?
   if( vm->scriptData()->hooks()->length() == 0 )
   {
      UnloadModule( vm->scriptData() );
      // vm is destroyed by now, so don't use it anymore.
   }

   return retval;
}

//=============================================================
// Main callback hooks
//=============================================================

extern "C" int script_hook_command_cb( char *word[], char *word_eol[], void *user_data)
{
   // user data is the running ScriptData, where relevant data has been stored
   XChatHook *hook = (XChatHook *) user_data;

   CoreObject *handler = hook->handler();
   Item i_callback;
   if ( ! handler->getProperty( "callback", i_callback ) || ! i_callback.isCallable() )
   {
      // someone must have canceled the callback, which is legal.
      return XCHAT_EAT_NONE; // allow someone else to process the message.
   }

   // commands require word[1] and word_eol + 2 to be passed as first and second parameter
   XChatVM *vm = hook->owner()->m_vm;
   vm->pushParameter( UTF8String(  word[1] )  );
   vm->pushParameter( UTF8String(  word_eol[2] )  );

   return internal_call_cb( vm, handler, i_callback, 2 );
}


extern "C" int script_hook_print_cb(char *word[], void *user_data)
{
   XChatHook *hook = (XChatHook *) user_data;

   CoreObject *handler = hook->handler();
   Item i_callback;
   if ( ! handler->getProperty( "callback", i_callback ) || ! i_callback.isCallable() )
   {
      // someone must have canceled the callback, which is legal.
      return XCHAT_EAT_NONE; // allow someone else to process the message.
   }

   XChatVM *vm = hook->owner()->m_vm;

   LinearDict *eventInfo = new LinearDict( 10 );

   // see if it's a registered event
   ParamMap::const_iterator paramIter = s_PMevent.find( hook->match() );
   if ( paramIter != s_PMevent.end() )
   {
      ParamDescList::const_iterator liter = paramIter->second.begin();
      int idWord = 1;
      while( (liter != paramIter->second.end()) && (word[idWord] != 0 && word[idWord][0] != '\0') )
      {
         const ParamDesc &pd = *liter;
         eventInfo->put( new CoreString( pd.m_param ),
               UTF8String( word[idWord] ) );
         ++liter;
         ++idWord;
      }
   }
   // If this is an unmanaged print event
   else {
      // create wordlist from everything we have
      create_wordlist( vm, eventInfo, word, 1 );
   }

   // Create the event name from what we know it should be
   eventInfo->put( new CoreString( "event" ), new CoreString( hook->match() ) );


   // add the event info
   vm->pushParameter( new CoreDict( eventInfo ) );
   return internal_call_cb( vm, handler, i_callback, 1 );
}


extern "C" int script_hook_server_cb(char *word[], char *word_eol[], void *user_data)
{
   XChatHook *hook = (XChatHook *) user_data;

   CoreObject *handler = hook->handler();
   Item i_callback;
   if ( ! handler->getProperty( "callback", i_callback ) || ! i_callback.isCallable() )
   {
      // someone must have canceled the callback, which is legal.
      return XCHAT_EAT_NONE; // allow someone else to process the message.
   }

   XChatVM *vm = hook->owner()->m_vm;
   LinearDict *eventInfo = new LinearDict( 10 );

   // Seek the proper event
   ParamMap::const_iterator paramIter = s_PMsrvmsg.find( hook->match() );
   if ( paramIter != s_PMsrvmsg.end() )
   {
      // great, create the matched data
      // don't store the event, as it's the second element.

      ParamDescList::const_iterator liter = paramIter->second.begin();
      int idWord = 1;
      bool bDone = false;

      while( ! bDone && liter != paramIter->second.end() && word[idWord] != 0 && word[idWord][0] != '\0' )
      {
         const ParamDesc &pd = *liter;
         const char *curWord = word[idWord];
         String *current;

         // special processing for first element, which may be a supernick
         if ( idWord == 1 )
         {
            // the first word generally contains an extra ":" at the beginning.
            // strip it
            uint32 start = curWord[ 0 ] == ':' ? 1 : 0;

            current = UTF8String( curWord + start );
            uint32 posBang = current->find( "!" );
            uint32 posAt = current->find( "@", posBang );
            // Have we a nick?
            if ( posBang < current->length() && posAt < current->length() )
            {
               String *nick = new CoreString( current->subString( 0, posBang ) );
               String *user = new CoreString( current->subString( posBang + 1, posAt ) );
               String *server = new CoreString( current->subString( posAt + 1 ) );

               eventInfo->put( new CoreString( "nick" ), nick  );
               eventInfo->put( new CoreString( "nick:user" ), user  );
               eventInfo->put( new CoreString( "nick:net" ), server  );
            }
         }
         // in managed messages, a ":" beyond the first element means "all the reset".
         else if ( curWord[0] == ':' )
         {
            current = UTF8String( word_eol[idWord] + 1 );
            // this will be the last loop
            bDone = true;
         }
         else {
            // just create the entity
            current = UTF8String( word[idWord] );
         }

         eventInfo->put( UTF8String( pd.m_param ), current );

         ++liter;
         ++idWord;
      }
   }
   // If this is an unmanaged server message
   else {
      // Create the event name from what we know it should be
      eventInfo->put( new CoreString( "event" ), new CoreString( hook->match(), -1 ) );

      // create wordlist from everything we have
      create_wordlist( vm, eventInfo, word, 1 );
   }

   // add the event info
   vm->pushParameter( new CoreDict( eventInfo ) );

   // process the call
   return internal_call_cb( vm, handler, i_callback, 1 );
}


extern "C" int script_hook_timer_cb(void *user_data)
{
   XChatHook *hook = (XChatHook *) user_data;
   CoreObject *handler = hook->handler();

   // we must remove ourself from the hooks,
   // because we won't be automatically called again.
   ( hook->owner()->m_vm, handler );

   Item i_callback;
   if ( ! handler->getProperty( "callback", i_callback ) || ! i_callback.isCallable() )
   {
      // someone must have canceled the callback, which is legal.
      return XCHAT_EAT_NONE; // allow someone else to process the message.
   }

   return internal_call_cb( hook->owner()->m_vm, handler, i_callback, 0 );
}


static void internal_hook( XChatHook *xhook,
                           Item *i_callable )
{


   // now we have the (internal representation of the hook.
   // Create the Falcon script hook object
   VMachine *vm = xhook->owner()->m_vm;
   xchat_hook *hook = xhook->hook();

   // Access the script data for faster retrival of the live-module
   XChatVM *xvm = static_cast<XChatVM *>( vm );

   Item *clitem = xvm->scriptData()->m_liveModule->findModuleItem( "XChatHook" );
   fassert( clitem != 0 );

   // Create the core object
   CoreObject *object = clitem->asClass()->createInstance();
   object->setUserData( xhook );
   xhook->handler( object );

   // set object data:
   object->setProperty( "match", xhook->match() );
   object->setProperty( "callback", *i_callable );

   // Ok, this hook object is ready to go. Record it and send it back to the caller.
   xhook->owner()->addHook( object );
   vm->retval( object );
}


/*#
   @method hookCommand XChat
   @brief Registers a new XChat command with its handler.
   @param cmd The new command to hook.
   @param cb A Falcon callable item to be called back at command execution.
   @optparam help A string containing an help entry for the command user.
   @return An instance of @a XChatHook controlling the callback hook.

   This method retgisters a command that will be available to XChat console
   and an handler that will manage it. The command is accesible from the
   XChat command line (prefixed with a leading "/"), from this plugin or
   from other plugins.

   When the command is invoked, the handler gets called with two parameters:
      - The command itself (so it is possible to use the same handler for different commands).
      - The text entered after the command.

   The handler should return one of the following values:
      - XCHAT_EAT_ALL: The handler has managed the input data, and the processing
                       of this command terminates here.
      - XCHAT_EAT_XCHAT: The handler wants XChat NOT to manage this command; other
                        plugins with a handler installed on the same command will have the chance to
                        process it.
      - XCHAT_EAT_PLUGIN: The handler wants XChat to manage this command, but other plugins
                          having an handler installed on this command will be skipped.
      - XCHAT_EAT_NONE: The handler acts as if it didn't process the command; other plugins and
                        XChat will process the command as if this handler wasn't called.

   Example:
   @code
      function OnReflect( cmd, text )
         > cmd, ": You said: ", text
         return XCHAT_EAT_ALL
      end

      XChat.hookCommand( "TEST", OnReflect, "Replay what you say" )
   @endcode

   The @b cb parameter may be any Falcon callable item, including sigmas; is it so
   possible to store extra parameters and modify the callback on the fly. In the following
   sample we pass a parameter by reference to the callback sigma; the parameter is
   modified by the callback.

   @code
      function OnReflect( times, cmd, text )
         > cmd, ": You said: ", text
         > cmd, ": You called this command ", times++, " times."
         return XCHAT_EAT_ALL
      end

      times = 0
      XChat.hookCommand( "TEST", [ OnReflect, $times ], "Replay what you say" )
   @endcode

   It would be equally possible to use a global variable and
   modify directly the sigma array, and of course, it would be possible to change the
   callable item in front of the sigma to modify the handling object.

   The @b hookCommand method returns an @a XChatHook class instance; changing the @b callback
   property of the returned item it is possible to dynamically modify
   the handler call (changing the handler function/method or just its sigma parameters).

   In example:

   @code
      function OnReflect( hook, id, cmd, text )
         > cmd, ": You said: ", text

         // change the callback
         hook.callback = [ OnReflect, $hook, id + 1 ]

         > "Called ", id, " times."

         return XCHAT_EAT_ALL
      end

      handler = XChat.hookCommand( "TEST", [ OnReflect, $handler, 0 ], "Replay what you say" )
   @endcode

   The hook handler can be unregistered through the @a XChatHook.unhook method even from the
   command handler.

   Callable items used as callback points can also be instance methods or sigmas built on methods.
*/

FALCON_FUNC  XChat_hookCommand( ::Falcon::VMachine *vm )
{
   // Parameters:
   // 0 -- the command << mandatory
   // 1 -- the callable << mandatory
   // 3 -- help text << can be nil

   Item *i_cmd = vm->param( 0 );
   Item *i_callable = vm->param( 1 );
   Item *i_help = vm->param( 2 );

   if ( i_cmd == 0 || ! i_cmd->isString() ||
      i_callable == 0 || ! i_callable->isCallable() ||
      ( i_help != 0 && ! i_help->isNil() && ! i_help->isString() )
      )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).
         extra( "S,C,[S]" ) );
      return;
   }

   AutoCString cmd( vm, *i_cmd );

   // we can now hook the command to xchat
   xchat_hook *hook;
   XChatVM *xvm = static_cast<XChatVM *>( vm );
   XChatHook *xhook = new XChatHook( xvm->scriptData(), *i_cmd->asString() );

   if ( i_help != 0 && i_help->isString() )
   {
      AutoCString helpText( vm, *i_help );
      hook = xchat_hook_command( the_plugin, cmd, XCHAT_PRI_NORM, script_hook_command_cb, helpText, xhook );
   }
   else {
      // Officially, null for help text would do, but it doesn't seem to work
      hook = xchat_hook_command( the_plugin, cmd, XCHAT_PRI_NORM, script_hook_command_cb, "", xhook );
   }

   if ( hook == 0 )
   {
      // Raise an error?
      vm->retnil();
      delete xhook;
      return;
   }

   xhook->hook( hook );
   internal_hook( xhook, i_callable );
}

/*#
   @method hookPrint XChat
   @brief Registers a new handler for XChat Print events.
   @param event The print event to be hooked.
   @param cb A Falcon callable item to be called back when the print event occurs.
   @return An instance of @a XChatHook controlling the callback hook.

   This method installs a print event handler that is called back when the print
   event occurs (actually, before it occurs). The handler has the chance to block
   the event and print something else instead, or let the event to flow and to be
   handled by other handlers and finally by XChat.

   The @b cb item can be any callable Falcon item, including object methods and
   Sigmas. During the callback, the @b cb item receives a single parameter (other
   than eventually declared parameters as Sigmas): a @b data dictionary
   containing informations on the event. The @b data dictionary is granted to have
   one field called "event" which contains the name of the event that has generated
   the callback.
   The other keys in the @b data dictionary are strings, and the composition of the dictionary
   depends from the event intercepted. The contents of the @b data dictionary is described
   by the /FALCON HELP EVENT <event name> command, and the list of events that can be
   intercepted can be obtained with the /FALCON HELP EVENTS command.

   In example:
   @code
      object objHandler
         count = 0

         function onEvent( event )
            > "You have received a print event (handler called ", \
               self.count++, " times)."

            > "Events content: "
            inspect( event )

            return XCHAT_EAT_NONE
         end
      end

      //=============
      // Main program

      XChat.hookPrint( "YOU JOIN", objHandler.onEvent )
      XChat.hookPrint( "YOU PART", objHandler.onEvent )
   @endcode

   may produce the following output in case of channel join:
   @code
   You have received a print event (handler called 0 times).
   Events content:
   test_print: Dict[4]{
   test_print:    "channel" => "#jmtest"
   test_print:    "event" => "YOU JOIN"
   test_print:    "host" => "~jm@217-133-50-123.b2b.tiscali.it"
   test_print:    "nick" => "jonnymind"
   test_print: }
   @endcode

   See the description of @a XChat.hookCommand for more informations about the possible
   usage of sigmas and XChatHook handlers.
*/
FALCON_FUNC  XChat_hookPrint( ::Falcon::VMachine *vm )
{
   // Parameters:
   // 0 -- the print event << mandatory
   // 1 -- the callable << mandatory

   Item *i_cmd = vm->param( 0 );
   Item *i_callable = vm->param( 1 );

   if ( i_cmd == 0 || ! i_cmd->isString() ||
      i_callable == 0 || ! i_callable->isCallable()
      )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).
         extra( "S,C" ) );
      return;
   }

   AutoCString cmd( vm, *i_cmd );

   // we can now hook the command to xchat
   xchat_hook *hook;
   XChatVM *xvm = static_cast<XChatVM *>( vm );
   XChatHook *xhook = new XChatHook( xvm->scriptData(), *i_cmd->asString() );

   hook = xchat_hook_print( the_plugin, cmd, XCHAT_PRI_NORM, script_hook_print_cb, xhook );

   if ( hook == 0 )
   {
      delete xhook;
      vm->retnil();
      return;
   }

   xhook->hook( hook );
   internal_hook( xhook, i_callable );
}

/*#
   @method hookServer XChat
   @brief Registers a new handler for XChat server message.
   @param event The server event to be hooked.
   @param cb A Falcon callable item to be called back when the server message occurs.
   @return An instance of @a XChatHook controlling the callback hook.

   This method installs a server message handler that is called back when the specified
   message is received from the server.

   The @b cb item can be any callable Falcon item, including object methods and
   Sigmas. During the callback, the @b cb item receives a single parameter (other
   than eventually declared parameters as Sigmas): a @b data dictionary
   containing informations on the event. The @b data dictionary is granted to have
   one field called "event" which contains the name of the event that has generated
   the callback.

   The other keys in the @b data dictionary are strings, and the composition of the dictionary
   depends from the event intercepted. The contents of the @b data dictionary is described
   by the /FALCON HELP MESSAGE <event name> command, and the list of events that can be
   intercepted can be obtained with the /FALCON HELP MESSAGES command.

   In example:
   @code
      object objHandler
         count = 0

         function onMessage( event )
            > "You have received a server message (handler called ", \
               self.count++, " times)."

            > "Events content: "
            inspect( event )

            return XCHAT_EAT_NONE
         end
      end

      //=============
      // Main program

      XChat.hookServer( "PRIVMSG", objHandler.onMessage )
      XChat.hookServer( "JOIN", objHandler.onMessage )
   @endcode

   may produce the following output in case of channel join:
   @code
      You have received a server message (handler called 0 times).
      Events content:
      test_server2: Dict[6]{
      test_server2:    "channel" => "#jmtest"
      test_server2:    "event" => "JOIN"
      test_server2:    "nick" => "jonnymind"
      test_server2:    "nick:all" => "jonnymind!~jm@217-133-50-123.b2b.tiscali.it"
      test_server2:    "nick:net" => "217-133-50-123.b2b.tiscali.it"
      test_server2:    "nick:user" => "~jm"
      test_server2: }
   @endcode

   See the description of @a XChat.hookCommand for more informations about the possible
   usage of sigmas and XChatHook handlers.
*/
FALCON_FUNC  XChat_hookServer( ::Falcon::VMachine *vm )
{
   // Parameters:
   // 0 -- the server event << mandatory
   // 1 -- the callable << mandatory
   // 2...n extra parameters passed back to the handler

   Item *i_cmd = vm->param( 0 );
   Item *i_callable = vm->param( 1 );

   if ( i_cmd == 0 || ! i_cmd->isString() ||
      i_callable == 0 || ! i_callable->isCallable()
      )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).
         extra( "S,C" ) );
      return;
   }

   AutoCString cmd( vm, *i_cmd );

   // we can now hook the command to xchat
   xchat_hook *hook;
   XChatVM *xvm = static_cast<XChatVM *>( vm );
   XChatHook *xhook = new XChatHook( xvm->scriptData(), *i_cmd->asString() );

   hook = xchat_hook_server( the_plugin, cmd, XCHAT_PRI_NORM, script_hook_server_cb, xhook );

   if ( hook == 0 )
   {
      delete xhook;
      vm->retnil();
      return;
   }

   xhook->hook( hook );
   internal_hook( xhook, i_callable );
}

/*#
   @method hookTimer XChat
   @brief Registers a new handler for XChat server message.
   @param timeout Number of seconds and fractions of seconds to wait for before timeout.
   @param cb A Falcon callable item to be called back at timeout expiration.
   @return An instance of @a XChatHook controlling the callback hook.

   This method installs a callback handler that will be called after a certain amout of
   time. The @b timeout parameter is expressed in seconds and can be a floating point number.

   The @b cb parameter may be any Falcon callable item, including sigmas and methods.

   The @b cb item is called only once; a new registration is necessary if it must be called
   again.

   @note This method is given for compatibility with simpler programming tecnhiques, but
   actually using sleep() and/or coroutines for handling timeouts and sleep periods may
   be preferred in most cases. The @b sleep() Falcon RTL function will actually give off
   the control to XChat, resuming the script only after the required sleep time has elapsed;
   same happens for sleeps and waits performed by coroutines.
*/
FALCON_FUNC  XChat_hookTimer( ::Falcon::VMachine *vm )
{
   // Parameters:
   // 0 -- the timeout
   // 1 -- the callable << mandatory

   Item *i_timeout = vm->param( 0 );
   Item *i_callable = vm->param( 1 );

   if ( i_timeout == 0 || ! i_timeout->isOrdinal() ||
      i_callable == 0 || ! i_callable->isCallable() )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).
         extra( "N,C" ) );
      return;
   }

   // we can now hook the command to xchat
   xchat_hook *hook;
   XChatVM *xvm = static_cast<XChatVM *>( vm );
   XChatHook *xhook = new XChatHook( xvm->scriptData(), "" );
   hook = xchat_hook_timer( the_plugin, (int) (i_timeout->forceNumeric() * 1000), script_hook_timer_cb, xhook );

   if ( hook == 0 )
   {
      delete xhook;
      vm->retnil();
      return;
   }

   xhook->hook( hook );
   internal_hook( xhook, i_callable );
}


//==================================================
// XChatContext class

/*#
   @class XChatContext
   @brief Represents an xchat context (a pane or a window).

   This class allow to switch the active context, or to write output, send commands,
   change the GUI controls of a given context without switching to it.

   @prop server Server associated with this context (if any).
   @prop channel Channel or (tab name) associated with this context. It may be an XChat generated
   name if the context is a GUI context or doesn't refer to an open IRC channel.

   @see XChat.findContext
*/

/*#
   @method set XChatContext
   @brief Activates this context.

   Makes the context pointed by this item to become the current context and to gain focus in the
   application.
*/
FALCON_FUNC  XChatContext_set( ::Falcon::VMachine *vm )
{
   xchat_context *ctx = (xchat_context *) vm->self().asObject()->getUserData();
   vm->retval( (int64) xchat_set_context( the_plugin, ctx ) );
}

/*#
   @method print XChatContext
   @brief Prints a line in the given context.
   @optparam ... Items to print.

   Writes the given items to the context pointed by this item. If the parameters
   are not string, they are converted to string on the fly by the Virtual Machine.
*/
FALCON_FUNC  XChatContext_print( ::Falcon::VMachine *vm )
{
   xchat_context *oldCtx = xchat_get_context( the_plugin );
   xchat_context *ctx = (xchat_context *) vm->self().asObject()->getUserData();

   if( xchat_set_context( the_plugin, ctx ) )
   {
      for( int i = 0; i < vm->paramCount(); i ++ )
      {
         Item *param = vm->param( i );

         if ( param->isString() )
         {
            vm->stdOut()->writeString( *param->asString() );
         }
         else
         {
            String tempStr;
            vm->itemToString( tempStr, param );
            vm->stdOut()->writeString( tempStr );
         }
      }

      xchat_set_context( the_plugin, oldCtx );
   }
}

/*#
   @method emit XChatContext
   @brief Emits an XChat print event in an aribrary context.
   @param evt A string containing a print event.
   @optparam ... Print event parameters (max 10).

   Generates an artificial XChat event in the context pointed by this
   instance, which needs not to be active.

   @see XChat.emit
*/
FALCON_FUNC  XChatContext_emit( ::Falcon::VMachine *vm )
{
   xchat_context *oldCtx = xchat_get_context( the_plugin );
   xchat_context *ctx = (xchat_context *) vm->self().asObject()->getUserData();

   if( xchat_set_context( the_plugin, ctx ) )
   {
      XChat_emit( vm );
      xchat_set_context( the_plugin, oldCtx );
   }
}

/*#
   @method commad XChatContext
   @brief Sends a command to a given context.
   @param cmd A Complete command as entered on the XChat command line.

   Sends an XChat command to context pointed by this
   instance, which needs not to be active.

   @see XChat.command
*/
FALCON_FUNC  XChatContext_command( ::Falcon::VMachine *vm )
{
   xchat_context *oldCtx = xchat_get_context( the_plugin );
   xchat_context *ctx = (xchat_context *) vm->self().asObject()->getUserData();

   if( xchat_set_context( the_plugin, ctx ) )
   {
      XChat_command( vm );
      xchat_set_context( the_plugin, oldCtx );
   }
}

/*#
   @method message XChatContext
   @brief Sends a PRIVMSG to a given context.
   @param msg The message to be sent to the context.

   Sends an private message to the channel or user which this
   context refers to. The context pointed by this
   instance needs not to be active.

   @see XChat.message
*/
FALCON_FUNC  XChatContext_message( ::Falcon::VMachine *vm )
{
   Item *i_message = vm->param( 0 );

   if ( i_message == 0 || ! i_message->isString() )
   {
      throw  new ParamError( ErrorParam( e_inv_params, __LINE__ ).extra( "S" ) );
      return;
   }

   CoreObject *self = vm->self().asObject();
   xchat_context *ctx = (xchat_context *) self->getUserData();

   Item channel;
   self->getProperty( "channel", channel );
   if( channel.isString() )
   {
      String temp = "PRIVMSG " + *channel.asString() + " : " + *i_message->asString();
      AutoCString ret( vm, &temp );
      xchat_command( the_plugin, ret.c_str() );
   }
   
   //TODO: notify a problem with the channel name
}

/*#
   @method getInfo XChatContext
   @brief Gets generic informations on the given context.
   @param info The info key to be retreived.
   @return The value of the required info, or nil if not available.

   @see XChat.getInfo
*/
FALCON_FUNC  XChatContext_getInfo( ::Falcon::VMachine *vm )
{
   xchat_context *oldCtx = xchat_get_context( the_plugin );
   xchat_context *ctx = (xchat_context *) vm->self().asObject()->getUserData();

   if( xchat_set_context( the_plugin, ctx ) )
   {
      XChat_getInfo( vm );
      xchat_set_context( the_plugin, oldCtx );
   }
}


/*#
   @method listUsers XChatContext
   @brief Lists the users active in the given (channel) context.
   @return A list of user informations, each being a dictionary of user data, or nil if nota available.

   @see XChat.listUsers
*/
FALCON_FUNC  XChatContext_listUsers( ::Falcon::VMachine *vm )
{

   xchat_context *oldCtx = xchat_get_context( the_plugin );
   xchat_context *ctx = (xchat_context *) vm->self().asObject()->getUserData();

   if( xchat_set_context( the_plugin, ctx ) )
   {
      internal_list( vm, "users" );
      xchat_set_context( the_plugin, oldCtx );
   }

}

/*#
   @method listNotify XChatContext
   @brief Lists the notifies active in the given (channel) context.
   @return A list of notify informations, each being a dictionary of notify data, or nil if nota available.

   @see XChat.listNotify
*/
FALCON_FUNC  XChatContext_listNotify( ::Falcon::VMachine *vm )
{
   xchat_context *oldCtx = xchat_get_context( the_plugin );
   xchat_context *ctx = (xchat_context *) vm->self().asObject()->getUserData();

   if( xchat_set_context( the_plugin, ctx ) )
   {
      internal_list( vm, "notify" );
      xchat_set_context( the_plugin, oldCtx );
   }
}


//==================================================
// XChatHook class

/*#
   @class XChatHook
   @brief Callback hook handler.

   An instance of this class is returned by hook functions. The instance can be used
   to cancel the hook (trhough the @a XChatHook.unhook method) or to modify the
   callback that is performed when the hook intercepts an event, message, command or
   timeout completion by changing the @b callback property.

   This class cannot be directly instantiated; it can only be returned by one of the
   XChat hook functions.

   @prop match Event name, server message or command that is intercepted by this hook.
      This entry is read only and modifying it doesn't alter the hook functionality.
      To hook the same callback to another event, unhook this instance and create
      another hook using the @b callback property.

   @prop callback Callable item that gets called when the event bound with this hook happens.
      Changing this property effectively changes the callack routine that will receive the
      notification. The property may be even changed by the hook callback itself, during the
      event handling.

   @see XChat.hookCommand
   @see XChat.hookPrint
   @see XChat.hookServer
   @see XChat.hookTimer
*/

/*#
   @method unhook XChatHook
   @brief Removes the handler associated with this hook.

   This method can be safely called even by the callback handler itself, if it
   doesn't want to be called again.

   @note If a script callback terminates and the script has not any active hook
   anymore, it is automatically unloaded.
*/

FALCON_FUNC  XChatHook_unhook( ::Falcon::VMachine *vm )
{
   CoreObject *self = vm->self().asObject();
   XChatVM *xvm = static_cast< XChatVM * >( vm );

   // remove self from hooked data
   xvm->scriptData()->removeHook( self );
}

} // namespace EXT

//================================================
// Main module function
//

Module *create_xchat_module()
{
   // initialize the module
   Module *self = new Falcon::Module();
   self->name( "fx-chat" );
   self->path( "" );
   self->engineVersion( FALCON_VERSION_NUM );
   self->version( VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION );

   //============================================================
   // Declare the api
   //

   //self->addExtFunc( "nickCompare", Falcon::Ext::nickCompare );

   // Xchat interface functions -- private class XChat
   Falcon::Symbol *c_xchat = self->addClass( "%XChat" );
   c_xchat->exported( false );
   self->addClassMethod( c_xchat, "command", &Falcon::Ext::XChat_command );
   self->addClassMethod( c_xchat, "message", &Falcon::Ext::XChat_message );
   self->addClassMethod( c_xchat, "emit", &Falcon::Ext::XChat_emit );
   self->addClassMethod( c_xchat, "sendModes", &Falcon::Ext::XChat_sendModes );
   self->addClassMethod( c_xchat, "findContext", &Falcon::Ext::XChat_findContext );
   self->addClassMethod( c_xchat, "getContext", &Falcon::Ext::XChat_getContext );
   self->addClassMethod( c_xchat, "getInfo", &Falcon::Ext::XChat_getInfo );
   self->addClassMethod( c_xchat, "getPrefs", &Falcon::Ext::XChat_getPrefs );
   self->addClassMethod( c_xchat, "nickcmp", &Falcon::Ext::XChat_nickcmp );
   self->addClassMethod( c_xchat, "strip", &Falcon::Ext::XChat_strip );

   self->addClassMethod( c_xchat, "listChannels", &Falcon::Ext::XChat_listChannels );
   self->addClassMethod( c_xchat, "listDcc", &Falcon::Ext::XChat_listDcc );
   self->addClassMethod( c_xchat, "listUsers", &Falcon::Ext::XChat_listUsers );
   self->addClassMethod( c_xchat, "listNotify", &Falcon::Ext::XChat_listNotify );
   self->addClassMethod( c_xchat, "listIgnore", &Falcon::Ext::XChat_listIgnore );
   self->addClassMethod( c_xchat, "list", &Falcon::Ext::XChat_list );

   self->addClassMethod( c_xchat, "hookCommand", &Falcon::Ext::XChat_hookCommand );
   self->addClassMethod( c_xchat, "hookPrint", &Falcon::Ext::XChat_hookPrint );
   self->addClassMethod( c_xchat, "hookServer", &Falcon::Ext::XChat_hookServer );
   self->addClassMethod( c_xchat, "hookTimer", &Falcon::Ext::XChat_hookTimer );

   // create a singletone instance of %XChat class.
   Symbol *o_xchat = new Symbol( self, "XChat" );
   o_xchat->setInstance( c_xchat );
   o_xchat->exported( true );
   self->addGlobalSymbol( o_xchat );

   // create the private class context
   Falcon::Symbol *c_ctx = self->addClass( "XChatContext" );
   c_xchat->exported( false );
   self->addClassProperty( c_ctx, "server" );
   self->addClassProperty( c_ctx, "channel" );
   self->addClassMethod( c_ctx, "set", &Falcon::Ext::XChatContext_set );
   self->addClassMethod( c_ctx, "print", &Falcon::Ext::XChatContext_print );
   self->addClassMethod( c_ctx, "emit", &Falcon::Ext::XChatContext_emit );
   self->addClassMethod( c_ctx, "command", &Falcon::Ext::XChatContext_command );
   self->addClassMethod( c_ctx, "message", &Falcon::Ext::XChatContext_message );
   self->addClassMethod( c_ctx, "getInfo", &Falcon::Ext::XChatContext_getInfo );
   self->addClassMethod( c_ctx, "listUsers", &Falcon::Ext::XChatContext_listUsers );
   self->addClassMethod( c_ctx, "listNotify", &Falcon::Ext::XChatContext_listNotify );


   // create the private class hook
   Falcon::Symbol *c_hook = self->addClass( "XChatHook" );
   c_xchat->exported( false );
   self->addClassProperty( c_hook, "match" );
   self->addClassProperty( c_hook, "callback" );
   self->addClassMethod( c_hook, "unhook", &Falcon::Ext::XChatHook_unhook );


   self->addConstant( "XCHAT_EAT_ALL", (Falcon::int64) XCHAT_EAT_ALL );
   self->addConstant( "XCHAT_EAT_XCHAT", (Falcon::int64) XCHAT_EAT_XCHAT );
   self->addConstant( "XCHAT_EAT_PLUGIN", (Falcon::int64) XCHAT_EAT_PLUGIN );
   self->addConstant( "XCHAT_EAT_NONE", (Falcon::int64) XCHAT_EAT_NONE );

   return self;
}

}

/* end of fxchat_ext.cpp */
