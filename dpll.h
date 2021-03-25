//
// Created by spiri on 2021/2/22.
//

#ifndef SAT_DPLL_H
#define SAT_DPLL_H

#include "function.h"

/*dpll函数版块*/
status Dpll(headNode *Head, int next_variable, dChart_ptr G) {
    //测试单子句,并测试
    int i=0;
    while (singleClause != 0) {
        next_variable = findVariable(Head);
        if (next_variable==0){
            return FALSE;
        }
        Delete(G, next_variable);
        if (sentence_number == 0) return OK;
        if (emptyClause == TRUE) {
            emptyClause = FALSE;
            return FALSE;
        }
    }
    next_variable = findVariable(Head);
    Delete(G, next_variable);
    if (Dpll(Head, next_variable, G) == OK) return OK;
    else {
        //假设的真值测试失败 换成其相反数假设为真值
        int j=0;
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
