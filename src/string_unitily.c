#include "string_unitily.h"

static char* 
toString(uint64_t value, char* buffer)
{
    do
    {
        *--buffer = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    
    return buffer;
}

char* unsignedToString(uint64_t value, char* buffer)
{
    return toString(value, buffer);
}

char* signedToString(int64_t value, char* buffer)
{
    char* bufferEnd = buffer + 21;
    char* next = bufferEnd;

    if (value < 0)
    {
        next = unsignedToString(0 - value, next);
        *--next = '-';
    }
    else
    {
        next = unsignedToString(value, next);
    }

    return next;
}