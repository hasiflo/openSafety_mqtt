/*[[[cog
from tests.core.gentestc import init
cg = init()
]]]*/
/*[[[end]]]*/

/*[[[cog
cg.generate_includes()
]]]*/
#include <stdio.h>
/*[[[end]]]*/

/*[[[cog
cg.generate_defines_section()
cg.generate_defines()
]]]*/
/*******************************************************************************
 **    "Constants / Defines"
 ******************************************************************************/
#define OUTSTR "Hello World!"
/*[[[end]]]*/

int main (void)
{
    /*[[[cog
    cg.generate_printf()
    ]]]*/
    printf("%s", OUTSTR);
    /*[[[end]]]*/

    return 0;
}
