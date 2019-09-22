# HookSigntool
## 简介
本项目编译结果为`HookSigntool.dll`，用于亚洲诚信数字签名工具（或其他类似的签名工具，如天威诚信代码签名证书助手，沃通代码签名工具，环玺信息数字签名工具等等）它的作用如下：
1. Hook数字签名工具对证书有效期的判断，无需修改系统的时间，即可用过期的证书进行数字签名。
2. 增加自建的时间戳服务器，配合过期证书使用，伪造证书有效期内的时间戳签名，使得整个签名能被验证。（此功能需要修改数字签名工具本身）

## 原理
编译出的`HookSigntool.dll`通过微软的Detours库Hook了签名工具的函数调用以达到目的
总共Hook了6个函数：
1. [crypt32.dll!CertVerifyTimeValidity](https://docs.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-certverifytimevalidity) 返回值改为0，让签名工具误以为所有证书都在有效期内，以便在不修改系统时间的情况下用过期证书签名。
2. [mssign32!SignerSign](https://docs.microsoft.com/en-us/windows/win32/seccrypto/signersign) 传入参数 pwszHttpTimeStamp 修改为自建时间戳地址（自建时间戳接受地址中设定的时间，用以伪造签名）
3. [mssign32!SignerTimeStamp](https://docs.microsoft.com/en-us/windows/win32/seccrypto/signertimestamp) 同上
4. [mssign32!SignerTimeStampEx2](https://docs.microsoft.com/zh-cn/windows/win32/seccrypto/signertimestampex2) 同上
5. [mssign32!SignerTimeStampEx3](https://docs.microsoft.com/zh-cn/windows/win32/seccrypto/signertimestampex3) 同上 （此函数在 Windows 7 上不存在）
6. [kernel32.dll!GetLocalTime](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlocaltime) 返回值根据配置文件修改，对于程序功能无影响。

## 用法
这个`dll`有两种设置方法，一种是`ini`文件，另一种是命令行参数
### 时间表示方法
本程序所用的时间表示方法为SimpleDateFormat，即格式为 `yyyy-MM-dd'T'HH:mm:ss` 的UTC时间
北京时间是UTC+8，所以时间需要减掉8小时才能变成UTC时间
举几个例子：
北京时间 `2011-04-01 08:00:00`，表示为 `2011-04-01T00:00:00`
北京时间 `2019-03-10 10:25:34`，表示为 `2019-03-10T02:25:34`
### ini文件
程序默认使用同目录下的`hook.ini`，当然，也可以通过命令行参数`-config`指定其他`ini`文件
```
;[Timestamp]Section中设置SignerTimeStamp的参数
;本处设置的是时间戳签名伪造的默认时间（SimpleDateFormat）
;可以通过命令行 -ts 参数传递替换的Timestamp
[Timestamp]
Timestamp=2011-04-01T00:00:00

;[Time]Section中设置GetLocalTime的返回值
;本处设置的时间不判断证书有效期，也不是时间戳时间，仅仅影响证书管理界面的证书颜色（到期天数）
;如需设置，请请删除注释分号
[Time]
;Year=2011
;Month=4
;Day=1
;Hour=0
;Minute=0
;Second=0
```
### 命令行参数
向数字签名工具的`exe`文件传递启动参数（如亚洲诚信数字签名工具，则是对`DSigntool.exe`传递参数）
#### -config
指定一个如上描述的`ini`文件的位置（相对路径或绝对路径），比如：
```
DSigntool.exe -config hook.ini
DSigntool.exe -config ../another.ini
DSigntool.exe -config D:\Signtool\config.ini
```
#### -ts
指定时间戳需要伪造的签名时间，用SimpleDateFormat表示
`-ts`传递的时间优先级高于`ini`文件中配置的时间，因此`-ts`和`-config`参数同时存在时，程序使用`-ts`的时间
```
DSigntool.exe -ts 2011-04-01T00:00:00
DSigntool.exe -ts 2019-03-10T02:25:34
```
### 快捷方式
由于有命令行参数启动这种方式，所以可以通过lnk快捷方式启动数字签名工具，来达到修改时间戳日期的功能。
编辑指向数字签名工具的lnk快捷方式，在目标后面加上`-ts`参数即可。例如：
```
目标 "C:\Program Files (x86)\DSignTool\DSignTool.exe" -ts 2015-04-01T00:00:00
起始位置 "C:\Program Files (x86)\DSignTool"
```
通过这个lnk启动的数字签名工具就被设定了时间戳日期。
也可以根据需要制作多个不同的快捷方式，设定不同的时间。

## 编译
编译环境：Visual Studio 2019 (生成工具v142)

依赖库：[Detours](https://github.com/Microsoft/Detours)库

编译步骤：
1. 下载微软Detours库 https://www.microsoft.com/en-us/download/details.aspx?id=52586 并解压缩
2. 开始菜单中打开`x86 Native Tools Command Prompt for VS 2019`，进入解压缩的Detours目录，运行`nmake`，编译出x86的lib (x64无需编译)
3. 将Detours目录下`include`,`lib.X86`目录下的文件复制到 `C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\{Version}`目录的`include`,`lib\x86`子目录中，完成Detours的安装
4. 打开项目文件`HookSigntool.sln`解决方案配置为Release，直接编译即可。

## 修改数字签名工具
想要数字签名工具加载`HookSigntool.dll`则需要用LordPE等工具修改这个`exe`的导入表，向导入表添加`HookSigntool.dll!attach`
### 绕过证书有效期验证
这个比较容易，只要签名工具加载了本`dll`，无需任何操作即可成功。
### 添加自定义时间戳
需要将软件内置的时间戳地址替换为特殊标记（包括`http://`部分整个替换）
一般是用十六进制编辑器直接找到字符串进行替换，原字符串的多余长度用 0x00 填充
SHA1的时间戳地址整个替换为 `{CustomTimestampMarker-SHA1}`
SHA256的时间戳地址整个替换为 `{CustomTimestampMarker-SHA256}`
这个标记与时间戳协议无关，无论是Authenticode还是RFC3161都相同
dll会特异性识别这个标记，自动将它修改为时间戳地址

## 关于时间戳服务器
时间戳服务器是我自己编写搭建的，域名是 `timestamp.pki.jemmylovejenny.tk`
根证书由我自己签发，时间戳有两条证书链，分别为SHA1和SHA256
以当前时间签名时，服务器地址是`http://timestamp.pki.jemmylovejenny.tk/SHA1/`和`http://timestamp.pki.jemmylovejenny.tk/SHA256/`
伪造任意时间的时间戳签名时，服务器地址在以上基础上加上SimpleDateFormat表示的时间，例如：
```
http://timestamp.pki.jemmylovejenny.tk/SHA1/2011-04-01T00:00:00
http://timestamp.pki.jemmylovejenny.tk/SHA256/2019-03-10T02:25:34
```
Authenticode和RFC3161协议的地址都是相同的，服务器会根据请求的不同自动识别并处理。
域名中的`timestamp`可以简写为`tsa`，即`tsa.pki.jemmylovejenny.tk`
### 配合微软signtool使用
首先将程序签好名（不带时间戳），假设有N个签名
那么对于第一个签名打时间戳：`signtool timestamp /t "<URL>" <filename>`
对于之后的任意个签名打时间戳：`signtool timestamp /tp <index> /tr "<URL>" <filename>`
其中`URL`为时间戳服务器地址，`index`从1开始递增，例如：
```
signtool timestamp /t "http://tsa.pki.jemmylovejenny.tk/SHA1/2011-04-01T00:00:00" test.exe
signtool timestamp /tp 1 /tr "http://tsa.pki.jemmylovejenny.tk/SHA256/2011-04-01T00:00:00" test.exe
```

## 关于驱动签名
根据微软的最新签名策略 https://docs.microsoft.com/en-us/windows-hardware/drivers/install/kernel-mode-code-signing-policy--windows-vista-and-later- 

任何有`Microsoft Code Verification Root`交叉签名，且颁发日期在2015-07-29以前的代码签名证书，配合伪造的时间戳签名，可以生成一个在任意Windows版本下都有效的驱动签名。

因此，采用泄露的证书，信任自建时间戳根证书，就可以在WinXP~Win10(SecureBoot Enabled)任意版本成功加载驱动。

## 关于我的自建PKI
我的自建PKI根证书为`JemmyLoveJenny EV Root CA`，我使用的所有自签名证书都由它颁发。
本程序使用的自建时间戳服务器的证书也不例外，因此想要时间戳受信，需要手动信任这个根证书。
更多信息请访问`https://pki.jemmylovejenny.tk/`