#include <stdio.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
  int i;
  int option;

  /* parse short options */

  puts("------------------------------");

printf("argc=%di\n",argc);
  for (i = optind; i < argc; i++) {
    puts(argv[i]);
  }
  while ((option = getopt(argc, argv, "bEnsTv")) != -1) {
    switch (option) {
    case 'b':
      puts("Put line numbers next to non-blank lines");
      break;
    case 'E':
      puts("Show the ends of lines as $");
      break;
    case 'n':
      puts("Put line numbers next to all lines");
      break;
    case 's':
      puts("Suppress printing repeated blank lines");
      break;
    case 'T':
      puts("Show tabs as ^I");
      break;
    case 'v':
      puts("Verbose");
      break;
    default:                          /* '?' */
      puts("What's that??");
    }
  }

  /* print the rest of the command line */

  puts("------------------------------");
printf("argc=%d\n",argc);
printf("optind=%d\n",optind);
  for (i = 0; i < argc; i++) {
    puts(argv[i]);
  }

  return 0;
}

