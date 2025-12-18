
add_executable(Silmaril
    src/Silmaril/Main.cpp

    src/Silmaril/Core/Types.hpp
    src/Silmaril/Core/Application.hpp
    src/Silmaril/Core/Application.cpp
)

target_include_directories(Silmaril
PRIVATE
    src
)

target_link_libraries(Silmaril
PRIVATE
)

target_compile_definitions(Silmaril
PRIVATE
    NOMINMAX
)

target_precompile_headers(Silmaril
PRIVATE
    src/Silmaril/PCH.hpp
)
