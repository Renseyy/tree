import { Tokenizer } from "./lib/tokenizer.js"
import { Modes } from "./grammar/modes.js"
import fs from 'fs/promises'
import { Times, match, oneOf, or } from "./lib/lexer.js"
import { crazyPrinter, yard } from "./lib/parser.js"
// //const code = await Bun.file("code.csl").text()
const code = await fs.readFile(new URL('./code.csl', import.meta.url), "utf-8")
const tokenizer = new Tokenizer(Modes)
const tokens = tokenizer.tokenize(code)

crazyPrinter(yard(tokens))

