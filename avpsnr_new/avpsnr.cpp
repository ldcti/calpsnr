#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "avpsnr.h"

void ReadOneLine(FILE *fp, char *Oneline){
	char c1;
	int i = 0;

	c1 = fgetc(fp);
	//	while (!(c1 == 0x0D && c2 == 0x0A))
	while (c1 != 0x0A && i<127)
	{
		Oneline[i] = c1;
		c1 = fgetc(fp);
		i++;
	}
	Oneline[i] = '\0';
}
void ReadOneLine_ep(FILE *fp, char *Oneline){
	char c1;
	int i = 0;

	c1 = fgetc(fp);
	while (c1 != 0x0A)
	{
		if (i == 158)
			i = 158;
		Oneline[i] = c1;
		c1 = fgetc(fp);
		i++;
	}
	Oneline[i] = '\0';
}

void Reading_HM(char HevcHmFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP) {
	FILE *fp_ref1;

	char *Oneline = new char[180];
	int i, QPcnt, tmp;
	char TmpS[128];
	char PType[128];
	float x1, x2, x3, x4;
	double *Bitrate, *Psnr_Y, *Psnr_U, *Psnr_V, *Psnr_YUV;
	char **IBP;


	IBP = new char*[NUM];
	for (i = 0; i<NUM; i++) {
		IBP[i] = new char[120];
	}

	Bitrate = new double[NUM];
	Psnr_Y = new double[NUM];
	Psnr_U = new double[NUM];
	Psnr_V = new double[NUM];
	Psnr_YUV = new double[NUM];
	memset(ped, 0, sizeof(AVS_CTX));

	fp_ref1 = fopen(HevcHmFile, "r");
	if (!fp_ref1) {
		printf("Can't open file %s!\n", HevcHmFile);
		exit(-1);
	}

	// Reading First QP
	for (QPcnt = 0; QPcnt < NoOfQP; QPcnt++) {
		while (!feof(fp_ref1)) {
			ReadOneLine(fp_ref1, Oneline);
			if (strstr(Oneline, "Output (internal) ChromaFormatIDC =   4:2:0") != NULL)
				break;
		}

		while (!feof(fp_ref1))
		{
			ReadOneLine(fp_ref1, Oneline);
			if (Oneline[0] != '\0') {
				sscanf(Oneline, "%s %d %s %d %s %s %s %d %s %d %s %f %s %s %f %s %s %f %s %s %f", TmpS, &tmp, TmpS, &tmp, TmpS, PType, TmpS, &tmp, TmpS, &tmp, TmpS, &x1, TmpS, TmpS, &x2, TmpS, TmpS, &x3, TmpS, TmpS, &x4);
				strcpy(IBP[ped->cnt[QPcnt]], PType);
				Bitrate[ped->cnt[QPcnt]] = x1;
				Psnr_Y[ped->cnt[QPcnt]] = x2;
				Psnr_U[ped->cnt[QPcnt]] = x3;
				Psnr_V[ped->cnt[QPcnt]] = x4;

#if 0 //HEVC
				x2 = (255 * 255) / pow(10.0, (double)x2 / 10);
				x3 = (255 * 255) / pow(10.0, (double)x3 / 10);
				x4 = (255 * 255) / pow(10.0, (double)x4 / 10);
				Psnr_YUV[ped->cnt[QPcnt]] = (x2 * 4 + x3 + x4) / 6;
				Psnr_YUV[ped->cnt[QPcnt]] = 10 * log10(255.0*255.0 / Psnr_YUV[ped->cnt[QPcnt]]);
#else
				Psnr_YUV[ped->cnt[QPcnt]] = (Psnr_Y[ped->cnt[QPcnt]] * 4 + Psnr_U[ped->cnt[QPcnt]] + Psnr_V[ped->cnt[QPcnt]]) / 6;
#endif
				ped->cnt[QPcnt]++;
			}
			else
				break;
		}

		if (((NoOfFrames != 0) && NoOfFrames <= ped->cnt[QPcnt])) {
			ped->cnt[QPcnt] = NoOfFrames;
		}
		else {
			printf("NoOfFrames error:- File=%s, QPcnt=%d: NoOfFrames = %d, ActualFrame = %d, exit!\n", HevcHmFile, QPcnt, NoOfFrames, ped->cnt[QPcnt]);
			exit(-1);
		}
		for (i = 0; i<ped->cnt[QPcnt]; i++) {
			ped->br[QPcnt] += Bitrate[i];
			ped->snr_Y[QPcnt] += Psnr_Y[i];
			ped->snr_U[QPcnt] += Psnr_U[i];
			ped->snr_V[QPcnt] += Psnr_V[i];
			ped->snr_YUV[QPcnt] += Psnr_YUV[i];
			if (strstr(IBP[i], "I-SLICE") != NULL) {
				ped->br_i[QPcnt] += Bitrate[i];
				ped->snr_Y_i[QPcnt] += Psnr_Y[i];
				ped->snr_U_i[QPcnt] += Psnr_U[i];
				ped->snr_V_i[QPcnt] += Psnr_V[i];
				ped->snr_YUV_i[QPcnt] += Psnr_YUV[i];
				ped->cnt_i[QPcnt]++;
			}
			else if (strstr(IBP[i], "P-SLICE") != NULL) {
				ped->br_p[QPcnt] += Bitrate[i];
				ped->snr_Y_p[QPcnt] += Psnr_Y[i];
				ped->snr_U_p[QPcnt] += Psnr_U[i];
				ped->snr_V_p[QPcnt] += Psnr_V[i];
				ped->snr_YUV_p[QPcnt] += Psnr_YUV[i];
				ped->cnt_p[QPcnt]++;
			}
			else if (strstr(IBP[i], "B-SLICE") != NULL) {
				ped->br_b[QPcnt] += Bitrate[i];
				ped->snr_Y_b[QPcnt] += Psnr_Y[i];
				ped->snr_U_b[QPcnt] += Psnr_U[i];
				ped->snr_V_b[QPcnt] += Psnr_V[i];
				ped->snr_YUV_b[QPcnt] += Psnr_YUV[i];
				ped->cnt_b[QPcnt]++;
			}
			else {
				printf("Unknown picture type!\n");
			}
		}

		if (FrameRate == 0)
			FrameRate = 30;
		if (ped->cnt[QPcnt]) {
			ped->br[QPcnt] = ped->br[QPcnt] / ped->cnt[QPcnt] * FrameRate / 1024 / 1024;
			ped->snr_Y[QPcnt] = ped->snr_Y[QPcnt] / ped->cnt[QPcnt];
			ped->snr_U[QPcnt] = ped->snr_U[QPcnt] / ped->cnt[QPcnt];
			ped->snr_V[QPcnt] = ped->snr_V[QPcnt] / ped->cnt[QPcnt];
			ped->snr_YUV[QPcnt] = ped->snr_YUV[QPcnt] / ped->cnt[QPcnt];
			//				printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt, ped->br[QPcnt], ped->snr_Y[QPcnt], ped->snr_U[QPcnt], ped->snr_V[QPcnt], ped->snr_YUV[QPcnt]);
			if (ped->cnt_i[QPcnt]) {
				ped->br_i[QPcnt] = ped->br_i[QPcnt] / ped->cnt_i[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_i[QPcnt] = ped->snr_Y_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_U_i[QPcnt] = ped->snr_U_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_V_i[QPcnt] = ped->snr_V_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_YUV_i[QPcnt] = ped->snr_YUV_i[QPcnt] / ped->cnt_i[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_i, ped->br_i[QPcnt], ped->snr_Y_i[QPcnt], ped->snr_U_i[QPcnt], ped->snr_V_i[QPcnt], ped->snr_YUV_i[QPcnt]);
			}
			if (ped->cnt_p[QPcnt]) {
				ped->br_p[QPcnt] = ped->br_p[QPcnt] / ped->cnt_p[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_p[QPcnt] = ped->snr_Y_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_U_p[QPcnt] = ped->snr_U_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_V_p[QPcnt] = ped->snr_V_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_YUV_p[QPcnt] = ped->snr_YUV_p[QPcnt] / ped->cnt_p[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_p, ped->br_p[QPcnt], ped->snr_Y_p[QPcnt], ped->snr_U_p[QPcnt], ped->snr_V_p[QPcnt], ped->snr_YUV_p[QPcnt]);
			}
			if (ped->cnt_b[QPcnt]) {
				ped->br_b[QPcnt] = ped->br_b[QPcnt] / ped->cnt_b[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_b[QPcnt] = ped->snr_Y_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_U_b[QPcnt] = ped->snr_U_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_V_b[QPcnt] = ped->snr_V_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_YUV_b[QPcnt] = ped->snr_YUV_b[QPcnt] / ped->cnt_b[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_b, ped->br_b[QPcnt], ped->snr_Y_b[QPcnt], ped->snr_U_b[QPcnt], ped->snr_V_b[QPcnt], ped->snr_YUV_b[QPcnt]);
			}
		}

	} // for QPcnt

	delete Oneline;
	delete Bitrate;
	delete Psnr_Y;
	delete Psnr_U;
	delete Psnr_V;
	delete Psnr_YUV;

	for (i = 0; i<NUM; i++) {
		delete IBP[i];
	}
	delete IBP;
	fclose(fp_ref1);
}
void Reading_Lion_AVS(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP) {
	FILE *fp_ref1;
	char *Oneline = new char[128];
	int i, QPcnt, tmp;
	char TmpS[128];
	float x1, x2, x3, x4;
	double *Bitrate, *Psnr_Y, *Psnr_U, *Psnr_V, *Psnr_YUV;
	char **IBP;

	IBP = new char*[NUM];
	for (i = 0; i<NUM; i++) {
		IBP[i] = new char[120];
	}

	Bitrate = new double[NUM];
	Psnr_Y = new double[NUM];
	Psnr_U = new double[NUM];
	Psnr_V = new double[NUM];
	Psnr_YUV = new double[NUM];
	memset(ped, 0, sizeof(AVS_CTX));

	fp_ref1 = fopen(LionFile, "r");
	if (!fp_ref1) {
		printf("Can't open file %s!\n", LionFile);
		exit(-1);
	}

	// Reading First QP
	for (QPcnt = 0; QPcnt<NoOfQP; QPcnt++) {
		while (!feof(fp_ref1)) {
			ReadOneLine(fp_ref1, Oneline);
			if (strstr(Oneline, "#Frame     QP   Bit/au  Bit/pic     SnrY       SnrU       SnrV") != NULL)
				break;
		}

		while (!feof(fp_ref1))
		{
			ReadOneLine(fp_ref1, Oneline);
			if (Oneline[0] != '\0') {
				sscanf(Oneline, "%s %d %f %d %f %f %f", TmpS, &tmp, &x1, &tmp, &x2, &x3, &x4);
				strcpy(IBP[ped->cnt[QPcnt]], TmpS);
				Bitrate[ped->cnt[QPcnt]] = x1;
				Psnr_Y[ped->cnt[QPcnt]] = x2;
				Psnr_U[ped->cnt[QPcnt]] = x3;
				Psnr_V[ped->cnt[QPcnt]] = x4;
				Psnr_YUV[ped->cnt[QPcnt]] = (Psnr_Y[ped->cnt[QPcnt]] * 4 + Psnr_U[ped->cnt[QPcnt]] + Psnr_V[ped->cnt[QPcnt]]) / 6;
				//			printf("%s %12.0f %7.3f %7.3f %7.3f %7.3f\n", IBP[ped->cnt], Bitrate[ped->cnt], Psnr_Y[ped->cnt], Psnr_U[ped->cnt], Psnr_V[ped->cnt], Psnr_YUV[ped->cnt]);
				ped->cnt[QPcnt]++;
			}
			else
				break;
		}

		if (((NoOfFrames != 0) && NoOfFrames <= ped->cnt[QPcnt])) {
			ped->cnt[QPcnt] = NoOfFrames;
		}
		else {
			printf("NoOfFrames error:- File=%s, QPcnt=%d: NoOfFrames = %d, ActualFrame = %d, exit!\n", LionFile, QPcnt, NoOfFrames, ped->cnt[QPcnt]);
			exit(-1);
		}
		for (i = 0; i<ped->cnt[QPcnt]; i++) {
			ped->br[QPcnt] += Bitrate[i];
			ped->snr_Y[QPcnt] += Psnr_Y[i];
			ped->snr_U[QPcnt] += Psnr_U[i];
			ped->snr_V[QPcnt] += Psnr_V[i];
			ped->snr_YUV[QPcnt] += Psnr_YUV[i];

			if (strstr(IBP[i], "I") != NULL) {
				ped->br_i[QPcnt] += Bitrate[i];
				ped->snr_Y_i[QPcnt] += Psnr_Y[i];
				ped->snr_U_i[QPcnt] += Psnr_U[i];
				ped->snr_V_i[QPcnt] += Psnr_V[i];
				ped->snr_YUV_i[QPcnt] += Psnr_YUV[i];
				ped->cnt_i[QPcnt]++;
			}
			else if (strstr(IBP[i], "G") != NULL) {
				ped->br_g[QPcnt] += Bitrate[i];
				ped->snr_Y_g[QPcnt] += Psnr_Y[i];
				ped->snr_U_g[QPcnt] += Psnr_U[i];
				ped->snr_V_g[QPcnt] += Psnr_V[i];
				ped->snr_YUV_g[QPcnt] += Psnr_YUV[i];
				ped->cnt_g[QPcnt]++;
			}
			else if (strstr(IBP[i], "S") != NULL) {
				ped->br_s[QPcnt] += Bitrate[i];
				ped->snr_Y_s[QPcnt] += Psnr_Y[i];
				ped->snr_U_s[QPcnt] += Psnr_U[i];
				ped->snr_V_s[QPcnt] += Psnr_V[i];
				ped->snr_YUV_s[QPcnt] += Psnr_YUV[i];
				ped->cnt_s[QPcnt]++;
			}
			else if (strstr(IBP[i], "P") != NULL) {
				ped->br_p[QPcnt] += Bitrate[i];
				ped->snr_Y_p[QPcnt] += Psnr_Y[i];
				ped->snr_U_p[QPcnt] += Psnr_U[i];
				ped->snr_V_p[QPcnt] += Psnr_V[i];
				ped->snr_YUV_p[QPcnt] += Psnr_YUV[i];
				ped->cnt_p[QPcnt]++;
			}
			else if (strstr(IBP[i], "F") != NULL) {
				ped->br_f[QPcnt] += Bitrate[i];
				ped->snr_Y_f[QPcnt] += Psnr_Y[i];
				ped->snr_U_f[QPcnt] += Psnr_U[i];
				ped->snr_V_f[QPcnt] += Psnr_V[i];
				ped->snr_YUV_f[QPcnt] += Psnr_YUV[i];
				ped->cnt_f[QPcnt]++;

			}
			else if (strstr(IBP[i], "B") != NULL) {
				ped->br_b[QPcnt] += Bitrate[i];
				ped->snr_Y_b[QPcnt] += Psnr_Y[i];
				ped->snr_U_b[QPcnt] += Psnr_U[i];
				ped->snr_V_b[QPcnt] += Psnr_V[i];
				ped->snr_YUV_b[QPcnt] += Psnr_YUV[i];
				ped->cnt_b[QPcnt]++;
			}
			else {
				printf("Unknown picture type!\n");
			}
		}

		if (FrameRate == 0)
			FrameRate = 30;
		if (ped->cnt[QPcnt]) {
			ped->br[QPcnt] = ped->br[QPcnt] / ped->cnt[QPcnt] * FrameRate / 1024 / 1024;
			ped->snr_Y[QPcnt] = ped->snr_Y[QPcnt] / ped->cnt[QPcnt];
			ped->snr_U[QPcnt] = ped->snr_U[QPcnt] / ped->cnt[QPcnt];
			ped->snr_V[QPcnt] = ped->snr_V[QPcnt] / ped->cnt[QPcnt];
			ped->snr_YUV[QPcnt] = ped->snr_YUV[QPcnt] / ped->cnt[QPcnt];
			//				printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt, ped->br[QPcnt], ped->snr_Y[QPcnt], ped->snr_U[QPcnt], ped->snr_V[QPcnt], ped->snr_YUV[QPcnt]);
			if (ped->cnt_i[QPcnt]) {
				ped->br_i[QPcnt] = ped->br_i[QPcnt] / ped->cnt_i[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_i[QPcnt] = ped->snr_Y_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_U_i[QPcnt] = ped->snr_U_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_V_i[QPcnt] = ped->snr_V_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_YUV_i[QPcnt] = ped->snr_YUV_i[QPcnt] / ped->cnt_i[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_i, ped->br_i[QPcnt], ped->snr_Y_i[QPcnt], ped->snr_U_i[QPcnt], ped->snr_V_i[QPcnt], ped->snr_YUV_i[QPcnt]);
			}
			if (ped->cnt_g[QPcnt]) {
				ped->br_g[QPcnt] = ped->br_g[QPcnt] / ped->cnt_g[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_g[QPcnt] = ped->snr_Y_g[QPcnt] / ped->cnt_g[QPcnt];
				ped->snr_U_g[QPcnt] = ped->snr_U_g[QPcnt] / ped->cnt_g[QPcnt];
				ped->snr_V_g[QPcnt] = ped->snr_V_g[QPcnt] / ped->cnt_g[QPcnt];
				ped->snr_YUV_g[QPcnt] = ped->snr_YUV_g[QPcnt] / ped->cnt_g[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_g, ped->br_g[QPcnt], ped->snr_Y_g[QPcnt], ped->snr_U_g[QPcnt], ped->snr_V_g[QPcnt], ped->snr_YUV_g[QPcnt]);
			}
			if (ped->cnt_s[QPcnt]) {
				ped->br_s[QPcnt] = ped->br_s[QPcnt] / ped->cnt_s[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_s[QPcnt] = ped->snr_Y_s[QPcnt] / ped->cnt_s[QPcnt];
				ped->snr_U_s[QPcnt] = ped->snr_U_s[QPcnt] / ped->cnt_s[QPcnt];
				ped->snr_V_s[QPcnt] = ped->snr_V_s[QPcnt] / ped->cnt_s[QPcnt];
				ped->snr_YUV_s[QPcnt] = ped->snr_YUV_s[QPcnt] / ped->cnt_s[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_s, ped->br_s[QPcnt], ped->snr_Y_s[QPcnt], ped->snr_U_s[QPcnt], ped->snr_V_s[QPcnt], ped->snr_YUV_s[QPcnt]);
			}
			if (ped->cnt_p[QPcnt]) {
				ped->br_p[QPcnt] = ped->br_p[QPcnt] / ped->cnt_p[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_p[QPcnt] = ped->snr_Y_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_U_p[QPcnt] = ped->snr_U_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_V_p[QPcnt] = ped->snr_V_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_YUV_p[QPcnt] = ped->snr_YUV_p[QPcnt] / ped->cnt_p[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_p, ped->br_p[QPcnt], ped->snr_Y_p[QPcnt], ped->snr_U_p[QPcnt], ped->snr_V_p[QPcnt], ped->snr_YUV_p[QPcnt]);
			}
			if (ped->cnt_f[QPcnt]) {
				ped->br_f[QPcnt] = ped->br_f[QPcnt] / ped->cnt_f[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_f[QPcnt] = ped->snr_Y_f[QPcnt] / ped->cnt_f[QPcnt];
				ped->snr_U_f[QPcnt] = ped->snr_U_f[QPcnt] / ped->cnt_f[QPcnt];
				ped->snr_V_f[QPcnt] = ped->snr_V_f[QPcnt] / ped->cnt_f[QPcnt];
				ped->snr_YUV_f[QPcnt] = ped->snr_YUV_f[QPcnt] / ped->cnt_f[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_f, ped->br_f[QPcnt], ped->snr_Y_f[QPcnt], ped->snr_U_f[QPcnt], ped->snr_V_f[QPcnt], ped->snr_YUV_f[QPcnt]);
			}
			if (ped->cnt_b[QPcnt]) {
				ped->br_b[QPcnt] = ped->br_b[QPcnt] / ped->cnt_b[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_b[QPcnt] = ped->snr_Y_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_U_b[QPcnt] = ped->snr_U_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_V_b[QPcnt] = ped->snr_V_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_YUV_b[QPcnt] = ped->snr_YUV_b[QPcnt] / ped->cnt_b[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_b, ped->br_b[QPcnt], ped->snr_Y_b[QPcnt], ped->snr_U_b[QPcnt], ped->snr_V_b[QPcnt], ped->snr_YUV_b[QPcnt]);
			}
		}

	} // for QPcnt
	delete Oneline;
	delete Bitrate;
	delete Psnr_Y;
	delete Psnr_U;
	delete Psnr_V;
	delete Psnr_YUV;

	for (i = 0; i<NUM; i++) {
		delete IBP[i];
	}
	delete IBP;
	fclose(fp_ref1);
}
void Reading_Lion_265(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP) {
	FILE *fp_ref1;
	char *Oneline = new char[180];
	int i, QPcnt, tmp;
	char TmpS[128];
	float x1, x2, x3, x4;
	double *Bitrate, *Psnr_Y, *Psnr_U, *Psnr_V, *Psnr_YUV;
	char **IBP;

	IBP = new char*[NUM];
	for (i = 0; i<NUM; i++) {
		IBP[i] = new char[120];
	}

	Bitrate = new double[NUM];
	Psnr_Y = new double[NUM];
	Psnr_U = new double[NUM];
	Psnr_V = new double[NUM];
	Psnr_YUV = new double[NUM];
	memset(ped, 0, sizeof(AVS_CTX));

	fp_ref1 = fopen(LionFile, "r");
	if (!fp_ref1) {
		printf("Can't open file %s!\n", LionFile);
		exit(-1);
	}

	// Reading First QP
	for (QPcnt = 0; QPcnt<NoOfQP; QPcnt++) {
		while (!feof(fp_ref1)) {
			ReadOneLine(fp_ref1, Oneline);
			if (strstr(Oneline, "#Frame     QP   Bit/au  Bit/pic     SnrY       SnrU       SnrV") != NULL)
				break;
		}

		while (!feof(fp_ref1))
		{
			ReadOneLine(fp_ref1, Oneline);
			if (Oneline[0] != '\0') {
				sscanf(Oneline, "%s %d %f %d %f %f %f", TmpS, &tmp, &x1, &tmp, &x2, &x3, &x4);
				strcpy(IBP[ped->cnt[QPcnt]], TmpS);
				Bitrate[ped->cnt[QPcnt]] = x1;
				Psnr_Y[ped->cnt[QPcnt]] = x2;
				Psnr_U[ped->cnt[QPcnt]] = x3;
				Psnr_V[ped->cnt[QPcnt]] = x4;
				Psnr_YUV[ped->cnt[QPcnt]] = (Psnr_Y[ped->cnt[QPcnt]] * 4 + Psnr_U[ped->cnt[QPcnt]] + Psnr_V[ped->cnt[QPcnt]]) / 6;
				//			printf("%s %12.0f %7.3f %7.3f %7.3f %7.3f\n", IBP[ped->cnt], Bitrate[ped->cnt], Psnr_Y[ped->cnt], Psnr_U[ped->cnt], Psnr_V[ped->cnt], Psnr_YUV[ped->cnt]);
				ped->cnt[QPcnt]++;
			}
			else
				break;
		}

		if (((NoOfFrames != 0) && NoOfFrames <= ped->cnt[QPcnt])) {
			ped->cnt[QPcnt] = NoOfFrames;
		}
		else {
			printf("NoOfFrames error:- File=%s, QPcnt=%d: NoOfFrames = %d, ActualFrame = %d, exit!\n", LionFile, QPcnt, NoOfFrames, ped->cnt[QPcnt]);
			exit(-1);
		}
		for (i = 0; i<ped->cnt[QPcnt]; i++) {
			ped->br[QPcnt] += Bitrate[i];
			ped->snr_Y[QPcnt] += Psnr_Y[i];
			ped->snr_U[QPcnt] += Psnr_U[i];
			ped->snr_V[QPcnt] += Psnr_V[i];
			ped->snr_YUV[QPcnt] += Psnr_YUV[i];

			if (strstr(IBP[i], "I") != NULL) {
				ped->br_i[QPcnt] += Bitrate[i];
				ped->snr_Y_i[QPcnt] += Psnr_Y[i];
				ped->snr_U_i[QPcnt] += Psnr_U[i];
				ped->snr_V_i[QPcnt] += Psnr_V[i];
				ped->snr_YUV_i[QPcnt] += Psnr_YUV[i];
				ped->cnt_i[QPcnt]++;
			}
			else if (strstr(IBP[i], "P") != NULL) {
				ped->br_p[QPcnt] += Bitrate[i];
				ped->snr_Y_p[QPcnt] += Psnr_Y[i];
				ped->snr_U_p[QPcnt] += Psnr_U[i];
				ped->snr_V_p[QPcnt] += Psnr_V[i];
				ped->snr_YUV_p[QPcnt] += Psnr_YUV[i];
				ped->cnt_p[QPcnt]++;
			}
			else if (strstr(IBP[i], "B") != NULL) {
				ped->br_b[QPcnt] += Bitrate[i];
				ped->snr_Y_b[QPcnt] += Psnr_Y[i];
				ped->snr_U_b[QPcnt] += Psnr_U[i];
				ped->snr_V_b[QPcnt] += Psnr_V[i];
				ped->snr_YUV_b[QPcnt] += Psnr_YUV[i];
				ped->cnt_b[QPcnt]++;
			}
			else {
				printf("Unknown picture type!\n");
			}
		}

		if (FrameRate == 0)
			FrameRate = 30;
		if (ped->cnt[QPcnt]) {
			ped->br[QPcnt] = ped->br[QPcnt] / ped->cnt[QPcnt] * FrameRate / 1024 / 1024;
			ped->snr_Y[QPcnt] = ped->snr_Y[QPcnt] / ped->cnt[QPcnt];
			ped->snr_U[QPcnt] = ped->snr_U[QPcnt] / ped->cnt[QPcnt];
			ped->snr_V[QPcnt] = ped->snr_V[QPcnt] / ped->cnt[QPcnt];
			ped->snr_YUV[QPcnt] = ped->snr_YUV[QPcnt] / ped->cnt[QPcnt];
			//				printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt, ped->br[QPcnt], ped->snr_Y[QPcnt], ped->snr_U[QPcnt], ped->snr_V[QPcnt], ped->snr_YUV[QPcnt]);
			if (ped->cnt_i[QPcnt]) {
				ped->br_i[QPcnt] = ped->br_i[QPcnt] / ped->cnt_i[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_i[QPcnt] = ped->snr_Y_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_U_i[QPcnt] = ped->snr_U_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_V_i[QPcnt] = ped->snr_V_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_YUV_i[QPcnt] = ped->snr_YUV_i[QPcnt] / ped->cnt_i[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_i, ped->br_i[QPcnt], ped->snr_Y_i[QPcnt], ped->snr_U_i[QPcnt], ped->snr_V_i[QPcnt], ped->snr_YUV_i[QPcnt]);
			}
			if (ped->cnt_p[QPcnt]) {
				ped->br_p[QPcnt] = ped->br_p[QPcnt] / ped->cnt_p[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_p[QPcnt] = ped->snr_Y_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_U_p[QPcnt] = ped->snr_U_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_V_p[QPcnt] = ped->snr_V_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_YUV_p[QPcnt] = ped->snr_YUV_p[QPcnt] / ped->cnt_p[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_p, ped->br_p[QPcnt], ped->snr_Y_p[QPcnt], ped->snr_U_p[QPcnt], ped->snr_V_p[QPcnt], ped->snr_YUV_p[QPcnt]);
			}
			if (ped->cnt_b[QPcnt]) {
				ped->br_b[QPcnt] = ped->br_b[QPcnt] / ped->cnt_b[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_b[QPcnt] = ped->snr_Y_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_U_b[QPcnt] = ped->snr_U_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_V_b[QPcnt] = ped->snr_V_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_YUV_b[QPcnt] = ped->snr_YUV_b[QPcnt] / ped->cnt_b[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_b, ped->br_b[QPcnt], ped->snr_Y_b[QPcnt], ped->snr_U_b[QPcnt], ped->snr_V_b[QPcnt], ped->snr_YUV_b[QPcnt]);
			}
		}

	} // for QPcnt
	delete Oneline;
	delete Bitrate;
	delete Psnr_Y;
	delete Psnr_U;
	delete Psnr_V;
	delete Psnr_YUV;

	for (i = 0; i<NUM; i++) {
		delete IBP[i];
	}
	delete IBP;
	fclose(fp_ref1);
}
void Reading_x264(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP) {
	FILE *fp_ref1;
	char *Oneline = new char[1024];
	int i, QPcnt;;
	char TmpS[128];
	double *Bitrate, *Psnr_Y, *Psnr_U, *Psnr_V, *Psnr_YUV;
	char **IBP;

	char PType[100], Bits[100], YPSNR[100], UPSNR[100], VPSNR[100];

	IBP = new char*[NUM];
	for (i = 0; i<NUM; i++) {
		IBP[i] = new char[120];
	}

	Bitrate = new double[NUM];
	Psnr_Y = new double[NUM];
	Psnr_U = new double[NUM];
	Psnr_V = new double[NUM];
	Psnr_YUV = new double[NUM];
	memset(ped, 0, sizeof(AVS_CTX));

	fp_ref1 = fopen(LionFile, "r");
	if (!fp_ref1) {
		printf("Can't open file %s!\n", LionFile);
		exit(-1);
	}

	// Reading First QP
	for (QPcnt = 0; QPcnt<NoOfQP; QPcnt++) {
		while (!feof(fp_ref1)) {
			ReadOneLine_ep(fp_ref1, Oneline);
			if (strstr(Oneline, "x264 [info]: profile High") != NULL)
				break;
		}

		while (!feof(fp_ref1))
		{
			ReadOneLine_ep(fp_ref1, Oneline);
			if (Oneline[0] != '\0') {
				sscanf(Oneline, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s  ", TmpS, TmpS, TmpS, TmpS, TmpS, TmpS, PType, TmpS, TmpS, TmpS, TmpS, Bits, TmpS, TmpS, YPSNR, UPSNR, VPSNR);

				//				sscanf(Oneline, "%s %d %f %d %f %f %f", TmpS, &tmp, &x1, &tmp, &x2, &x3, &x4);
				strcpy(IBP[ped->cnt[QPcnt]], PType);
				Bitrate[ped->cnt[QPcnt]] = (atof(Bits + 5))*8; //convert bytes to bits
				Psnr_Y[ped->cnt[QPcnt]] = atof(YPSNR + 2);
				Psnr_U[ped->cnt[QPcnt]] = atof(UPSNR + 2);
				Psnr_V[ped->cnt[QPcnt]] = atof(VPSNR + 2);
				Psnr_YUV[ped->cnt[QPcnt]] = (Psnr_Y[ped->cnt[QPcnt]] * 4 + Psnr_U[ped->cnt[QPcnt]] + Psnr_V[ped->cnt[QPcnt]]) / 6;
				//			printf("%s %12.0f %7.3f %7.3f %7.3f %7.3f\n", IBP[ped->cnt], Bitrate[ped->cnt], Psnr_Y[ped->cnt], Psnr_U[ped->cnt], Psnr_V[ped->cnt], Psnr_YUV[ped->cnt]);
				ped->cnt[QPcnt]++;
			}
			else
				break;
		}

		if (((NoOfFrames != 0) && NoOfFrames <= ped->cnt[QPcnt])) {
			ped->cnt[QPcnt] = NoOfFrames;
		}
		else {
			printf("NoOfFrames error:- File=%s, QPcnt=%d: NoOfFrames = %d, ActualFrame = %d, exit!\n", LionFile, QPcnt, NoOfFrames, ped->cnt[QPcnt]);
			exit(-1);
		}
		for (i = 0; i<ped->cnt[QPcnt]; i++) {
			ped->br[QPcnt] += Bitrate[i];
			ped->snr_Y[QPcnt] += Psnr_Y[i];
			ped->snr_U[QPcnt] += Psnr_U[i];
			ped->snr_V[QPcnt] += Psnr_V[i];
			ped->snr_YUV[QPcnt] += Psnr_YUV[i];

			if (strstr(IBP[i], "Slice:I") != NULL) {
				ped->br_i[QPcnt] += Bitrate[i];
				ped->snr_Y_i[QPcnt] += Psnr_Y[i];
				ped->snr_U_i[QPcnt] += Psnr_U[i];
				ped->snr_V_i[QPcnt] += Psnr_V[i];
				ped->snr_YUV_i[QPcnt] += Psnr_YUV[i];
				ped->cnt_i[QPcnt]++;
			}
			else if (strstr(IBP[i], "Slice:P") != NULL) {
				ped->br_p[QPcnt] += Bitrate[i];
				ped->snr_Y_p[QPcnt] += Psnr_Y[i];
				ped->snr_U_p[QPcnt] += Psnr_U[i];
				ped->snr_V_p[QPcnt] += Psnr_V[i];
				ped->snr_YUV_p[QPcnt] += Psnr_YUV[i];
				ped->cnt_p[QPcnt]++;
			}
			else if (strstr(IBP[i], "Slice:B") != NULL || strstr(IBP[i], "Slice:b") != NULL) {
				ped->br_b[QPcnt] += Bitrate[i];
				ped->snr_Y_b[QPcnt] += Psnr_Y[i];
				ped->snr_U_b[QPcnt] += Psnr_U[i];
				ped->snr_V_b[QPcnt] += Psnr_V[i];
				ped->snr_YUV_b[QPcnt] += Psnr_YUV[i];
				ped->cnt_b[QPcnt]++;
			}
			else {
				printf("Unknown picture type!\n");
			}
		}

		if (FrameRate == 0)
			FrameRate = 30;
		if (ped->cnt[QPcnt]) {
			ped->br[QPcnt] = ped->br[QPcnt] / ped->cnt[QPcnt] * FrameRate / 1024 / 1024;
			ped->snr_Y[QPcnt] = ped->snr_Y[QPcnt] / ped->cnt[QPcnt];
			ped->snr_U[QPcnt] = ped->snr_U[QPcnt] / ped->cnt[QPcnt];
			ped->snr_V[QPcnt] = ped->snr_V[QPcnt] / ped->cnt[QPcnt];
			ped->snr_YUV[QPcnt] = ped->snr_YUV[QPcnt] / ped->cnt[QPcnt];
			//				printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt, ped->br[QPcnt], ped->snr_Y[QPcnt], ped->snr_U[QPcnt], ped->snr_V[QPcnt], ped->snr_YUV[QPcnt]);
			if (ped->cnt_i[QPcnt]) {
				ped->br_i[QPcnt] = ped->br_i[QPcnt] / ped->cnt_i[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_i[QPcnt] = ped->snr_Y_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_U_i[QPcnt] = ped->snr_U_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_V_i[QPcnt] = ped->snr_V_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_YUV_i[QPcnt] = ped->snr_YUV_i[QPcnt] / ped->cnt_i[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_i, ped->br_i[QPcnt], ped->snr_Y_i[QPcnt], ped->snr_U_i[QPcnt], ped->snr_V_i[QPcnt], ped->snr_YUV_i[QPcnt]);
			}
			if (ped->cnt_p[QPcnt]) {
				ped->br_p[QPcnt] = ped->br_p[QPcnt] / ped->cnt_p[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_p[QPcnt] = ped->snr_Y_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_U_p[QPcnt] = ped->snr_U_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_V_p[QPcnt] = ped->snr_V_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_YUV_p[QPcnt] = ped->snr_YUV_p[QPcnt] / ped->cnt_p[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_p, ped->br_p[QPcnt], ped->snr_Y_p[QPcnt], ped->snr_U_p[QPcnt], ped->snr_V_p[QPcnt], ped->snr_YUV_p[QPcnt]);
			}
			if (ped->cnt_b[QPcnt]) {
				ped->br_b[QPcnt] = ped->br_b[QPcnt] / ped->cnt_b[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_b[QPcnt] = ped->snr_Y_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_U_b[QPcnt] = ped->snr_U_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_V_b[QPcnt] = ped->snr_V_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_YUV_b[QPcnt] = ped->snr_YUV_b[QPcnt] / ped->cnt_b[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_b, ped->br_b[QPcnt], ped->snr_Y_b[QPcnt], ped->snr_U_b[QPcnt], ped->snr_V_b[QPcnt], ped->snr_YUV_b[QPcnt]);
			}
		}

	} // for QPcnt
	delete Oneline;
	delete Bitrate;
	delete Psnr_Y;
	delete Psnr_U;
	delete Psnr_V;
	delete Psnr_YUV;

	for (i = 0; i<NUM; i++) {
		delete IBP[i];
	}
	delete IBP;
	fclose(fp_ref1);
}
void Reading_x265(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP) {
	FILE *fp_ref1;
	char *Oneline = new char[1024];
	int i, QPcnt;;
	char TmpS[128];
	double *Bitrate, *Psnr_Y, *Psnr_U, *Psnr_V, *Psnr_YUV;
	char **IBP;

	char PType[100], Bits[100], YPSNR[100], UPSNR[100], VPSNR[100];

	IBP = new char*[NUM];
	for (i = 0; i<NUM; i++) {
		IBP[i] = new char[120];
	}

	Bitrate = new double[NUM];
	Psnr_Y = new double[NUM];
	Psnr_U = new double[NUM];
	Psnr_V = new double[NUM];
	Psnr_YUV = new double[NUM];
	memset(ped, 0, sizeof(AVS_CTX));

	fp_ref1 = fopen(LionFile, "r");
	if (!fp_ref1) {
		printf("Can't open file %s!\n", LionFile);
		exit(-1);
	}

	// Reading First QP
	for (QPcnt = 0; QPcnt<NoOfQP; QPcnt++) {
		while (!feof(fp_ref1)) {
			ReadOneLine_ep(fp_ref1, Oneline);
			if (strstr(Oneline, "Encode Order, Type, POC, QP, Bits, Scenecut, Y PSNR, U PSNR,") != NULL)
				break;
		}

		while (!feof(fp_ref1))
		{
			ReadOneLine_ep(fp_ref1, Oneline);
			if (Oneline[0] != '\0') {
				sscanf(Oneline, "%s %s %s %s %s %s %s %s %s %s ", TmpS, PType, TmpS, TmpS, Bits, YPSNR, UPSNR, VPSNR, TmpS, TmpS);

				//				sscanf(Oneline, "%s %d %f %d %f %f %f", TmpS, &tmp, &x1, &tmp, &x2, &x3, &x4);
				strcpy(IBP[ped->cnt[QPcnt]], PType);
				Bitrate[ped->cnt[QPcnt]] = atof(Bits);
				Psnr_Y[ped->cnt[QPcnt]] = atof(YPSNR + 2);
				Psnr_U[ped->cnt[QPcnt]] = atof(UPSNR);
				Psnr_V[ped->cnt[QPcnt]] = atof(VPSNR);
				Psnr_YUV[ped->cnt[QPcnt]] = (Psnr_Y[ped->cnt[QPcnt]] * 4 + Psnr_U[ped->cnt[QPcnt]] + Psnr_V[ped->cnt[QPcnt]]) / 6;
				//			printf("%s %12.0f %7.3f %7.3f %7.3f %7.3f\n", IBP[ped->cnt], Bitrate[ped->cnt], Psnr_Y[ped->cnt], Psnr_U[ped->cnt], Psnr_V[ped->cnt], Psnr_YUV[ped->cnt]);
				ped->cnt[QPcnt]++;
			}
			else
				break;
		}

		if (((NoOfFrames != 0) && NoOfFrames <= ped->cnt[QPcnt])) {
			ped->cnt[QPcnt] = NoOfFrames;
		}
		else {
			printf("NoOfFrames error:- File=%s, QPcnt=%d: NoOfFrames = %d, ActualFrame = %d, exit!\n", LionFile, QPcnt, NoOfFrames, ped->cnt[QPcnt]);
			exit(-1);
		}
		for (i = 0; i<ped->cnt[QPcnt]; i++) {
			ped->br[QPcnt] += Bitrate[i];
			ped->snr_Y[QPcnt] += Psnr_Y[i];
			ped->snr_U[QPcnt] += Psnr_U[i];
			ped->snr_V[QPcnt] += Psnr_V[i];
			ped->snr_YUV[QPcnt] += Psnr_YUV[i];

			if (strstr(IBP[i], "I-SLICE") != NULL) {
				ped->br_i[QPcnt] += Bitrate[i];
				ped->snr_Y_i[QPcnt] += Psnr_Y[i];
				ped->snr_U_i[QPcnt] += Psnr_U[i];
				ped->snr_V_i[QPcnt] += Psnr_V[i];
				ped->snr_YUV_i[QPcnt] += Psnr_YUV[i];
				ped->cnt_i[QPcnt]++;
			}
			else if (strstr(IBP[i], "P-SLICE") != NULL) {
				ped->br_p[QPcnt] += Bitrate[i];
				ped->snr_Y_p[QPcnt] += Psnr_Y[i];
				ped->snr_U_p[QPcnt] += Psnr_U[i];
				ped->snr_V_p[QPcnt] += Psnr_V[i];
				ped->snr_YUV_p[QPcnt] += Psnr_YUV[i];
				ped->cnt_p[QPcnt]++;
			}
			else if (strstr(IBP[i], "B-SLICE") != NULL || strstr(IBP[i], "b-SLICE") != NULL) {
				ped->br_b[QPcnt] += Bitrate[i];
				ped->snr_Y_b[QPcnt] += Psnr_Y[i];
				ped->snr_U_b[QPcnt] += Psnr_U[i];
				ped->snr_V_b[QPcnt] += Psnr_V[i];
				ped->snr_YUV_b[QPcnt] += Psnr_YUV[i];
				ped->cnt_b[QPcnt]++;
			}
			else {
				printf("Unknown picture type!\n");
			}
		}

		if (FrameRate == 0)
			FrameRate = 30;
		if (ped->cnt[QPcnt]) {
			ped->br[QPcnt] = ped->br[QPcnt] / ped->cnt[QPcnt] * FrameRate / 1024 / 1024;
			ped->snr_Y[QPcnt] = ped->snr_Y[QPcnt] / ped->cnt[QPcnt];
			ped->snr_U[QPcnt] = ped->snr_U[QPcnt] / ped->cnt[QPcnt];
			ped->snr_V[QPcnt] = ped->snr_V[QPcnt] / ped->cnt[QPcnt];
			ped->snr_YUV[QPcnt] = ped->snr_YUV[QPcnt] / ped->cnt[QPcnt];
			//				printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt, ped->br[QPcnt], ped->snr_Y[QPcnt], ped->snr_U[QPcnt], ped->snr_V[QPcnt], ped->snr_YUV[QPcnt]);
			if (ped->cnt_i[QPcnt]) {
				ped->br_i[QPcnt] = ped->br_i[QPcnt] / ped->cnt_i[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_i[QPcnt] = ped->snr_Y_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_U_i[QPcnt] = ped->snr_U_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_V_i[QPcnt] = ped->snr_V_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_YUV_i[QPcnt] = ped->snr_YUV_i[QPcnt] / ped->cnt_i[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_i, ped->br_i[QPcnt], ped->snr_Y_i[QPcnt], ped->snr_U_i[QPcnt], ped->snr_V_i[QPcnt], ped->snr_YUV_i[QPcnt]);
			}
			if (ped->cnt_p[QPcnt]) {
				ped->br_p[QPcnt] = ped->br_p[QPcnt] / ped->cnt_p[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_p[QPcnt] = ped->snr_Y_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_U_p[QPcnt] = ped->snr_U_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_V_p[QPcnt] = ped->snr_V_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_YUV_p[QPcnt] = ped->snr_YUV_p[QPcnt] / ped->cnt_p[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_p, ped->br_p[QPcnt], ped->snr_Y_p[QPcnt], ped->snr_U_p[QPcnt], ped->snr_V_p[QPcnt], ped->snr_YUV_p[QPcnt]);
			}
			if (ped->cnt_b[QPcnt]) {
				ped->br_b[QPcnt] = ped->br_b[QPcnt] / ped->cnt_b[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_b[QPcnt] = ped->snr_Y_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_U_b[QPcnt] = ped->snr_U_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_V_b[QPcnt] = ped->snr_V_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_YUV_b[QPcnt] = ped->snr_YUV_b[QPcnt] / ped->cnt_b[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_b, ped->br_b[QPcnt], ped->snr_Y_b[QPcnt], ped->snr_U_b[QPcnt], ped->snr_V_b[QPcnt], ped->snr_YUV_b[QPcnt]);
			}
		}

	} // for QPcnt
	delete Oneline;
	delete Bitrate;
	delete Psnr_Y;
	delete Psnr_U;
	delete Psnr_V;
	delete Psnr_YUV;

	for (i = 0; i<NUM; i++) {
		delete IBP[i];
	}
	delete IBP;
	fclose(fp_ref1);
}
void Reading_CnM264(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP) {
	FILE *fp_ref1;
	char *Oneline = new char[1024];
	int i, QPcnt;
	double *Bitrate, *Psnr_Y, *Psnr_U, *Psnr_V, *Psnr_YUV;
	char **IBP;
	double bits, Y_PSNR, U_PSNR, V_PSNR;
	char PType[100];


	char TmpS1[80], TmpS2[80], TmpS3[80], TmpS4[80], TmpS5[80], TmpS6[80], TmpS7[80], TmpS8[80];

	IBP = new char*[NUM];
	for (i = 0; i<NUM; i++) {
		IBP[i] = new char[120];
	}

	Bitrate = new double[NUM];
	Psnr_Y = new double[NUM];
	Psnr_U = new double[NUM];
	Psnr_V = new double[NUM];
	Psnr_YUV = new double[NUM];
	memset(ped, 0, sizeof(AVS_CTX));

	fp_ref1 = fopen(LionFile, "r");
	if (!fp_ref1) {
		printf("Can't open file %s!\n", LionFile);
		exit(-1);
	}

	// Reading First QP
	for (QPcnt = 0; QPcnt<NoOfQP; QPcnt++) {
		while (!feof(fp_ref1)) {
			ReadOneLine_ep(fp_ref1, Oneline);
			if (strstr(Oneline, "Frame     Bit/pic  QP  Y-PSNR  U-PSNR  V-SPNR  RefFrames") != NULL)
				break;
		}
		ReadOneLine_ep(fp_ref1, Oneline);

		while (!feof(fp_ref1))
		{
			ReadOneLine_ep(fp_ref1, Oneline);
			if (Oneline[0] != '\0') {

				sscanf(Oneline, "%s %s %s %s %s %s %s %s ", TmpS1, TmpS2, TmpS3, TmpS4, TmpS5, TmpS6, TmpS7, TmpS8);
				if (*(TmpS1 + 5) == 'I')
				{
				
					PType[0] = *(TmpS1 + 5);
					bits = atof(TmpS2);
					Y_PSNR = atof(TmpS4);
					U_PSNR = atof(TmpS5);
					V_PSNR = atof(TmpS6);
				}
				else if (*TmpS2 == 'P' || *TmpS2 == 'B')
				{

					PType[0] = *(TmpS2);
					bits = atof(TmpS4);
					Y_PSNR = atof(TmpS6);
					U_PSNR = atof(TmpS7);
					V_PSNR = atof(TmpS8);
				}

				if (!(PType[0] == 'I' || PType[0] == 'P' || PType[0] == 'B'))
					break;

//				strcpy(IBP[ped->cnt[QPcnt]], PType);
				*IBP[ped->cnt[QPcnt]] = PType[0];
				Bitrate[ped->cnt[QPcnt]] = bits;
				Psnr_Y[ped->cnt[QPcnt]] = Y_PSNR;
				Psnr_U[ped->cnt[QPcnt]] = U_PSNR;
				Psnr_V[ped->cnt[QPcnt]] = V_PSNR;
				Psnr_YUV[ped->cnt[QPcnt]] = (Psnr_Y[ped->cnt[QPcnt]] * 4 + Psnr_U[ped->cnt[QPcnt]] + Psnr_V[ped->cnt[QPcnt]]) / 6;
				//			printf("%s %12.0f %7.3f %7.3f %7.3f %7.3f\n", IBP[ped->cnt], Bitrate[ped->cnt], Psnr_Y[ped->cnt], Psnr_U[ped->cnt], Psnr_V[ped->cnt], Psnr_YUV[ped->cnt]);
				ped->cnt[QPcnt]++;
			}
			else
				break;
		}

		if (((NoOfFrames != 0) && NoOfFrames <= ped->cnt[QPcnt])) {
			ped->cnt[QPcnt] = NoOfFrames;
		}
		else {
			printf("NoOfFrames error:- File=%s, QPcnt=%d: NoOfFrames = %d, ActualFrame = %d, exit!\n", LionFile, QPcnt, NoOfFrames, ped->cnt[QPcnt]);
			exit(-1);
		}
		for (i = 0; i<ped->cnt[QPcnt]; i++) {
			ped->br[QPcnt] += Bitrate[i];
			ped->snr_Y[QPcnt] += Psnr_Y[i];
			ped->snr_U[QPcnt] += Psnr_U[i];
			ped->snr_V[QPcnt] += Psnr_V[i];
			ped->snr_YUV[QPcnt] += Psnr_YUV[i];

			if (strstr(IBP[i], "I") != NULL) {
				ped->br_i[QPcnt] += Bitrate[i];
				ped->snr_Y_i[QPcnt] += Psnr_Y[i];
				ped->snr_U_i[QPcnt] += Psnr_U[i];
				ped->snr_V_i[QPcnt] += Psnr_V[i];
				ped->snr_YUV_i[QPcnt] += Psnr_YUV[i];
				ped->cnt_i[QPcnt]++;
			}
			else if (strstr(IBP[i], "P") != NULL) {
				ped->br_p[QPcnt] += Bitrate[i];
				ped->snr_Y_p[QPcnt] += Psnr_Y[i];
				ped->snr_U_p[QPcnt] += Psnr_U[i];
				ped->snr_V_p[QPcnt] += Psnr_V[i];
				ped->snr_YUV_p[QPcnt] += Psnr_YUV[i];
				ped->cnt_p[QPcnt]++;
			}
			else if (strstr(IBP[i], "B") != NULL) {
				ped->br_b[QPcnt] += Bitrate[i];
				ped->snr_Y_b[QPcnt] += Psnr_Y[i];
				ped->snr_U_b[QPcnt] += Psnr_U[i];
				ped->snr_V_b[QPcnt] += Psnr_V[i];
				ped->snr_YUV_b[QPcnt] += Psnr_YUV[i];
				ped->cnt_b[QPcnt]++;
			}
			else {
				printf("Unknown picture type!\n");
			}
		}

		if (FrameRate == 0)
			FrameRate = 30;
		if (ped->cnt[QPcnt]) {
			ped->br[QPcnt] = ped->br[QPcnt] / ped->cnt[QPcnt] * FrameRate / 1024 / 1024;
			ped->snr_Y[QPcnt] = ped->snr_Y[QPcnt] / ped->cnt[QPcnt];
			ped->snr_U[QPcnt] = ped->snr_U[QPcnt] / ped->cnt[QPcnt];
			ped->snr_V[QPcnt] = ped->snr_V[QPcnt] / ped->cnt[QPcnt];
			ped->snr_YUV[QPcnt] = ped->snr_YUV[QPcnt] / ped->cnt[QPcnt];
			//				printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt, ped->br[QPcnt], ped->snr_Y[QPcnt], ped->snr_U[QPcnt], ped->snr_V[QPcnt], ped->snr_YUV[QPcnt]);
			if (ped->cnt_i[QPcnt]) {
				ped->br_i[QPcnt] = ped->br_i[QPcnt] / ped->cnt_i[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_i[QPcnt] = ped->snr_Y_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_U_i[QPcnt] = ped->snr_U_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_V_i[QPcnt] = ped->snr_V_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_YUV_i[QPcnt] = ped->snr_YUV_i[QPcnt] / ped->cnt_i[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_i, ped->br_i[QPcnt], ped->snr_Y_i[QPcnt], ped->snr_U_i[QPcnt], ped->snr_V_i[QPcnt], ped->snr_YUV_i[QPcnt]);
			}
			if (ped->cnt_p[QPcnt]) {
				ped->br_p[QPcnt] = ped->br_p[QPcnt] / ped->cnt_p[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_p[QPcnt] = ped->snr_Y_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_U_p[QPcnt] = ped->snr_U_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_V_p[QPcnt] = ped->snr_V_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_YUV_p[QPcnt] = ped->snr_YUV_p[QPcnt] / ped->cnt_p[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_p, ped->br_p[QPcnt], ped->snr_Y_p[QPcnt], ped->snr_U_p[QPcnt], ped->snr_V_p[QPcnt], ped->snr_YUV_p[QPcnt]);
			}
			if (ped->cnt_b[QPcnt]) {
				ped->br_b[QPcnt] = ped->br_b[QPcnt] / ped->cnt_b[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_b[QPcnt] = ped->snr_Y_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_U_b[QPcnt] = ped->snr_U_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_V_b[QPcnt] = ped->snr_V_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_YUV_b[QPcnt] = ped->snr_YUV_b[QPcnt] / ped->cnt_b[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_b, ped->br_b[QPcnt], ped->snr_Y_b[QPcnt], ped->snr_U_b[QPcnt], ped->snr_V_b[QPcnt], ped->snr_YUV_b[QPcnt]);
			}
		}

	} // for QPcnt
	delete Oneline;
	delete Bitrate;
	delete Psnr_Y;
	delete Psnr_U;
	delete Psnr_V;
	delete Psnr_YUV;

	for (i = 0; i<NUM; i++) {
		delete IBP[i];
	}
	delete IBP;
	fclose(fp_ref1);
}
void Reading_CnM(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP) {
	FILE *fp_ref1;
	char *Oneline = new char[1024];
	int i, QPcnt;
	double *Bitrate, *Psnr_Y, *Psnr_U, *Psnr_V, *Psnr_YUV;
	char **IBP;
	float bits, Y_PSNR, U_PSNR, V_PSNR;
	char yuvpsnr[80];
	char *tempY;
	char PType[100];


	char TmpS1[80], TmpS2[80], TmpS3[80], TmpS4[80], TmpS5[80], TmpS6[80];
	float tmp1, tmp2, tmp7;

	IBP = new char*[NUM];
	for (i = 0; i<NUM; i++) {
		IBP[i] = new char[120];
	}

	Bitrate = new double[NUM];
	Psnr_Y = new double[NUM];
	Psnr_U = new double[NUM];
	Psnr_V = new double[NUM];
	Psnr_YUV = new double[NUM];
	memset(ped, 0, sizeof(AVS_CTX));

	fp_ref1 = fopen(LionFile, "r");
	if (!fp_ref1) {
		printf("Can't open file %s!\n", LionFile);
		exit(-1);
	}

	// Reading First QP
	for (QPcnt = 0; QPcnt<NoOfQP; QPcnt++) {
		while (!feof(fp_ref1)) {
			ReadOneLine_ep(fp_ref1, Oneline);
			if (strstr(Oneline, "EncPicNum SrcIdx EncIdx (   POC)             PicType  QP       bits") != NULL)
				break;
		}
		ReadOneLine_ep(fp_ref1, Oneline);
		ReadOneLine_ep(fp_ref1, Oneline);
		ReadOneLine_ep(fp_ref1, Oneline);

		while (!feof(fp_ref1))
		{
			ReadOneLine_ep(fp_ref1, Oneline);
			if (Oneline[0] != '\0') {
				//				sscanf(Oneline, "%s %s %s %s %s %s %s %s %s %s ", TmpS, PType, TmpS, TmpS, Bits, YPSNR, UPSNR, VPSNR, TmpS, TmpS);


				sscanf(Oneline, "%s %s %f %f %s %s %s %s %s %f %f %s %f %f",
					//sscanf(Oneline, "%s %s %f %f %s %s %s %s %s %f %f %f %f %f",
					TmpS1, TmpS2, &tmp1, &tmp2, TmpS3, TmpS4, PType, TmpS5, TmpS6, &tmp7, &bits, yuvpsnr, &U_PSNR, &V_PSNR);
				//TmpS1, TmpS2, &tmp1, &tmp2, TmpS3, TmpS4, PType, TmpS5, TmpS6, &tmp7, &bits, &Y_PSNR, &U_PSNR, &V_PSNR);  //modified for 520, discard yuvpsnr
				//				                 TmpS, TmpS,  &tmp,  &tmp,  TmpS,  TmpS, PType, TmpS,  TmpS, &tmp, &bits, yuvpsnr, &Y_PSNR, &U_PSNR, &V_PSNR);

				//				sscanf(Oneline, "%s %d %f %d %f %f %f", TmpS, &tmp, &x1, &tmp, &x2, &x3, &x4);
				tempY = strtok(yuvpsnr, "["); 
				Y_PSNR = (float)atof(tempY);

				if (!(PType[0] == 'I' || PType[0] == 'P' || PType[0] == 'B'))
					break;
				strcpy(IBP[ped->cnt[QPcnt]], PType);
				Bitrate[ped->cnt[QPcnt]] = bits;
				Psnr_Y[ped->cnt[QPcnt]] = Y_PSNR;
				Psnr_U[ped->cnt[QPcnt]] = U_PSNR;
				Psnr_V[ped->cnt[QPcnt]] = V_PSNR;
				Psnr_YUV[ped->cnt[QPcnt]] = (Psnr_Y[ped->cnt[QPcnt]] * 4 + Psnr_U[ped->cnt[QPcnt]] + Psnr_V[ped->cnt[QPcnt]]) / 6;
				//			printf("%s %12.0f %7.3f %7.3f %7.3f %7.3f\n", IBP[ped->cnt], Bitrate[ped->cnt], Psnr_Y[ped->cnt], Psnr_U[ped->cnt], Psnr_V[ped->cnt], Psnr_YUV[ped->cnt]);
				ped->cnt[QPcnt]++;
			}
			else
				break;
		}

		if (((NoOfFrames != 0) && NoOfFrames <= ped->cnt[QPcnt])) {
			ped->cnt[QPcnt] = NoOfFrames;
		}
		else {
			printf("NoOfFrames error:- File=%s, QPcnt=%d: NoOfFrames = %d, ActualFrame = %d, exit!\n", LionFile, QPcnt, NoOfFrames, ped->cnt[QPcnt]);
			exit(-1);
		}
		for (i = 0; i<ped->cnt[QPcnt]; i++) {
			ped->br[QPcnt] += Bitrate[i];
			ped->snr_Y[QPcnt] += Psnr_Y[i];
			ped->snr_U[QPcnt] += Psnr_U[i];
			ped->snr_V[QPcnt] += Psnr_V[i];
			ped->snr_YUV[QPcnt] += Psnr_YUV[i];

			if (strstr(IBP[i], "I") != NULL) {
				ped->br_i[QPcnt] += Bitrate[i];
				ped->snr_Y_i[QPcnt] += Psnr_Y[i];
				ped->snr_U_i[QPcnt] += Psnr_U[i];
				ped->snr_V_i[QPcnt] += Psnr_V[i];
				ped->snr_YUV_i[QPcnt] += Psnr_YUV[i];
				ped->cnt_i[QPcnt]++;
			}
			else if (strstr(IBP[i], "P") != NULL) {
				ped->br_p[QPcnt] += Bitrate[i];
				ped->snr_Y_p[QPcnt] += Psnr_Y[i];
				ped->snr_U_p[QPcnt] += Psnr_U[i];
				ped->snr_V_p[QPcnt] += Psnr_V[i];
				ped->snr_YUV_p[QPcnt] += Psnr_YUV[i];
				ped->cnt_p[QPcnt]++;
			}
			else if (strstr(IBP[i], "B") != NULL) {
				ped->br_b[QPcnt] += Bitrate[i];
				ped->snr_Y_b[QPcnt] += Psnr_Y[i];
				ped->snr_U_b[QPcnt] += Psnr_U[i];
				ped->snr_V_b[QPcnt] += Psnr_V[i];
				ped->snr_YUV_b[QPcnt] += Psnr_YUV[i];
				ped->cnt_b[QPcnt]++;
			}
			else {
				printf("Unknown picture type!\n");
			}
		}

		if (FrameRate == 0)
			FrameRate = 30;
		if (ped->cnt[QPcnt]) {
			ped->br[QPcnt] = ped->br[QPcnt] / ped->cnt[QPcnt] * FrameRate / 1024 / 1024;
			ped->snr_Y[QPcnt] = ped->snr_Y[QPcnt] / ped->cnt[QPcnt];
			ped->snr_U[QPcnt] = ped->snr_U[QPcnt] / ped->cnt[QPcnt];
			ped->snr_V[QPcnt] = ped->snr_V[QPcnt] / ped->cnt[QPcnt];
			ped->snr_YUV[QPcnt] = ped->snr_YUV[QPcnt] / ped->cnt[QPcnt];
			//				printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt, ped->br[QPcnt], ped->snr_Y[QPcnt], ped->snr_U[QPcnt], ped->snr_V[QPcnt], ped->snr_YUV[QPcnt]);
			if (ped->cnt_i[QPcnt]) {
				ped->br_i[QPcnt] = ped->br_i[QPcnt] / ped->cnt_i[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_i[QPcnt] = ped->snr_Y_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_U_i[QPcnt] = ped->snr_U_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_V_i[QPcnt] = ped->snr_V_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_YUV_i[QPcnt] = ped->snr_YUV_i[QPcnt] / ped->cnt_i[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_i, ped->br_i[QPcnt], ped->snr_Y_i[QPcnt], ped->snr_U_i[QPcnt], ped->snr_V_i[QPcnt], ped->snr_YUV_i[QPcnt]);
			}
			if (ped->cnt_p[QPcnt]) {
				ped->br_p[QPcnt] = ped->br_p[QPcnt] / ped->cnt_p[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_p[QPcnt] = ped->snr_Y_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_U_p[QPcnt] = ped->snr_U_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_V_p[QPcnt] = ped->snr_V_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_YUV_p[QPcnt] = ped->snr_YUV_p[QPcnt] / ped->cnt_p[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_p, ped->br_p[QPcnt], ped->snr_Y_p[QPcnt], ped->snr_U_p[QPcnt], ped->snr_V_p[QPcnt], ped->snr_YUV_p[QPcnt]);
			}
			if (ped->cnt_b[QPcnt]) {
				ped->br_b[QPcnt] = ped->br_b[QPcnt] / ped->cnt_b[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_b[QPcnt] = ped->snr_Y_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_U_b[QPcnt] = ped->snr_U_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_V_b[QPcnt] = ped->snr_V_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_YUV_b[QPcnt] = ped->snr_YUV_b[QPcnt] / ped->cnt_b[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_b, ped->br_b[QPcnt], ped->snr_Y_b[QPcnt], ped->snr_U_b[QPcnt], ped->snr_V_b[QPcnt], ped->snr_YUV_b[QPcnt]);
			}
		}

	} // for QPcnt
	delete Oneline;
	delete Bitrate;
	delete Psnr_Y;
	delete Psnr_U;
	delete Psnr_V;
	delete Psnr_YUV;

	for (i = 0; i<NUM; i++) {
		delete IBP[i];
	}
	delete IBP;
	fclose(fp_ref1);
}
void Reading_AVS(char AVSFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP) {
	FILE *fp_ref1;
	char *Oneline = new char[180];
	int i, QPcnt, tmp;
	char TmpS[128];
	float x1, x2, x3, x4;

	double *Bitrate, *Psnr_Y, *Psnr_U, *Psnr_V, *Psnr_YUV;
	char **IBP;

	IBP = new char*[NUM];
	for (i = 0; i<NUM; i++) {
		IBP[i] = new char[120];
	}

	Bitrate = new double[NUM];
	Psnr_Y = new double[NUM];
	Psnr_U = new double[NUM];
	Psnr_V = new double[NUM];
	Psnr_YUV = new double[NUM];

	memset(ped, 0, sizeof(AVS_CTX));

	fp_ref1 = fopen(AVSFile, "r");
	if (!fp_ref1) {
		printf("Can't open file %s!\n", AVSFile);
		exit(-1);
	}

	// Reading First QP
	for (QPcnt = 0; QPcnt<NoOfQP; QPcnt++) {
		while (!feof(fp_ref1)) {
			ReadOneLine(fp_ref1, Oneline);
			if (strstr(Oneline, "-------------------- DEBUG_INFO_START") != NULL)
				//			if (strstr(Oneline,"Sequence Header") != NULL) 
				break;
		}

		while (!feof(fp_ref1))
		{
			ReadOneLine(fp_ref1, Oneline);
			if (Oneline[0] != '\0') {
				sscanf(Oneline, "%d %s %f %d %f %f %f", &tmp, TmpS, &x1, &tmp, &x2, &x3, &x4);
				strcpy(IBP[ped->cnt[QPcnt]], TmpS);
				Bitrate[ped->cnt[QPcnt]] = x1;
				Psnr_Y[ped->cnt[QPcnt]] = x2;
				Psnr_U[ped->cnt[QPcnt]] = x3;
				Psnr_V[ped->cnt[QPcnt]] = x4;
				Psnr_YUV[ped->cnt[QPcnt]] = (Psnr_Y[ped->cnt[QPcnt]] * 4 + Psnr_U[ped->cnt[QPcnt]] + Psnr_V[ped->cnt[QPcnt]]) / 6;
				//			printf("%s %12.0f %7.3f %7.3f %7.3f %7.3f\n", IBP[ped->cnt], Bitrate[ped->cnt], Psnr_Y[ped->cnt], Psnr_U[ped->cnt], Psnr_V[ped->cnt], Psnr_YUV[ped->cnt]);
				ped->cnt[QPcnt]++;
			}
			else
				break;
		}

		if ((NoOfFrames != 0) && NoOfFrames <= ped->cnt[QPcnt]) {
			ped->cnt[QPcnt] = NoOfFrames;
		}
		else {
			printf("NoOfFrames error:- File=%s, QPcnt=%d: NoOfFrames = %d, ActualFrame = %d, exit!\n", AVSFile, QPcnt, NoOfFrames, ped->cnt[QPcnt]);
			exit(-1);
		}
		for (i = 0; i<ped->cnt[QPcnt]; i++) {
			ped->br[QPcnt] += Bitrate[i];
			ped->snr_Y[QPcnt] += Psnr_Y[i];
			ped->snr_U[QPcnt] += Psnr_U[i];
			ped->snr_V[QPcnt] += Psnr_V[i];
			ped->snr_YUV[QPcnt] += Psnr_YUV[i];


			if (strstr(IBP[i], "I") != NULL) {
				ped->br_i[QPcnt] += Bitrate[i];
				ped->snr_Y_i[QPcnt] += Psnr_Y[i];
				ped->snr_U_i[QPcnt] += Psnr_U[i];
				ped->snr_V_i[QPcnt] += Psnr_V[i];
				ped->snr_YUV_i[QPcnt] += Psnr_YUV[i];
				ped->cnt_i[QPcnt]++;
			}
			else if (strstr(IBP[i], "G") != NULL) {
				ped->br_g[QPcnt] += Bitrate[i];
				ped->snr_Y_g[QPcnt] += Psnr_Y[i];
				ped->snr_U_g[QPcnt] += Psnr_U[i];
				ped->snr_V_g[QPcnt] += Psnr_V[i];
				ped->snr_YUV_g[QPcnt] += Psnr_YUV[i];
				ped->cnt_g[QPcnt]++;
			}
			else if (strstr(IBP[i], "S") != NULL) {
				ped->br_s[QPcnt] += Bitrate[i];
				ped->snr_Y_s[QPcnt] += Psnr_Y[i];
				ped->snr_U_s[QPcnt] += Psnr_U[i];
				ped->snr_V_s[QPcnt] += Psnr_V[i];
				ped->snr_YUV_s[QPcnt] += Psnr_YUV[i];
				ped->cnt_s[QPcnt]++;
			}
			else if (strstr(IBP[i], "P") != NULL) {
				ped->br_p[QPcnt] += Bitrate[i];
				ped->snr_Y_p[QPcnt] += Psnr_Y[i];
				ped->snr_U_p[QPcnt] += Psnr_U[i];
				ped->snr_V_p[QPcnt] += Psnr_V[i];
				ped->snr_YUV_p[QPcnt] += Psnr_YUV[i];
				ped->cnt_p[QPcnt]++;
			}
			else if (strstr(IBP[i], "F") != NULL) {
				ped->br_f[QPcnt] += Bitrate[i];
				ped->snr_Y_f[QPcnt] += Psnr_Y[i];
				ped->snr_U_f[QPcnt] += Psnr_U[i];
				ped->snr_V_f[QPcnt] += Psnr_V[i];
				ped->snr_YUV_f[QPcnt] += Psnr_YUV[i];
				ped->cnt_f[QPcnt]++;
			}
			else if (strstr(IBP[i], "B") != NULL) {
				ped->br_b[QPcnt] += Bitrate[i];
				ped->snr_Y_b[QPcnt] += Psnr_Y[i];
				ped->snr_U_b[QPcnt] += Psnr_U[i];
				ped->snr_V_b[QPcnt] += Psnr_V[i];
				ped->snr_YUV_b[QPcnt] += Psnr_YUV[i];
				ped->cnt_b[QPcnt]++;
			}
			else {
				printf("Unknown picture type!\n");
			}
		}

		if (FrameRate == 0)
			FrameRate = 30;
		if (ped->cnt[QPcnt]) {
			ped->br[QPcnt] = ped->br[QPcnt] / ped->cnt[QPcnt] * FrameRate / 1024 / 1024;
			ped->snr_Y[QPcnt] = ped->snr_Y[QPcnt] / ped->cnt[QPcnt];
			ped->snr_U[QPcnt] = ped->snr_U[QPcnt] / ped->cnt[QPcnt];
			ped->snr_V[QPcnt] = ped->snr_V[QPcnt] / ped->cnt[QPcnt];
			ped->snr_YUV[QPcnt] = ped->snr_YUV[QPcnt] / ped->cnt[QPcnt];
			//				printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt, ped->br[QPcnt], ped->snr_Y[QPcnt], ped->snr_U[QPcnt], ped->snr_V[QPcnt], ped->snr_YUV[QPcnt]);

			if (ped->cnt_i[QPcnt]) {
				ped->br_i[QPcnt] = ped->br_i[QPcnt] / ped->cnt_i[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_i[QPcnt] = ped->snr_Y_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_U_i[QPcnt] = ped->snr_U_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_V_i[QPcnt] = ped->snr_V_i[QPcnt] / ped->cnt_i[QPcnt];
				ped->snr_YUV_i[QPcnt] = ped->snr_YUV_i[QPcnt] / ped->cnt_i[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_i, ped->br_i[QPcnt], ped->snr_Y_i[QPcnt], ped->snr_U_i[QPcnt], ped->snr_V_i[QPcnt], ped->snr_YUV_i[QPcnt]);
			}
			if (ped->cnt_g[QPcnt]) {
				ped->br_g[QPcnt] = ped->br_g[QPcnt] / ped->cnt_g[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_g[QPcnt] = ped->snr_Y_g[QPcnt] / ped->cnt_g[QPcnt];
				ped->snr_U_g[QPcnt] = ped->snr_U_g[QPcnt] / ped->cnt_g[QPcnt];
				ped->snr_V_g[QPcnt] = ped->snr_V_g[QPcnt] / ped->cnt_g[QPcnt];
				ped->snr_YUV_g[QPcnt] = ped->snr_YUV_g[QPcnt] / ped->cnt_g[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_g, ped->br_g[QPcnt], ped->snr_Y_g[QPcnt], ped->snr_U_g[QPcnt], ped->snr_V_g[QPcnt], ped->snr_YUV_g[QPcnt]);
			}
			if (ped->cnt_s[QPcnt]) {
				ped->br_s[QPcnt] = ped->br_s[QPcnt] / ped->cnt_s[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_s[QPcnt] = ped->snr_Y_s[QPcnt] / ped->cnt_s[QPcnt];
				ped->snr_U_s[QPcnt] = ped->snr_U_s[QPcnt] / ped->cnt_s[QPcnt];
				ped->snr_V_s[QPcnt] = ped->snr_V_s[QPcnt] / ped->cnt_s[QPcnt];
				ped->snr_YUV_s[QPcnt] = ped->snr_YUV_s[QPcnt] / ped->cnt_s[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_s, ped->br_s[QPcnt], ped->snr_Y_s[QPcnt], ped->snr_U_s[QPcnt], ped->snr_V_s[QPcnt], ped->snr_YUV_s[QPcnt]);
			}
			if (ped->cnt_p[QPcnt]) {
				ped->br_p[QPcnt] = ped->br_p[QPcnt] / ped->cnt_p[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_p[QPcnt] = ped->snr_Y_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_U_p[QPcnt] = ped->snr_U_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_V_p[QPcnt] = ped->snr_V_p[QPcnt] / ped->cnt_p[QPcnt];
				ped->snr_YUV_p[QPcnt] = ped->snr_YUV_p[QPcnt] / ped->cnt_p[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_p, ped->br_p[QPcnt], ped->snr_Y_p[QPcnt], ped->snr_U_p[QPcnt], ped->snr_V_p[QPcnt], ped->snr_YUV_p[QPcnt]);
			}
			if (ped->cnt_f[QPcnt]) {
				ped->br_f[QPcnt] = ped->br_f[QPcnt] / ped->cnt_f[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_f[QPcnt] = ped->snr_Y_f[QPcnt] / ped->cnt_f[QPcnt];
				ped->snr_U_f[QPcnt] = ped->snr_U_f[QPcnt] / ped->cnt_f[QPcnt];
				ped->snr_V_f[QPcnt] = ped->snr_V_f[QPcnt] / ped->cnt_f[QPcnt];
				ped->snr_YUV_f[QPcnt] = ped->snr_YUV_f[QPcnt] / ped->cnt_f[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_f, ped->br_f[QPcnt], ped->snr_Y_f[QPcnt], ped->snr_U_f[QPcnt], ped->snr_V_f[QPcnt], ped->snr_YUV_f[QPcnt]);
			}
			if (ped->cnt_b[QPcnt]) {
				ped->br_b[QPcnt] = ped->br_b[QPcnt] / ped->cnt_b[QPcnt] * FrameRate / 1024 / 1024;
				ped->snr_Y_b[QPcnt] = ped->snr_Y_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_U_b[QPcnt] = ped->snr_U_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_V_b[QPcnt] = ped->snr_V_b[QPcnt] / ped->cnt_b[QPcnt];
				ped->snr_YUV_b[QPcnt] = ped->snr_YUV_b[QPcnt] / ped->cnt_b[QPcnt];
				//					printf("%4d %12.2f %7.3f %7.3f %7.3f %7.3f\n",ped->cnt_b, ped->br_b[QPcnt], ped->snr_Y_b[QPcnt], ped->snr_U_b[QPcnt], ped->snr_V_b[QPcnt], ped->snr_YUV_b[QPcnt]);
			}
		}

	} // for QPcnt
	delete Oneline;
	delete Bitrate;
	delete Psnr_Y;
	delete Psnr_U;
	delete Psnr_V;
	delete Psnr_YUV;

	for (i = 0; i<NUM; i++) {
		delete IBP[i];
	}
	delete IBP;
	fclose(fp_ref1);
}


int equation(double a[4][5], int Nvar) {
	double p, r, x[5];
	int np1, i, j, k, ip1, kp1;
	np1 = Nvar + 1;
	for (k = 0; k<Nvar; k++) {
		p = a[k][k];
		if (fabs(p)<0.00001) {
			printf("divided by zero.\n");
			return(0);
		}
		else {
			for (j = k; j<np1; j++)
				a[k][j] = a[k][j] / p;
			kp1 = k + 1;
			for (i = kp1; i<Nvar; i++) {
				r = a[i][k];
				for (j = k; j<np1; j++)
					a[i][j] = a[i][j] - r*a[k][j];
			}
		}
	}
	x[Nvar - 1] = a[Nvar - 1][Nvar] / a[Nvar - 1][Nvar - 1];
	i = Nvar - 2;
	do {
		ip1 = i + 1;
		x[i] = a[i][Nvar];
		for (k = ip1; k<Nvar; k++)
			x[i] = x[i] - a[i][k] * x[k];
		i = i - 1;
	} while (i >= 0);

	for (i = 0; i<Nvar; i++)
		a[i][Nvar] = x[i];
	return (1);
}

void polyfitting(double *x, double *y, int size, double coeff[4]) {
	//					y = f(x)
	int i, j, eq;
	double a[4][5];
	double mean = 0, rms = 0;

	for (i = 0; i< 4; i++)
	for (j = 0; j<5; j++)
		a[i][j] = 0;

	a[0][0] = size;
	for (i = 0; i<size; i++) {
		a[0][1] += x[i];
		a[0][2] += x[i] * x[i];
		a[0][3] += x[i] * x[i] * x[i];

		a[1][3] += x[i] * x[i] * x[i] * x[i];
		a[2][3] += x[i] * x[i] * x[i] * x[i] * x[i];
		a[3][3] += x[i] * x[i] * x[i] * x[i] * x[i] * x[i];
		a[0][4] += y[i];
		a[1][4] += y[i] * x[i];
		a[2][4] += y[i] * x[i] * x[i];
		a[3][4] += y[i] * x[i] * x[i] * x[i];
	}
	a[1][0] = a[0][1];
	a[1][1] = a[0][2];
	a[1][2] = a[0][3];

	a[2][0] = a[0][2];
	a[2][1] = a[1][2];
	a[2][2] = a[1][3];

	a[3][0] = a[0][3];
	a[3][1] = a[1][3];
	a[3][2] = a[2][3];

	eq = equation(a, 4);

	for (i = 0; i<4; i++)
		coeff[i] = a[i][4];
#if 0
	mean = 0;
	rms = 0;
	for (i = 0; i<size; i++) {
		error[i] = coeff[0] + coeff[1] * x[i] + coeff[2] * x[i] * x[i] + coeff[3] * x[i] * x[i] * x[i] - y[i];
		mean += error[i];
	}
	mean = mean / 4.0;
	for (i = 0; i<size; i++) {
		rms += (error[i] - mean)*(error[i] - mean);
	}
	rms = sqrt(rms / 4.0);
	printf("\nFitting error (rms) = %f\n", rms);
#endif
}

double minimum1D(double *array, int m) {
	int i;
	double min;
	min = array[0];
	for (i = 1; i<m; i++)
	if (min > array[i]) {
		min = array[i];
	}
	return min;
}

double maximum1D(double *array, int m) {
	int i;
	double max;
	max = array[0];
	for (i = 1; i<m; i++)
	if (max < array[i]) {
		max = array[i];
	}
	return max;
}

extern FILE *fp_out;
//void RDPlot(double Br1_[NUM_QP], double Snr1[NUM_QP], double Br2_[NUM_QP], double Snr2[NUM_QP], BD_CTX *pBD, int prt, int valid) {

void RDPlot(double *Br1_, double *Snr1, double *Br2_, double *Snr2, BD_CTX *pBD, int size, int prt, int valid) {

	int i; // , size;
	double min_int, max_int;
	double coeff1[4], coeff2[4];
	double Br1[NUM_QP], Br2[NUM_QP];


	if (prt) {
		for (i = 0; i<size; i++) {
			printf("%12.3f, %7.3f, %12.3f, %7.3f\n", Br1_[i], Snr1[i], Br2_[i], Snr2[i]);
			fprintf(fp_out, "%12.3f, %7.3f, %12.3f, %7.3f\n", Br1_[i], Snr1[i], Br2_[i], Snr2[i]);
		}
	}

	if (valid == 1) {
		for (i = 0; i < size; i++)
		{
			Br1[i] = log(Br1_[i]);
			Br2[i] = log(Br2_[i]);
		}


		// Average Bitrate Difference: Br = f(Snr)
		double minS1, minS2, maxS1, maxS2;
		polyfitting(Snr1, Br1, size, coeff1);
		polyfitting(Snr2, Br2, size, coeff2);

		// integration interval
		minS1 = minimum1D(Snr1, size);
		minS2 = minimum1D(Snr2, size);
		maxS1 = maximum1D(Snr1, size);
		maxS2 = maximum1D(Snr2, size);

		min_int = minS1 > minS2 ? minS1 : minS2;
		max_int = maxS1 < maxS2 ? maxS1 : maxS2;

		//	find integral

		double RB1, RE1, RB2, RE2, int1, int2;

		RB1 = coeff1[0] * min_int + 0.5*coeff1[1] * min_int*min_int + (1.0 / 3)*coeff1[2] * min_int*min_int*min_int + 0.25*coeff1[3] * min_int*min_int*min_int*min_int;
		RE1 = coeff1[0] * max_int + 0.5*coeff1[1] * max_int*max_int + (1.0 / 3)*coeff1[2] * max_int*max_int*max_int + 0.25*coeff1[3] * max_int*max_int*max_int*max_int;

		RB2 = coeff2[0] * min_int + 0.5*coeff2[1] * min_int*min_int + (1.0 / 3)*coeff2[2] * min_int*min_int*min_int + 0.25*coeff2[3] * min_int*min_int*min_int*min_int;
		RE2 = coeff2[0] * max_int + 0.5*coeff2[1] * max_int*max_int + (1.0 / 3)*coeff2[2] * max_int*max_int*max_int + 0.25*coeff2[3] * max_int*max_int*max_int*max_int;

		int1 = (RE1 - RB1);
		int2 = (RE2 - RB2);

		pBD->Br_aver = (int2 - int1) / (max_int - min_int);

		pBD->Br_aver = (exp(pBD->Br_aver) - 1) * 100;
		/*
		for (i=0; i<size; i++) {
		printf("%f ", coeff1[i]);
		}
		*/
		//	printf("\nBr_aver=%f\n",pBD->Br_aver);


		// Average PSNR Difference: Snr = f(Br)
		double minR1, minR2, maxR1, maxR2;
		polyfitting(Br1, Snr1, size, coeff1);
		polyfitting(Br2, Snr2, size, coeff2);

		// integration interval
		minR1 = minimum1D(Br1, size);
		minR2 = minimum1D(Br2, size);
		maxR1 = maximum1D(Br1, size);
		maxR2 = maximum1D(Br2, size);

		min_int = minR1 > minR2 ? minR1 : minR2;
		max_int = maxR1 < maxR2 ? maxR1 : maxR2;

		//	find integral

		double SB1, SE1, SB2, SE2;

		SB1 = coeff1[0] * min_int + 0.5*coeff1[1] * min_int*min_int + (1.0 / 3)*coeff1[2] * min_int*min_int*min_int + 0.25*coeff1[3] * min_int*min_int*min_int*min_int;
		SE1 = coeff1[0] * max_int + 0.5*coeff1[1] * max_int*max_int + (1.0 / 3)*coeff1[2] * max_int*max_int*max_int + 0.25*coeff1[3] * max_int*max_int*max_int*max_int;

		SB2 = coeff2[0] * min_int + 0.5*coeff2[1] * min_int*min_int + (1.0 / 3)*coeff2[2] * min_int*min_int*min_int + 0.25*coeff2[3] * min_int*min_int*min_int*min_int;
		SE2 = coeff2[0] * max_int + 0.5*coeff2[1] * max_int*max_int + (1.0 / 3)*coeff2[2] * max_int*max_int*max_int + 0.25*coeff2[3] * max_int*max_int*max_int*max_int;

		int1 = (SE1 - SB1);
		int2 = (SE2 - SB2);

		pBD->PSNR_aver = (int2 - int1) / (max_int - min_int);
	}
	else {
		pBD->Br_aver = 0;
		pBD->PSNR_aver = 0;
	}

	/*
	for (i=0; i<size; i++) {
	printf("%f ", coeff1[i]);
	}
	*/
	//	printf("\nPSNR_aver=%f\n",pBD->PSNR_aver);
	if (prt) {
		printf("  Average_Bitrate_Increment:,,, %10.6f %%\n", pBD->Br_aver);
		fprintf(fp_out, "  Average_Bitrate_Increment:,,, %10.6f %%\n", pBD->Br_aver);
		printf("  Average___PSNR__Increment:,,, %10.6f dB\n", pBD->PSNR_aver);
		fprintf(fp_out, "  Average___PSNR__Increment:,,, %10.6f dB\n", pBD->PSNR_aver);
	}
}

EncodeFormat_e Str2Enum(char* Str_e) {
	int i;
	EncodeFormat_e enc_f;
	char enumStr[][80] = { "AVS", "Lion_AVS", "HM", "Lion_265", "x264", "x265", "CnM264", "CnM" };

	enc_f = Unknown_enc;
	for (i = 0; i < Unknown_enc; i++) {
		if (!strcmp(Str_e, enumStr[i])) {
			enc_f = (EncodeFormat_e)i;
			break;
		}
	}
	return enc_f;
}