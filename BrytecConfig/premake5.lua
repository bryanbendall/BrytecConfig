project "BrytecConfig"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	removefiles
	{
	}

	includedirs
	{
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.FontAwesome}",
		"%{IncludeDir.imnodes}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.nativefiledialog}",
	}

	links
	{
        "GLFW",
		"Glad",
		"ImGui",
		"imnodes",
		"yaml-cpp",
		"nativefiledialog"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLCORE_PLATFORM_WINDOWS"
		}

	filter "system:linux"
		links
		{
			"Xrandr",
			"Xi",
			--"GLU",
			"GL",
			"X11",
			"dl",
			"pthread",
			"stdc++fs",	--GCC versions 5.3 through 8.x need stdc++fs for std::filesystem
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
        optimize "on"
