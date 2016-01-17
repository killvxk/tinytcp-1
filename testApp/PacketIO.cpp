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

#include <pcap.h>
#include <Packet32.h>
#include <ntddndis.h>

#include "PacketIO.h"
#include "Utility.h"

#define Max_Num_Adapter 10
char		AdapterList[ Max_Num_Adapter ][ 1024 ];

int PacketIO::GetMACAddress( const char* adapter, uint8_t* mac )
{
   LPADAPTER	lpAdapter = 0;
   int			i;
   DWORD		   dwErrorCode;
   char		   AdapterName[ 8192 ];
   char		   *temp, *temp1;
   int			AdapterNum = 0, Open;
   ULONG		   AdapterLength;
   PPACKET_OID_DATA  OidData;
   BOOLEAN		Status;

   lpAdapter = PacketOpenAdapter( (char*)adapter );

   if( !lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE) )
   {
      dwErrorCode = GetLastError();
      printf( "Unable to open the adapter, Error Code : %lx\n", dwErrorCode );

      return -1;
   }

   // 
   // Allocate a buffer to get the MAC adress
   //

   OidData = (PACKET_OID_DATA*)malloc( 6 + sizeof( PACKET_OID_DATA ) );
   if( OidData == NULL )
   {
      printf( "error allocating memory!\n" );
      PacketCloseAdapter( lpAdapter );
      return -1;
   }

   // 
   // Retrieve the adapter MAC querying the NIC driver
   //

   OidData->Oid = OID_802_3_CURRENT_ADDRESS;

   OidData->Length = 6;
   ZeroMemory( OidData->Data, 6 );

   Status = PacketRequest( lpAdapter, FALSE, OidData );
   if( Status )
   {
      mac[ 0 ] = (OidData->Data)[ 0 ];
      mac[ 1 ] = (OidData->Data)[ 1 ];
      mac[ 2 ] = (OidData->Data)[ 2 ];
      mac[ 3 ] = (OidData->Data)[ 3 ];
      mac[ 4 ] = (OidData->Data)[ 4 ];
      mac[ 5 ] = (OidData->Data)[ 5 ];
      printf( "The MAC address of the adapter is %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
         (OidData->Data)[ 0 ],
         (OidData->Data)[ 1 ],
         (OidData->Data)[ 2 ],
         (OidData->Data)[ 3 ],
         (OidData->Data)[ 4 ],
         (OidData->Data)[ 5 ] );
   }
   else
   {
      printf( "error retrieving the MAC address of the adapter!\n" );
   }

   free( OidData );
   PacketCloseAdapter( lpAdapter );
   return (0);
}

void PacketIO::DisplayDevices()
{
   pcap_if_t *alldevs;
   pcap_if_t *d;
   int i = 0;
   char errbuf[ PCAP_ERRBUF_SIZE ];

   /* Retrieve the device list from the local machine */

   if( pcap_findalldevs( &alldevs, errbuf ) == -1 )
   {
      fprintf( stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf );
      exit( 1 );
   }

   /* Print the list */
   for( d = alldevs; d != NULL; d = d->next )
   {
      printf( "%d. %s", ++i, d->name );
      if( d->description )
      {
         printf( " (%s)\n", d->description );
      }
      else
      {
         printf( " (No description available)\n" );
      }
   }

   if( i == 0 )
   {
      printf( "\nNo interfaces found! Make sure WinPcap is installed.\n" );
      return;
   }

   /* We don't need any more the device list. Free it */
   pcap_freealldevs( alldevs );
}

void PacketIO::GetDevice( int interfaceNumber, char* buffer, size_t buffer_size )
{
   pcap_if_t *alldevs;
   pcap_if_t *d;
   int i = 0;
   char errbuf[ PCAP_ERRBUF_SIZE ];

   /* Retrieve the device list from the local machine */
   
   if( pcap_findalldevs( &alldevs, errbuf ) == -1 )
   {
      fprintf( stderr, "Error in pcap_findalldevs_ex: %s\n", errbuf );
      exit( 1 );
   }

   /* Print the list */
   d = alldevs;
   for( int i = 0; i < interfaceNumber-1; i++ )
   {
      if( d )
      {
         d = d->next;
      }
   }
   buffer[ 0 ] = 0;
   if( d )
   {
      strncpy( buffer, d->name, buffer_size );
   }

   /* We don't need any more the device list. Free it */
   pcap_freealldevs( alldevs );
}

PacketIO::PacketIO( const char* name ) :
   CaptureDevice( name )
{
   char errbuf[PCAP_ERRBUF_SIZE];

   /* Open the device */
   /* Open the adapter */
   if( (adhandle= pcap_open_live(CaptureDevice,   // name of the device
         65536,         // portion of the packet to capture. 
                                 // 65536 grants that the whole packet will be captured on all the MACs.
                      1,            // promiscuous mode (nonzero means promiscuous)
                      1,         // read timeout
                      errbuf         // error buffer
                      )) == NULL)
   {
      fprintf( stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", CaptureDevice );
   }
}

//============================================================================
//
//============================================================================

void PacketIO::Start( pcap_handler handler )
{
   /* start the capture */
   pcap_loop( adhandle, 0, handler, NULL );
}

//============================================================================
//
//============================================================================

void PacketIO::Stop( void )
{
   pcap_close( adhandle );
}

//============================================================================
//
//============================================================================

void PacketIO::TxData( void* packet, size_t length )
{
   //printf( "Ethernet Tx:\n" );
   //DumpData( packet, length, printf );

   if( pcap_sendpacket( adhandle, (u_char*)packet, length ) != 0 )
   {
      fprintf( stderr, "\nError sending the packet: %s\n", pcap_geterr( adhandle ) );
   }
}