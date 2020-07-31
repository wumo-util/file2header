function(res name isStatic namespace srcDir outputDir pattern)
  set(file2header ${CONAN_BIN_DIRS_FILE2HEADER}/file2header)
  file(MAKE_DIRECTORY "${outputDir}")
  file(GLOB_RECURSE resources "${srcDir}/${pattern}")
  set(outputFiles)
  foreach(file ${resources})
    get_filename_component(fileName ${file} NAME)
    get_filename_component(ext ${file} LAST_EXT)
    get_filename_component(dir ${file} DIRECTORY)
    file(RELATIVE_PATH relativeFile "${srcDir}" "${file}")
    file(RELATIVE_PATH relativeDir "${srcDir}" "${dir}")
    string(REGEX REPLACE "[^a-zA-Z0-9]" "_" cxxName ${fileName})
    set(outptuFile ${outputDir}/${relativeDir})
    set(outputFiles ${outputFiles} ${outptuFile}/${cxxName}.cpp)
    
    add_custom_command(OUTPUT ${outptuFile}/${cxxName}.hpp ${outptuFile}/${cxxName}.cpp
      COMMAND ${CMAKE_COMMAND} -E make_directory "${outputDir}/${relativeDir}"
      COMMAND ${file2header} -i ${file} -o ${outptuFile} -n ${cxxName} -s ${namespace} -r ${relativeDir}
      DEPENDS ${file})
  endforeach()
  if(${isStatic})
    add_library(${name} STATIC ${outputFiles})
  else()
    add_library(${name} SHARED ${outputFiles})
    target_compile_definitions(${name} PUBLIC "FILE2HEADER_BUILD_SHARED_LIBRARY")
  endif()
  
  target_include_directories(${name} INTERFACE ${outputDir})
endfunction()

function(static_res target srcDir outputDir pattern)
  res(${target}_resources True "" ${srcDir} ${outputDir} ${pattern})
  target_link_libraries(${target} PUBLIC ${target}_resources)
endfunction()

function(shared_res target srcDir outputDir pattern)
  res(${target}_resources False "" ${srcDir} ${outputDir} ${pattern})
  target_link_libraries(${target} PUBLIC ${target}_resources)
endfunction()

function(static_res_ns target namespace srcDir outputDir pattern)
  res(${target}_resources True ${namespace} ${srcDir} ${outputDir} ${pattern})
  target_link_libraries(${target} PUBLIC ${target}_resources)
endfunction()

function(shared_res_ns target namespace srcDir outputDir pattern)
  res(${target}_resources False ${namespace} ${srcDir} ${outputDir} ${pattern})
  target_link_libraries(${target} PUBLIC ${target}_resources)
endfunction()