#ifndef SDCARD_H
#define SDCARD_H
#include <stdint.h>
#define SD_CMD0_GO_IDLE_STATE 0
//send host capacity info and activate initialization process
#define SD_CMD1_SEND_OP_COND 0x40000000 
#define SD_CMD6_SWITCH_FUNC 
#define SD_CMD8_SEND_IF_COND 0x00000FFF //11:8 vdd, 7:0 checksum
#define SD_CMD9_SEND_CSD 0x0 //ask for CSD data
#define SD_CMD10_SEND_CID 0x0 //ask for CID data
#define SD_CMD12_STOP_TRANS 0x0 //stop trans in multiblock read
#define SD_CMD13_SEND_STATUS 0x0 //ask for status register
#define SD_CMD16_SET_BLOCKLEN //unused in SDHC/XC

void sdWriteBlock(uint32_t address);
void sdWriteMultipleBlocks(uint32_t address);
void sdReadBlock(uint32_t address, uint8_t *buf);
void sdReadMultipleBlocks(uint32_t address, uint8_t *buf);
void sdProgramCSD(uint32_t stuffbits);
void sdSetEraseStartAddr(uint32_t address);
void sdSetEraseEndAddr(uint32_t address);
void sdErase(uint32_t stuff);
void sdLockUnlock();
void sdGenCmd(uint32_t rdwr); //1 - rd, 0 - wr
void sdReadOCR();
void sdCRCOnOff(uint32_t option);
//acmd
void sdSendStatus();
void sdSendNumWrBlocks();
void sdSetWrBlkEraseCnt(uint32_t numBlocks);
void sdSendOpCond();
void sdSetClrCardDetect(uint32_t set_cd);
void sdSendSCR();

typedef struct descriptor {
   uint32_t CID[4]; //card identification number
   uint16_t RCA; //relative card address
   uint32_t CSD[4]; //card specific data
   uint64_t SCR; //sd configuration register
   uint32_t OCR; //operation condition register
   uint32_t SSR[16]; //sd status register
   uint32_t CSR; //card status register
} SdCardDescriptor;
//CID register data
uint32_t sdProductSerialNumber(SdCardDescriptor desrc);
uint8_t sdCRC7Checksum(SdCardDescriptor descr);
uint8_t sdManufacturerID(SdCardDescriptor descr);
uint16_t sdOEMApplicationID(SdCardDescriptor descr);
uint64_t sdProductName(SdCardDescriptor descr);
uint8_t sdProductRevision(SdCardDescriptor descr);
uint16_t sdManufacturerDateCode(SdCardDescriptor descr);
//CSD register data
#endif
