add_rules("mode.debug", "mode.release")
set_project("BoxUtils")
set_version("0.1")
if is_plat("windows") then
	-- Win32的储存库
	--print("不是Linux平台 coroutine被禁用")
	add_links("ws2_32")
	--add_requires("libiconv")
	--Windows上好像没有iconv包
	add_defines("BOX_NICONV")
	--WinSocket
elseif is_plat("mingw") then
	--Mingw编译
	add_links("ws2_32","iconv","pthread")
elseif is_plat("linux") then
	add_links("pthread","dl")
elseif is_plat("macosx") then
	add_requires("libiconv")
end
add_requires("libxml-2.0","libcurl")
add_cxxflags("-std=c++17")
-- 库
target("box_utils")
	--add_links("curl","xml2")
	add_includedirs("./include")
	add_includedirs("./include/Box")
	--设置种类
	--在加载的时候加入依赖
	set_kind("shared")
	--add_links("xml2")
	--add_links("curl")
	-- add files
	on_load(function(target)
		target:add(find_packages("libxml-2.0","libcurl"))
	end)
	if not is_mode("debug") then
		add_defines("NDEBUG")
	end
	--检查LIBXML2
	if has_package("libxml-2.0") then
		--LIBXML2
		add_files("./src/xpath.cpp")
		add_files("./src/xml.cpp")
	end
	
	if has_package("libcurl") then
		--CURL网络部分
		add_files("./src/curl/*.cpp")
	end
	--Socket
	add_files("./src/net/*.cpp")
	add_files("./src/net.cpp")
	--OS API
	add_files("./src/os/*.cpp")
	
	add_files("./src/table.cpp")

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
	add_files("./src/dylib.cpp")
	
	add_files("./src/posix/*.cpp")
	
	--add_files("./src/channal.cpp")
	add_files("./src/backtrace.cpp")
	add_files("./src/logger.cpp")
	add_files("./src/buffer.cpp")
	add_files("./src/assert.cpp")

	add_files("./src/co/*.cpp")
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
target("box_spider")
	set_kind("shared")
	add_includedirs("./include/Box")
	add_files("./spider/*.cpp")
	add_deps("box_utils")

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
