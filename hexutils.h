#ifndef _HEXUTILS_H_
#define _HEXUTILS_H_

/**
 * \brief Convert byte to length 2 hex string.
 * \param c The byte to convert into a hex string.
 *          The returned value is a statically allocated space, do not free or
 *          modify it.
 *          Note: The returned value is always a null terminated string with the
 *          length 2.
 */
const char* bytetohex(char c);

/**
 * \brief Convert hex string into bytes.
 * \param h The string to convert, this is assumed to be at least 2 bytes long.
 */
char hextobyte(const char* h);

#endif /* _HEXUTILS_H_ */
