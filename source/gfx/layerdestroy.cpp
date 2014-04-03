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

#include "layerdestroy.hpp"
#include "layerconstants.hpp"
#include "events/event.hpp"
#include "walker/constants.hpp"
#include "walker/walker.hpp"
#include "core/foreach.hpp"
#include "tilemap_camera.hpp"
#include "city/city.hpp"
#include "core/event.hpp"

using namespace constants;

namespace gfx
{

void LayerDestroy::_clearAll()
{
  TilesArray tiles4clear = _getSelectedArea();
  foreach( tile, tiles4clear )
  {
    events::GameEventPtr event = events::ClearLandEvent::create( (*tile)->pos() );
    event->dispatch();
  }
}

void LayerDestroy::_drawTileInSelArea( Engine& engine, Tile& tile, Tile* master, const Point& offset )
{
  if( master==NULL )
  {
    // single-tile
    drawTile( engine, tile, offset );
    engine.drawPicture( _clearPic, tile.mapPos() + offset );
  }
  else
  {
    if( master->getFlag( Tile::isDestructible ) )
    {
      engine.setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );
    }

    // multi-tile: draw the master tile.
    if( !master->getFlag( Tile::wasDrawn ) )
      drawTile( engine, *master, offset );

    engine.resetTileDrawMask();
  }
}


void LayerDestroy::render( Engine& engine )
{
  // center the map on the screen
  Point cameraOffset = _camera()->getOffset();

  _camera()->startFrame();

  const TilesArray& visibleTiles = _camera()->getTiles();

  Tilemap& tmap = _city()->tilemap();

  std::set<int> hashDestroyArea;
  TilesArray destroyArea = _getSelectedArea();

  //create list of destroy tiles add full area building if some of it tile constain in destroy area
  foreach( it, destroyArea)
  {
    Tile* tile = *it;
    hashDestroyArea.insert( tile->j() * 1000 + tile->i() );

    TileOverlayPtr overlay = tile->overlay();
    if( overlay.isValid() )
    {
      TilesArray overlayArea = tmap.getArea( overlay->pos(), overlay->size() );
      foreach( ovelayTile, overlayArea )
      {
        hashDestroyArea.insert( (*ovelayTile)->j() * 1000 + (*ovelayTile)->i() );
      }
    }
  }
  //Rect destroyArea = Rect( startPos.getI(), startPos.getJ(), stopPos.getI(), stopPos.getJ() );

  // FIRST PART: draw all flat land (walkable/boatable)
  foreach( it, visibleTiles )
  {
    Tile* tile = *it;
    Tile* master = tile->masterTile();

    if( !tile->isFlat() )
      continue;

    int tilePosHash = tile->j() * 1000 + tile->i();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      _drawTileInSelArea( engine, *tile, master, cameraOffset );
    }
    else
    {
      if( master==NULL )
      {
        // single-tile
        drawTile( engine, *tile, cameraOffset );
      }
      else if( !master->getFlag( Tile::wasDrawn ) )
      {
        // multi-tile: draw the master tile.
        drawTile( engine, *master, cameraOffset );
      }
    }
  }

  // SECOND PART: draw all sprites, impassable land and buildings
  //WalkerList walkerList = _getVisibleWalkerList();
  foreach( it, visibleTiles )
  {
    Tile* tile = *it;
    int z = tile->pos().z();

    int tilePosHash = tile->j() * 1000 + tile->i();
    if( hashDestroyArea.find( tilePosHash ) != hashDestroyArea.end() )
    {
      if( tile->getFlag( Tile::isDestructible ) )
      {
        engine.setTileDrawMask( 0x00ff0000, 0, 0, 0xff000000 );
      }
    }

    drawTileR( engine, *tile, cameraOffset, z, false );

    _drawWalkers( engine, *tile, cameraOffset );
    engine.resetTileDrawMask();
  }
}

void LayerDestroy::handleEvent(NEvent& event)
{
  if( event.EventType == sEventMouse )
  {
    switch( event.mouse.type  )
    {
    case mouseMoved:
    {
      _setLastCursorPos( event.mouse.pos() );
      if( !event.mouse.isLeftPressed() || _startCursorPos().x() < 0 )
      {
        _setStartCursorPos( _lastCursorPos() );
      }
    }
    break;

    case mouseLbtnPressed:
    {
      _setStartCursorPos( _lastCursorPos() );
    }
    break;

    case mouseLbtnRelease:            // left button
    {
      Tile* tile = _camera()->at( event.mouse.pos(), false );  // tile under the cursor (or NULL)
      if( tile == 0 )
      {
        break;
      }

      _clearAll();
      _setStartCursorPos( _lastCursorPos() );
    }
    break;

    case mouseRbtnRelease:
    {
      _setNextLayer( citylayer::simple );
    }
    break;

    default:
    break;
    }
  }

  if( event.EventType == sEventKeyboard )
  {
    bool pressed = event.keyboard.pressed;
    int moveValue = _camera()->getScrollSpeed() * ( event.keyboard.shift ? 4 : 1 ) * (pressed ? 1 : 0);

    switch( event.keyboard.key )
    {
    case KEY_UP:    _camera()->moveUp   ( moveValue ); break;
    case KEY_DOWN:  _camera()->moveDown ( moveValue ); break;
    case KEY_RIGHT: _camera()->moveRight( moveValue ); break;
    case KEY_LEFT:  _camera()->moveLeft ( moveValue ); break;
    default: break;
    }
  }
}

int LayerDestroy::getType() const
{
  return citylayer::destroy;
}

std::set<int> LayerDestroy::getVisibleWalkers() const
{
  std::set<int> ret;
  ret.insert( walker::all );

  return ret;
}

void LayerDestroy::drawTile( Engine& engine, Tile& tile, Point offset )
{
  Point screenPos = tile.mapPos() + offset;

  TileOverlayPtr overlay = tile.overlay();

  if( overlay.isValid() )
  {
    registerTileForRendering( tile );
  }

  if( !tile.getFlag( Tile::wasDrawn ) )
  {
    tile.setWasDrawn();
    engine.drawPicture( tile.picture(), screenPos );

    if( tile.animation().isValid() )
    {
      engine.drawPicture( tile.animation().currentFrame(), screenPos );
    }

    drawTilePass( engine, tile, offset, Renderer::foreground );
  }
}

LayerPtr LayerDestroy::create( Camera& camera, PlayerCityPtr city)
{
  LayerPtr ret( new LayerDestroy( camera, city ) );
  ret->drop();

  return ret;
}

LayerDestroy::LayerDestroy( Camera& camera, PlayerCityPtr city)
  : Layer( &camera, city )
{
  _clearPic = Picture::load( "oc3_land", 2 );
}

}//end namespace gfx
