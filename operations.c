#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "types.h"

void do_help()  // Display help information
{
    printf("--------------------------------------\n");
    printf("------- MP3 TAG READER & EDITOR ------\n");
    printf("--------------------------------------\n");
    printf("Usage:\n");
    printf("  ./a.out -v <filename>               → View tags\n");
    printf("  ./a.out -e <option> <new_value> <filename>\n");
    printf("Options:\n");
    printf("  -t  : Edit TITLE\n");
    printf("  -a  : Edit ARTIST\n");
    printf("  -A  : Edit ALBUM\n");
    printf("  -y  : Edit YEAR\n");
    printf("  -g  : Edit GENRE\n");
    printf("  -c  : Edit COMMENT\n");
}

//-------------------------------------------
//   View MP3 tag information
//-------------------------------------------
Status do_view(char *filename)  
{
    mp3 m; // mp3 structure

    strcpy(m.file, filename);// Copy filename
    m.fp = fopen(m.file, "rb");// Open file in binary read mode

    if (!m.fp) // File not found
    {
        printf("Error: File not found!\n");
        return e_failure;
    }

    if (mp3_file_version(&m) == e_failure)// Check for valid ID3v2 tag
    {
        printf("Not a valid ID3v2 MP3 file.\n");
        fclose(m.fp);
        return e_failure;
    }

    display_file_content(&m);// Display tag content
    fclose(m.fp);
    return e_success;
}

//-------------------------------------------
//   Edit MP3 tag information
//-------------------------------------------
Status do_edit(char *option, char *value, char *filename)
{
    mp3 m;// mp3 structure

    strcpy(m.file, filename);// Copy filename

    if (strcmp(option, "-t") == 0)// Edit TITLE
        edit_tag(&m, "TIT2", "TITLE", filename, value);

    else if (strcmp(option, "-a") == 0)// Edit ARTIST
        edit_tag(&m, "TPE1", "ARTIST", filename, value);

    else if (strcmp(option, "-A") == 0)// Edit ALBUM
        edit_tag(&m, "TALB", "ALBUM", filename, value);

    else if (strcmp(option, "-y") == 0)// Edit YEAR
        edit_tag(&m, "TYER", "YEAR", filename, value);

    else if (strcmp(option, "-g") == 0)// Edit GENRE
        edit_tag(&m, "TCON", "GENRE", filename, value);

    else if (strcmp(option, "-c") == 0)// Edit COMMENT
        edit_tag(&m, "COMM", "COMMENT", filename, value);

    else
    {
        printf("Invalid edit option.\n");
        return e_failure;
    }

    return e_success;
}

/*-------------------------------------------
   Read ID3v2 header → extract tag size
--------------------------------------------*/
Status mp3_file_version(mp3 *m)
{
    unsigned char header[10];

    fread(header, 10, 1, m->fp);// Read ID3v2 header

    if (strncmp((char *)header, "ID3", 3) != 0)
    {
        return e_failure;
    }

    // synchsafe int (7 bits each)
    m->size = (header[6] & 0x7F) << 21 |
              (header[7] & 0x7F) << 14 |
              (header[8] & 0x7F) << 7  |
              (header[9] & 0x7F);  // Calculate tag size

    printf("ID3v2 Tag Size: %d bytes\n", m->size);
    return e_success;
}

/*-------------------------------------------
   Display frames inside the tag
--------------------------------------------*/
Status display_file_content(mp3 *m)
{
    unsigned char h[10];
    char id[5];
    int size;

    fseek(m->fp, 10, SEEK_SET);

    printf("\n------------- MP3 TAG INFORMATION --------------\n");

    while (ftell(m->fp) < m->size + 10 && fread(h, 1, 10, m->fp) == 10)// Read frame header
    {
        memcpy(id, h, 4);// Copy frame ID
        id[4] = '\0';// Null-terminate string

        size = (h[4] << 24) | (h[5] << 16) | (h[6] << 8) | h[7];// Calculate frame size

        if (size <= 0 || size > 5000)// Invalid size check
            break;

        char *buf = malloc(size);
        fread(buf, size, 1, m->fp);

        // skip first encoding byte
        char *text = buf + 1;

        if (strcmp(id, "TIT2") == 0)// TITLE frame
            printf("Title   : %s\n", text);

        else if (strcmp(id, "TPE1") == 0)// ARTIST frame
            printf("Artist  : %s\n", text);

        else if (strcmp(id, "TALB") == 0)// ALBUM frame
            printf("Album   : %s\n", text);

        else if (strcmp(id, "TYER") == 0)// YEAR frame
            printf("Year    : %s\n", text);

        else if (strcmp(id, "TCON") == 0)// GENRE frame
            printf("Genre   : %s\n", text);

        else if (strcmp(id, "COMM") == 0)// COMMENT frame
            printf("Comment : %s\n", text);

        free(buf);
    }

    return e_success;
}

/*-------------------------------------------
   Modify specific frame
--------------------------------------------*/
Status read_and_edit_data(mp3 *m, char *data, char *frame_id)
{
    m->fp = fopen(m->file, "rb");
    if (!m->fp)// File not found
    {
        printf("Error opening source file.\n");
        return e_failure;
    }

    strcpy(m->temp, "edited.mp3");// Temporary file name
    m->fp2 = fopen(m->temp, "wb");// Open temporary file in binary write mode

    if (!m->fp2)// Error creating output file
    {
        printf("Error creating output file.\n");
        fclose(m->fp);
        return e_failure;
    }

    unsigned char header[10];
    fread(header, 10, 1, m->fp);// Read ID3v2 header
    fwrite(header, 10, 1, m->fp2);// Write header to new file

    unsigned char fh[10];
    char id[5];
    int size;

    while (fread(fh, 1, 10, m->fp) == 10)// Read frame header
    {
        memcpy(id, fh, 4);// Copy frame ID
        id[4] = '\0';

        size = (fh[4] << 24) | (fh[5] << 16) | (fh[6] << 8) | fh[7];// Calculate frame size

        if (size <= 0 || size > 5000)
            break;

        if (strcmp(id, frame_id) == 0)// Target frame found
        {
            int new_size = strlen(data) + 1;

            fh[4] = (new_size >> 24) & 0xFF;
            fh[5] = (new_size >> 16) & 0xFF;
            fh[6] = (new_size >> 8) & 0xFF;
            fh[7] = new_size & 0xFF;

            fwrite(fh, 10, 1, m->fp2);// Write modified frame header

            fputc(0, m->fp2);     // encoding byte
            fwrite(data, 1, strlen(data), m->fp2);// Write new data

            fseek(m->fp, size, SEEK_CUR);

            int c;
            while ((c = fgetc(m->fp)) != EOF)// Copy remaining data
                fputc(c, m->fp2);

            break;
        }
        else
        {
            // copy original
            fwrite(fh, 10, 1, m->fp2);// Write original frame header
            char *buf = malloc(size);
            fread(buf, size, 1, m->fp);// Read original frame data
            fwrite(buf, size, 1, m->fp2);// Write original frame data
            free(buf);
        }
    }

    fclose(m->fp);
    fclose(m->fp2);

    rename("edited.mp3", m->file);// Replace original file with edited file

    printf("Updated '%s' successfully!\n", frame_id);// Success message
    return e_success;
}

void edit_tag(mp3 *m, char *frame_id, const char *tag_name,
              const char *file_name, const char *new_value)
{
    printf("Editing %s → %s\n", tag_name, new_value);// Display edit action
    strcpy(m->data, new_value);// New tag value
    read_and_edit_data(m, m->data, frame_id);// Modify the tag
}
