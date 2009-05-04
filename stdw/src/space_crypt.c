/* space_crypt.c */

#include "config.h"
#include "space.h"

/* ------------------------------------------------------------------------ */

char *space_crunch_code(char *code);
char *space_crypt_code(char *code, char *text, int type);

char *space_crunch_code(char *code)
{
  char *in;
  char *out;
  static char output[BUFFER_LEN];

  out = output;
  in = code;
  while (*in) {
    while (*in == ESC_CHAR) {
      while (*in && *in != 'm')
        in++;
      in++;                     /* skip 'm' */
    }
    if ((*in >= 32) && (*in <= 126)) {
      *out++ = *in;
    }
    in++;
  }
  *out = '\0';
  return output;
}

char *space_crypt_code(char *code, char *text, int type)
{
  static char textbuff[BUFFER_LEN];
  char codebuff[BUFFER_LEN];
  int start = 32;
  int end = 126;
  int mod = end - start + 1;
  char *p, *q, *r;

  if (!text && !*text)
    return (char *) "";
  if (!code || !*code)
    return text;
  mush_strncpy(codebuff, space_crunch_code(code), BUFFER_LEN);
  if (!*codebuff)
    return text;
  textbuff[0] = '\0';

  p = text;
  q = codebuff;
  r = textbuff;
  /* Encryption: Simply go through each character of the text, get its ascii
   * value, subtract start, add the ascii value (less start) of the
   * code, mod the result, add start. Continue  */
  while (*p) {
    if ((*p < start) || (*p > end)) {
      p++;
      continue;
    }
    if (type)
      *r++ = (((*p++ - start) + (*q++ - start)) % mod) + start;
    else
      *r++ = (((*p++ - *q++) + 2 * mod) % mod) + start;
    if (!*q)
      q = codebuff;
  }
  *r = '\0';
  return textbuff;
}