/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_vm.cpp

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
   XChat specific VM.
*/

#include "fxchat_stream.h"
#include "fxchat_vm.h"
#include "fxchat_script.h"


XChatVM::XChatVM( ScriptData *owner ):
   VMachine( false ),  // prevent initialization of streams.
   m_scriptData( owner )
{
   m_stdOut = new XChatStream();
   m_stdErr = new XChatStream( owner->name() + ": " );
   init();
}

void XChatVM::onIdleTime( Falcon::numeric seconds )
{
   scriptData()->putAtSleep( seconds );
   breakRequest(true);
}


/* end of fxchat_vm.cpp */
