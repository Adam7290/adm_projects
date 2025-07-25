os.execute("cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DGLFW_BUILD_TESTS=0 -DGLFW_BUILD_EXAMPLES=0 ./glfw && cmake --build ./glfw")

externalproject "adm_glfw"
	language "C"
	kind "StaticLib"
	location "glfw/src"	
	usage "INTERFACE"
		links { "glfw3" }
		includedirs { "glfw/include" }
