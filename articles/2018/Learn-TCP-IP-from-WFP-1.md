# 从 Windows Filtering Platform 学习 TCP/IP（1）

> 2018/4/18
>
> 彼节者有间，而刀刃者无厚；以无厚入有间，恢恢乎其于游刃必有余地矣。——《庄子·养生主》

[heading-numbering]

## [no-toc] [no-number] TOC

[TOC]

## 背景

毕业设计要求开发一个匿名通信客户端（Windows 网络驱动）：

- 拦截/修改通过主机的流量，实现 [匿名通信协议](https://dl.acm.org/citation.cfm?doid=3140549.3140554)；
- 防止 [WinPcap (Wireshark)](https://www.winpcap.org/docs/iscc01-wpcap.pdf) 对原始流量的嗅探

换句话说，就是需要：

- 拦截/修改 **上层应用程序** 传输的数据包，客户端工作对其 **透明**
  - 需要实现修改（源/目的）TCP/UDP 端口、IP 地址、以太网 MAC 地址的能力
  - 例如，用户用浏览器打开 Baidu，经过驱动修改后，可以变成 Google
- 必须在 WinPcap 嗅探位置 **之前修改** 数据，从而使其嗅探不到原始流量

借此机会，学习了 TCP/IP 协议栈、Windows 驱动开发、Windows 网络架构。

## 调研

### WinPcap 流量嗅探原理

[WinPcap 的核心](https://www.winpcap.org/docs/docs_412/html/group__internals.html) 是一个叫做 NPF _(Netgroup Packet Filter)_ 的 [NDIS _(Network Driver Interface Specification)_ 协议驱动](https://docs.microsoft.com/en-us/windows-hardware/drivers/network/ndis-protocol-drivers2)，在 Windows 网络驱动栈的较底层（下层直接对接网卡驱动），实现了对经过网卡（物理网卡/虚拟网卡）的所有流量的嗅探。

[align-center]

![Npf Ndis](Learn-TCP-IP-from-WFP/npf-ndis.gif)

[align-center]

图 [NPF 在 NDIS 中的位置](https://www.winpcap.org/docs/docs_412/html/group__NPF.html)

> 注：
>
> - Wireshark/WinDump 使用 WinPcap 作为嗅探引擎
> - [npcap](https://nmap.org/npcap/) 和 [Win10Pcap](http://www.win10pcap.org/) 是对 WinPcap 的改进版本，原理相似
>   - https://github.com/nmap/npcap
>   - https://github.com/SoftEtherVPN/Win10Pcap

### Windows Vista 以后的 TCP/IP 栈

在 Windows Vista 以后，微软升级了 TCP/IP 栈，将核心的 TCP/IP 逻辑放入 `tcpip.sys` 驱动实现（例如，TCP 的建立连接三次握手、释放连接四次挥手，IP 的维护 ARP 表等）。

[align-center]

![Windows Next Generation Tcp Ip Stack](Learn-TCP-IP-from-WFP/windows-next-generation-tcp-ip-stack.png)

[align-center]

图 Windows 下一代 TCP/IP 协议栈（来源：Windows Internals, 6th Edition, Part 1）

### 技术方案

> 彼节者有间

微软的 TCP/IP 驱动设计时，为了提供更好的 **灵活性**（例如定制 TCP/IP 协议，实现 NAT、IPsec 等），在 TCP/IP 协议栈上的 **每层**（例如，应用层、传输层、网络层，Windows 8 加入了 MAC 子层）暴露一系列 [Windows Filter Platform _(WFP)_](https://msdn.microsoft.com/en-us/library/windows/desktop/aa366510%28v=vs.85%29.aspx) 接口。

> 以无厚入有间

利用这些接口，我们不仅可以过滤（放行/拦截）经过 TCP/IP 驱动的流量，还可以实现对流量的修改（修改内容/修改包头重定向）。恰好，TCP/IP 驱动和 WinPcap 的 NPF 驱动都是 NDIS 协议驱动，处于平级关系。

> 恢恢乎其于游刃必有余地矣

所以，我们使用 WFP 提供的接口，就可以绕过 WinPcap 的嗅探。

## 试验（修改目的 IP 地址/端口）

为了验证 WFP 是否满足我们的需求，我们基于微软官方的 [WFP 流量拦截样例](https://github.com/Microsoft/Windows-driver-samples/tree/master/network/trans/inspect) 进行修改，实现一个 proof of concept _(PoC)_：

- 修改 TCP/UDP（源/目的）**端口号**
- 修改 **发往** 主机 A 的包的 目的 IP 地址，**重定向** 到主机 B
- 修改从主机 B **收到** 的包的 目的 IP 地址，**还原** 为主机 A 的地址（否则上层应用无法识别）
- 对上层应用 **透明**，即上层应用不知道上述修改的存在

我们在同一个局域网内部署了两个主机，分别记作 `202` 和 `103`。在主机 `202` 上进行如下测试：

- 访问主机 `104`（一个不存在的主机），需要将流量重定向到主机 `103` 上
- 访问主机 `103` 的 `81` 端口，需要将流量重定向到 `80` 端口上
- 访问主机 `104` 的 `81` 端口，需要将流量重定向到主机 `103` 的 `80` 端口上

### 传输层修改收包

最初，我们的 PoC 是从官方的 WFP 拦截样例改过来的：

- 在 `OUTBOUND_TRANSPORT` 和 `INBOUND_TRANSPOR` 上完成修改
- 通过直接修改 `NET_BUFFER`，修改端口
- IP 地址的修改：发包设置 `FWPS_TRANSPORT_SEND_PARAMS`，收包修改 IP 包头

#### 结果

- 成功重定向 IP 地址、端口；主机 `103` 能接收到发往 `80` 端口的数据包
- 主机 `103` 的 `80` 端口发出的数据包，能正确送达主机 `202`（pcap 能抓到）
- 但是，`INBOUND_TRANSPORT` 层 **没有拦截到** 收回的数据包
- 从而导致，两个主机超时重传 `SYN`；最后主机 `103` 发出 `RST` 结束这个过程

![Transport Inbound Redirection](Learn-TCP-IP-from-WFP/transport-inbound-redirection.png)

#### 分析

每一个 TCP/UDP 包都可以用一个 [**五元组** _(5-tuple)_](http://www.w3.org/People/Frystyk/thesis/TcpIp.html) 表示 —— 源 IP、源端口、目的 IP、目的端口、协议（TCP 或者 UDP）。而一个 TCP/UDP 报文要能正确的向上传递，则需要有正确的五元组。

由于从主机 `103` 收回的包，在往上传递时，没有及时把 IP 地址还原到 `104` / 端口还原到 `81`；TCP 驱动发现这个包的五元组有问题，就会直接丢弃，导致 `INBOUND_TRANSPOR` 层不能拦截到这个包。

所以，我们要在到达 **传输层** 之前，即在 **网络层** 修改收包的 IP/端口（直接修改 `NET_BUFFER`）。

### 传输层修改发包

重新设计后，我们在 **传输层** 修改发包，**网络层** 修改收包。（`OUTBOUND_TRANSPORT` & `INBOUND_NETWORK`）

#### 结果

- TCP 建立连接三次握手、TCP 数据传输都没问题
- 但是，释放连接的第四次挥手（主机 `202` 最后一个 `ACK`）发不出去，导致
  - 主机 `202` 先关闭了 socket（发完了最后一个 `ACK` 就可以关闭了）
  - 主机 `103` 等待最后一个 `ACK`，不断重传 `FIN+ACK`，最后发送 `RST` 结束这个过程

![Transport Outbound Redirection](Learn-TCP-IP-from-WFP/transport-outbound-redirection.png)

#### 分析

在传输层，发送数据需要关联特定的 socket；在 `OUTBOUND_TRANSPORT` 层发送的数据包，调用 `FwpsInjectTransportSendAsync` 函数也需要指定 socket 对应端点的 `endpointHandle`。

根据 [MSDN 描述](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/content/fwpsk/nf-fwpsk-fwpsinjecttransportsendasync1)，如果不及时发送数据，socket 关闭、端点资源释放后，会导致数据无法发送。（详见 [Github Issue](https://github.com/Microsoft/Windows-driver-samples/issues/234)）

在我们的试验中，主机 `202` 收到了 `FIN+ACK` 后，发送最后一个 ACK，就可以直接关闭 socket。而这个 ACK 被我们的驱动拦截，在修改后重新发出时，端点资源可能已经被释放了，导致不能正确发送。

为了解决这个问题，我们推迟到 **网络层** 修改发包的 IP/端口（直接修改 `NET_BUFFER`）。

### 网络层修改发包

现在，收包、发包的修改都放到了 **网络层** 上。（`OUTBOUND_NETWORK` & `INBOUND_NETWORK`）

#### 结果

- 出人意料，`OUTBOUND_NETWORK` 层 **没有拦截到** 发出的数据包

#### 分析

经过原理性分析，如果一个 IP 包要通过网卡发到以太网上，必须要知道目的主机的 MAC 地址，从而构造出完整的 **以太网帧** _(Ethernet frame)_。为了验证这个猜想，我们利用 pcap 嗅探了 ARP 流量 —— 不出所料，主机 `202` 一直在询问主机 `104` 的 MAC 地址。

![Network Outbound Redirection Bad Arp](Learn-TCP-IP-from-WFP/network-outbound-redirection-bad-arp.png)

由于主机 `104` 并不存在，主机 `202` 收不到对应的 ARP 响应，无法构造发往主机 `104` 的以太网帧，从而导致 `OUTBOUND_NETWORK` 层不能拦截到发往主机 `104` 的包。

为了进一步验证这个猜想，我们把主机 `104` 换成了局域网内的另一台主机 `253` —— 即需要把访问主机 `253` 的 `81` 端口的流量，重定向到主机 `103` 的 `80` 端口上。然后，再利用 pcap 嗅探 ARP 流量。

![Network Outbound Redirection Good Arp](Learn-TCP-IP-from-WFP/network-outbound-redirection-good-arp.png)

果然，`OUTBOUND_NETWORK` 层拦截到了发往主机 `253` 的数据包；pcap 也能看出：主机 `202` 先询问了主机 `253` 的 MAC 地址，再询问了主机 `103` 的 MAC 地址。

也就是说，IP 驱动收到了 **传输层报文** _(transport diagram)_ 后，先会通过 ARP 查询，查找目的 IP 对应的主机的以太网地址，再传入我们的 `OUTBOUND_NETWORK` 层驱动。如果查不到目的 IP 对应的主机地址，就会直接丢弃这个包。

在 TCP/IP 栈上，**最复杂** 的就是 **网络层** —— 向上承接传输层，向下引起数据链路层。在传输层上，即使随意设置端口，也能构造出正确的 IP 包；但网络层中，如果设置的 IP 地址没有对应的主机，就不能构造出对应的以太网帧了。

## 小结

下一篇文章里，我们将

- 设计针对 ARP 协议的拦截/修改 PoC
- 设计修改源 IP 地址（在匿名通信网络中隐藏主机真实 IP 地址）的 PoC
- 总结 Windows TCP/IP 驱动实现流程

除了学习 WFP 和 Windows 网络架构之外，这次还学到了 Windows 驱动开发技巧：

- 代码里使用 `DbgPrint` 在每个关键路径上打日志，配合使用 DbgView 查看日志
- 当程序崩溃后，使用 WinDbg 分析 dump 文件，定位问题
- 使用 Windows 测试模式，禁用驱动签名检查（bcdedit -set TESTSIGNING ON）

如果有什么问题，**欢迎交流**。😄

Delivered under MIT License &copy; 2018, BOT Man
