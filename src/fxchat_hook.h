/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_hook.h

   Falcon script Xchat plugin
   XChat hook data carrier for hook objects
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-29 18:48:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin.
   XChat specific module.
*/

#ifndef fxchat_hook_H
#define fxchat_hook_H

#include <falcon/falcondata.h>
#include "xchat-plugin.h"

class ScriptData;

// This is a reflective carrier for hooks.
class XChatHook: public Falcon::FalconData
{
   Falcon::String m_sMatch;
   xchat_hook *m_hook;
   ScriptData *m_owner;
   Falcon::CoreObject *m_handler;

public:
   XChatHook( ScriptData *owner,
               const Falcon::String &sMatch ):
      m_hook( 0 ),
      m_sMatch( sMatch ),
      m_owner( owner ),
      m_handler( 0 )
   {
      m_sMatch.bufferize();
   }

   virtual ~XChatHook() {}

   ScriptData *owner() const { return m_owner; }
   const Falcon::String &match() const { return m_sMatch; }

   void hook( xchat_hook *h ) { m_hook = h; }
   xchat_hook *hook() const { return m_hook; }

   Falcon::CoreObject *handler() const { return m_handler; }
   void handler( Falcon::CoreObject *h ) { m_handler = h; }

   virtual Falcon::FalconData* clone() const { return 0; }
   virtual void gcMark( Falcon::uint32 ) {}

};

#endif

/* end of fxchat_hook.h */
