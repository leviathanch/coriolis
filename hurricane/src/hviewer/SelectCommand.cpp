
// -*- C++ -*-
//
// This file is part of the Coriolis Project.
// Copyright (C) Laboratoire LIP6 - Departement ASIM
// Universite Pierre et Marie Curie
//
// Main contributors :
//        Christophe Alexandre   <Christophe.Alexandre@lip6.fr>
//        Sophie Belloeil             <Sophie.Belloeil@lip6.fr>
//        Hugo Cl�ment                   <Hugo.Clement@lip6.fr>
//        Jean-Paul Chaput           <Jean-Paul.Chaput@lip6.fr>
//        Damien Dupuis                 <Damien.Dupuis@lip6.fr>
//        Christian Masson           <Christian.Masson@lip6.fr>
//        Marek Sroka                     <Marek.Sroka@lip6.fr>
// 
// The  Coriolis Project  is  free software;  you  can redistribute it
// and/or modify it under the  terms of the GNU General Public License
// as published by  the Free Software Foundation; either  version 2 of
// the License, or (at your option) any later version.
// 
// The  Coriolis Project is  distributed in  the hope that it  will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY  or FITNESS FOR  A PARTICULAR PURPOSE.   See the
// GNU General Public License for more details.
// 
// You should have  received a copy of the  GNU General Public License
// along with the Coriolis Project; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// License-Tag
// Authors-Tag
// ===================================================================
//
// $Id$
//
// x-----------------------------------------------------------------x 
// |                                                                 |
// |                  H U R R I C A N E                              |
// |     V L S I   B a c k e n d   D a t a - B a s e                 |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Module  :       "./SelectCommand.cpp"                      |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


# include <QMouseEvent>
# include <QKeyEvent>
# include <QAction>

# include "hurricane/Cell.h"

# include "hurricane/viewer/CellWidget.h"
# include "hurricane/viewer/SelectCommand.h"


namespace Hurricane {


// -------------------------------------------------------------------
// Class  :  "SelectCommand".


  SelectCommand::SelectCommand ()
    : AreaCommand()
    , _selectAction(NULL)
  { }


  SelectCommand::~SelectCommand ()
  { }


  void  SelectCommand::bindToAction ( QAction* action )
  {
    _selectAction = action;
  }


  bool  SelectCommand::mousePressEvent ( CellWidget* widget, QMouseEvent* event )
  {
    if ( isActive() ) return true;

    if ( (event->button() == Qt::RightButton) && !event->modifiers() ) {
      setActive     ( true );
      setStartPoint ( event->pos() );
    }

    return isActive();
  }


  bool  SelectCommand::mouseReleaseEvent ( CellWidget* widget, QMouseEvent* event )
  {
    if ( !isActive() ) return false;

    setActive ( false );

    QRect selectArea;
    if ( _startPoint == _stopPoint )
      selectArea = QRect ( _startPoint - QPoint(2,2), QSize(4,4) );
    else
      selectArea = QRect ( _startPoint, _stopPoint );
      
    widget->unselectAll ();
    forEach ( Occurrence, ioccurrence
            , widget->getCell()->getOccurrencesUnder(widget->screenToDbuBox(selectArea)) ) {
      widget->select ( *ioccurrence );
    }
    if ( _selectAction ) {
      if ( !_selectAction->isChecked() )
        _selectAction->setChecked ( true );
      else
        widget->redraw ();
    } else {
      widget->setShowSelection ( true );
      widget->redraw ();
    }

    emit selectionChanged(widget->getSelectorSet(),widget->getCell());

    return false;
  }


} // End of Hurricane namespace.
