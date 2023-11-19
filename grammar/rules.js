import { match, Times, oneOf, noneOf, or , List} from "../lib/lexer.js";
import { SEMICOLON } from "./fragments";
const rules = new List();

rules.expression = 

rules.line = match(
    rules.expression, 
    or(SEMICOLON, EOL, EOF)
)