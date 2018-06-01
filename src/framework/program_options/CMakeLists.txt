add_library(CmdError CmdError.cpp)

add_library(BaseCmdOpts BaseCmdOpts.cpp)
target_link_libraries(BaseCmdOpts
    PUBLIC ext_boost CmdError
    PRIVATE logging)
