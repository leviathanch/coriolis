
// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC/LIP6 2008-2010, All Rights Reserved
//
// ===================================================================
//
// $Id$
//
// x-----------------------------------------------------------------x 
// |                                                                 |
// |                   C O R I O L I S                               |
// |          U n i c o r n  -  M a i n   G U I                      |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Header  :       "./ImportCellDialog.h"                     |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


#ifndef  __UNICORN_IMPORT_CELL_DIALOG_H__
#define  __UNICORN_IMPORT_CELL_DIALOG_H__

#include  <QDialog>

class QCheckBox;
class QComboBox;
class QLineEdit;


namespace Unicorn {


  class ImportCellDialog : public QDialog {
      Q_OBJECT;

    public:
      enum Formats { AcmSigda=1, Ispd04, Iccad04, AllianceDef };
    public:
                     ImportCellDialog ( QWidget* parent=NULL );
      bool           runDialog        ( QString& name, int& format, bool& newViewerRequest );
      const QString  getCellName      () const;
      bool           useNewViewer     () const;
      int            getFormat        () const;
    protected:
      QLineEdit*     _lineEdit;
      QCheckBox*     _viewerCheckBox;
      QComboBox*     _formatComboBox;
  };


} // End of Unicorn namespace.


#endif
