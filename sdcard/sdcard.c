#include "sdcard.h"

#define CMD0   (0x40+0) /* GO_IDLE_STATE */
#define CMD1   (0x40+1) /* SEND_OP_COND (MMC) */
#define  ACMD41   (0xC0+41)   /* SEND_OP_COND (SDC) */
#define CMD8   (0x40+8) /* SEND_IF_COND */
#define CMD9   (0x40+9) /* SEND_CSD */
#define CMD10  (0x40+10)   /* SEND_CID */
#define CMD12  (0x40+12)   /* STOP_TRANSMISSION */
#define ACMD13 (0xC0+13)   /* SD_STATUS (SDC) */
#define CMD16  (0x40+16)   /* SET_BLOCKLEN */
#define CMD17  (0x40+17)   /* READ_SINGLE_BLOCK */
#define CMD18  (0x40+18)   /* READ_MULTIPLE_BLOCK */
#define CMD23  (0x40+23)   /* SET_BLOCK_COUNT (MMC) */
#define  ACMD23   (0xC0+23)   /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24  (0x40+24)   /* WRITE_BLOCK */
#define CMD25  (0x40+25)   /* WRITE_MULTIPLE_BLOCK */
#define CMD55  (0x40+55)   /* APP_CMD */
#define CMD58  (0x40+58)   /* READ_OCR */
#define WRITE_SPI(data) {\
   while(!EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG);\
   EUSCI_B0->TXBUF = hiByte;\
}
#define RECEIVE_SPI() {\
   while(!EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG);\
   return EUSCI_B0->RXBUF;
}

typedef struct descriptor {
   uint32_t CID[4]; //card identification number
   uint16_t RCA; //relative card address
   uint32_t CSD[4]; //card specific data
   uint64_t SCR; //sd configuration register
   uint32_t OCR; //operation condition register
   uint32_t SSR[16]; //sd status register
   uint32_t CSR; //card status register
} SdCardDescriptor;

void sdSendCommand(uint8_t cmd, uint32_t arg, uint8_t crc7)
{
   WRITE_SPI(cmd);
   WRITE_SPI(arg>>24);
   WRITE_SPI(arg>>16);
   WRITE_SPI(arg>>8);
   WRITE_SPI(arg);
   WRITE_SPI(crc7);
}

void sdSendToken(uint8_t token)
{
   WRITE_SPI(token);
}

void sdWriteBlock(uint32_t address, uint8_t *data, uint8_t crc7)
{
   sdSendCommand(CMD24, address, crc7);
}

void sdWriteMultipleBlocks(uint32_t address, uint8_t crc7)
{
   sdSendCommand(CMD25, address, crc7);
}

void sdReadBlock(uint32_t address, uint8_t *buf, SdCommand* cmd)
{
   
}
void sdReadMultipleBlocks(uint32_t address, uint8_t *buf, SdCommand* cmd)
{

}
void sdProgramCSD(uint32_t stuffbits, SdCommand* cmd)
{

}
void sdSetEraseStartAddr(uint32_t address, SdCommand* cmd)
{

}
void sdSetEraseEndAddr(uint32_t address, SdCommand* cmd)
{

}
void sdErase(uint32_t stuff, SdCommand* cmd)
{

}
void sdLockUnlock(SdCommand* cmd)
{

}
void sdGenCmd(uint32_t rdwr, SdCommand* cmd) //1 - rd, 0 - wr
{

}
void sdReadOCR(SdCommand* cmd)
{

}
void sdCRCOnOff(uint32_t option, SdCommand* cmd)
{

}
//acmd
void sdSendStatus(SdCommand* cmd)
{

}
void sdSendNumWrBlocks(SdCommand* cmd)
{

}
void sdSetWrBlkEraseCnt(uint32_t numBlocks, SdCommand* cmd)
{

}
void sdSendOpCond(SdCommand* cmd)
{

}
void sdSetClrCardDetect(uint32_t set_cd, SdCommand* cmd)
{

}
void sdSendSCR(SdCommand* cmd)
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
