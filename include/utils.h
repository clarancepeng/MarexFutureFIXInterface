#ifndef UTILS_H_
#define UTILS_H_
#include <pthread.h>
#include <spdlog/spdlog.h>
#include "fix/fix_utils.h"
namespace Utils {
    
inline void cpuAffinity(pthread_t thread_id, unsigned int cpu_on)
{
#ifdef __linux__
    if (cpu_on >= 1 && cpu_on <= std::thread::hardware_concurrency())
    {
        cpu_set_t cpu_set;
        int cpu = cpu_on;
        CPU_ZERO(&cpu_set);
        CPU_SET(cpu, &cpu_set);
        pthread_setaffinity_np(thread_id, sizeof(cpu_set), &cpu_set);
        spdlog::info("This thread({}) has bound to cpu {}.", (unsigned long int)thread_id, cpu);
    }
    else
    {
        spdlog::info("The thread({}) didn't bind to the cpu!", (unsigned long int)thread_id);
    }
#else
    spdlog::info("thread_id = {}, cpu_on = {}", (unsigned long int)thread_id, cpu_on);
#endif // __linux__
}

inline void cpuAffinity(unsigned int cpu_on)
{
#ifdef __linux__
    if (cpu_on >= 1)
    {
        cpu_set_t mask;
        int status;

        CPU_ZERO(&mask);
        CPU_SET(cpu_on, &mask);
        status = sched_setaffinity(0, sizeof(mask), &mask);
        if (status != 0)
        {
            spdlog::info("Could not cpu affinity for {}", cpu_on);
        }
    }
#else 
    spdlog::info("cpu_on = {}", cpu_on);
#endif // __linux__
}
};

#endif // UTILS_H_