function setTarget( _configuration, _platform, _basepath )
	local strtarget = string.format( "%s/build/bin/%s_%s/", _basepath, _configuration, _platform )
	local strobj = string.format( "%s/build/intermediate/%s_%s", _basepath, _configuration, _platform )
	configuration {_configuration, _platform}
		targetdir( strtarget )
		objdir( strobj )
end


--------------------------------------------------------
solution "oldgames"
	configurations { "Debug", "Release" }

	configuration "macosx"
		platforms { "native" }
	configuration "windows"
		platforms { "x64", "native" }

    local bigfxpath = path.getabsolute("../../bigfx")
    local bgfxpath = bigfxpath .. "/3rdparty/bgfx"
    dofile(bigfxpath .. "/build/genie.lua")

    createBgfxProject(bgfxpath)
    createBigfxProject(bigfxpath)

	---------------------------------------------------------
	project "oldgames"
		kind "ConsoleApp"
		language "C++"
		
		local projpath = path.getabsolute("..")
		files {
            projpath .. "/src/**.h",
            projpath .. "/src/**.cpp",
            projpath .. "/data/**.*" }

		targetname "oldgames"

		defines { "_CRT_SECURE_NO_WARNINGS", "_WINDOWS", "JSMN_STATIC", "BX_CONFIG_DEBUG=1", "ENTRY_CONFIG_IMPLEMENT_MAIN=1" }
		flags { "NoPCH", "NoNativeWChar", "NoEditAndContinue", "StaticRuntime", "NoExceptions" }

		local bimgpath = bigfxpath .. "/3rdparty/bimg"
		local bxpath = bigfxpath .. "/3rdparty/bx"
		includedirs {
            bigfxpath .. "/src/",
            bigfxpath .. "/3rdparty",
            --bigfxpath .. "/3rdparty/SDL2-2.0.16/include",
            --bigfxpath .. "/3rdparty/glew-2.1.0/include",
            bigfxpath .. "/3rdparty/zlib-1.2.11",
            bigfxpath .. "/3rdparty/jsmn",
            --bigfxpath .. "/3rdparty/imgui",
            --bigfxpath .. "/3rdparty/eigen3",
			bgfxpath .. "/include",
            bgfxpath .. "/3rdparty",
            bgfxpath .. "/3rdparty/dear-imgui",
            bgfxpath .. "/examples/common",
            bxpath .. "/include",
            bxpath .. "/include/compat/msvc",
            bimgpath .. "/include",
            projpath .. "/data/shader" }

			configuration "windows"	
			links { "bigfx" }--, "glew32", "sdl2", "sdl2main", "opengl32"}

		configuration {"windows", "Debug"}
			links { "bgfxDebug", "bimgDebug", "bimg_encodeDebug", "bimg_decodeDebug", "bxDebug", "example-commonDebug", "example-glueDebug"}
		configuration {"windows", "Release"}
			links { "bgfxRelease", "bimgRelease", "bimg_encodeRelease", "bimg_decodeRelease", "bxRelease", "example-commonRelease", "example-glueRelease"}

		configuration "macosx"
			links { "bigfx" }--, "glew", "SDL2.framework", "OpenGL.framework" }

		configuration "windows"
            setTarget( "Debug", "x64", projpath )
            setTarget( "Release", "x64", projpath )
            setBigfxLibs( "Debug", "x64", bigfxpath)
            setBigfxLibs( "Release", "x64", bigfxpath)
			
		configuration "macosx"
            setTarget( "Debug", "native", projpath )
            setTarget( "Release", "native", projpath )
            setBigfxLibs( "Debug", "x64", bigfxpath)
            setBigfxLibs( "Release", "x64", bigfxpath)

		configuration "Debug"
			defines { "_DEBUG" }
			flags { "Symbols" } 
 
		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize", "Symbols"}

		configuration "macosx"
            linkoptions  { "-std=c++11" } 
            buildoptions { "-std=c++11" }
			

