/**
 * @file	main.m
 * @brief	���C��
 */

#import "compiler.h"
#include "../../xmil.h"
#include "../../dosio.h"

/**
 * ���C��
 * @param[in] argc ����
 * @param[in] argv ����
 * @return ���U���g �R�[�h
 */
int main(int argc, char *argv[])
{
	NSString *pstrBundlePath = [[NSBundle mainBundle] bundlePath];
	file_setcd([pstrBundlePath UTF8String]);

	for (int i = 1; i < argc; i++)
	{
		if (strncmp(argv[i], "-psn_", 5) == 0)
		{
			argc--;
			memmove(argv + i, argv + i + 1, (argc - i + 1) * sizeof(argv[0]));
			break;
		}
	}

	return xmil_main(argc, argv);
}
