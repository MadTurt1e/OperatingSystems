#include <unistd.h>

int main() {
  char buffer[4096];
  write(1, buffer, read(0, buffer, 4096));
}
