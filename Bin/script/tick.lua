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
    tChan.timer = humble.getChan("test")
end

function onStop()
    
end

function onTimer(uiTick, uiCount)
    tChan.timer:Send(utile.Pack({uiTick, uiCount}))
    
    --1秒
    if 0 == ((uiTick * uiCount) % 1000) then 
        print("1 sec")
        print(string.format("cur load %d.", humble.getCurLoad()))
    end
end
