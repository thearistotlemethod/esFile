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

#ifndef ESFILE_DEFINITIONS_H__
#define ESFILE_DEFINITIONS_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern void PrintUart(char *fmt, ...);
extern void DisableInterrupts(void);
extern void EnableInterrupts(void);

#define ESFILE_VERSION                      2023
#define ESFILE_BUFFERSIZE                   ESFTL_NANDPAGESIZE

#define ESFILE_LOG(f_, ...)                 PrintUart((f_), ##__VA_ARGS__)
#define ENTER_CRITICAL()                    DisableInterrupts()
#define LEAVE_CRITICAL()                    EnableInterrupts()

#endif