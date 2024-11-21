#pragma once

#include <exception>  // 异常类
#include <string>     // 字符串类

    using namespace std;

/**
 *   表示与套接字调用执行有关的问题。
 */
class SocketException : public exception {
 public:
  /**
   *   使用说明性消息构造 SocketException。
   *   @param message 说明性消息
   *   @param incSysMsg 如果为 true，则将系统消息（来自 strerror(errno)）附加到用户提供的消息后面
   */
  SocketException(const string &message, bool inclSysMsg = false) throw();

  /**
   *   为了确保不抛出任何异常而提供。
   */
  ~SocketException() throw();

  /**
   *   获取异常消息
   *   @return 异常消息
   */
  const char *what() const throw();

 private:
  string userMessage;  // 异常消息
};

/**
 *   表示基本通信端点的基类
 */
class Socket {
 public:
  /**
   *   关闭和释放此套接字
   */
  ~Socket();

  /**
   *   获取本地地址
   *   @return 套接字的本地地址
   *   @exception SocketException 如果获取失败则抛出
   */
  string getLocalAddress() noexcept(false);

  /**
   *   获取本地端口
   *   @return 套接字的本地端口
   *   @exception SocketException 如果获取失败则抛出
   */
  unsigned short getLocalPort() const noexcept(false);

  /**
   *   将本地端口设置为指定的端口，并将本地地址设置为任何接口
   *   @param localPort 本地端口
   *   @exception SocketException 如果设置本地端口失败则抛出
   */
  void setLocalPort(unsigned short localPort) noexcept(false);

  /**
   *   将本地端口设置为指定的端口，并将本地地址设置为指定的地址。
   *   如果省略端口，则将选择一个随机端口。
   *   @param localAddress 本地地址
   *   @param localPort 本地端口
   *   @exception SocketException 如果设置本地端口或地址失败则抛出
   */
  void setLocalAddressAndPort(const string &localAddress, unsigned short localPort = 0) noexcept(false);

  /**
   *   如果是 WinSock，则卸载 WinSock DLL；否则什么也不做。
   *   我们在示例客户端代码中忽略这一点，但在库中包含它是为了完整性。
   *   如果您在 Windows 上运行并且担心 DLL 资源消耗，请在所有 Socket 实例完成后调用此函数。
   *   如果您在某个 Socket 实例存在时在 Windows 上执行此操作，则可能会出问题。
   *   为了客户端代码的可移植性，在非 Windows 平台上，这是一个空函数，因此您总是可以包含它。
   *   @param buffer 接收数据的缓冲区
   *   @param bufferLen 最大读入缓冲区的字节数
   *   @return 读取的字节数，EOF 为 0，错误为 -1
   *   @exception SocketException 如果 WinSock 清理失败则抛出
   */
  static void cleanUp() noexcept(false);

  /**
   *   将指定协议的服务解析为主机字节顺序中的相应端口号
   *   @param service 要解析的服务（例如 "http"）
   *   @param protocol 服务的协议。默认为 "tcp"。
   */
  static unsigned short resolveService(const string &service, const string &protocol = "tcp");

 private:
  // 防止用户在此对象上尝试使用值语义
  Socket(const Socket &sock);
  void operator=(const Socket &sock);

 protected:
  int sockDesc;  // 套接字描述符
  Socket(int type, int protocol) noexcept(false);
  Socket(int sockDesc);
};

/**
 *   能够连接、发送和接收的套接字
 */
class CommunicatingSocket : public Socket {
 public:
  /**
   *   与给定的远程地址和端口建立套接字连接
   *   @param foreignAddress 对方地址（IP 地址或名称）
   *   @param foreignPort 对方端口
   *   @exception SocketException 如果无法建立连接则抛出
   */
  void connect(const string &foreignAddress, unsigned short foreignPort) noexcept(false);

  /**
   *   将给定的缓冲区写入此套接字。在调用 send() 之前调用 connect()
   *   @param buffer 要写入的缓冲区
   *   @param bufferLen 要写入缓冲区的字节数
   *   @exception SocketException 如果无法发送数据则抛出
   */
  void send(const void *buffer, int bufferLen) noexcept(false);

  /**
   *   从此套接字读取最多 bufferLen 字节数据到给定的缓冲区中。在调用 recv() 之前调用 connect()
   *   @param buffer 接收数据的缓冲区
   *   @param bufferLen 最大要读入缓冲区的字节数
   *   @return 读取的字节数，EOF 为 0，错误为 -1
   *   @exception SocketException 如果无法接收数据则抛出
   */
  int recv(void *buffer, int bufferLen) noexcept(false);

  /**
   *   获取对方地址。在调用 recv() 之前调用 connect()
   *   @return 对方地址
   *   @exception SocketException 如果无法获取对方地址则抛出
   */
  string getForeignAddress() noexcept(false);

  /**
   *   获取对方端口。在调用 recv() 之前调用 connect()
   *   @return 对方端口
   *   @exception SocketException 如果无法获取对方端口则抛出
   */
  unsigned short getForeignPort() noexcept(false);

 protected:
  CommunicatingSocket(int type, int protocol) noexcept(false);
  CommunicatingSocket(int newConnSD);
};

/**
 *   用于与其他 TCP 套接字通信的 TCP 套接字
 */
class TCPSocket : public CommunicatingSocket {
 public:
  /**
   *   构造一个没有连接的 TCP 套接字
   *   @exception SocketException 如果无法创建 TCP 套接字则抛出
   */
  TCPSocket() noexcept(false);

  /**
   *   构造一个连接到给定远程地址和端口的 TCP 套接字
   *   @param foreignAddress 对方地址（IP 地址或名称）
   *   @param foreignPort 对方端口
   *   @exception SocketException 如果无法创建 TCP 套接字则抛出
   */
  TCPSocket(const string &foreignAddress, unsigned short foreignPort) noexcept(false);

 private:
  // 用于 TCPServerSocket::accept() 连接创建的访问
  friend class TCPServerSocket;
  TCPSocket(int newConnSD);
};

/**
 *   服务器使用的 TCP 套接字类
 */
class TCPServerSocket : public Socket {
 public:
  /**


   *   为服务器构造一个 TCP 套接字，接受在指定端口上任何接口上的连接
   *   @param localPort 服务器套接字的本地端口，值为零将给出一个系统分配的未使用端口
   *   @param queueLen 最大挂起连接请求的队列长度（默认为 5）
   *   @exception SocketException 如果无法创建 TCP 服务器套接字则抛出
   */
  TCPServerSocket(unsigned short localPort, int queueLen = 5) noexcept(false);

  /**
   *   为服务器构造一个 TCP 套接字，接受在指定地址的指定端口上的连接
   *   @param localAddress 服务器套接字的本地接口（地址）
   *   @param localPort 服务器套接字的本地端口
   *   @param queueLen 最大挂起连接请求的队列长度（默认为 5）
   *   @exception SocketException 如果无法创建 TCP 服务器套接字则抛出
   */
  TCPServerSocket(const string &localAddress, unsigned short localPort, int queueLen = 5) noexcept(false);

  /**
   *   阻塞，直到在此套接字上建立一个新连接或出现错误
   *   @return 新连接套接字
   *   @exception SocketException 如果尝试接受新连接失败则抛出
   */
  TCPSocket *accept() noexcept(false);

 private:
  void setListen(int queueLen) noexcept(false);
};

/**
 *   UDP 套接字类
 */
class UDPSocket : public CommunicatingSocket {
 public:
  /**
   *   构造一个 UDP 套接字
   *   @exception SocketException 如果无法创建 UDP 套接字则抛出
   */
  UDPSocket() noexcept(false);

  /**
   *   构造一个具有给定本地端口的 UDP 套接字
   *   @param localPort 本地端口
   *   @exception SocketException 如果无法创建 UDP 套接字则抛出
   */
  UDPSocket(unsigned short localPort) noexcept(false);

  /**
   *   构造一个具有给定本地端口和地址的 UDP 套接字
   *   @param localAddress 本地地址
   *   @param localPort 本地端口
   *   @exception SocketException 如果无法创建 UDP 套接字则抛出
   */
  UDPSocket(const string &localAddress, unsigned short localPort) noexcept(false);

  /**
   *   取消关联对方地址和端口
   *   @return 如果解除关联成功则返回 true
   *   @exception SocketException 如果无法断开 UDP 套接字的关联则抛出
   */
  void disconnect() noexcept(false);

  /**
   *   将给定缓冲区作为 UDP 数据报发送到指定的地址/端口
   *   @param buffer 要写入的缓冲区
   *   @param bufferLen 要写入的字节数
   *   @param foreignAddress 要发送到的地址（IP 地址或名称）
   *   @param foreignPort 要发送到的端口号
   *   @return 如果发送成功则返回 true
   *   @exception SocketException 如果无法发送数据报则抛出
   */
  void sendTo(const void *buffer, int bufferLen, const string &foreignAddress,
              unsigned short foreignPort) noexcept(false);

  /**
   *   从此套接字读取最多 bufferLen 字节数据。数据将被放置在给定的缓冲区中
   *   @param buffer 接收数据的缓冲区
   *   @param bufferLen 最多接收的字节数
   *   @param sourceAddress 数据报源的地址
   *   @param sourcePort 数据源的端口
   *   @return 接收的字节数和错误为 -1
   *   @exception SocketException 如果无法接收数据报则抛出
   */
  int recvFrom(void *buffer, int bufferLen, string &sourceAddress, unsigned short &sourcePort) noexcept(false);

  /**
   *   设置多播 TTL
   *   @param multicastTTL 多播 TTL
   *   @exception SocketException 如果无法设置 TTL 则抛出
   */
  void setMulticastTTL(unsigned char multicastTTL) noexcept(false);

  /**
   *   加入指定的多播组
   *   @param multicastGroup 要加入的多播组地址
   *   @exception SocketException 如果无法加入组则抛出
   */
  void joinGroup(const string &multicastGroup) noexcept(false);

  /**
   *   离开指定的多播组
   *   @param multicastGroup 要离开的多播组地址
   *   @exception SocketException 如果无法离开组则抛出
   */
  void leaveGroup(const string &multicastGroup) noexcept(false);

 private:
  void setBroadcast();
};