/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifdef WEBSTREAMER

#include <thread>
#include <libavformat/version.h>

#include "flowDeviceToWebStream.h"
#include "Utils.h"

namespace remo
{
  flowDeviceToWebStream::flowDeviceToWebStream ( stream* inStream_,
                                                 stream* outStream_)
    : flow ( inStream_, outStream_ ),
    _stop ( false )
  {
    _inDevice = static_cast<streamDeviceIn*>( _inStream );
    _outWebStreamer = static_cast<streamWebStreamer*>( _outStream );

    init ( );
  }

  void flowDeviceToWebStream::init ( )
  {
    Utils::getInstance ( )->getLog ( ) ( LOG_LEVEL::INFO,
                                         "Init FFmpeg/libAV functionality on on device to viewer flow",
                                         this->getDescription ( ));

#if LIBAVCODEC_VERSION_MAJOR < 57
    av_register_all ( );
    avcodec_register_all ( );
#endif

    avdevice_register_all ( );
    Utils::getInstance ( )->getLog ( ) ( LOG_LEVEL::INFO,
                                         "All required functions are registered successfully." );

    Utils::getInstance ( )
      ->getLog ( ) ( LOG_LEVEL::INFO, "Init in stream flow." );
    if ( _inStream != nullptr )
    {
      _inDevice->init ( );
    }
    else
    {
      Utils::getInstance ( )->getErrorManager ( )
                            ->criticalError ( "Error init in stream." );
    }

    Utils::getInstance ( )
      ->getLog ( ) ( LOG_LEVEL::INFO, "Init out stream flow." );
    if ( _outStream != nullptr )
    {
      _outWebStreamer->init ( );
    }
    else
    {
      Utils::getInstance ( )->getErrorManager ( )
                            ->criticalError ( "Error init out stream." );
    }

    Utils::getInstance ( )->getLog ( ) ( LOG_LEVEL::INFO,
                                         "All streams has been init succesfully." );

    if ( _ffPipeline != nullptr )
    {
      _ffPipeline->connectFramesAndPackages ( _frame, _frameProc, _packet );
    }
  }

  void flowDeviceToWebStream::releaseResources ( const std::string& msg_ )
  {
    av_packet_unref ( _packet );
    av_packet_free ( &_packet );

    av_frame_unref ( _frame );
    av_frame_free ( &_frame );

    av_frame_unref ( _frameProc );
    av_frame_free ( &_frameProc );

    Utils::getInstance ( )->getErrorManager ( )->criticalError ( msg_ );
  }

  void flowDeviceToWebStream::processStreams ( )
  {
    //### It needs to be analyzed!!! -> Sometimes crashes and others dont!
    std::thread th_ ( &flowDeviceToWebStream::loop, this );
    th_.detach ( ); //###This kills the the object of the execution at the end!

    Utils::getInstance ( )->getLog ( ) ( LOG_LEVEL::WARNING,
                                         "Press any key to finish the thread!." );
    std::cin.get ( );
    _stop = true;
  }

  void flowDeviceToWebStream::loop ( )
  {
    int value = 0;

    avformat_version ( );

    _packet = av_packet_alloc ( );
    if ( !_packet )
    {
      Utils::getInstance ( )->getErrorManager ( )->criticalError ( "Unable to"
                                                                   "reserve "
                                                                   "resources "
                                                                   "fo working "
                                                                   "packet." );
    }

    _frame = av_frame_alloc ( );
    if ( !_frame )
    {
      av_packet_unref ( _packet );
      av_packet_free ( &_packet );
      Utils::getInstance ( )->getErrorManager ( )->criticalError ( "Unable to "
                                                                   "reserve "
                                                                   "resources "
                                                                   "for "
                                                                   "working "
                                                                   "frame." );
    }

    _frameProc = av_frame_alloc ( );
    if ( !_frameProc )
    {
      av_packet_unref ( _packet );
      av_packet_free ( &_packet );
      av_frame_unref ( _frame );
      av_frame_free ( &_frame );
      Utils::getInstance ( )->getErrorManager ( )->criticalError ( "Unable to "
                                                                   "reserve "
                                                                   "resources "
                                                                   "for "
                                                                   "working "
                                                                   "YUVframe.");
    }

    int actNumFrames = 0;
    int how_many_packets_to_process = 8;

    while ( !_stop )
    {
      //Not needed!
      //if (_outWebStreamer->isSync ()) continue;

      if ( av_read_frame ( _inDevice->getFormatContext ( ), _packet ) >= 0 )
      {
        if (( _packet->stream_index == _inDevice->getVideoStreamIndx ( ))
          && ( --how_many_packets_to_process < 0 ))
        {
          value = avcodec_send_packet ( _inDevice->getCodecContext ( ), _packet );
          if ( value < 0 )
          {
            releaseResources ( "Unable to decode video." );
          }

          value = avcodec_receive_frame ( _inDevice->getCodecContext ( ), _frame );

          if ( value == AVERROR( EAGAIN ) || value == AVERROR_EOF)
          {
            Utils::getInstance ( )
              ->getLog ( ) ( LOG_LEVEL::INFO, "Error receiving frame." );
            break;
          }
          else if ( value < 0 )
          {
            Utils::getInstance ( )->getLog ( ) ( LOG_LEVEL::INFO,
                                                 "Legitimate decoding error:",
                                                 value );
          }
          else
          {
            if ( _ffPipeline != nullptr )
            {
              _ffPipeline->process ( );
            }
            _outWebStreamer->pushFrame (_frame);
            ++actNumFrames;
          }
        }
      }
      av_packet_unref ( _packet );
    }
    av_packet_free ( &_packet );

    av_frame_unref ( _frame );
    av_frame_free ( &_frame );

    av_frame_unref ( _frameProc );
    av_frame_free ( &_frameProc );
  }
}

#endif //WEBSTREAMER
