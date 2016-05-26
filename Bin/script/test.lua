
require("macros")
local humble = require("humble")
local utile = require("utile")
local table = table
local string = string
local pChan = g_pChan

function initTask()

end

function runTask()
    local varRecv = pChan:Recv()
    local itick, icount = utile.unPack(varRecv)
    --print(string.format("timer task tick %d count %d", itick, icount))    
end

function destroyTask()
    
end