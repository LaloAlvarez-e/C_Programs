#ifndef __CRC16_CCITT__
#define __CRC16_CCITT__

#include <stdint.h>

uint16_t CRC_CCITT__u16Calculate(const uint8_t* pu8Values, uint32_t u32Length, uint16_t u16InitSeed);

#endif