/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_ext.h

   Falcon script Xchat plugin
   Interface extension functions - header file
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-07-28 18:55:17.075

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin
   Interface extension functions - header file
*/

#ifndef fxchat_ext_H
#define fxchat_ext_H

#include <falcon/module.h>

namespace Falcon {

Module *create_xchat_module();

namespace Ext {

FALCON_FUNC  XChat_command( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_emit( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_sendModes( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_findContext( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_getContext( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_getInfo( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_getPrefs( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_nickcmp( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_strip( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_listChannels( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_listDcc( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_listUsers( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_listNotify( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_listIgnore( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_list( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_hookCommand( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_hookPrint( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_hookServer( ::Falcon::VMachine *vm );
FALCON_FUNC  XChat_hookTimer( ::Falcon::VMachine *vm );

FALCON_FUNC  XChatContext_set( ::Falcon::VMachine *vm );

FALCON_FUNC  XChatHook_unhook( ::Falcon::VMachine *vm );

}
}

#endif

/* end of fxchat_ext.h */
