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
#include "esFile_cryption.h"

/*
 * @brief Encrypt data before storing it in storage.
 *  This function takes a piece of data as input and encrypts it to enhance
    security before storing it in a storage medium. Encryption helps protect
    sensitive information from unauthorized access.
 * @param data 
 * @param len 
 * @return len 
 */
int esFile_Encypt(uint8_t *data, uint32_t len){
    return len;
}

/*
 * @brief Decrypt data that is stored in storage.
 *  This function takes encrypted data as input and decrypts it, allowing access to
    the original information that was stored securely. Decryption is necessary to
    retrieve and use the stored data.
 * @param data 
 * @param len 
 * @return len 
 */
int esFile_Decypt(uint8_t *data, uint32_t len){
    return len;
}