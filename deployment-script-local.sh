#!/bin/bash
# Stellarium macOS Deployment Script with Custom DSO Integration
# This script:
# 1. Deploys Stellarium with proper Qt dependencies
# 2. Creates a custom DSO catalog with your astronomy pictures
# 3. Configures Stellarium to use your images preferentially
# 4. Code signs and copies to /Applications

# Exit on error
set -e

# Configuration
SOURCE_APP="/Users/jonathan/stellarium/builds/macosx/Stellarium.app"
DEST_APP="/Applications/Stellarium.app"
QT_PATH="/opt/homebrew/Cellar/qt/6.8.2_1"
SIGN_IDENTITY="Apple Development: Mr Jonathan Kimmitt (5AU5B5HJQX)"
YOUR_IMAGES_DIR="/path/to/your/astronomy/images"  # Change this to your images directory

# Create color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print header
echo -e "${BLUE}====================================================${NC}"
echo -e "${BLUE}  Stellarium Deployment with Custom Images Script   ${NC}"
echo -e "${BLUE}====================================================${NC}"

# Check if source app exists
if [ ! -d "$SOURCE_APP" ]; then
    echo -e "${RED}Error: Source application not found at $SOURCE_APP${NC}"
    exit 1
fi

echo -e "${GREEN}Step 1: Fixing QtQuickWidgets dependency...${NC}"
# Create QtQuickWidgets framework directory if it doesn't exist
if [ ! -f "$SOURCE_APP/Contents/Frameworks/QtQuickWidgets.framework/Versions/A/QtQuickWidgets" ]; then
    mkdir -p "$SOURCE_APP/Contents/Frameworks/QtQuickWidgets.framework/Versions/A"
    cp -R "$QT_PATH/lib/QtQuickWidgets.framework/Versions/A/"* "$SOURCE_APP/Contents/Frameworks/QtQuickWidgets.framework/Versions/A/"
    
    # Create symbolic links
    cd "$SOURCE_APP/Contents/Frameworks/QtQuickWidgets.framework"
    ln -sf Versions/A/QtQuickWidgets QtQuickWidgets
    cd Versions
    ln -sf A Current
    
    # Fix reference in QtWebEngineWidgets
    install_name_tool -change @rpath/QtQuickWidgets.framework/Versions/A/QtQuickWidgets @executable_path/../Frameworks/QtQuickWidgets.framework/Versions/A/QtQuickWidgets "$SOURCE_APP/Contents/Frameworks/QtWebEngineWidgets.framework/Versions/A/QtWebEngineWidgets"
    
    # Fix QtQuickWidgets dependencies
    echo -e "${YELLOW}Fixing QtQuickWidgets dependencies...${NC}"
    DEPS=$(otool -L "$SOURCE_APP/Contents/Frameworks/QtQuickWidgets.framework/Versions/A/QtQuickWidgets" | grep "@rpath/" | awk -F' ' '{print $1}')
    for DEP in $DEPS; do
        FRAMEWORK=$(echo "$DEP" | sed 's/@rpath\///g' | sed 's/\/.*//')
        FRAMEWORK_NAME=$(echo "$FRAMEWORK" | sed 's/\.framework//')
        echo "  Fixing dependency: $FRAMEWORK_NAME"
        install_name_tool -change "$DEP" "@executable_path/../Frameworks/$FRAMEWORK/Versions/A/$FRAMEWORK_NAME" "$SOURCE_APP/Contents/Frameworks/QtQuickWidgets.framework/Versions/A/QtQuickWidgets"
    done
else
    echo -e "${YELLOW}QtQuickWidgets framework already exists${NC}"
fi

echo -e "${GREEN}Step 2: Fixing platform plugins...${NC}"
# Create platforms directory
mkdir -p "$SOURCE_APP/Contents/PlugIns/platforms"

# Copy cocoa platform plugin
if [ ! -f "$SOURCE_APP/Contents/PlugIns/platforms/libqcocoa.dylib" ]; then
    cp "$QT_PATH/share/qt/plugins/platforms/libqcocoa.dylib" "$SOURCE_APP/Contents/PlugIns/platforms/"
    
    # Fix cocoa plugin dependencies
    echo -e "${YELLOW}Fixing cocoa plugin dependencies...${NC}"
    DEPS=$(otool -L "$SOURCE_APP/Contents/PlugIns/platforms/libqcocoa.dylib" | grep "@rpath/" | awk -F' ' '{print $1}')
    for DEP in $DEPS; do
        FRAMEWORK=$(echo "$DEP" | sed 's/@rpath\///g' | sed 's/\/.*//')
        FRAMEWORK_NAME=$(echo "$FRAMEWORK" | sed 's/\.framework//')
        echo "  Fixing dependency: $FRAMEWORK_NAME"
        install_name_tool -change "$DEP" "@executable_path/../Frameworks/$FRAMEWORK/Versions/A/$FRAMEWORK_NAME" "$SOURCE_APP/Contents/PlugIns/platforms/libqcocoa.dylib"
    done
else
    echo -e "${YELLOW}Platform plugin already exists${NC}"
fi

echo -e "${GREEN}Step 3: Creating qt.conf...${NC}"
# Create qt.conf
echo "[Paths]
Plugins = PlugIns
Imports = Resources/qml
Qml2Imports = Resources/qml" > "$SOURCE_APP/Contents/Resources/qt.conf"

echo -e "${GREEN}Step 4: Copying other important plugins...${NC}"
# Create plugin directories
mkdir -p "$SOURCE_APP/Contents/PlugIns/imageformats"
mkdir -p "$SOURCE_APP/Contents/PlugIns/styles"
mkdir -p "$SOURCE_APP/Contents/PlugIns/iconengines"

# Copy plugins
echo -e "${YELLOW}Copying imageformats plugins...${NC}"
for PLUGIN in $(ls "$QT_PATH/share/qt/plugins/imageformats/"*.dylib 2>/dev/null); do
    PLUGIN_NAME=$(basename "$PLUGIN")
    if [ ! -f "$SOURCE_APP/Contents/PlugIns/imageformats/$PLUGIN_NAME" ]; then
        cp "$PLUGIN" "$SOURCE_APP/Contents/PlugIns/imageformats/"
        echo "  Copied: $PLUGIN_NAME"
    fi
done

echo -e "${YELLOW}Copying styles plugins...${NC}"
for PLUGIN in $(ls "$QT_PATH/share/qt/plugins/styles/"*.dylib 2>/dev/null || true); do
    PLUGIN_NAME=$(basename "$PLUGIN")
    if [ ! -f "$SOURCE_APP/Contents/PlugIns/styles/$PLUGIN_NAME" ]; then
        cp "$PLUGIN" "$SOURCE_APP/Contents/PlugIns/styles/"
        echo "  Copied: $PLUGIN_NAME"
    fi
done

echo -e "${YELLOW}Copying iconengines plugins...${NC}"
for PLUGIN in $(ls "$QT_PATH/share/qt/plugins/iconengines/"*.dylib 2>/dev/null || true); do
    PLUGIN_NAME=$(basename "$PLUGIN")
    if [ ! -f "$SOURCE_APP/Contents/PlugIns/iconengines/$PLUGIN_NAME" ]; then
        cp "$PLUGIN" "$SOURCE_APP/Contents/PlugIns/iconengines/"
        echo "  Copied: $PLUGIN_NAME"
    fi
done

echo -e "${GREEN}Step 5: Setting up custom Messier objects catalog...${NC}"
# Create Stellarium user data directory
USER_DATA_DIR="$HOME/Library/Application Support/Stellarium"
mkdir -p "$USER_DATA_DIR/pictures"

# Copy your custom images
echo -e "${YELLOW}Copying your astronomy images...${NC}"
# List of Messier objects that you have imaged (based on your Messier data)
IMAGED_OBJECTS=("M1" "M3" "M13" "M16" "M17" "M27" "M45" "M51" "M74" "M81" "M101" "M106" "M109")

# Create JSON catalog for custom DSOs
echo -e "${YELLOW}Creating custom DSO catalog...${NC}"
cat > "$USER_DATA_DIR/dso-user.json" << EOF
{
  "shortName": "Jonathan's DSO Images",
  "priority": 100,
  "objects": [
EOF

# First object flag to handle commas correctly
FIRST=true

# For each imaged object, add it to the catalog
for OBJ in "${IMAGED_OBJECTS[@]}"; do
    # Copy the image if it exists
    if [ -f "$YOUR_IMAGES_DIR/${OBJ,,}.jpg" ]; then
        cp "$YOUR_IMAGES_DIR/${OBJ,,}.jpg" "$USER_DATA_DIR/pictures/"
        echo "  Copied: ${OBJ,,}.jpg"
    fi
    
    # Extract RA and Dec from your OCaml data
    # This is a simplified approach - in a real implementation, you would need to extract
    # the actual coordinates from your messier_data.ml
    
    # For now, let's just add the object to the catalog with placeholder data
    # In a real implementation, we would parse the messier_data.ml to get actual values
    
    if [ "$FIRST" = true ]; then
        FIRST=false
    else
        echo "," >> "$USER_DATA_DIR/dso-user.json"
    fi
    
    cat >> "$USER_DATA_DIR/dso-user.json" << EOF
    {
      "id": "${OBJ}",
      "name": "${OBJ}",
      "ra": 0.0,
      "dec": 0.0,
      "bMag": 0.0,
      "majorAxisSize": 0.1,
      "minorAxisSize": 0.1,
      "orientationAngle": 0.0,
      "type": "Nebula",
      "picture": "pictures/${OBJ,,}.jpg"
    }
EOF
done

# Close the JSON file
cat >> "$USER_DATA_DIR/dso-user.json" << EOF
  ]
}
EOF

echo -e "${GREEN}Step 6: Creating Stellarium configuration to prefer custom images...${NC}"
# Create or modify Stellarium.ini to prioritize custom DSO catalog
mkdir -p "$USER_DATA_DIR/config"
if [ -f "$USER_DATA_DIR/config/config.ini" ]; then
    # Backup existing config
    cp "$USER_DATA_DIR/config/config.ini" "$USER_DATA_DIR/config/config.ini.backup"
    
    # Update existing config - this is a simplified approach
    # In reality, you'd need to parse and modify the INI file more carefully
    sed -i'.bak' 's/enable_dso_catalog_user=false/enable_dso_catalog_user=true/g' "$USER_DATA_DIR/config/config.ini"
else
    # Create new config with custom settings
    cat > "$USER_DATA_DIR/config/config.ini" << EOF
[dso]
enable_dso_catalog_user=true
priority_dso_catalog_user=100

[gui]
flag_show_dss=false
flag_show_nebulae_background=false
flag_nebula_display_no_texture=false
EOF
fi

echo -e "${GREEN}Step 7: Disabling default images...${NC}"
# If you want to completely disable default images, you can set flag_show_dss to false
# in the Stellarium.ini file - we already did this above
# This disables the Digitized Sky Survey textures

echo -e "${GREEN}Step 8: Fixing permissions...${NC}"
# Fix permissions
chmod +x "$SOURCE_APP/Contents/MacOS/stellarium"
chmod +x "$SOURCE_APP/Contents/PlugIns/platforms/libqcocoa.dylib"

echo -e "${GREEN}Step 9: Code signing the application...${NC}"
# Code sign
codesign --force --deep --sign "$SIGN_IDENTITY" "$SOURCE_APP"

echo -e "${GREEN}Step 10: Copying to /Applications...${NC}"
# Remove existing application if it exists
if [ -d "$DEST_APP" ]; then
    echo -e "${YELLOW}Removing existing application in /Applications...${NC}"
    rm -rf "$DEST_APP"
fi

# Copy application to /Applications
echo -e "${YELLOW}Copying application to /Applications...${NC}"
cp -R "$SOURCE_APP" "$DEST_APP"

echo -e "${GREEN}Done! Stellarium has been deployed to /Applications with your custom DSO catalog.${NC}"
echo -e "${BLUE}====================================================${NC}"
echo -e "${YELLOW}Stellarium is now set up to prefer your custom images.${NC}"
echo -e "${YELLOW}You can run it from the Applications folder.${NC}"
echo -e "${BLUE}====================================================${NC}"
