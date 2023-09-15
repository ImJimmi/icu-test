cmake_minimum_required(VERSION 3.25)
project(localisation)

set(ICU_LIB_DIR ${CMAKE_SOURCE_DIR}/dependencies/icu/lib)

add_library(IcuData STATIC IMPORTED)
add_library(IcuI18n STATIC IMPORTED)
add_library(IcuIO STATIC IMPORTED)
add_library(IcuTU STATIC IMPORTED)
add_library(IcuUC STATIC IMPORTED)

if (APPLE)
    set(ICU_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/icu/include-macOS)

    set_target_properties(IcuData PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/libicudata.a
    )
    set_target_properties(IcuI18n PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/libicui18n.a
    )
    set_target_properties(IcuIO PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/libicuio.a
    )
    set_target_properties(IcuTU PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/libicutu.a
    )
    set_target_properties(IcuUC PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/libicuuc.a
    )
elseif(WIN32)
    set(ICU_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/icu/include-Win32)

    set_target_properties(IcuData PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/icudt.lib
    )
    set_target_properties(IcuI18n PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/icuin.lib
    )
    set_target_properties(IcuIO PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/icuio.lib
    )
    set_target_properties(IcuTU PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/icutu.lib
    )
    set_target_properties(IcuUC PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ICU_INCLUDE_DIR}
        IMPORTED_LOCATION ${ICU_LIB_DIR}/icuuc.lib
    )
endif()

add_library(localisation INTERFACE IMPORTED)
set_property(TARGET localisation PROPERTY
    INTERFACE_LINK_LIBRARIES IcuData IcuI18n IcuIO IcuTU IcuUC
)

set(U_STATIC_IMPLEMENTATION ON)

add_executable(foo
    main.cpp
)

target_link_libraries(foo
PUBLIC
    localisation
)

target_compile_features(foo
PUBLIC
    cxx_std_17
)
