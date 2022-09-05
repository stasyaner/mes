import { getNextToken, readToken } from "./tokenizer.js";

export function parse() {
    // const lookahead = getNextToken(input);
    // return program(lookahead);
    return program();
}

// export function program(token) {
export function program() {
    return {
        type: 'File',
        content: statementList(),
    };
}

// function statementList(lookahead) {
function statementList() {
    let lookahead = getNextToken();
    const list = [statement(lookahead)];

    while(lookahead = getNextToken()) {
        // if(readToken(lookahead, 'EOF')) break;
        try {
            readToken(lookahead, 'EOF');
            break;
        } catch(e) {}

        list.push(statement(lookahead));
    }

    return {
        type: 'StatementList',
        value: list,
    };
}

function statement(lookahead) {
    return expressionStatement(lookahead);
}

function expressionStatement(lookahead) {
    const value = expression(lookahead);

    const nextLookahead = getNextToken();
    console.log('lookahead', lookahead, 'nextLookahead', nextLookahead);
    // readToken(nextLookahead, ';', 'Linebreak', 'EOF');
    readToken(nextLookahead, ';', 'EOF');

    return {
        type: 'ExpressionStatement',
        value,
    };
}

function expression(lookahead) {
    switch(lookahead.type) {
        case 'Number':
        case 'String':
            return literal(lookahead);
        case 'JSXOpening':
            return jsxExpression(lookahead);
    }
}

function jsxExpression(lookahead) {
    const openingLookahed = lookahead;
    const contentLookahead = getNextToken();
    const closingLookahead = getNextToken();

    return {
        type: 'JSXExpression',
        openingElement: jsxOpeningElement(openingLookahed),
        closingElement: jsxClosingElement(closingLookahead),
        value: jsxContent(contentLookahead),
    };
}

function jsxOpeningElement(lookahead) {
    const token = readToken(lookahead, 'JSXOpening');
    return {
        type: 'JSXOpeningElement',
        value: token,
    };
}

function jsxContent(lookahead) {
    const token = readToken(lookahead, 'JSXText');
    return {
        type: 'JSXText',
        value: token,
    };
}

function jsxClosingElement(lookahead) {
    const token = readToken(lookahead, 'JSXClosing');
    return {
        type: 'JSXClosingElement',
        value: token,
    }
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
