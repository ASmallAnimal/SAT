//
// Created by spiri on 2021/2/25.
//

#ifndef SAT_SUDOKU_H
#define SAT_SUDOKU_H

#include "function.h"
#include "dpll.h"
#include <cmath>

void Sudoku() {
    int mark_accessory[100];
    int answer[50] = {0};
    int position[50] = {0};
    FILE *fp1 = NULL;
    //FILE *fp = NULL;
    int flag = FALSE, op = 1;

    char filename[100];
    char conf[1000];
    char result[100];
    int t2, t1;
    int n, fileLen;
    int count = 0;
    int hole;
    int digReady = YES;
    int rlblank, blank;
    int confine;

    headNode_ptr T;
    headNode Arr;
    T = &Arr;
    dChart arr;
    dChart_ptr G;
    G = &arr;

    isGame = TRUE;

    while (op) {
        system("cls");
        printf("******\t\tSudoku\t\t******\n");
        printf("\t1. 挖洞法生成棋盘\t2. 读入已有棋盘\n");
        printf("\t0. 返回上一级\n");
        printf("请输入对应功能前的数字：");
        scanf("%d", &op);
        switch (op) {
            case 1:
                printf("请输入生成棋盘的阶数，并选择棋盘存储的位置！\n");
                printf("棋盘的阶数为：");
                scanf("%d", &order);
                fileLen = (int) pow(order, 2);
                printf("生成棋盘存储的位置为：(.txt)结尾\n");
                scanf("%s", filename);
                //生成棋盘终盘
                while (flag != OK) {
                    GenerateSquare(filename);
                    Unique(filename);
                    strcpy(result, filename);
                    for (int i = 0; i < 70; i++) {
                        if (result[i] == '.') {
                            result[i + 1] = 'c';
                            result[i + 2] = 'n';
                            result[i + 3] = 'f';
                            break;
                        }
                    }
                    fp1 = fopen(result, "r");
                    readCnf(&T, fp1, G);
                    fclose(fp1);
                    for (int i = 1; i < variable_number + 1; i++)
                        mark_true[i] = 0;
                    Mark(T, mark_times, mark_number);
                    strategy = 2;
                    flag = Dpll(T, 1, G);
                }
                printf("终盘生成成功！\n");
                outputSquare(fileLen, mark_true, count);

                printf("正在执行挖洞法...（难度简单）\n");
                //保留棋盘副本
                for (int i = 1; i <= fileLen; ++i)
                    mark_accessory[i] = mark_true[i];
                //设置限定条件
                rlblank = order - 1, blank = fileLen / 2 - 6;
                confine = TRUE;
                //初始化时间种子
                srand((unsigned) time(NULL));
                while (blank) {
                    //约束一
                    hole = rand() % fileLen;
                    if (hole == 0) continue;
                    //约束2
                    int b_count = 0;
                    for (int i = 1; i <= fileLen; ++i) {
                        if (mark_accessory[i] == 0) b_count++;
                        if (i % order == 0) {
                            if (b_count > rlblank) {
                                confine = FALSE;
                                break;
                            }
                            b_count = 0;
                        } else confine = TRUE;
                    }

                    //挖洞
                    if (confine == TRUE) {
                        if (mark_accessory[hole] != 0) {
                            if (mark_accessory[hole] == -1)
                                mark_accessory[hole] = 1;
                            else mark_accessory[hole] = -1;
                            digReady = YES;
                        } else digReady = NO;
                    } else continue;
                    if (digReady == NO) continue;

                    //检验挖洞唯一解的正确性
                    saveSquare(filename, mark_accessory, fileLen);
                    readSquare(filename, conf, fileLen);
                    SudokuToCnf(conf, fileLen, filename);
                    fp1 = fopen(result, "r");
                    readCnf(&T, fp1, G);
                    fclose(fp1);
                    for (int i = 1; i < variable_number + 1; i++)
                        mark_true[i] = 0;
                    Mark(T, mark_times, mark_number);
                    strategy = 2;
                    flag = Dpll(T, 1, G);
                    if (flag == FALSE) {
                        if (mark_accessory[hole] == -1)
                            answer[hole] = 1;
                        else answer[hole] = -1;
                        mark_accessory[hole] = 0;
                        digReady = YES;
                        blank--;
                    } else {
                        if (mark_accessory[hole] == -1)
                            mark_accessory[hole] = 1;
                        else mark_accessory[hole] = -1;
                    }
                }

                //保存挖洞之后的棋盘
                saveSquare(filename, mark_accessory, fileLen);
                //打印挖洞之后的棋盘
                printf("使用挖洞法生成棋盘成功！\n");
                outputSquare(fileLen, mark_accessory, count);

                //填写答案
                inputAnswer(position, count, answer);
                //检查答案
                checkAnswer(count, position, fileLen, answer);
                getchar();
                getchar();
                break;
            case 2:
                //读文件
                printf("请输入棋盘信息所在地址：\n");
                scanf("%s", filename);
                if (readSquare(filename, conf, fileLen) == OK) {
                    n = int(sqrt(fileLen));
                    printf("读入成功！数独为%d阶\n", n);
                    //打印棋盘：
                    for (int i = 1; i <= fileLen; i++) {
                        if (conf[i - 1] == 48)
                            printf(" 0 ");
                        else if (conf[i - 1] == 49)
                            printf(" 1 ");
                        else
                            printf(" _ ");
                        if (i % n == 0) printf("\n");
                    }
                }
                //转换文件
                SudokuToCnf(conf, fileLen, filename);
                printf("棋盘cnf文件保存成功！\n");
                printf("cnf文件文字数%d 公式数%d", variable_number, sentence_number);
                strcpy(result, filename);
                for (int i = 0; i < 70; i++) {
                    if (result[i] == '.') {
                        result[i + 1] = 'c';
                        result[i + 2] = 'n';
                        result[i + 3] = 'f';
                        break;
                    }
                }
                fp1 = fopen(result, "r");
                readCnf(&T, fp1, G);

                for (int i = 1; i < variable_number + 1; i++)
                    mark_true[i] = 0;
                Mark(T, mark_times, mark_number);
                t1 = clock();
                strategy = 2;
                flag = Dpll(T, 1, G);
                t2 = clock();
                printf("时间：%4d ms\n", t2 - t1);

                for (int i = 0; i < 70; i++) {
                    if (result[i] == '.') {
                        result[i + 1] = 'r';
                        result[i + 2] = 'e';
                        result[i + 3] = 's';
                        break;
                    }
                }

                if (flag == OK) {
                    fp1 = fopen(result, "wb");
                    fprintf(fp1, "%d\n", 1);
                    printf("cnf文件的解为：\n");
                    for (int i = 1; i < variable_number + 1; i++) {
                        if (mark_true[i] == 1) {
                            fprintf(fp1, "%d  ", i);
                        } else if (mark_true[i] == -1) {
                            fprintf(fp1, "%d  ", -i);
                        }
                        if (i % order == 0) printf("\n");
                    }
                    //打印求解之后的棋盘
                    printf("棋盘求解后为：\n");
                    outputSquare(fileLen, mark_true, count);
                    fprintf(fp1, "\n%d\n", t2 - t1);
                    fclose(fp1);
                } else {
                    fp1 = fopen(result, "wb");
                    fprintf(fp1, "%d\n", 0);
                    fprintf(fp1, "%d\n", t2 - t1);
                    fclose(fp1);
                    printf("dpll算法处理失败，棋盘无解！\n");
                }
                getchar();
                getchar();
                break;
            case 0:
                getchar();
                break;
        }
    }
}

#endif //SAT_SUDOKU_H
