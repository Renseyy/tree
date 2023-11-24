/**
 * Transitions
 * Przekształenie pozwalają nam automatycznie tworzyć tokeny bazując na tych już istniejących
 * NP
 * AUTO_SEMICOLON: ~operator + NL -> SEMICOLON //Automatyczne wyznaczanie nowej lini
 * CALL: referentable + OPEN_PAREN -> CALL_FUNCTION
 */


const operators = {
	'STATIC_MEMBER': {
		precedence: 1,
		assoc: false
	},
	'SCOPE_PATH': {
		precedence: 1,
		assoc: false
	},
	'POSTFIX_INCREMENT': {
		precedence: 2,
		assoc: false
	},
	'POSTFIX_DECREMENT': {
		precedence: 2,
		assoc: false
	},
	'LOGICAL_NOT': {
		precedence: 2,
		assoc: false
	},
	'BITWISE_NOT': {
		precedence: 2,
		assoc: false
	},
	'MEMBER_OF': {
		precedence: 2,
		assoc: false
	},
	'GET_FROM': {
		precedence: 2,
		assoc: false
	},
	'GET_TO': {
		precedence: 2,
		assoc: false
	},
	'ADD': {
		assoc: false,
		precedence: 6
		
	},
	'SET': {
		assoc: true,
		precedence: 16
	},
	'OPEN_PAREN': {
		precedence: 3
	},
	'/': {
		precedence: 3
	},
	'^': {
		assoc: true,
		precedence: 2
	},
	'COMMA': {
		assoc: false,
		precedence: 17
	},
	'MULTIPLY': {
		assoc: false,
		precedence: 5
	},
	'NEW_LINE': {
		assoc: false,
		precedence: 24
	},
	'CALL_FUNCTION': {
		assoc: false, 
		precedence: 0
	}
}

// const 

const openings = {
	'OPEN_PAREN': {
		openingToken: 'OPEN_PAREN',
		closingToken: 'CLOSE_PAREN'
	}
}

export function yard(ctx, tokens, stack = [], output = [], openingInfo = null){
	//console.log(stack, tokens, openingInfo)
	let lastToken = null
	while(tokens.length > 0){
		let token = tokens.shift()
		if(lastToken){
			if(lastToken.token == 'IDENTIFIER' && token.token == 'OPEN_PAREN'){
				tokens.unshift(token);
				token = {
					token: 'CALL_FUNCTION',
					id: null
				}
			}
		}
		lastToken = token;
		if(!operators[token.token] && token.token != 'OPEN_PAREN' && token.token != 'CLOSE_PAREN'){
			output.push(token)
		}else{
			if(openings[token.token]){
				stack.push(token)
				yard(ctx, tokens, stack, output, openings[token.token])

			}else if(openingInfo && token.token == openingInfo.closingToken){
				
				let top = stack.pop()
				
				while(top.token != openingInfo.openingToken){
					output.push(top)
					top = stack.pop()
				}
			}else{
				const operator = operators[token.token]
				const precedence = operator.precedence
				const assoc = !operator.assoc
				operator.token = token.token
				while(
						stack[stack.length - 1 ] && (
							[stack.length - 1 ].data != ')' && (
								(
									assoc && precedence >= [stack.length - 1 ].precedence
								) || (
									!assoc && precedence > [stack.length - 1 ].precedence
								)
							)
						)
					){
					output.push(stack.pop())
				}
				stack.push(operator)	
			}
			
		}
	}
	if(openingInfo == null){
		while(stack.length > 0){
			output.push(stack.pop())
		}
	}
	return output
}
export function crazyPrinter(tokens){
    let output = ""
    for(const token of tokens){
        //console.log(token)
		if(token.token === 'WS') continue
		if(token.assoc !== undefined){
            output+=`[${token.token}] `
        }else{
            output+=`${token.content.match}[${token.token}] `
        }
    }
    console.log(output)
}