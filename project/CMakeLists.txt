set(PROJECT_SOURCE
	source/ApplicationRC.cpp
	source/ApplicationDX.cpp
	source/main.cpp
)

set(PROJECT_HEADER
	source/ApplicationRC.h
	source/ApplicationDX.h
)



add_executable(project ${PROJECT_SOURCE} ${PROJECT_HEADER})

target_compile_features(project PRIVATE cxx_std_17)

target_include_directories(project SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../engine"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
 LINK_FLAGS "/SUBSYSTEM:WINDOWS"
 VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_link_libraries(project PUBLIC engine)

