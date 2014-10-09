
#include "example_base.h"


UINT16 adxM3_Example::TXBUF1[BUFSIZE] = { 0 };
UINT16 adxM3_Example::TXBUF2[BUFSIZE] = { 0 };
UINT16 adxM3_Example::RXBUF1[BUFSIZE] = { 0 };
UINT16 adxM3_Example::RXBUF2[BUFSIZE] = { 0 };

volatile unsigned int adxM3_Example::TXIDX1 = 0;
volatile unsigned int adxM3_Example::TXIDX2 = 0;
volatile unsigned int adxM3_Example::RXIDX1 = 0;
volatile unsigned int adxM3_Example::RXIDX2 = 0;

volatile UINT16 adxM3_Example::ParityWord1 = 0;
volatile UINT16 adxM3_Example::ParityWord2 = 0;

