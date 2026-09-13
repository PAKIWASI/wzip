#include "views_single.h"
#include <stdio.h>


int main(void)
{
    String s;
    String_create_stk(&s, "hello");

    String_ensure_null_term(&s);
    printf("%s\n", String_cstr_view(&s));

    StrView sv = StrView_from_String(&s);
    StrView_print(sv);

    return 0;
}
