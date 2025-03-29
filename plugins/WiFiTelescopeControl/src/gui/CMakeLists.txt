SET(WiFiTelescopeControl_GUI_SRCS
    ../WiFiTelescopeControlDialog.hpp
    ../WiFiTelescopeControlDialog.cpp
)

# Process UI files
SET(WiFiTelescopeControl_GUI_UIS
    WiFiTelescopeControlDialog.ui
)

IF (${QT_VERSION_MAJOR} EQUAL "5")
    QT5_WRAP_UI(WiFiTelescopeControl_GUI_UIS_H ${WiFiTelescopeControl_GUI_UIS})
ELSE()
    QT_WRAP_UI(WiFiTelescopeControl_GUI_UIS_H ${WiFiTelescopeControl_GUI_UIS})
ENDIF()

# Create GUI library
ADD_LIBRARY(WiFiTelescopeControl_gui STATIC
    ${WiFiTelescopeControl_GUI_SRCS}
    ${WiFiTelescopeControl_GUI_UIS_H}
)

TARGET_LINK_LIBRARIES(WiFiTelescopeControl_gui
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Set folder for IDE organization
SET_TARGET_PROPERTIES(WiFiTelescopeControl_gui PROPERTIES FOLDER "plugins/WiFiTelescopeControl")
