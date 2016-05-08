--[[
函数注册
--]]

require("timewheel")
local utile = require("utile")

local table = table
local type = type
local string = string
local tonumber = tonumber
local callFunc = utile.callFunc
local m_iOneDay = 24 * 60 * 60

local cbfunc = {}

if not g_RegFuncs then
    g_RegFuncs = {}
    
    --网络消息
    g_RegFuncs.NetEvent = {}
    --游戏事件
    g_RegFuncs.GameEvent = {}
    --延迟事件
    g_RegFuncs.DelayEvent = WheelMgr:new()
end

local RegFuncs = g_RegFuncs

--[[
描述：延迟事件执行
参数：
返回值：无
--]]
function cbfunc.onDelayEvent()   
    RegFuncs.DelayEvent:onTime()
end

--[[
描述：延迟事件注册
参数：iTime --延后时间（秒）
返回值：TvecBase
--]]
function cbfunc.regDelayEvent(iTime, Func, ...)
    return RegFuncs.DelayEvent:addTimer(Func, iTime, table.unpack({...}))
end
function cbfunc.regDelayEventByBase(objTvecBase)
    return RegFuncs.DelayEvent:Add(objTvecBase:getTime(), objTvecBase)
end
--strTime 格式(24小时制)：12:36:28
function cbfunc.regDelayEventAtTime(strTime, Func, ...)
    local strHour, strMin, strSec = string.match(strTime, "(%d+):(%d+):(%d+)")
    local iTime = (tonumber(strHour) * 60 * 60) + (tonumber(strMin) * 60) + tonumber(strSec)
    local tNow = os.date("*t", time)
    local iNowTime = (tonumber(tNow.hour) * 60 * 60) + (tonumber(tNow.min) * 60) + tonumber(tNow.sec)
    
    local iDelayTime = 0
    
    if iTime >= iNowTime then
        iDelayTime = iTime - iNowTime
    else
        iDelayTime = m_iOneDay - (iNowTime - iTime)
    end
    
    return cbfunc.regDelayEvent(iDelayTime, Func, table.unpack({...}))
end

--[[
描述：延迟事件移除
参数：objTvecBase
返回值：无
--]]
function cbfunc.removDelayEvent(objTvecBase)
    RegFuncs.DelayEvent:Remove(objTvecBase)
end

--[[
描述：延迟事件总共走过多少时间
参数：无
返回值：无
--]]
function cbfunc.getDelayEventTick()
    return RegFuncs.DelayEvent:getTick()
end

--[[
描述：游戏事件回调
参数：iEvent --事件编号
返回值：无
--]]
function cbfunc.onGameEvent(iEvent, ...)
    if not RegFuncs.GameEvent[iEvent] then
        return
    end
    
    for _, val in pairs(RegFuncs.GameEvent[iEvent]) do     
        if val then
            callFunc(val, table.unpack{...})
        end
    end
end

--[[
描述：游戏事件注册
参数：
返回值：无
--]]
function cbfunc.regGameEvent(iEvent, Func)
    if "function" ~= type(Func) then
        return
    end
    
    if not RegFuncs.GameEvent[iEvent] then
        RegFuncs.GameEvent[iEvent] = {}
    end
    
    table.insert(RegFuncs.GameEvent[iEvent], Func)
end

--[[
描述：网络可读事件回调
参数：
返回值：无
--]]
function cbfunc.onNetEvent(usSockType, iProtocol, ...)
    if not RegFuncs.NetEvent[usSockType] then
        return
    end
    
    local Func = RegFuncs.NetEvent[usSockType][iProtocol]
    if Func then
        callFunc(Func, table.unpack{...})
    end
end

--[[
描述：网络可读事件注册
参数：
返回值：无
--]]
function cbfunc.regNetEvent(usSockType, iProtocol, Func)
    if "function" ~= type(Func) then
        return
    end
    
    if not RegFuncs.NetEvent[usSockType] then
        RegFuncs.NetEvent[usSockType] = {}
    end
    
    RegFuncs.NetEvent[usSockType][iProtocol] = Func
    utile.Debug("register protocol %d", iProtocol)
end

return cbfunc
