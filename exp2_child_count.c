#include <stdio.h>
#include <unistd.h>
int main() {
  fork();
  fork() && fork() || fork();
  fork();
  printf("Yes from %d\n", getpid());
  return 0;
}
