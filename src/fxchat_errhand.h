/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_errhand.h

   Falcon script Xchat plugin
   Definition of xchat specific error handler.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin.
   Integration with Xchat.
   Definition of xchat specific error handler.
*/

#ifndef fxchat_errhand_H
#define fxchat_errhand_H

#include <falcon/engine.h>

class ScriptData;

class XChatErrHand
{

public:
	static void handleError( Falcon::Error *error, ScriptData* module=0 );
};


#endif

/* end of fxchat_errhand.h */
