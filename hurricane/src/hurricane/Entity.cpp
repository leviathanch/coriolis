// ****************************************************************************************************
// File: Entity.cpp
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#include "Entity.h"
#include "Quark.h"
#include "Cell.h"
#include "Instance.h"
#include "SharedPath.h"

namespace Hurricane {



// ****************************************************************************************************
// Entity implementation
// ****************************************************************************************************

Entity::Entity()
// *************
:    Inherit()
{
}

void Entity::_preDestroy()
// **********************
{
// trace << "entering Entity::_preDestroy: " << this << endl;
// trace_in();

    vector<Entity*> slaveEntities;
    SlaveEntityMap::iterator  it;
    SlaveEntityMap::iterator  end;
    getCell()->_getSlaveEntities(this,it,end);
    for(; it != end ; it++)
      slaveEntities.push_back(it->second);
    for(; slaveEntities.size() ; slaveEntities.pop_back()) {
      cerr << "Erasing " << slaveEntities.back() << endl;
      slaveEntities.back()->destroy();
    }

    Quark* quark = _getQuark();
    if (quark) quark->destroy();

    stack<SharedPath*> sharedPathStack;
    for_each_instance(instance, getCell()->getSlaveInstances()) {
        SharedPath* sharedPath = instance->_getSharedPath(NULL);
        if (sharedPath) sharedPathStack.push(sharedPath);
        end_for;
    }
    while (!sharedPathStack.empty()) {
        SharedPath* sharedPath = sharedPathStack.top();
        sharedPathStack.pop();
        Quark* quark = _getQuark(sharedPath);
        if (quark) quark->destroy();
        Cell* cell = sharedPath->getOwnerCell();
        for_each_instance(instance, cell->getSlaveInstances()) {
            SharedPath* sharedPath2 = instance->_getSharedPath(sharedPath);
            if (sharedPath2) sharedPathStack.push(sharedPath2);
            end_for;
        }
    }

    Inherit::_preDestroy();

// trace << "exiting Entity::_preDestroy:" << endl;
// trace_out();
}

string Entity::_getString() const
// ******************************
{
    return Inherit::_getString();
}

Record* Entity::_getRecord() const
// *************************
{
    Record* record = Inherit::_getRecord();
    if (record) {
        Occurrence occurrence = Occurrence(this);
        if (occurrence.HasProperty())
            record->Add(getSlot("Occurrence", occurrence));
    }
    return record;
}



} // End of Hurricane namespace.

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************
