// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2014 Dalerank, dalerankn8@gmail.com

#include "hippodrome.hpp"
#include "game/resourcegroup.hpp"
#include "constants.hpp"
#include "gfx/picture.hpp"
#include "city/city.hpp"
#include "events/event.hpp"

using namespace constants;

HippodromeSection::HippodromeSection( Hippodrome& base, constants::Direction direction, Type type )
  : Building( building::fortArea, Size(5) )
{
  setState( Construction::inflammability, 0 );
  setState( Construction::collapsibility, 0 );

  _basepos = base.pos();
  _direction = direction;

  Picture pic;
  switch( type )
  {
  case middle:
    switch( _direction )
    {
    case north:
      pic = Picture::load( ResourceGroup::hippodrome, 3);
      pic.setOffset( 0, pic.height() / 2 + 16 );
    break;

    case west:
      pic = Picture::load( ResourceGroup::hippodrome, 12);
      pic.setOffset( 0, pic.height() / 2 + 16 );
    break;

    default: break;
    }
  break;

  case ended:
    switch( _direction )
    {
    case north:
      pic = Picture::load( ResourceGroup::hippodrome, 1);
      pic.setOffset( 0, pic.height() / 2 + 43 );
    break;

    case west:
      pic = Picture::load( ResourceGroup::hippodrome, 14);
      pic.setOffset( 0, pic.height() / 2 + 16 );
    break;

    default: break;
    }
  break;
  }

  setPicture( pic );
}

HippodromeSection::~HippodromeSection(){}

void HippodromeSection::destroy()
{
  Building::destroy();

  HippodromePtr hp = ptr_cast<Hippodrome>( _city()->getOverlay( _basepos ) );
  if( hp.isValid() )
  {
    events::GameEventPtr e = events::ClearLandEvent::create( _basepos );
    e->dispatch();
    _basepos = TilePos( -1, -1 );
  }
}

class Hippodrome::Impl
{
public:
  Direction direction;
  HippodromeSectionPtr sectionMiddle, sectionEnd;
};

Hippodrome::Hippodrome() : EntertainmentBuilding(Service::hippodrome, building::hippodrome, Size(15,5) ), _d( new Impl )
{
  _fgPicturesRef().resize(5);
  _d->direction = west;
  _init();

  _addNecessaryWalker( walker::charioter );
}

std::string Hippodrome::troubleDesc() const
{
  std::string ret = EntertainmentBuilding::troubleDesc();

  if( ret.empty() )
  {
    ret = isRacesCarry() ? "##trouble_hippodrome_full_work##" : "##trouble_hippodrome_no_charioters##";
  }

  return ret;
}

bool Hippodrome::canBuild(PlayerCityPtr city, TilePos pos, const TilesArray& aroundTiles) const
{
  const_cast<Hippodrome*>( this )->_checkDirection( city, pos );
  if( _d->direction != noneDirection )
  {
    const_cast<Hippodrome*>( this )->_init();
  }

  return _d->direction != noneDirection;
}

void Hippodrome::build(PlayerCityPtr city, const TilePos& pos)
{
  _checkDirection( city, pos );

  setSize( Size( 5 ) );
  EntertainmentBuilding::build( city, pos );

  TilePos offset = _d->direction == north ? TilePos( 0, 5 ) : TilePos( 5, 0 );

  _d->sectionMiddle = _addSection( HippodromeSection::middle, offset );
  _d->sectionEnd = _addSection( HippodromeSection::ended, offset * 2 );

  _init( true );
  city->addOverlay( _d->sectionMiddle.object() );
  city->addOverlay( _d->sectionEnd.object() );
}

void Hippodrome::destroy()
{
  EntertainmentBuilding::destroy();

  if( _d->sectionEnd.isValid() )
  {
    events::GameEventPtr e = events::ClearLandEvent::create( _d->sectionEnd->pos() );
    e->dispatch();
    _d->sectionEnd = 0;
  }

  if( _d->sectionMiddle.isValid() )
  {
    events::GameEventPtr e = events::ClearLandEvent::create( _d->sectionMiddle->pos() );
    e->dispatch();
    _d->sectionMiddle = 0;
  }
}

bool Hippodrome::isRacesCarry() const{ return getTraineeValue( walker::charioter ) > 0; }

void Hippodrome::_init( bool onBuild )
{
  if( onBuild )
  {
    _fgPicture( 0 ) = Picture::getInvalid();
    _fgPicture( 1 ) = Picture::getInvalid();
  }

  switch( _d->direction )
  {
  case north:
  {
    setPicture( ResourceGroup::hippodrome, 5 );
    Picture sectionMdl = Picture::load( ResourceGroup::hippodrome, 3);
    Picture sectionEnd = Picture::load( ResourceGroup::hippodrome, 1);

    if( !onBuild )
    {
      sectionMdl.setOffset( 150, 181 );
      sectionEnd.setOffset( 300, 310 );
      _fgPicture( 0 ) = sectionMdl;
      _fgPicture( 1 ) = sectionEnd;
    }
  }
  break;

  case west:
  {
    Picture pic = Picture::load( ResourceGroup::hippodrome, 10 );
    pic.setOffset( 0, pic.height() / 2 + 42 );
    setPicture( pic );
    Picture sectionMdl = Picture::load( ResourceGroup::hippodrome, 12);
    Picture sectionEnd = Picture::load( ResourceGroup::hippodrome, 14);

    if( !onBuild )
    {
      sectionMdl.setOffset( 150, 31 );
      sectionEnd.setOffset( 300, -43 );
      _fgPicture( 0 ) = sectionMdl;
      _fgPicture( 1 ) = sectionEnd;
    }
  }
  break;

  default:
    _CAESARIA_DEBUG_BREAK_IF( true && "Hippodrome: Unknown direction");
  }
}

void Hippodrome::_initAnimation()
{
  if( !isRacesCarry() )
  {
    _fgPicture( 2 ) = Picture::getInvalid();
    _fgPicture( 3 ) = Picture::getInvalid();
    _fgPicture( 4 ) = Picture::getInvalid();
    return;
  }

  switch(_d->direction)
  {
  case north:
    _fgPicture( 2 ) = Picture::load( ResourceGroup::hippodrome, 7 );
    _fgPicture( 3 ) = Picture::load( ResourceGroup::hippodrome, 8 );
    _fgPicture( 4 ) = Picture::load( ResourceGroup::hippodrome, 9 );
  break;

  case west:
    _fgPicture( 2 ) = Picture::load( ResourceGroup::hippodrome, 7 );
    _fgPicture( 3 ) = Picture::load( ResourceGroup::hippodrome, 8 );
    _fgPicture( 4 ) = Picture::load( ResourceGroup::hippodrome, 9 );
  break;

  default: break;
  }
}

HippodromeSectionPtr Hippodrome::_addSection(HippodromeSection::Type type, TilePos offset )
{
  HippodromeSectionPtr ret = new HippodromeSection( *this, _d->direction, type );
  ret->build( _city(), pos() + offset );
  ret->drop();

  return ret;
}

void Hippodrome::_checkDirection(PlayerCityPtr city, TilePos pos)
{
  const_cast<Hippodrome*>( this )->setSize( Size( 15, 5 ) );
  _d->direction = west;
  bool mayBuild = EntertainmentBuilding::canBuild( city, pos, TilesArray() ); //check horizontal direction

  if( !mayBuild )
  {
    _d->direction = north;
    const_cast<Hippodrome*>( this )->setSize( Size( 5, 15 ) );
    mayBuild = EntertainmentBuilding::canBuild( city, pos, TilesArray() ); //check vertical direction
  }

  if( !mayBuild )
  {
    _d->direction = noneDirection;
  }
}
