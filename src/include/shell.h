#ifndef SHELL_H
#define SHELL_H

void shell_init(void);
void shell_run(void);
int shell_read_line(char *buffer, int max_len);
void shell_execute(const char *cmd);
void shell_help(void);

#endif