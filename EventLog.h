/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

#pragma once

BOOL InstallEventSource();
BOOL WriteEventLog(DWORD eventId, WORD eventType, const TCHAR*/* PTCHAR*/ source, CString message);
BOOL UninstallEventSource();