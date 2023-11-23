const operators = {
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
	}
}

export function yard(tokens){
	const stack = []
	stack.last = () => {
		return stack[stack.length - 1 ]
	}
	const output = []
	while(tokens.length > 0){
		const token = tokens.shift()
		if(!operators[token.token] && token.token != 'OPEN_PAREN' && token.token != 'CLOSE_PAREN'){
			output.unshift(token)
		}else{
			if(token.token == 'OPEN_PAREN'){
				stack.push(token)
			}else if(token.token == 'CLOSE_PAREN'){
				let top = stack.pop()
				while(top.token != 'OPEN_PAREN'){
					output.unshift(top)
					top = stack.pop()
				}
				
			}else{
				const operator = operators[token.token]
				const precedence = operator.precedence
				const assoc = !operator.assoc
				operator.token = token.token
				while(
						stack.last() && (
							stack.last().data != ')' && (
								(
									assoc && precedence >= stack.last().precedence
								) || (
									!assoc && precedence > stack.last().precedence
								)
							)
						)
					){
					output.unshift(stack.pop())
				}
				stack.push(operator)	
			}
			
		}
	}
	// while(stack.length > 0){
	// 	output.unshift(stack.pop())
	// }
	return output.reverse()
}
export function crazyPrinter(tokens){
    let output = ""
    for(const token of tokens){
        if(token.assoc !== undefined){
            output+=`[${token.token}] `
        }else{
            output+=`${token.content.match}[${token.token}]`
        }
    }
    console.log(output)
}