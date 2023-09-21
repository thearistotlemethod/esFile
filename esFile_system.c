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
#include "esFile_disk.h"
#include "esFile_system.h"
#include "esFile_cache.h"

static int IsUidUsed(uint8_t did, uint32_t uid);

/*
 * @brief The size of the opened file in bytes (int)
 *  This function reads the file system data from a storage disk, allowing you to
    access and work with the file system's structure and information stored on the disk.
 * @param did 
 * @return 0 if it is successful 
 */
int esFile_ReadFileSystem(uint8_t did)
{
    uint8_t *buffer = NULL;
    esFile_System *fs = NULL;
    
    buffer = esFile_GetDiskBuffer();
    fs = esFile_GetFileSystems();

    if (esFile_DiskRead(did, 0, buffer, 0, sizeof(esFile_System)) == 0)
    {
        memcpy(&fs[did], buffer, sizeof(esFile_System));
        return 0;
    }
    else
    {
        ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
    }

    return -1;
}

/*
 * @brief Write file system data to a storage disk.
 *  This function writes file system data to a storage disk, allowing you to
    store or update the file system's structure and information on the disk.
 * @param did 
 * @return 0 if it is successful 
 */
int esFile_WriteFileSystem(uint8_t did)
{
    esFile_DriveInfo *dInfos = NULL;
    uint8_t *buffer = NULL;
    esFile_System *fs = NULL;

    dInfos = esFile_GetDriveInfos();
    buffer = esFile_GetDiskBuffer();
    fs = esFile_GetFileSystems();

    esFile_ClearDiskBuffer();
    memcpy(buffer, &fs[did], sizeof(esFile_System));

    if (esFile_DiskWrite(did, 0, buffer, 0, dInfos[did].sectorCapacity) == 0)
    {
        return 0;
    }

    return -2;
}

/*
 * @brief Calculate the disk usage.
 *  This function calculates and returns the total disk usage, which represents
    the amount of storage space used on the disk. It provides valuable information
    about the disk's capacity and how much of it is currently in use.
 * @param did 
 * @return The total disk usage in bytes (int)
 */
int esFile_CalcDiskUsage(uint8_t did){
    uint32_t disksize = 0;
    esFile_FileInfo fi;
    uint8_t *buffer = NULL;
    esFile_DriveInfo *dInfos = NULL;

    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();

    for (int i = 1; i < dInfos[did].fiSectorCount; i++)
    {
        if (esFile_DiskRead(did, i, buffer, 0, dInfos[did].sectorCapacity) == 0)
        {
            for (int j = 0; j < dInfos[did].sectorCapacity / ESFILE_FILENGTH; j++)
            {
                if (buffer[j * ESFILE_FILENGTH] != 0)
                {
                    memcpy(&fi, &buffer[j * ESFILE_FILENGTH], sizeof(esFile_FileInfo));
                    disksize += fi.size;
                }
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
            break;
        }
    }

    disksize = (disksize / (dInfos[did].sectorCapacity - sizeof(esFile_DataSectorHeader))) + 1;
    ESFILE_LOG("esFile_CalcDiskUsage: %d\n", disksize);
    return disksize;
}

/*
 * @brief Retrieve an available file info index.
 *  This function retrieves an available index for storing file information. The
    index can be used to track and manage file-related data structures and resources.
 * @param did 
 * @return An available file info index (int)
 */
int esFile_GetFreeFileInfoIndex(uint8_t did)
{
    int rv = -1;
    uint8_t *buffer = NULL;
    esFile_DriveInfo *dInfos = NULL;

    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();

    for (int i = 1; i < dInfos[did].fiSectorCount; i++)
    {
        if (esFile_DiskRead(did, i, buffer, 0, dInfos[did].sectorCapacity) == 0)
        {
            for (int j = 0; j < dInfos[did].sectorCapacity / ESFILE_FILENGTH; j++)
            {
                if (buffer[j * ESFILE_FILENGTH] == 0)
                {
                    rv = i * dInfos[did].sectorCapacity + j * ESFILE_FILENGTH;
                    return rv;
                }
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
            break;
        }
    }

    return rv;
}

/*
 * @brief Read file information for a specific file.
 *  This function retrieves information about a particular file based on its
    path. It provides access to metadata and attributes associated with the file.
 * @param did 
 * @param path 
 * @param fi 
 * @return file info index (int)
 */
int esFile_GetFileInfo(uint8_t did, const char *path, esFile_FileInfo *fi)
{
    int rv = -1, filecount = 0;
    uint8_t *buffer = NULL;
    esFile_System *fs = NULL;
    esFile_DriveInfo *dInfos = NULL;

    if (fi == NULL)
    {
        return -1;
    }

    buffer = esFile_GetDiskBuffer();
    fs = esFile_GetFileSystems();
    dInfos = esFile_GetDriveInfos();

    for (int i = 1; i < dInfos[did].fiSectorCount; i++)
    {
        if (esFile_DiskRead(did, i, buffer, 0, dInfos[did].sectorCapacity) == 0)
        {
            for (int j = 0; j < dInfos[did].sectorCapacity / ESFILE_FILENGTH; j++)
            {
                memcpy(fi, &buffer[j * ESFILE_FILENGTH], sizeof(esFile_FileInfo));
                if (!strcmp(fi->name, path))
                {
                    rv = i * dInfos[did].sectorCapacity + j * ESFILE_FILENGTH;
                    return rv;
                }

                if (fi->name[0])
                {
                    filecount++;
                }

                if (filecount >= fs[did].filecount)
                {
                    return rv;
                }
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error drive: %d sector:%d: %s %d \n", did, i, __FILE__, __LINE__);
            break;
        }
    }

    return rv;
}

/*
 * @brief Write file information for a specific file.
 *  This function writes or updates information for a particular file based on index. It allows you to store metadata and attributes associated with the file.
 * @param did 
 * @param fi 
 * @param idx 
 * @return 0 if it is successful
 */
int esFile_WriteFileInfo(uint8_t did, esFile_FileInfo *fi, int idx)
{
    uint8_t *buffer = NULL;
    esFile_DriveInfo *dInfos = NULL;
    int sector = 0;

    buffer = esFile_GetDiskBuffer();
    dInfos = esFile_GetDriveInfos();

    sector = idx / dInfos[did].sectorCapacity;
    if (esFile_DiskRead(did, sector, buffer, 0, dInfos[did].sectorCapacity) == 0)
    {
        memcpy(&buffer[idx % dInfos[did].sectorCapacity], fi, sizeof(esFile_FileInfo));
        esFile_DiskWrite(did, sector, buffer, 0, dInfos[did].sectorCapacity);
        return 0;
    }
    else
    {
        ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
        return -1;
    }
}

/*
 * @brief Read file information for a specific file.
 *  This function reads information about a particular file based on index. It provides access to metadata and attributes associated with the file.
 * @param did 
 * @param fi 
 * @param idx 
 * @return 0 if it is successful
 */
int esFile_ReadFileInfo(uint8_t did, esFile_FileInfo *fi, int idx)
{
    esFile_DriveInfo *dInfos = NULL;
    uint8_t *buffer = NULL;
    int sector = 0;

    dInfos = esFile_GetDriveInfos();
    buffer = esFile_GetDiskBuffer();

    sector = idx / dInfos[did].sectorCapacity;
    if (sector < 1 || sector >= dInfos[did].fiSectorCount)
    {
        ESFILE_LOG("ReadFileInfo error: %s %d \n", __FILE__, __LINE__);
        return -1;
    }

    if (esFile_DiskRead(did, sector, buffer, idx % dInfos[did].sectorCapacity, sizeof(esFile_FileInfo)) == 0)
    {
        memcpy(fi, buffer, sizeof(esFile_FileInfo));
        return 0;
    }
    else
    {
        ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
        return -1;
    }
}

/*
 * @brief Generate a unique ID for a file.
 *  This function generates a unique identifier that can be used to uniquely identify
    a file within a file system or storage system. The unique ID is typically used for
    file management and retrieval purposes.
 * @param did 
 * @return A unique file ID (uint32_t)
 */
uint32_t esFile_GenerateUid(uint8_t did)
{
    esFile_System *fs = NULL;

    fs = esFile_GetFileSystems();
    while (1)
    {
        fs[did].lastuid += 1;
        if (fs[did].lastuid >= 0xFFFFFFFF)
        	fs[did].lastuid = 1;

        if (IsUidUsed(did, fs[did].lastuid))
            break;
    }

    return fs[did].lastuid;
}

/*
 * @brief Update metadata for a specific sector.
 *  This function updates the metadata associated with a particular sector, identified
    by its sector number. It allows you to modify or add metadata to the sector's attributes.
 * @param did 
 * @param sno 
 * @param dsh 
 * @return 0 if it is successful
 */
int esFile_UpdateDataSectorHeader(uint8_t did, uint16_t sno, esFile_DataSectorHeader *dsh)
{
	int rv = -1;
    esFile_DriveInfo *dInfos = NULL;
    uint8_t *buffer = NULL;

    dInfos = esFile_GetDriveInfos();
    buffer = esFile_GetDiskBuffer();

    esFile_DiskRead(did, sno, buffer, 0, dInfos[did].sectorCapacity);
    memcpy(buffer, dsh, sizeof(esFile_DataSectorHeader));
    esFile_DiskWrite(did, sno, buffer, 0, dInfos[did].sectorCapacity);
    rv = 0;

    return rv;
}

/*
 * @brief Retrieve a free sector for data storage.
 *  This function retrieves and provides access to a sector that is currently
    available and not in use for data storage. It allows you to obtain a sector
    for storing new data.
 * @param did 
 * @return The number of the retrieved free sector (int)
 */
int esFile_GetFreeSector(uint8_t did)
{
    esFile_DriveInfo *dInfos = NULL;
    uint32_t count = 0, sno = 0;
    int rv = -1, i = 0;

    dInfos = esFile_GetDriveInfos();    
    count = dInfos[did].dataSectorEnd - dInfos[did].dataSectorStart;

    for (i = 0; i < count; i++)
    {
        sno = dInfos[did].dataSectorStart + i;
        if (!esFile_GetSectorFlag(did, sno))
        {
            rv = sno;
            esFile_SetSectorFlag(did, sno, 1);
            break;
        }
    }

    return rv;
}

/*
 * @brief Check if a unique ID is already in use.
 *  This function checks whether a specific unique ID is already in use within the
    system. It is commonly used to ensure the uniqueness of identifiers before
    assigning them to new entities.
 * @param did 
 * @param uid 
 * @return 0 if it is used
 */
static int IsUidUsed(uint8_t did, uint32_t uid)
{
    esFile_DriveInfo *dInfos = NULL;
    uint8_t *buffer = NULL;
    esFile_System *fs = NULL;
    esFile_FileInfo fi;
    int rv = -1, i = 0, j = 0, filecount = 0;

    dInfos = esFile_GetDriveInfos();
    buffer = esFile_GetDiskBuffer();
    fs = esFile_GetFileSystems();

    for (i = 1; i < dInfos[did].fiSectorCount; i++)
    {
        if (esFile_DiskRead(did, i, buffer, 0, dInfos[did].sectorCapacity) == 0)
        {
            for (j = 0; j < dInfos[did].sectorCapacity / ESFILE_FILENGTH; j++)
            {
                memcpy(&fi, &buffer[j * ESFILE_FILENGTH], sizeof(esFile_FileInfo));
                if (fi.name[0])
                {
                    filecount++;
                    if (fi.uid == uid)
                        return 0;
                }

                if (filecount >= fs[did].filecount)
                {
                    return rv;
                }
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
            break;
        }
    }

    return rv;
}

