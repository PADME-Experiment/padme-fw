#ifndef _TOOLS_H_
#define _TOOLS_H_

pid_t create_lock(); // Function to create lock file for current process
void remove_lock(); // Function to remove lock file for current process

int create_initok_file(); // Function to create initok control file
int create_initfail_file(); // Function to create initfail control file

int generate_filename(char*,const time_t); // Function to add time tag to file name
char* format_time(const time_t time); // Function to format time tags

#endif
