/*
 * \file: Runnable.h
 * \brief: Created by hushouguo at 09:45:05 Apr 04 2019
 */
 
#ifndef __RUNNABLE_H__
#define __RUNNABLE_H__

BEGIN_NAMESPACE_TNODE {
	class Runnable {
		public:
			virtual void run() = 0;

		public:
			void schedule() {
				if (!this->_isrunning) {
					this->_isrunning = true;
					sThreadPool.add([](Runnable* runnable) {
							Debug.cout("schedule: %p run", this);
							runnable->run();
							runnable->_isrunning = false;
							Debug.cout("schedule: %p run over", this);
							}, this);
				}
			}

		private:
			bool _isrunning = false;
	};
}

#endif
