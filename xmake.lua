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
	add_requires("libxml2")
	--WinSocket
elseif is_plat("mingw") then
	--Mingw编译
	add_links("ws2_32","iconv","pthread")
elseif is_plat("linux") then
	add_links("pthread","dl")
elseif is_plat("macosx") then
	add_requires("libiconv")
end
add_requires("libcurl")
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
		target:add(find_packages("libxml-2.0","libcurl","libxml2"))
	end)
	if not is_mode("debug") then
		add_defines("NDEBUG")
	end
	--检查LIBXML2
	if has_package("libxml-2.0") or has_package("libxml2") then
		--LIBXML2
		add_files("./src/lxml/*.cpp")
		add_links("xml2")
	end
	if has_package("libcurl") then
		--CURL网络部分
		add_files("./src/curl/*.cpp")
		add_links("curl")
	end
	-- Add thirdparty
	add_files("./src/thirdparty/*.c")
	--Common files
	add_files("./src/*.cpp")
	--Crypto
	add_files("./src/crypto/*.cpp")
	if is_plat("linux") or is_plat("macos") then
		add_files("./src/posix/*.cpp")
	end

	add_files("./src/os/*.cpp")
	add_files("./src/net/*.cpp")
	add_files("./src/sync/*.cpp")
	add_files("./src/utils/*.cpp")
	add_files("./src/thread/*.cpp")
	add_files("./src/libc/*.c")
if is_mode("debug") then
	--debug
	target("json")
		add_files("./tests/json.cpp")
		add_deps("box_utils")
		add_includedirs("./include")
end