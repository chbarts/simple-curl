#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#define _GNU_SOURCE
#include <getopt.h>

extern int optind;

static void version(void)
{
    puts("simple-curl2 version 0.1");
    puts("Copyright 2012, Chris Barts");
    puts("Licensed under the GNU General Public License, version 3.0 or, at your option, any later version.");
    puts("This software has NO WARRANTY, even for USABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
}

static void help(char name[])
{
    printf("%s [options...] url\n", name);
    puts("Options:");
    puts("--out file | -o file    Output data to file");
    puts("--dump file | -d file   Dump header to file");
    puts("--help | -h             Print this help");
    puts("--version | -v          Print version information");
}

int main(int argc, char *argv[])
{
    FILE *ofh = stdout, *dfh = stderr;
    CURL *curl;
    char *url, *pnam, *ofn = NULL, *dfn = NULL;
    char errbuf[CURL_ERROR_SIZE];
    int c, lind, res;
    struct option longopts[] = {
        {"out", 1, 0, 0},
        {"dump", 1, 0, 0},
        {"help", 0, 0, 0},
        {"version", 0, 0, 0},
        {0, 0, 0, 0}
    };

    pnam = argv[0];

    if (argc == 1) {
        printf("%s: [options...] url\n", pnam);
        exit(EXIT_SUCCESS);
    }

    while ((c = getopt_long(argc, argv, "o:d:hv", longopts, &lind)) != -1) {
        switch (c) {
        case 0:
            switch (lind) {
            case 0:
                ofn = optarg;
                break;
            case 1:
                dfn = optarg;
                break;
            case 2:
                help(pnam);
                exit(EXIT_SUCCESS);
            case 3:
                version();
                exit(EXIT_SUCCESS);
            default:
                help(pnam);
                exit(EXIT_FAILURE);
            }

            break;
        case 'o':
            ofn = optarg;
            break;
        case 'd':
            dfn = optarg;
            break;
        case 'h':
            help(pnam);
            exit(EXIT_SUCCESS);
        case 'v':
            version();
            exit(EXIT_SUCCESS);
        default:
            help(pnam);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "%s: URL required\n", pnam);
        exit(EXIT_FAILURE);
    }

    if (!(curl = curl_easy_init())) {
        fprintf(stderr, "simple-curl: curl_easy_init() failed\n");
        exit(EXIT_FAILURE);
    }

    url = argv[optind];
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(curl, CURLOPT_URL, url);

    if (ofn) {
        if (!(ofh = fopen(ofn, "ab"))) {
            perror("simple-curl2: failed to open output file");
            curl_easy_cleanup(curl);
            exit(EXIT_FAILURE);
        }

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ofh);
    }

    if (dfn) {
        if (!(dfh = fopen(dfn, "ab"))) {
            perror("simple-curl2: failed to open dump file");
            curl_easy_cleanup(curl);
            if (ofh != stdout)
                fclose(ofh);
            exit(EXIT_FAILURE);
        }
    }

    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, dfh);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (ofh != stdout)
        fclose(ofh);
    if (dfh != stderr)
        fclose(dfh);

    if (res != 0) {
        fprintf(stderr, "simple-curl2: curl error: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
