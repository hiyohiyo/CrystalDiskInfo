/* 
 * QR Code generator library (C)
 * 
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/qr-code-generator-library
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#pragma once

/// #include <stdbool.h>
#ifndef _STDBOOL
#define _STDBOOL

#define __bool_true_false_are_defined 1

#ifndef __cplusplus

#define bool  _Bool
#define false 0
#define true  1

#endif /* __cplusplus */

#endif /* _STDBOOL */

#include <stddef.h>

///#include <stdint.h>
#define _STDINT

//#include <vcruntime.h>

//#if _VCRT_COMPILER_PREPROCESSOR

//#pragma warning(push)
//#pragma warning(disable: _VCRUNTIME_DISABLED_WARNINGS)

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
//typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
//typedef unsigned long long uint64_t;

typedef signed char        int_least8_t;
typedef short              int_least16_t;
typedef int                int_least32_t;
//typedef long long          int_least64_t;
typedef unsigned char      uint_least8_t;
typedef unsigned short     uint_least16_t;
typedef unsigned int       uint_least32_t;
//typedef unsigned long long uint_least64_t;

typedef signed char        int_fast8_t;
typedef int                int_fast16_t;
typedef int                int_fast32_t;
//typedef long long          int_fast64_t;
typedef unsigned char      uint_fast8_t;
typedef unsigned int       uint_fast16_t;
typedef unsigned int       uint_fast32_t;
//typedef unsigned long long uint_fast64_t;

//typedef long long          intmax_t;
//typedef unsigned long long uintmax_t;

// These macros must exactly match those in the Windows SDK's intsafe.h.
#define INT8_MIN         (-127i8 - 1)
#define INT16_MIN        (-32767i16 - 1)
#define INT32_MIN        (-2147483647i32 - 1)
#define INT64_MIN        (-9223372036854775807i64 - 1)
#define INT8_MAX         127i8
#define INT16_MAX        32767i16
#define INT32_MAX        2147483647i32
#define INT64_MAX        9223372036854775807i64
#define UINT8_MAX        0xffui8
#define UINT16_MAX       0xffffui16
#define UINT32_MAX       0xffffffffui32
#define UINT64_MAX       0xffffffffffffffffui64

#define INT_LEAST8_MIN   INT8_MIN
#define INT_LEAST16_MIN  INT16_MIN
#define INT_LEAST32_MIN  INT32_MIN
#define INT_LEAST64_MIN  INT64_MIN
#define INT_LEAST8_MAX   INT8_MAX
#define INT_LEAST16_MAX  INT16_MAX
#define INT_LEAST32_MAX  INT32_MAX
#define INT_LEAST64_MAX  INT64_MAX
#define UINT_LEAST8_MAX  UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INT_FAST8_MIN    INT8_MIN
#define INT_FAST16_MIN   INT32_MIN
#define INT_FAST32_MIN   INT32_MIN
#define INT_FAST64_MIN   INT64_MIN
#define INT_FAST8_MAX    INT8_MAX
#define INT_FAST16_MAX   INT32_MAX
#define INT_FAST32_MAX   INT32_MAX
#define INT_FAST64_MAX   INT64_MAX
#define UINT_FAST8_MAX   UINT8_MAX
#define UINT_FAST16_MAX  UINT32_MAX
#define UINT_FAST32_MAX  UINT32_MAX
#define UINT_FAST64_MAX  UINT64_MAX

#ifdef _WIN64
#define INTPTR_MIN   INT64_MIN
#define INTPTR_MAX   INT64_MAX
#define UINTPTR_MAX  UINT64_MAX
#else
#define INTPTR_MIN   INT32_MIN
#define INTPTR_MAX   INT32_MAX
#define UINTPTR_MAX  UINT32_MAX
#endif

#define INTMAX_MIN       INT64_MIN
#define INTMAX_MAX       INT64_MAX
#define UINTMAX_MAX      UINT64_MAX

#define PTRDIFF_MIN      INTPTR_MIN
#define PTRDIFF_MAX      INTPTR_MAX

#ifndef SIZE_MAX
	// SIZE_MAX definition must match exactly with limits.h for modules support.
#ifdef _WIN64
#define SIZE_MAX 0xffffffffffffffffui64
#else
#define SIZE_MAX 0xffffffffui32
#endif
#endif

#define SIG_ATOMIC_MIN   INT32_MIN
#define SIG_ATOMIC_MAX   INT32_MAX

//#define WCHAR_MIN        0x0000
//#define WCHAR_MAX        0xffff

#define WINT_MIN         0x0000
#define WINT_MAX         0xffff

#define INT8_C(x)    (x)
#define INT16_C(x)   (x)
#define INT32_C(x)   (x)
#define INT64_C(x)   (x ## LL)

#define UINT8_C(x)   (x)
#define UINT16_C(x)  (x)
#define UINT32_C(x)  (x ## U)
#define UINT64_C(x)  (x ## ULL)

#define INTMAX_C(x)  INT64_C(x)
#define UINTMAX_C(x) UINT64_C(x)

//#pragma warning(pop) // _VCRUNTIME_DISABLED_WARNINGS

//#endif // _VCRT_COMPILER_PREPROCESSOR

#ifdef __cplusplus
extern "C" {
#endif


/* 
 * This library creates QR Code symbols, which is a type of two-dimension barcode.
 * Invented by Denso Wave and described in the ISO/IEC 18004 standard.
 * A QR Code structure is an immutable square grid of dark and light cells.
 * The library provides functions to create a QR Code from text or binary data.
 * The library covers the QR Code Model 2 specification, supporting all versions (sizes)
 * from 1 to 40, all 4 error correction levels, and 4 character encoding modes.
 * 
 * Ways to create a QR Code object:
 * - High level: Take the payload data and call qrcodegen_encodeText() or qrcodegen_encodeBinary().
 * - Low level: Custom-make the list of segments and call
 *   qrcodegen_encodeSegments() or qrcodegen_encodeSegmentsAdvanced().
 * (Note that all ways require supplying the desired error correction level and various byte buffers.)
 */


/*---- Enum and struct types----*/

/* 
 * The error correction level in a QR Code symbol.
 */
enum qrcodegen_Ecc {
	// Must be declared in ascending order of error protection
	// so that an internal qrcodegen function works properly
	qrcodegen_Ecc_LOW = 0 ,  // The QR Code can tolerate about  7% erroneous codewords
	qrcodegen_Ecc_MEDIUM  ,  // The QR Code can tolerate about 15% erroneous codewords
	qrcodegen_Ecc_QUARTILE,  // The QR Code can tolerate about 25% erroneous codewords
	qrcodegen_Ecc_HIGH    ,  // The QR Code can tolerate about 30% erroneous codewords
};


/* 
 * The mask pattern used in a QR Code symbol.
 */
enum qrcodegen_Mask {
	// A special value to tell the QR Code encoder to
	// automatically select an appropriate mask pattern
	qrcodegen_Mask_AUTO = -1,
	// The eight actual mask patterns
	qrcodegen_Mask_0 = 0,
	qrcodegen_Mask_1,
	qrcodegen_Mask_2,
	qrcodegen_Mask_3,
	qrcodegen_Mask_4,
	qrcodegen_Mask_5,
	qrcodegen_Mask_6,
	qrcodegen_Mask_7,
};


/* 
 * Describes how a segment's data bits are interpreted.
 */
enum qrcodegen_Mode {
	qrcodegen_Mode_NUMERIC      = 0x1,
	qrcodegen_Mode_ALPHANUMERIC = 0x2,
	qrcodegen_Mode_BYTE         = 0x4,
	qrcodegen_Mode_KANJI        = 0x8,
	qrcodegen_Mode_ECI          = 0x7,
};


/* 
 * A segment of character/binary/control data in a QR Code symbol.
 * The mid-level way to create a segment is to take the payload data
 * and call a factory function such as qrcodegen_makeNumeric().
 * The low-level way to create a segment is to custom-make the bit buffer
 * and initialize a qrcodegen_Segment struct with appropriate values.
 * Even in the most favorable conditions, a QR Code can only hold 7089 characters of data.
 * Any segment longer than this is meaningless for the purpose of generating QR Codes.
 * Moreover, the maximum allowed bit length is 32767 because
 * the largest QR Code (version 40) has 31329 modules.
 */
struct qrcodegen_Segment {
	// The mode indicator of this segment.
	enum qrcodegen_Mode mode;
	
	// The length of this segment's unencoded data. Measured in characters for
	// numeric/alphanumeric/kanji mode, bytes for byte mode, and 0 for ECI mode.
	// Always zero or positive. Not the same as the data's bit length.
	int numChars;
	
	// The data bits of this segment, packed in bitwise big endian.
	// Can be null if the bit length is zero.
	uint8_t *data;
	
	// The number of valid data bits used in the buffer. Requires
	// 0 <= bitLength <= 32767, and bitLength <= (capacity of data array) * 8.
	// The character count (numChars) must agree with the mode and the bit buffer length.
	int bitLength;
};



/*---- Macro constants and functions ----*/

#define qrcodegen_VERSION_MIN   1  // The minimum version number supported in the QR Code Model 2 standard
#define qrcodegen_VERSION_MAX  40  // The maximum version number supported in the QR Code Model 2 standard

// Calculates the number of bytes needed to store any QR Code up to and including the given version number,
// as a compile-time constant. For example, 'uint8_t buffer[qrcodegen_BUFFER_LEN_FOR_VERSION(25)];'
// can store any single QR Code from version 1 to 25 (inclusive). The result fits in an int (or int16).
// Requires qrcodegen_VERSION_MIN <= n <= qrcodegen_VERSION_MAX.
#define qrcodegen_BUFFER_LEN_FOR_VERSION(n)  ((((n) * 4 + 17) * ((n) * 4 + 17) + 7) / 8 + 1)

// The worst-case number of bytes needed to store one QR Code, up to and including
// version 40. This value equals 3918, which is just under 4 kilobytes.
// Use this more convenient value to avoid calculating tighter memory bounds for buffers.
#define qrcodegen_BUFFER_LEN_MAX  qrcodegen_BUFFER_LEN_FOR_VERSION(qrcodegen_VERSION_MAX)



/*---- Functions (high level) to generate QR Codes ----*/

/* 
 * Encodes the given text string to a QR Code, returning true if successful.
 * If the data is too long to fit in any version in the given range
 * at the given ECC level, then false is returned.
 * 
 * The input text must be encoded in UTF-8 and contain no NULs.
 * Requires 1 <= minVersion <= maxVersion <= 40.
 * 
 * The smallest possible QR Code version within the given range is automatically
 * chosen for the output. Iff boostEcl is true, then the ECC level of the result
 * may be higher than the ecl argument if it can be done without increasing the
 * version. The mask is either between qrcodegen_Mask_0 to 7 to force that mask, or
 * qrcodegen_Mask_AUTO to automatically choose an appropriate mask (which may be slow).
 * 
 * About the arrays, letting len = qrcodegen_BUFFER_LEN_FOR_VERSION(maxVersion):
 * - Before calling the function:
 *   - The array ranges tempBuffer[0 : len] and qrcode[0 : len] must allow
 *     reading and writing; hence each array must have a length of at least len.
 *   - The two ranges must not overlap (aliasing).
 *   - The initial state of both ranges can be uninitialized
 *     because the function always writes before reading.
 * - After the function returns:
 *   - Both ranges have no guarantee on which elements are initialized and what values are stored.
 *   - tempBuffer contains no useful data and should be treated as entirely uninitialized.
 *   - If successful, qrcode can be passed into qrcodegen_getSize() and qrcodegen_getModule().
 * 
 * If successful, the resulting QR Code may use numeric,
 * alphanumeric, or byte mode to encode the text.
 * 
 * In the most optimistic case, a QR Code at version 40 with low ECC
 * can hold any UTF-8 string up to 2953 bytes, or any alphanumeric string
 * up to 4296 characters, or any digit string up to 7089 characters.
 * These numbers represent the hard upper limit of the QR Code standard.
 * 
 * Please consult the QR Code specification for information on
 * data capacities per version, ECC level, and text encoding mode.
 */
bool qrcodegen_encodeText(const char *text, uint8_t tempBuffer[], uint8_t qrcode[],
	enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl);


/* 
 * Encodes the given binary data to a QR Code, returning true if successful.
 * If the data is too long to fit in any version in the given range
 * at the given ECC level, then false is returned.
 * 
 * Requires 1 <= minVersion <= maxVersion <= 40.
 * 
 * The smallest possible QR Code version within the given range is automatically
 * chosen for the output. Iff boostEcl is true, then the ECC level of the result
 * may be higher than the ecl argument if it can be done without increasing the
 * version. The mask is either between qrcodegen_Mask_0 to 7 to force that mask, or
 * qrcodegen_Mask_AUTO to automatically choose an appropriate mask (which may be slow).
 * 
 * About the arrays, letting len = qrcodegen_BUFFER_LEN_FOR_VERSION(maxVersion):
 * - Before calling the function:
 *   - The array ranges dataAndTemp[0 : len] and qrcode[0 : len] must allow
 *     reading and writing; hence each array must have a length of at least len.
 *   - The two ranges must not overlap (aliasing).
 *   - The input array range dataAndTemp[0 : dataLen] should normally be
 *     valid UTF-8 text, but is not required by the QR Code standard.
 *   - The initial state of dataAndTemp[dataLen : len] and qrcode[0 : len]
 *     can be uninitialized because the function always writes before reading.
 * - After the function returns:
 *   - Both ranges have no guarantee on which elements are initialized and what values are stored.
 *   - dataAndTemp contains no useful data and should be treated as entirely uninitialized.
 *   - If successful, qrcode can be passed into qrcodegen_getSize() and qrcodegen_getModule().
 * 
 * If successful, the resulting QR Code will use byte mode to encode the data.
 * 
 * In the most optimistic case, a QR Code at version 40 with low ECC can hold any byte
 * sequence up to length 2953. This is the hard upper limit of the QR Code standard.
 * 
 * Please consult the QR Code specification for information on
 * data capacities per version, ECC level, and text encoding mode.
 */
bool qrcodegen_encodeBinary(uint8_t dataAndTemp[], size_t dataLen, uint8_t qrcode[],
	enum qrcodegen_Ecc ecl, int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl);


/*---- Functions (low level) to generate QR Codes ----*/

/* 
 * Encodes the given segments to a QR Code, returning true if successful.
 * If the data is too long to fit in any version at the given ECC level,
 * then false is returned.
 * 
 * The smallest possible QR Code version is automatically chosen for
 * the output. The ECC level of the result may be higher than the
 * ecl argument if it can be done without increasing the version.
 * 
 * About the byte arrays, letting len = qrcodegen_BUFFER_LEN_FOR_VERSION(qrcodegen_VERSION_MAX):
 * - Before calling the function:
 *   - The array ranges tempBuffer[0 : len] and qrcode[0 : len] must allow
 *     reading and writing; hence each array must have a length of at least len.
 *   - The two ranges must not overlap (aliasing).
 *   - The initial state of both ranges can be uninitialized
 *     because the function always writes before reading.
 *   - The input array segs can contain segments whose data buffers overlap with tempBuffer.
 * - After the function returns:
 *   - Both ranges have no guarantee on which elements are initialized and what values are stored.
 *   - tempBuffer contains no useful data and should be treated as entirely uninitialized.
 *   - Any segment whose data buffer overlaps with tempBuffer[0 : len]
 *     must be treated as having invalid values in that array.
 *   - If successful, qrcode can be passed into qrcodegen_getSize() and qrcodegen_getModule().
 * 
 * Please consult the QR Code specification for information on
 * data capacities per version, ECC level, and text encoding mode.
 * 
 * This function allows the user to create a custom sequence of segments that switches
 * between modes (such as alphanumeric and byte) to encode text in less space.
 * This is a low-level API; the high-level API is qrcodegen_encodeText() and qrcodegen_encodeBinary().
 */
bool qrcodegen_encodeSegments(const struct qrcodegen_Segment segs[], size_t len,
	enum qrcodegen_Ecc ecl, uint8_t tempBuffer[], uint8_t qrcode[]);


/* 
 * Encodes the given segments to a QR Code, returning true if successful.
 * If the data is too long to fit in any version in the given range
 * at the given ECC level, then false is returned.
 * 
 * Requires 1 <= minVersion <= maxVersion <= 40.
 * 
 * The smallest possible QR Code version within the given range is automatically
 * chosen for the output. Iff boostEcl is true, then the ECC level of the result
 * may be higher than the ecl argument if it can be done without increasing the
 * version. The mask is either between qrcodegen_Mask_0 to 7 to force that mask, or
 * qrcodegen_Mask_AUTO to automatically choose an appropriate mask (which may be slow).
 * 
 * About the byte arrays, letting len = qrcodegen_BUFFER_LEN_FOR_VERSION(qrcodegen_VERSION_MAX):
 * - Before calling the function:
 *   - The array ranges tempBuffer[0 : len] and qrcode[0 : len] must allow
 *     reading and writing; hence each array must have a length of at least len.
 *   - The two ranges must not overlap (aliasing).
 *   - The initial state of both ranges can be uninitialized
 *     because the function always writes before reading.
 *   - The input array segs can contain segments whose data buffers overlap with tempBuffer.
 * - After the function returns:
 *   - Both ranges have no guarantee on which elements are initialized and what values are stored.
 *   - tempBuffer contains no useful data and should be treated as entirely uninitialized.
 *   - Any segment whose data buffer overlaps with tempBuffer[0 : len]
 *     must be treated as having invalid values in that array.
 *   - If successful, qrcode can be passed into qrcodegen_getSize() and qrcodegen_getModule().
 * 
 * Please consult the QR Code specification for information on
 * data capacities per version, ECC level, and text encoding mode.
 * 
 * This function allows the user to create a custom sequence of segments that switches
 * between modes (such as alphanumeric and byte) to encode text in less space.
 * This is a low-level API; the high-level API is qrcodegen_encodeText() and qrcodegen_encodeBinary().
 */
bool qrcodegen_encodeSegmentsAdvanced(const struct qrcodegen_Segment segs[], size_t len, enum qrcodegen_Ecc ecl,
	int minVersion, int maxVersion, enum qrcodegen_Mask mask, bool boostEcl, uint8_t tempBuffer[], uint8_t qrcode[]);


/* 
 * Tests whether the given string can be encoded as a segment in numeric mode.
 * A string is encodable iff each character is in the range 0 to 9.
 */
bool qrcodegen_isNumeric(const char *text);


/* 
 * Tests whether the given string can be encoded as a segment in alphanumeric mode.
 * A string is encodable iff each character is in the following set: 0 to 9, A to Z
 * (uppercase only), space, dollar, percent, asterisk, plus, hyphen, period, slash, colon.
 */
bool qrcodegen_isAlphanumeric(const char *text);


/* 
 * Returns the number of bytes (uint8_t) needed for the data buffer of a segment
 * containing the given number of characters using the given mode. Notes:
 * - Returns SIZE_MAX on failure, i.e. numChars > INT16_MAX or the internal
 *   calculation of the number of needed bits exceeds INT16_MAX (i.e. 32767).
 * - Otherwise, all valid results are in the range [0, ceil(INT16_MAX / 8)], i.e. at most 4096.
 * - It is okay for the user to allocate more bytes for the buffer than needed.
 * - For byte mode, numChars measures the number of bytes, not Unicode code points.
 * - For ECI mode, numChars must be 0, and the worst-case number of bytes is returned.
 *   An actual ECI segment can have shorter data. For non-ECI modes, the result is exact.
 */
size_t qrcodegen_calcSegmentBufferSize(enum qrcodegen_Mode mode, size_t numChars);


/* 
 * Returns a segment representing the given binary data encoded in
 * byte mode. All input byte arrays are acceptable. Any text string
 * can be converted to UTF-8 bytes and encoded as a byte mode segment.
 */
struct qrcodegen_Segment qrcodegen_makeBytes(const uint8_t data[], size_t len, uint8_t buf[]);


/* 
 * Returns a segment representing the given string of decimal digits encoded in numeric mode.
 */
struct qrcodegen_Segment qrcodegen_makeNumeric(const char *digits, uint8_t buf[]);


/* 
 * Returns a segment representing the given text string encoded in alphanumeric mode.
 * The characters allowed are: 0 to 9, A to Z (uppercase only), space,
 * dollar, percent, asterisk, plus, hyphen, period, slash, colon.
 */
struct qrcodegen_Segment qrcodegen_makeAlphanumeric(const char *text, uint8_t buf[]);


/* 
 * Returns a segment representing an Extended Channel Interpretation
 * (ECI) designator with the given assignment value.
 */
struct qrcodegen_Segment qrcodegen_makeEci(long assignVal, uint8_t buf[]);


/*---- Functions to extract raw data from QR Codes ----*/

/* 
 * Returns the side length of the given QR Code, assuming that encoding succeeded.
 * The result is in the range [21, 177]. Note that the length of the array buffer
 * is related to the side length - every 'uint8_t qrcode[]' must have length at least
 * qrcodegen_BUFFER_LEN_FOR_VERSION(version), which equals ceil(size^2 / 8 + 1).
 */
int qrcodegen_getSize(const uint8_t qrcode[]);


/* 
 * Returns the color of the module (pixel) at the given coordinates, which is false
 * for light or true for dark. The top left corner has the coordinates (x=0, y=0).
 * If the given coordinates are out of bounds, then false (light) is returned.
 */
bool qrcodegen_getModule(const uint8_t qrcode[], int x, int y);


#ifdef __cplusplus
}
#endif
