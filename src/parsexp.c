/**
 * Interpretr imperativního jazyka IFJ12
 *
 * @author Biberle Zdeněk <xbiber00@stud.fit.vutbr.cz>
 * @author Doležal Jan    <xdolez52@stud.fit.vutbr.cz>
 * @author Fryč Martin    <xfrycm01@stud.fit.vutbr.cz>
 * @author Kalina Jan     <xkalin03@stud.fit.vutbr.cz>
 * @author Tretter Zdeněk <xtrett00@stud.fit.vutbr.cz>
 */

#include <assert.h>
#include "parsexp.h"
#include "syntax.h"
#include "alloc.h"

/** Inicializace noveho zasobniku pro precedencni analyzu s EOF na vrcholu */
ExpStack newExpStack(){
    ExpItem* array = newArray( ExpItem, 8 );
    array[0] = (ExpItem){
        .type = term,
        .val.term = (Token){
            .type=tokEndOfFile
        }
    };
    return (ExpStack){
        .array = array,
        .count = 1,
        .allocated = 8,
        .termIndex = 0
    };
}

/** Uvolni zasobnik (Expression na ktere ukazuje zustanou nedotceny) */
void freeExpStack(ExpStack* stack){
    free(stack->array);
}

/** Prida polozku do vrcholu zasobniku */
void addToExpStack(ExpStack* stack, ExpItem item){
    if(stack->count==stack->allocated){
        stack->allocated += 8;
        stack->array = resizeArray( stack->array, ExpItem, stack->allocated );
    }
    stack->array[stack->count] = item;
    if(item.type==term){
        stack->termIndex = stack->count;
    }
    stack->count++;
}

/** Prida polozku do zasobniku za nejvrchnejsi terminal */
void addToAfterTermExpStack(ExpStack* stack, ExpItem item){
    if(stack->count==stack->allocated){
        stack->allocated += 8;
        stack->array = resizeArray( stack->array, ExpItem, stack->allocated );
    }
    for( int i = stack->count ; i > (stack->termIndex+1) ; i-- ){
        stack->array[i] = stack->array[i-1];
    }
    stack->array[stack->termIndex+1] = item;
    if(item.type==term){
        stack->termIndex = stack->termIndex+1;
    }
    stack->count++;
}

/** Odstrani ze zasobniku \a i polozek (EOF se odstranit nesmi!) */
void removeFromExpStack(ExpStack* stack, int i){
    assert( i >= 1 && i <= stack->count );
    stack->count -= i;
    
    for( i=stack->count-1; stack->array[i].type!=term; i-- );
    stack->termIndex = i;
}

/** Vrati \a i-ty prvek ze zasobniku (pocitano od vrcholu, 1=vrchol) */
ExpItem* itemFromStack(ExpStack* stack, int i){
    assert( i >= 1 && i <= stack->count );
    return &(stack->array[ stack->count - i ]);
}

/** Vrati ze zasobniku operator nejblizsi vrcholu */
Token operatorFromExpStack(ExpStack* stack){
    return stack->array[ stack->termIndex ].val.term;
}

/** Hlavicky precedencni tabulky */
typedef enum {
    open,  // <
    close, // >
    equal, // =
    blank, // -
} tableRel;

typedef enum {
    oPlus=0, oMinus=1,
    oMultiple=2, oDivide=3,
    oPower=4,
    oLParen=5, oRParen=6,
    oLT=7, oGT=8, oLE=9, oGE=10, oNE=11, oEQ=12,
    oAND=13, oOR=14, oNOT=15,
    oId=16, oEOF=17
} tableOp;

/** Precedencni tabulka */ // TODO: dodelat tuto tabulku pro porovnavaci a logicke operatory!
tableRel precTable[18][18] = {
             /*  +      -      *      /     **      (      )      <      >     <=     >=     !=     ==     and    or     not    id     EOF */
 /*    +  */ { close, close, open,  open,  open,  open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*    -  */ { close, close, open,  open,  open,  open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*    *  */ { close, close, close, close, open,  open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*    /  */ { close, close, close, close, open,  open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*   **  */ { close, close, close, close, close, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*    (  */ { open,  open,  open,  open,  open,  open,  equal, open,  open,  open,  open,  open,  open,  open,  open,  open,  open,  blank },
 /*    )  */ { close, close, close, close, close, blank, close, close, close, close, close, close, close, close, close, close, blank, close },
 /*    <  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*    >  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*   <=  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*   >=  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*   !=  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*   ==  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*  and  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*   or  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*  not  */ { blank, blank, blank, blank, blank, open,  close, blank, blank, blank, blank, blank, blank, blank, blank, blank, open,  close },
 /*   id  */ { close, close, close, close, close, blank, close, close, close, close, close, close, close, close, close, close, blank, close },
 /*  EOF  */ { open,  open,  open,  open,  open,  open,  blank, open,  open,  open,  open,  open,  open,  open,  open,  open,  open,  blank },
};

tableOp token2tableOp(Token t){
    switch(t.type){
        case tokEndOfFile: return oEOF; break;
        case tokEndOfLine: return oEOF; break;
        case tokId: return oId; break;
        case tokLiteral: return oId; break;
        case tokLParen: return oLParen; break;
        case tokRParen: return oRParen; break;
        case tokOp:
            switch(t.data.op){
                case opPlus: return oPlus;
                case opMinus: return oMinus;
                case opMultiple: return oMultiple;
                case opDivide: return oDivide;
                case opPower: return oPower;
                case opLT: return oLT;
                case opGT: return oGT;
                case opLE: return oLE;
                case opGE: return oGE;
                case opNE: return oNE;
                case opEQ: return oEQ;
                case opAND: return oAND;
                case opOR: return oOR;
                case opNOT: return oNOT;
            }
        break;
        default:
            throw(SyntaxError,((SyntaxErrorException){.type=BadTokenInExpression, .line_num=t.line_num}));
    }
    return oEOF; // jen pro kompilator nechapajici vyjimky
}

/** Zkusi nahradit konec zasobniku podle pravidel gramatiky */
bool tryUseRules( ExpStack* stack, SyntaxContext* ctx ){
    ExpItem a,b,c;
    Expression E;
    
    // E -> id
    if(
      (itemFromStack(stack,1)->type==term && itemFromStack(stack,1)->val.term.type==tokId) &&
      (itemFromStack(stack,2)->type==bracket)
    ){
        E.type=VARIABLE;
        E.value.variable = getSymbol(itemFromStack(stack,1)->val.term.data.id,ctx->globalSymbols,ctx->localSymbols);
        removeFromExpStack(stack,2);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        printf("E->id\n"); // DEBUG
        return true;
    }
    
    // E -> literal
    if(
      (itemFromStack(stack,1)->type==term && itemFromStack(stack,1)->val.term.type==tokLiteral) &&
      (itemFromStack(stack,2)->type==bracket)
    ){
        E.type=CONSTANT;
        Value val = itemFromStack(stack,1)->val.term.data.val;
        E.value.constant=copyValue(&val);
        removeFromExpStack(stack,2);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        printf("E->lit\n"); // DEBUG
        return true;
    }
    
    // E -> (E)
    if(
      (itemFromStack(stack,1)->type==term && itemFromStack(stack,1)->val.term.type==tokRParen) &&
      (itemFromStack(stack,2)->type==exp) &&
      (itemFromStack(stack,3)->type==term && itemFromStack(stack,3)->val.term.type==tokLParen) &&
      (itemFromStack(stack,4)->type==bracket)
    ){
        E = itemFromStack(stack,2)->val.exp;
        removeFromExpStack(stack,4);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        printf("E->(E)\n"); // DEBUG
        return true;
    }
    
    // E -> E + E (a vsechny ostatni binarni operatory)
    if(
      (itemFromStack(stack,1)->type==exp) &&
      (itemFromStack(stack,2)->type==term && itemFromStack(stack,2)->val.term.type==tokOp && itemFromStack(stack,2)->val.term.data.op!=opNOT) &&
      (itemFromStack(stack,3)->type==exp) &&
      (itemFromStack(stack,4)->type==bracket)
    ){
        E.type = OPERATOR;
        E.value.operator.type = BINARYOP;
        
        switch(itemFromStack(stack,2)->val.term.data.op){
            case opPlus: E.value.operator.value.binary.type = ADD; break;
            case opMinus: E.value.operator.value.binary.type = SUBTRACT; break;
            case opMultiple: E.value.operator.value.binary.type = MULTIPLY; break;
            case opDivide: E.value.operator.value.binary.type = DIVIDE; break;
            case opPower: E.value.operator.value.binary.type = POWER; break;
            case opLT: E.value.operator.value.binary.type = LESS; break;
            case opGT: E.value.operator.value.binary.type = GREATER; break;
            case opLE: E.value.operator.value.binary.type = LEQUAL; break;
            case opGE: E.value.operator.value.binary.type = GEQUAL; break;
            case opNE: E.value.operator.value.binary.type = NOTEQUALS; break;
            case opEQ: E.value.operator.value.binary.type = EQUALS; break;
            case opAND: E.value.operator.value.binary.type = AND; break;
            case opOR: E.value.operator.value.binary.type = OR; break;
            default: assert(false);
        }
        
        Expression* Eleft = new(Expression);
        Expression* Eright = new(Expression);
        *Eleft = itemFromStack(stack,3)->val.exp;
        *Eright = itemFromStack(stack,1)->val.exp;
        E.value.operator.value.binary.left = Eleft;
        E.value.operator.value.binary.right = Eright;
        //E.value.operator.value.binary.left = &(itemFromStack(stack,3)->val.exp);
        //E.value.operator.value.binary.right = &(itemFromStack(stack,1)->val.exp);
        removeFromExpStack(stack,4);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        printf("E->ExE\n"); // DEBUG
        return true;
    }
    
    printf("zadne pouzitelne pravidlo!\n"); // DEBUG
    return false;
}

/** Funkce zahajujici samotnou precedencni analyzu */
void parseExpression( Scanner* s, Expression* expr, SyntaxContext* ctx ){
    ExpStack stack = newExpStack();
    Token a,b;
    
    while(true){
        a = operatorFromExpStack(&stack); // nejvrchnejsi terminal
        b = getTok(s); // terminal na vstupu
        
        if(a.type==tokEndOfFile && (b.type==tokEndOfFile || b.type==tokEndOfLine)){
            break;
        }
        
        switch(precTable[token2tableOp(a)][token2tableOp(b)]){
            case equal: // =
                printf("=\n");
                addToExpStack(&stack, (ExpItem){.type=term, .val.term=b}); // push(b)
                consumeTok(s); // precti dalsi symbol ze vstupu
            break;
            case open: // <
                printf("<\n");
                addToAfterTermExpStack(&stack, (ExpItem){.type=bracket}); // vlozit < za a
                addToExpStack(&stack, (ExpItem){.type=term, .val.term=b}); // push(b)
                consumeTok(s); // precti dalsi symbol ze vstupu
            break;
            case close: // >
                printf(">\n");
                if(!tryUseRules(&stack,ctx)){
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=b.line_num}));
                }
            break;
            case blank: // prazdne policko: chyba
                printf("prazdne pole v tabulce! [%d,%d]\n",token2tableOp(a),token2tableOp(b)); // DEBUG
                throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=b.line_num}));
            break;
        }
    }
    
    ExpItem* ret = itemFromStack(&stack,1);
    assert(ret->type==exp);
    *expr=ret->val.exp;
    freeExpStack(&stack);

}

