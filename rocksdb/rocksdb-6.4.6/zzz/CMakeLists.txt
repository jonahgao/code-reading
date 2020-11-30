set(mytest_SRCS
    basic.cpp
    ingest.cpp
)

foreach(src ${mytest_SRCS})
    get_filename_component(exename ${src} NAME_WE)
    add_executable(${exename}${ARTIFACT_SUFFIX}
            ${src})
    target_link_libraries(${exename}${ARTIFACT_SUFFIX} ${LIBS})
    #list(APPEND mytest_deps ${exename})
endforeach()
