/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat.cpp

   Falcon script Xchat plugin
   Xchat plugin interface.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin.
   Xchat plugin interface.
*/

#include <falcon/engine.h>
#include <falcon/sys.h>

#include "fxchat.h"
#include "fxchat_ext.h"
#include "fxchat_events.h"
#include "fxchat_errhand.h"
#include "fxchat_script.h"
#include "fxchat_vm.h"

#include "xchat-plugin.h"

#include "version.h"

/*#
   @main The Falcon-Xchat module.

   This modules provides a complete, simple, comfortable and powerful
   scripting environment for the XChat IRC client.

   The module is currently available only in Linux and other Unices
   where XChat is normally shipped by available distros.

   The module should compile out of the box with Falcon version 0.8.10
   or following configured for development (devel package installed). A
   simple "make" on the top dir should create a fxchat.so which links
   agains the system falcon libraries. However, it should be easy
   to compile this module also in Windows; just
   at the moment the make files / build systems are not provided.

   Once compiled the module in a shared object (or dll), it should be
   made available to XChat putting it in the $HOME/.xchat2 subdirectory
   or in the standard XChat plugin directory (see xhcat documentation).

   See the related pages of this module for details about the module usage.
*/

//==============================================
// Function used privately by this program
//

static void Cmd_FalconList();
static void Cmd_FalconLoad( const Falcon::String &fname, char **params );
static void Cmd_FalconUnload( const Falcon::String &fname );
static void Cmd_FalconReload( const Falcon::String &fname, char **params );
static void Cmd_FalconReset( const Falcon::String &fname  );
static void Cmd_FalconAbout();
static void Cmd_FalconHelp( char **params, char *rest );

static int Cmd_Falcon(char *word[], char *word_eol[], void *userdata);
static int Cmd_Load(char *word[], char *word_eol[], void *userdata);
static int Cmd_Unload(char *word[], char *word_eol[], void *userdata);

//==============================================
// Module wide define
//
static const char usage[] =
   PNAME ": Usage: /FALCON LOAD <filename>\n"
   PNAME ":                UNLOAD <filename|name>\n"
   PNAME ":                RELOAD <filename|name>\n"
   PNAME ":                LIST\n"
   PNAME ":                HELP\n"
   PNAME ":                ABOUT\n\n";

static const char help_on_help[] =
   PNAME ": For more help, use /FALCON HELP <parameter>\n"
   PNAME ": Parameters: \n"
   PNAME ":    GENERAL - Generic help on the plugin\n"
   PNAME ":    EVENTS - description of intercepted xchat 'print' events\n"
   PNAME ":    MESSAGES - description of intercepted chat server mesasges\n\n";

static const char about[] =
   "X-Chat Falcon Interface " VERSION_DESC "\n"
   "Copyright (C) 2007-2008  Giancarlo Niccolai <gc@falconpl.org>\n\n";


//==============================================
//Global module definitions
//

// plugin handle
static xchat_plugin *ph;

// we repeat the plugin handle because ph is a reserved name for windows.
xchat_plugin *the_plugin;

static int enable = 1;
static bool init = false;

static int init_count = 0;

//==============================================
// Public Falcon data.
//

Falcon::ModuleLoader *s_loader;
Falcon::Module *s_modCore;
Falcon::Module *s_modXchat;

static ScriptDataList *s_modules;

//==============================================
// Utilities & generic functions
//

void xchat_print_falcon( const Falcon::String &str )
{
   // transform in a utf8 string
   // try to do it the fast way using stack memory.
   if ( str.size() < 2048 )
   {
      char buffer[2048];
      if( str.toCString( buffer, 2048 ) ) {
         xchat_print( ph, buffer );
         return;
      }
   }

   // it didn't work; do it the right way.
   int maxlen = str.length() * 4 + 4; // max utf8 size
   char *buffer = new char[ maxlen ];
   str.toCString( buffer, maxlen );
   xchat_print( ph, buffer );
   delete[] buffer;
}

//==============================================
// Command implementation
//

static int Cmd_Falcon(char *word[], char *word_eol[], void *userdata)
{
   Falcon::String cmd( word[2] );
   bool bOk = false;

   if ( cmd.compareIgnoreCase( "LIST" ) == 0 )
   {
      Cmd_FalconList();
      bOk = true;
   }
   else if ( cmd.compareIgnoreCase( "LOAD" ) == 0 && word[3][0] != 0 )
   {
      Cmd_FalconLoad( word[3], word + 4 );
      bOk = true;
   }
   else if ( cmd.compareIgnoreCase( "UNLOAD" ) == 0 && word[3][0] != 0 )
   {
      Cmd_FalconUnload( word[3] );
      bOk = true;
   }
   else if ( cmd.compareIgnoreCase( "RELOAD" ) == 0 && word[3][0] != 0 )
   {
      Cmd_FalconReload( word[3], word + 4 );
      bOk = true;
   }
   else if ( cmd.compareIgnoreCase( "RESET" ) == 0 && word[3][0] != 0 )
   {
      Cmd_FalconReset( word[3] );
      bOk = true;
   }
   else if ( cmd.compareIgnoreCase( "ABOUT" ) == 0)
   {
      Cmd_FalconAbout();
      bOk = true;
   }
   else if ( cmd.compareIgnoreCase( "HELP" ) == 0)
   {
      Cmd_FalconHelp( word + 3, word_eol[4] );
      bOk = true;
   }

   if ( ! bOk )
      xchat_print(ph, usage);

   return XCHAT_EAT_ALL;
}

static int Cmd_Load(char *word[], char *word_eol[], void *userdata)
{
   Falcon::String fname( word[2] );
   if ( fname.rfind( ".fal" ) == fname.length() - 4 ||
      fname.rfind( ".fam" ) == fname.length() - 4 )
   {
      Cmd_FalconLoad( fname, word + 3 );
      return XCHAT_EAT_XCHAT;
   }

   return XCHAT_EAT_NONE;
}

static int Cmd_Unload(char *word[], char *word_eol[], void *userdata)
{
   Falcon::String fname( word[2] );
   if ( fname.rfind( ".fal" ) == fname.length() - 4 ||
      fname.rfind( ".fam" ) == fname.length() - 4 )
   {
      Cmd_FalconUnload( fname );
      return XCHAT_EAT_XCHAT;
   }

   return XCHAT_EAT_NONE;
}

static void Cmd_FalconList()
{
   s_modules->list();
}

static void Cmd_FalconLoad( const Falcon::String &fname, char **args )
{
   // let's try to load that module.
   ScriptData *xmodule = 0;
   bool delmod = true; // delete the module in case of problems.
   
   try
   {
      Falcon::Module *mod = s_loader->loadSource( fname );
      // great; the module has been loaded. Now, reslove all the references.
      // to do that properly, we need an ardmed runtime.
      Falcon::Runtime r( s_loader );
      r.addModule( mod );

      // great, now we can create an instance of the VM, which we have in XChatModule
      xmodule = new ScriptData( mod, args );

      // we can abandon now our reference to the module. Xmodule has one.
      mod->decref();

      // insert the loaded module(s) in the VM.
      xmodule->m_vm->link( &r );

      // ready the run symbol
      if( xmodule->m_vm->mainModule()->module()->findGlobalSymbol( "__main__" ) == 0 )
      {
         // not a valid module? -- kill it
         delete xmodule;
         return;
      }

      // we are in; save the module and run the script.
      s_modules->append( xmodule );
      delmod = false;
      
      Falcon::AutoCString modName( mod->name() );
      xchat_printf( the_plugin, PNAME ": Loaded module %s", modName.c_str() );

      xmodule->RunVM( true );
   }
   catch( Falcon::Error* err )
   {
      XChatErrHand::handleError( err, xmodule ); // can be 0
      if ( delmod )
         delete xmodule;
   }
}

static void Cmd_FalconUnload( const Falcon::String &fname )
{
   if( fname == "" )
   {
      xchat_print(ph, usage);
   }

   ScriptData *mod = s_modules->find( fname );
   if( mod == 0 )
   {
      xchat_print_falcon( PNAME ": Module " + fname + " not found\n" );
      return;
   }

   UnloadModule( mod );
}

static void Cmd_FalconReload( const Falcon::String &fname, char **params )
{
   ScriptData *mod = s_modules->find( fname );

   if( mod == 0 )
   {
      xchat_print_falcon( PNAME ": Module " + fname + " not found\n" );
      return;
   }

   xchat_print_falcon( PNAME ": Reloading module " + fname + "\n" );
   Falcon::String path = mod->m_module->path();
   s_modules->remove( mod );
   delete mod;

   Cmd_FalconLoad( path, params );
   xchat_print_falcon( PNAME ": Reloading complete\n" );
}


static void Cmd_FalconReset( const Falcon::String &fname  )
{
   ScriptData *mod = s_modules->find( fname );

   if( mod == 0 )
   {
      xchat_print_falcon( PNAME ": Module " + fname + " not found\n" );
      return;
   }

   if( mod->m_bStatus )
   {
      xchat_print_falcon( PNAME ": Module " + fname + " is not currently in error state.\n" );
      return;
   }

   try {
      mod->m_bStatus = true;
      mod->RunVM( true );
   }
   catch( Falcon::Error* err )
   {
      XChatErrHand::handleError( err, mod );
      xchat_print_falcon( PNAME ": Module " + fname + " reset failed.\n" );
   }

}


static void Cmd_FalconAbout()
{
   xchat_print(ph, about);
}

static void Cmd_FalconHelp( char **param, char *rest )
{
   if ( param == 0 || param[0] == 0 || param[0][0] == '\0' )
   {
      xchat_print_falcon( help_on_help );
      return;
   }

   Falcon::String cmd( param[0] );

   if ( cmd.compareIgnoreCase( "EVENTS" ) == 0 )
   {
      ParamMap::const_iterator paramIter = s_PMevent.begin();
      xchat_print_falcon( PNAME ": known events:\n");
      while( paramIter != s_PMevent.end() )
      {
         xchat_print_falcon( PNAME ":   " + paramIter->first );
         ++paramIter;
      }
      xchat_print_falcon( PNAME ": for a description of the data filled in event callbacks, use /FALCON HELP EVENT <eventName>\n\n");
   }
   else if ( cmd.compareIgnoreCase( "MESSAGES" ) == 0 )
   {
      ParamMap::const_iterator paramIter = s_PMsrvmsg.begin();
      xchat_print_falcon( PNAME ": known server messages:\n");
      while( paramIter != s_PMsrvmsg.end() )
      {
         xchat_print_falcon( PNAME ":   " + paramIter->first );
         ++paramIter;
      }
      xchat_print_falcon( PNAME ": for a description of the data filled in server message callbacks, use /FALCON HELP MESSAGE <message>\n\n");
   }
   else if ( cmd.compareIgnoreCase( "EVENT" ) == 0 )
   {
      // try to search the described event.
      ParamMap::const_iterator paramIter = s_PMevent.find( rest );
      if ( paramIter == s_PMevent.end() )
      {
         xchat_print_falcon( PNAME ": Sorry, event not knwon\n" );
      }
      else {
         xchat_print_falcon( PNAME ": Print event \"" + paramIter->first + "\" parameters:\n");
         if ( paramIter->second.empty() )
         {
            xchat_print_falcon( PNAME ":   None.\n" );
         }
         else {
            ParamDescList::const_iterator liter = paramIter->second.begin();
            while( liter != paramIter->second.end() )
            {
               const ParamDesc &pd = *liter;
               xchat_print_falcon( Falcon::String( PNAME ":   * " ) + pd.m_param + " = " + pd.m_desc );
               ++liter;
            }
         }
      }
   }
   else if ( cmd.compareIgnoreCase( "MESSAGE" ) == 0 )
   {
      // try to search the described server message.
      ParamMap::const_iterator paramIter = s_PMsrvmsg.find( rest );
      if( paramIter == s_PMsrvmsg.end() )
      {
         xchat_print_falcon( PNAME ": Sorry, help server message not known\n" );
      }
      else {
         xchat_print_falcon( PNAME ": Server message \"" + paramIter->first + "\" parameters:\n");

         // this parameters are common for all the server messages.
         xchat_print_falcon( Falcon::String( PNAME ":   * nick = The nickname sent with this message" ) );
         xchat_print_falcon( Falcon::String( PNAME ":   * nick:user = The user name part of this nick." ) );
         xchat_print_falcon( Falcon::String( PNAME ":   * nick:net = The network part of this nick." ) );

         ParamDescList::const_iterator liter = paramIter->second.begin();
         while( liter != paramIter->second.end() )
         {
            const ParamDesc &pd = *liter;
            xchat_print_falcon( Falcon::String( PNAME ":   * " ) + pd.m_param + " = " + pd.m_desc );
            ++liter;
         }
      }
   }
   else if ( cmd.compareIgnoreCase( "GENERAL" ) == 0 )
   {
      xchat_print_falcon( PNAME ": Not written yet, sorry.\n");
   }
   else
   {
      xchat_print_falcon( PNAME ": Sorry, help topic unknown\n" );
   }
}


void UnloadModule( ScriptData *mod )
{
   // unhook all the hooked elements
   mod->unhookAll();
   s_modules->remove( mod );
   xchat_print_falcon( PNAME ": Unloaded module " + mod->name() + "\n" );
   delete mod;
}

//==============================================
// Root plugin interface
//

void xchat_plugin_get_info(char **name, char **desc, char **version, void **reserved)
{
   *name = (char*) PNAME;
   *desc = (char*) PDESC;
   *version = (char*) VERSION_DESC;
}

int xchat_plugin_init(xchat_plugin *plugin_handle,
                     char **plugin_name,
                     char **plugin_desc,
                     char **plugin_version,
                     char *arg)
{
   Falcon::Engine::Init();
   
   /* we need to save this for use with any xchat_* functions */
   ph = plugin_handle;
   the_plugin = ph;

   /* tell xchat our info */
   *plugin_name = (char*) PNAME;
   *plugin_desc = (char*) PDESC;
   *plugin_version = (char*)VERSION_DESC;

   /* Block double initalization. */
   if ( init_count++ > 0 )
   {
      xchat_print(ph, PNAME ": Falcon interface already loaded.\n");
      return 0;
   }

   // So, first of all, search for FALCON_LOAD_PATH that should
   // tell us where it is.
   Falcon::String envpath;
   if( ! Falcon::Sys::_getEnv( "FALCON_LOAD_PATH", envpath ) )
   {
      xchat_print(ph, PNAME ": Warning: system has not a FALCON_LOAD_PATH variable. Using defaults.\n" );
      envpath = FALCON_DEFAULT_LOAD_PATH;
   }

   // ensure local path is added to the load path.
   if ( envpath != "." && envpath.find( ".;" ) != 0 ) // not strictly necessary but...
   {
      envpath = ".;" + envpath;
   }

   // create the loader and set the error handler to xchat.
   s_loader = new Falcon::ModuleLoader( envpath );

   // Create also an instance of the Falcon Core module
   s_modCore = Falcon::core_module_init();

   // and an instance of our module
   s_modXchat = Falcon::create_xchat_module();

   // and finally, the list where we'll store loaded modules
   s_modules = new ScriptDataList;

   // we're armed and ready for combat. Just add xchat hooks:

   xchat_hook_command(ph, "FALCON", XCHAT_PRI_NORM, Cmd_Falcon, usage, 0);
   xchat_hook_command(ph, "FXCHAT", XCHAT_PRI_NORM, Cmd_Falcon, usage, 0);
   xchat_hook_command(ph, "LOAD", XCHAT_PRI_NORM, Cmd_Load, 0, 0);
   xchat_hook_command(ph, "UNLOAD", XCHAT_PRI_NORM, Cmd_Load, 0, 0);

   LoadEventParamMap();
   LoadSrvMsgParamMap();

   xchat_print(ph, PNAME ": Falcon interface succesfully loaded.\n" );

   return 1;
}


int xchat_plugin_deinit()
{
   if ( --init_count > 0 ) {
      return 1;
   }

   //TODO: Running scripts

   // destroy all the scripts
   delete s_modules;

   // delete the standard modules
   s_modCore->decref();
   s_modXchat->decref();

   delete s_loader;

   Falcon::Engine::Shutdown();

   xchat_print(ph, PNAME ": Falcon interface unloaded.\n");
   return 1;
}


/* end of fxchat.cpp */
