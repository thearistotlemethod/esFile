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

#ifndef ESFILE_OPEN_H__
#define ESFILE_OPEN_H__

#define	ESFILE_MODE_READ			0x01
#define	ESFILE_MODE_WRITE			0x02
#define	ESFILE_MODE_OPEN_EXISTING	0x00
#define	ESFILE_MODE_CREATE_NEW		0x04
#define	ESFILE_MODE_CREATE_ALWAYS	0x08
#define	ESFILE_MODE_OPEN_ALWAYS		0x10
#define	ESFILE_MODE_OPEN_APPEND		0x30

typedef struct {
    uint8_t did;
    uint32_t uid;
    uint32_t infoLoc;
    uint16_t sector;
    uint32_t size;
    uint32_t index;
    uint16_t currentSector;
    uint16_t sectorIndex;
    uint8_t encrypted;
} esFile_FileDescriptor;

int esFile_Open(esFile_FileDescriptor *fp, const char *path, uint8_t mode);

#endif