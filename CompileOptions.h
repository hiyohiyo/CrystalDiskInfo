/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : MIT License
/*---------------------------------------------------------------------------*/

//------------------------------------------------
// Option Flags
//------------------------------------------------

// For Task Tray Icon Feature
#define OPTION_TASK_TRAY

#if ! defined(_M_ARM) && ! defined(_M_ARM64)
#define JMICRON_USB_RAID_SUPPORT
#endif
