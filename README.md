智居泛控示例

## 介绍

本示例展示了如何使用智居泛控API实现智居大模型的多轮对话的语义控制，此示例依赖讯飞语音作为音频识别输入，并使用智居大模型服务作为对话引擎。

## 一、编译说明

新建一个构建目录，进入构建目录使用CMake生成编译脚本，最后执行make生成目标执行文件。

```sh
mkdir build
cd build
cmake ../hai-speech
make
```

编译正常结束，会在构建目录的子目录中生成目标执行文件： `build/bin/hai-speech-app`

## 二、使用示例

可以使用`-h | --help`来查看执行参数提示：

```sh
> ./bin/hai-speech-app -h
SYNOPSIS
        ./bin/hai-speech-app [-c <alsa channels>] [-r <alsa rate>] [-s <alsa period_size>] [-n <alsa
                             period_count>] [-f <alsa format: 16|32>] [-C <sound card number>] [-D
                             <sound card device number>] [-H <service host>] [-P <service port>] [-R
                             <resource path>] [-W <work path>] [-S <session id>] [-L <in which
                             room?>] [-T <work time, n seconds>] [-V <debug infometion details, 1 or
                             2>]
```

关键参数说明：

* option("-C", "--card"): 如果有多个声卡，请指定声卡号
* option("-D", "--device"): 如果设备内有多个录音设备，请指定设备号
* option("-H", "--host"): 指定智居大模型服务地址或域名
* option("-P", "--port"): 指定智居大模型服务端口
* option("-R", "--resdir"): 本地资源目录，比如唤醒词识别文件
* option("-S", "--session"): 请求服务的会话ID，此ID对应一组唯一的对话历史列表
* option("-L", "--local"): 指定控制缺省使用的位置或房间
* option("-T", "--time"): 指定运行时长，以秒单位
* option("-V", "--debug"): 打印调试信息，1级或2级

> 查看当前系统的声卡列表信息： `aplay -l`

> 该示例使用讯飞语音，其中唤醒功能需要依赖唤醒词识别文件，源码中预置了一个：`hai-speech/ifly/res/wakeupresource.jet`。此预置文件的唤醒词是`小牛小牛`。

启动示例：

该示例指定了使用声卡1，资源目录位于"$PWD/hai-speech/ifly/res"，云端服务端口为18127，并且打印1级和调试信息

```sh
./build/bin/hai-speech-app -R "$PWD/hai-speech/ifly/res" -C 1 -P 18127 -V 1
```

## 三、源码目录说明

### 3.1. hai-speech

此目录是示例源码，该示例实现了功能流程： 录音 -> 语音识别 -> 智居泛控服务

### 3.2. tinyalsa

hai-speech的依赖库，该库提供了轻量级的linux的标准音频接口。用于抓取音轨。

### 3.3. hude

hai-speech的依赖库，该库提供了适合嵌入式环境的轻量级异步IO库，主要是其Websocket组件。

## 四、源码关键事件处理

通常可以把源码文件`hai-speech/app/app.cpp`作为应用模板，在其中找到这几行代码：

```cpp
int main( int argc, char* argv[] )
{
    ...

    // Streaming events returned by the server
    auto on_stream = [=]( int err, std::string* info ) {
        if( err ) {
            llog_err("<app>", "cloud request error: %d", err );
        } else {
            llog_nte("<app>", "cloud relpy stream: %s", info->c_str() );
        }
    };

    // Wake-Up event
    auto on_wakeup = []( std::string* info ) {
        llog_inf("<app>", "wakeup event, info: %s", info->c_str() );
    };

    // voice recognition event
    auto on_recognition = [=, &ws]( std::string* text ) {
        llog_inf("<app>", "to sleep event, last result: %s", text? text->c_str() : "nullptr" );

        if( text == nullptr ) return;

        llog_dug("<app>", "llm service request: %s", text->c_str() );

        ws.dialog( *text, on_stream );
    };

    ...
```

这几行代码分别设置了语音唤醒事件（on_wakeup）、语音内容识别事件（on_recognition）和智泛服务响应事件（on_stream）的处理过程。

1. 语音唤醒事件（on_wakeup），该事件在发生语音唤醒时触发。
2. 语音内容识别事件（on_recognition），在语音唤醒后立即开始语音识别，如果在识别到语音内容结果则发生该事件。
3. 智泛服务响应事件（on_stream），通常语音识别事件的处理过程会直接将识别结果用于向智泛服务发起对话请求，当智泛服务响应到达时发生on_stream事件。
