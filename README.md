# UnrealAutomator

A UE4 plugin for game testers, especially for mobile game testers, **currently still in development**

UnrealAutomator holds an HTTP server to receive requests, serves lots of basic modules for remote controllers to control the game client

Run standalone game to try it~

UnrealAutomator主要用于QA的日常测试以及自动化工作。利用UE4自带的HTTP Server框架，UnrealAutomator可以方便地扩展功能，和其他的服务对接。

UnrealAutomator当前包含了Widget信息导出（GAutomator/PocoSDK的功能）、lua支持，以及其他的一些基础功能，并没有掺杂更加复杂的要素。不论是QA还是开发者，都可以在上面集成更多的功能。可以当作一个GM指令的SDK，也可以当作一个专项测试的backend。

## Modules

- UWidget Info Dump
- UnLua/slua_unreal Support
- Input Action Support
- Profiling Info Dump

## Requirements

- UE4（.24+？）

## References

- [GAutomator](https://github.com/Tencent/GAutomator)
- [PocoSDK](https://github.com/AirtestProject/Poco-SDK/tree/master/Unreal/PocoSDK)
