#ifndef ___BMP__H___
#define ___BMP__H___

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gl_canvas2d.h"

#define HEADER_SIZE      14 
#define INFOHEADER_SIZE  40 
#define uchar unsigned char

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

typedef struct {
    unsigned short int type;
    unsigned int size;
    unsigned short int reserved1, reserved2;
    unsigned int offset;
} HEADER;

typedef struct {
    unsigned int size;
    int width, height;
    unsigned short int planes;
    unsigned short int bits;
    unsigned int compression;
    unsigned int imagesize;
    int xresolution, yresolution;
    unsigned int ncolours;
    unsigned int impcolours;
} INFOHEADER;

class Bmp
{
private:
    int width, height, imagesize, bytesPerLine, bits;
    unsigned char* data;

    HEADER     header;
    INFOHEADER info;

    void load(const char* fileName) {
        FILE* fp = fopen(fileName, "rb");
        if (fp == NULL) {
            printf("\nErro ao abrir arquivo %s para leitura", fileName);
            return;
        }

        fread(&header.type, sizeof(unsigned short int), 1, fp);
        fread(&header.size, sizeof(unsigned int), 1, fp);
        fread(&header.reserved1, sizeof(unsigned short int), 1, fp);
        fread(&header.reserved2, sizeof(unsigned short int), 1, fp);
        fread(&header.offset, sizeof(unsigned int), 1, fp);

        fread(&info.size, sizeof(unsigned int), 1, fp);
        fread(&info.width, sizeof(int), 1, fp);
        fread(&info.height, sizeof(int), 1, fp);
        fread(&info.planes, sizeof(unsigned short int), 1, fp);
        fread(&info.bits, sizeof(unsigned short int), 1, fp);
        fread(&info.compression, sizeof(unsigned int), 1, fp);
        fread(&info.imagesize, sizeof(unsigned int), 1, fp);
        fread(&info.xresolution, sizeof(int), 1, fp);
        fread(&info.yresolution, sizeof(int), 1, fp);
        fread(&info.ncolours, sizeof(unsigned int), 1, fp);
        fread(&info.impcolours, sizeof(unsigned int), 1, fp);

        width = info.width;
        height = info.height;
        bits = info.bits;
        bytesPerLine = (3 * (width + 1) / 4) * 4;
        imagesize = bytesPerLine * height;

        if (header.type != 19778) {
            printf("\nError: Arquivo BMP invalido");
            exit(0);
        }

        data = new unsigned char[imagesize];
        fseek(fp, header.offset, SEEK_SET);
        fread(data, sizeof(unsigned char), imagesize, fp);
        fclose(fp);

        convertBGRtoRGB(); // Converte automaticamente ao carregar
    }

public:
    Bmp(const char* fileName) {
        width = height = 0;
        data = NULL;
        if (fileName != NULL && strlen(fileName) > 0) {
            load(fileName);
        }
    }

    uchar* getImage() { return data; }
    int getWidth(void) { return width; }
    int getHeight(void) { return height; }

    void convertBGRtoRGB() {
        unsigned char tmp;
        if (data != NULL) {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int idx = y * bytesPerLine + x * 3;
                    tmp = data[idx];
                    data[idx] = data[idx + 2];
                    data[idx + 2] = tmp;
                }
            }
        }
    }


    void resize(int newWidth, int newHeight) {
        if (data == NULL || newWidth <= 0 || newHeight <= 0) {
            printf("\nErro no resize: imagem invalida ou dimensoes invalidas.");
            return;
        }

        if (bits != 24) {
            printf("\nErro no resize: esta classe suporta apenas BMP 24 bits. Bits atuais: %d", bits);
            return;
        }

        int oldWidth = width;
        int oldHeight = height;
        int oldBytesPerLine = bytesPerLine;

        int newBytesPerLine = ((newWidth * 3 + 3) / 4) * 4;
        int newImageSize = newBytesPerLine * newHeight;

        unsigned char* newData = new unsigned char[newImageSize];
        memset(newData, 0, newImageSize);

        for (int y = 0; y < newHeight; y++) {
            for (int x = 0; x < newWidth; x++) {
                int oldX = x * oldWidth / newWidth;
                int oldY = y * oldHeight / newHeight;

                int oldIdx = oldY * oldBytesPerLine + oldX * 3;
                int newIdx = y * newBytesPerLine + x * 3;

                newData[newIdx] = data[oldIdx];
                newData[newIdx + 1] = data[oldIdx + 1];
                newData[newIdx + 2] = data[oldIdx + 2];
            }
        }

        delete[] data;

        data = newData;
        width = newWidth;
        height = newHeight;
        bytesPerLine = newBytesPerLine;
        imagesize = newImageSize;

        info.width = newWidth;
        info.height = newHeight;
        info.imagesize = newImageSize;
        header.size = header.offset + newImageSize;

        //printf("\nResize concluido: %dx%d", width, height);
    }


    // Renderiza a imagem na posição x, y (canto inferior esquerdo)
    void render(int x, int y) {
        if (data == NULL) return;
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                int index = row * bytesPerLine + col * 3;
                CV::color(data[index] / 255.0f, data[index + 1] / 255.0f, data[index + 2] / 255.0f);
                CV::point(x + col, y + row);
            }
        }
    }
};

#endif