#include <stdio.h>
#include <unistd.h>
using namespace std;

int main(){
  char chars[] = {'-', '\\', '|', '/'};
      unsigned int i;

      for (i = 0; ; ++i) {
              printf("%% %d\r",i);
              fflush(stdout);
              usleep(200000);
      }

      return 0;
}
