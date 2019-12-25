#include "genMips_opt.h"
#include "tmpCode.h"

int main() {
    lexer();
    now = vec.begin();
    parser();
    if (!isError) {
        original_tmpCode();
        std_tmpCode();
        std_tmpCode2();
        genMips_opt();
    }
    return 0;
}