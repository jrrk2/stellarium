INCLUDE_DIRECTORIES(. gui)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/src)

SET(WiFiTelescopeControl_SRCS
    WiFiTelescopeControl.hpp
    WiFiTelescopeControl.cpp
    WiFiTelescope.hpp
    WiFiTelescope.cpp
    WiFiTelescopeControlDialog.hpp
    WiFiTelescopeControlDialog.cpp
)

# Make sure the UI file path is ABSOLUTE
SET(WiFiTelescopeControl_UIS
    "${CMAKE_CURRENT_SOURCE_DIR}/gui/WiFiTelescopeControlDialog.ui"
)

# Resources files
SET(WiFiTelescopeControl_RES 
    "${CMAKE_CURRENT_SOURCE_DIR}/../resources/WiFiTelescopeControl.qrc"
)

# Create a variable to store the generated UI header file location
SET(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/gui")

# Set AutoUIC to ON to automatically handle .ui files
SET(CMAKE_AUTOUIC ON)

# Process UI and resource files
IF (${QT_VERSION_MAJOR} EQUAL "5")
    QT5_ADD_RESOURCES(WiFiTelescopeControl_RES_CXX ${WiFiTelescopeControl_RES})
ELSE()
    QT_ADD_RESOURCES(WiFiTelescopeControl_RES_CXX ${WiFiTelescopeControl_RES})
ENDIF()

# Create static library
ADD_LIBRARY(WiFiTelescopeControl-static STATIC 
    ${WiFiTelescopeControl_SRCS} 
    ${WiFiTelescopeControl_RES_CXX}
    ${WiFiTelescopeControl_UIS}
)

# Include directories for finding the generated UI header
TARGET_INCLUDE_DIRECTORIES(WiFiTelescopeControl-static PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}  # For the generated ui_*.h files
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
)

# Link libraries
TARGET_LINK_LIBRARIES(WiFiTelescopeControl-static 
    Qt${QT_VERSION_MAJOR}::Core 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# Set properties
SET_TARGET_PROPERTIES(WiFiTelescopeControl-static PROPERTIES OUTPUT_NAME "WiFiTelescopeControl")
SET_TARGET_PROPERTIES(WiFiTelescopeControl-static PROPERTIES COMPILE_FLAGS "-DQT_STATICPLUGIN")
ADD_DEPENDENCIES(AllStaticPlugins WiFiTelescopeControl-static)
SET_TARGET_PROPERTIES(WiFiTelescopeControl-static PROPERTIES FOLDER "plugins/WiFiTelescopeControl")
