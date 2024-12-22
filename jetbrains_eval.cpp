#include <iostream>
#include <fstream>
#include <windows.h>
#include <DbgHelp.h>
#include <Userenv.h>

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
    char filePath[MAX_PATH] = "%appdata%\\JetBrains\\PyCharm2021.2\\eval\\PyCharm212.evaluation.key";
    if (argc >= 2) {
        strcpy_s(filePath, argv[1]);
    }
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        // 处理获取令牌句柄失败的情况，比如打印错误信息
        DWORD error = GetLastError();
        printf("获取用户令牌句柄失败，错误码: %d\n", error);
        return -1;
    }
    // 扩展环境变量字符串，将%appdata%等环境变量替换为实际路径
    if (10 > ExpandEnvironmentStringsForUserA(hToken, filePath, filePath, MAX_PATH))
    {
        DWORD error = GetLastError();
        printf("获取用户环境变量失败，错误码: %d\n", error);
        return -2;
    }
    
    if (!MakeSureDirectoryPathExists(filePath))
    {
        DWORD error = GetLastError();
        printf("新建目标文件夹错误，错误码: %d\n", error);
        return -3;
    }


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
        DWORD error = GetLastError();
        printf("无法打开文件: %s，错误码：%d\n", filePath, error);
        return -4;
    }

    // 将 negT 以大端字节序写入文件
    outFile.write(reinterpret_cast<const char*>(&bigEndianT), sizeof(bigEndianT));
    outFile.close();

    printf("写入成功: %s\n", filePath);

    return 0;
}
