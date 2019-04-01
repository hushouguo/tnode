
function print_r ( t )  
    local print_r_cache={}
    local function sub_print_r(t,indent)
        if (print_r_cache[tostring(t)]) then
            cc.log_trace(indent.."*"..tostring(t))
        else
            print_r_cache[tostring(t)]=true
            if (type(t)=="table") then
                for pos,val in pairs(t) do
                    if (type(val)=="table") then
                        cc.log_trace(indent.."["..pos.."] => "..tostring(t).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        cc.log_trace(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val)=="string") then
                        cc.log_trace(indent.."["..pos..'] => "'..val..'"')
                    else
                        cc.log_trace(indent.."["..pos.."] => "..tostring(val))
                    end
                end
            else
                cc.log_trace(indent..tostring(t))
            end
        end
    end
    if (type(t)=="table") then
        cc.log_trace(tostring(t).." {")
        sub_print_r(t,"  ")
        cc.log_trace("}")
    else
        sub_print_r(t,"  ")
    end
    --print()
end

cc.log_trace("login service start")
cc.log_debug("this is debug log")
cc.log_error("this is error log")
cc.log_alarm("this is alarm log")
cc.log_trace("timestamp: " .. cc.timestamp())
cc.log_trace("{a=1, [2]=2, name='hushouguo'}")
local o = {a=1, [2]=2, name='hushouguo'}
local jsonstr = cc.json_encode(o);
cc.log_trace(jsonstr)
local oo = cc.json_decode(jsonstr);
print_r(oo)
cc.log_trace(oo.a)
cc.log_trace(oo.name)
cc.log_trace(oo["2"])

cc.log_trace("hash_string('hushouguo'): " .. cc.hash_string('hushouguo'))
cc.log_trace("random: " .. cc.random())
cc.log_trace("random_between(1, 100): " .. cc.random_between(1, 100))
cc.log_trace("base64")
local rawstring = "hushouguo";
local encodedstring = cc.base64_encode(rawstring)
cc.log_trace("raw: " .. rawstring .. ", encodestring: " .. encodedstring .. ", decode: " .. cc.base64_decode(encodedstring))
local digest = cc.md5(rawstring)
print_r(digest)
cc.log_trace("msleep")
cc.msleep(1000)
cc.log_trace("wakeup")
cc.log_trace("second: " .. cc.timesec() .. ", millisecond: " .. cc.timemsec())

--cc.listen(function(fd) 
--	cc.accept(fd);
--	cc.close(fd);
--end)

--cc.dispatch(function(source, fd, msgid, data, len)
--end)

cc.dispatch(msgid, function(source, fd, data, len)
end)

