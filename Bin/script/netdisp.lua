--[[
网络、定时器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
local humble = require("humble")
local table = table
local pnetworker = g_pNetWorker
local channam = channam
local socktype = socktype

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

function onstart()
    --net 初始化
    --pnetworker:tcpListen(ustype, strhost, usport)
    --pnetworker:addTcpLink(ustype, strhost, usport)
    
    tChan.close = humble.regrecvchan(channam.closesock, "netdisp", 0)
    tChan.timer = humble.regsendchan(channam.timer, "netdisp", 0)
end

function ontcplinked(pSession)
    
end

function ontcpclose(pSession)
    
end

function ontcpread(pSession)
    
end

function ontimer(uiTick, uiCount)
    tChan.timer:Send(serialize.pack({uiTick, uiCount}))
    
    if tChan.close:canRecv() then
        pnetworker:closeSock(table.unpack(serialize.unpack(tChan.close:Recv())))
    end
end
