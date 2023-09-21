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

#include "esFtl.h"
#include "esFile_definitions.h"
#include "esFile_disk_nand.h"

/*
 * @brief Initialize the NAND disk.
 *  This function initializes the NAND disk, preparing it for read and write
    operations. It typically involves setting up data structures, checking
    for errors, and making the disk ready for use. 
 * @param format 
 * @return 0 
 */
int esFile_NandDiskInit(uint8_t format)
{
    return esFtl_Init(format);
}

/*
 * @brief Read sector data from the NAND disk.
 *  This function reads data from a specific sector on the NAND disk, allowing you
    to retrieve information stored in that particular sector. It is used for precise
    data retrieval operations based on sector numbers.
 * @param sector 
 * @param buff 
 * @param idx 
 * @param count 
 * @return 0 
 */
int esFile_NandDiskRead(int sector, uint8_t *buff, int idx, int count)
{
    return esFtl_Read(sector, buff, idx, count);
}

/*
 * @brief Write sector data to the NAND disk.
 *  This function writes data to a specific sector on the NAND disk, allowing you
    to store information in that particular sector. It is used for precise data
    storage operations based on sector numbers.
 * @param sector 
 * @param buff 
 * @param idx 
 * @param count 
 * @return 0 
 */
int esFile_NandDiskWrite(int sector, uint8_t *buff, int idx, int count)
{
    return esFtl_FtlDriverWrite(sector, buff, idx, count);
}

/*
 * @brief Release a sector in the NAND disk.
 *  This function releases (frees up) a specific sector on the NAND disk, making it
    available for future data storage. Releasing a sector allows it to be used for
    storing new data.
 * @param sector 
 * @return 0 
 */
int esFile_NandDiskRelease(int sector)
{
    return esFtl_FtlDriverRelease(sector);
}