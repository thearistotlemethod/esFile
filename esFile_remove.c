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
#include "esFile_disk.h"
#include "esFile_cache.h"
#include "esFile_remove.h"

/*
 * @brief Remove (delete) a file.
 *  This function deletes the file located at the specified file path. It permanently
    removes the file from the file system, and the data cannot be recovered.
 * @param path 
 * @return 0 if it is successful 
 */
int esFile_Remove(const char *path)
{
    uint8_t *buffer = NULL;
    esFile_DriveInfo *dInfos = NULL;
    esFile_System *fs = NULL;
    esFile_FileInfo fi;
    int did = 0, infoLoc = 0;

    ENTER_CRITICAL();

    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();
    
    did = esFile_DiskDriveIdFromPath(path);
    fs = esFile_GetFileSystems();
    
    infoLoc = esFile_GetFileInfo(did, path, &fi);
    if (infoLoc >= 0)
    {
        int sector = infoLoc / dInfos[did].sectorCapacity;
        if (esFile_DiskRead(did, sector, buffer, 0, dInfos[did].sectorCapacity) == 0)
        {
            memset(&buffer[infoLoc % dInfos[did].sectorCapacity], 0, ESFILE_FILENGTH);
            esFile_DiskWrite(did, sector, buffer, 0, dInfos[did].sectorCapacity);

            fs[did].filecount--;

            fi.uid = 0;
            esFile_ReleaseSectors(did, &fi);
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
        }
    }

    LEAVE_CRITICAL();
    return 0;
}

/*
 * @brief Release sectors associated with a specific file.
 *  This function releases (frees up) sectors that are associated with a particular
    file identified by its unique file ID. Releasing sectors allows them to be used
    for storing new data.
 * @param did 
 * @param fi 
 */
void esFile_ReleaseSectors(uint8_t did, esFile_FileInfo *fi)
{
    esFile_DataSectorHeader dsh;
    uint8_t *buffer = NULL;
    int sno = 0;

    if (!fi || did != 0)
        return;

    buffer = esFile_GetDiskBuffer();
    
    sno = fi->startSector;
    while (1)
    {
        if (esFile_DiskRead(did, sno, buffer, 0, sizeof(esFile_DataSectorHeader)) == 0)
        {
            memcpy(&dsh, buffer, sizeof(esFile_DataSectorHeader));
            if (dsh.uid != fi->uid)
            {
                esFile_SetSectorFlag(did, sno, 0);
                esFile_DiskRelease(did, sno);
                if (dsh.nextsector > 0)
                    sno = dsh.nextsector;
                else
                    break;
            }
            else
            {
                ESFILE_LOG("FS: FATAL ERROR: %s %d\n", __FILE__, __LINE__);
                break;
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
            break;
        }
    }
}
