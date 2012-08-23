/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_script.cpp

   Falcon script Xchat plugin
   XChat specific VM.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin
   XChat specific module.
*/

#include "fxchat_script.h"
#include "fxchat_errhand.h"
#include "fxchat_hook.h"
#include "fxchat_vm.h"
#include "fxchat.h"

#include <stdio.h>

ScriptData::ScriptData( Falcon::Module *mod, char **params ):
   m_module( mod ),
   m_vm( new XChatVM( this ) ),
   m_next( 0 ),
   m_prev( 0 ),
   m_bStatus( true ),
   m_pSleepHook( 0 )
{
   m_module->incref();

   // insert the standard modules in the VM
   m_vm->link( s_modCore );
   m_liveModule = m_vm->link( s_modXchat );

   // We'll add the args that the user wants to provide us.
   Falcon::Item *item = m_vm->findGlobalItem( "args" );
   if( item != 0 )
   {
      Falcon::CoreArray *args = new Falcon::CoreArray;
      *item = args;
      if ( params != 0 )
      {
         while( *params != 0 && **params != '\0' )
         {
            args->append( Falcon::UTF8String( *params ) );
            ++args;
         }
      }
   }

   // set also scriptname and scriptpath
   item = m_vm->findGlobalItem( "scriptName" );
   if( item != 0 )
   {
      *item = new Falcon::CoreString( mod->name() );
   }

   item = m_vm->findGlobalItem( "scriptPath" );
   if( item != 0 )
   {
      *item = new Falcon::CoreString( mod->path() );
   }

   // create a CoreArray for the hooks and save it via gc lock
   m_hooks = new Falcon::CoreArray;
   m_hook_lock = new Falcon::GarbageLock( Falcon::Item(m_hooks) );
}

ScriptData::~ScriptData()
{
	delete m_hook_lock;
   m_module->decref();
   // this will also destroy the core array used for hooks.
   m_vm->finalize();
}

void ScriptData::insertAfter( ScriptData *prev )
{
   m_next = prev->m_next;
   prev->m_next = this;
   m_prev = prev;

   if ( m_next != 0 )
   {
      m_next->m_prev = this;
   }
}

void ScriptData::remove()
{
   if ( m_next != 0 )
   {
      m_next->m_prev = m_prev;
   }

   if ( m_prev != 0 )
   {
      m_prev->m_next = m_next;
   }

   m_prev = m_next = 0;
}

void ScriptData::unhookAll()
{
   cancelSleep();

   for( int i = 0; i < m_hooks->length(); i++ )
   {
      Falcon::CoreObject *hook = m_hooks->at( i ).asObject();
      XChatHook *xh = (XChatHook *) hook->getUserData();

      // the hook may have already dis-hooked itself.
      if ( xh != 0 )
      {
         xchat_unhook( the_plugin, xh->hook() );
         // void the hook
         hook->setUserData( (Falcon::FalconData*)0 );
         delete xh;
      }
   }

   // empty the array of hooks
   m_hooks->resize( 0 );
   // notice that this also causes the hook object to be reclaimable,
   // in case the script has dropped them too.
}

extern "C" int mod_sleep_timer_cb( void *user_data )
{
   ScriptData *mod = (ScriptData *) user_data;

   // resume the VM
   try {
      mod->RunVM();
   }
   catch( Falcon::Error* err )
   {
      XChatErrHand::handleError( err, mod );
   }
}


void ScriptData::putAtSleep( Falcon::numeric seconds )
{
   if( m_pSleepHook != 0 )
   {
      xchat_unhook( the_plugin, m_pSleepHook );
   }

   int slt = (int)(seconds*1000);
   m_pSleepHook = xchat_hook_timer( the_plugin, slt, mod_sleep_timer_cb, this );

}

void ScriptData::cancelSleep()
{
   if ( m_pSleepHook != 0 )
   {
      xchat_unhook( the_plugin, m_pSleepHook );
      m_pSleepHook = 0;
   }
}


void ScriptData::RunVM( bool reset )
{
   if ( reset )
   {
      // restart from beginning of the program
      if ( m_vm->mainModule()->module()->findGlobalSymbol( "__main__" ) == 0 )
      {
         Falcon::AutoCString name( m_module->name() );
         xchat_printf( the_plugin, PNAME ": the module %s has no main routine and cannot be launched\n",
            name.c_str() );
         return;
      }
      
      m_vm->launch();
   }
   else
      m_vm->run();

   // in case of error, the callers must catch us.

   if ( ! isSleeping() )
   {
      // if had not a sleep request...
      // ...and If the modue has not registered any hook, unload it.
      if( m_hooks->length() == 0 )
      {
         UnloadModule( this );
      }
   }

   // else everything went fine.
}

void ScriptData::addHook( Falcon::CoreObject *hook )
{
   m_hooks->append( hook );
}

void ScriptData::removeHook( Falcon::CoreObject *hook )
{
   for( Falcon::uint32 i = 0; i < m_hooks->length(); i++ )
   {
      if ( m_hooks->at(i).asObject() == hook )
      {
         m_hooks->remove( i );
         // no need to destroy anything: the GC will rip it at good time.
         // it will rip also the internal data, that is, the XChatHook
         break;
      }
   }
}

//===========================================================
// Module list
//

ScriptDataList::ScriptDataList():
   m_head( 0 ),
   m_tail( 0 )
{}

ScriptDataList::~ScriptDataList()
{
   ScriptData *p = m_head;
   while( p != 0 )
   {
      ScriptData *cur = p;
      p = p->m_next;
      delete cur;
   }
}



void ScriptDataList::append( ScriptData *mod )
{
   if ( m_tail == 0 )
   {
      m_head = m_tail = mod;
   }
   else {
      mod->insertAfter( m_tail );
   }
}

ScriptData *ScriptDataList::find( const Falcon::String &name )
{
   ScriptData *p = m_head;
   while( p != 0 )
   {
      if ( p->m_module->name() == name )
         return p;

      p = p->m_next;
   }

   return 0;
}

void ScriptDataList::remove( ScriptData *mod )
{
   if ( m_head == mod )
      m_head = mod->m_next;

   if ( m_tail == mod )
      m_tail = mod->m_prev;

   mod->remove();
}

bool ScriptDataList::remove( const Falcon::String &name )
{
   ScriptData *p = find( name );
   if ( p != 0 )
   {
      if ( m_head == p )
         m_head = p->m_next;
      if ( m_tail == p )
         m_tail = p->m_prev;

      p->remove();
      delete p;

      return true;
   }

   return false;
}

void ScriptDataList::list()
{
   xchat_print( the_plugin, PNAME ": -------------------------------------------\n" );
   if ( m_head == 0 ) {
      xchat_print( the_plugin, PNAME ":    Currently, no module loaded.\n" );
      return;
   }

   xchat_print( the_plugin,
      PNAME ": Status  Name\n"
      PNAME ": ------  ----------------------------------\n" );

   ScriptData *mod = m_head;
   while( mod != 0 )
   {
      Falcon::String status = mod->m_bStatus ? "Ok  " : "Error";
      xchat_print_falcon( PNAME ": "+ status + "    " + mod->m_module->name() +"\n" );
      mod = mod->m_next;
   }
   xchat_print( the_plugin, PNAME ": ----------------------------------------------\n" );
}




/* end of fxchat_script.cpp */
