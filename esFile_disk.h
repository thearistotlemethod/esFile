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

#ifndef ESFILE_DISK_H__
#define ESFILE_DISK_H__

typedef int (*funcDiskInit)(uint8_t);
typedef int (*funcDiskRead)(int, uint8_t *, int, int);
typedef int (*funcDiskWrite)(int, uint8_t *, int, int);
typedef int (*funcDiskRelease)(int);

typedef struct {
    uint16_t fiSectorCount;
    uint16_t dataSectorStart;
    uint16_t dataSectorEnd;
    uint16_t sectorCapacity;
    funcDiskInit diskInit;
    funcDiskRead diskRead;
    funcDiskWrite diskWrite;
    funcDiskRelease diskRelease;
} esFile_DriveInfo;

void esFile_DiskInit(uint8_t format);
int esFile_DiskRead(int pdrv, int sector, uint8_t *buff, int idx, int count);
int esFile_DiskWrite(int pdrv, int sector, uint8_t *buff, int idx, int count);
int esFile_DiskRelease(int pdrv, int sector);
int esFile_DiskDriveIdFromPath(const char *path);
esFile_DriveInfo *esFile_GetDriveInfos(void);

#endif