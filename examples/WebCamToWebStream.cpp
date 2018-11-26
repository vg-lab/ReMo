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

#include <iostream>

#include <flowDeviceToWebStream.h>
#include <mediaWebCam.h>
#include <mediaWebStreamer.h>
#include <Utils.h>

using namespace std;

int main ( )
{
  remo::Utils::getInstance ( )
    ->getLog ( ) ( remo::LOG_LEVEL::INFO, "Init logging." );

  //Define the input media and stream
  std::unique_ptr < remo::media >
    im = std::unique_ptr < remo::mediaWebCam > ( new remo::mediaWebCam ( ));
  std::unique_ptr < remo::stream >
    is = std::unique_ptr < remo::streamDeviceIn > ( new remo::streamDeviceIn
      ( im.get ( )));

  //Define the output media and stream
  std::unique_ptr < remo::media > om =
    std::unique_ptr < remo::mediaWebStreamer > ( new remo::mediaWebStreamer ( ));
  std::unique_ptr < remo::stream >
    os = std::unique_ptr < remo::streamWebStreamer > ( new
      remo::streamWebStreamer ( om.get ( )));

  //Define the flow and process
  remo::flowDeviceToWebStream f ( is.get ( ), os.get ( ));
  f.processStreams ( );

  remo::Utils::getInstance ( )->getLog ( ) ( remo::LOG_LEVEL::INFO,
                                             "WebCam to webStream successfully executed." );
  return 0;
}
