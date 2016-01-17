//----------------------------------------------------------------------------
// Copyright( c ) 2015, Robert Kimball
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------------------

#include <stdio.h>

#include "Utility.h"

void DumpData( void* buffer, int len, PrintfFunctionPtr pfunc )
{
   int   i, j;
   unsigned char* buf = (unsigned char*)buffer;
   char  tmpBuf[90];
   int            tmpIndex = 0;
   int            size = sizeof(tmpBuf);
   int            count;

   if( buf == 0 )
   {
      return;
   }

   i = 0;
   j = 0;
   while( i+1 <= len )
   {
      count = _snprintf_s( &tmpBuf[tmpIndex], size, size, "%04X ", i );
      tmpIndex += count;
      size -= count;
      for( j=0; j<16; j++ )
      {
         if( i+j < len )
         {
            count = _snprintf_s( &tmpBuf[tmpIndex], size, size, "%02X ", buf[i+j] );
            tmpIndex += count;
            size -= count;
         }
         else
         {
            count = _snprintf_s( &tmpBuf[tmpIndex], size, size, "   " );
            tmpIndex += count;
            size -= count;
         }

         if( j == 7 )
         {
            count = _snprintf_s( &tmpBuf[tmpIndex], size, size, "- " );
            tmpIndex += count;
            size -= count;
         }
      }
      count = _snprintf_s( &tmpBuf[tmpIndex], size, size, "  " );
      tmpIndex += count;
      size -= count;
      for( j=0; j<16; j++ )
      {
         if( buf[i+j] >= 0x20 && buf[i+j] <= 0x7E )
         {
            count = _snprintf_s( &tmpBuf[tmpIndex], size, size, "%c", buf[i+j] );
            tmpIndex += count;
            size -= count;
         }
         else
         {
            count = _snprintf_s( &tmpBuf[tmpIndex], size, size, "." );
            tmpIndex += count;
            size -= count;
         }
         if( i+j+1 == len )
         {
            break;
         }
      }

      i += 16;

      (void)pfunc( "%s\n", tmpBuf );
      tmpIndex = 0;
      size = sizeof(tmpBuf);
   }
}

void DumpBits( void* buf, int size, PrintfFunctionPtr pfunc )
{
   int      bitIndex;
   int      i;
   unsigned char* buffer = (unsigned char*)buf;

   for( i=0; i<size; i++ )
   {
      (void)pfunc( "%3d - ", i );
      for( bitIndex = 0x80; bitIndex != 0; bitIndex >>= 1 )
      {
         if( buffer[i] & bitIndex )
         {
            (void)pfunc( "1" );
         }
         else
         {
            (void)pfunc( "0" );
         }
      }
      (void)pfunc( "\n" );
   }
}

