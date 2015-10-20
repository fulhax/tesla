function ( makelistabs FILELIST FILELISTNAME)

    foreach(RELFILE ${FILELIST})
        get_filename_component(ABSFILE ${RELFILE} ABSOLUTE)
        list(APPEND ABSLIST ${ABSFILE})
    endforeach()
    set(${FILELISTNAME} ${ABSLIST} PARENT_SCOPE)
    set(ABSLIST "")
endfunction()
