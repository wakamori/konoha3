FIND_PACKAGE_EXTRA_LIBRARY(curl)
set(PACKAGE_SOURCE_CODE Exception_glue.c)
set(PACKAGE_SCRIPT_CODE Exception_glue.k)
add_konoha_package(MiniKonoha.Exception)
