import { getNextToken, readToken } from "./tokenizer.js";

export function parse(input) {
    // const lookahead = getNextToken(input);
    // return program(lookahead);
    return program(input);
}

// export function program(token) {
export function program(input) {
    return {
        type: 'File',
        content: statementList(input),
    };
}

// function statementList(lookahead) {
function statementList(input) {
    let lookahead = getNextToken(input);
    const list = [statement(lookahead, input)];

    while(lookahead = getNextToken(input)) {
        // if(readToken(lookahead, 'EOF')) break;
        try {
            readToken(lookahead, 'EOF');
            break;
        } catch(e) {}

        list.push(statement(lookahead, input));
    }

    return {
        type: 'StatementList',
        value: list,
    };
}

function statement(lookahead, input) {
    return expressionStatement(lookahead, input);
}

function expressionStatement(lookahead, input) {
    const value = expression(lookahead);

    const nextLookahead = getNextToken(input);
    console.log('lookahead', lookahead, 'nextLookahead', nextLookahead);
    // readToken(nextLookahead, ';', 'Linebreak', 'EOF');
    readToken(nextLookahead, ';', 'EOF');

    return {
        type: 'ExpressionStatement',
        value,
    };
}

function expression(lookahead) {
    return literal(lookahead);
}

function numericLiteral(lookahead) {
    const token = readToken(lookahead, 'Number');
    return {
        type: 'NumericLiteral',
        value: Number(token),
    };
}

function stringLiteral(lookahead) {
    const token = readToken(lookahead, 'String');
    return {
        type: 'StringLiteral',
        value: token,
    };
}

function literal(lookahead) {
    switch(lookahead.type) {
        case 'Number':
            return numericLiteral(lookahead);
        case 'String':
            return stringLiteral(lookahead);
    }
}
