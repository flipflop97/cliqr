#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qrencode.h>

const int version_major = 1;
const int version_minor = 1;
const int version_build = 0;

void printhelp(char* progname) {
    printf("Usage: %s [Options] [String]\n\n", progname);
    printf("String:\n");
    printf("    Any string in UTF-8 format,\n");
    printf("    if left empty there will be read from standard input.\n\n");
    printf("Options:\n");
    printf("   -large        Print the qr code in a bigger size\n");
    printf("  --help         Display this help message\n");
    printf("  --version      Display program information\n");
}

void printversion() {
    printf("qr version %i.%i.%i\n", version_major, version_minor, version_build);
    printf("Made by Philip Goto\n");
}

void print_qr_large(QRcode* qr) {
    printf("\033[107m");
    for(int col = -1; col <= qr->width; col++)
        printf("  ");
    printf("\033[0m\n");

    for(int row = 0; row < qr->width; row++) {
        printf("\033[107m  ");
        for(int col = 0; col < qr->width; col++) {
            if(*(qr->data + row*qr->width + col) & 1)
                printf("\033[40m  ");
            else
                printf("\033[107m  ");
        }
        printf("\033[107m  \033[0m\n");
    }

    printf("\033[107m");
    for(int col = -1; col <= qr->width; col++)
        printf("  ");
    printf("\033[0m\n");
}

void print_qr_small(QRcode* qr) {
    for(int row = -1; row < qr->width; row += 2) {
        printf("\033[107m\033[97m▀");

        for(int col = 0; col < qr->width; col++) {
            if(row == -1 || row == qr->width)
                printf("\033[97m");
            else if(*(qr->data + row*qr->width + col) & 1)
                printf("\033[30m");
            else
                printf("\033[97m");

            if(row + 1 == qr->width)
                printf("\033[107m");
            else if(*(qr->data + (row + 1)*qr->width + col) & 1)
                printf("\033[40m");
            else
                printf("\033[107m");

            printf("▀");
        }

        printf("\033[107m\033[97m▀\033[0m\n");
    }

    printf("\033[0m\033[97m");
    for(int col = -1; col <= qr->width; col++)
        printf("▀");
    printf("\033[0m\n");
}

int main(int argc, char *argv[]) {
    char* input = NULL;
    int large = 0;

    for(int argn = 1; argn < argc; argn++) {
        if(!strcmp(argv[argn], "--help")) {
            printhelp(argv[0]);
            return(0);
        }
        else if(!strcmp(argv[argn], "--version")) {
            printversion();
            return(0);
        }
        if(!strcmp(argv[argn], "-large")) {
            large = 1;
        }
        else if(argn == argc - 1) {
            input = argv[argc - 1];
        }
        else {
            fprintf(stderr, "Unknown argument \"%s\"\n\n", argv[argn]);
            printhelp(argv[0]);
            return(1);
        }
    }

    size_t bufsize = 256;
    char* buf = malloc(bufsize);

    if(input == NULL) {
        off_t i = 0;
        while((buf[i++] = getc(stdin)) != EOF) {
            if(i == bufsize) {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
                if(!buf) {
                    fprintf(stderr, "Input is too long\n");
                    return(1);
                }
            }
        }

        buf[i-1] = 0;
        input = buf;
    }

    if(*input == '\0') {
        fprintf(stderr, "No input received\n");
        return(1);
    }
    QRcode* qr = QRcode_encodeString8bit(input, 0, QR_ECLEVEL_L);
    if(qr != NULL)
        if(large)
            print_qr_large(qr);
        else
            print_qr_small(qr);
    else {
        fprintf(stderr, "Input is too long\n");
        return(1);
    }

    QRcode_free(qr);
    free(buf);

    return(0);
}
