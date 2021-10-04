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
IncludeDir["GLFW"] = "vendor/glfw/include"
IncludeDir["Glad"] = "vendor/glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["imnodes"] = "vendor/imnodes"
IncludeDir["FontAwesome"] = "vendor/fontawesome"
IncludeDir["yaml_cpp"] = "vendor/yaml-cpp/include"
IncludeDir["nativefiledialog"] = "vendor/nativefiledialog/src/include"

-- Projects
group "Dependencies"
	include "BrytecConfig/vendor/glfw"
	include "BrytecConfig/vendor/glad"
	include "BrytecConfig/vendor/imgui"
	include "BrytecConfig/vendor/imnodes"
	include "BrytecConfig/vendor/FontAwesome"
	include "BrytecConfig/vendor/yaml-cpp"
	include "BrytecConfig/vendor/nativefiledialog"
group ""

include "BrytecConfig"