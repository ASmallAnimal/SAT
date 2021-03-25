//
// Created by spiri on 2021/2/21.
//

#ifndef SAT_FUNCTION_H
#define SAT_FUNCTION_H

#include <stddef.h>//malloc等动态分配函数
#include <stdlib.h>//系统清屏函数
#include <stdio.h>//标准输入输出函数
#include <math.h>
#include <limits.h>
#include <string.h>//用于字符串文件读写操作
#include <time.h> //用于求解过程的计算

#define FALSE -1
#define OK 1
#define N 3500
#define RECORDED 1
#define DELETED 0
#define TRUE 1
#define ERROR 0
#define YES 0
#define NO 2

typedef int status;
int variable_number, sentence_number,vn;
int strategy;
int Flag;
int mark_true[100000]; //储存当前真值的下标 如果为零表示已经在进行测试或者已经经过测试
int mark_times[100000]; //按照各个文字出现的频次递减排序，存储的是下表
int mark_number[100000];  //真值的下标 按照出现频次降序排列
int c[100000] = {0};  //存储组合数
int signal = 1;
int isRowReady = ERROR;
int isGame = FALSE;
int difference = 1; //附加变元行列辨识符 1为行 2为列
int order;  //读取数独的阶数
int emptyClause = FALSE;
int singleClause;
int check;
//参考word设计的结构 存储cnf的公式
/*子句结构*/
typedef struct clauseNode {
    int variable; //变元文字的值
    int mark;   //标记变元是否在cnf文件中出现 1表示是 0表示否
    struct clauseNode *next;  //句子中的下一个文字
} clauseNode;

/*root 子句头节点集合*/
typedef struct headNode {
    int count; //一个句子中变元的个数
    int mark;  //标记公式中是否有该子句
    int flag;  //表示句子中选中的真值 句子删除之后用以标记
    clauseNode *content;
    struct headNode *next;
} headNode, *headNode_ptr;

//索引表 便于遍历 节约时间成本
typedef struct SearchNode {
    int signal;  //标记符号
    clauseNode *variable;//指向变元
    headNode *head;
    struct SearchNode *next;
} SearchNode, SearchList[N + 1];

typedef struct {//用于删除合适的假设真值
    SearchList searchList;
    int mark;
} dChart, *dChart_ptr;

//读取cnf文件的功能
status readCnf(headNode_ptr *Head, FILE *fp, dChart_ptr &G) {
    singleClause = 0;
    int inter, count;
    char ch;
    if (fp == NULL) {
        printf("文件打开失败！\n ");
        return FALSE;
    } else {
        if (isGame == FALSE) {
            for (int i = 0; i < 5; i++)
                fread(&ch, sizeof(char), 1, fp);
            //跳过p cnf 读取cnf文件配置信息
            fscanf(fp, "%d", &variable_number);
            fscanf(fp, "%d", &sentence_number);
            printf("cnf文件中的文字数：%d     公式数： %d\n", variable_number, sentence_number);
        }
        count = sentence_number;
        vn=sentence_number;
        //读取公式部分
        clauseNode *p = NULL, *temp = NULL;
        headNode *root = NULL, *reer = NULL;
        SearchNode *n = NULL, *m = NULL;

        /*
         * 优化后
         * 建立索引表 较小查找成本
         */
        for (int i = 0; i <= N; i++) {
            G->mark = 0;
            G->searchList[i].signal = 0;
            G->searchList[i].variable = NULL;
            G->searchList[i].next = NULL;
            G->searchList[i].head = NULL;
        }

        //储存cnf文件的公式，按照word文档所给的模板存储
        //创建邻接表的头
        root = (headNode *) malloc(sizeof(headNode));
        *Head = root;
        root->next = NULL;
        root->content = NULL;
        root->count = 0;
        root->mark = DELETED;
        root->flag = 0;
        reer = root;

        while (fscanf(fp, "%d", &inter) != EOF && count) {
            if (inter != 0) {
                //读入文字
                p = (clauseNode *) malloc(sizeof(clauseNode));
                p->next = NULL;
                p->variable = inter;
                p->mark = RECORDED; //已经记录 存在于cnf公式中
                if (reer->content == NULL) //表示是新接的句子
                    reer->content = p;
                else { //续接文字
                    temp = reer->content;
                    while (temp->next)
                        temp = temp->next; //遍历到链尾
                    temp->next = p;  ////接入下一个文字
                }
                reer->count++;

                //建立索引表
                n = (SearchNode *) malloc(sizeof(SearchNode));
                inter > 0 ? n->signal = 1 : n->signal = -1;
                n->variable = p;
                n->head = reer;
                n->next = NULL;
                m = G->searchList[abs(inter)].next;
                //链地址法处理冲突
                //为便于辨识 和统一性头部不存放文字信息 而是从next开始存放
                if (!m) G->searchList[abs(inter)].next = n;
                else {
                    while (m->next)
                        m = m->next;
                    m->next = n;
                }
            } else {
                if (reer->count == 1) singleClause++;
                //读入下一个公式
                headNode_ptr new_hnode = (headNode *) malloc(sizeof(headNode));
                new_hnode->next = NULL;
                new_hnode->content = NULL;
                new_hnode->count = 0;
                new_hnode->mark = DELETED;
                new_hnode->flag = 0;

                if (reer->content != NULL) reer->mark = RECORDED;
                reer->next = new_hnode;
                reer = new_hnode;

                count--;
            }
        }
    }
    return OK;
}

//输出cnf文件
status Traverse(headNode_ptr Head) {
    clauseNode *p = NULL;
    headNode *q = NULL;
    q = Head;
    printf("==============================\n");
    while (q) {
        if (q->next != NULL)printf("%3d : ", q->count);
        p = q->content;
        while (p) {
            if (p->mark == RECORDED)
                printf("%6d", p->variable);
            else printf(" 0 ");
            p = p->next;
        }
        if (q->mark == RECORDED) printf("\n");
        q = q->next;
    }
    printf("==============================\n");
    return OK;
}


//保存cnf文件
status Save_File(headNode_ptr Head) {
    clauseNode *p = NULL;
    headNode *q = NULL;
    FILE *fp;
    char filename[60];
    printf("input file name: ");
    scanf("%s", filename);
    if ((fp = fopen(filename, "wb")) == NULL) {
        printf("File open error！\n ");
        return FALSE;
    } else {
        fprintf(fp, "%s", "p cnf ");
        fprintf(fp, "%d ", variable_number);
        fprintf(fp, "%d \n", sentence_number);
        q = Head;
        while (q) {
            p = q->content;
            while (p) {
                fprintf(fp, "%d  ", p->variable);
                p = p->next;
            }
            fprintf(fp, "%d \n", 0);
            q = q->next;
        }
        fclose(fp);
    }
    return OK;
}

//消除链表
void Realse_Link(headNode_ptr *Head) {
    clauseNode *p = NULL, *r = NULL;
    headNode *q = NULL, *t = NULL;

    q = *Head;
    while (q) {
        p = q->content;
        while (p) {
            r = p->next;
            free(p);
            p = r;
        }
        q->content = NULL;
        q = q->next;
    }
    q = *Head;
    while (q) {
        t = q->next;
        free(q);
        q = t;
    }
    *Head = NULL;
}

//标记函数 将cnf文件中出现的文字标记 后续作为正负标记
//优化之后找到出现次数最多的数，可以较快的化简集合S
void Mark(headNode_ptr Head, int markTimes1[], int markNumber[]) {
    int temp, t;
    clauseNode *p;
    headNode *q;
    q = Head;
    while (q) {
        if (q->mark == RECORDED) {
            p = q->content;
            while (p) {
                markTimes1[abs(p->variable)]++;
                p = p->next;
            }
        }
        q = q->next;
    }

    for (int i = 1; i <= variable_number; i++) {
        temp = 0;
        t = 0;
        for (int word = 1; word <= variable_number; word++) {
            if (markTimes1[word] > temp) {
                t = word;
                temp = markTimes1[word];
            }
        }
        markNumber[i] = t;
        markTimes1[t] = 0;
    }
}


/*删除子句 和 变元*/
status Delete(dChart_ptr G, int flag) {
    SearchNode *p;
    p = G->searchList[abs(flag)].next;
    while (p) {
        if (p->variable->variable == flag && p->head->mark == RECORDED) {//删除子句
            if (p->head->count == 1) singleClause--;
            p->head->mark = DELETED;
            p->head->flag = flag;
            p = p->next;
            sentence_number--;
            continue;
        }
        if (p->variable->variable == -flag && p->head->mark == RECORDED && p->variable->mark == RECORDED) { //删除变元
            p->variable->mark = DELETED;
            p->head->count--;
            if (p->head->count == 0) emptyClause = TRUE;
            if (p->head->count == 1) singleClause++;
        }
        p = p->next;
    }
    return OK;
}

//纯文字规则
int pureWord(dChart_ptr G) {
    for (int i = 1; i <= variable_number; ++i) {
        if (mark_true[i] != 0) continue;
        SearchNode *p = G->searchList[i].next;
        int sgl = p->signal;
        while (p) {
            //跳过单子句
            if (p->head->count == 1) break;
            //记录纯文字的条件
            //文字所在句子存在 文字存在
            if (p->head->mark != 0 && p->variable->mark == RECORDED) {
                if (sgl == p->signal) {
                    sgl = p->signal;
                    p = p->next;
                } else break;
                //创造单子句
                if (!p) {
                    printf("纯文字为%d\n", i);
                    sgl == 1 ? mark_true[i] = 1 : mark_true[-i] = -1;
                    return abs(i);
                }
            } else p = p->next;
        }
    }
}


//恢复单子句
status Rewind(dChart_ptr G, int flag) {
    SearchNode *p;
    p = G->searchList[abs(flag)].next;
    while (p) {
        //为负，恢复变元
        if (p->variable->variable == -flag && p->head->mark == RECORDED && p->variable->mark == DELETED) {
            p->variable->mark = RECORDED;
            p->head->count++;
            if (p->head->count == 1) singleClause++;
        } else if (p->variable->variable == flag && p->head->mark == DELETED &&
                   p->head->flag == flag) {//为正，恢复子句
            p->head->mark = RECORDED;
            p->head->flag = 0;
            sentence_number++;
            if (p->head->count == 1) singleClause++;
        }
        p = p->next;
    }
    return OK;
}

//寻找假设的真值
int findVariable(headNode *Head) {
    int flag;
    clauseNode *p = NULL;
    headNode *q = NULL;
    q = Head;
    //先找单子句
    if (singleClause != 0)
        while (q) {
            p = q->content;
            //这里检索单子句也可以优化
            if (q->count == 1 && q->mark == RECORDED)//记录变元数量
            {
                while (p) {
                    if (p->mark == RECORDED) {
                        flag = p->variable;
                        if (mark_true[abs(flag)] == 0) {
                            flag > 0 ? mark_true[flag] = 1 : mark_true[-flag] = -1;
                            return flag;
                        }
                    }
                    p = p->next;
                }
            }
            q = q->next;
        }
    //如果没有找到单子句 就假设选取文字数为2的子句 且 出现平均频次较多 的文字是下一个真值
    if (strategy == 1) {
        for (int i = 1; i <= variable_number; i++) {
            flag = mark_number[i];
            if (flag != 0 && mark_true[flag] == 0) {
                mark_true[flag] = 1;
                return flag;
            }
        }
    } else if (strategy == 2) {
        for (int i = 1; i <= variable_number; i++) {
            if (mark_true[i] == 0) {
                mark_true[i] = 1;
                return i;
            }
        }
    }
    return 0;
}

//读入棋格盘
status readSquare(char filename[], char *conf, int &fileLen) {
    FILE *fp = NULL;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("文件读取失败！\n");
        return FALSE;
    } else {
        fseek(fp, 0, SEEK_END);
        fileLen = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        fread(conf, fileLen, sizeof(char), fp);
        fclose(fp);
        return OK;
    }
}

//匹配棋盘变元序列
int match_normal(int variable) {
    return (variable / 10 - 1) * order + (variable % 10);
}

//计算组合数的个数
int factorial(int m, int n) {
    int ans = 1;
    if (m < n - m) m = n - m;
    for (int i = m + 1; i <= n; i++) ans *= i;
    for (int j = 1; j <= n - m; j++) ans /= j;
    return ans * order;
}

//整数求和
int sum(int high) {
    int sum = 0;
    for (int i = 0; i < high; i++)
        sum += i;
    return sum;
}

//附加变元转换函数
int match_extra(int variable) {
    int count = 0;
    int pn;
    int high, low, leave;
    int temp = variable;
    pn = variable / 1000;
    high = (variable / 10 - pn * 100) / 10;
    low = (variable / 10 - pn * 100) % 10;
    leave = variable % 10;
    while (temp) {
        count++;
        temp /= 10;
    }
    if (count == 4) {
        if (pn == 1)
            return order * order + (high * order - (order - 1 + sum(high)) - 1) * order + (low - high - 1) * order +
                   leave;
        if (pn == 2)
            return order * order + factorial(2, order) + (high * order - (order - 1 + sum(high)) - 1) * order +
                   (low - high - 1) * order + leave;
    }
    if (count == 5) {
        int ini = match_extra(variable / 10);
        int ini2 = order * order + 2 * factorial(2, order) + (ini - order * order - 1) * 2 + leave + 1;
        return ini2;
    }
}


//cnf规则吸入文件
void print(FILE *fp, int M, int j) {
    static int count = 0;
    static int d_count = 0;
    int high;
    int combine[order];
    static int isHigh = 1;
    int first, second;
    for (int i = 0; i < M; ++i) {
        //规则二
        if (difference == 0) {
            if (signal == 1)
                fprintf(fp, "%d ", (isRowReady == ERROR) ? match_normal((j + 1) * 10 + c[i + 1]) : match_normal(
                        c[i + 1] * 10 + j + 1));
            if (signal == -1)
                fprintf(fp, "%d ", (isRowReady == ERROR) ? -match_normal((j + 1) * 10 + c[i + 1]) : -match_normal(
                        c[i + 1] * 10 + j + 1));
        }
        //规则三 行
        if (difference == 1) {
            if (isHigh == 1) {
                high = c[i + 1] * 10;
                isHigh = 0;
            } else {
                for (int t = 0; t < order; t++) {
                    combine[t] = (difference * 100 + high + c[i + 1]) * 10 + t + 1;
                    fprintf(fp, "%d ", -match_extra(combine[t]));
                    d_count++;
                }
                if (d_count % order == 0 && difference != 0) {
                    fprintf(fp, "0\n");
                    variable_number += order;
                    sentence_number++;
                }
                for (int t = 0; t < order; t++) {
                    fprintf(fp, "%d %d 0\n", -match_extra(combine[t] * 10 + 1), match_extra(combine[t]));
                    fprintf(fp, "%d %d 0\n", -match_extra(combine[t] * 10 + 0), match_extra(combine[t]));
                    fprintf(fp, "%d %d %d 0\n", match_extra(combine[t] * 10 + 1), match_extra(combine[t] * 10 + 0),
                            -match_extra(combine[t]));
                    first = ((combine[t] - t - 1) / 10 - difference * 100) / 10;
                    second = ((combine[t] - t - 1) / 10 - difference * 100) % 10;
                    fprintf(fp, "%d %d 0\n", match_normal(first * 10 + t + 1), -match_extra(combine[t] * 10 + 1));
                    fprintf(fp, "%d %d 0\n", match_normal(second * 10 + t + 1), -match_extra(combine[t] * 10 + 1));
                    fprintf(fp, "%d %d %d 0\n", -match_normal(first * 10 + t + 1), -match_normal(second * 10 + t + 1),
                            match_extra(combine[t] * 10 + 1));
                    fprintf(fp, "%d %d 0\n", -match_normal(first * 10 + t + 1), -match_extra(combine[t] * 10 + 0));
                    fprintf(fp, "%d %d 0\n", -match_normal(second * 10 + t + 1), -match_extra(combine[t] * 10 + 0));
                    fprintf(fp, "%d %d %d 0\n", match_normal(first * 10 + t + 1), match_normal(second * 10 + t + 1),
                            match_extra(combine[t] * 10 + 0));
                    variable_number += 2;
                    sentence_number += 9;
                }

                isHigh = 1;
            }
        }
        //规则三 列
        if (difference == 2) {
            if (isHigh == 1) {
                high = c[i + 1] * 10;
                isHigh = 0;
            } else {
                for (int t = 0; t < order; t++) {
                    combine[t] = (difference * 100 + high + c[i + 1]) * 10 + t + 1;
                    fprintf(fp, "%d ", -match_extra(combine[t]));
                    d_count++;
                }
                if (d_count % order == 0 && difference != 0) {
                    fprintf(fp, "0\n");
                    variable_number += order;
                    sentence_number++;
                }
                for (int t = 0; t < order; t++) {
                    fprintf(fp, "%d %d 0\n", -match_extra(combine[t] * 10 + 1), match_extra(combine[t]));
                    fprintf(fp, "%d %d 0\n", -match_extra(combine[t] * 10 + 0), match_extra(combine[t]));
                    fprintf(fp, "%d %d %d 0\n", match_extra(combine[t] * 10 + 1), match_extra(combine[t] * 10 + 0),
                            -match_extra(combine[t]));
                    first = ((combine[t] - t - 1) / 10 - difference * 100) / 10;
                    second = ((combine[t] - t - 1) / 10 - difference * 100) % 10;
                    fprintf(fp, "%d %d 0\n", match_normal(first * 10 + t + 1), -match_extra(combine[t] * 10 + 1));
                    fprintf(fp, "%d %d 0\n", match_normal(second * 10 + t + 1), -match_extra(combine[t] * 10 + 1));
                    fprintf(fp, "%d %d %d 0\n", -match_normal(first * 10 + t + 1), -match_normal(second * 10 + t + 1),
                            match_extra(combine[t] * 10 + 1));
                    fprintf(fp, "%d %d 0\n", -match_normal(first * 10 + t + 1), -match_extra(combine[t] * 10 + 0));
                    fprintf(fp, "%d %d 0\n", -match_normal(second * 10 + t + 1), -match_extra(combine[t] * 10 + 0));
                    fprintf(fp, "%d %d %d 0\n", match_normal(first * 10 + t + 1), match_normal(second * 10 + t + 1),
                            -match_extra(combine[t] * 10 + 0));
                    variable_number += 2;
                    sentence_number += 9;
                }
                isHigh = 1;
            }
        }
        if (difference == 0) {
            count++;
            if (count % M == 0 && difference == 0) {
                fprintf(fp, "0\n");
                sentence_number++;
            }
        }
    }
}

//大行组合数
void comp_rl(FILE *fp, int m, int n, int M) {
    if (m == M + 1) {
        print(fp, M, 0);
    } else {
        for (c[m] = c[m - 1] + 1; c[m] <= n - M + m; c[m]++) {
            comp_rl(fp, m + 1, n, M);
        }
    }
}

//数独条件条件二的组合数
void comp(FILE *fp, int m, int n, int M, int j) {
    if (m == M + 1) {
        print(fp, M, j);
    } else {
        for (c[m] = c[m - 1] + 1; c[m] <= n - M + m; c[m]++) {
            comp(fp, m + 1, n, M, j);
        }
    }
}


//数独配置文件写成cnf文件
status SudokuToCnf(char conf[], int fileLen, char filename[]) {
    variable_number = fileLen;
    sentence_number = 0;
    FILE *fp;

    char result[100];
    strcpy(result, filename);
    for (int i = 0; i < 70; i++) {
        if (result[i] == '.') {
            result[i + 1] = 'c';
            result[i + 2] = 'n';
            result[i + 3] = 'f';
            break;
        }
    }
    fp = fopen(result, "w");
    //遍历产生单子句集
    order = int(sqrt(fileLen));
    for (int i = 0; i < fileLen; i++) {
        int single = (i / order + 1) * 10 + i % order + 1;
        single = match_normal(single);
        if (*conf == 49) {
            fprintf(fp, "%d ", single);
            fprintf(fp, "0\n");
        }
        if (*conf == 48) {
            fprintf(fp, "%d ", -single);
            fprintf(fp, "0\n");
        }
        conf++;
    }

    //写入规则一：
    for (int j = 0; j < order; j++) {
        for (int i = 0; i < order - 2; i++) {
            int vicinity = (j + 1) * 10 + i + 1;
            vicinity = match_normal(vicinity);
            //行
            fprintf(fp, "%d %d %d 0\n", vicinity, vicinity + 1, vicinity + 2);
            sentence_number++;
            fprintf(fp, "%d %d %d 0\n", -vicinity, -(vicinity + 1), -(vicinity + 2));
            sentence_number++;
            //列
            vicinity = (i + 1) * 10 + j + 1;
            vicinity = match_normal(vicinity);
            fprintf(fp, "%d %d %d 0\n", vicinity, vicinity + order, vicinity + 2 * order);
            sentence_number++;
            fprintf(fp, "%d %d %d 0\n", -vicinity, -(vicinity + order), -(vicinity + 2 * order));
            sentence_number++;
        }
    }

    //写入规则二
    difference = 0;
    //写入行
    isRowReady = ERROR;
    for (int j = 0; j < order; j++) {
        //正数
        comp(fp, 1, order, order / 2 + 1, j);
        signal = -1;
        //负数
        comp(fp, 1, order, order / 2 + 1, j);
        signal = 1;
    }
    isRowReady = TRUE;
    //写入列
    for (int j = 0; j < order; j++) {
        //正数
        comp(fp, 1, order, order / 2 + 1, j);
        signal = -1;
        //负数
        comp(fp, 1, order, order / 2 + 1, j);
        signal = 1;
    }

    //写入规则三
    //行不重复
    difference = 1;
    //先写大行
    //创建任意两行的组合数
    comp_rl(fp, 1, order, 2);
    //大列
    difference = 2;
    comp_rl(fp, 1, order, 2);

    fclose(fp);
    return OK;
};

//生成棋盘终盘
void GenerateSquare(char *filename) {
    FILE *fp = fopen(filename, "w");
    int length = (int) pow(order, 2);
    int square[20];
    //随机选取格子
    srand((unsigned) time(NULL));
    for (int i = 0; i < 9; ++i)
        square[i] = rand() % length;
    //排序
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 6 - i; ++j) {
            int temp;
            if (square[j] > square[j + 1]) {
                temp = square[j + 1];
                square[j + 1] = square[j];
                square[j] = temp;
            }

        }
    }
    //随机选取0 1,存入文件中
    int j = 0, dou;
    for (int i = 0; i < length; i++) {
        if (i == square[j]) {
            dou = rand();
            if (dou % 2 == 0) {
                fprintf(fp, "0");
            } else
                fprintf(fp, "1");
            j++;
            continue;
        }
        fprintf(fp, "*");
    }
    fclose(fp);
}

//生成终盘规则写入文件 下一步由dpll判定是否有解
void Unique(char *filename) {
    int fileLen = (int) pow(order, 2);
    char conf[100];
    readSquare(filename, conf, fileLen);
    SudokuToCnf(conf, fileLen, filename);
}

void showCnf(headNode_ptr Head) {
    headNode *p = Head;
    for (int i = 0; i < vn; ++i) {
        if (p->mark != 0) {
            clauseNode* q = p->content;
            while (q) {
                if (q->mark != 0)
                    printf("%d ", q->variable);
                else
                    printf("0 ");
                q = q->next;
            }
            printf("\n");
        }
        p = p->next;
    }
}

/*
 *
 * //寻找空子句
status findEmptyClause(headNode *Head) {
    headNode *q = NULL;
    q = Head;
    while (q) {
        if (q->count == 0 && q->mark == RECORDED)
            return YES;
        q = q->next;
    }
    return NO;
}
 */


/*
 * //判断s是否为空集
status emptySet() {
    if (sentence_number != 0) return FALSE;
    return OK;
}
 */
#endif //SAT_FUNCTION_H
