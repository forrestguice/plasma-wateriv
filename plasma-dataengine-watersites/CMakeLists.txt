project(plasma-dataengine-watersites)
set (watersites_engine_VERSION_MAJOR 0)
set (watersites_engine_VERSION_MINOR 1)
set (watersites_engine_VERSION_PATCH 0)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES})
 
set(watersites_engine_SRCS watersitesengine.cpp 
                           sitesrequest.cpp 
                           watersitesdata_mapper.cpp 
                           watersitesdata_statecodes.cpp
                           watersitesdata_countycodes.cpp
                           watersitesdata_sitetypes.cpp
                           watersitesdata_agencycodes.cpp)
 
kde4_add_plugin(plasma_engine_watersites ${watersites_engine_SRCS})
target_link_libraries(plasma_engine_watersites
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_PLASMA_LIBS}
                      ${QT_QTNETWORK_LIBRARY}
                      ${QT_QTXML_LIBRARY})
 
install(TARGETS plasma_engine_watersites
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-dataengine-watersites.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES data/statecodes.xml
        DESTINATION ${DATA_INSTALL_DIR}/plasma-dataengine-watersites)
install(FILES data/agencycodes.xml
        DESTINATION ${DATA_INSTALL_DIR}/plasma-dataengine-watersites)
install(FILES data/countycodes.xml
        DESTINATION ${DATA_INSTALL_DIR}/plasma-dataengine-watersites)
install(FILES data/sitetypecodes.xml
        DESTINATION ${DATA_INSTALL_DIR}/plasma-dataengine-watersites)

# CPack driven installer package
include (InstallRequiredSystemLibraries)

set (CPACK_PACKAGE_VERSION_MAJOR "${watersites_engine_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${watersites_engine_VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${watersites_engine_VERSION_PATCH}")

set (CPACK_GENERATOR "TGZ")

set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "plasma-dataengine-watersites-${watersites_engine_VERSION_MAJOR}.${watersites_engine_VERSION_MINOR}.${watersites_engine_VERSION_PATCH}")
set (CPACK_SOURCE_IGNORE_FILES "Makefile;lib/;CMakeFiles/;CMakeTmp/;CMakeCache.txt;./*.cmake;./*automoc*;./moc_*;./*.tar.gz;./.svn/")

#set (CPACK_RESOURCE_FILE_README "README.txt")
#set (CPACK_RESOURCE_FILE_LICENSE "COPYING.txt")

set (CPACK_STRIP_FILES "lib/plasma_dataengine_watersites.so")
include (CPack)

