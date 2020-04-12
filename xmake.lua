add_rules("mode.debug", "mode.release")
set_project("BoxUtils")
set_version("0.1")
if is_plat("windows") then
	-- Win32的储存库
	cprint("${red}不是Linux平台 coroutine被禁用")
	add_cxxflags("-lws2_32")
	--WinSocket
	add_requires("xml2","curl",{optional = true})
else
	add_includedirs("/usr/include/libxml2")
end
-- define target
target("box_utils")
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
	add_files("cJSON.c")
	add_files("socket.cpp")
	add_files("string.cpp")
	
	add_files("xv.cpp")
	add_files("exception.cpp")
	add_files("dns.cpp")
	
	add_files("os.cpp")
	add_files("random.cpp")
	add_files("filesystem.cpp")
	add_files("sem.cpp")
	
	add_files("time.cpp")
	
	add_files("base64.cpp")
	add_files("json.cpp")
	add_files("iconv.cpp")
	add_files("dylib.cpp")
	--CURL网络部分
	add_files("net*.cpp")
	--LIBXML2
	add_files("xpath.cpp")
	add_files("xml.cpp")
	add_files("channal.cpp")
	add_files("backtrace.cpp")
	if is_plat("linux") then
		--Linux携程
		add_files("coroutine.cpp")
		add_files("scheduler.cpp")
	end
	if not is_mode("debug") then
		add_defines("NDEBUG")
	end


