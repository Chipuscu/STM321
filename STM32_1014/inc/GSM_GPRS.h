#ifndef  __GSM_GPRS_H__
#define __GSM_GPRS_H__
#include "main.h"


#define		GSM_RECONNECT		2		/* Che do ket noi lai vao server */

void CheckSignalQuality(void);
void MC60_Process_100ms(void);
void MC60_ChangeMode(uint8_t Mode);

#define		GSM_OK				0	   	/* Trang thai khong van de gi ca */
#define		GSM_RESET			1		/* Trang thai khoi dong module GSM */
#define		GSM_RECONNECT		2		/* Che do ket noi lai vao server */
#define		GSM_TIMEOUT			3		/* Trang thai GSM Timeout sau moi lan truyen, phai nghi x giay moi duoc truyen tiep tuc */
#define		GSM_WAITSENDRES		4 		/* Trang thai cho ket qua sau khi du lieu */ 
#define		GSM_SENDDATA		5		/* Trang thai truyen du lieu */
#define		GSM_DEACT			6		/* Deactive ket noi voi server */
#define		GSM_FTPUPLOAD		7		/* Thuc hien qua trinh xu ly upload FTP */
#define		GSM_FTPDOWNLOAD		8		/* Thuc hien qua trinh xu ly download FTP */
#define		GSM_HTTP_TRANSFER	9		/* Thuc hien qua trinh giao dich HTTP */
#endif
