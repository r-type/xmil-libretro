#include	"compiler.h"
#include	"dosio.h"
#include	"z80core.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"fddfile.h"
#include	"fdd_2d.h"
#include	"fdd_d88.h"
#include	"fdd_mtr.h"


		BYTE		FDC_c[4];
		BYTE		driveset = 0;
		BYTE		fdcdummyread = 0;
		WORD		readdiag = 0;

static const UINT8 fdctype[] = {1,1,1,1,1,1,1,1,2,2,2,2,3,4,3,3};


static BYTE fdd_stat(void) {

	FDDFILE fdd;

	fdd = fddfile + fdc.drv;
	switch(fdd->type) {
		case DISKTYPE_NOTREADY:
			return(0);

		case DISKTYPE_BETA:
			return(fdd_stat_2d());

		case DISKTYPE_D88:
		default:
			return(fdd_stat_d88());
	}
	return(0);
}


/***********************************************************************
	�e�c�b �i�w�P���猩��h�^�n�ɑ�������֐��j
***********************************************************************/

static int inc_off(void) {

	BYTE	ret;
	FDDFILE fdd;

	if (!fdc.motor) {
		return(0);
	}
	if (fdcdummyread) {
		fdcdummyread--;
		return(0);
	}
	else {
		fdd = fddfile + fdc.drv;
		switch(fdd->type) {
			case DISKTYPE_NOTREADY:
				return(0);

			case DISKTYPE_BETA:
				ret = fdd_incoff_2d();
				break;

			case DISKTYPE_D88:
			default:
				ret = fdd_incoff_d88();
				break;
		}
		if (ret) {
			dma.DMA_REDY = 8;	// <- DMA � ��ި�ݺ޳ � �ر
		}
	}
	return(ret);
}

void IOOUTCALL fdc_o(UINT port, REG8 value) {

	REG8	cmd;

	if ((port & (~7)) != 0x0ff8) {
		return;
	}
	port &= 0xf;
	if (port == 8) {						// �R�}���h
		driveset = 0;
		fdc.cmd = value;
		cmd = (REG8)(value >> 4);
		fdc.type = fdctype[cmd];
		fdc.skip = 2;
		switch(cmd) {
			case 0x00:								// ���X�g�A
				if (value & 8) {					// LAYDOCK
					fdc.skip = 0;
					fdc.busyclock = 20;				// ver0.25 (now testing)
					fdc.busystart = h_cntbase + h_cnt;
				}
				fdc.motor = 0x80;					// ���[�^�[On?
				fdc.c = 0;
				fdc.treg = 0;
				FDDMTR_MOVE;
				fdc.step = 1;
#if 1												// ver0.25
				fdc.r = 0;							// �f�[�j���[���h
				fdc.rreg = 0;
#endif
				break;

			case 0x01:								// �V�[�N
				fdc.motor = 0x80;					// ���[�^�[On
				fdc.step = (char)((fdc.c<=fdc.data)?1:-1);
#if 0												// �X�^�N��		// ver0.50
				fdc.rreg = fdc.r = 0;
#endif
				fdc.c = fdc.data;
				fdc.treg = fdc.data;
				FDDMTR_MOVE;
				break;

			case 0x02:								// �X�e�b�v
			case 0x03:
				if (fdc.motor) {
					fdc.c += fdc.step;
					if (cmd & 1) {
						FDDMTR_MOVE;
						fdc.treg = fdc.c;
					}
				}
				break;

			case 0x04:								// �X�e�b�v�C��
			case 0x05:
				if (fdc.motor) {
					fdc.step = 1;
					fdc.c++;
					if (cmd & 1) {
						FDDMTR_MOVE;
						fdc.treg = fdc.c;
					}
				}
				break;

			case 0x06:								// �X�e�b�v�A�E�g
			case 0x07:
				if (fdc.motor) {
					fdc.step = -1;
					fdc.c--;
					if (cmd & 1) {
						FDDMTR_MOVE;
						fdc.treg = fdc.c;
					}
				}
				break;

			case 0x08:								// ���[�h�f�[�^
			case 0x09:
			case 0x0a:								// ���C�g�f�[�^
			case 0x0b:
				fdc.off = 0;
				fdcdummyread = 2;
				if (fdc.motor) {
					if (dma.DMA_ENBL) {
						dma.DMA_REDY = 0;
//						fdc.skip = 0;			// DMA�Ŋ��荞�݂��Ď����鎖�I
					}
				}
				break;

			case 0xc:								// ���[�h�A�h���X
				fdc.s.bufpos = 0;
				fdc.s.bufsize = 0;
				fdcdummyread = 2;
				if (fdc.motor) {									// ver0.25
					if (dma.DMA_ENBL) {
						dma.DMA_REDY = 0;
					}
				}
				{
					FDDFILE fdd = fddfile + fdc.drv;
					switch(fdd->type) {
						case DISKTYPE_BETA:
							fdd2d_crc(fdd);
							break;

						case DISKTYPE_D88:
						default:
							fddd88_crc(fdd);
							break;
					}
				}
				break;

			case 0x0d:								// �t�H�[�X�C���^���v�g
				fdcdummyread = 0;					// �K�v�Ȃ��H
				fdc.skip = 0;						// 000330
				dma.DMA_REDY = 8;					// ver0.25
				break;

			case 0x0e:								// ���[�h�g���b�N
				readdiag = 0;
				break;

			case 0x0f:								// ���C�g�g���b�N
				{
					FDDFILE fdd = fddfile + fdc.drv;
					switch(fdd->type) {
						case DISKTYPE_NOTREADY:
						case DISKTYPE_BETA:
							break;

						case DISKTYPE_D88:
						default:
							if (fdc.motor) {						// ver0.25
								init_tao_d88();
								if (dma.DMA_ENBL) {
									dma.DMA_REDY = 0;
								}
							}
							break;
					}
				}
				break;
		}
	}
	else {
		cmd = (REG8)(fdc.cmd >> 4);
		switch(port) {
			case 0x09:								// �g���b�N
				fdc.treg = value;
				break;

			case 0x0a:								// �Z�N�^
				FDDMTR_WAITSEC(value);
				fdc.r = value;
				fdc.rreg = value;
				break;

			case 0x0b:								// �f�[�^
				fdc.data = value;
				if ((cmd == 0x0a) || (cmd == 0x0b)) {
					FDDFILE fdd = fddfile + fdc.drv;
					switch(fdd->type) {
						case DISKTYPE_NOTREADY:
							break;

						case DISKTYPE_BETA:
							fdd_write_2d();
							break;

						case DISKTYPE_D88:
						default:
							fdd_write_d88();
							break;
					}
					inc_off();
				}
				else if (cmd == 0x0f) {				// TRACK WRITE
					FDDFILE fdd = fddfile + fdc.drv;
					switch(fdd->type) {
						case DISKTYPE_NOTREADY:
						case DISKTYPE_BETA:
							break;

						case DISKTYPE_D88:
						default:
							fdd_wtao_d88(value);
							break;
					}
				}
				break;

			case 0x0c:								// �h���C�u�E�T�C�h
				driveset = 1;
				FDC_c[fdc.drv] = fdc.c;
				fdc.c = FDC_c[value & 0x03];			// XTAL !!!
				fdc.motor = (UINT8)(value & 0x80);
				fdc.drv = (UINT8)(value & 0x03);
				fdc.h = (UINT8)((value >> 4) & 1);

				fdc.cmd = 0;							// T&E SORCERIAN
//				fdc.data = 0;							// �e�̓`��
				fdc.type = 0;

				FDDMTR_DRVSET;
				if (!fdc.motor) {
					fdc.r = 0;							// SACOM TELENET
					fdc.rreg = 0;
#if 0													// XTAL
					fdc.c = 0;
					fdc.step = 1;
#endif
				}
				break;
		}
	}
}

REG8 IOINPCALL fdc_i(UINT port) {

static	BYTE	timeoutwait;
static	BYTE	last_r;
static	short	last_off;
		BYTE	ans;
		REG8	cmd;

	if ((port & (~7)) != 0x0ff8) {
		return(0xff);
	}
	cmd = (REG8)(fdc.cmd >> 4);
	if ((port &= 0xf) != 8) {
		last_r = -1;
		last_off = -1;
		timeoutwait = 4;
	}
	switch(port) {
		case 0x8:	// �ð��
			ans = 0;
			fdcdummyread = 0;
			if (fdc.skip) {
				fdc.skip--;
				return(0x01);
			}
			if (fdc.busyclock) {					// ver0.25
				if (((h_cntbase + h_cnt) - fdc.busystart) < fdc.busyclock) {
					return(0x01);
				}
				fdc.busyclock = 0;
			}
		{
			FDDFILE fdd = fddfile + fdc.drv;
			if (fdd->type == DISKTYPE_NOTREADY) {
				if (fdc.type == 1 && fdc.c == 0) {	// �h���C�u�`�F�b�N !!!
					return(0x84);					// ���ڑ�����Ă鎞����
				}
				return(0x80);
			}
		}
			if (fdc.type == 2) {
				if (last_r == fdc.r && last_off == fdc.off &&
						!(--timeoutwait)) {
					inc_off();
					timeoutwait = 4;
				}
				last_r = fdc.r;
				last_off = fdc.off;
			}							// Read Write���݂̂̕ω��ł�����
			if (!((ans = fdd_stat()) & 2)) {
				dma.DMA_REDY = 8;				// <- DMA � ��ި�ݺ޳ � �ر
			}
#if 1
			if (driveset) {					// 0xffc��@�������ゾ������
				ans &= 0xc4;
			}
#endif
			return(ans);

		case 0x9:							// �ׯ�
			return(fdc.treg);

		case 0xa:							// ���
			return(fdc.rreg);				// ver0.25

		case 0xb:							// �ް�
			if (fdc.motor) {
				if ((cmd == 0x08) || (cmd == 0x09)) {	// ذ�ޥ�ް�
					FDDFILE fdd;
					fdd = fddfile + fdc.drv;
					switch(fdd->type) {
						case DISKTYPE_NOTREADY:
							break;

						case DISKTYPE_BETA:
							fdd_read_2d();
							break;

						case DISKTYPE_D88:
						default:
							fdd_read_d88();		// WOODY POCO !!!
							break;
					}
					inc_off();
				}
				else if (cmd == 0x0c) {			// ذ�ޥ���ڽ
					if (fdc.s.bufpos < 6) {
						fdc.data = fdc.s.buffer[fdc.s.bufpos];
						if (fdcdummyread) {
							fdcdummyread--;
						}
						else {
							fdc.s.bufpos++;
						}
					}
				}
				else if (cmd == 0x0e) {								// ver0.25
					fdc.data = 0;
					readdiag++;
				}
			}
			return(fdc.data);					// WOODY POCO !!!

//		case 0xc:								// FM
//		case 0xd:								// MFM
//			break;

		case 0xe:								// 1.6M
			fdc.s.media = DISKTYPE_2HD;
			return(0xff);

		case 0xf:								// 500K/1M
			fdc.s.media = DISKTYPE_2D;
			return(0xff);
	}
	return(0);
}


// ----

void fdc_reset(void) {

	FDDMTR_INIT;
	ZeroMemory(&fdc, sizeof(fdc));
	fdc.step = 1;
	ZeroMemory(FDC_c, 4);
}

