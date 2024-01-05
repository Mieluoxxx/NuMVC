add_rules("mode.debug", "mode.release")
add_includedirs("include")
set_languages("c++17")
-- 定义宏 PLAT_IS_WIN 或 PLAT_IS_LINUX
add_defines("PLAT_IS_$(plat:upper)")
includes("src/utils", "src/numvc")

target("numvc")
    set_kind("binary")
    add_files("*.cpp")