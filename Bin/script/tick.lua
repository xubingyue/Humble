--[[
定时器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
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
        tChan.timer:Send(serialize.pack({uiTick, uiCount}))
    else
        utile.Log(LogLV.Warn, "%s", "service maybe overloaded.")
    end
end
