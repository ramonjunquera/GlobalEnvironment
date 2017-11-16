--Doit WiFi Robo Car Ctronl Demo
--ap mode
--Created @ 2015-05-13 by Doit Studio
--Modified: null
--Global Site:	http://doit.am/
--China Site:	http://cn.doit.am/
--Global Shop:	http://www.smartarduino.com/
--China Shop:	http://szdoit.taobao.com/
--Chinese BBS: 	bbs.iot.fm

print("\n")
print("ESP8266 Started")

local exefile="webserver"
local luaFile = {exefile..".lua"}
for i, f in ipairs(luaFile) do
	if file.open(f) then
      file.close()
      print("Compile File:"..f)
      node.compile(f)
	  print("Remove File:"..f)
      file.remove(f)
	end
 end

if file.open(exefile..".lc") then
	dofile(exefile..".lc")
else
	print(exefile..".lc not exist")
end
exefile=nil;luaFile = nil
collectgarbage()