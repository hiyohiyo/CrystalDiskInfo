/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

//------------------------------------------------
// Option Flags
//------------------------------------------------

// For Task Tray Icon Feature
#define OPTION_TASK_TRAY

#if ! defined(_M_ARM) && ! defined(_M_ARM64)
// For JMicron USB RAID support
// This option is available only to the developer hiyohiyo.
#define JMICRON_USB_RAID_SUPPORT
#endif