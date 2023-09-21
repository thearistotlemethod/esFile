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
#include "esFile_seek.h"

/*
 * @brief Seek the cursor position within a file.
 *  This function allows you to move the cursor position within a file to a specific
    offset. The 'ofs' parameter determines the new position.
 * @param fp 
 * @param ofs 
 * @return 0 if it is successful 
 */
int esFile_Seek(esFile_FileDescriptor *fp, uint32_t ofs)
{
    esFile_FileInfo fi;
    esFile_DataSectorHeader headSector;
    esFile_DriveInfo *dInfos = NULL;
    uint8_t *buffer = NULL;
    int rv = 0;

    if (fp == NULL)
    {
        return -1;
    }

    ENTER_CRITICAL();

    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();    
    
    esFile_ReadFileInfo(fp->did, &fi, fp->infoLoc);

    if (fi.uid == fp->uid)
    {
        if (fp->index > ofs)
        {
            while (fp->index > ofs)
            {
                if (esFile_DiskRead(fp->did, fp->currentSector, buffer, 0, sizeof(esFile_DataSectorHeader)) == 0)
                {
                    memcpy(&headSector, buffer, sizeof(esFile_DataSectorHeader));
                    for (; (fp->sectorIndex >= sizeof(esFile_DataSectorHeader)) && (fp->index > ofs); fp->sectorIndex--)
                    {
                        fp->index--;
                    }
                }
                else
                {
                    ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
                    rv = -2;
                    break;
                }

                if (fp->sectorIndex < sizeof(esFile_DataSectorHeader))
                {
                    if (headSector.presector > 0)
                    {
                        fp->currentSector = headSector.presector;
                        fp->sectorIndex = dInfos[fp->did].sectorCapacity - 1;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            while (fp->index < ofs)
            {
                if (esFile_DiskRead(fp->did, fp->currentSector, buffer, 0, sizeof(esFile_DataSectorHeader)) == 0)
                {
                    memcpy(&headSector, buffer, sizeof(esFile_DataSectorHeader));
                    for (; (fp->sectorIndex < dInfos[fp->did].sectorCapacity) && (fp->index < ofs); fp->sectorIndex++)
                    {
                        fp->index++;
                    }
                }
                else
                {
                    ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
                    rv = -2;
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
        }
    }
    else
    {
        ESFILE_LOG("FATAL ERROR: %s %d \n", __FILE__, __LINE__);
    }

    LEAVE_CRITICAL();
    return rv;
}