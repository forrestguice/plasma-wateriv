project(plasma-applet-wateriv.waterwatcher)
 
find_package(KDE4 REQUIRED)
 
include(KDE4Defaults)
 
install(DIRECTORY plasma_applet_wateriv.waterwatcher/
        DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.plasma.applet.wateriv-waterwatcher)
 
install(FILES package/metadata.desktop
        DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-wateriv-waterwatcher.desktop)
