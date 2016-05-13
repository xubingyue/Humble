--[[
网络器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local humble = require("humble")
local utile = require("utile")
local tcp = require("tcp")
local httpd = require("httpd")
local table = table
local pairs = pairs
local SockType = SockType
local pEvBuffer = g_pEvBuffer

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

--初始化
function onStart()
    humble.tcpListen(1, "0.0.0.0", 15000)
    --humble.addTcpLink(1, "127.0.0.1", 15000)  
    
    humble.regTask("echo")
    humble.regTask("test")      
    
    tChan.echo = humble.getChan("echo")
end

--退出，主要清理掉连接
function onStop()
    humble.closeByType(1)
end

function onTcpLinked(sock, uiSession, usType)
    
end

function onTcpClose(sock, uiSession, usType)
    
end

local function funcOnRead(varMsg, sock, uiSession, pChan)
    pChan:Send(utile.Pack({sock, uiSession, varMsg}))
end

function onTcpRead(sock, uiSession, usType)
    --httpd.parsePack(pEvBuffer, funcOnRead, sock, uiSession, tChan.echo)
    tcp.parsePack(pEvBuffer, funcOnRead, sock, uiSession, tChan.echo)
end
