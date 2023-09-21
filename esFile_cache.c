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
#include "esFile_system.h"
#include "esFile_disk.h"
#include "esFile_cache.h"

static uint8_t diskBuffer[ESFILE_BUFFERSIZE + 1];
static uint8_t nandSectorTable[(ESFTL_NANDNUMBLOCKS * ESFTL_NANDNUMPAGEBLOCK) / 8];
static uint8_t simSectorTable[256 / 8];
static esFile_System fs[2];

/*
 * @brief Initialize the cache structures and buffers
 *  This function is responsible for setting up the necessary data structures
    and memory buffers for caching data. It prepares the cache for subsequent
    read and write operations.
 */
void esFile_CacheInit(void)
{
    memset(nandSectorTable, 0, sizeof(nandSectorTable));
    memset(simSectorTable, 0, sizeof(simSectorTable));
    memset(fs, 0, sizeof(fs));
    esFile_ClearDiskBuffer();
}

/*
 * @brief Retrieve the disk read-write buffer.
 *  This function returns the buffer used for reading and writing data to/from the
    disk.
 * @return The disk read-write buffer
 */
uint8_t *esFile_GetDiskBuffer(void)
{
    return diskBuffer;
}

/*
 * @brief Clear the disk read-write buffer.
 *  This function is responsible for clearing the buffer used for temporary storage
    of data read from or written to the disk. It ensures that the buffer is emptied
    and ready for new data operations.
 */
void esFile_ClearDiskBuffer(void)
{
    memset(diskBuffer, 0, sizeof(diskBuffer));
}

/*
 * @brief Retrieve the reference to the filesystems.
 *  This function returns the reference to the filesystems
    object, allowing access to multiple filesystem instances for various operations.
 * @return A pointer or reference to the filesystems
 */
esFile_System *esFile_GetFileSystems(void)
{
    return fs;
}

/*
 * @brief Populates the sector-page map table for a specific drive.
 *  Given a drive object, this function updates the sector-page map table with the
    appropriate mapping information for that drive. The sector-page map table is
    used for efficient data retrieval and storage on the drive.
 */
void esFile_EvaluateSectorTable(uint8_t did)
{
    int sno = 0;
    esFile_FileInfo fi;
    esFile_DataSectorHeader dsh;
    uint8_t temp[ESFILE_BUFFERSIZE + 1];
    esFile_DriveInfo *dInfos = NULL;

    dInfos = esFile_GetDriveInfos();

    for (int i = 1; i < dInfos[did].fiSectorCount; i++)
    {
        if (esFile_DiskRead(did, i, esFile_GetDiskBuffer(), 0, dInfos[did].sectorCapacity) == 0)
        {
            for (int j = 0; j < dInfos[did].sectorCapacity / ESFILE_FILENGTH; j++)
            {
                if (esFile_GetDiskBuffer()[j * ESFILE_FILENGTH] != 0)
                {
                    memcpy(&fi, &esFile_GetDiskBuffer()[j * ESFILE_FILENGTH], sizeof(esFile_FileInfo));
                    sno = fi.startSector;
                    while (1)
                    {
                        if (esFile_DiskRead(did, sno, temp, 0, sizeof(esFile_DataSectorHeader)) == 0)
                        {
                            memcpy(&dsh, temp, sizeof(esFile_DataSectorHeader));
                            if (dsh.uid == fi.uid)
                            {
                                esFile_SetSectorFlag(did, sno, 1);
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
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
            break;
        }
    }
}

/*
 * @brief Set the usage flag of a sector to mark it as used or unused.
 *  This function allows you to set the usage flag of a specific sector to mark it
    as used (True) or unused (False). This flag helps track the status of sectors
    for efficient data management.
 * @param did
 * @param sno
 * @param used
 */
void esFile_SetSectorFlag(int did, int sno, int used)
{
    if (did)
    {
        if (used)
            simSectorTable[sno / 8] |= 1 << sno % 8;
        else
            simSectorTable[sno / 8] &= ~(1 << sno % 8);
    }
    else
    {
        if (used)
            nandSectorTable[sno / 8] |= 1 << sno % 8;
        else
            nandSectorTable[sno / 8] &= ~(1 << sno % 8);
    }
}

/*
 * @brief Check the usage flag of a sector to determine if it is marked as used.
 *  This function allows you to check the usage flag of a specific sector to
    determine whether it is marked as used (1) or unused (0). This flag
    helps track the status of sectors for efficient data management.
 * @param did
 * @param sno
 * @return 1 if the sector is marked as used, 0 if it is unused (int)
 */
int esFile_GetSectorFlag(int did, int sno)
{
    if (did)
        return simSectorTable[sno / 8] & (1 << sno % 8);
    else
        return nandSectorTable[sno / 8] & (1 << sno % 8);
}

/*
 * @brief Evaluate the number of used files and the latest unique ID associated with a file.
 *  This function performs an evaluation to determine the count of used files and the latest unique ID associated with a file. It stores valuable
    information about the state of the file system to the cache.
 * @param did
 */
void esFile_CalculateFileCountAndUid(uint8_t did)
{
    esFile_DriveInfo *dInfos = NULL;
    esFile_FileInfo fi;
    int i, j;

    dInfos = esFile_GetDriveInfos();

    fs[did].filecount = 0;
    fs[did].lastuid = 0;

    for (i = 1; i < dInfos[did].fiSectorCount; i++)
    {
        if (esFile_DiskRead(did, i, esFile_GetDiskBuffer(), 0, dInfos[did].sectorCapacity) == 0)
        {
            for (j = 0; j < dInfos[did].sectorCapacity / ESFILE_FILENGTH; j++)
            {
                if (esFile_GetDiskBuffer()[j * ESFILE_FILENGTH] != 0)
                {
                    fs[did].filecount++;
                    memcpy(&fi, &esFile_GetDiskBuffer()[j * ESFILE_FILENGTH], sizeof(esFile_FileInfo));
                    if (fi.uid > fs[did].lastuid)
                        fs[did].lastuid = fi.uid;
                }
            }
        }
        else
        {
            ESFILE_LOG("DiskRead error: %s %d \n", __FILE__, __LINE__);
            break;
        }
    }
}
