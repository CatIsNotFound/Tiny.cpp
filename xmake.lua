add_rules("mode.debug", "mode.release")
set_project("Tiny.cpp")
set_version("1.2.0")

set_languages("c++11")

option("build_test")
    set_default(true)
    set_showmenu(true)
    set_description("Build test module")
option_end()

option("build_demo")
    set_default(true)
    set_showmenu(true)
    set_description("Build demo module")
option_end()

option("include_docs")
    set_default(true)
    set_showmenu(true)
    set_description("Include docs directory when installing library")
option_end()

option("use_gpm")
    set_default(false)
    set_showmenu(true)
    set_description("Include gpm library for project (Only for linux).")
option_end()

-- Target for static library (Needed build this):
target("Tiny.cpp")
    set_kind("static")
    add_files("src/**.cpp")
    add_headerfiles("src/(**.hpp)")
    if is_plat("windows") then
        add_links("pdh", "advapi32", "ws2_32", "user32", "shell32")
    end
    on_load(function (target)
        if has_config("use_gpm") then
            if not is_plat("linux") then
                raise("You are not using Linux, please disable `use_gpm` config.")
            end
            import("lib.detect.find_library")
            local gpm_lib = find_library("libgpm")
            if gpm_lib then
                add_links("libgpm")
                add_defines("TINY_CPP_USE_GPM=1")
            else
                print("-- WARNING: You have not installed libgpm! It will be ignored!")
            end
        end
    end)
    if has_config("include_docs") then
        add_installfiles("docs/**", {
            prefixdir = "share/Tiny/docs"
        })
        add_installfiles("README*.md", {
            prefixdir = "share/Tiny"
        })
    end
    if has_config("build_test") then
        add_installfiles("test/assets/(**)", {
            prefixdir = "bin/assets"
        })
    end
target_end()

if has_config("build_test") then
    -- print("Building Test module...")
    add_requires("gtest 1.12.1")

    for _, name in ipairs(os.files("test/Test*.cpp|Test_*.cpp")) do
        local file = path.basename(name)
        target(file)
            set_kind("binary")
            add_packages("gtest")
            add_includedirs("src")
            add_deps("Tiny.cpp")
            set_default(true)
            add_files(name)
            add_tests(file)
        target_end()
    end

    for _, name in ipairs(os.files("test/Test_*.cpp")) do
        local file = path.basename(name)
        target(file)
            set_kind("binary")
            add_includedirs("src")
            add_deps("Tiny.cpp")
            set_default(true)
            add_files(name)
            add_tests(file)
        target_end()
    end
end

if has_config("build_demo") then
    -- print("Building Demo module...")
    for _, name in ipairs(os.files("demos/**.cpp")) do
        local file = path.basename(name)
        local dir = path.directory(name)
        local dir_name = path.basename(dir)
        target(dir_name)
            set_kind("binary")
            set_default(true)
            add_includedirs("src")
            add_deps("Tiny.cpp")
            add_files(name)
            add_tests(dir_name)
        target_end()
    end
end
