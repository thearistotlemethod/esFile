#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "M95M01_driver.h"

#define SPI_EEPROM_CS_LOW() //EEPROM_CS_GPIO_Port->BSRR = EEPROM_CS_Pin << 16
#define SPI_EEPROM_CS_HIGH() //EEPROM_CS_GPIO_Port->BSRR = EEPROM_CS_Pin

static void M95M01_SPI_Transfer(uint8_t *pData, uint16_t Size)
{
	//MX_SPI_Transmit(SPI_BAUDRATEPRESCALER_8, pData, Size, SPI_EEPROM_TIMEOUT);
}

static void M95M01_SPI_Receive(uint8_t *pData, uint16_t Size)
{
	//MX_SPI_Receive(SPI_BAUDRATEPRESCALER_8, pData, Size, SPI_EEPROM_TIMEOUT);
}

void M95M01_WriteEnable(void)
{
	uint8_t instr = WREN;
	SPI_EEPROM_CS_LOW();
	M95M01_SPI_Transfer(&instr, 1);
	SPI_EEPROM_CS_HIGH();
}

void M95M01_WriteDisable(void)
{
	uint8_t instr = WRDI;
	SPI_EEPROM_CS_LOW();
	M95M01_SPI_Transfer(&instr, 1);
	SPI_EEPROM_CS_HIGH();
	int i = 0;
	while (i++ < 10000)
		;
}

void M95M01_WriteStatus(uint8_t status_reg)
{
	M95M01_WriteEnable();
	uint8_t buffer[2] = {0};
	buffer[0] = WRSR;
	buffer[1] = status_reg;
	SPI_EEPROM_CS_LOW();
	M95M01_SPI_Transfer(buffer, 2);
	SPI_EEPROM_CS_HIGH();
	M95M01_WriteDisable();
}

uint8_t M95M01_ReadStatus(void)
{
	uint8_t instr = RDSR;
	uint8_t m_rx_buf = 0;
	SPI_EEPROM_CS_LOW();
	M95M01_SPI_Transfer(&instr, 1);
	M95M01_SPI_Receive(&m_rx_buf, 1);
	SPI_EEPROM_CS_HIGH();
	return (uint8_t)m_rx_buf;
}

uint8_t M95M01_SingleReadMemory(uint32_t addr)
{
	uint8_t m_rx_buf = 0;
	uint8_t m_tx_buf[5] = {0};
	m_tx_buf[0] = READ;
	m_tx_buf[1] = (addr >> 16);
	m_tx_buf[2] = (addr >> 8);
	m_tx_buf[3] = addr;
	SPI_EEPROM_CS_LOW();
	M95M01_SPI_Transfer(m_tx_buf, 4);
	M95M01_SPI_Receive(&m_rx_buf, 1);
	SPI_EEPROM_CS_HIGH();
	return (uint8_t)m_rx_buf;
}

uint8_t M95M01_PageReadMemory(uint32_t addr, uint8_t *pData, unsigned int size)
{
	if (size > M95M01_PAGE_SIZE)
	{
		return M95M01_PAGE_SIZE_EXCCED;
	}
	else
	{
		uint8_t m_tx_buf[4] = {0};
		m_tx_buf[0] = READ;
		m_tx_buf[1] = (addr >> 16);
		m_tx_buf[2] = (addr >> 8);
		m_tx_buf[3] = addr;
		SPI_EEPROM_CS_LOW();
		M95M01_SPI_Transfer(m_tx_buf, 4);
		M95M01_SPI_Receive(pData, size);
		SPI_EEPROM_CS_HIGH();
	}
	return M95M01_OK;
}

void M95M01_SingleWriteMemory(uint32_t addr, uint8_t data)
{
	M95M01_WriteEnable();
	uint8_t m_tx_buf[5] = {0};
	m_tx_buf[0] = WRITE;
	m_tx_buf[1] = (addr >> 16);
	m_tx_buf[2] = (addr >> 8);
	m_tx_buf[3] = addr;
	m_tx_buf[4] = data;
	SPI_EEPROM_CS_LOW();
	M95M01_SPI_Transfer(m_tx_buf, 5);
	SPI_EEPROM_CS_HIGH();
	M95M01_WriteDisable();
}

uint8_t M95M01_PageWriteMemory(uint32_t addr, uint8_t *pData, unsigned int size)
{

	if ((size > M95M01_PAGE_SIZE) || (size == 0))
	{
		return M95M01_PAGE_SIZE_EXCCED;
	}
	else
	{
		uint8_t buff[4];
		int i = 0;
		M95M01_WriteEnable();
		SPI_EEPROM_CS_LOW();
		buff[0] = WRITE;
		buff[1] = (addr >> 16) & 0xff;
		buff[2] = (addr >> 8) & 0xff;
		buff[3] = addr & 0xff;
		M95M01_SPI_Transfer(buff, 4);
		M95M01_SPI_Transfer(pData, size);
		SPI_EEPROM_CS_HIGH();
		M95M01_WriteDisable();
		while ((M95M01_ReadStatus() & M95M01_STATUS_WIP) && (i++ < 1000))
			;
	}
	return M95M01_OK;
}

uint8_t M95M01_WriteSectors(const uint8_t *pBuffer, uint32_t SectorNo, uint16_t SectorSize, uint32_t SectorCount)
{
	for (unsigned int i = 0; i < SectorCount; i++)
	{
		for (unsigned int j = 0; j < (SectorSize / M95M01_PAGE_SIZE); j++)
		{
			M95M01_PageWriteMemory(SectorNo * SectorSize + j * M95M01_PAGE_SIZE + i * SectorSize, (uint8_t *)&pBuffer[j * M95M01_PAGE_SIZE + i * SectorSize], M95M01_PAGE_SIZE);
		}
	}
	return 0;
}

uint8_t M95M01_ReadSectors(uint8_t *pBuffer, uint32_t SectorNo, uint16_t SectorSize, uint32_t SectorCount)
{

	for (unsigned int i = 0; i < SectorCount; i++)
	{
		for (unsigned int j = 0; j < (SectorSize / M95M01_PAGE_SIZE); j++)
		{
			M95M01_PageReadMemory(SectorNo * SectorSize + j * M95M01_PAGE_SIZE + i * SectorSize, (uint8_t *)&pBuffer[j * M95M01_PAGE_SIZE + i * SectorSize], M95M01_PAGE_SIZE);
		}
	}

	return 0;
}
