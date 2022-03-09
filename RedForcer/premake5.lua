project "RedForcer"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "RF_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "RF_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines "RF_RELEASE"
        runtime "Release"
        optimize "on"
    