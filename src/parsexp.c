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

typedef struct{
    enum {
        exp,     // E
        bracket, // <
        term     // $ + - id...
    } type;
    union {
        Expression exp;
        Token term;
    } val;
} ExpItem;

typedef struct{
    ExpItem* array; // ukazatel na pole polozek zasobniku (zacatek, nikoli vrchol!)
    int count; // pocet polozek na zasobniku
    int allocated; // alokovana delka pole polozek (>=count)
    int termIndex; // index nejvrcholovejsiho terminalu (primy index do array)
} ExpStack;

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
    // zvetseni zasobniku, neni-li dostatecne velky
    if(stack->count==stack->allocated){
        stack->allocated += 8;
        stack->array = resizeArray( stack->array, ExpItem, stack->allocated );
    }
    // pridani polozky
    stack->array[stack->count] = item;
    if(item.type==term){
        stack->termIndex = stack->count; // nyni je nevrcholovejsim terminalem
        
        // kopirovani hodnot pro potreby pozdejsiho uvolnovani pameti
        if( item.val.term.type == tokId )
        {
            stack->array[stack->count].val.term.data.id = copyRCString( &item.val.term.data.id );
        }
        if( item.val.term.type == tokLiteral )
        {
            stack->array[stack->count].val.term.data.val = copyValue( &item.val.term.data.val );
        }
    }
    stack->count++; // zvyseni poctu polozek
}

/** Prida polozku do zasobniku za nejvrchnejsi terminal (jen pro zavorky, bez uvolnovani!) */
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
    stack->count++; // zvyseni poctu polozek
}

/** Odstrani ze zasobniku \a i polozek (EOF se odstranit nesmi!) */
void removeFromExpStack(ExpStack* stack, int i){
    assert( i >= 1 && i <= stack->count );
    
    // uvolneni odstranovanych prvku
    for( int ii = stack->count - i - 1 ; ii < stack->count ; ii++ )
    {
        if(stack->array[ii].type==term)
        {
            if( stack->array[ii].val.term.type == tokId )
            {
                deleteRCString( &stack->array[ii].val.term.data.id );
            }
            if( stack->array[ii].val.term.type == tokLiteral )
            {
                freeValue( &stack->array[ii].val.term.data.val );
            }
        }
    }
    // snizeni poctu prvku
    stack->count -= i;
    
    // nalezeni noveho nejvrcholovejsiho terminalu
    for( i=stack->count-1; stack->array[i].type!=term; i-- );
    stack->termIndex = i;
}

/** Vrati \a i-ty prvek ze zasobniku (pocitano od vrcholu, 1=vrchol) */
static inline ExpItem* itemFromStack(ExpStack* stack, int i){
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
    func,  // volani funkce
    minus  // zacatek unarniho nebo binarniho minus (zatim nevime)
} tableRel;

typedef enum {
    oPlus=0, oMinus=1,
    oMultiple=2, oDivide=3,
    oPower=4,
    oLParen=5, oRParen=6,
    oLT=7, oGT=8, oLE=9, oGE=10, oNE=11, oEQ=12,
    oIn=13, oNotIn=14,
    oAND=15, oOR=16, oNOT=17,
    oId=18, oEOF=19,
    oUminus=20
} tableOp;

/** Precedencni tabulka */ // (majitele eee PC mne budou nenavidet)
tableRel precTable[21][20] = {
             /*  +      -      *      /     **      (      )      <      >     <=     >=     !=     ==     in    notin   and    or     not    id     EOF */
 /*    +  */ { close, minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*    -  */ { close, minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*    *  */ { close, minus, close, close, open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*    /  */ { close, minus, close, close, open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*   **  */ { close, minus, close, close, close, open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },

 /*    (  */ { open,  minus, open,  open,  open,  open,  equal, open,  open,  open,  open,  open,  open,  open,  open,  open,  open,  open,  open,  blank },
 /*    )  */ { close, minus, close, close, close, blank, close, close, close, close, close, close, close, close, close, close, close, close, blank, close },

 /*    <  */ { open,  minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*    >  */ { open,  minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*   <=  */ { open,  minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*   >=  */ { open,  minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*   !=  */ { open,  minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*   ==  */ { open,  minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /*   in  */ { open,  minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
 /* notin */ { open,  minus, open,  open,  open,  open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },

 /*  and  */ { open,  minus, open,  open,  open,  open,  close, open,  open,  open,  open,  open,  open,  open,  open,  close, close, open,  open,  close },
 /*   or  */ { open,  minus, open,  open,  open,  open,  close, open,  open,  open,  open,  open,  open,  open,  open,  close, close, open,  open,  close },
 /*  not  */ { open,  minus, open,  open,  open,  open,  close, open,  open,  open,  open,  open,  open,  open,  open,  close, close, open,  open,  close },

 /*   id  */ { close, close, close, close, close, func,  close, close, close, close, close, close, close, close, close, close, close, close, blank, close },
 /*  EOF  */ { open,  minus, open,  open,  open,  open,  blank, open,  open,  open,  open,  open,  open,  open,  open,  open,  open,  open,  open,  blank },

 /*  u -  */ { close, minus, close, close, close, open,  close, close, close, close, close, close, close, close, close, close, close, close, open,  close },
};

tableOp token2tableOp(Token t){
    switch(t.type){
        case tokEndOfFile: return oEOF; break;
        case tokEndOfLine: return oEOF; break;
        case tokComma: return oEOF; break;
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
                case opIn: return oIn;
                case opNotIn: return oNotIn;
                case opAND: return oAND;
                case opOR: return oOR;
                case opNOT: return oNOT;
                case opUMinus: return oUminus;
            }
        break;
        default:
            //printf("nelze pretypovat token typu %x\n",t.type);
            throw(SyntaxError,((SyntaxErrorException){.type=BadTokenInExpression, .line_num=t.line_num}));
    }
    return oEOF; // jen pro kompilator nechapajici vyjimky
}

/** Zkusi nahradit konec zasobniku podle pravidel gramatiky */
bool tryUseRules( ExpStack* stack, SyntaxContext* ctx ){
    Expression E;
    
    // E -> id
    if(
      (itemFromStack(stack,1)->type==term && itemFromStack(stack,1)->val.term.type==tokId) &&
      (itemFromStack(stack,2)->type==bracket)
    ){
        //printf("E->id\n"); // DEBUG
        E.type=VARIABLE;
        E.value.variable = getSymbol(itemFromStack(stack,1)->val.term.data.id,ctx->globalSymbols,ctx->localSymbols);
        
        removeFromExpStack(stack,2);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        return true;
    }
    
    // E -> literal
    if(
      (itemFromStack(stack,1)->type==term && itemFromStack(stack,1)->val.term.type==tokLiteral) &&
      (itemFromStack(stack,2)->type==bracket)
    ){
        //printf("E->lit\n"); // DEBUG
        E.type=CONSTANT;
        Value val = itemFromStack(stack,1)->val.term.data.val;
        E.value.constant=copyValue(&val);
        
        removeFromExpStack(stack,2);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        return true;
    }
    
    // E -> E
    if(
      (itemFromStack(stack,1)->type==exp) &&
      (itemFromStack(stack,2)->type==bracket)
    ){
        //printf("E->E\n"); // DEBUG
        E = itemFromStack(stack,1)->val.exp;
        removeFromExpStack(stack,2);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        return true;
    }
    
    // E -> E + E (a vsechny ostatni binarni operatory)
    if(
      (itemFromStack(stack,1)->type==exp) &&
      (itemFromStack(stack,2)->type==term && itemFromStack(stack,2)->val.term.type==tokOp && itemFromStack(stack,2)->val.term.data.op!=opNOT) &&
      (itemFromStack(stack,3)->type==exp) &&
      (itemFromStack(stack,4)->type==bracket)
    ){
        //printf("E->ExE\n"); // DEBUG
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
            case opIn: E.value.operator.value.binary.type = IN; break;
            case opNotIn: E.value.operator.value.binary.type = NOTIN; break;
            case opAND: E.value.operator.value.binary.type = AND; break;
            case opOR: E.value.operator.value.binary.type = OR; break;
            default: assert(false);
        }
        // operandy do nove alokovane pameti
        Expression* Eleft = new(Expression);
        Expression* Eright = new(Expression);
        *Eleft = itemFromStack(stack,3)->val.exp;
        *Eright = itemFromStack(stack,1)->val.exp;
        E.value.operator.value.binary.left = Eleft;
        E.value.operator.value.binary.right = Eright;
        
        removeFromExpStack(stack,4);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        return true;
    }
    
    // E -> -E (a ostatni unarni operatory)
    if(
      (itemFromStack(stack,1)->type==exp) &&
      (itemFromStack(stack,2)->type==term && itemFromStack(stack,2)->val.term.type==tokOp &&
        (itemFromStack(stack,2)->val.term.data.op==opUMinus || itemFromStack(stack,2)->val.term.data.op==opNOT)) &&
      (itemFromStack(stack,3)->type==bracket)
    ){
        //printf("E->-E\n"); // DEBUG
        E.type = OPERATOR;
        E.value.operator.type = UNARYOP;        
        switch(itemFromStack(stack,2)->val.term.data.op){
            case opUMinus: E.value.operator.value.unary.type = MINUS; break;
            case opNOT: E.value.operator.value.unary.type = NOT; break;
            default: assert(false);
        }
        // operand do nove alokovane pameti
        Expression* Eoperand = new(Expression);
        *Eoperand = itemFromStack(stack,1)->val.exp;
        E.value.operator.value.unary.operand = Eoperand;
        
        removeFromExpStack(stack,3);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        return true;
    }
    
    // E -> (E)
    if(
      (itemFromStack(stack,1)->type==term && itemFromStack(stack,1)->val.term.type==tokRParen) &&
      (itemFromStack(stack,2)->type==exp) &&
      (itemFromStack(stack,3)->type==term && itemFromStack(stack,3)->val.term.type==tokLParen) &&
      (itemFromStack(stack,4)->type==bracket)
    ){
        //printf("E->(E)\n"); // DEBUG
        E = itemFromStack(stack,2)->val.exp;
        
        removeFromExpStack(stack,4);
        addToExpStack(stack, (ExpItem){ .type=exp, .val.exp=E } );
        return true;
    }
    
    //printf("zadne pouzitelne pravidlo!\n"); // DEBUG
    return false; // Nastala syntakticka chyba
}

/**
 * Ze vstupu bylo precteno id( - nacte parametry a nahradi retezec
 * tokenu volani funkce vyrazem Expression, ktery vrati na zasobnik
 */
bool replaceByFunctionCall( ExpStack* stack, Scanner* s, SyntaxContext* ctx ){

    Expression E, subExp;
    E.type = FUNCTION_CALL;
    E.value.functionCall.function = getFunctionId( ctx, &itemFromStack(stack,1)->val.term.data.id );
    E.value.functionCall.params = (ExpressionList){NULL,0};
    consumeTok(s); // zkonzumovat '('
    while(getTok(s).type!=tokRParen){
        parseExpression(s,&subExp,ctx);
        addExpressionToExpressionList(&E.value.functionCall.params,&subExp);
        // musi nasledovat ',' nebo ')'
        testTok( s, tokComma | tokRParen );
		  if( getTok( s ).type == tokComma ){
            consumeTok(s); // konzumace ','
        }
    }
    consumeTok(s); // konzumace ')'
    ExpItem ei = {
        .type = exp,
        .val.exp = E
    };
    removeFromExpStack( stack, 1 );
    addToExpStack( stack, ei );

    return true;
}

/** Funkce zahajujici samotnou precedencni analyzu */
void parseExpression( Scanner* s, Expression* expr, SyntaxContext* ctx ){
    ExpStack stack = newExpStack();
    Token a,b;
    while(true){
        a = operatorFromExpStack(&stack); // nejvrchnejsi terminal
        b = getTok(s); // terminal na vstupu
        
        if((b.type==tokEndOfFile || b.type==tokEndOfLine || b.type==tokComma || b.type==tokRParen) && a.type==tokEndOfFile){
            break;
        }
        
        switch(precTable[token2tableOp(a)][token2tableOp(b)]){
            case equal: // =
                //printf("=\n");
                addToExpStack(&stack, (ExpItem){.type=term, .val.term=b}); // push(b)
                consumeTok(s); // precti dalsi symbol ze vstupu
            break;
            case open: // <
                //printf("<\n");
                addToAfterTermExpStack(&stack, (ExpItem){.type=bracket}); // vlozit < za a
                addToExpStack(&stack, (ExpItem){.type=term, .val.term=b}); // push(b)
                consumeTok(s); // precti dalsi symbol ze vstupu
            break;
            case close: // >
                //printf(">\n");
                if(!tryUseRules(&stack,ctx)){
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=b.line_num}));
                }
            break;
            case blank: // prazdne policko: chyba
                //printf("prazdne pole v tabulce! [%d,%d]\n",token2tableOp(a),token2tableOp(b)); // DEBUG
                throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=b.line_num}));
            break;
            case func: // volani funkce (precteno id( )
                //printf("func\n");
                if(!replaceByFunctionCall(&stack,s,ctx)){
                    throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=b.line_num}));
                }
            break;
            case minus: // unarni minus (<) nebo binarni minus (>)
                //printf("minus\n");
                // pokud minus nasleduje po $ nebo po operatoru, je unarni
                if(
                  (itemFromStack(&stack,1)->type==term && itemFromStack(&stack,1)->val.term.type==tokEndOfFile) ||
                  (itemFromStack(&stack,1)->type==term && itemFromStack(&stack,1)->val.term.type==tokLParen) ||
                  (itemFromStack(&stack,1)->type==term && itemFromStack(&stack,1)->val.term.type==tokOp)
                ){ // unarni
                    //printf("U\n");
                    addToExpStack(&stack, (ExpItem){.type=bracket}); // push(bracket)
                    addToExpStack(&stack, (ExpItem){.type=term, .val.term.type=tokOp, .val.term.data.op=opUMinus}); // push(-)
                    consumeTok(s); // precti dalsi symbol ze vstupu
                }else{ // binarni
                    //printf("B\n");
                    if(a.type==tokEndOfFile){ // po $ - <
                        //printf("-<\n");
                        addToAfterTermExpStack(&stack, (ExpItem){.type=bracket}); // vlozit < za a
                        addToExpStack(&stack, (ExpItem){.type=term, .val.term=b}); // push(b)
                        consumeTok(s); // precti dalsi symbol ze vstupu
                    }else{ // po jinem operatoru - >
                        //printf("->\n");
                        if(!tryUseRules(&stack,ctx)){
                            throw(SyntaxError,((SyntaxErrorException){.type=StrangeSyntax, .line_num=b.line_num}));
                        }
                    }
                }
            break;
        }
    }
    
    ExpItem* ret = itemFromStack(&stack,1);
    assert(ret->type==exp);
    *expr=ret->val.exp;
    freeExpStack(&stack);
}

