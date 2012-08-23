/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_vm.h

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
   Falcon script Xchat plugin.
   XChat specific VM.
*/

#ifndef fxchat_vm_H
#define fxchat_vm_H

#include <falcon/engine.h>

// The specific xchat vmachine sets up standard streams and
// provides a back-link to the owner script data.

class ScriptData;

class XChatVM: public Falcon::VMachine
{
   ScriptData *m_scriptData;
public:
   XChatVM( ScriptData *owner );

   // Override idle time requests.
   virtual void onIdleTime( Falcon::numeric seconds );
   
   ScriptData *scriptData() const { return m_scriptData; }
};


#endif

/* end of fxchat_vm.h */
