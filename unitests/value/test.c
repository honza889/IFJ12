#include <stdio.h>
#include "value.h"

int main(){
 
 char *s;
 Value v = {typeUndefined};
 
 setValueNil(&v);
 printf("%s:%d: %s\n",__FILE__,__LINE__,s=getValueString(&v));free(s);
 printf("%s:%d: %s\n",__FILE__,__LINE__,(getValueBoolean(&v)?"true":"false"));
 
 setValueBoolean(&v,true);
 printf("%s:%d: %s\n",__FILE__,__LINE__,s=getValueString(&v));free(s);
 printf("%s:%d: %s\n",__FILE__,__LINE__,(getValueBoolean(&v)?"true":"false"));

 setValueBoolean(&v,false);
 printf("%s:%d: %s\n",__FILE__,__LINE__,s=getValueString(&v));free(s);
 printf("%s:%d: %s\n",__FILE__,__LINE__,(getValueBoolean(&v)?"true":"false"));

 setValueNumeric(&v,123456.789012);
 printf("%s:%d: %s\n",__FILE__,__LINE__,s=getValueString(&v));free(s);
 printf("%s:%d: %s\n",__FILE__,__LINE__,(getValueBoolean(&v)?"true":"false"));
 
 setValueString(&v,"Toto je řetězec znaků");
 printf("%s:%d: %s\n",__FILE__,__LINE__,s=getValueString(&v));free(s);
 printf("%s:%d: %s\n",__FILE__,__LINE__,(getValueBoolean(&v)?"true":"false"));

 freeValue(&v);
 
 printf("Hotovo\n");
}
