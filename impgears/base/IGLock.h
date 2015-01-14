/*
base/Lock.h

Define a lock used for mutual exclusion data access.
*/

#ifndef IMP_LOCK_H
#define IMP_LOCK_H

#include "impBase.hpp"
#include <pthread.h>

IMPGEARS_BEGIN

class Lock
{
	public:

		Lock();
		virtual ~Lock();

		void lock();
		void unlock();

	protected:
	private:

	pthread_mutex_t mtx;
};

IMPGEARS_END

#endif // IGLOCK_H
