// -*- C++ -*-
//
// This file is part of the Coriolis Software.
// Copyright (c) UPMC 2014-2014, All Rights Reserved
//
// +-----------------------------------------------------------------+
// |                   C O R I O L I S                               |
// |      K i t e  -  D e t a i l e d   R o u t e r                  |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// | =============================================================== |
// |  C++ Module  :       "./BuildPreRouteds.cpp"                    |
// +-----------------------------------------------------------------+


#include <map>
#include <list>
#include "hurricane/Error.h"
#include "hurricane/Warning.h"
#include "hurricane/DataBase.h"
#include "hurricane/Technology.h"
#include "hurricane/BasicLayer.h"
#include "hurricane/RegularLayer.h"
#include "hurricane/Horizontal.h"
#include "hurricane/Vertical.h"
#include "hurricane/RoutingPad.h"
#include "hurricane/NetExternalComponents.h"
#include "hurricane/Instance.h"
#include "hurricane/Plug.h"
#include "hurricane/Path.h"
#include "hurricane/Query.h"
#include "crlcore/AllianceFramework.h"
#include "katabatic/AutoContact.h"
#include "kite/RoutingPlane.h"
#include "kite/TrackFixedSegment.h"
#include "kite/Track.h"
#include "kite/KiteEngine.h"


namespace {

  using namespace std;
  using Hurricane::tab;
  using Hurricane::inltrace;
  using Hurricane::ltracein;
  using Hurricane::ltraceout;
  using Hurricane::ForEachIterator;
  using Hurricane::Warning;
  using Hurricane::Error;
  using Hurricane::DbU;
  using Hurricane::Box;
  using Hurricane::Interval;
  using Hurricane::Horizontal;
  using Hurricane::Vertical;
  using Hurricane::RoutingPad;
  using Hurricane::NetExternalComponents;
  using Hurricane::Instance;
  using Hurricane::Plug;
  using Hurricane::Path;
  using Hurricane::Query;
  using Hurricane::Go;
  using Hurricane::Rubber;
  using Hurricane::Layer;
  using Hurricane::BasicLayer;
  using Hurricane::RegularLayer;
  using Hurricane::Transformation;
  using Hurricane::Technology;
  using Hurricane::DataBase;
  using CRL::AllianceFramework;
  using Katabatic::AutoContact;
  using Katabatic::AutoSegment;
  using Katabatic::ChipTools;
  using namespace Kite;


} // Anonymous namespace.


namespace Kite {


  using Hurricane::DataBase;
  using Hurricane::Technology;
  using Hurricane::BasicLayer;
  using Hurricane::ForEachIterator;


  void  KiteEngine::buildPreRouteds ()
  {
    forEach ( Net*, inet, getCell()->getNets() ) {
      if (*inet == _blockageNet) continue;
      if (inet->getType() == Net::Type::POWER ) continue;
      if (inet->getType() == Net::Type::GROUND) continue;
    // Don't consider the clock.

      vector<Segment*>  segments;
      vector<Contact*>  contacts;

      bool   isPreRouted = false;
      size_t rpCount     = 0;
      forEach ( Component*, icomponent, inet->getComponents() ) {
        Horizontal* horizontal = dynamic_cast<Horizontal*>(*icomponent);
        if (horizontal) {
          segments.push_back( horizontal );
          isPreRouted = true;
        } else {
          Vertical* vertical = dynamic_cast<Vertical*>(*icomponent);
          if (vertical) {
            isPreRouted = true;
            segments.push_back( vertical );
          } else {
            Contact* contact = dynamic_cast<Contact*>(*icomponent);
            if (contact) {
              isPreRouted = true;
              contacts.push_back( contact );
            } else {
              RoutingPad* rp = dynamic_cast<RoutingPad*>(*icomponent);
              if (rp) {
                ++rpCount;
              } else {
                // Plug* plug = dynamic_cast<Plug*>(*icomponent);
                // if (plug) {
                //   cerr << "buildPreRouteds(): " << plug << endl;
                //   ++rpCount;
                // }
              }
            }
          }
        }
      }

      if (isPreRouted or (rpCount < 2)) {
        _preRouteds.insert( make_pair(inet->getName(),*inet) );

        if (rpCount > 1) {
          for ( auto icontact : contacts ) {
            AutoContact::createFrom( icontact );
          }

          for ( auto isegment : segments ) {
            AutoContact* source = Session::base()->lookup( dynamic_cast<Contact*>( isegment->getSource() ));
            AutoContact* target = Session::base()->lookup( dynamic_cast<Contact*>( isegment->getTarget() ));
            AutoSegment* autoSegment = AutoSegment::create( source, target, isegment );
            autoSegment->setFlags( Katabatic::SegUserDefined|Katabatic::SegAxisSet );
          }
        }
      }
    }

    Session::revalidate ();
  }


  void  KiteEngine::setFixedPreRouted ()
  {
    for ( size_t depth=0 ; depth<_routingPlanes.size() ; ++depth ) {
      RoutingPlane* rp = _routingPlanes[depth];
      if (rp->getLayerGauge()->getType() == Constant::PinOnly ) continue;
      if (rp->getLayerGauge()->getDepth() > getConfiguration()->getAllowedDepth() ) continue;

      size_t tracksSize = rp->getTracksSize();
      for ( size_t itrack=0 ; itrack<tracksSize ; ++itrack ) {
        Track* track = rp->getTrackByIndex( itrack );

        for ( size_t ielement=0 ; ielement<track->getSize() ; ++ielement ) {
          TrackElement* element = track->getSegment( ielement );
         
          if (element->getNet() == NULL) continue;
          element->setRouted();
        }
      }
    }
  }


} // Kite namespace.