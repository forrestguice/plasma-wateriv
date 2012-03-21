project(plasma-engine-wateriv)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES})
 
set(wateriv_engine_SRCS waterivengine.cpp)
 
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
