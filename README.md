# UnrealAutomator

A UE4 plugin for game testers, especially for mobile game testers, **currently still in development**

UnrealAutomator holds an HTTP server to receive requests, serves lots of basic modules for remote controllers to control the game client

Run standalone game to try it~

## Requirements

- UE4.24+

## Introduction

国内手游针对UE4的代码驱动测试，当前的主流方案是在客户端中打一个插件或者模块，开一个socket服务器，接收外来信息执行对应操作，典型的解决方案是GAutomator和PocoSDK。

GAutomator和PocoSDK都只满足了UI自动化的需求，以手游测试为例，这两个最核心的功能是从游戏中获取Widget的坐标，然后通过外部的adb client操作input tap以及swipe等命令去实现对手机的操作。但UE4里面可挖掘的地方还有很多，仅仅找UI肯定是不够的，因此心血来潮，准备单独造一个叫做UnrealAutomator的轮子，去集成更多的利于UE4手游测试业务的功能。

作为对外服务的模块，UnrealAutomator需要保持稳定的通信机制，如果采用非规范的socket通信与协议的话，插件本身的扩展性会大打折扣，且开发时也要为底层通信下一些功夫。UE4较新的版本引入了HTTP Server的模块，考虑到HTTP是适用性最为广泛、且通信较为稳定的协议，UnrealAutomator采用了这个设定，基于[UnrealHttpServer](https://github.com/utmhikari/UnrealHttpServer)进行二次开发，扩展了相关功能。唯一的缺点，UE4提供的HTTPServer方案稍微简陋一点，提供的接口跟restful规范也并不是最为完善的，但在UnrealAutomator上已经足够用了。

HTTP Web框架本身就有很多研发范式，因此UnrealAutomator非常容易模块化，非常适合用作二次开发以及向平台方向扩展。如果用过GAutomator的同学，一定为GAutomator的python客户端中耦合了UIAutomator、WeTest等大量模块而犯愁。因此UnrealAutomator在设计上，对外是自由的，对内是严苛的。对外自由，意味着UnrealAutomator客户端的开发者只需要集成简单的功能（嗯，adb相关的肯定得要），就能够实现相关需求；对内是严苛的，意味着内部的逻辑一定要尽可能深入挖掘信息服务给外部，代码是要精细且经得起推敲，使得让人能够很快看懂，能够很容易地去修改。

如果说手游测试是以玩家的角度去看待游戏产品的话，那么UnrealAutomator就是以玩家的角度去看待UE4 C++模块。虽然自己以前没怎么写过C++，但简易的Web后端框架还是比较随手的，因此借者UnrealAutomator这个机会，希望能够多一点输出，为手游测试行业做出一点贡献。

## References

- [GAutomator](https://github.com/Tencent/GAutomator)
- [PocoSDK](https://github.com/AirtestProject/Poco-SDK/tree/master/Unreal/PocoSDK)
