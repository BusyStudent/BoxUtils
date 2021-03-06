add_rules("mode.debug", "mode.release")
set_project("BoxUtils")
set_version("0.1")
if is_plat("windows") then
	-- Win32的储存库
	cprint("${red}不是Linux平台 coroutine被禁用")
	add_links("ws2_32")
	--WinSocket
	add_requires("xml2","curl",{optional = true})
else
	add_includedirs("/usr/include/libxml2")
end
-- 库
target("box_utils")
	add_cxxflags("-std=c++17")
	--add_links("curl","xml2")
	add_includedirs("./include")
	add_includedirs("./include/Box")
	--设置种类
	--在加载的时候加入依赖
	on_load(function (target)
		--CURL的网络部分
		target:add(find_packages("curl","xml2"))
	end)
	set_kind("shared")
	--add_links("xml2")
	--add_links("curl")
	-- add files
	add_files("./src/cJSON.c")
	add_files("./src/cJSON_Utils.c")
	add_files("./src/socket.cpp")
	add_files("./src/string.cpp")
	
	--add_files("./src/xv.cpp")
	add_files("./src/exception.cpp")
	add_files("./src/dns.cpp")
	
	add_files("./src/os.cpp")
	add_files("./src/random.cpp")
	add_files("./src/filesystem.cpp")
	--add_files("./src/sem.cpp")
	add_files("./src/sync/*.cpp")

	add_files("./src/timer.cpp")
	add_files("./src/threadpool.cpp")

	add_files("./src/base64.cpp")
	add_files("./src/json.cpp")
	add_files("./src/iconv.cpp")
	add_files("./src/dylib.cpp")
	--CURL网络部分
	add_files("./src/net/*.cpp")
	add_files("./src/net.cpp")
	--LIBXML2
	add_files("./src/xpath.cpp")
	add_files("./src/xml.cpp")
	
	--add_files("./src/channal.cpp")
	add_files("./src/backtrace.cpp")
	add_files("./src/logger.cpp")
	add_files("./src/buffer.cpp")
	add_files("./src/assert.cpp")

	if not is_mode("debug") then
		add_defines("NDEBUG")
	end
	--C组件
	add_files("./src/libc/*.c")
	--Fmt
	add_files("./src/fmt.cpp")
--SDL部分
-- target("box_sdl")
-- 	set_kind("shared")
-- 	add_cxxflags("-std=c++17")
-- 	on_load(function (target)
-- 		target:add(find_packages("SDL2","SDL2_mixer","SDL2_ttf","SDL2_image"))
-- 	end)
-- 	add_files("./src/sdl/*.cpp")
--测试
target("test_buffer")
	set_kind("binary")
	add_includedirs("./include")
	add_files("./tests/test_buffer.cpp")
	add_deps("box_utils")
	add_includedirs("./src")
target("thpool")
	--thread pool
	set_kind("binary")
	add_includedirs("./include")
	add_files("./tests/thpool_test.cpp")
	add_deps("box_utils")
	add_includedirs("./src")
