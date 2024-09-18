#include "src/threadPool.h"
#include "src/server.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {

    Server server;

    server.eventListen();
    
    server.eventLoop();

    // auto start = std::chrono::system_clock::now();
    // ThreadPool<int>* th = new ThreadPool<int>(4);

    // for (int i = 0; i < 1000000; i++) {
    //     int* a = new int(6);
    //     th->addTask(a);
    // }

    // while (1) {
    //     if (con == 1000000)
    //         break;
    // }
    // th->shotdown();
    // delete th;

    // auto end = std::chrono::system_clock::now();
    // std::chrono::duration<double> diff = end-start;
    // // 计算毫秒时间差并输出
    // // 如果要求其他时间单位可以修改 std::chrono::milliseconds 为其他类型
    // // 比如std::chrono::seconds
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << "Time to fill and iterate a vector of " << duration.count()  << " ms\n";

    return 0;
}