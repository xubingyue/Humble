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

function runTask()
        local varRecv = pChan:Recv()
        local sock, uiSession, strMsg = table.unpack(utile.unPack(varRecv))        
     
        humble.Send(sock, uiSession, strMsg)
end

function destroyTask()
    
end
