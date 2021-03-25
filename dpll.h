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
        singleTrue[i++] = abs(next_variable); //储存当前循环中的单子句
        if (next_variable == 0) {
            while (i--) {
                Rewind(G, singleTrue[i - 1]);
                mark_true[singleTrue[i - 1]] = 0;
            }
            return FALSE;
        }
        Delete(G, next_variable);
        if (sentence_number == 0) return OK;
        if (emptyClause == TRUE) {
            while (i--) {
                Rewind(G, singleTrue[i - 1]);
                mark_true[singleTrue[i - 1]] = 0;
            }
            emptyClause = FALSE;
            return FALSE;
        }
    }
    next_variable = findVariable(Head);
    Delete(G, next_variable);
    if (Dpll(Head, next_variable, G) == OK) return OK;
    else {
        //假设的真值测试失败 换成其相反数假设为真值
        Rewind(G, next_variable);
        Delete(G, -next_variable);
        if (Dpll(Head, -next_variable, G) == OK) {
            mark_true[next_variable] = -1;
            return OK;
        } else {
            //返回上一层假设
            Rewind(G, -next_variable);
            mark_true[abs(next_variable)] = 0;
            return FALSE;
        }
    }
}

#endif //SAT_DPLL_H
