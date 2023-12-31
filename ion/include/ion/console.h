#ifndef ION_CONSOLE_H
#define ION_CONSOLE_H

namespace Ion {
namespace Console {

void writeChar(char c);
char readChar();
bool readCharNonblocking(char * dest);

// The lines are NULL-terminated
void writeLine(const char * line);
void readLine(char * line, int maxLineLength);

}
}

#endif
