#include <iostream>
#include <windows.h>
#include <fstream>

// 将 long long 转换为大端字节序
long long toBigEndian(long long value) {
    long long result = 0;
    for (int i = 0; i < 8; ++i) {
        result |= ((value >> (i * 8)) & 0xFF) << ((7 - i) * 8);
    }
    return result;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "请提供key文件路径作为命令行参数！\n";
        std::cerr << "默认key文件路径：\n";
        std::cerr << "C:\\Users\\[USERNAME]\\AppData\\Roaming\\JetBrains\\PyCharm2021.2\\eval\\PyCharm212.evaluation.key\n";
        return 1;
    }

    // 获取文件路径
    const char* filePath = argv[1];

    // 获取当前时间（自1601年1月1日以来的100纳秒间隔）
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    // 转换为自Unix纪元（1970年1月1日）以来的毫秒数
    long long t = ((long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    t = (t - 116444736000000000LL) / 10000LL;

    //t -= 15 * 24 * 3600 * 1000;

    long long negT = ~t;

    // 转换为大端字节序
    long long bigEndianT = toBigEndian(negT);

    // 以二进制方式打开文件，将大端字节序的 negT 写入文件
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        std::cerr << "无法打开文件: " << filePath << std::endl;
        return 1;
    }

    // 将 negT 以大端字节序写入文件
    outFile.write(reinterpret_cast<const char*>(&bigEndianT), sizeof(bigEndianT));
    outFile.close();

    std::cout << "写入成功: " << filePath << std::endl;

    return 0;
}
