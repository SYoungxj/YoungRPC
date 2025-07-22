#include "MprpcConfig.h"
#include <iostream>
#include <fstream>

using std::cout;
using std::ifstream;

MprpcConfig * MprpcConfig::pInstance_ = nullptr;

auto MprpcConfig::GetInstance() -> MprpcConfig *
{
    if(nullptr == pInstance_)
    {
        pInstance_ = new MprpcConfig();
    }
    return pInstance_;
}

// 负责加载配置文件
void MprpcConfig::LoadConfigFile(const string & configFile)
{
    ifstream ifs(configFile);
    if(!ifs.good())
    {
        cout << "ifstream open " << configFile << "error\n";
        exit(EXIT_FAILURE);
    }

    string line;
    while(getline(ifs, line))
    {
        int idx = line.find('=');
        string key = line.substr(0, idx);
        string value = line.substr(idx+1);
        configMap_.insert({key, value});
    }
}

// 查询配置项信息
auto MprpcConfig::Load(const string & key) -> string
{
    auto it = configMap_.find(key);
    if(it == configMap_.end())
    {
        return "";
    }
    return it->second;
}
