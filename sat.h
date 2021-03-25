//
// Created by spiri on 2021/2/25.
//

#ifndef SAT_SAT_H
#define SAT_SAT_H

#include "function.h"
#include "dpll.h"

void SAT() {
    FILE *fp = NULL;
    isGame = FALSE;
    headNode_ptr T;
    headNode Arr;
    T = &Arr;

    dChart arr;
    dChart_ptr Chart;
    Chart = &arr;

    int times1 = 0;
    int times2 = 0;
    int time[2];
    int singleTrue[500];
    int op = 1;
    int t1, t2, flag = FALSE;
    int isRead;
    char ch;
    char filename[100], result[100];
    while (op) {
        system("cls");
        printf("******\t\tSAT\t\t******\n");
        printf("\t1. 读取cnf文件\t2. 输出cnf文件\n");
        printf("\t3. 保存cnf文件\t4. dpll求解\t\n");
        printf("\t\t0. 返回主菜单\n");
        printf("请输入对应功能前的数字：");
        scanf("%d", &op);
        setbuf(stdin, NULL);
        if (op >= 0 && op <= 5 && !fmod(op, 1)) {
            switch (op) {
                case 1:
                    printf("请输入cnf文件路径：\n");
                    scanf("%s", filename);
                    if (times1 >= 2 || times2 >= 2) {
                        Realse_Link(&T);  //读取新的cnf文件时要正确释放内存
                        isRead = 0;
                        times1 = 0;
                        times2 = 0;
                    }
                    if (isRead == 1 && times1 == 1 && times2 == 1) {
                        Realse_Link(&T);  //读取新的cnf文件时要正确释放内存
                        isRead = 0;
                        times1 = 0;
                        times2 = 0;
                    }
                    fp = fopen(filename, "r");
                    if (fp == NULL) {
                        printf("文件打开失败！\n ");
                    } else {
                        printf("\ncnf文件中的注释内容为：\n");
                        while (fread(&ch, sizeof(char), 1, fp)) {
                            if (ch != 'p') printf("%c", ch);
                            else break;
                        }
                    }
                    if (readCnf(&T, fp, Chart) == OK) {
                        isRead = 1;
                        printf("\tcnf文件成功读取！\n");
                    } else printf("\tcnf文件读取失败！\n");
                    getchar();
                    getchar();
                    break;
                case 2:
                    if (isRead == 1 && Traverse(T) == OK)
                        printf("cnf文件输出成功！\n");
                    else printf("cnf文件输出失败！\n");
                    getchar();
                    getchar();
                    break;
                case 3:
                    if (isRead == 1 && Save_File(T) == OK)
                        printf("另存为cnf文件成功！\n");
                    else printf("另存为为cnf文件失败！\n");
                    getchar();
                    getchar();
                    break;
                case 4:
                    if (isRead == 1) {
                        for (int i = 1; i < variable_number + 1; i++)
                            mark_true[i] = 0;
                        for (int i = 0; i <500 ; ++i) {
                            singleTrue[i]=0;
                        }
                        printf("请选择你需要的策略：\n");
                        printf("1：按照文字出现频次猜测   2:文字逐一猜测!\n");
                        scanf("%d", &Flag);
                        printf("你想检查吗？");
                        scanf("%d",&check);
                        if (Flag == 1)//选择策略
                        {
                            strategy = 1;
                            //排序
                            mark_times[N]={0};
                            mark_times[10000]={0};
                            Mark(T, mark_times, mark_number);
                            t1 = clock();
                            flag = Dpll(T, 1, Chart);
                            t2 = clock();
                            time[0] = t2 - t1;
                            printf("时间：%4d ms\n", time[0]);
                            times1++;
                        } else if (Flag == 2) {
                            strategy = 2;
                            t1 = clock();
                            flag = Dpll(T, 1, Chart);
                            t2 = clock();
                            time[1] = t2 - t1;
                            printf("时间：%4d ms\n\n", time[1]);
                            times2++;
                        }
                        //保存结果部分
                        //保存路径
                        strcpy(result, filename);
                        for (int i = 0; i < 70; i++) {
                            if (result[i] == '.') {
                                result[i + 1] = 'r';
                                result[i + 2] = 'e';
                                result[i + 3] = 's';
                                break;
                            }
                        }
                        FILE *new_fp = fopen(result, "wb");
                        //保存结果配置文件
                        if (flag == OK) {
                            fprintf(new_fp, "s %d\n", 1);
                            fprintf(new_fp, "v ");
                            printf("cnf文件的解为：\n");
                            for (int i = 1; i < variable_number + 1; i++) {
                                if (mark_true[i] == 1) {
                                    fprintf(new_fp, "%d  ", i);
                                    printf("%6d", i);
                                } else if (mark_true[i] == -1) {
                                    fprintf(new_fp, "%d  ", -i);
                                    printf("%6d", -i);
                                }
                                if (i % 10 == 0) printf("\n");
                            }
                            fprintf(new_fp, "\nt %d\n", t2 - t1);
                            printf("\ncnf的解输出成功！\n");
                            fclose(new_fp);
                        } else {
                            fprintf(new_fp, "%d\n", 0);
                            fprintf(new_fp, "%d\n", t2 - t1);
                            fclose(new_fp);
                            printf("dpll算法处理失败，cnf文件无解！\n");
                        }
                        flag = FALSE;
                        printf("请在重复上述操作并选择另一策略以计算优化率！\n");
                        if (times1 == 1 && times2 == 1) {
                            printf("需要计算优化率吗？1--是，2--否\n");
                            int yesOrNo = 0;
                            scanf("%d", &yesOrNo);
                            isRead = 0;
                            if (yesOrNo == 1) {
                                if (strategy == 1) {
                                    printf("本策略错使用多个时间为%dms\n", time[0]);
                                    printf("另一策略所花时间为：%dms\n", time[1]);
                                } else {
                                    printf("本策略所用的时间为:%dms\n", time[1]);
                                    printf("另一策略所花时间为：%dms\n", time[0]);
                                }
                                printf("优化率为：%.2f百分号", 100.0 * (time[1] - time[0]) / time[0]);
                            }
                        }
                    } else printf("操作失败！\n");
                    getchar();
                    getchar();
                    break;
                case 0:
                    getchar();
                    break;
            }
        } else {
            printf("输入错误！\n");
            getchar();
            getchar();
            getchar();
        }
    }//end of while
}

#endif //SAT_SAT_H
