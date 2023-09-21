/*
 *   Copyright (c) 2023 thearistotlemethod@gmail.com
 *   All rights reserved.

 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at

 *   http://www.apache.org/licenses/LICENSE-2.0

 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include "esFile_definitions.h"
#include "M95M01_driver.h"
#include "esFile_disk.h"

/*
 * @brief Initialize the eeprom flash memory.
 *  This function initializes the eeprom flash memory, preparing it for read and write
    operations. It typically involves setting up data structures, checking
    for errors, and making the disk ready for use. 
 * @param format 
 * @return 0 
 */
int esFile_SimDiskInit(uint8_t format)
{
	if (format)
	{
		unsigned char clear_buffer[EEPROM_PAGE_SIZE] = {0};
		memset(clear_buffer, 0xFF, EEPROM_PAGE_SIZE);
		for (unsigned int i = 0; i < (EEPROM_SIZE / EEPROM_PAGE_SIZE); i++)
		{
			M95M01_PageWriteMemory(i * EEPROM_PAGE_SIZE, clear_buffer, EEPROM_PAGE_SIZE);
		}
	}

	return 0;
}

/*
 * @brief Read sector data from the eeprom flash memory.
 *  This function reads data from a specific sector on the eeprom flash memory, allowing you
    to retrieve information stored in that particular sector. It is used for precise
    data retrieval operations based on sector numbers.
 * @param sector 
 * @param buff 
 * @param idx 
 * @param count 
 * @return 0 
 */
int esFile_SimDiskRead(int sector, uint8_t *buff, int idx, int count)
{
	int rv = -1;
	uint8_t tmpBuff[512];

	if(idx > 0){
		rv = M95M01_ReadSectors(tmpBuff, sector, 512, 1);        

		if(idx + count <= 512){
			memcpy(buff, &tmpBuff[idx], count);
		} else {
			ESFILE_LOG("FS: FATAL ERROR: %s %d\n", __FILE__, __LINE__);
		}                
	} else{
		rv = M95M01_ReadSectors(buff, sector, 512, 1);    
	}

	return rv;
}

/*
 * @brief Write sector data to the eeprom flash memory.
 *  This function writes data to a specific sector on the eeprom flash memory, allowing you
    to store information in that particular sector. It is used for precise data
    storage operations based on sector numbers.
 * @param sector 
 * @param buff 
 * @param idx 
 * @param count 
 * @return 0 
 */
int esFile_SimDiskWrite(int sector, uint8_t *buff, int idx, int count)
{
	return M95M01_WriteSectors(buff, sector, 512, 1);
}

/*
 * @brief Release a sector in the eeprom flash memory.
 *  This function releases (frees up) a specific sector on the eeprom flash memory, making it
    available for future data storage. Releasing a sector allows it to be used for
    storing new data.
 * @param sector 
 * @return 0 
 */
int esFile_SimDiskRelease(int sector)
{
	return 0;
}
