/*
 * Copyright (C) 2010  Evan Martin <martine@danga.com>
 *
 *  This is part of HarfBuzz, an OpenType Layout engine library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include "hb.h"
#include "hb-ft.h"
#include "hb-glib.h"

#include <stdio.h>
#include <glib.h>

/* Print an error message and exit 1. */
void
die (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  fprintf (stderr, "FATAL: ");
  vfprintf (stderr, fmt, ap);
  exit (1);
}

/* Open and return a font file, aborting on failure. */
FT_Face
open_font (FT_Library library, const char *filename)
{
  FT_Face face;
  FT_Error error = FT_New_Face (library,
                                filename,
                                0,  /* TODO(?): face index */
                                &face);
  if (error)
    die ("FT_New_Face error %d", error);

  error = FT_Set_Char_Size (face, 0, 16*64,
                            0, 0);  /* use default 72dpi */
  if (error)
    die ("FT_Set_Char_Size error %d", error);

  return face;
}

void
do_shape (FT_Face ftface, hb_buffer_t *buffer, char *input_text)
{
  hb_font_t *font = hb_ft_font_create (ftface, NULL);
  hb_face_t *face = hb_ft_face_create (ftface, NULL);

  hb_buffer_set_unicode_funcs (buffer, hb_glib_get_unicode_funcs ());
  /* TODO: hb_buffer_set_script, language */
  int input_len = strlen (input_text);
  hb_buffer_add_utf8 (buffer, input_text, input_len, 0, input_len);

  hb_shape (font, face, buffer,
            NULL, 0);  /* TODO: hb_feature_t */
}


/* Dump, as JSON, the result of shaping the buffer. */
void
dump (char *input_text, hb_buffer_t *buffer)
{
  hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos (buffer);
  hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions (buffer);
  unsigned int len = hb_buffer_get_length (buffer);

  printf ("{\n");
  printf ("  \"input\": \"%s\",\n", input_text);  /* XXX escaping */
  if (hb_buffer_get_direction (buffer) == HB_DIRECTION_RTL)
    printf ("  \"rtl\": 1,\n");
  printf ("  \"XXX\": \"font info (name, hash) would go here.\",\n");
  printf ("  \"info\": [\n");
  unsigned i;
  for (i = 0; i < len; i++)
  {
    hb_glyph_info_t *info = &glyph_info[i];
    printf ("    { \"codepoint\":%d, \"mask\":%d, \"cluster\":%d }%s\n",
            info->codepoint, info->mask, info->cluster,
            i < len-1 ? "," : "");
  }
  printf ("  ],\n");
  printf ("  \"position\": [\n");
  for (i = 0; i < len; i++)
  {
    hb_glyph_position_t *pos = &glyph_pos[i];
    printf ("    { \"x_advance\":%d, \"y_advance\":%d, "
            "\"x_offset\":%d, \"y_offset\":%d }%s\n",
            pos->x_advance, pos->y_advance,
            pos->x_offset, pos->y_offset,
            i < len-1 ? "," : "");
  }
  printf ("  ]\n");
  printf ("}\n");
}

gboolean
parse_args (int *argc, char*** argv, hb_buffer_t *buffer,
            char **filename, char **input)
{
  gboolean rtl = FALSE;
  /* Just enough use of GOptionEntry to show the idea. */
  GOptionEntry option_entries[] =
  {
    { "rtl", 0, 0, G_OPTION_ARG_NONE, &rtl,
      "Set buffer direction to RTL", NULL },
    { NULL }
  };

  GOptionContext *option_context = g_option_context_new ("FONTFILE INPUT");
  g_option_context_add_main_entries (option_context, option_entries, NULL);
  GError *error = NULL;
  if (!g_option_context_parse (option_context, argc, argv, &error))
  {
    fprintf (stderr, "%s\n", error->message);
    goto err;
  }

  if (*argc < 3)
  {
    gchar *help = g_option_context_get_help (option_context, TRUE, NULL);
    fprintf (stderr, "%s", help);
    g_free (help);
    goto err;
  }
  g_option_context_free (option_context);

  hb_buffer_set_direction (buffer, rtl ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
  *filename = (*argv)[1];
  *input = (*argv)[2];

  return TRUE;

err:
  g_option_context_free (option_context);
  return FALSE;
}

int
main (int argc, char **argv)
{
  hb_buffer_t *buffer = hb_buffer_create(0);

  char *filename, *input_text;
  if (!parse_args (&argc, &argv, buffer, &filename, &input_text))
    return 1;

  FT_Library ftlibrary;
  FT_Error fterror = FT_Init_FreeType (&ftlibrary);
  if (fterror)
    die ("FT_Init_FreeType error %d", fterror);
  FT_Face ftface = open_font (ftlibrary, filename);

  do_shape (ftface, buffer, input_text);

  dump (input_text, buffer);

  FT_Done_FreeType (ftlibrary);

  return 0;
}
