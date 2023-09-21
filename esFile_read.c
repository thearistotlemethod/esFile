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
#include "esFile_system.h"
#include "esFile_cache.h"
#include "esFile_disk.h"
#include "esFile_open.h"
#include "esFile_cryption.h"
#include "esFile_read.h"

/*
 * @brief Read data from a file.
 *  This function reads data from the file located at the specified file path.
    It allows you to retrieve and work with the contents of the file.
 * @param fp 
 * @param buff 
 * @param btr 
 * @param br 
 * @return 0 if it is successful
 */
int esFile_Read(esFile_FileDescriptor *fp, void *buff, uint32_t btr, uint32_t *br)
{
    esFile_DataSectorHeader headSector;
    esFile_DriveInfo *dInfos = NULL;
    uint8_t *buffer = NULL, *tmpBuff = NULL;
    uint32_t idx = 0;
    int rv = 0;

    if (fp == NULL)
    {
        return -1;
    }

    ENTER_CRITICAL();

    
    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();
    
    tmpBuff = (uint8_t *)buff;
    while ((btr > 0) && (fp->index < fp->size))
    {
        if (esFile_DiskRead(fp->did, fp->currentSector, buffer, 0, dInfos[fp->did].sectorCapacity) == 0)
        {
            memcpy(&headSector, buffer, sizeof(esFile_DataSectorHeader));
            if (headSector.uid != fp->uid)
            {
                ESFILE_LOG("FS: FATAL ERROR: %s %d\n", __FILE__, __LINE__);
                rv = -2;
                break;
            }

            if (fp->encrypted)
            {
                esFile_Decypt(&buffer[sizeof(esFile_DataSectorHeader)], dInfos[fp->did].sectorCapacity - sizeof(esFile_DataSectorHeader));
            }

            for (; (fp->sectorIndex < dInfos[fp->did].sectorCapacity) && (btr > 0) && (fp->index < fp->size); fp->sectorIndex++)
            {
                if (tmpBuff)
                {
                    tmpBuff[idx] = buffer[fp->sectorIndex];
                }
                fp->index++;
                idx++;
                btr--;
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
            rv = -3;
            break;
        }

        if (fp->sectorIndex >= dInfos[fp->did].sectorCapacity)
        {
            if (headSector.nextsector > 0)
            {
                fp->currentSector = headSector.nextsector;
                fp->sectorIndex = sizeof(esFile_DataSectorHeader);
            }
            else
            {
                break;
            }
        }
    }

    if (br)
        *br = idx;

    LEAVE_CRITICAL();
    return rv;
}


