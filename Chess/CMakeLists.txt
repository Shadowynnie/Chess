# CMakeList.txt : CMake project for Chess, include source and define
# project specific logic here.
#
find_package(SFML 3 REQUIRED COMPONENTS Graphics)
# Add source to this project's executable.
add_executable (Chess "Chess.cpp" "Chess.h" "Tile.h" "Tile.cpp" "Figurines/Figure.h" "Figurines/Figure.cpp" "Figurines/Pawn.h" "Figurines/Pawn.cpp" "Figurines/Rook.h" "Figurines/Rook.cpp" "Figurines/Knight.h" "Figurines/Knight.cpp" "Figurines/Bishop.h" "Figurines/Bishop.cpp" "Figurines/Queen.h" "Figurines/Queen.cpp" "Figurines/King.h" "Figurines/King.cpp" "AssetManager.h" "AssetManager.cpp" "Networking/NetworkManager.h" "Networking/NetworkManager.cpp" "GameManager.h" "GameManager.cpp"  )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chess PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
# Linking with SFML libs
# graphics, window, system, audio, network, etc.
target_link_libraries(Chess PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    ${ENET_LIB}
)

# Link against ENet
#target_link_libraries(${PROJECT_NAME} ${ENET_LIB})

# Post-build step: Copy enet.dll to the output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$ENV{ENET_ROOT}/lib/enet.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)