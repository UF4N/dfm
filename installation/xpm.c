#include "xpm.h"

main()
{
   int Iversion;
   int Irevision;
   int Lversion;
   int Lrevision;
   Iversion=XpmIncludeVersion/100-(XpmIncludeVersion/10000)*100;
   Lversion=XpmLibraryVersion()/100-(XpmLibraryVersion()/10000)*100;
   Irevision=XpmIncludeVersion-(XpmIncludeVersion/100)*100;
   Lrevision=XpmLibraryVersion()-(XpmLibraryVersion()/100)*100;

   if (Iversion!=Lversion || Irevision!=Lrevision) {
     printf("----------------------ERROR----------------------\n");
     printf("Your xpm installation isn't correct:\n");
     printf("There is a difference between the version number\n");
     printf("of the library and the xpm.h file.\n %d.%d != %d.%d\n",Lversion,Lrevision,Iversion,Irevision);
     printf("-------------------------------------------------\n");
     exit(-1);
   }
   if ((Lversion==4 && Lrevision<7) || Lversion < 4) {
     printf("----------------------ERROR----------------------\n");
     printf("This Xpm library is too old: %d.%d < 4.7\n",Lversion,Lrevision);
     printf("-------------------------------------------------\n");
     exit(-1);
   } 
   printf("Xpm library version %d.%d OK ...\n",Lversion,Lrevision);
   exit(0);
}
