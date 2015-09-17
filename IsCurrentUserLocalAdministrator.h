// http://support.microsoft.com/kb/118626/en-us
// How To Determine Whether a Thread Is Running in User Context of Local Administrator Account

#pragma once

#ifdef _UNICODE
BOOL IsCurrentUserLocalAdministrator(VOID);
#endif