/*
   FALCON - The Falcon Programming Language.
   FILE: fxchat_events.cpp

   Falcon script Xchat plugin
   Declaration of xchat event-to-parameter mappings.
   -------------------------------------------------------------------
   Author: Giancarlo Niccolai
   Begin: 2007-08-04 19:57:00

   -------------------------------------------------------------------
   (C) Copyright 2004: the FALCON developers (see list in AUTHORS file)

   See LICENSE file for licensing details.
*/

/** \file
   Falcon script Xchat plugin
   Declaration of xchat event-to-parameter mappings.
*/

#include "fxchat_events.h"

// Our maps...
ParamMap s_PMevent;
ParamMap s_PMsrvmsg;


void LoadEventParamMap()
{
   ParamDescList *p;

   p = &s_PMevent[ "Add Notify" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "Ban List" ];
      p->push_back( ParamDesc( "channel", "Channel Name" ) );
      p->push_back( ParamDesc( "mask", "Mask used for ban" ) );
      p->push_back( ParamDesc( "banner", "Who set the ban" ) );
      p->push_back( ParamDesc( "time", "Ban time" ) );

   p = &s_PMevent[ "Banned" ];
      p->push_back( ParamDesc( "channel", "Channel Name" ) );

   p = &s_PMevent[ "Beep" ];

   p = &s_PMevent[ "CTCP Generic" ];
      p->push_back( ParamDesc( "ctcp", "The CTCP event" ) );
      p->push_back( ParamDesc( "nick", "The nick of the person" ) );

   p = &s_PMevent[ "CTCP Generic to Channel" ];
      p->push_back( ParamDesc( "ctcp", "The CTCP event" ) );
      p->push_back( ParamDesc( "nick", "The nick of the person" ) );
      p->push_back( ParamDesc( "channel", "The Channel it's going to" ) );

   p = &s_PMevent[ "CTCP Send" ];
      p->push_back( ParamDesc( "receiver", "Receiver" ) );
      p->push_back( ParamDesc( "message", "Message" ) );

   p = &s_PMevent[ "CTCP Sound" ];
      p->push_back( ParamDesc( "sound", "The sound" ) );
      p->push_back( ParamDesc( "nick", "The nick of the person" ) );

   p = &s_PMevent[ "CTCP Sound to Channel" ];
      p->push_back( ParamDesc( "sound", "The sound" ) );
      p->push_back( ParamDesc( "nick", "The nick of the person" ) );
      p->push_back( ParamDesc( "channel", "The channel" ) );

   p = &s_PMevent[ "Change Nick" ];
      p->push_back( ParamDesc( "nick", "Old nickname" ) );
      p->push_back( ParamDesc( "newnick", "New nickname" ) );

   p = &s_PMevent[ "Channel Action" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "action", "The action" ) );
      p->push_back( ParamDesc( "mode", "Mode char" ) );

   p = &s_PMevent[ "Channel Action Hilight" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "action", "The action" ) );
      p->push_back( ParamDesc( "mode", "Mode char" ) );

   p = &s_PMevent[ "Channel Ban" ];
      p->push_back( ParamDesc( "banner", "The nick of the person who did the banning" ) );
      p->push_back( ParamDesc( "mask", "The ban mask" ) );

   p = &s_PMevent[ "Channel Creation" ];
      p->push_back( ParamDesc( "channel", "The channel" ) );
      p->push_back( ParamDesc( "time", "The time" ) );

   p = &s_PMevent[ "Channel DeHalfOp" ];
      p->push_back( ParamDesc( "opper", "The nick of the person of did the dehalfop'ing" ) );
      p->push_back( ParamDesc( "opped", "The nick of the person who has been dehalfop'ed" ) );

   p = &s_PMevent[ "Channel DeOp" ];
      p->push_back( ParamDesc( "opper", "The nick of the person of did the deop'ing" ) );
      p->push_back( ParamDesc( "opped", "The nick of the person who has been deop'ed" ) );

   p = &s_PMevent[ "Channel DeVoice" ];
      p->push_back( ParamDesc( "opper", "The nick of the person of did the devoice'ing" ) );
      p->push_back( ParamDesc( "opped", "The nick of the person who has been devoice'ed" ) );

   p = &s_PMevent[ "Channel Exempt" ];
      p->push_back( ParamDesc( "opper", "The nick of the person who did the exempt" ) );
      p->push_back( ParamDesc( "opped", "The exempt mask" ) );

   p = &s_PMevent[ "Channel Half-Operator" ];
      p->push_back( ParamDesc( "opper", "The nick of the person who has been halfop'ed" ) );
      p->push_back( ParamDesc( "opped", "The nick of the person who did the halfop'ing" ) );

   p = &s_PMevent[ "Channel INVITE" ];
      p->push_back( ParamDesc( "nick", "The nick of the person who did the invite" ) );
      p->push_back( ParamDesc( "mask", "The invite mask" ) );

   p = &s_PMevent[ "Channel List" ];

   p = &s_PMevent[ "Channel Message" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "message", "The text" ) );
      p->push_back( ParamDesc( "mode", "Mode char" ) );
      p->push_back( ParamDesc( "idtext", "Identified text" ) );

   p = &s_PMevent[ "Channel Mode Generic" ];
      p->push_back( ParamDesc( "nick", "The nick of the person setting the mode" ) );
      p->push_back( ParamDesc( "sign", "The mode's sign (+/-)" ) );
      p->push_back( ParamDesc( "mode", "The mode letter" ) );
      p->push_back( ParamDesc( "channel", "The channel it's being set on" ) );

   p = &s_PMevent[ "Channel Modes" ];
      p->push_back( ParamDesc( "channel", "Channel Name" ) );
      p->push_back( ParamDesc( "mode", "Modes string" ) );

   p = &s_PMevent[ "Channel Msg Hilight" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "text", "The text" ) );
      p->push_back( ParamDesc( "mode", "Mode char" ) );
      p->push_back( ParamDesc( "idtext", "Identified text" ) );

   p = &s_PMevent[ "Channel Notice" ];
      p->push_back( ParamDesc( "nick", "Who it's from" ) );
      p->push_back( ParamDesc( "channel", "The Channel it's going to" ) );
      p->push_back( ParamDesc( "message", "The message" ) );

   p = &s_PMevent[ "Channel Operator" ];
      p->push_back( ParamDesc( "opper", "The nick of the person who did the op'ing" ) );
      p->push_back( ParamDesc( "opped", "The nick of the person who has been op'ed" ) );

   p = &s_PMevent[ "Channel Remove Exempt" ];
      p->push_back( ParamDesc( "nick", "The nick of the person removed the exempt" ) );
      p->push_back( ParamDesc( "mask", "The exempt mask" ) );

   p = &s_PMevent[ "Channel Remove Invite" ];
      p->push_back( ParamDesc( "nick", "The nick of the person removed the invite" ) );
      p->push_back( ParamDesc( "mask", "The invite mask" ) );

   p = &s_PMevent[ "Channel Remove Keyword" ];
      p->push_back( ParamDesc( "nick", "The nick who removed the key" ) );

   p = &s_PMevent[ "Channel Remove Limit" ];
      p->push_back( ParamDesc( "nick", "The nick who removed the limit" ) );

   p = &s_PMevent[ "Channel Set Key" ];
      p->push_back( ParamDesc( "nick", "The nick of the person who set the key" ) );
      p->push_back( ParamDesc( "key", "The key" ) );

   p = &s_PMevent[ "Channel Set Limit" ];
      p->push_back( ParamDesc( "nick", "The nick of the person who set the limit" ) );
      p->push_back( ParamDesc( "limit", "The limit" ) );

   p = &s_PMevent[ "Channel UnBan" ];
      p->push_back( ParamDesc( "nick", "The nick of the person of did the unban'ing" ) );
      p->push_back( ParamDesc( "mask", "The ban mask" ) );

   p = &s_PMevent[ "Channel Voice" ];
      p->push_back( ParamDesc( "opper", "The nick of the person who did the voice'ing" ) );
      p->push_back( ParamDesc( "opped", "The nick of the person who has been voice'ed" ) );

   p = &s_PMevent[ "Connected" ];

   p = &s_PMevent[ "Connecting" ];
      p->push_back( ParamDesc( "host", "Host" ) );
      p->push_back( ParamDesc( "ip", "IP" ) );
      p->push_back( ParamDesc( "port", "Port" ) );

   p = &s_PMevent[ "Connection Failed" ];
      p->push_back( ParamDesc( "error", "Error" ) );

   p = &s_PMevent[ "DCC CHAT Abort" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "DCC CHAT Connect" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "nick", "IP address" ) );

   p = &s_PMevent[ "DCC CHAT Failed" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "ip", "IP address" ) );
      p->push_back( ParamDesc( "port", "Port" ) );
      p->push_back( ParamDesc( "error", "Error" ) );

   p = &s_PMevent[ "DCC CHAT Offer" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "DCC CHAT Offering" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "DCC CHAT Reoffer" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "DCC Conection Failed" ];
      p->push_back( ParamDesc( "type", "DCC Type" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "error", "Error" ) );

   p = &s_PMevent[ "DCC Generic Offer" ];
      p->push_back( ParamDesc( "dcc", "DCC String" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "DCC Header" ];

   p = &s_PMevent[ "DCC Malformed" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "packet", "The Packet" ) );

   p = &s_PMevent[ "DCC Offer" ];
      p->push_back( ParamDesc( "file", "Filename" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "path", "Pathname" ) );

   p = &s_PMevent[ "DCC Offer Not Valid" ];

   p = &s_PMevent[ "DCC RECV Abort" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "file", "Filename" ) );

   p = &s_PMevent[ "DCC RECV Complete" ];
      p->push_back( ParamDesc( "file", "Filename" ) );
      p->push_back( ParamDesc( "destfile", "Destination filename" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "cps", "CPS - transfer speed in character per seconds" ) );

   p = &s_PMevent[ "DCC RECV Connect" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "ip", "IP address" ) );
      p->push_back( ParamDesc( "file", "Filename" ) );

   p = &s_PMevent[ "DCC RECV Failed" ];
      p->push_back( ParamDesc( "file", "Filename" ) );
      p->push_back( ParamDesc( "destfile", "Destination filename" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "error", "Error" ) );

   p = &s_PMevent[ "DCC RECV File Open Error" ];
      p->push_back( ParamDesc( "Filename", "Filename" ) );
      p->push_back( ParamDesc( "error", "Error" ) );

   p = &s_PMevent[ "DCC RESUME Request" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "error", "Filename" ) );
      p->push_back( ParamDesc( "position", "Position" ) );

   p = &s_PMevent[ "DCC Rename" ];
      p->push_back( ParamDesc( "file", "Old Filename" ) );
      p->push_back( ParamDesc( "newfile", "New Filename" ) );

   p = &s_PMevent[ "DCC SEND Abort" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "file", "Filename" ) );

   p = &s_PMevent[ "DCC SEND Complete" ];
      p->push_back( ParamDesc( "file", "Filename" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "cps", "CPS" ) );

   p = &s_PMevent[ "DCC SEND Connect" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "ip", "IP address" ) );
      p->push_back( ParamDesc( "file", "Filename" ) );

   p = &s_PMevent[ "DCC SEND Failed" ];
      p->push_back( ParamDesc( "file", "Filename" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "error", "Error" ) );

   p = &s_PMevent[ "DCC SEND Offer" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "file", "Filename" ) );
      p->push_back( ParamDesc( "size", "Size" ) );
      p->push_back( ParamDesc( "ip", "IP address" ) );

   p = &s_PMevent[ "DCC Stall" ];
      p->push_back( ParamDesc( "", "DCC Type" ) );
      p->push_back( ParamDesc( "file", "Filename" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "DCC Timeout" ];
      p->push_back( ParamDesc( "type", "DCC Type" ) );
      p->push_back( ParamDesc( "file", "Filename" ) );
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "Delete Notify" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "Disconnected" ];
      p->push_back( ParamDesc( "error", "Error" ) );

   p = &s_PMevent[ "Found IP" ];
      p->push_back( ParamDesc( "ip", "IP" ) );

   p = &s_PMevent[ "Generic Message" ];
      p->push_back( ParamDesc( "left", "Left message" ) );
      p->push_back( ParamDesc( "right", "Right message" ) );

   p = &s_PMevent[ "Ignore Add" ];
      p->push_back( ParamDesc( "mask", "Hostmask" ) );

   p = &s_PMevent[ "Ignore Changed" ];
      p->push_back( ParamDesc( "mask", "Hostmask" ) );

   p = &s_PMevent[ "Ignore Footer" ];

   p = &s_PMevent[ "Ignore Header" ];

   p = &s_PMevent[ "Ignore Remove" ];
      p->push_back( ParamDesc( "mask", "Hostmask" ) );

   p = &s_PMevent[ "Ignorelist Empty" ];

   p = &s_PMevent[ "Invite" ];
      p->push_back( ParamDesc( "name", "Channel Name" ) );

   p = &s_PMevent[ "Invited" ];
      p->push_back( ParamDesc( "name", "Channel Name" ) );
      p->push_back( ParamDesc( "nick", "Nick of person who invited you" ) );
      p->push_back( ParamDesc( "server", "Server Name" ) );

   p = &s_PMevent[ "Join" ];
      p->push_back( ParamDesc( "nick", "The nick of the joining person" ) );
      p->push_back( ParamDesc( "channel", "The channel being joined" ) );
      p->push_back( ParamDesc( "host", "The host of the person" ) );

   p = &s_PMevent[ "Keyword" ];
      p->push_back( ParamDesc( "name", "Channel Name" ) );

   p = &s_PMevent[ "Kick" ];
      p->push_back( ParamDesc( "kicker", "The nickname of the kicker" ) );
      p->push_back( ParamDesc( "kicker", "The person being kicked" ) );
      p->push_back( ParamDesc( "channel", "The channel" ) );
      p->push_back( ParamDesc( "reason", "The reason" ) );

   p = &s_PMevent[ "Killed" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "reason", "Reason" ) );

   p = &s_PMevent[ "MOTD Skipped" ];

   p = &s_PMevent[ "Message Send" ];
      p->push_back( ParamDesc( "receiver", "Receiver" ) );
      p->push_back( ParamDesc( "message", "Message" ) );

   p = &s_PMevent[ "Motd" ];
      p->push_back( ParamDesc( "tesxt", "Text" ) );
      p->push_back( ParamDesc( "server", "Server Name" ) );

   p = &s_PMevent[ "Nick Clash" ];
      p->push_back( ParamDesc( "nick", "Nickname in use" ) );
      p->push_back( ParamDesc( "newnick", "Nick being tried" ) );

   p = &s_PMevent[ "Nick Failed" ];

   p = &s_PMevent[ "No DCC" ];

   p = &s_PMevent[ "No Running Process" ];

   p = &s_PMevent[ "Notice" ];
      p->push_back( ParamDesc( "from", "Who it's from" ) );
      p->push_back( ParamDesc( "message", "The message" ) );

   p = &s_PMevent[ "Notice Send" ];
      p->push_back( ParamDesc( "receiver", "Receiver" ) );
      p->push_back( ParamDesc( "message", "Message" ) );

   p = &s_PMevent[ "Notify Empty" ];

   p = &s_PMevent[ "Notify Header" ];

   p = &s_PMevent[ "Notify Number" ];
      p->push_back( ParamDesc( "number", "Number of notify items" ) );

   p = &s_PMevent[ "Notify Offline" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "server", "Server Name" ) );
      p->push_back( ParamDesc( "network", "Network" ) );

   p = &s_PMevent[ "Notify Online" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "server", "Server Name" ) );
      p->push_back( ParamDesc( "network", "Network" ) );

   p = &s_PMevent[ "Open Dialog" ];

   p = &s_PMevent[ "Part" ];
      p->push_back( ParamDesc( "nick", "The nick of the person leaving" ) );
      p->push_back( ParamDesc( "host", "The host of the person" ) );
      p->push_back( ParamDesc( "channel", "The channel" ) );

   p = &s_PMevent[ "Part with Reason" ];
      p->push_back( ParamDesc( "nick", "The nick of the person leaving" ) );
      p->push_back( ParamDesc( "host", "The host of the person" ) );
      p->push_back( ParamDesc( "channel", "The channel" ) );
      p->push_back( ParamDesc( "reason", "The reason" ) );

   p = &s_PMevent[ "Ping Reply" ];
      p->push_back( ParamDesc( "from", "Who it's from" ) );
      p->push_back( ParamDesc( "time", "The time in x.x format (see below)" ) );

   p = &s_PMevent[ "Ping Timeout" ];
      p->push_back( ParamDesc( "seconds", "Seconds" ) );

   p = &s_PMevent[ "Private Message" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "message", "The message" ) );
      p->push_back( ParamDesc( "idText", "Identified text" ) );

   p = &s_PMevent[ "Private Message to Dialog" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "message", "The message" ) );
      p->push_back( ParamDesc( "idText", "Identified text" ) );

   p = &s_PMevent[ "Process Already Running" ];

   p = &s_PMevent[ "Quit" ];
      p->push_back( ParamDesc( "nick", "Nick" ) );
      p->push_back( ParamDesc( "reason", "Reason" ) );
      p->push_back( ParamDesc( "host", "Host" ) );

   p = &s_PMevent[ "Raw Modes" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "modes", "Modes string" ) );

   p = &s_PMevent[ "Receive Wallops" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "message", "The message" ) );

   p = &s_PMevent[ "Resolving User" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "host", "Hostname" ) );

   p = &s_PMevent[ "Server Connected" ];

   p = &s_PMevent[ "Server Error" ];
      p->push_back( ParamDesc( "text", "Text" ) );

   p = &s_PMevent[ "Server Lookup" ];
      p->push_back( ParamDesc( "server", "Server Name" ) );

   p = &s_PMevent[ "Server Notice" ];
      p->push_back( ParamDesc( "text", "Text" ) );
      p->push_back( ParamDesc( "server", "Server Name" ) );

   p = &s_PMevent[ "Server Text" ];
      p->push_back( ParamDesc( "text", "Text" ) );
      p->push_back( ParamDesc( "server", "Server Name" ) );

   p = &s_PMevent[ "Stop Connection" ];

   p = &s_PMevent[ "Topic" ];
      p->push_back( ParamDesc( "channel", "Channel" ) );
      p->push_back( ParamDesc( "topic", "Topic" ) );

   p = &s_PMevent[ "Topic Change" ];
      p->push_back( ParamDesc( "", "Nick of person who changed the topic" ) );
      p->push_back( ParamDesc( "topic", "Topic" ) );
      p->push_back( ParamDesc( "channel", "Channel" ) );

   p = &s_PMevent[ "Topic Creation" ];
      p->push_back( ParamDesc( "channel", "The channel" ) );
      p->push_back( ParamDesc( "creator", "The creator" ) );
      p->push_back( ParamDesc( "time", "The time" ) );

   p = &s_PMevent[ "Unknown Host" ];

   p = &s_PMevent[ "User Limit" ];
      p->push_back( ParamDesc( "name", "Channel Name" ) );

   p = &s_PMevent[ "Users On Channel" ];
      p->push_back( ParamDesc( "name", "Channel Name" ) );
      p->push_back( ParamDesc( "users", "Users" ) );

   p = &s_PMevent[ "WhoIs Authenticated" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "message", "Message" ) );
      p->push_back( ParamDesc( "account", "Account" ) );

   p = &s_PMevent[ "WhoIs Away Line" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "reason", "Away reason" ) );

   p = &s_PMevent[ "WhoIs Channel/Oper Line" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "member", "Channel Membership/\"is an IRC operator\"" ) );

   p = &s_PMevent[ "WhoIs End" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );

   p = &s_PMevent[ "WhoIs Identified" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "message", "Message" ) );

   p = &s_PMevent[ "WhoIs Idle Line" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "idle", "Idle time" ) );

   p = &s_PMevent[ "WhoIs Idle Line with Signon" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "idle", "Idle time" ) );
      p->push_back( ParamDesc( "signon", "Signon time" ) );

   p = &s_PMevent[ "WhoIs Name Line" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "user", "Username" ) );
      p->push_back( ParamDesc( "host", "Host" ) );
      p->push_back( ParamDesc( "name", "Full name" ) );

   p = &s_PMevent[ "WhoIs Real Host" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "real", "Real user@host" ) );
      p->push_back( ParamDesc( "ip", "Real IP" ) );
      p->push_back( ParamDesc( "message", "Message" ) );

   p = &s_PMevent[ "WhoIs Server Line" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "info", "Server Information" ) );

   p = &s_PMevent[ "WhoIs Special" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "message", "Message" ) );

   p = &s_PMevent[ "You Join" ];
      p->push_back( ParamDesc( "nick", "The nick of the joining person" ) );
      p->push_back( ParamDesc( "channel", "The channel being joined" ) );
      p->push_back( ParamDesc( "host", "The host of the person" ) );

   p = &s_PMevent[ "You Kicked" ];
      p->push_back( ParamDesc( "kicked", "The person being kicked" ) );
      p->push_back( ParamDesc( "channel", "The channel" ) );
      p->push_back( ParamDesc( "kicker", "The nickname of the kicker" ) );
      p->push_back( ParamDesc( "reason", "The reason" ) );

   p = &s_PMevent[ "You Part" ];
      p->push_back( ParamDesc( "nick", "The nick of the person leaving" ) );
      p->push_back( ParamDesc( "host", "The host of the person" ) );
      p->push_back( ParamDesc( "channel", "The channel" ) );

   p = &s_PMevent[ "You Part with Reason" ];
      p->push_back( ParamDesc( "nick", "The nick of the person leaving" ) );
      p->push_back( ParamDesc( "host", "The host of the person" ) );
      p->push_back( ParamDesc( "channel", "The channel" ) );
      p->push_back( ParamDesc( "reason", "The reason" ) );

   p = &s_PMevent[ "Your Action" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "action", "The action" ) );
      p->push_back( ParamDesc( "mode", "Mode char" ) );

   p = &s_PMevent[ "Your Invitation" ];
      p->push_back( ParamDesc( "", "Nick of person who have been invited" ) );
      p->push_back( ParamDesc( "name", "Channel Name" ) );
      p->push_back( ParamDesc( "server", "Server Name" ) );

   p = &s_PMevent[ "Your Message" ];
      p->push_back( ParamDesc( "nick", "Nickname" ) );
      p->push_back( ParamDesc( "text", "The text" ) );
      p->push_back( ParamDesc( "mode", "Mode char" ) );
      p->push_back( ParamDesc( "idText", "Identified text" ) );

   p = &s_PMevent[ "Your Nick Changing" ];
      p->push_back( ParamDesc( "", "Old nickname" ) );
      p->push_back( ParamDesc( "", "New nickname" ) );

//===============================================
// Special events created by hand.
   p = &s_PMevent[ "Open Context" ];

   p = &s_PMevent[ "Close Context" ];

   p = &s_PMevent[ "Focus Tab" ];

   p = &s_PMevent[ "Focus Window" ];

   p = &s_PMevent[ "Focus Window" ];

   p = &s_PMevent[ "DCC Chat Text" ];
      p->push_back( ParamDesc( "address", "Address" ) );
      p->push_back( ParamDesc( "port", "Port" ) );
      p->push_back( ParamDesc( "nick", "Nick" ) );
      p->push_back( ParamDesc( "message", "Message" ) );

   p = &s_PMevent[ "Key Press" ];
      p->push_back( ParamDesc( "key", "Key Value" ) );
      p->push_back( ParamDesc( "state", "State Bitfield (shift, capslock, alt)" ) );
      p->push_back( ParamDesc( "string", "String version of the key" ) );
      p->push_back( ParamDesc( "length", "Length of the string (may be 0 for unprintable keys)" ) );
}


void LoadSrvMsgParamMap()
{
   ParamDescList *p;

   p = &s_PMsrvmsg[ "NICK" ];
      p->push_back( ParamDesc( "nick:all", "Complete nickname of the user changing nick" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "newnick", "The new nickname" ) );

   p = &s_PMsrvmsg[ "USER" ];
      p->push_back( ParamDesc( "nick:all", "Complete nickname of the user" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "user", "User name" ) );
      p->push_back( ParamDesc( "host", "Host" ) );
      p->push_back( ParamDesc( "server", "Server name" ) );
      p->push_back( ParamDesc( "name", "Real user name" ) );

   p = &s_PMsrvmsg[ "SERVER" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "server", "Name of the remote server" ) );
      p->push_back( ParamDesc( "hopcount", "Hop count to reach the server" ) );
      p->push_back( ParamDesc( "info", "Extra server info" ) );

   p = &s_PMsrvmsg[ "SERVER" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "server", "Name of the remote server" ) );
      p->push_back( ParamDesc( "hopcount", "Hop count to reach the server" ) );
      p->push_back( ParamDesc( "info", "Extra server info" ) );

   p = &s_PMsrvmsg[ "SQUIT" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "server", "Name of the remote server" ) );
      p->push_back( ParamDesc( "comment", "Hop count to reach the server" ) );

   p = &s_PMsrvmsg[ "JOIN" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "channel", "Joined channel" ) );

   p = &s_PMsrvmsg[ "PART" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "channel", "Parted channel" ) );
      p->push_back( ParamDesc( "reason", "Reason" ) );

   p = &s_PMsrvmsg[ "MODE" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "target", "target of mode change" ) );
      p->push_back( ParamDesc( "mode", "set mode" ) );

   p = &s_PMsrvmsg[ "TOPIC" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "channel", "Channel where the topic is set" ) );
      p->push_back( ParamDesc( "topic", "The topic" ) );

      // names is unmanaged
      // list is unmanaged
   p = &s_PMsrvmsg[ "INVITE" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "invited", "Invited nick" ) );
      p->push_back( ParamDesc( "channel", "Channel where the nick is invited" ) );

   p = &s_PMsrvmsg[ "KICK" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "channel", "Channel where the nick is kicked" ) );
      p->push_back( ParamDesc( "kicked", "Kicked nick" ) );

   p = &s_PMsrvmsg[ "VERSION" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "mask", "Mask used for versioning" ) );

   p = &s_PMsrvmsg[ "PRIVMSG" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "target", "Nick or channel where the privmsg is headed" ) );
      p->push_back( ParamDesc( "message", "Sent message" ) );

   p = &s_PMsrvmsg[ "NOTIFY" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "target", "Nick or channel where the privmsg is headed" ) );
      p->push_back( ParamDesc( "message", "Sent message" ) );

   p = &s_PMsrvmsg[ "PONG" ];
      p->push_back( ParamDesc( "nick:all", "Complete name of the entity sending the message" ) );
      p->push_back( ParamDesc( "event", "The name of the sent event" ) );
      p->push_back( ParamDesc( "entity", "Entity at the other end of the pong" ) );
      p->push_back( ParamDesc( "lag", "Lag string" ) );

}

/* end of fxchat_events.cpp */
