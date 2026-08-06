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
    add_headerfiles("src/(**.hpp)", {
        prefixdir = "Tiny",
        install   = true,
        public    = true
    })
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
        add_installfiles("$(projectdir)/docs/(**.md)", {
            prefixdir = "share/Tiny/docs"
        })
        add_installfiles("$(projectdir)/README*.md", {
            prefixdir = "share/Tiny"
        })
        add_installfiles("$(projectdir)/LICENSE", {
            prefixdir = "share/Tiny"
        })
    end
    if has_config("build_test") then
        add_installfiles("$(projectdir)/test/assets/(**)", {
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

package("Tiny.cpp")
    set_homepage("https://github.com/CatIsNotFound/Tiny.cpp/")
    set_description("An extremely small, lightweight, and easy-to-use foundational library.")
    set_license("MIT")
    set_urls("https://github.com/CatIsNotFound/Tiny.cpp.git")

    add_configs("build_test", {description = "Build test module", default = false, type = "boolean"})
    add_configs("build_demo", {description = "Build demo module", default = false, type = "boolean"})
    add_configs("include_docs", {description = "Install docs", default = false, type = "boolean"})
    add_configs("use_gpm", {description = "Enable libgpm (Linux only) ", default = false, type = "boolean"})

    on_load(function (package)
        if package:is_plat("windows") then
            package:add("syslinks", "pdh", "advapi32", "ws2_32", "user32", "shell32")
        else
            package:add("syslinks")
        end

        if package:config("use_gpm") and package:is_plat("linux") then
            package:add("gpm")
        end
    end)

    on_install(function (package)
        import("package.tools.xmake").install(package, {
            configs = {
                build_test = package:config("build_test"),
                build_demo = package:config("build_demo"),
                include_docs = package:config("include_docs"),
                use_gpm = package:config("use_gpm")
            }
        })
    end)
package_end()
