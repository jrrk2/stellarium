# This is the cmake config file for the WiFi Telescope Control plugin
SET(WIFITELESCOPE_VERSION "${VERSION}")
ADD_DEFINITIONS(-DWIFITELESCOPE_PLUGIN_VERSION="${WIFITELESCOPE_VERSION}")
ADD_DEFINITIONS(-DWIFITELESCOPE_PLUGIN_LICENSE="GNU GPLv2 or later")

# Add the src subdirectory where the actual build logic resides
ADD_SUBDIRECTORY(src)
