project "adm_utils"
	language "C"
	kind "StaticLib"
	files { "*.c" }
	usage "INTERFACE"
		links { "adm_utils", "m" }
		includedirs { "../" }

project "adm_utils_sandbox"
	language "C"
	kind "ConsoleApp"
	files { "sandbox/*.c" }
	uses { "adm_utils" }
