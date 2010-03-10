
                   include ( ${QT_USE_FILE} )

       include_directories ( ${HURRICANE_INCLUDE_DIR} )
       include_directories ( ${CRLCORE_SOURCE_DIR}/src/fonts
                             ${CRLCORE_SOURCE_DIR}/src/ccore
                             ${CRLCORE_SOURCE_DIR}/src/ccore/properties
                             ${CRLCORE_SOURCE_DIR}/src/ccore/bookshelf
                             ${CRLCORE_SOURCE_DIR}/src/ccore/cspice
                             ${CRLCORE_SOURCE_DIR}/src/ccore/lefdef
                             ${CRLCORE_SOURCE_DIR}/src/ccore/alliance/ap
                             ${CRLCORE_SOURCE_DIR}/src/ccore/alliance/vst
                             ${CRLCORE_SOURCE_DIR}/src/ccore/agds
                             ${CRLCORE_SOURCE_DIR}/src/ccore/cif
                             ${CRLCORE_SOURCE_DIR}/src/ccore/spice
                             ${CRLCORE_SOURCE_DIR}/src/ccore/liberty
                             ${CRLCORE_SOURCE_DIR}/src/ccore/toolbox
                           )

           add_definitions ( -DCORIOLIS_TOP=\\"${PROJECT_BINARY_DIR}\\" )

                       set ( includes          crlcore/Utilities.h
                                               crlcore/Memory.h
                                               crlcore/Banner.h
                                               crlcore/COptions.h
                                               crlcore/XmlParser.h
                                               crlcore/GdsDriver.h
                                               crlcore/CifDriver.h
                                               crlcore/SearchPath.h
                                               crlcore/Environment.h
                                               crlcore/Catalog.h
                                               crlcore/AllianceLibrary.h
                                               crlcore/ParsersDrivers.h
                                               crlcore/LefDefExtension.h
                                               crlcore/Ioc.h
                                               crlcore/NetExtension.h
                                               crlcore/RoutingGauge.h
                                               crlcore/RoutingLayerGauge.h
                                               crlcore/CellGauge.h
                                               crlcore/SymbolicTechnologyParser.h
                                               crlcore/RealTechnologyParser.h
                                               crlcore/GraphicsParser.h
                                               crlcore/AllianceFramework.h
                                               crlcore/ToolEngine.h
                                               crlcore/ToolEngines.h
                                               crlcore/ToolBox.h
                                               crlcore/Hierarchy.h
                                               crlcore/CellPath.h
                                               crlcore/CellPaths.h
                                               crlcore/TimingEvent.h
                           )
                       set ( mocincludes       crlcore/GraphicToolEngine.h )
                       set ( ccore_cpps        Utilities.cpp
                                               Memory.cpp
                                               Banner.cpp
                                               COptions.cpp
                                               XmlParser.cpp
                                               GdsDriver.cpp
                                               CifDriver.cpp
                                               SearchPath.cpp
                                               Environment.cpp
                                               Catalog.cpp
                                               AllianceLibrary.cpp
                                               ParsersDrivers.cpp
                                               RoutingGauge.cpp
                                               CellGauge.cpp
                                               RoutingLayerGauge.cpp
                                               SymbolicTechnologyParser.cpp
                                               RealTechnologyParser.cpp
                                               GraphicsParser.cpp
                                               AllianceFramework.cpp
                                               ToolEngine.cpp
                                               GraphicToolEngine.cpp
                           )
                       set ( spice_cpps        spice/SpiceParser.cpp
                                               spice/SpiceDriver.cpp
                                               spice/Spice.cpp
                           )
                       set ( bookshelf_cpps    bookshelf/BookshelfParser.cpp
                                               bookshelf/BookshelfDriver.cpp
                           )
                       set ( ap_cpps           alliance/ap/ApParser.cpp
                                               alliance/ap/ApDriver.cpp
                           )
                       set ( agds_cpps         agds/AgdsDriver.cpp
                           )
                       set ( cif_cpps          cif/CifDriver.cpp
                           )
                       set ( toolbox_cpps      toolbox/HyperNetPortOccurrences.cpp
                                               toolbox/ToolBox.cpp
                                               toolbox/UniqueCellOccurrences.cpp
                                               toolbox/RoutingPads.cpp
                           )
                       set ( vst_driver_cpps   alliance/vst/VstDriver.cpp )
                       set ( properties_cpps   properties/NetExtension.cpp )
#                      set ( liberty_cpps      liberty/CellPath.cpp
#                                              liberty/LuTableTemplate.cpp
#                                              liberty/LibertyTechnology.cpp
#                                              liberty/TimingEvent.cpp
#                                              liberty/LuTable.cpp
#                          )
                       set ( lefdef_cpps       lefdef/LefDef.h
                                               lefdef/LefDefExtension.cpp
                                               lefdef/LefParser.cpp
                                               lefdef/DefParser.cpp
                                               lefdef/DefDriver.cpp
                           )

 if ( OA_FOUND )
                           include_directories ( ${OA_INCLUDE_DIR} )
                           set( openaccess_cpps openaccess/COpenAccessBridgeLayer.cpp
                                                openaccess/OAWrapper.cpp
                           )
 endif ( OA_FOUND )
 if ( LEFDEF_FOUND )
                           include_directories ( ${LEFDEF_INCLUDE_DIR} )
 endif ( LEFDEF_FOUND )
                         set (openaccess_cpps ${openaccess_cpps}
                                              openaccess/OpenAccessWrapper.cpp )

                         set ( VstParserSourceDir  ${CRLCORE_SOURCE_DIR}/src/ccore/alliance/vst )
                         set ( VstParserBinaryDir  ${CRLCORE_BINARY_DIR}/src/ccore/alliance/vst )
                         set ( VstParserScanner    ${VstParserSourceDir}/VstParserScanner.ll  )
                         set ( VstParserGrammar    ${VstParserSourceDir}/VstParserGrammar.yy  )
                         set ( VstParserScannerCpp ${VstParserBinaryDir}/VstParserScanner.cpp )
                         set ( VstParserGrammarCpp ${VstParserBinaryDir}/VstParserGrammar.cpp )
            add_subdirectory ( alliance )
           add_custom_target ( VstParser echo "Creating VST parser" )
          add_custom_command ( SOURCE   ${VstParserScanner}
                               COMMAND  ${FLEX_EXECUTABLE}
                               ARGS     -PVST -o${VstParserScannerCpp} ${VstParserScanner}
                               TARGET   VstParser
                               OUTPUTS  ${VstParserScannerCpp}
                             )
          add_custom_command ( SOURCE   ${VstParserGrammar}
                               COMMAND  ${BISON_EXECUTABLE} 
                               ARGS     -d -v -p VST -y ${VstParserGrammar} -o ${VstParserGrammarCpp}
                               TARGET   VstParser
                               DEPENDS  ${VstParserScannerCpp}
                               OUTPUTS  ${VstParserGrammarCpp}
                             )
         include_directories ( ${VstParserBinaryDir} )
                         set ( vst_parser_cpps ${VstParserScannerCpp}
                                               ${VstParserGrammarCpp}
                             )
 set_source_files_properties ( ${VstParserScannerCpp} GENERATED )
 set_source_files_properties ( ${VstParserGrammarCpp} GENERATED )


                         set ( IocParserSourceDir  ${CRLCORE_SOURCE_DIR}/src/ccore/ioc )
                         set ( IocParserBinaryDir  ${CRLCORE_BINARY_DIR}/src/ccore/ioc )
                         set ( IocParserScanner    ${IocParserSourceDir}/IocParserScanner.ll  )
                         set ( IocParserGrammar    ${IocParserSourceDir}/IocParserGrammar.yy  )
                         set ( IocParserScannerCpp ${IocParserBinaryDir}/IocParserScanner.cpp )
                         set ( IocParserGrammarCpp ${IocParserBinaryDir}/IocParserGrammar.cpp )
            add_subdirectory ( ioc )
           add_custom_target ( IocParser echo "Creating IOC parser" )
          add_custom_command ( SOURCE   ${IocParserScanner}
                               COMMAND  ${FLEX_EXECUTABLE}
                               ARGS     -PIoc -o${IocParserScannerCpp} ${IocParserScanner}
                               TARGET   IocParser
                               OUTPUTS  ${IocParserScannerCpp}
                             )
          add_custom_command ( SOURCE   ${IocParserGrammar}
                               COMMAND  ${BISON_EXECUTABLE} 
                               ARGS     -d -v -p Ioc -y ${IocParserGrammar} -o ${IocParserGrammarCpp}
                               TARGET   IocParser
                               DEPENDS  ${IocParserScannerCpp}
                               OUTPUTS  ${IocParserGrammarCpp}
                             )
         include_directories ( ${IocParserBinaryDir} )
                         set ( ioc_parser_cpps ${IocParserScannerCpp}
                                               ${IocParserGrammarCpp}
                             )
 set_source_files_properties ( ${IocParserScannerCpp} GENERATED )
 set_source_files_properties ( ${IocParserGrammarCpp} GENERATED )


# #                          set ( LibertyParserSourceDir  ${CRLCORE_SOURCE_DIR}/src/ccore/liberty )
# #                          set ( LibertyParserBinaryDir  ${CRLCORE_BINARY_DIR}/src/ccore/liberty )
# #                          set ( LibertyParserScanner    ${LibertyParserSourceDir}/LibertyParserScanner.ll  )
# #                          set ( LibertyParserGrammar    ${LibertyParserSourceDir}/LibertyParserGrammar.yy  )
# #                          set ( LibertyParserScannerCpp ${LibertyParserBinaryDir}/LibertyParserScanner.cpp )
# #                          set ( LibertyParserGrammarCpp ${LibertyParserBinaryDir}/LibertyParserGrammar.cpp )
# #             add_subdirectory ( liberty )
# #            add_custom_target ( LibertyParser echo "Creating LIBERTY parser" )
# #           add_custom_command ( SOURCE   ${LibertyParserScanner}
#                                COMMAND  ${FLEX_EXECUTABLE}
#                                ARGS     -PLIBERTY -o${LibertyParserScannerCpp} ${LibertyParserScanner}
#                                TARGET   LibertyParser
#                                OUTPUTS  ${LibertyParserScannerCpp}
#                              )
#           add_custom_command ( SOURCE   ${LibertyParserGrammar}
#                                COMMAND  ${BISON_EXECUTABLE} 
#                                ARGS     -d -v -p LIBERTY -y ${LibertyParserGrammar} -o ${LibertyParserGrammarCpp}
#                                TARGET   LibertyParser
#                                DEPENDS  ${LibertyParserScannerCpp}
#                                OUTPUTS  ${LibertyParserGrammarCpp}
#                              )
#          include_directories ( ${LibertyParserBinaryDir} )
#                          set ( liberty_parser_cpps ${LibertyParserScannerCpp}
#                                                    ${LibertyParserGrammarCpp}
#                              )
#  set_source_files_properties ( ${LibertyParserScannerCpp} GENERATED )
#  set_source_files_properties ( ${LibertyParserGrammarCpp} GENERATED )


                qt4_wrap_cpp ( moc_cpps ${mocincludes} )
                     install ( FILES ${includes} ${mocincludes} DESTINATION /include/coriolis/crlcore ) 

                 add_library ( crlcore               ${ccore_cpps}
                                                     ${moc_cpps}
                                                     ${ap_cpps}
                                                     ${agds_cpps}
                                                     ${cif_cpps}
                                                     ${toolbox_cpps}
                                                     ${vst_parser_cpps}
                                                     ${vst_driver_cpps}
                                                     ${properties_cpps}
                                                     ${ioc_parser_cpps}
                                                     ${liberty_cpps}
                                                     ${liberty_parser_cpps}
                                                     ${bookshelf_cpps}
                                                     ${spice_cpps}
                                                     ${lefdef_cpps}
                                                     ${openaccess_cpps}
                             )
       target_link_libraries ( crlcore               ${HURRICANE_LIBRARIES}
                                                     ${HURRICANE_GRAPHICAL_LIBRARIES}
                                                     ${QT_LIBRARIES}
                                                     ${IO_LIBRARIES}
                                                     ${LEFDEF_LIBRARIES}
                                                     ${OA_LIBRARIES}
                                                     ${AGDS_LIBRARY}
                                                     ${CIF_LIBRARY}
                             )
                     install ( TARGETS               crlcore        DESTINATION /lib )