FILE(GLOB SRC *.cpp *.h)
SET(EXE_NAME einheri)

# Make beautiful folder in visual studio
GROUP_FILES()
FIND_PACKAGE(SFML COMPONENTS system network graphics audio )
FIND_PACKAGE(OpenGL)
INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIRS})

ADD_EXECUTABLE(${EXE_NAME} ${SRC})

TARGET_LINK_LIBRARIES(${EXE_NAME} einheri-client)
TARGET_LINK_LIBRARIES(${EXE_NAME} einheri-common)
TARGET_LINK_LIBRARIES(${EXE_NAME} ${SFML_LIBRARIES})
TARGET_LINK_LIBRARIES(${EXE_NAME} ${BULLET_LIBRARIES})

IF(APPLE)
	TARGET_LINK_LIBRARIES(${EXE_NAME} "-framework Accelerate")
ENDIF(APPLE)

INSTALL( TARGETS ${EXE_NAME} RUNTIME DESTINATION bin COMPONENT applications)
