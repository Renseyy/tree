/**
 * Transitions
 * Przekształenie pozwalają nam automatycznie tworzyć tokeny bazując na tych już istniejących
 * NP
 * AUTO_SEMICOLON: ~operator + NL -> SEMICOLON //Automatyczne wyznaczanie nowej lini
 * CALL: referentable + OPEN_PAREN -> CALL_FUNCTION
 */


// const operators = {
// 	'STATIC_MEMBER': {
// 		precedence: 1,
// 		assoc: false
// 	},
// 	'SCOPE_PATH': {
// 		precedence: 1,
// 		assoc: false
// 	},
// 	'POSTFIX_INCREMENT': {
// 		precedence: 2,
// 		assoc: false
// 	},
// 	'POSTFIX_DECREMENT': {
// 		precedence: 2,
// 		assoc: false
// 	},
// 	'LOGICAL_NOT': {
// 		precedence: 2,
// 		assoc: false
// 	},
// 	'BITWISE_NOT': {
// 		precedence: 2,
// 		assoc: false
// 	},
// 	'MEMBER_OF': {
// 		precedence: 2,
// 		assoc: false
// 	},
// 	'GET_FROM': {
// 		precedence: 2,
// 		assoc: false
// 	},
// 	'GET_TO': {
// 		precedence: 2,
// 		assoc: false
// 	},
// 	'ADD': {
// 		assoc: false,
// 		precedence: 6
		
// 	},
// 	'SET': {
// 		assoc: true,
// 		precedence: 16
// 	},
// 	'OPEN_PAREN': {
// 		precedence: 3
// 	},
// 	'/': {
// 		precedence: 3
// 	},
// 	'^': {
// 		assoc: true,
// 		precedence: 2
// 	},
// 	'COMMA': {
// 		assoc: false,
// 		precedence: 17
// 	},
// 	'MULTIPLY': {
// 		assoc: false,
// 		precedence: 5
// 	},
// 	'NEW_LINE': {
// 		assoc: false,
// 		precedence: 24
// 	},
// 	'CALL_FUNCTION': {
// 		assoc: false, 
// 		precedence: 0
// 	},
// 	'IDENTIFIER': {
// 		submatch: true,
// 		match: {
// 			'var': {
// 				precedence: 15, 
// 				assoc: false
// 			}
// 		}
// 	}
// }

const operators = {
	"MULTIPLY": {
		arguments: 2, 
		precedence: 3
	},
	"ADD": {
		arguments: 2, 
		precedence: 2
	},
	"SET": {
		arguments: 2, 
		precedence: 1
	},
	"IDENTIFIER": {
		submatch: true,
		match: {
			'var': {
				arguments: 1, 
				precedence: 3
			},
			'const': {
				precedence: 3
			},
			'unsigned': {
				precedence: 3
			},
			'positive': {
				precedence: 3
			},
			'nullalbe': {
				precedence: 3
			}
		}
	},
	"CALL_FUNCTION": {
		precedence: 20
	},
	"SEMICOLON": {
		precedence: 0
	}
}
// const 

const openings = {
	'OPEN_PAREN': {
		openingToken: 'OPEN_PAREN',
		closingToken: 'CLOSE_PAREN'
	},
	'START_LINE': {
		openingToken: 'START_LINE',
		closingToken: 'SEMICOLON',
		terminator: '|LINE_TERMINATOR|'
	},
	'<SOF>': {
		openingToken: '<SOF>',
		closingToken: '<EOF>'
	}
}
const closings = {
	'CLOSE_PAREN' : true,
	'<EOF>': true
}

function isNotOperator(token){
	return !operators[token.token] || (operators[token.token] && operators[token.token].submatch && !operators[token.token].match[token.content.match])
}

/**
 * @done Empty lines are not ignored
 * @error Two function run in one row is allowed, this must be disabled
 */
export function yard(ctx, tokens, openingInfo = null, depth = 0){
	let stack = [], output = []
	let lastToken = null
	if(openingInfo == null){
		openingInfo = openings['<SOF>']
	}
	stack.push(tokens.shift())
	while(tokens.length > 0){
		let token = tokens.shift()
		
		// Transformations
		if(token.token == 'NEW_LINE' && (lastToken == null || lastToken.token == 'SEMICOLON')){
			continue
		}
		if(lastToken){
			
			if(lastToken.token == 'IDENTIFIER' && token.token == 'OPEN_PAREN'){
				tokens.unshift(token);
				token = {
					token: 'CALL_FUNCTION',
					id: null,
					isOperator: true
				}
			}
			if(isNotOperator(lastToken) && token.token == 'NEW_LINE'){
				token = {
					token: 'SEMICOLON',
					id: null,
					isOperator: true
				}
			}
		}
		// if(token.token == '<EOF>'){
		// 	token = {
		// 		token: 'SEMICOLON',
		// 		id: null,
		// 		isOperator: true
		// 	}
		// }
		if(token.token == "WS")
			continue
		lastToken = token;
		if(operators[token.token] && (!operators[token.token].submatch || operators[token.token].match[token.content.match])){
			let operator = {...operators[token.token]}
			operator.isOperator = true
			operator.token = token.token
			if(operator.submatch){
				operator = operator.match[token.content.match]
				operator.token = token.content.match.toUpperCase()
			}
			const precedence = operator.precedence
			const rightAssociative = operator.rightAssociative
			while(true){
				const item = stack.pop()
				if(!item) break
				if(openingInfo && !item.token == openingInfo.closingToken){
					// return to stack
					stack.push(item)
					break
				}
				if(item.precedence > precedence){
					output.push(item)
					continue
				}
				if(item.precedence == precedence && rightAssociative){
					output.push(item)
					continue
				}
				stack.push(item)
				break
			}
			stack.push(operator)
		}else if(openings[token.token]){
			tokens.unshift(token)
			output.push(...yard(ctx, tokens, openings[token.token], depth+1))
			continue
		}else if(openingInfo && token.token == openingInfo.closingToken){
			let top = stack.pop()
			while(top.token != openingInfo.openingToken){
				output.push(top)
				top = stack.pop()
			}
			if(stack.length > 0){
				throw new Error("Błąd składni 2")
			}
			return output
		}else{
			output.push(token)
			continue	
		}	
	}
	console.log(openingInfo, stack)
	throw new Error(`Błąd składni:\n głębokość: ${depth}\n otwarcie: ${openingInfo.openingToken}`)	
}
export function crazyPrinter(tokens){
    let output = ""
    for(const token of tokens){
		if(token.token === 'WS') continue
		if(token.isOperator || token.precedence !== undefined){
            output+=`[${token.token}] `
        }else{
            output+=`${token.content.match}[${token.token}] `
        }
    }
    console.log(output)
}
