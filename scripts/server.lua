require('scripts/helper')

cc.regmsg(20, "NetData.PlayerRegisterRequest")
--cc.regmsg(60003, "NetData.PlayerRegisterRequest")
cc.regmsg(60005, "NetData.Heartbeat")
local fd = cc.newserver("127.0.0.1", 12306)
if fd ~= -1 then
	cc.log_trace("server on")
else
	cc.log_alarm("newserver error")
end
function msgParser(fd, entityid, msgid, o)
	cc.log_trace("fd: " .. fd .. ", entityid: " .. entityid .. ", msgid: " .. msgid)
	dump(o)
end
