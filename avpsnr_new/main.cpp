using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "avpsnr.h"

FILE *fp_out;
#define FILENAMELEN 240
int main(int argc, char *argv[])
{
	int QPcount, FrameRate, NoOfFrames = 0, detail = 0, NoOfQP;
	EncodeFormat_e enc_f1, enc_f2;

	BD_CTX BD_YUV, BD_Y, BD_U, BD_V,
		BD_YUV_i, BD_Y_i, BD_U_i, BD_V_i,
		BD_YUV_g, BD_Y_g, BD_U_g, BD_V_g,
		BD_YUV_s, BD_Y_s, BD_U_s, BD_V_s,
		BD_YUV_p, BD_Y_p, BD_U_p, BD_V_p,
		BD_YUV_f, BD_Y_f, BD_U_f, BD_V_f,
		BD_YUV_b, BD_Y_b, BD_U_b, BD_V_b;
	AVS_CTX ed, avs;
	char InputFile2[FILENAMELEN], InputFile1[FILENAMELEN], OutFile[FILENAMELEN], OutFile_stat[FILENAMELEN], StatName[FILENAMELEN], *pStatName, Label[FILENAMELEN];
	time_t timer;
	char buffer[26];
	FILE *fp_out_stat;

	FrameRate = 0;
	NoOfFrames = 0;
	detail = 0;

	switch (argc) {
	case 10:
		strcpy(InputFile1, argv[1]);
//		enc_f1 = (EncodeFormat_e)atoi(argv[2]);
		enc_f1 = Str2Enum(argv[2]);
		strcpy(InputFile2, argv[3]);
//		enc_f2 = (EncodeFormat_e)atoi(argv[4]);
		enc_f2 = Str2Enum(argv[4]);
		NoOfFrames = atoi(argv[5]);
		NoOfQP = atoi(argv[6]);
		FrameRate = atoi(argv[7]);
		detail = atoi(argv[8]);
		strcpy(Label, argv[9]);
		if (enc_f1 == Unknown_enc) {
			printf("%s has unknow encoder format %s!\n\n", InputFile1, argv[2]);
			exit(-1);
		}
		if (enc_f2 == Unknown_enc) {
			printf("%s has unknow encoder format %s!\n\n", InputFile2, argv[4]);
			exit(-1);
		}

		break;

	default:
		printf("avpsnr.exe InputFile1 Option1 InputFile2 Option2 NoOfFrames NoOfQP FrameRate detail Label\n");
		exit(1);
	}

	//	Reading first file
	switch (enc_f1) {
	case AVS:
		Reading_AVS(InputFile1, &avs, NoOfFrames, FrameRate, NoOfQP);
		break;
	case Lion_AVS:
		Reading_Lion_AVS(InputFile1, &avs, NoOfFrames, FrameRate, NoOfQP);
		break;
	case HM:
		Reading_HM(InputFile1, &avs, NoOfFrames, FrameRate, NoOfQP);
		break;
	case Lion_265:
		Reading_Lion_265(InputFile1, &avs, NoOfFrames, FrameRate, NoOfQP);
		break;
	case x264:
		Reading_x264(InputFile1, &avs, NoOfFrames, FrameRate, NoOfQP);
		break;
	case x265:
		Reading_x265(InputFile1, &avs, NoOfFrames, FrameRate, NoOfQP);
		break;
	case CnM264:
		Reading_CnM264(InputFile1, &avs, NoOfFrames, FrameRate, NoOfQP);
		break;
	case CnM:
		Reading_CnM(InputFile1, &avs, NoOfFrames, FrameRate, NoOfQP);
		break;
	default:
		printf("Encoder 1 Unknown file format!");
		exit(-1);
		break;
	}
	//	Reading first file
	switch (enc_f2) {
	case AVS:
		Reading_AVS(InputFile2, &ed, NoOfFrames, FrameRate, NoOfQP);
		break;
	case Lion_AVS:
		Reading_Lion_AVS(InputFile2, &ed, NoOfFrames, FrameRate, NoOfQP);
		break;
	case HM:
		Reading_HM(InputFile2, &ed, NoOfFrames, FrameRate, NoOfQP);
		break;
	case Lion_265:
		Reading_Lion_265(InputFile2, &ed, NoOfFrames, FrameRate, NoOfQP);
		break;
	case x264:
		Reading_x264(InputFile2, &ed, NoOfFrames, FrameRate, NoOfQP);
		break;
	case x265:
		Reading_x265(InputFile2, &ed, NoOfFrames, FrameRate, NoOfQP);
		break;
	case CnM264:
		Reading_CnM264(InputFile2, &ed, NoOfFrames, FrameRate, NoOfQP);
		break;
	case CnM:
		Reading_CnM(InputFile2, &ed, NoOfFrames, FrameRate, NoOfQP);
		break;
	default:
		printf("Encoder 2 Unknown file format!");
		exit(-1);
		break;
	}

	if (0) {
		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", ed.cnt[QPcount], ed.br[QPcount], ed.snr_Y[QPcount], ed.snr_U[QPcount], ed.snr_V[QPcount], ed.snr_YUV[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", ed.cnt_i[QPcount], ed.br_i[QPcount], ed.snr_Y_i[QPcount], ed.snr_U_i[QPcount], ed.snr_V_i[QPcount], ed.snr_YUV_i[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", ed.cnt_g[QPcount], ed.br_g[QPcount], ed.snr_Y_g[QPcount], ed.snr_U_g[QPcount], ed.snr_V_g[QPcount], ed.snr_YUV_g[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", ed.cnt_s[QPcount], ed.br_s[QPcount], ed.snr_Y_s[QPcount], ed.snr_U_s[QPcount], ed.snr_V_s[QPcount], ed.snr_YUV_s[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", ed.cnt_p[QPcount], ed.br_p[QPcount], ed.snr_Y_p[QPcount], ed.snr_U_p[QPcount], ed.snr_V_p[QPcount], ed.snr_YUV_p[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", ed.cnt_f[QPcount], ed.br_f[QPcount], ed.snr_Y_f[QPcount], ed.snr_U_f[QPcount], ed.snr_V_f[QPcount], ed.snr_YUV_f[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", ed.cnt_b[QPcount], ed.br_b[QPcount], ed.snr_Y_b[QPcount], ed.snr_U_b[QPcount], ed.snr_V_b[QPcount], ed.snr_YUV_b[QPcount]);


		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", avs.cnt[QPcount], avs.br[QPcount], avs.snr_Y[QPcount], avs.snr_U[QPcount], avs.snr_V[QPcount], avs.snr_YUV[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", avs.cnt_i[QPcount], avs.br_i[QPcount], avs.snr_Y_i[QPcount], avs.snr_U_i[QPcount], avs.snr_V_i[QPcount], avs.snr_YUV_i[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", avs.cnt_g[QPcount], avs.br_g[QPcount], avs.snr_Y_g[QPcount], avs.snr_U_g[QPcount], avs.snr_V_g[QPcount], avs.snr_YUV_g[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", avs.cnt_s[QPcount], avs.br_s[QPcount], avs.snr_Y_s[QPcount], avs.snr_U_s[QPcount], avs.snr_V_s[QPcount], avs.snr_YUV_s[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", avs.cnt_p[QPcount], avs.br_p[QPcount], avs.snr_Y_p[QPcount], avs.snr_U_p[QPcount], avs.snr_V_p[QPcount], avs.snr_YUV_p[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", avs.cnt_f[QPcount], avs.br_f[QPcount], avs.snr_Y_f[QPcount], avs.snr_U_f[QPcount], avs.snr_V_f[QPcount], avs.snr_YUV_f[QPcount]);

		for (QPcount = 0; QPcount < NoOfQP; QPcount++)
			printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n", avs.cnt_b[QPcount], avs.br_b[QPcount], avs.snr_Y_b[QPcount], avs.snr_U_b[QPcount], avs.snr_V_b[QPcount], avs.snr_YUV_b[QPcount]);
	}


	strcpy(OutFile, InputFile2);
	OutFile[strlen(OutFile) - 4] = '\0';
	strcat(OutFile, "_rdpt_");
	strcat(OutFile, Label);
	strcat(OutFile, ".txt");

	strcpy(OutFile_stat, InputFile2);
	OutFile_stat[strlen(OutFile_stat) - 4] = '\0';
	//strcat(OutFile_stat,"_stat.txt");
	strcat(OutFile_stat, "_stat_");
	strcat(OutFile_stat, Label);
	strcat(OutFile_stat, ".txt");

	fp_out = fopen(OutFile, "w");
	if (!fp_out) {
		printf("Can't open file %s!\n", OutFile);
		exit(-1);
	}

	fp_out_stat = fopen(OutFile_stat, "w");
	if (!fp_out_stat) {
		printf("Can't open file %s!\n", OutFile_stat);
		exit(-1);
	}


	time(&timer);


	strftime(buffer, 26, "%Y-%m-%d_%H:%M:%S", localtime(&timer));
	printf("%s    %s :: %s\n", buffer, InputFile1, InputFile2);
	fprintf(fp_out, "%s    %s :: %s\n", buffer, InputFile1, InputFile2);

	int valid;
	//   All Frames
	valid = 1;
	for (QPcount = 0; QPcount < NoOfQP; QPcount++) {
		if (avs.cnt[QPcount] == 0 || ed.cnt[QPcount] == 0) {
			valid = 0;
			break;
		}
	}

	printf("\nAll_Frames, YUV:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt[0], ed.cnt[0], avs.cnt[1], ed.cnt[1], avs.cnt[2], ed.cnt[2], avs.cnt[3], ed.cnt[3]);
	fprintf(fp_out, "\nAll_Frames, YUV:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt[0], ed.cnt[0], avs.cnt[1], ed.cnt[1], avs.cnt[2], ed.cnt[2], avs.cnt[3], ed.cnt[3]);
	RDPlot(avs.br, avs.snr_YUV, ed.br, ed.snr_YUV, &BD_YUV, NoOfQP, 1, valid);

	if (detail) {
		printf("\nAll_Frames, Y:\n");
		fprintf(fp_out, "\nAll_Frames, Y:\n");
	}
	RDPlot(avs.br, avs.snr_Y, ed.br, ed.snr_Y, &BD_Y, NoOfQP, detail, valid);

	if (detail) {
		printf("\nAll_Frames, U:\n");
		fprintf(fp_out, "\nAll_Frames, U:\n");
	}
	RDPlot(avs.br, avs.snr_U, ed.br, ed.snr_U, &BD_U, NoOfQP, detail, valid);

	if (detail) {
		printf("\nAll_Frames, V:\n");
		fprintf(fp_out, "\nAll_Frames, V:\n");
	}
	RDPlot(avs.br, avs.snr_V, ed.br, ed.snr_V, &BD_V, NoOfQP, detail, valid);

	// I -Frame
	valid = 1;
	for (QPcount = 0; QPcount < NoOfQP; QPcount++) {
		if (avs.cnt_i[QPcount] == 0 || ed.cnt_i[QPcount] == 0) {
			valid = 0;
			break;
		}
	}

	printf("\nI_Frames, YUV_i:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_i[0], ed.cnt_i[0], avs.cnt_i[1], ed.cnt_i[1], avs.cnt_i[2], ed.cnt_i[2], avs.cnt_i[3], ed.cnt_i[3]);
	fprintf(fp_out, "\nI_Frames, YUV_i:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_i[0], ed.cnt_i[0], avs.cnt_i[1], ed.cnt_i[1], avs.cnt_i[2], ed.cnt_i[2], avs.cnt_i[3], ed.cnt_i[3]);
	RDPlot(avs.br_i, avs.snr_YUV_i, ed.br_i, ed.snr_YUV_i, &BD_YUV_i, NoOfQP, 1, valid);

	if (detail) {
		printf("\nI_Frames, Y_i: \n");
		fprintf(fp_out, "\nI_Frames, Y_i: \n");
	}
	RDPlot(avs.br_i, avs.snr_Y_i, ed.br_i, ed.snr_Y_i, &BD_Y_i, NoOfQP, detail, valid);

	if (detail) {
		printf("\nI_Frames, U_i:\n");
		fprintf(fp_out, "\nI_Frames, U_i:\n");
	}
	RDPlot(avs.br_i, avs.snr_U_i, ed.br_i, ed.snr_U_i, &BD_U_i, NoOfQP, detail, valid);

	if (detail) {
		printf("\nI_Frames, V_i:\n");
		fprintf(fp_out, "\nI_Frames, V_i:\n");
	}
	RDPlot(avs.br_i, avs.snr_V_i, ed.br_i, ed.snr_V_i, &BD_V_i, NoOfQP, detail, valid);

	// P - Frame
	valid = 1;
	for (QPcount = 0; QPcount < NoOfQP; QPcount++) {
		if (avs.cnt_p[QPcount] == 0 || ed.cnt_p[QPcount] == 0) {
			valid = 0;
			break;
		}
	}

	printf("\nP_Frames, YUV_p:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_p[0], ed.cnt_p[0], avs.cnt_p[1], ed.cnt_p[1], avs.cnt_p[2], ed.cnt_p[2], avs.cnt_p[3], ed.cnt_p[3]);
	fprintf(fp_out, "\nP_Frames, YUV_p:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_p[0], ed.cnt_p[0], avs.cnt_p[1], ed.cnt_p[1], avs.cnt_p[2], ed.cnt_p[2], avs.cnt_p[3], ed.cnt_p[3]);
	RDPlot(avs.br_p, avs.snr_YUV_p, ed.br_p, ed.snr_YUV_p, &BD_YUV_p, NoOfQP, 1, valid);

	if (detail) {
		printf("\nP_Frames, Y_p: \n");
		fprintf(fp_out, "\nP_Frames, Y_p: \n");
	}
	RDPlot(avs.br_p, avs.snr_Y_p, ed.br_p, ed.snr_Y_p, &BD_Y_p, NoOfQP, detail, valid);

	if (detail) {
		printf("\nP_Frames, U_p:\n");
		fprintf(fp_out, "\nP_Frames, U_p:\n");
	}
	RDPlot(avs.br_p, avs.snr_U_p, ed.br_p, ed.snr_U_p, &BD_U_p, NoOfQP, detail, valid);

	if (detail) {
		printf("\nP_Frames, V_p:\n");
		fprintf(fp_out, "\nP_Frames, V_p:\n");
	}
	RDPlot(avs.br_p, avs.snr_V_p, ed.br_p, ed.snr_V_p, &BD_V_p, NoOfQP, detail, valid);

	// B -Frame
	valid = 1;
	for (QPcount = 0; QPcount < NoOfQP; QPcount++) {
		if (avs.cnt_b[QPcount] == 0 || ed.cnt_b[QPcount] == 0) {
			valid = 0;
			break;
		}
	}

	printf("\nB_Frames, YUV_b:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_b[0], ed.cnt_b[0], avs.cnt_b[1], ed.cnt_b[1], avs.cnt_b[2], ed.cnt_b[2], avs.cnt_b[3], ed.cnt_b[3]);
	fprintf(fp_out, "\nB_Frames, YUV_b:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_b[0], ed.cnt_b[0], avs.cnt_b[1], ed.cnt_b[1], avs.cnt_b[2], ed.cnt_b[2], avs.cnt_b[3], ed.cnt_b[3]);
	RDPlot(avs.br_b, avs.snr_YUV_b, ed.br_b, ed.snr_YUV_b, &BD_YUV_b, NoOfQP, 1, valid);

	if (detail) {
		printf("\nB_Frames, Y_b: \n");
		fprintf(fp_out, "\nB_Frames, Y_b: \n");
	}
	RDPlot(avs.br_b, avs.snr_Y_b, ed.br_b, ed.snr_Y_b, &BD_Y_b, NoOfQP, detail, valid);

	if (detail) {
		printf("\nB_Frames, U_b:\n");
		fprintf(fp_out, "\nB_Frames, U_b:\n");
	}
	RDPlot(avs.br_b, avs.snr_U_b, ed.br_b, ed.snr_U_b, &BD_U_b, NoOfQP, detail, valid);

	if (detail) {
		printf("\nB_Frames, V_b:\n");
		fprintf(fp_out, "\nB_Frames, V_b:\n");
	}
	RDPlot(avs.br_b, avs.snr_V_b, ed.br_b, ed.snr_V_b, &BD_V_b, NoOfQP, detail, valid);
	if (enc_f1 == 0 || enc_f2 == 0) { // AVS
		// F - Frame
		valid = 1;
		for (QPcount = 0; QPcount < NoOfQP; QPcount++) {
			if (avs.cnt_f[QPcount] == 0 || ed.cnt_f[QPcount] == 0) {
				valid = 0;
				break;
			}
		}

		printf("\nF_Frames, YUV_f:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_f[0], ed.cnt_f[0], avs.cnt_f[1], ed.cnt_f[1], avs.cnt_f[2], ed.cnt_f[2], avs.cnt_f[3], ed.cnt_f[3]);
		fprintf(fp_out, "\nF_Frames, YUV_f:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_f[0], ed.cnt_f[0], avs.cnt_f[1], ed.cnt_f[1], avs.cnt_f[2], ed.cnt_f[2], avs.cnt_f[3], ed.cnt_f[3]);
		RDPlot(avs.br_f, avs.snr_YUV_f, ed.br_f, ed.snr_YUV_f, &BD_YUV_f, NoOfQP, 1, valid);

		if (detail) {
			printf("\nF_Frames, Y_f: \n");
			fprintf(fp_out, "\nP_Frames, Y_f: \n");
		}
		RDPlot(avs.br_f, avs.snr_Y_f, ed.br_f, ed.snr_Y_f, &BD_Y_f, NoOfQP, detail, valid);

		if (detail) {
			printf("\nF_Frames, U_f:\n");
			fprintf(fp_out, "\nP_Frames, U_f:\n");
		}
		RDPlot(avs.br_f, avs.snr_U_f, ed.br_f, ed.snr_U_f, &BD_U_f, NoOfQP, detail, valid);

		if (detail) {
			printf("\nF_Frames, V_f:\n");
			fprintf(fp_out, "\nP_Frames, V_f:\n");
		}
		RDPlot(avs.br_f, avs.snr_V_f, ed.br_f, ed.snr_V_f, &BD_V_f, NoOfQP, detail, valid);


		strcpy(StatName, InputFile1);
		StatName[strlen(StatName) - 4] = '\0';

		pStatName = &StatName[0];
		pStatName = strstr(StatName, "_") + 1;
		if (pStatName == NULL) {
			printf("File format wrong!\n");
			exit(-1);
		}

		// G -Frame
		valid = 1;
		for (QPcount = 0; QPcount < NoOfQP; QPcount++) {
			if (avs.cnt_g[QPcount] == 0 || ed.cnt_g[QPcount] == 0) {
				valid = 0;
				break;
			}
		}

		printf("\nG_Frames, YUV_g:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_g[0], ed.cnt_g[0], avs.cnt_g[1], ed.cnt_g[1], avs.cnt_g[2], ed.cnt_g[2], avs.cnt_g[3], ed.cnt_g[3]);
		fprintf(fp_out, "\nG_Frames, YUV_g:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_g[0], ed.cnt_g[0], avs.cnt_g[1], ed.cnt_g[1], avs.cnt_g[2], ed.cnt_g[2], avs.cnt_g[3], ed.cnt_g[3]);
		RDPlot(avs.br_g, avs.snr_YUV_g, ed.br_g, ed.snr_YUV_g, &BD_YUV_g, NoOfQP, 1, valid);

		if (detail) {
			printf("\nG_Frames, Y_g: \n");
			fprintf(fp_out, "\nG_Frames, Y_g: \n");
		}
		RDPlot(avs.br_g, avs.snr_Y_g, ed.br_g, ed.snr_Y_g, &BD_Y_g, NoOfQP, detail, valid);

		if (detail) {
			printf("\nG_Frames, U_g:\n");
			fprintf(fp_out, "\nG_Frames, U_g:\n");
		}
		RDPlot(avs.br_g, avs.snr_U_g, ed.br_g, ed.snr_U_g, &BD_U_g, NoOfQP, detail, valid);

		if (detail) {
			printf("\nG_Frames, V_g:\n");
			fprintf(fp_out, "\nG_Frames, V_g:\n");
		}
		RDPlot(avs.br_g, avs.snr_V_g, ed.br_g, ed.snr_V_g, &BD_V_g, NoOfQP, detail, valid);

		// S - Frame
		valid = 1;
		for (QPcount = 0; QPcount < NoOfQP; QPcount++) {
			if (avs.cnt_s[QPcount] == 0 || ed.cnt_s[QPcount] == 0) {
				valid = 0;
				break;
			}
		}

		printf("\nS_Frames, YUV_s:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_s[0], ed.cnt_s[0], avs.cnt_s[1], ed.cnt_s[1], avs.cnt_s[2], ed.cnt_s[2], avs.cnt_s[3], ed.cnt_s[3]);
		fprintf(fp_out, "\nS_Frames, YUV_s:,%d %d | %d %d | %d %d | %d %d |\n", avs.cnt_s[0], ed.cnt_s[0], avs.cnt_s[1], ed.cnt_s[1], avs.cnt_s[2], ed.cnt_s[2], avs.cnt_s[3], ed.cnt_s[3]);
		RDPlot(avs.br_s, avs.snr_YUV_s, ed.br_s, ed.snr_YUV_s, &BD_YUV_s, NoOfQP, 1, valid);

		if (detail) {
			printf("\nS_Frames, Y_s: \n");
			fprintf(fp_out, "\nS_Frames, Y_s: \n");
		}
		RDPlot(avs.br_s, avs.snr_Y_s, ed.br_s, ed.snr_Y_s, &BD_Y_s, NoOfQP, detail, valid);

		if (detail) {
			printf("\nS_Frames, U_s:\n");
			fprintf(fp_out, "\nS_Frames, U_s:\n");
		}
		RDPlot(avs.br_s, avs.snr_U_s, ed.br_s, ed.snr_U_s, &BD_U_s, NoOfQP, detail, valid);

		if (detail) {
			printf("\nS_Frames, V_s:\n");
			fprintf(fp_out, "\nS_Frames, V_s:\n");
		}
		RDPlot(avs.br_s, avs.snr_V_s, ed.br_s, ed.snr_V_s, &BD_V_s, NoOfQP, detail, valid);
	}

	//	strcpy(StatName,pStatName);


	fprintf(fp_out_stat, "%s,%s %s,", buffer, InputFile1, InputFile2);
	fprintf(fp_out_stat, "%7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f,",
		BD_YUV.Br_aver, BD_Y.Br_aver, BD_U.Br_aver, BD_V.Br_aver,
		BD_YUV_i.Br_aver, BD_Y_i.Br_aver, BD_U_i.Br_aver, BD_V_i.Br_aver,
		BD_YUV_p.Br_aver, BD_Y_p.Br_aver, BD_U_p.Br_aver, BD_V_p.Br_aver,
		BD_YUV_b.Br_aver, BD_Y_b.Br_aver, BD_U_b.Br_aver, BD_V_b.Br_aver,
		BD_YUV_f.Br_aver, BD_Y_f.Br_aver, BD_U_f.Br_aver, BD_V_f.Br_aver,
		BD_YUV_g.Br_aver, BD_Y_g.Br_aver, BD_U_g.Br_aver, BD_V_g.Br_aver,
		BD_YUV_s.Br_aver, BD_Y_s.Br_aver, BD_U_s.Br_aver, BD_V_s.Br_aver);

	fprintf(fp_out_stat, "%7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f\n",
		BD_YUV.PSNR_aver, BD_Y.PSNR_aver, BD_U.PSNR_aver, BD_V.PSNR_aver,
		BD_YUV_i.PSNR_aver, BD_Y_i.PSNR_aver, BD_U_i.PSNR_aver, BD_V_i.PSNR_aver,
		BD_YUV_p.PSNR_aver, BD_Y_p.PSNR_aver, BD_U_p.PSNR_aver, BD_V_p.PSNR_aver,
		BD_YUV_b.PSNR_aver, BD_Y_b.PSNR_aver, BD_U_b.PSNR_aver, BD_V_b.PSNR_aver,
		BD_YUV_f.PSNR_aver, BD_Y_f.PSNR_aver, BD_U_f.PSNR_aver, BD_V_f.PSNR_aver,
		BD_YUV_g.PSNR_aver, BD_Y_g.PSNR_aver, BD_U_g.PSNR_aver, BD_V_g.PSNR_aver,
		BD_YUV_s.PSNR_aver, BD_Y_s.PSNR_aver, BD_U_s.PSNR_aver, BD_V_s.PSNR_aver);

	fclose(fp_out);
	fclose(fp_out_stat);
	return 0;
}
