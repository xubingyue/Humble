# Humble

Humble是c++、lua语言开发的多线程服务器框架,网络底层使用libevent。    
目前支持MQTT、WebSocket、Http等协议。     

##一、编译Humble     
* windows使用vs2015；  
* linux使用mklib.sh编译依赖库，然后再mk.sh。  

##二、配置文件  
* config.ini 文件配置服务器启动参数。   

##三、使用(回显服务)   
* 建立监听(start.lua onStart()):   
--监听   
humble.addListener(1, "0.0.0.0", 15000)    
--数据解析   
humble.setParser(1, "tcp1")    
    
* 创建echo服务，加入函数(echo.lua)：     
function initTask()      
end    
function runTask()    
end     
function destroyTask()   
end   

* 注册echo服务(start.lua onStart())    
--echo 为lua文件名，即模块名   
humble.regTask("echo")      
--获取echo消息通道，每个模块注册后都会有一对应的消息通道(g_pChan)   
tChan.echo = humble.getChan("echo")      

* 向echo发送消息(start.lua onTcpRead(......))    
tChan.echo:Send(utile.Pack(sock, uiSession, buffer))    

* 回显(echo.lua runTask())          
--取出消息   
local sock, uiSession, buffer = utile.unPack(pChan:Recv())
......
--返回消息       
humble.Send(sock, uiSession, buffer)   

##联系我    
279133271@qq.com    
