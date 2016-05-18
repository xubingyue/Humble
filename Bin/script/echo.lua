local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local utile = require("utile")
local humble = require("humble")
local tcp = require("tcp")
local httpd = require("httpd")
local table = table
local string = string
local pChan = g_pChan

if not g_pBinary then
    g_pBinary = CBinary()
end
local pBinary = g_pBinary

function initTask()
    
end

local iTime = 0
local iCount = 0

function runTask()
    local a = os.clock()
    local varRecv = pChan:Recv()
    local sock, uiSession, strMsg = table.unpack(utile.unPack(varRecv))        
     
    humble.Send(sock, uiSession, strMsg)
    local b = os.clock()
        
    iTime = iTime + b - a
    iCount = iCount + 1
end

function destroyTask()
    print("destroyTask"..iTime)
    print("destroyTask"..iCount)
end
