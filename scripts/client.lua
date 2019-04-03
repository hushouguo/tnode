require('scripts/helper')

local fd = cc.newclient("127.0.0.1", 12306, function(fd, entityid, msgid, msg) end)
if fd ~= -1 then
	cc.log_trace("client on")
else
	cc.log_alarm("newclient error")
end

