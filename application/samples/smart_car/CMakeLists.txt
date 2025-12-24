    set(SOURCES_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/motor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/track.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bluetooth.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/led.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/buzzer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/joy.c
    )

    set(PUBLIC_HEADER_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    set(SOURCES "${SOURCES_LIST}" PARENT_SCOPE)
    set(PUBLIC_HEADER "${PUBLIC_HEADER_LIST}" PARENT_SCOPE)

    add_compile_definitions(CONFIG_PWM_USING_V150)