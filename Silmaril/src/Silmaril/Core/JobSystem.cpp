#include "JobSystem.hpp"

#include "Logger.hpp"

namespace Silmaril {

    void JobSystem::Init(u32 worker)
    {
        if (s_Running) return;
        s_Running = true;

        u32 cores = std::min(std::max(worker, 1u), std::thread::hardware_concurrency() - 1);
        LOG_INFO("Initializing JobSystem with {} workers", cores);

        for (u32 i = 0; i < cores; ++i) {
            s_WorkerThreads.emplace_back(WorkerLoop);
        }
    }

    void JobSystem::Shutdown()
    {
        if (!s_Running) return;
        s_Running = false;

        s_WakeCondition.notify_all();

        for (auto& thread : s_WorkerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        s_WorkerThreads.clear();
    }

    void JobSystem::Execute(const std::function<void()>& job)
    {
        {
            std::scoped_lock<std::mutex> lock(s_QueueMutex);
            s_JobsInProgress++;
            s_JobQueue.push(job);
        }

        s_WakeCondition.notify_one();
    }

    void JobSystem::Dispatch(u32 jobCount, u32 groupSize, const std::function<void(JobDispatchArgs)>& job)
    {
        if (jobCount == 0 || groupSize ==  0) return;

        u32 groupCount = (jobCount - 1) / groupSize + 1;

        for (u32 groupIndex = 0; groupIndex < groupCount; ++groupIndex) {
            Execute([job, groupIndex, groupSize, jobCount]() {
                u32 groupJobOffset = groupIndex * groupSize;
                u32 groupJobEnd = std::min(groupJobOffset + groupSize, jobCount);

                JobDispatchArgs args;
                args.groupIndex = groupIndex;

                for (u32 i = groupJobOffset; i < groupJobEnd; ++i) {
                    args.jobIndex = i;
                    job(args);
                }
            });
        }
    }

    void JobSystem::Sync()
    {
        std::unique_lock<std::mutex> lock(s_QueueMutex);
        s_WaitCondition.wait(lock, []() { return s_JobsInProgress.load() == 0; });
    }

    void JobSystem::WorkerLoop()
    {
        while (true) {
            std::function<void()> job;

            {
                std::unique_lock<std::mutex> lock(s_QueueMutex);
                s_WakeCondition.wait(lock, []() { return !s_JobQueue.empty() || !s_Running; });

                if (!s_Running && s_JobQueue.empty()) {
                    return;
                }

                if (!s_JobQueue.empty()) {
                    job = std::move(s_JobQueue.front());
                    s_JobQueue.pop();
                }
            }

            if (job) {
                job();

                if (s_JobsInProgress.fetch_sub(1) == 1) {
                    std::scoped_lock<std::mutex> lock(s_QueueMutex);
                    s_WaitCondition.notify_all();
                }
            }
        }
    }

}
