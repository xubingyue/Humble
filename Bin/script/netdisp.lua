--[[
网络器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
local humble = require("humble")
local table = table
local ChanNam = ChanNam
local SockType = SockType
local pTcpBuffer = g_pTcpBuffer

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--初始化
function onStart()   
    tChan.echo = humble.regSendChan("echochan", "netdisp")
    
    humble.tcpListen(1, "0.0.0.0", 15000)
    --humble.addTcpLink(1, "127.0.0.1", 15000)    --
    
    humble.regTask("echo")
    humble.regTask("test")
end

--退出，主要清理掉连接
function onStop()
    print("net onStop")
end

function onTcpLinked(pSession)
    print("onTcpLinked")
end

function onTcpClose(pSession)
    print("onTcpClose")
end

function onTcpRead(pSession)
    local iLens = pTcpBuffer:getTotalLens()
    local objBinary = pTcpBuffer:readBuffer(iLens)
    local strMsg = objBinary:getByte(objBinary:getSurpLens())
    tChan.echo:Send(serialize.pack({pSession.sock, pSession.session, strMsg}))
    pTcpBuffer:delBuffer(iLens)
end
