/*[[[cog
from tests.core.gentestc import init
cg = init()
]]]*/
/*[[[end]]]*/

/*[[[cog
cg.generate_includes()
]]]*/
/*[[[end]]]*/

/*[[[cog
cg.generate_defines_section()
cg.generate_defines()
]]]*/
/*[[[end]]]*/

int main (void)
{
    /*[[[cog
    cg.generate_printf()
    ]]]*/
    /*[[[end]]]*/

    return 0;
}
