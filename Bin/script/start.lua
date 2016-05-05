--[[
网络器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
local humble = require("humble")
local table = table
local SockType = SockType
local pTcpBuffer = g_pTcpBuffer

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--初始化
function onStart()
    humble.tcpListen(1, "0.0.0.0", 15000)
    --humble.addTcpLink(1, "127.0.0.1", 15000)  
    
    humble.regChan("echochan", 1024)
    humble.regChan("timetick", 1024)
    
    humble.regTask("echo")
    humble.regTask("test")      
    
    tChan.echo = humble.getSendChan("echochan", "start")
end

--退出，主要清理掉连接
function onStop()
    humble.closeByType(1)
end

function onTcpLinked(sock, uiSession)
    
end

function onTcpClose(sock, uiSession)
    
end

function onTcpRead(sock, uiSession)
    local iLens = pTcpBuffer:getTotalLens()
    local objBinary = pTcpBuffer:readBuffer(iLens)
    local strMsg = objBinary:getByte(objBinary:getSurpLens())
    --if tChan.echo:canSend() then
        tChan.echo:Send(serialize.pack({sock, uiSession, strMsg}))
    --end
    pTcpBuffer:delBuffer(iLens)
end
