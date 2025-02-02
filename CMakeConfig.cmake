set(EXEC_NAME camera)
set(PROJECT_VERSION 2.11)

###### Special libraries ######
set(EXTRA_LIBS "")
set(TOOL_LIBS "")		
if(0)
  set(EXTRA_LIBS "pthread" "util")
endif()

###### Boost libraries ######
if(0)
  find_package(Boost 1.56 REQUIRED COMPONENTS system filesystem iostreams)
  if (Boost_FOUND)
    list(APPEND EXTRA_LIBS ${Boost_IOSTREAMS_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
  else ()
    message("Couldn't find Boost library")
  endif (Boost_FOUND)
endif()

###### SFML libraries ######
if(0)
  find_package(SFML REQUIRED COMPONENTS graphics window system)
  if (SFML_FOUND)
    list(APPEND EXTRA_LIBS "sfml-graphics" "sfml-window" "sfml-system")
  else ()
    message("Couldn't find SFML library")
  endif (SFML_FOUND)
endif()

if(0)
  find_package(OpenCV REQUIRED)
  if (OpenCV_FOUND)
    list(APPEND EXTRA_LIBS ${OpenCV_LIBS})
    list(APPEND EXTRA_INCLUDES ${OpenCV_INCLUDE_DIRS})
    message("${PREFIX_MSG}-- ${ColourBold}${Esc}[${color_code}m${EXEC_NAME}${ColourReset} Opencv library Found")
  else ()
    message("${PREFIX_MSG}-- ${ColourBold}${Esc}[${color_code}m${EXEC_NAME}${ColourReset} Couldn't find Opencv library")
  endif (OpenCV_FOUND)
  endif()


