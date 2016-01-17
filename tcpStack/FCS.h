//----------------------------------------------------------------------------
//  Copyright 2015 Robert Kimball
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http ://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//----------------------------------------------------------------------------

#ifndef FCS_H
#define FCS_H

#include <cinttypes>

class FCS
{
public:
   static uint16_t Checksum( uint8_t* buffer, int length );
   static uint32_t ChecksumAdd( uint8_t* buffer, int length, uint32_t checksum );
   static uint16_t ChecksumComplete( uint32_t checksum );
};

#endif
