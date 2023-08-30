# Read the input file into a variable


set(file_path ${PARAMETER1})
set(temp_dir  ${PARAMETER2})
message("${file_path}")
message("${temp_dir}")

file(READ "${file_path}" file_content)

# Split file content into lines
string(REGEX REPLACE "\n" ";" file_lines "${file_content}")

# Initialize a counter for renaming
set(counter 1)
# Create a new list to hold modified lines
set(new_file_lines "")
foreach(line ${file_lines})
    if(line MATCHES "^ADDLIB (.+)$")
        string(REGEX REPLACE "^ADDLIB (.+)$" "\\1" lib_file_path "${line}")
        
        # Get the file name without the directory path
        get_filename_component(file_name ${lib_file_path} NAME)
        
        # Copy the file to the temporary directory and rename it
        set(new_file_name "${counter}.a")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${lib_file_path} ${temp_dir}/${new_file_name}
        )
        
        # Replace the original line in the content with the new file path
        string(REGEX REPLACE "^ADDLIB (.+)$" "ADDLIB ${temp_dir}/${new_file_name}" line "${line}") 
        message(${line})
        # Increment the counter for the next file
        math(EXPR counter "${counter} + 1")
    endif()
    list(APPEND new_file_lines "${line}")
endforeach()

# Join the modified lines back into content
string(REPLACE ";" ";\n" modified_content "${new_file_lines}")

# Write the modified content back to the source file
file(WRITE "${file_path}" "${modified_content}")
