#include "Lock.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
Lock::Lock()
{
	mtx = PTHREAD_MUTEX_INITIALIZER;
}

//--------------------------------------------------------------
Lock::~Lock()
{
	unlock();
}

//--------------------------------------------------------------
void Lock::lock()
{
	pthread_mutex_lock(&mtx);
}

//--------------------------------------------------------------
void Lock::unlock()
{
	pthread_mutex_unlock(&mtx);
}

IMPGEARS_END
