// denseSift.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string>
#include <highgui.h>
extern "C" {
#include "sift.h"
#include "imgfeatures.h"
#include "utils.h"
}

#include "npy.h"
#include "CmdLine.h"

char* out_img_name = NULL;
int display = 1;
int intvls = SIFT_INTVLS;
double sigma = SIFT_SIGMA;
double contr_thr = SIFT_CONTR_THR;
int curv_thr = SIFT_CURV_THR;
int img_dbl = SIFT_IMG_DBL;
int descr_width = SIFT_DESCR_WIDTH;
int descr_hist_bins = SIFT_DESCR_HIST_BINS;



//void aa()
//{
//    int  ndims = 2;
//
//		int  shape[2];
//    double a[3][3] = { { 1, 2, 3 },
//                       { 4, 5, 6 },
//                       { 7, 8, 9 } };
//    float  b[2][3] = { { 0.5, 1.0, 1.5 },
//                       { 2.0, 2.5, 3.0 } };
//    int    c[3][3] = { { 1, 2, 3 },
//                       { 4, 5, 6 },
//                       { 7, 8, 9 } };
//  
//
//    printf("Creating files a.npy, b.npy, c.npy, d.npy, e.npy\n");
//    shape[0] = 3;
//    shape[1] = 3;
//
//	 shape[0] = 3;
//    npy_save_int("d:\\scratch\\c.npy", 0, ndims, shape, &c[0][0]);
//
//	 npy_save_double("d:\\scratch\\a.npy", 0, ndims, shape, &a[0][0]);
//    shape[0] = 2;
//    npy_save_float("d:\\scratch\\b.npy", 0, ndims, shape, &b[0][0]);
//   
//    exit(0);
//}

void export_features_numpy(const char* filename, feature* features, int n)
{
	float* p = (float*)malloc(sizeof(float)*n*(128+4));

	float* q = p;
	
	for( int i = 0; i < n; i++ )
	{
		/*if(features[i].bInvalid == 1)
			continue;*/


		*q = features[i].y; q++;
		*q = features[i].x; q++;
		*q = features[i].scl; q++;
		*q = features[i].ori; q++;

		for( int j = 0; j < 128; j++ )
		{
			*q = features[i].descr[j];  q++;
		}
	}


	int  ndims = 2;
	int  shape[2];

    shape[0] = n;
    shape[1] = 128+4;
    npy_save_float((char*)filename, 0, ndims, shape, p);

	free(p);

	
}

void show_help()
{
	printf("\nParameters:\n\
	-i image\n\
	-o outputfile\n\
	-s stepSize initial sampling step size [default 5]\n\
	-txt text output format, if you omit this flag the output is in numpy format\n\n");
}

int main(int argc, char* argv[])
{
	//aa();

	std::string img_file_name;
	std::string out_file_name;
	bool bExportTxt = false;
	int nStepSize = 0;

   CCmdLine cmdLine;

	if (cmdLine.SplitLine(argc, argv) < 1)     {        exit(-1);     }
	if (cmdLine.HasSwitch("-h"))    {      show_help();     exit(0);      }

	try{  
		img_file_name = cmdLine.GetArgument("-i", 0);
	}
	catch (...)
	{
		show_help();
		exit(-1);
	}

	out_file_name = cmdLine.GetSafeArgument("-o", 0,(img_file_name + ".sift").c_str());

	nStepSize =    atoi(cmdLine.GetSafeArgument("-s", 0, "5").c_str());

	// since opt2 has no arguments, just test for the presence of
	// the '-opt2' switch
	bExportTxt = cmdLine.HasSwitch("-txt");


	IplImage* img;
	struct feature* features;
	

	img = cvLoadImage( img_file_name.c_str(), 1 );

	if( ! img )	{
		fprintf( stderr, "unable to load image from %s", img_file_name );
		exit( 1 );
	}

	int n = _sift_features( img, &features, intvls, sigma, contr_thr, curv_thr,
						img_dbl, descr_width, descr_hist_bins ,nStepSize);


	fprintf(stdout,"Saving %d SIFT descriptors in %s.\n",n,out_file_name.c_str());


	if(bExportTxt)
		export_features( out_file_name.c_str(), features, n );
	else
		export_features_numpy( out_file_name.c_str(), features, n );


	return 0;
}



	/*if( display )
	{
		draw_features( img, features, n );
		cvNamedWindow( img_file_name, 1 );
		cvShowImage( img_file_name, img );
		cvWaitKey( 0 );
	}*/
