--[[
定时器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local humble = require("humble")
local utile = require("utile")
local LogLV = LogLV

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

function onStart()
    tChan.timer = humble.getSendChan("timetick", "tick")
end

function onStop()
    
end

function onTimer(uiTick, uiCount) 
    if tChan.timer:canSend() then
        tChan.timer:Send(utile.Pack({uiTick, uiCount}))
    else
        utile.Log(LogLV.Warn, "%s", "service maybe overloaded.")
    end
    
    --1秒
    if 0 == ((uiTick * uiCount) % 1000) then 
        print("1 sec")
    end
end
