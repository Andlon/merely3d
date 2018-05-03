
# Loads all shaders stored in .glsl files in the given shader_dir,
# embeds them into a header file configured with shader_header_src
# and stores the resulting configured header in shader_configured_path.
function(embed_shaders shader_dir shader_header_src shader_configured_path)
    file(GLOB shader_files ${shader_dir}/*.glsl)
    set(shaders_cpp_source "")
    foreach(shader_file ${shader_files})
        get_filename_component(shader_name "${shader_file}" NAME_WE)
        file(READ "${shader_file}" shader_body)
        string(CONCAT shaders_cpp_source
                "${shaders_cpp_source}"
                "\nconst char * const ${shader_name} = R\"~~~(\n" "${shader_body}" "\n)~~~\";\n" )
    endforeach()

    configure_file("${shader_header_src}" "${shader_configured_path}")
endfunction()
