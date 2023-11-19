import { match, Times, oneOf, noneOf, or , List, after} from "../../lib/lexer.js";
import { HEX_DIGIT, NUMBER, UNIT, MODULUS, IDENTIFIER, NON_SPECIAL, DOLAR, NON_SPECIAL_CONTENT, ET, BACK_SLASH, QUOTE, LESS_THAN, OPEN_PAREN, CLOSE_PAREN, MORE_THAN, FLAG, INT_NUMBER  } from "../fragments.js"
import { main } from "./main.js";

export const typeFragment = new List();

//in Flags you can't use 

typeFragment.START_TYPE_FRAGMENT = main.START_TYPE_FRAGMENT;
typeFragment.COUNT = match(
    '/',
    INT_NUMBER
)
typeFragment.SIZE = INT_NUMBER;
typeFragment.DIV = match('/')
typeFragment.FLAG = FLAG
typeFragment.IDENTIFIER = IDENTIFIER;

/** ADD MORE */

//!END
typeFragment.END_TYPE_FRAGMENT = after(MORE_THAN, (e) => e.popMode())