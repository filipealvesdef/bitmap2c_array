#include <stdio.h>
#include <stdlib.h>

int getBytes(FILE* fp, int bytes) {
    int size = 0, i;

    for (i = 0; i < bytes; i++)
        size |= (fgetc(fp) << (i << 3));

    return size;
}

int main() {

    char imageClassFile[50] = "image.h";
    char inName[50] = "spritesheet-example.bmp";
    char outName[50] = "spritesheet-example-out.c";
    char tmp[50];

    FILE* in = fopen(inName, "rb");

    int width, height, size, bitsPerPixel, startOffset;
    int i, j;

    printf("HEADER Windows 16 bits R5G6B5\n\n");
    printf("signature: %c%c\n", fgetc(in), fgetc(in));

    size = getBytes(in, 4);
    printf("Size: %d bytes\n", size);

    printf("Reserved: %d\n", getBytes(in, 4));

    startOffset = getBytes(in, 4);
    printf("Start offset: %d\n", startOffset);
    printf("size of bitmap core header: %d\n", getBytes(in, 4));

    width = getBytes(in, 4);
    printf("Image width: %d pixels\n", width);

    height = getBytes(in, 4);
    printf("Image height: %d pixels\n", height);

    printf("number of planes: %d\n", getBytes(in, 2));

    bitsPerPixel = getBytes(in, 2);
    printf("bits per pixel: %d\n\n", bitsPerPixel);

    FILE* out = fopen(outName, "wt");

    sprintf(tmp, "%c%s%c", 34, imageClassFile, 34);
    fprintf(out, "#include %s;\n\nshort int %s[%d][%d] = {\n\t{ ", tmp, outName, height, width);

    for (i = height - 1; i >= 0; i--) {
        fseek(in, startOffset + ((width << 1) * i), SEEK_SET);

        for (j = 0; j < width; j++) {
            fprintf(out, "0x%04x", getBytes(in, 2));

            if (j == width - 1)
                break;

            fprintf(out, ", ");
        }

        fprintf(out, " }");

        if (i != 0)
            fprintf(out, ",\n\t{ ");
    }

    fprintf(out, "\n};\n\n");
    fprintf(out, "Image* create_%s() {\n\treturn createImage(%s, %d, %d);\n}", outName, outName, width, height);

    /*fprintf(out, "Image %s;\n%s.width = %d;\n%s.height = %d;\n\n", outName, outName, width, outName, height);
    fprintf(out, "%s.pixels = (short int**) malloc(sizeof(short int*) * %d);\n\n", outName, height);
    fprintf(out, "int i;\nfor (i = 0; i < %d; i++)\n\t", height);
    fprintf(out, "%s.pixels[i] = (short int*) malloc(sizeof(short int) * %d);\n\n", outName, width);

    for (i = height - 1; i >= 0; i--) {
        fseek(in, startOffset + ((width << 1) * i), SEEK_SET);

        for (j = 0; j < width; j++)
            fprintf(out, "%s.pixels[%d][%d] = 0x%04x;\n", outName, (height - 1) - i, j, getBytes(in, 2));
    }*/

    printf("spritesheet file generated successfully!");

    fclose(out);
    fclose(in);
}
