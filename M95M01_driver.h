#ifndef _SPI_EEPROM_H__
#define _SPI_EEPROM_H__

#define EEPROM_PAGE_SIZE 256
#define EEPROM_SIZE 0x20000

#define SPI_EEPROM_TIMEOUT 100

#define WREN 0x06
#define WRDI 0x04
#define RDSR 0x05
#define WRSR 0x01
#define READ 0x03
#define WRITE 0x02
#define RDID 0x83
#define WRID 0x82
#define RDLS 0x83
#define LID 0x82

#define M95M01_PAGE_SIZE 256

#define M95M01_OK 0x00
#define M95M01_PAGE_SIZE_EXCCED 0x01
#define M95M01_STATUS_WIP 0x01 

void M95M01_eeprom_uninit(void);
void M95M01_Init(void);
void M95M01_WriteEnable(void);
void M95M01_WriteDisable(void);
uint8_t M95M01_ReadStatus(void);
void M95M01_WriteStatus(uint8_t status_reg);
uint8_t M95M01_SingleReadMemory(uint32_t addr);
uint8_t M95M01_PageReadMemory(uint32_t addr, uint8_t *pData, unsigned int size);
void M95M01_SingleWriteMemory(uint32_t addr, uint8_t data);
uint8_t M95M01_PageWriteMemory(uint32_t addr, uint8_t *pData, unsigned int size);
uint8_t M95M01_ReadSectors(uint8_t *pBuffer, uint32_t SectorNo, uint16_t SectorSize, uint32_t SectorCount);
uint8_t M95M01_WriteSectors(const uint8_t *pBuffer, uint32_t SectorNo, uint16_t SectorSize, uint32_t SectorCount);

#endif
