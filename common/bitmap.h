
typedef struct {
	BYTE	bfType[2];					// WORD
	BYTE	bfSize[4];					// DWORD
	BYTE	bfReserved1[2];				// WORD
	BYTE	bfReserved2[2];				// WORD
	BYTE	bfOffBits[4];				// DWORD
	BYTE	biSize[4];					// DWORD
	BYTE	biWidth[4];					// long
	BYTE	biHeight[4];				// long
	BYTE	biPlanes[2];				// WORD
	BYTE	biBitCount[2];				// WORD
	BYTE	biCompression[4];			// DWORD
	BYTE	biSizeImage[4];				// DWORD
	BYTE	biXPelsPerMeter[4];			// long
	BYTE	biYPelsPerMeter[4];			// long
	BYTE	biClrUsed[4];				// DWORD
	BYTE	biClrImportant[4];			// DWORD
} BMP_T;

#define	BMPINFO_SIZE		40
