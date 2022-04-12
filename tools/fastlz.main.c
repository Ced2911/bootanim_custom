/*
  fastlz - file compressor using FastLZ (lightning-fast compression library)
  Copyright (C) 2007-2020 Ariya Hidayat <ariya.hidayat@gmail.com>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIXPACK_VERSION_MAJOR 0
#define SIXPACK_VERSION_MINOR 1
#define SIXPACK_VERSION_REVISION 0
#define SIXPACK_VERSION_STRING "snapshot 20070615"

#include "fastlz.h"

#define BLOCK_SIZE (2 * 64 * 1024)

void usage(void)
{
  printf("fastlz: high-speed file compression tool\n");
  printf("Copyright (C) Ariya Hidayat\n");
  printf("\n");
  printf("Usage: fastlz [options]  input-file  output-file\n");
  printf("\n");
  printf("Options:\n");
  printf("  -1    compress faster\n");
  printf("  -2    compress better\n");
  printf("  -v    show program version\n");
#ifdef SIXPACK_BENCHMARK_WIN32
  printf("  -mem  check in-memory compression speed\n");
#endif
  printf("\n");
}

int pack_file_compressed(const char *input_file, int method, int level, FILE *f)
{
  FILE *in;
  unsigned long fsize;
  unsigned long checksum;
  const char *shown_name;
  unsigned char buffer[BLOCK_SIZE];
  unsigned char dec[BLOCK_SIZE];
  unsigned char result[BLOCK_SIZE * 2]; /* FIXME twice is too large */
  unsigned char progress[20];
  int c;
  unsigned long percent;
  unsigned long total_read;
  unsigned long total_compressed;
  int chunk_size;
  int bytes_read = 0;

  /* sanity check */
  in = fopen(input_file, "rb");
  if (!in)
  {
    printf("Error: could not open %s\n", input_file);
    return -1;
  }

  fseek(in, 0, SEEK_END);
  fsize = ftell(in);
  fseek(in, 0, SEEK_SET);

  fread(buffer, fsize, 1, in);

  total_compressed = fastlz_compress_level(level, buffer, fsize, result);

  fwrite(result, total_compressed, 1, f);
  fclose(in);

  if (total_compressed < fsize)
  {
    if (fsize < (1 << 20))
      percent = total_compressed * 1000 / fsize;
    else
      percent = total_compressed / 256 * 1000 / (fsize >> 8);
    percent = 1000 - percent;
    printf("%2d.%d%% saved", (int)percent / 10, (int)percent % 10);
  }
  printf("\n");

  
  int d = fastlz_decompress(result, total_compressed, dec, 32*1024);

  printf("f:%d\n", fsize);
  printf("%d\n", d);
  return 0;
}

int pack_file(int compress_level, const char *input_file, const char *output_file)
{
  FILE *f;
  int result;

  f = fopen(output_file, "rb");
  if (f)
  {
    fclose(f);
    printf("Warning: file %s already exists.\n\n", output_file);
    // return -1;
  }

  f = fopen(output_file, "wb");
  if (!f)
  {
    printf("Error: could not create %s. Aborted.\n\n", output_file);
    return -1;
  }

  result = pack_file_compressed(input_file, 1, compress_level, f);
  fclose(f);

  return result;
}

int main(int argc, char **argv)
{
  int i;
  int compress_level;
  int benchmark;
  char *input_file;
  char *output_file;

  /* show help with no argument at all*/
  if (argc == 1)
  {
    usage();
    return 0;
  }

  /* default compression level, not the fastest */
  compress_level = 2;

  /* do benchmark only when explicitly specified */
  benchmark = 0;

  /* no file is specified */
  input_file = 0;
  output_file = 0;

  for (i = 1; i <= argc; i++)
  {
    char *argument = argv[i];

    if (!argument)
      continue;

    /* display help on usage */
    if (!strcmp(argument, "-h") || !strcmp(argument, "--help"))
    {
      usage();
      return 0;
    }

    /* check for version information */
    if (!strcmp(argument, "-v") || !strcmp(argument, "--version"))
    {
      printf("fastlz: high-speed file compression tool\n");
      // printf("Version %s (using FastLZ %s)\n", SIXPACK_VERSION_STRING, FASTLZ_VERSION_STRING);
      printf("Copyright (C) Ariya Hidayat\n");
      printf("\n");
      return 0;
    }

    /* test compression speed? */
    if (!strcmp(argument, "-mem"))
    {
      benchmark = 1;
      continue;
    }

    /* compression level */
    if (!strcmp(argument, "-1") || !strcmp(argument, "--fastest"))
    {
      compress_level = 1;
      continue;
    }
    if (!strcmp(argument, "-2"))
    {
      compress_level = 2;
      continue;
    }

    /* unknown option */
    if (argument[0] == '-')
    {
      printf("Error: unknown option %s\n\n", argument);
      printf("To get help on usage:\n");
      printf("  fastlz --help\n\n");
      return -1;
    }

    /* first specified file is input */
    if (!input_file)
    {
      input_file = argument;
      continue;
    }

    /* next specified file is output */
    if (!output_file)
    {
      output_file = argument;
      continue;
    }

    /* files are already specified */
    printf("Error: unknown option %s\n\n", argument);
    printf("To get help on usage:\n");
    printf("  fastlz --help\n\n");
    return -1;
  }

  if (!input_file)
  {
    printf("Error: input file is not specified.\n\n");
    printf("To get help on usage:\n");
    printf("  fastlz --help\n\n");
    return -1;
  }

  if (!output_file && !benchmark)
  {
    printf("Error: output file is not specified.\n\n");
    printf("To get help on usage:\n");
    printf("  fastlz --help\n\n");
    return -1;
  }

  return pack_file(compress_level, input_file, output_file);

  /* unreachable */
  return 0;
}