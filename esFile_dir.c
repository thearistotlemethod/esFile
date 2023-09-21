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
#include "esFile_dir.h"

/*
 * @brief Open a directory for reading and exploration.
 *  This function opens a directory at the specified path, allowing you to
    read and explore its contents, such as files and subdirectories.
 * @param dp 
 * @param path 
 * @return 0 
 */
int esFile_OpenDir(esFile_DirDescriptor *dp, const char *path)
{
    if (dp == NULL)
    {
        return -1;
    }

    dp->did = esFile_DiskDriveIdFromPath(path);
    dp->idx = 0;
    return 0;
}

/*
 * @brief Read an entity (file or subdirectory) from the specified directory.
 *  This function reads an entity (either a file or a subdirectory) from the
    directory specified by the provided path. It allows you to access and
    work with the contents of the specified entity.
 * @param dp 
 * @param fiOut 
 * @return 0
 */
int esFile_ReadDir(esFile_DirDescriptor *dp, esFile_FileInfo *fiOut)
{
    esFile_FileInfo fi;
    uint8_t *buffer = NULL;
    esFile_DriveInfo *dInfos = NULL;
    int idx = 0, i = 0, j = 0, rv = -1;

    if (dp == NULL)
    {
        return -1;
    }

    ENTER_CRITICAL();

    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();
        
    for (i = 1; i < dInfos[dp->did].fiSectorCount; i++)
    {
        if (esFile_DiskRead(dp->did, i, buffer, 0, dInfos[dp->did].sectorCapacity) == 0)
        {
            for (j = 0; j < dInfos[dp->did].sectorCapacity / ESFILE_FILENGTH; j++)
            {
                memcpy(&fi, &buffer[j * ESFILE_FILENGTH], sizeof(esFile_FileInfo));
                if (idx == dp->idx)
                {
                    if(fiOut)
                        memcpy(fiOut, &fi, sizeof(esFile_FileInfo));

                    dp->idx++;

                    i = 0x0FFFFFFF;
                    rv = 0;
                    break;
                }
                idx++;
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
            break;
        }
    }

    LEAVE_CRITICAL();
    return rv;
}

/*
 * @brief Close the directory instance.
 *  This function closes the directory instance that was previously opened. It ensures
    that any resources associated with the directory are released properly.
 * @param dp 
 * @return 0 
 */
int esFile_CloseDir(esFile_DirDescriptor *dp)
{
    return 0;
}
