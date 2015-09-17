//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: MSG_601
//
// MessageText:
//
//  %1!s!
//
#define MSG_601                          0x80000259L

//
// MessageId: MSG_602
//
// MessageText:
//
//  %1!s!
//
#define MSG_602                          0x8000025AL

//
// MessageId: MSG_603
//
// MessageText:
//
//  %1!s!
//
#define MSG_603                          0x8000025BL

//
// MessageId: MSG_604
//
// MessageText:
//
//  %1!s!
//
#define MSG_604                          0x8000025CL

//
// MessageId: MSG_605
//
// MessageText:
//
//  %1!s!
//
#define MSG_605                          0x8000025DL

//
// MessageId: MSG_606
//
// MessageText:
//
//  %1!s!
//
#define MSG_606                          0x8000025EL

//
// MessageId: MSG_701
//
// MessageText:
//
//  %1!s!
//
#define MSG_701                          0x400002BDL

//
// MessageId: MSG_702
//
// MessageText:
//
//  %1!s!
//
#define MSG_702                          0x400002BEL

//
// MessageId: MSG_703
//
// MessageText:
//
//  %1!s!
//
#define MSG_703                          0x400002BFL

//
// MessageId: MSG_704
//
// MessageText:
//
//  %1!s!
//
#define MSG_704                          0x400002C0L

//
// MessageId: MSG_705
//
// MessageText:
//
//  %1!s!
//
#define MSG_705                          0x400002C1L

