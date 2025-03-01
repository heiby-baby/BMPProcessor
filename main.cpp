#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <Windows.h>
using namespace std;

// #ifdef _WIN32
//     #include <windows.h>
// #else
//     // Ручное определение структур BMP для Linux
//     #pragma pack(push, 1)
//     struct BITMAPFILEHEADER {
//         uint16_t bfType;
//         uint32_t bfSize;
//         uint16_t bfReserved1;
//         uint16_t bfReserved2;
//         uint32_t bfOffBits;
//     };
//     struct BITMAPINFOHEADER {
//         uint32_t biSize;
//         int32_t  biWidth;
//         int32_t  biHeight;
//         uint16_t biPlanes;
//         uint16_t biBitCount;
//         uint32_t biCompression;
//         uint32_t biSizeImage;
//         int32_t  biXPelsPerMeter;
//         int32_t  biYPelsPerMeter;
//         uint32_t biClrUsed;
//         uint32_t biClrImportant;
//     };
//     #pragma pack(pop)
//     // Определение константы для Linux
//     #define BI_RGB 0
// #endif


// https://learn.microsoft.com/ru-ru/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
// https://learn.microsoft.com/ru-ru/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader




class BMPProcessor {
private:
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    vector<unsigned char> pixelData;
    int width;
    int height;
    int bitsPerPixel;
    int bytesPerPixel;
    int paddingSize;

public:
    BMPProcessor() : width(0), height(0), bitsPerPixel(0), bytesPerPixel(0), paddingSize(0) {}

    bool Read(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            cerr << "Cannot open file: " << filename << endl;
            return false;
        }

        char fileHeaderBuffer[sizeof(BITMAPFILEHEADER)];
        file.read(fileHeaderBuffer, sizeof(BITMAPFILEHEADER));
        memcpy(&bmfh, fileHeaderBuffer, sizeof(BITMAPFILEHEADER));

        if (bmfh.bfType != 0x4D42) {
            cerr << "Not a BMP file" << endl;
            return false;
        }

        char infoHeaderBuffer[sizeof(BITMAPINFOHEADER)];
        file.read(infoHeaderBuffer, sizeof(BITMAPINFOHEADER));
        memcpy(&bmih, infoHeaderBuffer, sizeof(BITMAPINFOHEADER));

        if (bmih.biSize != sizeof(BITMAPINFOHEADER)) {
            cerr << "Unsupported BMP format" << endl;
            return false;
        }

        if(bmih.biPlanes != 1) {
            cerr << "Unsupported BMP format" << endl;
            return false;
        }

        bitsPerPixel = bmih.biBitCount;
        if (bitsPerPixel != 24 && bitsPerPixel != 32) {
            cerr << "Only 24 or 32 bit BMP supported" << endl;
            return false;
        }


        bytesPerPixel = bitsPerPixel / 8;
        width = bmih.biWidth;
        height = bmih.biHeight;

        if (height <= 0) {
            cerr << "Negative height not supported" << endl;
            return false;
        }

        if (bmih.biCompression != BI_RGB) {
            cerr << "Compressed BMP not supported" << endl;
            return false;
        }

        paddingSize = (bitsPerPixel == 24) ? (4 - (width * 3) % 4) % 4 : 0;

        file.seekg(bmfh.bfOffBits, ios::beg);

        pixelData.resize(width * height * bytesPerPixel);

        for (int y = 0; y < height; ++y) {
            file.read(
                static_cast<char*>(static_cast<void*>(&pixelData[y * width * bytesPerPixel])), 
                width * bytesPerPixel
            );
            if (bitsPerPixel == 24) {
                file.seekg(paddingSize, ios::cur);
            }
        }

        return true;
    }

    void Display() const {
        for (int y = height - 1; y >= 0; --y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * bytesPerPixel;
                unsigned char b = pixelData[index];
                unsigned char g = pixelData[index + 1];
                unsigned char r = pixelData[index + 2];

                if (r == 0 && g == 0 && b == 0) {
                    cout << ' ';
                } else if (r == 255 && g == 255 && b == 255) {
                    cout << '@';
                } else {
                    cout << 'X';
                }
            }
            cout << endl;
        }
    }

    void DrawLine(int x1, int y1, int x2, int y2) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = x1 < x2 ? 1 : -1;
        int sy = y1 < y2 ? 1 : -1;
        int err = dx - dy;

        while (true) {
            SetPixel(x1, y1, 0, 0, 0);

            if (x1 == x2 && y1 == y2) break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    }

    bool Save(const string& filename) const {
        ofstream file(filename, ios::binary);
        if (!file) {
            cerr << "Cannot create file: " << filename << endl;
            return false;
        }

        file.write(
            static_cast<const char*>(static_cast<const void*>(&bmfh)), 
            sizeof(bmfh)
        );
        file.write(
            static_cast<const char*>(static_cast<const void*>(&bmih)), 
            sizeof(bmih)
        );

        for (int y = 0; y < height; ++y) {
            const char* row = static_cast<const char*>(
                static_cast<const void*>(&pixelData[y * width * bytesPerPixel])
            );
            file.write(row, width * bytesPerPixel);

            if (bitsPerPixel == 24) {
                static const char padding[3] = {0};
                file.write(padding, paddingSize);
            }
        }
        return true;
    }


    int GetWidth() const { return width; }
    int GetHeight() const { return height; }


    ~BMPProcessor()
    {
        
    }
private:
    void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;

        int index = (y * width + x) * bytesPerPixel;
        pixelData[index] = b;
        pixelData[index + 1] = g;
        pixelData[index + 2] = r;
    }
};

int main() {
    BMPProcessor processor;

    string inputFile;
    cout << "Enter input BMP file name: ";
    cin >> inputFile;

    if (!processor.Read(inputFile)) {
        return 1;
    }

    cout << "Original image:" << endl;
    processor.Display();

    int width = processor.GetWidth();
    int height = processor.GetHeight();

    processor.DrawLine(0, height-1, width-1, 0);
    processor.DrawLine(0, 0, width-1, height-1);

    cout << "Modified image:" << endl;
    processor.Display();

    string outputFile;
    cout << "Enter output BMP file name: ";
    cin >> outputFile;

    if (!processor.Save(outputFile)) {
        return 1;
    }

    return 0;
}