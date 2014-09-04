#!/usr/bin/env python
#
# This file is part of the Coriolis Software.
# Copyright (c) UPMC 2014-2014, All Rights Reserved
#
# +-----------------------------------------------------------------+
# |                   C O R I O L I S                               |
# |      C u m u l u s  -  P y t h o n   T o o l s                  |
# |                                                                 |
# |  Author      :                    Jean-Paul CHAPUT              |
# |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
# | =============================================================== |
# |  Python      :       "./plugins/ClockTreePlugin.py"             |
# +-----------------------------------------------------------------+

try:
  import sys
  import traceback
  import os.path
  import math
  import Cfg
  import Hurricane
  import Viewer
  import CRL
  from   CRL import RoutingLayerGauge
  import helpers
  from   helpers   import trace
  from   helpers   import ErrorMessage
  import Mauka
  import Unicorn
  import plugins
  import clocktree.ClockTree
  import chip.Configuration
except ImportError, e:
  module = str(e).split()[-1]

  print '[ERROR] The <%s> python module or symbol cannot be loaded.' % module
  print '        Please check the integrity of the <coriolis> package.'
  sys.exit(1)
except Exception, e:
  print '[ERROR] A strange exception occurred while loading the basic Coriolis/Python'
  print '        modules. Something may be wrong at Python/C API level.\n'
  print '        %s' % e
  sys.exit(2)


# --------------------------------------------------------------------
# Plugin hook functions, unicornHook:menus, ScritMain:call

def unicornHook ( **kw ):
    plugins.kwUnicornHook( 'plugins.clockTree'
                         , 'ClockTree'
                         , 'Build a buffered H-Tree for the clock'
                         , sys.modules[__name__].__file__
                         , **kw
                         )
    return


def ScriptMain ( **kw ):
  try:
    helpers.staticInitialization( quiet=True )
   #helpers.setTraceLevel( 550 )

    errorCode = 0

    print '  o  Cleaning up any previous run.'
    for fileName in os.listdir('.'):
      if fileName.endswith('.ap'):
        print '      - <%s>' % fileName
        os.unlink(fileName)

    cell = None
    if kw.has_key('cell') and kw['cell']:
      cell = kw['cell']

    editor = None
    if kw.has_key('editor') and kw['editor']:
      editor = kw['editor']
      print '  o  Editor detected, running in graphic mode.'
      if cell == None: cell = editor.getCell()

    if cell == None:
      raise ErrorMessage( 3, 'ClockTree: No cell loaded yet.' )

    framework = CRL.AllianceFramework.get()
    cellGauge = framework.getCellGauge()

    if cell.getAbutmentBox().isEmpty():
      spaceMargin = Cfg.getParamPercentage('nimbus.spaceMargin').asPercentage() / 100.0 + 0.02
      aspectRatio = Cfg.getParamPercentage('nimbus.aspectRatio').asPercentage() / 100.0
      clocktree.ClockTree.computeAbutmentBox( cell, spaceMargin, aspectRatio, cellGauge )
      if editor: editor.fit()

    ht = clocktree.ClockTree.HTree.create( chip.Configuration.GaugeConfWrapper(chip.Configuration.GaugeConf())
                                         , cell, None, cell.getAbutmentBox() )
    if editor: editor.refresh()
    mauka = Mauka.MaukaEngine.create( cell )
    mauka.run()
    mauka.destroy()
    ht.connectLeaf()
    ht.prune()
    ht.route()
    ht.save( cell )

   #showNet( cell, 'ck_htree_bl_bl_bl' )
   #showNet( cell, 'ck_htree_bl_bl_br' )
   #showNet( cell, 'ck_htree_bl_bl_tl' )
   #showNet( cell, 'ck_htree_bl_bl_tr' )

  except ErrorMessage, e:
      print e; errorCode = e.code
  except Exception, e:
      print '\n\n', e; errorCode = 1
      traceback.print_tb(sys.exc_info()[2])
      
  return 0