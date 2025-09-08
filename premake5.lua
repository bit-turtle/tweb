-- premake5.lua
workspace "MyProject"
    configurations { "release" }
    startproject "tweb"

project "tweb"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    files { "src/*.cxx" }
    links { "ssl", "crypto" }
    defines { "_DEFAULT_SOURCE", "_XOPERN_SOURCE" }
