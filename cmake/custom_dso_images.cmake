# custom_dso_images.cmake
# Script to process and install custom DSO images

# Define paths
set(SOURCE_IMAGES_DIR "${CMAKE_SOURCE_DIR}/custom_images")
set(PROCESSED_IMAGES_DIR "${CMAKE_BINARY_DIR}/processed_images")
set(DESTINATION_DIR "${CMAKE_INSTALL_PREFIX}/share/stellarium/nebulae/default")

# Create the output directory
file(MAKE_DIRECTORY ${PROCESSED_IMAGES_DIR})

# Define the list of Messier objects to process
set(MESSIER_OBJECTS M1 M3 M13 M16 M17 M27 M45 M51 M74 M81 M101 M106 M109)

# Process each image
foreach(IMG IN LISTS MESSIER_OBJECTS)
    string(TOLOWER ${IMG} IMG_LOWER)
    
    # Define paths
    set(SOURCE_IMG "${SOURCE_IMAGES_DIR}/${IMG}.jpg")
    set(DEST_IMG "${PROCESSED_IMAGES_DIR}/${IMG_LOWER}.png")
    
    # Add a custom command to process the image
    add_custom_command(
        OUTPUT ${DEST_IMG}
        COMMAND ${CMAKE_COMMAND} -E echo "Processing ${IMG}..."
        COMMAND convert ${SOURCE_IMG} -crop WIDTHxHEIGHT+X+Y ${DEST_IMG}
        DEPENDS ${SOURCE_IMG}
        COMMENT "Cropping ${IMG} to proper dimensions"
    )
    
    # Add the processed image to the list of files to install
    install(FILES ${DEST_IMG} DESTINATION ${DESTINATION_DIR})
    
    # Add the output file to a list
    list(APPEND PROCESSED_FILES ${DEST_IMG})
endforeach()

# Create a custom target that depends on all processed images
add_custom_target(custom_dso_images ALL DEPENDS ${PROCESSED_FILES})
