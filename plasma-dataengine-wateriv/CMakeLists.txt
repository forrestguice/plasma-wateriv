project(plasma-dataengine-wateriv)
set (wateriv_engine_VERSION_MAJOR 0)
set (wateriv_engine_VERSION_MINOR 2)
set (wateriv_engine_VERSION_PATCH 0)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES})
 
set(wateriv_engine_SRCS waterivengine.cpp ivrequest.cpp)
 
kde4_add_plugin(plasma_engine_wateriv ${wateriv_engine_SRCS})
target_link_libraries(plasma_engine_wateriv
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_PLASMA_LIBS}
                      ${QT_QTNETWORK_LIBRARY}
                      ${QT_QTXML_LIBRARY})
 
install(TARGETS plasma_engine_wateriv
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-dataengine-wateriv.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

# CPack driven installer package
include (InstallRequiredSystemLibraries)

set (CPACK_PACKAGE_VERSION_MAJOR "${wateriv_engine_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${wateriv_engine_VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${wateriv_engine_VERSION_PATCH}")

set (CPACK_SOURCE_IGNORE_FILES "Makefile;lib/;CMakeFiles/;CMakeTmp/;CMakeCache.txt;./*.cmake;./*automoc*;./moc_*;./*.tar.gz;./.svn/")
set (CPACK_GENERATOR "TGZ")
set (CPACK_SOURCE_GENERATOR "TGZ")

set (CPACK_RESOURCE_FILE_README "README.txt")
set (CPACK_RESOURCE_FILE_LICENSE "COPYING.txt")

set (CPACK_STRIP_FILES "lib/plasma_dataengine_wateriv.so")
include (CPack)
