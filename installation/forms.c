#include "forms.h"

main()
{
   int version;
   int revision;
   fl_library_version(&version,&revision);
   if (version!=FL_VERSION || revision!=FL_REVISION) {
     printf("----------------------ERROR----------------------\n");
     printf("Your xforms installation isn't correct:\n");
     printf("There is a difference between the version number\n");
     printf("of the library and the forms.h file!\n %d.%d != %d.%d\n",version,revision,FL_VERSION,FL_REVISION);
     printf("-------------------------------------------------\n");
     exit(-1);
   }
   if (version==0 && revision<86) {
     printf("----------------------ERROR----------------------\n");
     printf("This forms library is too old: %d.%d < 0.86\n",version,revision);
     printf("-------------------------------------------------\n");
     exit(-1);
   }
   if (version!=0 || revision>88)
     printf("WARNING : Forms library version %d.%d haven't been tested yet! OK ...\n",version,revision);
   else
     printf("Forms library version %d.%d OK ...\n",version,revision);
   exit(0);
}
