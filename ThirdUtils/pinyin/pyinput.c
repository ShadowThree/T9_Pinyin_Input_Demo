#include "pymb.h"
#include "pyinput.h"
#include "string.h"
#include "dbger.h"

pyinput t9 = {get_pymb, 0};

// 比较两个字符串的匹配情况
// 返回值: 0xff-表示完全匹配;
//		 		 其他-匹配的字符数
uint8_t str_match(uint8_t* str1, uint8_t* str2)
{
	uint8_t i = 0;
	while(1) {
		if(*str1 != *str2) break;								//部分匹配
		if(*str1 == '\0') {i = 0XFF; break;}		//完全匹配
		i++;
		str1++;
		str2++;
	}
	return i;		//两个字符串相等
}

// 获取匹配的拼音码表
// *strin: 输入的字符串,形如:"726"
// **matchlist: 输出的匹配表.
// 返回值: bit7: 0-表示完全匹配；1-表示部分匹配（仅在没有完全匹配的时候才会出现）
//		 		 bit[6-0]: 完全匹配的时候，表示完全匹配的拼音个数
//			   部分匹配的时候，表示有效匹配的位数				    	 
uint8_t get_matched_pymb(uint8_t *strin, py_index **matchlist)
{
	py_index *bestmatch = 0;		// 最佳匹配
	uint16_t pyindex_len = 0, i = 0;
	uint8_t temp, mcnt = 0, bmcnt = 0;
	
	bestmatch = (py_index*)&py_index3[0];			// 默认为a的匹配
	pyindex_len = sizeof(py_index3) / sizeof(py_index3[0]);			// 得到py索引表的大小.
	for(i = 0; i < pyindex_len; i++)
	{
		temp = str_match(strin, (uint8_t*)py_index3[i].py_input);
		if(temp)
		{
			if(temp==0xFF) {
				matchlist[mcnt++] = (py_index*)&py_index3[i];
			} else if(temp>bmcnt) {			//找最佳匹配
				bmcnt = temp;
			  bestmatch = (py_index*)&py_index3[i];		//最好的匹配.
			}
		}
	}
	if(mcnt == 0 && bmcnt)//没有完全匹配的结果,但是有部分匹配的结果
	{
		matchlist[0] = bestmatch;
		mcnt = bmcnt | 0x80;		//返回部分匹配的有效位数
	}
	return mcnt;//返回匹配的个数
}

//得到拼音码表.
//str:输入字符串
//返回值:匹配个数.
uint8_t get_pymb(uint8_t* str)
{
	return get_matched_pymb(str, t9.pymb);
}

// inputstr:	input number string, such as "24" for ["ai", "bi", "ci"], "248264" for ["chuang"];
void test_number(char *inputstr)
{
	uint8_t t = 0, i = 0;
	
	t = t9.getpymb((uint8_t*)inputstr);
	
	if(t & 0X80) {
		printf("\n输入数字为: %s\n", inputstr);
		printf("部分匹配位数: %d\n", t & 0X7F);
		printf("部分匹配结果(%s): %s\n", t9.pymb[0]->py, t9.pymb[0]->pymb);
		printf("\t[%d]:", strlen((char*)(t9.pymb[0]->pymb)));
		for(uint16_t len = 0; len < strlen((char*)(t9.pymb[0]->pymb)); len++) {
			printf(" %02X", t9.pymb[0]->pymb[len]);
		}
		printf("\n");
	} else if(t) {
		printf("\n输入数字为: %s\n", inputstr);
		printf("完全匹配个数: %d\n", t);
		printf("完全匹配的结果:\n");
		for(i = 0; i < t; i++) {
			printf("\t%s: %s\n", t9.pymb[i]->py, t9.pymb[i]->pymb);
			printf("\t[%d]:", strlen((char*)(t9.pymb[i]->pymb)));
			for(uint16_t len = 0; len < strlen((char*)(t9.pymb[i]->pymb)); len++) {
				printf(" %02X", t9.pymb[i]->pymb[len]);
			}
			printf("\n");
		}
	} else {
		printf("%s: 没有任何匹配结果！\n", inputstr);
	}
	printf("\n");
}

// @in	pinyin:	such as "a", "bi", "chuang";
void test_pinyin(char* pinyin)
{
	uint8_t match_res;
	uint16_t pyindex_len = 0, i = 0;
	
	pyindex_len = sizeof(py_index3) / sizeof(py_index3[0]);			// 得到py索引表的大小.
	for(; i < pyindex_len; i++) {
		match_res = str_match((uint8_t*)pinyin, (uint8_t*)py_index3[i].py);
		if(match_res == 0xFF) {
			printf("input pinyin: %s\n", pinyin);
			printf("match result: %s\n", py_index3[i].pymb);
			printf("\t[%d]:", strlen((char*)(py_index3[i].pymb)));
			for(uint16_t len = 0; len < strlen((char*)(py_index3[i].pymb)); len++) {
				printf(" %02X", py_index3[i].pymb[len]);
			}
			printf("\n");
			break;
		}
	}
	if(i == pyindex_len) {
		printf("NOT match!\n");
	}
	printf("\n");
}
