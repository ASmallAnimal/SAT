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
    int userAnswer[50];
    int position[50] = {0};
    FILE *fp1 = NULL;
    FILE *fp = NULL;
    int flag = FALSE, op = 1;

    char filename[100];
    char conf[1000];
    char result[100];
    int t2, t1;
    int n, fileLen;
    int count = 0;
    int hole;
    int digReady = YES;
    int singleTrue[500]={0};

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
        printf("\t1. 读取棋盘信息\t2. 保存棋盘cnf文件\n");
        printf("\t3. 输出棋盘cnf文件 \t4. 输出棋盘的解\n");
        printf("\t0. 返回上一级\n");
        printf("请输入对应功能前的数字：");
        scanf("%d", &op);
        switch (op) {
            case 1:
                printf("请选择自动生成棋盘还是读入已有棋盘求解！\n");
                printf("1--挖洞法生成棋盘 2--读入已有的棋盘：");
                int choice;
                scanf("%d", &choice);
                if (choice == 1) {
                    printf("请输入生成棋盘的阶数，并选择棋盘存储的位置！\n");
                    printf("棋盘的阶数为：");
                    scanf("%d", &order);
                    fileLen = (int) pow(order, 2);
                    printf("生成棋盘存储的位置为：(.txt)结尾\n");
                    scanf("%s", filename);
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
                    for (int i = 1; i < fileLen + 1; i++) {
                        if (mark_true[i] == 1) {
                            printf(" 1 ");
                        } else if (mark_true[i] == -1) {
                            printf(" 0 ");
                        }
                        if (i % order == 0) printf("\n");
                    }
                    printf("正在执行挖洞法...（难度简单）\n");
                    //棋盘副本
                    for (int i = 1; i <= fileLen; ++i)
                        mark_accessory[i] = mark_true[i];

                    int rlblank = order - 1, blank = fileLen / 2 - 6;
                    int confine = TRUE;
                    srand((unsigned) time(NULL)); //初始化时间种子

                    while (blank) { //约束一
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

                        //检验唯一解
                        fp = fopen(filename, "w");
                        for (int i = 1; i < fileLen + 1; i++) {
                            if (mark_accessory[i] == 0) fprintf(fp, "*");
                            else if (mark_accessory[i] == -1) {
                                fprintf(fp, "0");
                            } else
                                fprintf(fp, "1");
                        }
                        fclose(fp);
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
                    printf("使用挖洞法生成棋盘成功！\n");
                    for (int i = 1; i < fileLen + 1; i++) {
                        if (mark_accessory[i] == 1) {
                            printf(" 1 ");
                        } else if (mark_accessory[i] == -1) {
                            printf(" 0 ");
                        } else {
                            printf(" _ ");
                            count++;
                        }
                        if (i % order == 0) printf("\n");
                    }

                    printf("请输入上述数独棋盘的解，请用行列式表达！（ij v 分别表示行列，格子要填入的数）\n");
                    for (int i = 0; i < count; ++i) {
                        scanf("%d %d", &rlblank, &blank);
                        rlblank = match_normal(rlblank);
                        if (blank == 1)
                            userAnswer[rlblank] = 1;
                        if (blank == 0)
                            userAnswer[rlblank] = -1;
                        if (userAnswer[rlblank] != answer[rlblank])
                            position[i] = rlblank;
                    }
                    //checkAnswer
                    flag = TRUE;
                    for (int i = 0; i < count; ++i) {
                        if (position[i] != 0) {
                            printf("位置%d错误!", (position[i] / order + 1) * 10 + i % order + 1);
                            flag = FALSE;
                        }
                    }
                    if (flag == TRUE) printf("答案输入正确！");

                    printf("棋盘答案为：\n");
                    for (int i = 1; i < fileLen + 1; i++) {
                        if (answer[i] == 1)
                            printf("%d %d\n", i, answer[i]);
                        if (answer[i] == -1)
                            printf("%d %d\n", i, answer[i] + 1);
                    }
                    getchar();
                    getchar();
                    break;
                }
                if (choice == 2) {
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
                }

                getchar();
                getchar();
                break;
            case 2:
                //读文件
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
                getchar();
                getchar();
                break;
            case 3:
                if (Traverse(T) == OK)
                    printf("cnf文件输出成功！\n");
                else printf("cnf文件输出失败！\n");
                getchar();
                getchar();
                break;
            case 4:
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
                            printf("%6d", i);
                        } else if (mark_true[i] == -1) {
                            fprintf(fp1, "%d  ", -i);
                            printf("%6d", -i);
                        }
                        if (i % order == 0) printf("\n");
                    }
                    printf("\ncnf的解输出成功！\n");
                    //打印求解之后的棋盘
                    printf("棋盘求解后为：\n");
                    for (int i = 1; i < fileLen + 1; i++) {
                        if (mark_true[i] == 1) {
                            printf(" 1 ");
                        } else if (mark_true[i] == -1) {
                            printf(" 0 ");
                        }
                        if (i % order == 0) printf("\n");
                    }
                    fprintf(fp1, "\n%d\n", t2 - t1);
                    fclose(fp1);
                } else {
                    fp1 = fopen(result, "wb");
                    fprintf(fp1, "%d\n", 0);
                    fprintf(fp1, "%d\n", t2 - t1);
                    fclose(fp1);
                    printf("dpll算法处理失败，cnf文件无解！\n");
                }
                flag = 0;
                getchar();
                getchar();
                break;
        }
    }
}

#endif //SAT_SUDOKU_H
