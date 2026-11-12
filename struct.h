#ifndef STRUCT_H   
#define STRUCT_H

#include <stdio.h>
#include "types.h"

typedef struct
{
    char file[100];      // MP3 filename
    FILE *fp;            // Source file pointer
    int size;            // Tag size
    char data[100];      // New value for editing
    char temp[100];      // Temporary filename
    FILE *fp2;           // Duplicate file pointer

} mp3;

Status validate(mp3 *m);// Validate MP3 file and ID3v2 tag
Status mp3_file_version(mp3 *m);// Read ID3v2 header → extract tag size
Status display_file_content(mp3 *m);// Display frames inside the tag
Status read_and_edit_data(mp3 *m, char *data, char *frame_id);// Modify specific frame
void edit_tag(mp3 *m, char *frame_id, const char *tag_name,
              const char *file_name, const char *new_value);// Edit specific tag

void do_help();// Display help information
Status do_view(char *filename);// View MP3 tag information
Status do_edit(char *option, char *value, char *filename);// Edit MP3 tag information

#endif
