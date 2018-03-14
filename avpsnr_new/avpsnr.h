#ifndef __AVPSNR_H
#define __AVPSNR_H

#define NUM 1000
#define NUM_QP 8

typedef enum EncodeFormat
{
	AVS = 0,
	Lion_AVS,
	HM,
	Lion_265,
	x264,
	x265,
	CnM264, 
	CnM,
	Unknown_enc
} EncodeFormat_e;

typedef struct avs_ctx {
	int cnt[NUM_QP];
	double br[NUM_QP], snr_Y[NUM_QP], snr_U[NUM_QP], snr_V[NUM_QP], snr_YUV[NUM_QP];
	int cnt_i[NUM_QP];
	double br_i[NUM_QP], snr_Y_i[NUM_QP], snr_U_i[NUM_QP], snr_V_i[NUM_QP], snr_YUV_i[NUM_QP];
	int cnt_g[NUM_QP];
	double br_g[NUM_QP], snr_Y_g[NUM_QP], snr_U_g[NUM_QP], snr_V_g[NUM_QP], snr_YUV_g[NUM_QP];
	int cnt_s[NUM_QP];
	double br_s[NUM_QP], snr_Y_s[NUM_QP], snr_U_s[NUM_QP], snr_V_s[NUM_QP], snr_YUV_s[NUM_QP];
	int cnt_p[NUM_QP];
	double br_p[NUM_QP], snr_Y_p[NUM_QP], snr_U_p[NUM_QP], snr_V_p[NUM_QP], snr_YUV_p[NUM_QP];
	int cnt_f[NUM_QP];
	double br_f[NUM_QP], snr_Y_f[NUM_QP], snr_U_f[NUM_QP], snr_V_f[NUM_QP], snr_YUV_f[NUM_QP];
	int cnt_b[NUM_QP];
	double br_b[NUM_QP], snr_Y_b[NUM_QP], snr_U_b[NUM_QP], snr_V_b[NUM_QP], snr_YUV_b[NUM_QP];
} AVS_CTX, *pAVS_CTX;


typedef struct bdplot {
	double Br_aver;
	double PSNR_aver;
} BD_CTX;

void ReadOneLine(FILE *fp, char *Oneline);
void Reading_Lion_AVS(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP);
void Reading_Lion_265(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP);
void Reading_AVS(char AVSFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP);
void Reading_HM(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP);
void Reading_x264(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP);
void Reading_x265(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP);
void Reading_CnM264(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP);
void Reading_CnM(char LionFile[120], AVS_CTX *ped, int NoOfFrames, int FrameRate, int NoOfQP);

void polyfitting(double *ob, double *sb, int size, double b[4]);

int equation(double a[4][5], int Nvar);
double maximum1D(double *array, int m);
double minimum1D(double *array, int m);

void RDPlot(double *Br1_, double *Snr1, double *Br2_, double *Snr2, BD_CTX *pBD, int size, int prt, int valid);
EncodeFormat_e Str2Enum(char*);
#endif
