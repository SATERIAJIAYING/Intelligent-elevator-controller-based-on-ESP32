# Intelligent-elevator-controller-based-on-ESP32
一、设计背景

在当下疫情形势严峻的特殊时期，传统接触式电梯按键上存在病毒传播的风险。我们设计了一个智慧电梯系统，通过语音识别的无接触方式控制电梯内部设备的运作，并配置火灾烟雾报警、扫描人员“身份码”，LCD显示等扩展功能，并实时与云平台交互数据，降低了疫情传播的风险，进一步达到特殊时期社区服务“智能化”“实用化”的目的。

二、系统方案

1、LCD显示

LCD屏幕上显示电梯所在楼层、上下行信息以及用户指引提示。因为屏幕显示相关函数封装于该类，所以我们设计将所有逻辑判断的代码写于该类。LCD类包含Speech类可以获得语音识别结果，根据语音识别结果ID得到所在楼层和上下行信息，并且进行用户权限判断。

以下描述我们是如何在LCD屏幕上显示图片。因为who的LCD驱动只支持显示完整图片，首先我们利用PS对图像进行批量化处理，得到想要显示不同楼层和其他功能的图片，之后我们利用matlab imread函数读取各图像，并分别提取RGB值，将RGB888矩阵对应转化为RGB565数据并存入logo_en_240x240_lcd.h头文件中，通过void AppLCD::draw_wallpaper()函数构建位图功能进行画图。
2、语音识别

语音识别用于用户的无接触输入，在说出唤醒词“Hi，乐鑫”后，可使用“到达X楼”语音控制电梯到达的楼层。

通过更改menuconfig，可以修改基于Multinet的离线语音识别命令。Speech类作为LCD类的成员数据，可以让LCD显示函数直接访问到上次语音识别的命令ID，以此判断用户选择几楼。

同时将app_speech.cpp中采集到的语音数据，通过sdmmc.h中的相关函数，写入到对应的SD卡中，该功能的调试正常，也可以正常烧录运行，但可能导致SD卡出现不能被windows识别的情况
3、二维码识别

二维码识别用于验证用户的身份，以判断用户可以命令电梯前往那些非公共楼层。二维码由物业使用包含用户权限和身份的加密字符串生成，电梯的摄像头拍到用户展示的二维码会把它解码成字符串并解密，然后根据其中信息通过LCD屏幕对用户做出反馈。

因为who里的二维码识别S3-EYE板的摄像头，所以选择使用百度智能云云计算功能。实现方法为先从摄像头的缓冲区里提取单帧图像数据转码成base64并编码成url的参数，然后向百度智能云二维码识别API的url发送post请求，百度智能云识别完成图片后会立即响应一个json识别结果，解析json即可得到二维码解码的字符串数据。（截至竞赛结束该功能未完美实现，与百度智能云的请求响应部分存在bug）

4、烟雾报警

用于检测电梯内是否有人抽烟或者是否发生火灾，如果发生异常立即上报云平台。

MQ-2烟雾传感器外设的数字信号口与S3-EYE板的子板上的多余stripping管脚相连，可以给烟雾传感器设定一个阈值，如果烟雾浓度超过该值就会触发烟雾报警。
5、wifi模块

物联网设备作为wifista，通过使用http_request功能实现基于百度智能云平台的二维码识别，将识别结果在主机和云平台之间完成传输。

三、功能实现

测试的过程见测试视频。

语音识别结合LCD显示的功能正常，但存在板子刚上电时LCD显示乱码，需要语音指定一个楼层后正常显示的问题。

通过将二维码识别将过程打印到串口上显示，发现相机读取、base64转码、url编码和wifista功能正常，但是向云平台的请求响应功能无法正常运行，导致二维码功能无法正常运转。

未彻底完成的部分：成功将音频文件储存到SD卡中（但是在格式上似乎有点问题，难以在windows系统里打开）

四、特色与创新点分析

语音智能控制：通过语音识别代替传统接触式按钮，实现无接触控制电梯，既提供了便捷的操作，也降低了疫情传播的风险。

多元信息显示：在LCD屏幕上显示出多元的运行信息（如具体楼层、上行下行等状态），以及“乐鑫欢迎您回家”的欢迎语，提升居民幸福感。

智能云平台交互及身份认证：通过和智能云平台进行数据交互，可以将通过摄像头监视到的信息上传到云平台，实现对用户的身份信息的识别，减少无关人员使用电梯的频率。
    
协助疫情防控：同时该“识别码”功能还存在与“健康码”进行互联的可能，代替工作人员的人工检测，降低人工成本，进一步实现疫情防控的自动化。

火灾报警检测：增设额外的MQ-2烟雾气敏传感器模块，检测环境液化气/烟雾/城市煤气是否超过设定的阈值，向主机及云平台发出报警信号，进一步保障住户安全。

-----------------------------------------------------------------------------------------------------------------------------------------

除去克隆该仓库，要想让LCD屏幕正确显示，需要将path\esp-idf\components\modules\lcd\logo_en_240x240_lcd.h替换成该仓库根目录下的logo_en_240x240_lcd.h文件

该仓库的功能还暂时不完整，我们将持续更新该项目

