#include <stdio.h>
#include <string.h>
#include <err.h>
#include <blkid/blkid.h>


int main (int argc, char *argv[]) {
   blkid_probe pr = blkid_new_probe_from_filename(argv[1]);
   if (!pr) {
      err(1, "Failed to open %s", argv[1]);
   }

   // Get number of partitions
   blkid_partlist ls;
   int nparts, i;

   ls = blkid_probe_get_partitions(pr);
   nparts = blkid_partlist_numof_partitions(ls);
   printf("Number of partitions:%d\n", nparts);

   if (nparts <= 0){
      printf("Please enter correct device name! e.g. \"/dev/sdc\"\n");
      return;
   }

   // Get UUID, label and type
   const char *uuid;
   const char *label;
   const char *type;

   for (i = 0; i < nparts; i++) {
      char dev_name[20];

      sprintf(dev_name, "%s%d", argv[1], (i+1));

      pr = blkid_new_probe_from_filename(dev_name);
      blkid_do_probe(pr);

      blkid_probe_lookup_value(pr, "UUID", &uuid, NULL);

      blkid_probe_lookup_value(pr, "LABEL", &label, NULL);

      blkid_probe_lookup_value(pr, "TYPE", &type, NULL);

      printf("Name=%s, UUID=%s, LABEL=%s, TYPE=%s\n", dev_name, uuid, label, type);

   }

   blkid_free_probe(pr);

   return 0;
}
