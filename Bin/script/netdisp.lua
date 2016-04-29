--[[
网络、定时器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
local humble = require("humble")
local table = table
local pNet = g_pNetWorker--g_pNetWorker的c++函数只能在该模块使用
local ChanNam = ChanNam
local SockType = SockType

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--初始化
function onStart()
    --net 初始化
    --pNet:tcpListen(ustype, strhost, usport)
    --pNet:addTcpLink(ustype, strhost, usport)
    
    tChan.Close = humble.regRecvChan(ChanNam.CloseSock, "netdisp", 1024)
    tChan.testTimer = humble.regSendChan("testtimer", "netdisp", 10)
    
    --
    humble.regTask("test")
end

--退出，主要清理掉连接
function onStop()
    print("net onStop")
end

function onTimer(uiTick, uiCount)
    if tChan.testTimer:canSend() then
        tChan.testTimer:Send(serialize.pack({uiTick, uiCount}))
    end
    
    if tChan.Close:canRecv() then
        local tval = serialize.unpack(tChan.Close:Recv())
        table.print(tval)
        pNet:closeSock(table.unpack(tval))
    end
end

function onTcpLinked(pSession)
    
end

function onTcpClose(pSession)
    
end

function onTcpRead(pSession)
    
end
