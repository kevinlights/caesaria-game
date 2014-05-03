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

#include "infobox_land.hpp"
#include "label.hpp"
#include "core/gettext.hpp"
#include "objects/constants.hpp"

using namespace constants;
using namespace gfx;

namespace gui
{

InfoboxLand::InfoboxLand( Widget* parent, const Tile& tile )
  : InfoboxSimple( parent, Rect( 0, 0, 510, 350 ), Rect( 16, 60, 510 - 16, 60 + 180) )
{ 
  Label* lbText = new Label( this, Rect( 38, 60, 470, 60+180 ), "", true, Label::bgNone, lbTextId );
  lbText->setFont( Font::create( FONT_2 ) );
  lbText->setTextAlignment( align::upperLeft, align::center );
  lbText->setWordwrap( true );

  std::string text;
  std::string title;

  if( tile.getFlag( Tile::tlTree ) )
  {
    title = "##trees_and_forest_caption##";
    text = "##trees_and_forest_text##";
  } 
  else if( tile.getFlag( Tile::tlWater ) )
  {
    title = "##water_caption##";
    text = "##water_text##";
  }
  else if( tile.getFlag( Tile::tlRock ) )
  {
    title = "##rock_caption##";
    text = "##rock_text##";
  }
  else if( tile.getFlag( Tile::tlRoad ) )
  {
    if( tile.overlay()->type() == construction::plaza )
    {
      title = "##plaza_caption##";
      text = "##plaza_text##";
    }
    else 
    {
      title = "##road_caption##";
      text = "##road_text##";
    }
  }
  else if( tile.getFlag( Tile::tlMeadow ) )
  {
    title = "##meadow_caption##";
    text = "##meadow_text##";
  }
  else 
  {
    title = "##clear_land_caption##";
    text = "##clear_land_text##";
  }
  
  //int index = (size - tile.getJ() - 1 + border_size) * 162 + tile.getI() + border_size;

  text += StringHelper::format( 0xff, "Tile at: (%d,%d) ID:%04X",
                                           tile.i(), tile.j(),  
                                          ((short int) tile.originalImgId() ) );
  
  setTitle( _( title ));
  setText( _( text ) );
}

void InfoboxLand::setText( const std::string& text )
{
  if( Widget* lb = findChild( lbTextId ) )
    lb->setText( text );
}

InfoboxFreeHouse::InfoboxFreeHouse( Widget* parent, const Tile& tile )
    : InfoboxLand( parent, tile )
{
  setTitle( _("##freehouse_caption##") );

  ConstructionPtr cnst = ptr_cast<Construction>( tile.overlay() );
  if( cnst.isValid() && cnst->getAccessRoads().size() == 0 )
  {
    setText( _("##freehouse_text_noroad##") );
  }
  else
  {
    setText( _("##freehouse_text##") );
  }
}

}//end namespace gui
