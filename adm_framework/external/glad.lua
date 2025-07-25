project "adm_glad"
	language "C"
	kind "StaticLib"
	files { "src/glad.c" }
	usage "INTERFACE"
		links { "adm_glad" }
		includedirs { "include" }
