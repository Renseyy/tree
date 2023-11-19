import { List } from "../lib/lexer.js";
import { main } from './modes/main.js'
import { typeFragment } from "./modes/typeFragment.js";

export const Modes = new List()
Modes.main = main;
Modes.typeFragment = typeFragment
