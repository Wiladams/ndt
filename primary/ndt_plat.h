#pragma once
#include "definitions.h"

namespace ndt
{
	class Semaphore
	{
	public:
		Semaphore(const int initial = 1);
		~Semaphore();

		// Acquire semaphore
		const bool Lock();
		const bool TryLock(const uint64_t milliseconds = 1, const bool exit_lock = true);

		// Release semaphore
		const bool Unlock(const bool reschedule = false);

		inline const bool isLocked() const { return fAvailable <= 0; }

	private:
		std::atomic<int32_t> fAvailable;
#if defined WIN32
		HANDLE fSemaphore;
#endif

	};
	
	
	class Spinlock
	{
		std::atomic_flag fLocked = ATOMIC_FLAG_INIT;
		bool fIsLocked = false;

		inline void idle()
		{
#if USE_STD_THREAD_YIELD
				std::this_thread::yield();
#else
				_mm_pause();
#endif

		}
		
	public:
		const bool Lock() {
			while (fLocked.test_and_set(std::memory_order_acquire)) {
				idle();
			}
			fIsLocked = true;
			return true;
		}
		
		const bool	TryLock(const int num_cycles = 256)
		{
			if (fIsLocked) 
				return false;

			int c = 0;
			while (fLocked.test_and_set(std::memory_order_acquire)) {


				if (++c >= num_cycles) {
					return false;
				}

				idle();
			}
			fIsLocked = true;
			return true;
		}
			
		const bool	Unlock()
		{
			fLocked.clear(std::memory_order_release);
			fIsLocked = false;
			return true;
		}

		inline const bool	isLocked() const { return fIsLocked; }

		inline void	lock() { Lock(); }
		inline void unlock() { Unlock(); }
	};
	
}