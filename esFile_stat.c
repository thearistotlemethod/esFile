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
#include "esFile_stat.h"

/*
 * @brief Retrieve the status of a file.
 *  This function retrieves the status (metadata) of a file located at the specified
    file path. It provides information such as file size, permissions, and modification
    timestamps, allowing you to query various attributes of the file.
 * @param path 
 * @param info 
 * @return 0 if it is successful 
 */
int esFile_Stat(const char *path, esFile_FileInfo *info)
{
    esFile_FileInfo fi;
    int rv = -1, did = 0, infoLoc = 0;

    if (path == NULL)
    {
        return -1;
    }

    ENTER_CRITICAL();

    did = esFile_DiskDriveIdFromPath(path);
    infoLoc = esFile_GetFileInfo(did, path, &fi);
    if (infoLoc >= 0)
    {
        if(info)
            memcpy(info, &fi, sizeof(esFile_FileInfo));

        rv = 0;
    }

    LEAVE_CRITICAL();
    return rv;
}

/*
 * @brief Retrieve the offset of the cursor within a file.
 *  This function retrieves the current position (offset) of the cursor within the
    specified file. It allows you to determine the exact location within the file
    where the next read or write operation will occur.
 * @param fp 
 * @return int 
 */
int esFile_Tell(esFile_FileDescriptor *fp)
{
    if (fp == NULL)
    {
        return -1;
    }

    return fp->index;
}

/*
 * @brief Retrieve the size of an opened file.
 *  This function retrieves the size (in bytes) of the currently opened file. It provides
    information about the amount of data stored in the file that is currently being
    accessed.
 * @param fp 
 * @return The size of the opened file in bytes (int)
 */
int esFile_Size(esFile_FileDescriptor *fp)
{
    if (fp == NULL)
    {
        return -1;
    }

    return fp->size;
}
