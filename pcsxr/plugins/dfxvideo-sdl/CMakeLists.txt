message("Configuring dfxvideo-sdl")


#deps

#defs
add_definitions(-DLOCALE_DIR="${CMAKE_INSTALL_FULL_DATAROOTDIR}/locale/" -DPSEMU_DATA_DIR="${CMAKE_INSTALL_FULL_DATAROOTDIR}/psemu" -DDEF_PLUGIN_DIR="${CMAKE_INSTALL_FULL_LIBDIR}/games/psemu")

#sources
set(PLUGIN_SRCS
    gpu.c
)

include_directories(${CMAKE_SOURCE_DIR}/libpcsxcore)
add_library(DFXSDLVideo MODULE ${PLUGIN_SRCS})
#target_link_libraries(DFXSDVideo ${X11_LIBRARIES} -lm)
