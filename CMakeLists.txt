cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(bite VERSION 0.1.0 LANGUAGES C)

message("Compiling for ${CMAKE_SYSTEM_NAME}")

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set(USE_FLAGS "-sFULL_ES2 --emrun")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX .js)
    set(EMSCRIPTEN 1)
endif()

if (WIN32 AND MSVC)
    if (MINGW)
        set(LIBRARIES mingw32)
    endif()
    set(LIBRARIES ${LIBRARIES} opengl32 glu32 winmm gdi32)
else ()
    set(LIBRARIES X11 GL dl m)
endif()

file(GLOB BITE_SOURCE bite.c)

add_library(biteLibStatic STATIC ${BITE_SOURCE})
target_link_libraries(biteLibStatic ${LIBRARIES})
set_target_properties(biteLibStatic PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

if (NOT EMSCRIPTEN)
    add_library(biteLib SHARED ${BITE_SOURCE})
    target_link_libraries(biteLib ${LIBRARIES})
    set_target_properties(biteLib PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
endif()

add_executable(example main.c)
target_link_libraries(example biteLibStatic)

if (EMSCRIPTEN)
    set_target_properties(example PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
    file(GLOB INDEX_HTML index.html)
    add_custom_command(TARGET example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${INDEX_HTML} "$<TARGET_FILE_DIR:example>")
endif()