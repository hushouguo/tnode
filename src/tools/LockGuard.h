/*
 * \file: LockGuard.h
 * \brief: Created by hushouguo at 11:16:43 Apr 08 2019
 */
 
#ifndef __LOCKGUARD_H__
#define __LOCKGUARD_H__

BEGIN_NAMESPACE_TNODE {
	class lock_guard {
		std::atomic_flag* _flag = nullptr;
		public:
			void lock() {
				while (this->trylock());
			}

			bool trylock() {
				return !this->_flag->test_and_set();
			}

			void unlock() {
				this->_flag->clear();
			}

		public:
			lock_guard(std::atomic_flag* flag) : _flag(flag) {
			}

			~lock_guard() {
				this->unlock();
			}
	};
}

#endif
