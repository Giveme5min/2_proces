// proces.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <windows.h>
#include <conio.h>
#include "MutexSection.h"
HANDLE imutex;
HANDLE hSharedMemory;
MutexCritSection cs;

#define SHARED_MEMORY_NAME L"Shared"
#define SHARED_MEMORY_SIZE 100
#define CyclesRW 10// количество вызовов

int main()
{
    std::cout  << long long(SHARED_MEMORY_SIZE);
    cs.InitializeCriticalSection(L"SyncMutex");

    // сформируем общую память
    if ((hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEMORY_SIZE, SHARED_MEMORY_NAME)) == NULL)
    {
        std::cout << "Error to Mapping file!!" << std::endl;
        return -1;
    }

    imutex = CreateMutex(NULL, TRUE, L"Instance");
    // первый процесс (записывает в разделяемую память)

    if (GetLastError() != ERROR_ALREADY_EXISTS)//проверяет что мьютекс создался 
    {
        // создадим второй процесс
        STARTUPINFO si = { NULL };
        PROCESS_INFORMATION pi = { NULL };
        TCHAR commandArgs[] = TEXT("C:\\work_win\\VS_project\\proces\\Debug\\proces.exe");


        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, L"MyEvent");

        if (!CreateProcess(NULL, commandArgs, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        {
            std::cout << "Cannot create second process!" << std::endl;
            return -1;
        }

        TCHAR message[100];
        
        LPCTSTR shared_data_poiter = (LPCTSTR)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);
        for (int i = 0; i < CyclesRW; i++)
        {
            cs.Enter();
            int numb = rand();
            wsprintf(message, L"%d", numb);
            std::wcout << "First process write: " << message << std::endl;
            CopyMemory((PVOID)shared_data_poiter, message, 100 * sizeof(wchar_t));
             Sleep(500);
            SetEvent(hEvent);
            cs.Leave();
           
        }
        UnmapViewOfFile(shared_data_poiter);
    }
    // второй процесс (читает из разделяемой памяти)
    else
    {
        TCHAR message[100];
        HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, L"MyEvent");
        if (hEvent == NULL)
        {
            std::cout << "Event doesn't opened!" << std::endl;
            return 0;
        }
        LPCTSTR shared_data_poiter = (LPCTSTR)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);
        for (int i = 0; i < CyclesRW; i++)
        {
            WaitForSingleObject(hEvent, INFINITE);
            cs.Enter();
            ResetEvent(hEvent);
            int numb = 0;
            wsprintf(message, L"%d", numb);
            std::cout << "Second proress read: ";
            std::wcout << shared_data_poiter << std::endl;
            cs.Leave();
        }
        UnmapViewOfFile(shared_data_poiter);
    }
    _getch();
    CloseHandle(hSharedMemory);
    CloseHandle(imutex);
    return 0;
}


