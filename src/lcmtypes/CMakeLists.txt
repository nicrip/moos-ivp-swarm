# Set System Specific Libraries
if (${WIN32})
  # Windows Libraries
  SET(SYSTEM_LIBS
     wsock32 )
else (${WIN32})
  # Linux and Apple Libraries
  SET(SYSTEM_LIBS
      m
      pthread )
endif (${WIN32})

execute_process(COMMAND lcm-gen -x ${PROJECT_SOURCE_DIR}/src/lcmtypes/node_in_t.lcm ${PROJECT_SOURCE_DIR}/src/lcmtypes/node_out_t.lcm --cpp-hpath ${PROJECT_SOURCE_DIR}/include)
