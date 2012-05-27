project(plasma-applet-waterwatcher)
set (waterwatcher_VERSION_MAJOR 0)
set (waterwatcher_VERSION_MINOR 3)
set (waterwatcher_VERSION_PATCH 2)
 
find_package(KDE4 REQUIRED)
 
include(KDE4Defaults)
 
install(DIRECTORY plasma_applet_wateriv.waterwatcher/
        DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/waterwatcher)
 
install(FILES plasma_applet_wateriv.waterwatcher/metadata.desktop
        DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-waterwatcher.desktop)

#
# CPack driven installer package
#
include (InstallRequiredSystemLibraries)

set (CPACK_GENERATOR "ZIP")
#set (CPACK_PACKAGE_NAME "plasma-applet-waterwatcher")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "plasma-applet-waterwatcher-${waterwatcher_VERSION_MAJOR}.${waterwatcher_VERSION_MINOR}.${waterwatcher_VERSION_PATCH}")
set (CPACK_PACKAGE_VERSION_MAJOR "${waterwatcher_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${waterwatcher_VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "${waterwatcher_VERSION_PATCH}")

set (CPACK_DEBIAN_PACKAGE_MAINTAINER "Forrest Guice <ftg2@users.sourceforge.net>")
set (CPACK_DEBIAN_PACKAGE_DESCRIPTION "Plasmoid for viewing recent USGS real-time water data.")
set (CPACK_DEBIAN_PACKAGE_SECTION "kde")
set (CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "plasma-applet-waterwatcher-${waterwatcher_VERSION_MAJOR}.${waterwatcher_VERSION_MINOR}.${waterwatcher_VERSION_PATCH}")
set (CPACK_SOURCE_IGNORE_FILES "Makefile;lib/;test/;CMakeFiles/;CMakeTmp/;_CPack_Packages/;CMakeCache.txt;./*.cmake;./*automoc*;./moc_*;./*.tar.gz;./.svn/;./*.save;./*.tmp;./install_manifest.txt")

set (CPACK_RESOURCE_FILE_README "README.txt")
set (CPACK_RESOURCE_FILE_LICENSE "COPYING.txt")

include (CPack)

