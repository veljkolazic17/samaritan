# Macro for setting up file groups
macro(SETUP_GROUPS src_files)
  foreach(FILE ${src_files})
    get_filename_component(PARENT_DIR "${FILE}" PATH)
    set(GROUP "${PARENT_DIR}")
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    source_group("${GROUP}" FILES "${FILE}")
  endforeach()
endmacro()


# Macro for setting up projects
macro(SETUP_APP projname graphics_api target_platform user_remote_logging window_api memstats test_code_enabled)
  set(PROJ_NAME ${projname})
  project(${PROJ_NAME})

  file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c??)
  file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)
  include_directories(src)

  add_executable(${PROJ_NAME} ${SRC_FILES} ${HEADER_FILES})
  SETUP_GROUPS("${SRC_FILES}")
  SETUP_GROUPS("${HEADER_FILES}")

  # Variables to be used in rest of macro
  set(CXX_COMPILE_FLAGS )

  # Platform specific flags
  if(${target_platform} MATCHES Win)
    list(APPEND CXX_COMPILE_FLAGS PUBLIC TARGET_WIN)
  elseif(${target_platform} MATCHES Linux)
    list(APPEND CXX_COMPILE_FLAGS PUBLIC TARGET_LINUX)
  endif()

   # Enable Memory stats logging
  if(${test_code_enabled} MATCHES true)
    list(APPEND CXX_COMPILE_FLAGS PUBLIC MEM_STATS_ENABLED)
  endif()

  # Enable Memory stats logging
  if(${memstats} MATCHES true)
    list(APPEND CXX_COMPILE_FLAGS PUBLIC TEST_CODE_ENBALED)
  endif()

    # Remote logging (Redis)
  if(${user_remote_logging} MATCHES true)
    list(APPEND CXX_COMPILE_FLAGS PUBLIC REMOTE_LOGGING)
    find_package(hiredis CONFIG REQUIRED)
    target_link_libraries(${PROJ_NAME} PRIVATE hiredis::hiredis)
  endif()

  # Window API
  list(APPEND CXX_COMPILE_FLAGS PUBLIC ${window_api})
  if(${window_api} MATCHES WINDOW_GLFW)
    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(${PROJ_NAME} PRIVATE glfw)
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIRS})
  elseif(${window_api} MATCHES WINDOW_SDL)

  elseif(${window_api} MATCHES WINDOW_WIN_NATIVE)

  endif()

  # Adding graphics api
  if(${graphics_api} MATCHES vulkanapi)
    find_package(Vulkan REQUIRED)
    target_link_libraries(${PROJ_NAME} PRIVATE Vulkan::Vulkan)
  elseif(${graphics_api} MATCHES directx)
    # TODO Add DirectX12 to files
  endif()

  # Add math library
  find_package(glm CONFIG REQUIRED)
  target_link_libraries(${PROJ_NAME} PRIVATE glm::glm)

  set_target_properties(${PROJ_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${PROJ_NAME}_Debug COMPILE_OPTIONS -DDEBUG)
  set_target_properties(${PROJ_NAME} PROPERTIES OUTPUT_NAME_RELEASE ${PROJ_NAME}_Release)
  set_target_properties(${PROJ_NAME} PROPERTIES OUTPUT_NAME_RELWITHDEBINFO ${PROJ_NAME}_ReleaseDebInfo)

  target_compile_definitions(${PROJ_NAME} ${CXX_COMPILE_FLAGS})

  set_property(TARGET ${PROJ_NAME} PROPERTY CXX_STANDARD 20)
  set_property(TARGET ${PROJ_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

  # compiler specific options
  if(MSVC)

    add_definitions(-D_CONSOLE)

    set_property(TARGET ${PROJ_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

  endif()

  add_custom_command(TARGET ${projname} POST_BUILD COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/shadercompile.bat ${CMAKE_CURRENT_SOURCE_DIR})

endmacro()