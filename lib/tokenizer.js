/**
 * TO IMPROVE
 */

// MAIN PROGRAM
function printError(program, i, mode){
    let bottomText = ""
    for(let x = 0; x < i; x++){
        bottomText+=" "
    }
    bottomText+= "^"
    console.error(`Unexpected "${program[i]}" in mode ${mode}`)
    console.error();
    const n = program.indexOf('\n')
    console.error(program.substring(0, (i + 10 - n)> 0 ? n : i + 10))
    console.error(bottomText)
    process.exit(-1)
}

export class Tokenizer{
    tokens = []
    modeStack = []
    modes = null
    constructor(modes){
        this.modes = modes
        this.modeStack.push('main')
        this.modeStack.last = () => {
            return this.modeStack[this.modeStack.length - 1];
        }
        this.modeStack.push('main')
    }
    #skip = false;
    afterContextWrapper = {
        skip: () => {
            this.#skip = true;
        },
        pushMode: (name) => {
            this.modeStack.push(name)
        },
        popMode: () => {
            this.modeStack.pop()
        }
    }
    tokenize(program, addEOF = true){
        if(addEOF){
            this.tokens.push({
                token: "<SOF>"
            })
        }
        this.program = program
        for(let i = 0; i < program.length;){
            let done = false;
            const actualMode = this.modes[this.modeStack.last()];
            if(!actualMode){
                console.error(`Tryb: ${this.modeStack.last()} nie jest zdefiniowany`)
            }
            for(const {key, value} of actualMode){
                const res = value(program.substring(i));
                if(res === null) continue
                i += res.match.length;
                if(value.afterCallable){
                    value.after(this.afterContextWrapper)
                }
                if(!this.#skip){
                    this.tokens.push({
                        token: key, 
                        content: res,
                        position: i - res.match.length,
                        attributes : res.attributes || []
                        // mode: this.modeStack.last(),
                        // depth: this.modeStack.length - 1
                    })
                }
                this.#skip = false;
                done = true;
                break;
            }
           
            if(!done){
                printError(program, i, this.modeStack.last())
            }
        }
        if(addEOF){
            const lastToken = this.tokens.pop()
            if(lastToken.token != 'NEW_LINE')
                this.tokens.push(lastToken)
            this.tokens.push({
                token: "<EOF>"
            })
        }
        const tokensWithId = []
        for(let i = 0; i<this.tokens.length; i++){
            const t = this.tokens[i];
            t.id = i
            tokensWithId.push(t)
        }
        return tokensWithId;
    }
    asRoar(){
        let allTokens = new Set();
        for(const {value} of this.modes){
            for(const {key} of value){
                allTokens.add(key)
            }
        }
        let i = 0;
        let map = {};
        let arr = []
        for(const token of allTokens){
            map[token] = i;
            arr.push(token)
            i++;
        }
        this.header = {
            map: map,
            array: arr
        }
        // Describe to xytes
        const xytes = [];
        const indexes = [];
        let index = 0;
        for(const token of this.tokens){
            xytes.push(map[token.token]);
            index += token.content.length
            indexes.push(index);
        }
        return {
            descriptor: this.header,
            tokens: xytes,
            borders: indexes,
            text: this.program
        }
    }
    
}