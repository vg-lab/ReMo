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

#include "Utils.h"

namespace remo
{
  Utils* Utils::_instance = nullptr;

  Utils* Utils::getInstance ( void )
  {
    if ( _instance == nullptr )
      _instance = new Utils;
    return _instance;
  }

  Utils::~Utils ( void )
  {
    if ( _instance != nullptr )
      delete _instance;
  }

  ErrorManager* Utils::getErrorManager ( void )
  {
    if ( _errorManager == nullptr )
      _errorManager->setLog ( &_logInstance );
    return _errorManager;
  };
}
