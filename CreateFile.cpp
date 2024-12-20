#include <windows.h>
#include <iostream>
#include <string>

void HandleError(const std::string& message) {
    std::cerr << message << " (Error: " << GetLastError() << ")" << std::endl;
    exit(EXIT_FAILURE);
}

int main() {
    const char* fileName = "example.txt"; 

    
    HANDLE hFile = CreateFileA(
        fileName,                
        GENERIC_READ,            
        FILE_SHARE_READ,         
        NULL,                    
        OPEN_EXISTING,           
        FILE_ATTRIBUTE_NORMAL,   
        NULL                     
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        HandleError("Не вдалося відкрити файл");
    }

    
    HANDLE hMapping = CreateFileMappingA(
        hFile,                   
        NULL,                    
        PAGE_READONLY,           
        0,                       
        0,                       
        NULL                     
    );

    if (hMapping == NULL) {
        CloseHandle(hFile);
        HandleError("Не вдалося створити відображення файлу");
    }

    
    LPCVOID pFileView = MapViewOfFile(
        hMapping,                
        FILE_MAP_READ,           
        0,                       
        0,                       
        0                        
    );

    if (pFileView == NULL) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        HandleError("Не вдалося створити представлення файлу у пам'ять");
    }

    
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        UnmapViewOfFile(pFileView);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        HandleError("Не вдалося отримати розмір файлу");
    }

    
    const char* fileData = static_cast<const char*>(pFileView);
    size_t charCount = 0;

    for (DWORD i = 0; i < fileSize; ++i) {
        if (fileData[i] != '\0') {
            ++charCount;
        }
    }

    std::cout << "Кількість символів у файлі: " << charCount << std::endl;

    
    UnmapViewOfFile(pFileView);
    CloseHandle(hMapping);
    CloseHandle(hFile);

    return 0;
}
