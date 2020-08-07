#ifndef __FLOAT_H_
#define __FLOAT_H_

union Float {
    float m_float;
    uint8_t m_bytes[sizeof(float)];
};

#endif