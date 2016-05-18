--[[
网络器服务
--]]

local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local humble = require("humble")
local utile = require("utile")
local table = table
local pairs = pairs
local pBuffer = g_pBuffer

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

if not g_tListener then
    g_tListener = {}
end
local tListener = g_tListener

if not g_tLinker then
    g_tLinker = {}
end
local tLinker = g_tLinker

--初始化
function onStart()
    tListener.test = humble.addListener(1, "0.0.0.0", 15000)
    --tLinker.test = humble.addTcpLink(1, "127.0.0.1", 15000)  
    humble.setParser(1, "tcp1")
    
    humble.regTask("echo")
    humble.regTask("test")      
    
    tChan.echo = humble.getChan("echo")
end

local iTime = 0
local iCount = 0

--退出，主要清理掉连接
function onStop()
    humble.closeByType(1)
    humble.delListener(tListener.test)
    
    print("lua"..iTime)
    print("lua"..iCount)
end

function onTcpLinked(sock, uiSession, usSockType)
    
end

function onTcpClose(sock, uiSession, usSockType)
    
end

function onTcpRead(sock, uiSession, usSockType)
    local a = os.clock()
    local strBuf = pBuffer:getByte(pBuffer:getSurpLens())
    
    local strVal = utile.Pack({sock, uiSession, strBuf})
    local b = os.clock()
    tChan.echo:Send(strVal)    
    
    iTime = iTime + b - a
    iCount = iCount + 1
end
