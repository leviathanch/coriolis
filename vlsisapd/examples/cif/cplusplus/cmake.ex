PROJECT(DRIVECIF)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.0)

SET(CMAKE_MODULE_PATH "$ENV{VLSISAPD_USER_TOP}/share/cmake/Modules"
                      "$ENV{VLSISAPD_TOP}/share/cmake/Modules" 
   )

FIND_PACKAGE(VLSISAPD REQUIRED)

IF(CIF_FOUND)
    INCLUDE_DIRECTORIES ( ${CIF_INCLUDE_DIR} )
    ADD_EXECUTABLE ( driveCif driveCif.cpp )
    TARGET_LINK_LIBRARIES ( driveCif ${CIF_LIBRARY} )
    INSTALL ( TARGETS driveCif DESTINATION . )
ENDIF(CIF_FOUND)
