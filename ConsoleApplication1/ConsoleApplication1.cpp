#include <windows.h>
#include <iostream>
#include <string>



int main()
{
    bool wm_copydataMode = true;

    std::string response;

    std::cout << "Enter the operating mode [1:WM_COPYDATA|2:mapped file]:" << std::endl;
    do
    {
        std::getline(std::cin, response);

        if (response == "2") {
            wm_copydataMode = false;
            break;
        }
        else if (response == "1") break;
        std::cout << "Unexpected input, please try again [1:WM_COPYDATA|2:mapped file]:";

    } while (!std::cin.fail());


    if (wm_copydataMode) {
        std::cout << "Welcome to MessageSender (through messages)!" << std::endl;

        COPYDATASTRUCT messageStruct;
        HWND handle;

        const char* message = "This message was delivered to you through WM_COPYDATA";

        std::cout << "Confirm that receiver is ready [Enter]:" << std::endl;
        std::cin.get();

        handle = FindWindow(L"MessageEater", NULL);

        if (!handle) return EXIT_FAILURE;

        messageStruct.dwData = 0;
        messageStruct.cbData = strlen(message);
        messageStruct.lpData = (PVOID)message;

        SendMessage(handle, WM_COPYDATA, NULL, (LPARAM)&messageStruct);
        SendMessage(handle, WM_QUIT, NULL, (LPARAM)&messageStruct);

        return EXIT_SUCCESS;
    }
    else {
        std::cout << "Welcome to MessageSender (through a mapped file)!" << std::endl;

        const char *message = "This message was delivered to you by a mapped file";

        LPCTSTR lpMapName = L"sharedMappedFile";
        LPVOID mem;
        HANDLE mapFile;
        
        mapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 100, lpMapName);
        if (mapFile == NULL) {
            std::cout << "Couldn't create file mapping object!" << std::endl;
            return EXIT_FAILURE;
        }

        mem = MapViewOfFile(mapFile, FILE_MAP_WRITE, 0, 0, 0);
        if (mem == NULL) {
            std::cout << "Couldn't map a view of a file!" << std::endl;
            return EXIT_FAILURE;
        }
        CopyMemory(mem, message, strlen(message)*sizeof(char));

        std::cout << "Confirm message reception [Enter]:" << std::endl;
        std::cin.get();

        UnmapViewOfFile(mem);
        CloseHandle(mapFile);

        return EXIT_SUCCESS;
    }
}