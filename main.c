/*
NAME    : SWARAJ A S
DATE    : 07/03/2026
PROJECT : MP3 Tag Reader and Editor (ID3v2)

============================================================================
DESCRIPTION:
    This project implements an **MP3 Tag Reader and Editor** based on the
    ID3v2 metadata format used in MP3 audio files. ID3 tags store song
    information such as Title, Artist, Album, Year, Genre, and Comments.

*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h"
#include "types.h"

int main(int argc, char *argv[])// Main function
{
    if (argc == 1)// No arguments provided
    {
        printf("Invalid arguments!\n");
        printf("Use:\n");
        printf("  ./a.out -v <filename>\n");
        printf("  ./a.out -e <option> <new_value> <filename>\n");
        printf("  ./a.out --help\n");
        return 0;
    }

    if (strcmp(argv[1], "--help") == 0)// Help option
    {
        do_help();
        return 0;
    }

    if (strcmp(argv[1], "-v") == 0)// View tags
    {
        if (argc != 3)// Incorrect number of arguments
        {
            printf("Usage: ./a.out -v <filename>\n");
            return 0;
        }
        return do_view(argv[2]);
    }

    if (strcmp(argv[1], "-e") == 0)// Edit tags
    {
        if (argc != 5)// Incorrect number of arguments
        {
            printf("Usage: ./a.out -e <option> <new_value> <filename>\n");
            return 0;
        }
        return do_edit(argv[2], argv[3], argv[4]);
    }

    printf("Invalid option. Use --help\n");
    return 0;
}
