/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat.h

   Falcon script Xchat plugin
   Integration with Xchat
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
   Definitions
*/

#ifndef fxchat_H
#define fxchat_H

#include <falcon/engine.h>
#include "xchat-plugin.h"

#define PNAME "fxchat"
#define PDESC "Falcon xchat interface";

void xchat_print_falcon( const Falcon::String &str );

class ScriptData;
void UnloadModule( ScriptData *mod );

// The plugin handle pointer, used in the various modules.
extern xchat_plugin *the_plugin;
extern Falcon::Module *s_modCore;
extern Falcon::Module *s_modXchat;

// interface with X-Chat
extern "C" {
void xchat_plugin_get_info(char **name, char **desc, char **version, void **reserved);
int xchat_plugin_init(xchat_plugin *plugin_handle,
                      char **plugin_name,
                      char **plugin_desc,
                      char **plugin_version,
                      char *arg);

int xchat_plugin_deinit();
}
#endif

/* end of fxchat.h */
