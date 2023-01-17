#pragma once


#include <array>
#include <string>
#include <iostream>
#include <chrono>

// required headers
#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <shared_mutex>

#define BREAK __debugbreak();

#define ALWAYS_ASSERT(expression, ...) \
	if (!(expression)) \
	{ \
		BREAK; \
		std::abort(); \
	} else {}
	// else block is needed so external else doesn't relate to the if block in the macro in this case:
	// if (condition) ALWAYS_ASSERT(expression);
	// else { ... }

#ifdef NDEBUG
#define DEV_ASSERT(...)
#else
#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);
#endif

namespace Engine
{



	struct ParallelExecutor
	{
		static const uint32_t MAX_THREADS; // 100% CPU occupation, it may cause OS hitches.
		// No point to have more threads than the number of CPU logical cores.

		static const uint32_t HALF_THREADS; // 50-100% CPU occupation

		using Func = std::function<void(uint32_t, uint32_t)>; // (threadIndex, taskIndex)

		ParallelExecutor(uint32_t numThreads);

		~ParallelExecutor();

		uint32_t numThreads() const { return m_threads.size(); }
		bool isWorking() const { return m_finishedThreadNum < m_threads.size(); }

		void wait();

		// Executes a function in parallel blocking the caller thread.
		void execute(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch);

		// Executes a function in parallel asynchronously.
		void executeAsync(const Func& func, uint32_t numTasks, uint32_t tasksPerBatch);

	protected:
		void awake();

		void workLoop(uint32_t threadIndex);

		bool m_isLooping;

		std::atomic<uint32_t> m_finishedThreadNum;
		std::atomic<uint32_t> m_completedBatchNum;
		std::function<void(uint32_t)> m_executeTasks;

		std::shared_mutex m_mutex;
		std::condition_variable_any m_waitCV;
		std::condition_variable_any m_workCV;

		std::vector<std::thread> m_threads;
	};

	

}