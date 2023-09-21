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

#ifndef ESFILE_SYSTEM_H__
#define ESFILE_SYSTEM_H__

typedef struct {
    uint32_t infoLoc;
    uint32_t uid;
    uint16_t presector;
    uint16_t nextsector;
    uint32_t rfu;
} esFile_DataSectorHeader;

typedef struct {
    uint32_t version;
    uint32_t lastuid;
    uint32_t filecount;
} esFile_System;

typedef struct {
    char name[64];
    uint16_t startSector;
    uint32_t size;
    uint32_t uid;
    uint8_t encrypted;
} esFile_FileInfo;
#define ESFILE_FILENGTH 128

int esFile_ReadFileSystem(uint8_t did);
int esFile_WriteFileSystem(uint8_t did);
int esFile_CalcDiskUsage(uint8_t did);
int esFile_GetFreeFileInfoIndex(uint8_t did);
int esFile_GetFileInfo(uint8_t did, const char *path, esFile_FileInfo *fi);
int esFile_WriteFileInfo(uint8_t did, esFile_FileInfo *fi, int idx);
int esFile_ReadFileInfo(uint8_t did, esFile_FileInfo *fi, int idx);
uint32_t esFile_GenerateUid(uint8_t did);
int esFile_UpdateDataSectorHeader(uint8_t did, uint16_t sno, esFile_DataSectorHeader *dsh);
int esFile_GetFreeSector(uint8_t did);

#endif