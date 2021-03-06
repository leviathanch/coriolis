
// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC/LIP6 2008-2015, All Rights Reserved
//
// ===================================================================
//
// $Id$
//
// x-----------------------------------------------------------------x 
// |                                                                 |
// |                   C O R I O L I S                               |
// |     V L S I   B a c k e n d   D a t a - B a s e                 |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Header  :       "./SelectorCriterion.h"                    |
// | *************************************************************** |
// |  U p d a t e s                                                  |
// |                                                                 |
// x-----------------------------------------------------------------x


#ifndef  __HURRICANE_SELECTOR_CRITERION__
#define  __HURRICANE_SELECTOR_CRITERION__


#include  <string>
#include  "hurricane/Commons.h"
#include  "hurricane/Name.h"


namespace Hurricane {


  using std::string;
  class CellWidget;


  class SelectorCriterion {
    public: 
      inline             SelectorCriterion ();
      virtual           ~SelectorCriterion ();
      virtual bool       isValid           ( CellWidget* ) const = 0;
      inline  bool       isEnabled         () const;
      virtual const Net* getNet            () const;
      virtual const Box& getArea           () const;
      inline  void       enable            ();
      inline  void       disable           ();
      virtual void       doSelection       ( CellWidget* ) = 0;
      virtual void       undoSelection     ( CellWidget* );
      virtual Record*    _getRecord        () const = 0;
      virtual string     _getString        () const = 0;
      virtual string     _getTypeName      () const = 0;
    private:
      bool  _isEnabled;
  };


  inline        SelectorCriterion::SelectorCriterion () : _isEnabled(false) { }
  inline  bool  SelectorCriterion::isEnabled         () const { return _isEnabled; }
  inline  void  SelectorCriterion::enable            () { _isEnabled=true; }
  inline  void  SelectorCriterion::disable           () { _isEnabled=false; }


  class NetSelectorCriterion : public SelectorCriterion {
    public:
                         NetSelectorCriterion ( const Net* );
      virtual           ~NetSelectorCriterion ();
      virtual const Net* getNet               () const;
      virtual bool       isValid              ( CellWidget* ) const;
      virtual void       doSelection          ( CellWidget* );
      virtual void       undoSelection        ( CellWidget* );
      virtual Record*    _getRecord           () const;
      virtual string     _getString           () const;
      virtual string     _getTypeName         () const;
    protected:
      const Net* _net;
      const Name _name;
  };


  class AreaSelectorCriterion : public SelectorCriterion {
    public:
                         AreaSelectorCriterion ( const Box& );
      virtual           ~AreaSelectorCriterion ();
      virtual const Box& getArea               () const;
      virtual bool       isValid               ( CellWidget* ) const;
      virtual void       doSelection           ( CellWidget* );
      virtual Record*    _getRecord            () const;
      virtual string     _getString            () const;
      virtual string     _getTypeName          () const;
    protected:
      const Box  _area;
  };


}


INSPECTOR_P_SUPPORT(Hurricane::SelectorCriterion);


#endif  // __HURRICANE_SELECTOR_CRITERION__
