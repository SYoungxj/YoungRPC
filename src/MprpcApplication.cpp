#include "MprpcApplication.h"
#include "MprpcConfig.h"
#include <unistd.h>
#include <iostream>
#include <string>

using std::cout;
using std::string;

void ShowArgsHelp()
{
    cout << "format: command -i <configfile>\n";
}

MprpcApplication * MprpcApplication::pInstance_ = nullptr;
MprpcConfig * MprpcApplication::pConfig_ = nullptr; 

auto MprpcApplication::GetInstance() -> MprpcApplication *
{
    if(nullptr == pInstance_)
    {
        pInstance_ = new MprpcApplication();
    }
    return pInstance_;
}
MprpcConfig * MprpcApplication::GetConfig()
{
    return pConfig_;
}

void MprpcApplication::Init(int argc, char ** argv)
{
    if(argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch(c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            cout << "invalid args!\n";
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            cout << "need <configfile>\n";
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 初始化读配置文件类
    pConfig_ = MprpcConfig::GetInstance();
    // 开始加载配置文件了
    pConfig_->LoadConfigFile(config_file);

    cout << pConfig_->Load("RPCServerIP") << "\n";
    cout << pConfig_->Load("RPCServerPORT") << "\n";
    cout << pConfig_->Load("ZookeeperIP") << "\n";
    cout << pConfig_->Load("ZookeeperPORT") << "\n";
}
