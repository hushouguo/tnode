require('scripts/helper')

local fd = cc.newserver("127.0.0.1", 12306, function(fd, entityid, msgid, msg) end)
if fd ~= -1 then
	cc.log_trace("server on")
else
	cc.log_alarm("newserver error")
end

