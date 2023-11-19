import { match, Times, oneOf, noneOf, or , List} from "../lib/lexer.js";

export const L_SPECIAL = `!"#$%&'()*+,./:;<=>?@[\\]^\`{|}~ \t\n\r`
export const SPECIAL = oneOf(L_SPECIAL);
export const NON_SPECIAL = noneOf(L_SPECIAL);
export const NON_SPECIAL_F = noneOf(L_SPECIAL+"-");
export const NON_SPECIAL_M = noneOf(L_SPECIAL+"_-");
export const BIG_LETTER = oneOf('A-Z');
export const UNIT_CHARS = noneOf(`!"#$%&'()*+,./:;<=>?@[\\]^\`{|}~ \t\n\r0-9`)
export const UNIT_START = noneOf(`!"#$%&'()*+,./:;<=>?@[\\]^\`{|}~ \t\n\r0-9_-`)
export const HEX_DIGIT = oneOf("0-9a-fA-F");

export const BYTE = 
    match( 
        '0',
        oneOf('bB'), 
        oneOf('01'),
        Times.Opt(
            match(
                Times.NoneOrMore(oneOf("01_'")),
                oneOf("01")
            )
        )
    )
export const OCTAL =   
    match( 
        '0',
        oneOf('oO'), 
        oneOf('0-7'),
        Times.Opt(
            match(
                Times.NoneOrMore(oneOf("0-7'_")),
                oneOf("0-7")
            )
        )
    )
export const DECIMAL = 
    match( 
        Times.Opt(
            match(
                '0',
                oneOf("dD")
            )
        ),
        oneOf('0-9'),
        Times.Opt(
            match(
                Times.NoneOrMore(oneOf("0-9'_")),
                oneOf("0-9")
            )
        )
    )
export const HEX = 
    match( 
        '0',
        oneOf('xX'),
        oneOf('0-9a-fA-F'),
        Times.Opt(
            match(
                Times.NoneOrMore(oneOf("0-9a-fA-F_'")),
                oneOf("0-9a-fA-F")
            )
        )
    )
export const FULL = 
    match( 
        '0',
        oneOf('fF'),
        oneOf('0-9a-zA-Z'),
        Times.Opt(
            match(
                Times.NoneOrMore(oneOf("0-9a-fA-Z_'")),
                oneOf("0-9a-zA-Z")
            )
        )
    )  

export const INT_NUMBER = 
    or(
        BYTE,
        HEX,
        FULL,
        OCTAL,
        DECIMAL
    ) 

export const FLOAT_CONTENT =
    match(
        Times.Opt(
            match(
                oneOf('0-9'),
                Times.Opt(
                    match(
                        Times.NoneOrMore(
                            oneOf("0-9_'")
                        ),
                        oneOf("0-9")
                    )
                )
            )
        ),
        ".",
        Times.OnceOrMore(
            match(
                oneOf('0-9'),
                Times.Opt(
                    match(
                        Times.NoneOrMore(
                            oneOf("0-9_'")
                        ),
                        oneOf("0-9")
                    )
                )
            )
        )
    )
export const FLOAT =
    match(
        Times.Opt(
            oneOf('fF')
        ),
        FLOAT_CONTENT
    )
export const DOUBLE =
    match(
        Times.Opt(
            oneOf('dD')
        ),
        FLOAT_CONTENT
    )

export const FLOAT_NUMBER = 
    or(
        FLOAT,
        DOUBLE
    )

export const SIGN = oneOf("+-");

export const NUMBER =
    match(
        Times.Opt(
            SIGN
        ),
        or(
            INT_NUMBER,
            FLOAT_NUMBER
        )
    )

export const UNIT_SUB =
    match(
        UNIT_START,
        Times.NoneOrMore(UNIT_CHARS),
        Times.Opt(match(
            '^',
            INT_NUMBER
        ))

    )

    
export const UNIT_SUB_MULTIPLY =
    match(
        UNIT_SUB,
        Times.NoneOrMore(
            match(
                '*',
                UNIT_SUB
            )
        )
    )

export const UNIT =
    match(
        Times.Opt('`'),
        UNIT_SUB_MULTIPLY,
        Times.Opt(
            match(
                '/',
                UNIT_SUB_MULTIPLY
            )
        )
    )



export const DOLAR = match('$');
export const ET = match('&')  
export const BACK_SLASH = match('\\');

export const NON_SPECIAL_CONTENT = match(
    Times.NoneOrMore(NON_SPECIAL),
    NON_SPECIAL_M
)
export const MODULUS = match("%")

export const QUOTE = match('"')

export const LESS_THAN = match("<")
export const MORE_THAN = match(">")

export const GRAVIS = match("`")

export const OPEN_PAREN = match("(")
export const CLOSE_PAREN = match(")")


export const CLASS_IDENTIFIER = match(
    BIG_LETTER,
    Times.Opt(
        match(
            Times.NoneOrMore(
                NON_SPECIAL
            ),
            UNIT_START
        )
    ) 
)
export const IDENTIFIER = match(
    UNIT_CHARS,
    Times.Opt(
        match(
            Times.NoneOrMore(
                NON_SPECIAL
            ),
            UNIT_START
        )
    )    
)

export const FLAG = match(
    UNIT_CHARS,
    Times.Opt(
        match(
            Times.NoneOrMore(
                NON_SPECIAL_F
            ),
            UNIT_START
        )
    ),
    "-" 
)

export const SEMICOLON = match(";")
export const COLON = match(":")
export const COMMA = match(",")

export const SET = match("=")

export const DOT = match(".")

export const FROM_LEFT = match("->")
export const FROM_RIGHT = match("<-")
export const FROM_JOIN = match("<->")

export const FROM_STATIC = match('::')

