#include <stdio.h>

main(int argc, char** argv) {
  int zeichen;
  while ((zeichen= getchar ()) != EOF) {
    if (zeichen=='@')
      printf("%s",argv[1]);
    else
      putchar(zeichen);
  }
}
