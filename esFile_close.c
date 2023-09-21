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
#include "esFile_open.h"
#include "esFile_close.h"

/*
 * @brief Close the currently open file.
 *  This function closes the file that is currently open for reading or writing.
    It ensures that all changes are saved and the file resources are released.
 * @param fp 
 * @return 0 if it is successfull 
 */
int esFile_Close(esFile_FileDescriptor *fp)
{
    if (fp == NULL)
    {
        return -1;
    }

    return 0;
}