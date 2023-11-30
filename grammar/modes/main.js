import { match, Times, oneOf, noneOf, or , List, after} from "../../lib/lexer.js";
import { HEX_DIGIT, NUMBER, UNIT, MODULUS, UNIT_CHARS, IDENTIFIER, DOLAR, NON_SPECIAL_CONTENT, ET, BACK_SLASH, QUOTE, LESS_THAN, OPEN_PAREN, CLOSE_PAREN, SEMICOLON, SET, COMMA, DOT, FROM_LEFT, FROM_STATIC, MORE_THAN  } from "../fragments.js"
export const main = new List();

const WS = Times.OnceOrMore(
	oneOf(" \t")
)

main.FROM_LEFT = FROM_LEFT
 

main.STATIC_MEMBER = match('::')
main.SCOPE_PATH = BACK_SLASH
main.LOGICAL_NOT = match('!')
main.BITWISE_NOT = match('~')
main.MEMBER_OF = match('.')
main.NEW_LINE = match('\n')

// COMMENT
main.COMMENT =
    after(
        match(
            "/*",
            Times.NoneOrMore(
                or(
                    "\\/*",
                    noneOf("")
                )
            ),
            "*/"
        ),
        (e) => {
            e.skip()
        }
    )
    
main.SINGLE_LINE_COMMENT = 
    match(
        '//',
        Times.NoneOrMore(
            noneOf("")
        ),
        '\n'
    )
main.WS = WS

main.NL = 
    Times.OnceOrMore(
        oneOf("\n\r")
    )

/** Symbols */
main.OPEN_PAREN = OPEN_PAREN
main.CLOSE_PAREN = CLOSE_PAREN

//? COLOR
main.HEX_COLOR =
    match(
        "#",
        Times.OnceOrMore(HEX_DIGIT)
    )

//? CONSTS - NUMBERS



main.DIMENSION = 
    match(
        NUMBER,
        UNIT
    )

main.NUMBER = NUMBER


main.IDENTIFIER = IDENTIFIER

main.STATE = 
        match(":")
        
main.VARIABLE = 
    match(
        DOLAR,
        NON_SPECIAL_CONTENT
    )

main.REFERENCE = 
    match(
        ET,
        NON_SPECIAL_CONTENT
    )
    
main.CLASS = 
    match(
        DOT,
        NON_SPECIAL_CONTENT
    )

// main.WORKSPACE = 
//     match(
//         BACK_SLASH,
//         NON_SPECIAL_CONTENT
//     )

main.FROM_STATIC = FROM_STATIC

// Simple sign
main.REFERENCE_FROM = ET
main.VARIABLE_FROM = DOLAR

main.SIMPLE_STRING =
    match(
        "'",
        Times.NoneOrMore(
            or(
                "\\'",
                noneOf("'") 
            )
        ),
        "'"
    )

// ROZPOCZĘCIE INNYCH TRYBÓW
main.FRAGMENT_START = 
    after(
        match("{"),
        (e) => {
            e.pushMode('main')
        }
    )

main.ADVANCED_STRING_START = 
    after(
        QUOTE,
        (e) => {
            e.pushMode('advancedString')
        }
    )

main.PAREN_FRAGMENT_START = 
    after(
        OPEN_PAREN,
        (e) => {
            e.pushMode('parenFragment')
        }
    )

// Wyjście z obecnego trybu
main.FRAGMENT_END = 
    after(
        match("}"),
        (e) => {
            e.popMode()
        }
    )

main.SEMICOLON = SEMICOLON

main.EQUALS = match("==")
main.NOT_EQUAL = match("!=")
main.IDENTITY = match("===")
main.NOT_IDNETICAL = match("!==")
main.SET = SET

main.COMMA = COMMA
main.DOT = DOT

//main.CHILD_SELECTOR = MORE_THAN;

// OPERATORS

main.POWER = match('**')
main.ROOT = match('//')

main.MULTIPLY = match('*')
main.DIVIDE = match('/')
main.MODULUS = match('%')
main.INT_DIVIDE = or('/%', '%/')

main.UN_INCREMENT = match('++')
main.UN_DECREMENT = match('--')
main.UN_AVRAGE = match('~~')

main.ADD = match('+')
main.MINUS = match('-')

main.AVR = match('~')
// Logic operators
main.NOT = match('!')

// SHIFTS
main.LEFT_SHIFT = match('<<')

main.UNSIGNED_RIGHT_SHIFT = match('>>>')
main.RIGHT_SHIFT = match('>>')


main.LESS_OR_EQUAL = match('<=')
main.MORE_OR_EQUAL = match('>=')
main.LESS = match('<')
main.MORE = match('>')


