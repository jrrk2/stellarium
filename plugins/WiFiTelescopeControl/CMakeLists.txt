SET(WIFITELESCOPE_MODULE_NAME "WiFiTelescopeControl")

ADD_DEFINITIONS(-DWIFITELESCOPE_PLUGIN_VERSION="${STELLARIUM_VERSION}")

ADD_STELLARIUM_PLUGIN(${WIFITELESCOPE_MODULE_NAME} src/WiFiTelescopeControl.hpp
                                                  src/WiFiTelescopeControl.cpp
                                                  src/WiFiTelescopeControlDialog.hpp
                                                  src/WiFiTelescopeControlDialog.cpp
                                                  src/WiFiTelescope.hpp
                                                  src/WiFiTelescope.cpp
                                                  src/gui/WiFiTelescopeControlDialog.ui
)

SET_TARGET_PROPERTIES(${WIFITELESCOPE_MODULE_NAME} PROPERTIES FOLDER "plugins/${WIFITELESCOPE_MODULE_NAME}")

TARGET_INCLUDE_DIRECTORIES(${WIFITELESCOPE_MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Activate Qt resources for this plugin
QT5_ADD_RESOURCES(WIFITELESCOPE_RES resources/WiFiTelescopeControl.qrc)
TARGET_SOURCES(${WIFITELESCOPE_MODULE_NAME} PRIVATE ${WIFITELESCOPE_RES})
