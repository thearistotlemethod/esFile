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
#include "esFile_open.h"
#include "esFile_stat.h"
#include "esFile_rename.h"

/*
 * @brief Rename a file.
 *  This function renames a file located at the old file path to the new file name.
    It allows you to change the name of the file while keeping its content intact.
 * @param path_old 
 * @param path_new 
 * @return 0 
 */
int esFile_Rename(const char *path_old, const char *path_new)
{
    esFile_FileInfo fi;
    esFile_DriveInfo *dInfos = NULL;
    uint8_t *buffer = NULL;
    int did = 0, did_new = 0, infoLoc = 0, sector = 0;

    ENTER_CRITICAL();

    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();

    did = esFile_DiskDriveIdFromPath(path_old);
    did_new = esFile_DiskDriveIdFromPath(path_new);

    if (did == did_new)
    {
        if (esFile_Stat(path_new, NULL))
        {            
            infoLoc = esFile_GetFileInfo(did, path_old, &fi);
            if (infoLoc >= 0)
            {
                sector = infoLoc / dInfos[did].sectorCapacity;
                if (esFile_DiskRead(did, sector, buffer, 0, dInfos[did].sectorCapacity) == 0)
                {
                    memset(fi.name, 0, sizeof(fi.name));
                    strcpy(fi.name, path_new);
                    memcpy(&buffer[infoLoc % dInfos[did].sectorCapacity], &fi, sizeof(esFile_FileInfo));
                    esFile_DiskWrite(did, sector, buffer, 0, dInfos[did].sectorCapacity);
                }
                else
                {
                    ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
                }
            }
        }
        else
        {
            ESFILE_LOG("New File name is already taken: %s %d \n", __FILE__, __LINE__);
        }
    }
    else
    {
        ESFILE_LOG("Drives are not match: %s %d \n", __FILE__, __LINE__);
    }

    LEAVE_CRITICAL();
    return 0;
}
