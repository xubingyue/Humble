local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local humble = require("humble")
local utile = require("utile")
local table = table
local string = string

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

function initTask()
    tChan.timer = humble.getRecvChan("timetick", "test")
end

function runTask()
    if tChan.timer:canRecv() then
        local varRecv = tChan.timer:Recv()
        local itick, icount = table.unpack(utile.unPack(varRecv))
        --print(string.format("timer task tick %d count %d", itick, icount))
    end
end

function destroyTask()
    
end