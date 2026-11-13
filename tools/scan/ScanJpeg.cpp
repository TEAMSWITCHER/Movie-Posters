#include <cstdio>
#include <setjmp.h>
#include "jpeg-9e\jpeglib.h"

struct my_error_mgr
{
   struct jpeg_error_mgr pub;	/* "public" fields */
   jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr* my_error_ptr;

//
// Here's the routine that will replace the standard error_exit method:
//
static void my_error_exit(j_common_ptr cinfo)
{
   /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
   my_error_ptr myerr = (my_error_ptr)cinfo->err;

   /* Always display the message. */
   /* We could postpone this until after returning, if we chose. */
   (*cinfo->err->output_message) (cinfo);

   /* Return control to the setjmp point */
   longjmp(myerr->setjmp_buffer, 1);
}


int ScanJpeg(const char *filename, unsigned int *width, unsigned int *height)
{
   FILE *infile = NULL;		/* source file */
   if ((infile = fopen(filename, "rb")) == NULL)
   {
      fprintf(stderr, "can't open %s\n", filename);
      return 0;
   }

   // We set up the normal JPEG error routines, then override error_exit
   struct jpeg_decompress_struct cinfo;
   struct my_error_mgr jerr;
   cinfo.err = jpeg_std_error(&jerr.pub);
   jerr.pub.error_exit = my_error_exit;
   if (setjmp(jerr.setjmp_buffer))
   {
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
      return 0;
   }
   
   // Now we can initialize the JPEG decompression object
   jpeg_create_decompress(&cinfo);
   
   // Specify data source
   jpeg_stdio_src(&cinfo, infile);

   // Read file parameters
   (void) jpeg_read_header(&cinfo, TRUE);

   *width = cinfo.image_width;
   *height = cinfo.image_height;

   fclose(infile);
   return 1;
}