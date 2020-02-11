#include <rpc/export.h>
#include <iostream>

RPC_API int rpc_api()
{
    std::cout << "hello from rpc" << std::endl;
    return 42;
}
