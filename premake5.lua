workspace "adm_projects"
	configurations { "debug", "release" }
	objdir "build/%{cfg.buildcfg}/obj"
	targetdir "build/%{cfg.buildcfg}/bin"

	include "adm_utils"
	include "adm_framework"

	filter { "configurations:debug" }
		symbols "On"
	filter { "configurations:release" }
		optimize "On"
	filter { }
