#include "sdcard.h"

typedef struct descriptor {
   uint32_t CID[4]; //card identification number
   uint16_t RCA; //relative card address
   uint32_t CSD[4]; //card specific data
   uint64_t SCR; //sd configuration register
   uint32_t OCR; //operation condition register
   uint32_t SSR[16]; //sd status register
   uint32_t CSR; //card status register
} SdCardDescriptor;
void sdWriteBlock(uint32_t address)
{

}
void sdWriteMultipleBlocks(uint32_t address)
{

}
void sdReadBlock(uint32_t address, uint8_t *buf)
{

}
void sdReadMultipleBlocks(uint32_t address, uint8_t *buf)
{

}
void sdProgramCSD(uint32_t stuffbits)
{

}
void sdSetEraseStartAddr(uint32_t address)
{

}
void sdSetEraseEndAddr(uint32_t address)
{

}
void sdErase(uint32_t stuff)
{

}
void sdLockUnlock()
{

}
void sdGenCmd(uint32_t rdwr) //1 - rd, 0 - wr
{

}
void sdReadOCR()
{

}
void sdCRCOnOff(uint32_t option)
{

}
//acmd
void sdSendStatus()
{

}
void sdSendNumWrBlocks()
{

}
void sdSetWrBlkEraseCnt(uint32_t numBlocks)
{

}
void sdSendOpCond()
{

}
void sdSetClrCardDetect(uint32_t set_cd)
{

}
void sdSendSCR()
{

}
//CID register data
uint32_t sdProductSerialNumber(SdCardDescriptor desrc)
{

}
uint8_t sdCRC7Checksum(SdCardDescriptor descr)
{

}
uint8_t sdManufacturerID(SdCardDescriptor descr)
{

}
uint16_t sdOEMApplicationID(SdCardDescriptor descr)
{

}
uint64_t sdProductName(SdCardDescriptor descr)
{

}
uint8_t sdProductRevision(SdCardDescriptor descr)
{

}
uint16_t sdManufacturerDateCode(SdCardDescriptor descr)
{

}
