/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_script.h

   Falcon script Xchat plugin
   XChat specific module.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin.
   XChat specific module.
*/

#ifndef fxchat_script_H
#define fxchat_script_H

#include <falcon/engine.h>
#include "xchat-plugin.h"

class ScriptDataList;
class XChatVM;

// The main structure holding our modules.
class ScriptData
{
   ScriptData *m_next;
   ScriptData *m_prev;

   void remove();
   void insertAfter( ScriptData *prev );

   friend class ScriptDataList;

   // This is the sleep/vm yield timeout callback.
   // it is managed transparently with respect to the VM, so we may have
   // just an xchat_hook for it.
   xchat_hook *m_pSleepHook;

   // This is the list of hooks that the script has registered.
   // As the regitered hooks, on the script standpoint, are VM items,
   // we use a Falcon array of items to store them. This will also
   // allow us to transparently GC-Lock the items, by just locking the
   // container, so that GC will never destroy any of our hooks, even
   // if the script wants to remove them.
   //---
   // There is no cleanup nor destructor action related with m_hooks here,
   // as it is automatically destroyed when the VM unrolls the GC for
   // destruction.
   Falcon::CoreArray *m_hooks;
	Falcon::GarbageLock *m_hook_lock;
	
   
public:
   Falcon::Module *m_module;
   // Pre-cached live-module pointer
   Falcon::LiveModule *m_liveModule;

   XChatVM *m_vm;

   bool m_bStatus;

   ScriptData( Falcon::Module *mod, char **args );
   ~ScriptData();

   void addHook( Falcon::CoreObject *hook );
   void removeHook( Falcon::CoreObject *hook );
   void unhookAll();
   void putAtSleep( Falcon::numeric seconds );
   void cancelSleep();
   bool isSleeping() const { return m_pSleepHook != 0; }

   // MAY THROW, check out for errors.
   void RunVM( bool reset = false );

   const Falcon::String &name() const { return m_module->name(); }
   Falcon::CoreArray *hooks() const { return m_hooks; }
};


class ScriptDataList
{
   ScriptData *m_head;
   ScriptData *m_tail;

public:
   ScriptDataList();
   ~ScriptDataList();

   void append( ScriptData *mod );
   ScriptData *find( const Falcon::String &name );
   void remove( ScriptData *mod );
   bool remove( const Falcon::String &name );

   void list();
};

#endif

/* end of fxchat_script.h */
