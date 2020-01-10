#include <examples/ace/ttcp/common.h>

#include <boost/program_options.hpp>

#include <iostream>

#include <netdb.h>
#include <stdio.h>

namespace po = boost::program_options;

bool parseCommandLine(int argc, char* argv[], Options* opt)
{
  //boost::program_options 用法
  //步骤一: 构造选项描述器和选项存储器
  //选项描述器，其参数为该描述的名字
  po::options_description desc("Allowed options");

  //步骤二: 为选项描述器增加选项
  //其参数依次为 key value的类型，该选项的描述
  desc.add_options()
      ("help,h", "Help")
      ("port,p", po::value<uint16_t>(&opt->port)->default_value(5001), "TCP port")
      ("length,l", po::value<int>(&opt->length)->default_value(65536), "Buffer length")
      ("number,n", po::value<int>(&opt->number)->default_value(8192), "Number of buffers")
      ("trans,t",  po::value<std::string>(&opt->host), "Transmit")
      ("recv,r", "Receive")
      ("nodelay,D", "set TCP_NODELAY")
      ;
  //选项存储器，继承自map容器
  po::variables_map vm;
  //parse_command_line() 对输入的选项做解析
  //store() 将解析后的结果存入选项存储器
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  //count()检测该选项是否被输入
  opt->transmit = vm.count("trans");
  opt->receive = vm.count("recv");
  opt->nodelay = vm.count("nodelay");
  if (vm.count("help"))
  {
    //options_description对象支持流输出， 会自动打印所有的选项信息
    std::cout << desc << std::endl;
    return false;
  }

  if (opt->transmit == opt->receive)
  {
    printf("either -t or -r must be specified.\n");
    return false;
  }

  printf("port = %d\n", opt->port);
  if (opt->transmit)
  {
    printf("buffer length = %d\n", opt->length);
    printf("number of buffers = %d\n", opt->number);
  }
  else
  {
    printf("accepting...\n");
  }
  return true;
}

#pragma GCC diagnostic ignored "-Wold-style-cast"

struct sockaddr_in resolveOrDie(const char* host, uint16_t port)
{
  struct hostent* he = ::gethostbyname(host);
  if (!he)
  {
    perror("gethostbyname");
    exit(1);
  }
  assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
  return addr;
}

