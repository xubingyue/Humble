--[[
定时器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
local humble = require("humble")
local ChanNam = ChanNam

if not g_tChan then
    g_tChan = {}
    g_tChan.timer = humble.regSendChan(ChanNam.Timer, "tick")
end
local tChan = g_tChan

function onTimer(uiTick, uiCount)
    tChan.timer:Send(serialize.pack({uiTick, uiCount}))
end
