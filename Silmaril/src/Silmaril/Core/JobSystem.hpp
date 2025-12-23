#pragma once

namespace Silmaril {

    struct JobDispatchArgs
    {
        u32 jobIndex;
        u32 groupIndex;
    };

    class JobSystem
    {
    public:
        static void Init(u32 worker = std::thread::hardware_concurrency() - 1);
        static void Shutdown();

        static void Execute(const std::function<void()>& job);
        static void Dispatch(u32 jobCount, u32 groupSize, const std::function<void(JobDispatchArgs)>& job);

        static void Sync();

    private:
        static void WorkerLoop();

    private:
        inline static std::queue<std::function<void()>> s_JobQueue;
        inline static std::mutex s_QueueMutex;
        inline static std::condition_variable s_WakeCondition;
        inline static std::vector<std::thread> s_WorkerThreads;

        inline static std::atomic<bool> s_Running { false };
        inline static std::atomic<u32> s_JobsInProgress { 0 };
        inline static std::condition_variable s_WaitCondition;
    };

}
