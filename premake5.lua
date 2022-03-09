workspace "RedForcer"
    architecture "x64"
    startproject "RedForcer"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "RedForcer"
