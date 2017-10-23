#include <stdio.h>
#include <string.h>

static char const *cpu_name (char *buf);

int main (int argc, char *const argv[])
{
   char buf[256];
   __builtin_cpu_init ();


   int avx2 = __builtin_cpu_supports ("avx2");
   if (avx2) { puts ("avx2"); return 0; }

   int avx  = __builtin_cpu_supports ("avx");
   if (avx)  { puts ("avx");  return 0; }

   else      { puts ("gen");  return 0; }

   char const *cpu = cpu_name (buf);
   printf ("cpu %s\n", cpu);



   return 0;

}

#define Check(_what, _s) if (__builtin_cpu_is (_what)) { strcat (_s, " "); strcat (_s, _what); }

static char const *cpu_name (char *str)
{
/*
   static const char *Cpus[] = { "intel",       // Intel CPU. 
                                 "atom",        // Intel Atom CPU. 
                                 "core2",       // Intel Core 2 CPU. 
                                 "corei7",      // Intel Core i7 CPU. 
                                 "nehalem",     // Intel Core i7 Nehalem CPU. 
                                 "westmere",    // Intel Core i7 Westmere CPU. 
                                 "sandybridge", // Intel Core i7 Sandy Bridge CPU. 
                                 "amd",         // AMD CPU. 
                                 "amdfam10h",   // AMD Family 10h CPU. 
                                 "barcelona",   // AMD Family 10h Barcelona CPU. 
                                 "shanghai",    // AMD Family 10h Shanghai CPU. 
                                 "istanbul",    // AMD Family 10h Istanbul CPU. 
                                 "btver1",      // AMD Family 14h CPU. 
                                 "amdfam15h",   // AMD Family 15h CPU. 
                                 "bdver1",      // AMD Family 15h Bulldozer version 1. 
                                 "bdver2",      // AMD Family 15h Bulldozer version 2. 
                                 "bdver3",      // AMD Family 15h Bulldozer version 3. 
                                 "btver2"       // AMD Family 16h CPU.
   };
*/

   str[0] = 0;

   Check ("intel"      , str);  // Intel CPU
   Check ("atom"       , str);  // Intel Atom CPU. 
   Check ("core2"      , str);  // Intel Core 2 CPU. 
   Check ("corei7"     , str);  // Intel Core i7 CPU.  
   Check ("nehalem"    , str);  // Intel Core i7 Nehalem CPU.  
   Check ("westmere"   , str);  // Intel Core i7 Westmere CPU. 
   Check("sandybridge" , str);  // Intel Core i7 Sandy Bridge CPU. 
   //Check("haswell"     , str);  // Intel Core i7 Haswell CPU. 
   Check("amd"         , str);  // AMD CPU. 
   Check("amdfam10h"   , str);  // AMD Family 10h CPU. 
   Check("barcelona"   , str);  // AMD Family 10h Barcelona CPU. 
   Check("shanghai"    , str);  // AMD Family 10h Shanghai CPU. 
   Check("istanbul"    , str);  // AMD Family 10h Istanbul CPU. 
   //Check("btver1"    , str);  // AMD Family 14h CPU. 
   Check("amdfam15h"   , str);  // AMD Family 15h CPU. 
   Check("bdver1"      , str);  // AMD Family 15h Bulldozer version 1. 
   Check("bdver2"      , str);  // AMD Family 15h Bulldozer version 2. 
   Check("bdver3"      , str);  // AMD Family 15h Bulldozer version 3. 
   //Check("btver2"    , str);  // AMD Family 16h CPU.


   return str;
}
