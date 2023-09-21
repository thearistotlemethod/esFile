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
#include "esFile_write.h"

/*
 * @brief Write data to a file.
 *  This function writes the provided data to the file located at the specified
    file path. It allows you to store information or content in the file.
 * @param fp 
 * @param buff 
 * @param btw 
 * @param bw 
 * @return 0 if it is successful 
 */
int esFile_Write(esFile_FileDescriptor *fp, const void *buff, uint32_t btw, uint32_t *bw)
{
    uint8_t *buffer = NULL, *tmpBuff = NULL;
    esFile_DriveInfo *dInfos = NULL;
    uint32_t idx = 0;
    esFile_DataSectorHeader dsh;    
    esFile_FileInfo fi;
    int rv = 0;

    if (fp == NULL || btw <= 0)
    {
        return -1;
    }

    ENTER_CRITICAL();

    
    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();
    tmpBuff = (uint8_t *)buff;

    while (btw > idx)
    {
        if (esFile_DiskRead(fp->did, fp->currentSector, buffer, 0, dInfos[fp->did].sectorCapacity) == 0)
        {
            memcpy(&dsh, buffer, sizeof(esFile_DataSectorHeader));

            if (fp->encrypted)
            {
                esFile_Decypt(&buffer[sizeof(esFile_DataSectorHeader)], dInfos[fp->did].sectorCapacity - sizeof(esFile_DataSectorHeader));
            }

            for (; (fp->sectorIndex < dInfos[fp->did].sectorCapacity) && (btw > idx); fp->sectorIndex++)
            {
                if(tmpBuff)
                    buffer[fp->sectorIndex] = tmpBuff[idx];
                else
                    buffer[fp->sectorIndex] = 0;    

                idx++;            
            }

            if (fp->encrypted)
            {
                esFile_Encypt(&buffer[sizeof(esFile_DataSectorHeader)], dInfos[fp->did].sectorCapacity - sizeof(esFile_DataSectorHeader));
            }

            esFile_DiskWrite(fp->did, fp->currentSector, buffer, 0, dInfos[fp->did].sectorCapacity);
        }
        else
        {
        	ESFILE_LOG("DiskRead error %d: %s %d \n", fp->did, __FILE__, __LINE__);
            rv = -2;
            break;
        }

        if (fp->sectorIndex >= dInfos[fp->did].sectorCapacity)
        {
            if(dsh.nextsector > 0){
                fp->currentSector = dsh.nextsector;
                fp->sectorIndex = sizeof(esFile_DataSectorHeader);
            } else {
                int newSector = esFile_GetFreeSector(fp->did);
                if (newSector > 0)
                {
                    dsh.nextsector = newSector;
                    esFile_UpdateDataSectorHeader(fp->did, fp->currentSector, &dsh);

                    dsh.presector = fp->currentSector;
                    dsh.nextsector = 0;
                    fp->currentSector = newSector;
                    fp->sectorIndex = sizeof(esFile_DataSectorHeader);

                    esFile_UpdateDataSectorHeader(fp->did, fp->currentSector, &dsh);
                }
                else
                {
                    ESFILE_LOG("Unable to write Disk Full: %s %d\n", __FILE__, __LINE__);
                    rv = -3;
                    break;
                }
            }
        }
    }

    if (!rv)
    {
        if (bw)
        {
            *bw = idx;
        }

        fp->index += idx;
        if (fp->index > fp->size)
        {
            fp->size = fp->index;
            
            esFile_ReadFileInfo(fp->did, &fi, fp->infoLoc);
            fi.size = fp->size;
            esFile_WriteFileInfo(fp->did, &fi, fp->infoLoc);
        }
    }

    LEAVE_CRITICAL();
    return rv;
}
