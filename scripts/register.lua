require('scripts/helper')
cc.log_trace("service: " .. _G["cc.service"])
cc.regmsg(20, "NetData.PlayerRegisterRequest")
--cc.regmsg(60003, "NetData.PlayerRegisterRequest")
cc.regmsg(60005, "NetData.Heartbeat")
function msgParser(fd, entityid, msgid, o)
	cc.log_trace("service: " .. _G["cc.service"] .. ", fd: " .. fd .. ", entityid: " .. entityid .. ", msgid: " .. msgid)
	dump(o)
end
