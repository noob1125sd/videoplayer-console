#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

FILE* pipe;

void play() {
    HWND console = GetConsoleWindow();
    HDC hdc = GetDC(console);
    HDC mem = CreateCompatibleDC(hdc);
    

    int video_w = 1280, video_h = 720;
    int video_frame_size = video_w * video_h * 3;
    unsigned char* video_frame = malloc(video_frame_size);
    

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = video_w;
    bmi.bmiHeader.biHeight = -video_h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    HBITMAP bmp = CreateCompatibleBitmap(hdc, video_w, video_h);
    SelectObject(mem, bmp);
    int frames =0;
    RECT rect;
    int console_w;
    int console_h;
    while (fread(video_frame, 1, video_frame_size, pipe) == video_frame_size) {

        if (frames=10){
        GetClientRect(console, &rect);
        console_w = rect.right - rect.left;
        console_h = rect.bottom - rect.top;
        frames=0;
        }

        SetDIBitsToDevice(mem, 0, 0, video_w, video_h, 0, 0, 0, video_h, 
                         video_frame, &bmi, DIB_RGB_COLORS);
        

        StretchBlt(hdc, 0, 0, console_w, console_h, 
                  mem, 0, 0, video_w, video_h, SRCCOPY);
        frames++;
        Sleep(1);
    }
    
    free(video_frame);
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(console, hdc);
    _pclose(pipe);
}

void aviinit(const char* filename) {
    char command[256];
    snprintf(command, sizeof(command), 
             "ffmpeg -i \"%s\" -vf scale=1280:720 -r 60 -f rawvideo -pix_fmt bgr24 -loglevel quiet -fflags nobuffer -flags low_delay -avioflags direct -threads 4 -", 
             filename);
    pipe = _popen(command, "rb");
}

void hide_cursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}
int main() {
    hide_cursor();
    char filename[100];
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;
    aviinit(filename);
    play();
    return 0;

}
