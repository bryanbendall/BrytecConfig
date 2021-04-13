workspace "BrytecConfig"
	architecture "x86"
	startproject "BrytecConfig"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["FontAwesome"] = "vendor/fontawesome"

-- Projects
group "Dependencies"
	include "BrytecConfig/vendor/GLFW"
	include "BrytecConfig/vendor/Glad"
	include "BrytecConfig/vendor/imgui"
	include "BrytecConfig/vendor/FontAwesome"
group ""

include "BrytecConfig"