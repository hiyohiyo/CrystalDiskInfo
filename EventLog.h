/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

BOOL InstallEventSource();
BOOL WriteEventLog(DWORD eventId, WORD eventType, PTCHAR source, CString message);
BOOL UninstallEventSource();