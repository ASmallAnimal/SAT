//
// Created by spiri on 2021/2/22.
//

#ifndef SAT_DPLL_H
#define SAT_DPLL_H

#include "function.h"

/*dpll函数版块*/
status Dpll(headNode *Head, int next_variable, dChart_ptr G) {
    //测试单子句,并测试
    //如果单子句出错 回溯所有单子句返回上一层假设
    int i = 0;
    while (singleClause != 0) {
        int singleTrue[500];
        next_variable = findVariable(Head);
        singleTrue[i++] = next_variable; //储存当前循环中的单子句
        if (next_variable == 0) {
            printf("出现错误！\n");
            while (i >= 0) {
                Rewind(G, singleTrue[i]);
                if (check == 1) {
                    showCnf(Head);
                    printf("回溯单子句！%d", singleTrue[i]);
                    getchar();
                    getchar();
                }
                mark_true[abs(singleTrue[i])] = 0;
                i--;
            }
            return FALSE;
        }
        Delete(G, next_variable);
        if (check == 1) {
            showCnf(Head);
            printf("删除单子句！%d", singleTrue[i - 1]);
            getchar();
            getchar();
        }
        if (sentence_number == 0) return OK;
        if (emptyClause == TRUE) {
            while (i >= 0) {
                Rewind(G, singleTrue[i]);
                if (check == 1) {
                    showCnf(Head);
                    printf("回溯单子句！%d", singleTrue[i]);
                    getchar();
                    getchar();
                }
                if (singleTrue[i] == -14)
                    getchar();
                mark_true[abs(singleTrue[i])] = 0;
                i--;
            }
            emptyClause = FALSE;
            return FALSE;
        }
    }
    next_variable = findVariable(Head);
    Delete(G, next_variable);
    if (check == 1) {
        showCnf(Head);
        printf("删除文字！%d", next_variable);
        getchar();
        getchar();
    }
    if (Dpll(Head, next_variable, G) == OK) return OK;
    else {
        //假设的真值测试失败 换成其相反数假设为真值
        Rewind(G, next_variable);
        if (check == 1) {
            showCnf(Head);
            printf("回溯文字！%d", next_variable);
            getchar();
            getchar();
        }
        Delete(G, -next_variable);
        if (check == 1) {
            showCnf(Head);
            printf("删除文字！%d", -next_variable);
            getchar();
            getchar();
        }
        if (Dpll(Head, -next_variable, G) == OK) {
            mark_true[next_variable] = -1;
            return OK;
        } else {
            //返回上一层假设
            Rewind(G, -next_variable);
            if (check == 1) {
                showCnf(Head);
                printf("回溯文字！%d", next_variable);
                getchar();
                getchar();
            }
            mark_true[abs(next_variable)] = 0;
            return FALSE;
        }
    }
}

#endif //SAT_DPLL_H
