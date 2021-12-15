#include <windows.h>
#include <iostream>
#include <string>

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COPYDATA) {
        COPYDATASTRUCT messageStruct = *((PCOPYDATASTRUCT)lParam);
        char* message = new char[messageStruct.cbData];
        
        message = (char*)messageStruct.lpData;
        std::cout << message << std::endl;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


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
        std::cout << "Welcome to MessageReceiver (through messages)!" << std::endl;

        std::cout << "Confirm readiness to receive the message [Enter]:" << std::endl;
        std::cin.get();

        WNDCLASS windowClass = {};
        windowClass.lpfnWndProc = WindowProcedure;
        LPCWSTR windowClassName = L"MessageEater";
        windowClass.lpszClassName = windowClassName;

        if (!RegisterClass(&windowClass)) {
            std::cout << "Failed to register window class" << std::endl;
            return EXIT_FAILURE;
        }

        HWND messageWindow = CreateWindow(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
        if (!messageWindow) {
            std::cout << "Failed to create message-only window" << std::endl;
            return EXIT_FAILURE;
        }
        ChangeWindowMessageFilterEx(messageWindow, WM_COPYDATA, MSGFLT_ALLOW, 0);
        ChangeWindowMessageFilterEx(messageWindow, WM_QUIT, MSGFLT_ALLOW, 0);

        MSG msg;
        while (GetMessage(&msg, 0, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return EXIT_SUCCESS;
    }
    else {
        std::cout << "Welcome to MessageReceiver (through a mapped file)!" << std::endl;

        LPCWSTR lpMapName = L"sharedMappedFile";
        char *mem;
        HANDLE mapFile;

        std::cout << "Confirm readiness to receive the message [Enter]:" << std::endl;
        std::cin.get();

        mapFile = OpenFileMapping(FILE_MAP_READ, false, lpMapName);
        if (mapFile == NULL) {
            std::cout << "Couldn't create file mapping object!" << std::endl;
            return EXIT_FAILURE;
        }

        mem = (char*)MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, 0);
        if (mem == NULL) {
            std::cout << "Couldn't map a view of a file!" << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << mem << std::endl;

        UnmapViewOfFile(mem);
        CloseHandle(mapFile);

        return EXIT_SUCCESS;
    }
}
