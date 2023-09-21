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
#include "esFile_remove.h"
#include "esFile_open.h"
#include "esFile_seek.h"

/*
 * @brief Open/Create a file for reading or/and writing.
 *  This function opens a file at the specified file path using the given mode.
    It allows you to perform reading or writing operations on the file.
 * @param fp 
 * @param path 
 * @param mode 
 * @return 0 if it is successful 
 */
int esFile_Open(esFile_FileDescriptor *fp, const char *path, uint8_t mode)
{
    esFile_FileInfo fi;
    esFile_System *fs = NULL;
    int rv = -1, did = 0, infoLoc = 0;

    if (fp == NULL || path == NULL)
    {
        return -1;
    }

    ENTER_CRITICAL();
    
    did = esFile_DiskDriveIdFromPath(path);
    fs = esFile_GetFileSystems();

    infoLoc = esFile_GetFileInfo(did, path, &fi);
    if (mode & ESFILE_MODE_CREATE_NEW || mode & ESFILE_MODE_CREATE_ALWAYS)
    {
        if (infoLoc >= 0)
        {
            fi.uid = esFile_GenerateUid(did);
            fi.size = 0;

            esFile_ReleaseSectors(did, &fi);
            esFile_SetSectorFlag(did, fi.startSector, 1);

            esFile_DataSectorHeader dsh = {infoLoc, fi.uid, 0, 0};
            esFile_UpdateDataSectorHeader(did, fi.startSector, &dsh);

            esFile_WriteFileInfo(did, &fi, infoLoc);
        }
        else
        {
            infoLoc = esFile_GetFreeFileInfoIndex(did);
            if (infoLoc >= 0)
            {
                int freesector = esFile_GetFreeSector(did);
                if (freesector > 0)
                {
                    memset(&fi, 0, sizeof(esFile_FileInfo));
                    strcpy(fi.name, path);
                    fi.uid = esFile_GenerateUid(did);
                    fi.startSector = freesector;
                    fi.encrypted = 1;

                    esFile_DataSectorHeader dsh = {infoLoc, fi.uid, 0, 0};
                    esFile_UpdateDataSectorHeader(did, fi.startSector, &dsh);

                    esFile_WriteFileInfo(did, &fi, infoLoc);

                    fs[did].filecount++;
                }
                else
                {
                    ESFILE_LOG("Disk Full: %s %d\n", __FILE__, __LINE__);
                    infoLoc = -1;
                }
            }
            else
            {
                ESFILE_LOG("File Limit is reached: %s %d\n", __FILE__, __LINE__);
            }
        }
    }

    if (infoLoc >= 0)
    {
        fp->did = did;
        fp->uid = fi.uid;
        fp->infoLoc = infoLoc;
        fp->sector = fi.startSector;
        fp->size = fi.size;
        fp->index = 0;
        fp->currentSector = fi.startSector;
        fp->sectorIndex = sizeof(esFile_DataSectorHeader);
        fp->encrypted = fi.encrypted;

        if (mode & ESFILE_MODE_OPEN_APPEND)
        {
            esFile_Seek(fp, fp->size);
        }

        rv = 0;
    }

    LEAVE_CRITICAL();
    return rv;
}
