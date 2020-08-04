#include <stdlib.h>
#include <inttypes.h>

#include "float.h"

union Float {
    float m_float;
    uint8_t m_bytes[sizeof(float)];
};

void float_to_bytes(float value, uint8_t *buffer)
{
    union Float f;
    f.m_float = value;
    memcpy(buffer, f.m_bytes, sizeof(float));
}

float bytes_to_float(uint8_t* bytes)
{
    union Float f;
    memcpy(f.m_bytes, bytes, sizeof(float));
    return f.m_float;
}