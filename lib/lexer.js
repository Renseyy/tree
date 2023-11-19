
function _matchString(text, literal){
    if(text.length < literal.length) return null;
    if(text.substring(0,literal.length) == literal) return literal;
    return null
}

export function match(...args){
    return (text, x)=>{
        if(text.length == 0) return null
        let result = [];
        let done = 0
        let previousArgument = null;
        for(const arg of args){
            let res = null;
            if( previousArgument && previousArgument.isMutable && result.length > 0){
                let previousResult = result.pop();
                for(let i = previousResult.length; i >= 0; i--){
                    const previousText = previousResult.substring(0, i);
                    const nextText = previousResult.substring(i) + text;
                    const pRes = previousArgument(previousText);
                    if(pRes === null){
                        return null;
                    }
                    let nRes = null;
                    if(typeof arg == "string"){
                        nRes = _matchString(nextText, arg);
                    }else{
                        nRes = arg(nextText);
                    }
                    if(nRes !== null){
                        result.push(previousText)
                        res = nRes;
                        break;
                    }
                }
                if(res === null) return null;
                done++
            }else{
                if(typeof arg == "string"){
                    res = _matchString(text, arg);
                    if(res === null) return null;
                }else{
                    res = arg(text);
                    if(res === null) return null;
                }
                done++
            }
            result.push(res);
            text = text.substring(res.length);
            previousArgument = arg;
        }
        return done == args.length ? result.join("") : null;
    }
}
export function oneOf(rule){
    return (text)=>{
        if(!text) return null
        let check = text[0]
        for(let i = 0; i < rule.length; i++){
            const char = rule[i];
            const mayBeRange = i < rule.length - 2;
            if(mayBeRange && rule[i+1] == "-" && rule[i+2] > char){
                if(check <= rule[i+2] && check >= char){
                    return check;
                }
                i += 2;
            }else if(char == check){
                return check;
            }
        }
        return null
    }
}
export function or(...args){
    return (text) => {
        for(const arg of args){
            const fun = typeof arg == "string" ? match(arg) : arg
            let res = fun(text);
            if(res !== null) return res;
        }
        return null
    } 
}

export function noneOf(rule){
    return (text)=>{
        let check = text[0]
        if(text.length == 0) return null
        for(let i = 0; i < rule.length; i++){
            const char = rule[i];
            const mayBeRange = i < rule.length - 2;
            if(mayBeRange && rule[i+1] == "-" && rule[i+2] > char){
                if(check <= rule[i+2] && check >= check){
                    return null;
                }
                i += 2;
            }else if(char == check){
                return null;
            }
        }
        return check
    }
}
function startWith(text, rules){
    for(const rule of rules){
        if(text.length < rule.length) break;
        if(text.substring(0, rule.length) == rule) return true;
    }
    return false
}
export function notStarting(rules){
    return (text)=>{
        let check = text[0]
        if(startWith(text, rules)) return null;
        return check
    }
}
class __TimesWrapper extends Function {
    constructor() {
      super('...args', 'return this._bound._call(...args)')
      this.isMutable = true;
      // Or without the spread/rest operator:
      // super('return this._bound._call.apply(this._bound, arguments)')
      this._bound = this.bind(this)
  
      return this._bound
    }
    
    __run(text){
        let fun;
        if(typeof this.what == "string"){
            fun = match(this.what);
        }else fun = this.what;
        //Determine how many times run
        if(this.times == "None"){
            return fun(text) === null ? "" : null;
        }else if(this.times == "NoneOrOnce"){
            const r = fun(text);
            return r ? r : "";
        }else if(this.times == "NoneOrMore"){
            //collect max run of fun
            let result = ""
            while(true){
                if(text == "") break;
                let res = fun(text);
                if(res === null) break;
                result += res;
                text = text.substring(res.length)
            }
            return result;
        }else if(this.times == "OnceOrMore"){
            //collect max run of fun
            let result = ""
            while(true){
                if(text == "") break;
                let res = fun(text);
                if(res === null) break;
                result += res;
                text = text.substring(res.length)
            }
            return result != "" ? result : null;
        }
    }
    _call(text){
        this.text = text;
        return this.__run(text);
    } 
}
function _TimesWrapper(what, times){
    const wrap = new __TimesWrapper();
    wrap.what = what;
    wrap.times = times;
    wrap.isMutable = true;
    return wrap;
}


/**
 * NONE = 0,
    NONE_OR_ONCE = 1,
    NONE_OR_MORE_MIN = 2,
    NONE_OR_MORE_MAX = 3,
    ONCE = 4 ,
    ONCE_OR_MORE_MIN = 5,
    ONCE_OR_MORE_MAX = 6
 */
export const Times = {
    None: (w) => _TimesWrapper(w, "None"),
    NoneOrOnce: (w) => _TimesWrapper(w, "NoneOrOnce"),
    Opt: (w) => _TimesWrapper(w, "NoneOrOnce"),
    NoneOrMore: (w) => _TimesWrapper(w, "NoneOrMore"),
    Once: (w) => _TimesWrapper(w, "Once"),
    OnceOrMore: (w) => _TimesWrapper(w, "OnceOrMore"),

}

export class List{
    constructor(){
        return new Proxy(this, {
            container: [],
            get(target, name){
                if(typeof name == "symbol") return this[name].bind(this)
                if(+name == name){
                    return this.container[name]?.value || undefined;
                }else{
                    for(const cont of this.container){
                        if(cont.name === name) return cont.value; 
                    }
                    return undefined;
                }
            },
            set(target, name, value){
                let found = false;
                for(const cont of this.container){
                    if(cont.name !== name) continue;
                    found = true;
                    cont.value = value; 
                    break;
                }
                if(found) return;
                this.container.push({
                    name: name, 
                    value: value
                })
                return true;
            },
            keys(){
                return this.container.map(e => e.name)
            },
            *[Symbol.iterator](){
                for(const cont of this.container){
                    yield {
                        key: cont.name,
                        value: cont.value
                    }
                }
            }
        })
    }
}

export class TokenList{
    constructor(){
        return new Proxy(this, {
            container: [],
            get(target, name){
                if(typeof name == "symbol") return this[name].bind(this)
                if(+name == name){
                    return this.container[name]?.value || undefined;
                }else{
                    for(const cont of this.container){
                        if(cont.name === name) return cont.value; 
                    }
                    return undefined;
                }
            },
            set(target, name, value){
                let found = false;
                for(const cont of this.container){
                    if(cont.name !== name) continue;
                    found = true;
                    cont.value = value; 
                    break;
                }
                if(found) return;
                this.container.push({
                    name: name, 
                    value: value
                })
                return true;
            },
            keys(){
                return this.container.map(e => e.name)
            },
            *[Symbol.iterator](){
                for(const cont of this.container){
                    yield {
                        key: cont.name,
                        value: cont.value
                    }
                }
            }
        })
    }
}

class __After extends Function {
    constructor(fun, after) {
        super('...args', 'return this._bound._call(...args)')
        // Or without the spread/rest operator:
        // super('return this._bound._call.apply(this._bound, arguments)')
        this._bound = this.bind(this)
        this._bound.__run = fun
        this._bound.afterCallable = true
        this._bound.__after = after
        return this._bound
    }
    after(wrapper){
        return this.__after(wrapper)
    }
    _call(...args){
        return this.__run(...args)
    } 
}

export function after(rule, after = ()=>{}){
    return new __After(rule, after);
}