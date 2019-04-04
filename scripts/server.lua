require('scripts/helper')

cc.regmsg(20, "NetData.PlayerRegisterRequest")
--cc.regmsg(60003, "NetData.PlayerRegisterRequest")
cc.regmsg(60005, "NetData.Heartbeat")
local fd = cc.newserver("127.0.0.1", 12306, function(fd, entityid, msgid, msg) 
	cc.log_trace("receive message: " .. msgid)
end)
if fd ~= -1 then
	cc.log_trace("server on")
else
	cc.log_alarm("newserver error")
end
--cc.msgfunc(60005, function(fd, entityid, msgid, msg)
--	print("msgfunc: 600005\n")
--	cc.log_trace("receive 60005, fd: " .. fd  .. ", entityid: " .. entityid .. ", msgid: " .. msgid)
--end)

cc.msgfunc(60005, function(fd)
	cc.log_trace("msgfunc: 600005, fd: " .. fd)
end)
