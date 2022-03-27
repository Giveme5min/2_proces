//#include "MutexSection.h"

struct MutexCritSection {
    MutexCritSection();
    ~MutexCritSection();
    void InitializeCriticalSection(LPCWSTR name);
    void Enter();
    void Leave();

    HANDLE smutex;
};
MutexCritSection::MutexCritSection(void)
{
}



void MutexCritSection::InitializeCriticalSection(LPCWSTR name)
{
    smutex = CreateMutex(NULL, FALSE, name);
}

void  MutexCritSection::Enter()
{
    WaitForSingleObject(smutex, INFINITE);
}

void MutexCritSection::Leave()
{
    ReleaseMutex(smutex);
}

MutexCritSection::~MutexCritSection(void)
{
    CloseHandle(smutex);
}

