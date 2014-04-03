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

#include "mission_target_window.hpp"
#include "game/resourcegroup.hpp"
#include "gfx/decorator.hpp"
#include "gui/label.hpp"
#include "listbox.hpp"
#include "game/player.hpp"
#include "texturedbutton.hpp"
#include "core/gettext.hpp"
#include "gfx/engine.hpp"
#include "groupbox.hpp"
#include "environment.hpp"
#include "city/city.hpp"
#include "core/foreach.hpp"
#include "core/stringhelper.hpp"
#include "city/win_targets.hpp"
#include "core/logger.hpp"
#include "gameautopause.hpp"

using namespace gfx;

namespace gui
{

class MissionTargetsWindow::Impl
{
public:
  GameAutoPause locker;
  PlayerCityPtr city;
  PictureRef background;
  Label* title;
  Label* subTitle;
  Label* lbPopulation;
  Label* lbProsperity;
  Label* lbFavour;
  Label* lbCulture;
  Label* lbPeace;
  Label* lbShortDesc;

  ListBox* lbxHelp;
};

MissionTargetsWindow* MissionTargetsWindow::create(Widget* parent, PlayerCityPtr city, int id )
{
  Size size( 610, 430 );

  Rect rectangle( Point( (parent->width() - size.width())/2, (parent->height() - size.height())/2 ), size );
  MissionTargetsWindow* ret = new MissionTargetsWindow( parent, id, rectangle );
  ret->setCity( city );
  return ret;
}

MissionTargetsWindow::~MissionTargetsWindow()
{

}

MissionTargetsWindow::MissionTargetsWindow( Widget* parent, int id, const Rect& rectangle ) 
  : Widget( parent, id, rectangle ), _d( new Impl )
{
  _d->locker.activate();
  _d->background.reset( Picture::create( size() ) );

  PictureDecorator::draw( *_d->background, Rect( Point( 0, 0 ), size() ), PictureDecorator::whiteFrame );

  Label* lbToCity = new Label( this, Rect( width() / 2, height() - 40, width() - 110, height() - 10 ), _("##mission_wnd_tocity##" ) );
  lbToCity->setTextAlignment( alignCenter, alignCenter );

  TexturedButton* btnExit = new TexturedButton( this, Point( width() - 110, height() - 40), Size( 27 ), -1, 179 );
  CONNECT( btnExit, onClicked(), this, MissionTargetsWindow::deleteLater );

  _d->title = new Label( this, Rect( 16, 16, width() - 16, 16 + 30), "##player_name##");
  //_d->subTitle = new Label( this, Rect( 16, _d->title->getBottom(), getWidth() - 16, _d->title->getBottom() + 20), "##sub_title##" );

  GroupBox* gbTargets = new GroupBox( this, Rect( 16, 64, width() - 64, 64 + 80), Widget::noId, GroupBox::blackFrame );
  Label* lbTtargets = new Label( gbTargets, Rect( 15, 0, 490, 28), _("##mission_wnd_targets_title##") );
  lbTtargets->setFont( Font::create( FONT_1_WHITE ) );
  lbTtargets->setTextAlignment( alignUpperLeft, alignUpperLeft );

  _d->lbPopulation = new Label( gbTargets, Rect( 16, 32, 16 + 240, 32 + 20), _("##mission_wnd_population##"), false, Label::bgSmBrown );
  _d->lbProsperity = new Label( gbTargets, Rect( 16, 54, 16 + 240, 54 + 20), _("##mission_wnd_prosperity##"), false, Label::bgSmBrown );

  _d->lbFavour = new Label( gbTargets, Rect( 270, 10, 270 + 240, 10 + 20), _("##mission_wnd_favour##"), false, Label::bgSmBrown );
  _d->lbCulture = new Label( gbTargets, Rect( 270, 32, 270 + 240, 32 + 20), _("##mission_wnd_culture##"), false, Label::bgSmBrown );
  _d->lbPeace = new Label( gbTargets, Rect( 270, 54, 270 + 240, 54 + 20), _("##mission_wnd_peace##"), false, Label::bgSmBrown );
  _d->lbShortDesc = new Label( gbTargets, Rect( 16, 54, 270 + 240, 54 + 20), "", false, Label::bgSmBrown );

  _d->lbxHelp = new ListBox( this, Rect( 16, 152, width() - 20, height() - 40 ) );
  _d->lbxHelp->setItemFont( Font::create( FONT_2_WHITE ) );
  _d->lbxHelp->setItemTextOffset( Point( 10, 0 ) );
}

void MissionTargetsWindow::draw( gfx::Engine& painter )
{
  if( !isVisible() )
    return;

  if( _d->background )
  {
    painter.drawPicture( *_d->background, screenLeft(), screenTop() );
  }

  Widget::draw( painter );
}

void MissionTargetsWindow::setCity(PlayerCityPtr city)
{
  _d->city = city;
  const city::WinTargets& wint = _d->city->getWinTargets();
  _d->lbCulture->setVisible( wint.needCulture() > 0 );
  _d->lbPeace->setVisible( wint.needPeace() > 0 );
  _d->lbFavour->setVisible( wint.needFavour() > 0 );
  _d->lbProsperity->setVisible( wint.needProsperity() > 0 );
  _d->title->setText( _d->city->player()->getName()  );
  _d->lbShortDesc->setVisible( !wint.getShortDesc().empty() );

  std::string text = StringHelper::format( 0xff, "%s:%d", _("##mission_wnd_population##"), wint.needPopulation() );
  _d->lbPopulation->setText( text );

  text = StringHelper::format( 0xff, "%s:%d", _("##senatepp_prsp_rating##"), wint.needProsperity() );
  _d->lbProsperity->setText( text );

  text = StringHelper::format( 0xff, "%s:%d", _("##senatepp_favour_rating##"), wint.needFavour() );
  _d->lbFavour->setText( text );

  text = StringHelper::format( 0xff, "%s:%d", _("##senatepp_clt_rating##"), wint.needCulture() );
  _d->lbCulture->setText( text );

  text = StringHelper::format( 0xff, "%s:%d", _("##senatepp_peace_rating##"), wint.needPeace() );
  _d->lbPeace->setText( text );

  _d->lbxHelp->setItemFont( Font::create( FONT_2_WHITE ) );
  _d->lbxHelp->setItemTextOffset( Point( 20, 0 ) );
  _d->lbxHelp->setItemDefaultColor( ListBoxItem::LBC_TEXT, 0xffe0e0e0 );

  foreach( it, wint.getOverview() )
  {
    std::string text = *it;
    if( text.substr( 0, 5 ) == "@img=" )
    {
      Picture pic = Picture::load( text.substr( 5 ) );
      ListBoxItem& item = _d->lbxHelp->addItem( pic );
      item.setItemTextAlignment( alignCenter, alignUpperLeft );
      int lineCount = pic.height() / _d->lbxHelp->getItemHeight();
      StringArray lines;
      lines.resize( lineCount );
      _d->lbxHelp->addItems( lines );
    }
    else { _d->lbxHelp->fitText( _( text ) ); }
  }

  _d->lbShortDesc->setText( _(wint.getShortDesc()) );
}

}//end namespace gui
