/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : http://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

BOOL InstallEventSource();
BOOL WriteEventLog(DWORD eventId, WORD eventType, PTCHAR source, CString message);
BOOL UninstallEventSource();