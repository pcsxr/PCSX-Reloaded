//
// Copyright (c) 2008, Wei Mingzhi. All rights reserved.
//
// Use, redistribution and modification of this code is unrestricted
// as long as this notice is preserved.
//

#ifndef __GLIB_H
#define __GLIB_H

typedef char gchar;

#include <stdio.h>
#include <stdarg.h>

inline static gchar *g_build_filename(const gchar *first_element, ...)
{
   gchar *str, *p;
   va_list args;

   str = (gchar *)malloc(4096);
   strcpy(str, first_element);

   va_start(args, first_element);

   while (1)
   {
      p = va_arg(args, gchar *);
      if (p == NULL)
         break;

      strcat(str, "/");
      strcat(str, p);
   }

   va_end(args);

   return str;
}

inline static void g_free(void *p)
{
	free(p);
}

#endif
