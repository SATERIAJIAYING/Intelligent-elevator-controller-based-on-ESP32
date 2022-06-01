
#include <stdio.h>
#include <string.h>
/**
 * @brief URLEncode 对字符串URL编码
 *
 * @param str 原字符串
 * @param strSize 原字符串长度(不包括最后的\0)
 * @param result 结果缓冲区的地址
 * @param resultSize 结果缓冲区的大小(包括最后的\0)
 *
 * @return: >0:resultstring 里实际有效的长度
 *            0: 解码失败.
 */
int URLEncode(const char *str, const int strSize, char *result, const int resultSize)
{
	int i;
	int j = 0; //for result index
	char ch;

	if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0))
	{
		return 0;
	}
	result[0] = '{';
	result[1] = '\"';
	result[2] = 'i';
	result[3] = 'm';
	result[4] = 'a';
	result[5] = 'g';
	result[6] = 'e';
	result[7] = '\"';
	result[8] = ':';
	result[9] = '\"';
	j=10;
	for (i = 0; (i < strSize) && (j < resultSize); ++i)
	{
		ch = str[i];
		if (((ch >= 'A') && (ch < 'Z')) ||
			((ch >= 'a') && (ch < 'z')) ||
			((ch >= '0') && (ch < '9')))
		{
			result[j++] = ch;
		}
		else if (ch == ' ')
		{
			result[j++] = '+';
		}
		else if (ch == '.' || ch == '-' || ch == '_' || ch == '*')
		{
			result[j++] = ch;
		}
		else
		{
			if (j + 3 < resultSize)
			{
				sprintf(result + j, "%%%02X", (unsigned char)ch);
				j += 3;
			}
			else
			{
				return 0;
			}
		}
	}
	result[j++] = '\"';
	result[j++] = ',';
	const char *token = "\"access_token\":\"24.0faf0b824d3a9a426778d98ec74d8280.2592000.1656397553.282335-26339198\"";
	for (int i = 0; token[i]!='\0';i++)
		result[j++] = token[i];
	result[j++] = '}';
	result[j] = '\0';
	return j;
}
