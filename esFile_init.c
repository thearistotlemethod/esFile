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
#include "esFile_disk.h"
#include "esFile_system.h"
#include "esFile_cache.h"
#include "esFile_init.h"

/*
 * @brief Initialize the file system.
 *  This function performs the initialization of the file system, setting up data
    structures and preparing the system for file operations. It ensures that the
    file system is ready for use.
 * @param format 
 * @return 0 if it is successful
 */
int esFile_Init(uint8_t format)
{
    esFile_DriveInfo *dInfos = NULL;
    uint8_t *buffer = NULL;
    esFile_System *fs = NULL;
    int rv = 0, usedPages = 0;
    ENTER_CRITICAL();

    esFile_CacheInit();
    esFile_DiskInit(format);

    dInfos = esFile_GetDriveInfos();
    buffer = esFile_GetDiskBuffer();
    fs = esFile_GetFileSystems();

    if (format)
    {
        ESFILE_LOG("Formating Nand Disk...\n");
        esFile_ClearDiskBuffer();
        for (int i = 0; i < dInfos[0].fiSectorCount; i++)
        {
            esFile_DiskWrite(0, i, buffer, 0, dInfos[0].sectorCapacity);
        }
        fs[0].version = ESFILE_VERSION;
        fs[0].lastuid = 0;
        fs[0].filecount = 0;
        esFile_WriteFileSystem(0);
        ESFILE_LOG("Formating Nand Finished\n");

        ESFILE_LOG("Formating Ram Disk...\n");
        esFile_ClearDiskBuffer();
        for (int i = 0; i < dInfos[1].fiSectorCount; i++)
        {
            esFile_DiskWrite(1, i, buffer, 0, dInfos[1].sectorCapacity);
        }
        fs[1].version = ESFILE_VERSION;
        fs[1].lastuid = 0;
        fs[1].filecount = 0;
        esFile_WriteFileSystem(1);
        ESFILE_LOG("Formating Ram Finished\n");
    }
    else
    {
        esFile_ReadFileSystem(0);
        if (fs[0].version != ESFILE_VERSION)
        {
            ESFILE_LOG("Versions are mismatch for disk 0. It should be reformatted\n");
            rv = -1;
        }
        else
        {
            esFile_EvaluateSectorTable(0);

            esFile_ReadFileSystem(1);
            if (fs[1].version != ESFILE_VERSION)
            {
                ESFILE_LOG("Versions are mismatch for disk 1. It should be reformatted\n");
                rv = -1;
            } else {     
                esFile_EvaluateSectorTable(1);

                esFile_CalculateFileCountAndUid(0);
                esFile_CalculateFileCountAndUid(1);

                usedPages = esFtl_CalcUsedPages();
                if(usedPages > (ESFTL_NANDNUMBLOCKS/10)*ESFTL_NANDNUMPAGEBLOCK && (esFile_CalcDiskUsage(0) * 2) < usedPages){
                    esFtl_Defrag();
                }              
            }
        }

    }

    LEAVE_CRITICAL();
    return rv;
}
