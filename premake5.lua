newaction {
	trigger = "clean",
	description = "clean project",
	execute = function()		
		print("clean the build...")
		os.rmdir(os.realpath(".") .. "\\bin")
		os.rmdir(os.realpath(".") .. "\\obj")
		os.rmdir(os.realpath(".") .. "\\project")

		print("done.")
	end
}

workspace "Dagger"
	configurations { "Debug", "Release" }
	platforms { "x64" }

project "Dagger"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	location "project"

	objdir "obj/%{cfg.buildcfg}"
	targetdir "bin/%{cfg.buildcfg}"

	files { "include/**.h", "src/**.cpp" }

	includedirs { 
		"include", 
		"include/**", 
		"tools/vcpkg/installed/x64-windows/include",
		"tools/vcpkg/installed/x64-windows-static/include",
		"tools/vcpkg/installed/arm64-osx/include"
	}

	libdirs {
		"tools/vcpkg/installed/x64-windows/lib/",
		"tools/vcpkg/installed/x64-windows-static/lib/",
		"tools/vcpkg/installed/arm64-osx/lib/"
	}

	vpaths {
	    ["Headers"] = { "**.h", "**.hpp" },
	    ["Sources"] = { "**.c", "**.cpp"},
	    ["Docs"] = "**.md"
	}

	flags { "NoPCH" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		debugdir "bin/%{cfg.buildcfg}"

		postbuildcommands {
			"{COPYFILE} ../dagger.ini ../bin/Debug/",
			"{COPY} ../data ../bin/Debug/data/"
		}

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

		postbuildcommands {
			"{COPYFILE} ../dagger.ini ../bin/Release/",
			"{COPY} ../data ../bin/Release/data/"
		}

	filter {}

	-- linker here
	links { 
		"imgui",
		"implot" 
	}
