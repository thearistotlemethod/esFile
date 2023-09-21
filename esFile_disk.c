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
#include "esFile_disk_simulator.h"
#include "esFile_disk.h"

const esFile_DriveInfo esFile_dInfos[] = {
    {
        32, 
        32, 
        ESFTL_NANDNUMBLOCKS * ESFTL_NANDNUMPAGEBLOCK - 1,
        ESFTL_NANDPAGEDATASIZE,
        esFile_NandDiskInit,
        esFile_NandDiskRead,
        esFile_NandDiskWrite,
        esFile_NandDiskRelease
    },
    {
        8, 
        8, 
        256, 
        512,
        esFile_SimDiskInit,
        esFile_SimDiskRead,
        esFile_SimDiskWrite,
        esFile_SimDiskRelease
    }
};

/*
 * @brief Initialize disk devices.
 *  This function initializes one or more disk devices, preparing them for read
    and write operations. Initialization may involve setting up data structures,
    checking for errors, and making the disks ready for use.
 * @param format 
 */
void esFile_DiskInit(uint8_t format){
    for(int i = 0; i < sizeof(esFile_dInfos) / sizeof(esFile_DriveInfo); i++){
        esFile_dInfos[i].diskInit(format);
    }
}

/*
 * @brief Determine the disk ID from a path string.
 *  This function extracts the disk ID from a given path string. The disk ID
    is typically used to uniquely identify a specific disk device within a
    storage system based on its path.
 * @param path 
 * @return The extracted disk ID (int) 
 */
int esFile_DiskDriveIdFromPath(const char *path){
    if (!memcmp(path, "e:", 2))
    {
        return 1;
    }

    return 0;
}

/*
 * @brief Read sector data from a specific disk.
 *  This function reads data from a specific sector on a particular disk identified
    by its unique disk ID. It allows you to retrieve information stored in the
    specified sector of the selected disk.
 * @param pdrv 
 * @param sector 
 * @param buff 
 * @param idx 
 * @param count 
 * @return 0 
 */
int esFile_DiskRead(int pdrv, int sector, uint8_t *buff, int idx, int count)
{
    if(sector >= esFile_dInfos[pdrv].dataSectorEnd){
    	ESFILE_LOG("Sector No Error %d %d\n", pdrv, sector);
    	return -1;
    }

    return esFile_dInfos[pdrv].diskRead(sector, buff, idx, count);
}

/*
 * @brief Write sector data to a specific disk.
 *  This function writes data to a specific sector on a particular disk identified
    by its unique disk ID. It allows you to store information in the specified sector
    of the selected disk.
 * @param pdrv 
 * @param sector 
 * @param buff 
 * @param idx 
 * @param count 
 * @return 0 
 */
int esFile_DiskWrite(int pdrv, int sector, uint8_t *buff, int idx, int count)
{
    return esFile_dInfos[pdrv].diskWrite(sector, buff, idx, count);
}

/*
 * @brief Release a sector in a specific disk.
 *  This function releases (frees up) a specific sector on a particular disk
    identified by its unique disk ID. Releasing a sector makes it available for
    future data storage operations on the selected disk.
 * @param pdrv 
 * @param sector 
 * @return int 
 */
int esFile_DiskRelease(int pdrv, int sector){
    return esFile_dInfos[pdrv].diskRelease(sector);
}

/*
 * @brief Get a pointer to drive information data.
 *  This function returns a pointer reference to the drive
    information data, allowing you to access and work with information about
    the drives in the system.
 * @return esFile_DriveInfo* 
 */
esFile_DriveInfo *esFile_GetDriveInfos(void){
    return (esFile_DriveInfo *)esFile_dInfos;
}
