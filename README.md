# Humble
========
Humble是c++、lua语言开发的多线程服务器框架。    
目前支持MQTT、WebSocket、Http等协议。     

##一、编译Humble  
* windows使用vs2015；  
* linux使用mklib.sh编译依赖库，然后再mk.sh。

##二、配置文件  
* config.ini 文件配置服务器启动参数。   

##三、使用(回显服务)     
* 创建echo.lua，加入函数：     
function initTask()      
end    
function runTask()    
end     
function destroyTask()   
end   

* start.lua onStart() 函数中将其注册    
--echo 为lua文件名，即模块名   
humble.regTask("echo")      
--获取echo消息通道，每个模块注册后都会有一对应的消息通道(g_pChan)   
tChan.echo = humble.getChan("echo")                                     

* start模块onTcpRead读取到网络消息并向echo模块发送    
tChan.echo:Send(utile.Pack({sock, uiSession, buffer}))   

* 当有echo:Send调用后会将echo加入到任务队列等待执行(runTask()函数)    

* 执行(runTask()函数)      
--取出消息   
local sock, uiSession, buffer = table.unpack(utile.unPack(pChan:Recv()))   
....
