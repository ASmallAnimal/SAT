#include "function.h"

/*dpll函数版块*/
status Dpll(headNode *Head, int next_variable, dChart_ptr G) {
    //首先设置递归终止条件 如果为空 真值设置成立 如果有空子句 真值设置失败
    if (sentence_number == 0) return OK;
    else {
        if (emptyClause == TRUE) {
            //出现空子句就恢复空子句并回溯
            emptyClause = FALSE;
            return FALSE;
        } else {
            //找一个合适的真值,并测试
            next_variable = findVariable(Head);
            if (next_variable == 0) return FALSE;
            Delete(G, next_variable);
            //pureWord(G);
            if (Dpll(Head, next_variable, G) == OK) return OK;
            else {
                //假设的真值测试失败 换成其相反数假设为真值
                Rewind(G, next_variable);
                Delete(G, -next_variable);
                if (Dpll(Head, -next_variable, G) == OK) {
                    next_variable > 0 ? mark_true[next_variable] = -1 : mark_true[next_variable] = 1;
                    return OK;
                } else {//相反数也测试失败，则标记该假设，返回上一个假设
                    Rewind(G, -next_variable);
                    mark_true[abs(next_variable)] = 0;  //恢复之后 赋值零 表示尚未测试
                    return FALSE;
                }
            }
        }
    }
}