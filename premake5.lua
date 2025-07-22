workspace "adm_projects"
	configurations { "Debug", "Release" }
	targetdir "bin/%{cfg.buildcfg}"
	include "src/adm_utils"
