#pragma once
enum EUnicodeByteOrder
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};

// This function will attempt to decode a UTF-8 encoded character in the buffer.
// If the encoding is invalid, the function returns -1.
int DecodeUTF8(const char *Buffer, unsigned int *outCharLength);

// This function will encode the value into the buffer.
// If the value is invalid, the function returns -1, else the encoded length.
int EncodeUTF8(unsigned int value, char *outEncodedBuffer, unsigned int *outCharLength);

// This function will attempt to decode a UTF-16 encoded character in the buffer.
// If the encoding is invalid, the function returns -1.
int DecodeUTF16(const char *Buffer, unsigned int *outCharLength);

// This function will encode the value into the buffer.
// If the value is invalid, the function returns -1, else the encoded length.
int EncodeUTF16(unsigned int value, char *outEncodedBuffer, unsigned int *outCharLength);

int oem_to_unicode(unsigned short src);
int unicode_to_oem(unsigned short src);
