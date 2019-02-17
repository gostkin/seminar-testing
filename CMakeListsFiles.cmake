#
# Copyright (c) 2019 Eugene Gostkin
# Distributed under Apache 2.0 license (see LICENSE FILE)
#

function(configure_include_dirs)
    target_include_directories(AppLib PUBLIC src/lib deps/gtest/googletest/include)
    target_include_directories(App PUBLIC src/main)

    target_include_directories(GoogleTestLib PUBLIC
            deps/gtest/googletest deps/gtest/googletest/include)
    target_include_directories(Tests PUBLIC src/test)
endfunction()

get_filename_component(STDAFX_FULL_PATH "src/lib/StdAfx.hpp" ABSOLUTE)
get_filename_component(STDAFX_MAIN_FULL_PATH "src/main/StdAfxMain.hpp" ABSOLUTE)

set(APPLIB_SOURCE_FILES
        src/lib/Stack.hpp
        src/lib/StdAfx.cpp
        src/lib/StdAfx.hpp
        )

set(APP_SOURCE_FILES
        src/main/main.cpp
        src/main/StdAfxMain.cpp
        src/main/StdAfxMain.hpp
        )

set(TESTS_SOURCE_FILES
        src/test/TestMain.cpp
        src/test/TestStack.cpp
        src/test/StdAfxTest.cpp
        src/test/StdAfxTest.hpp
        )

set(GOOGLETESTLIB_SOURCE_FILES
        deps/gtest/googletest/src/gtest-all.cc
        )