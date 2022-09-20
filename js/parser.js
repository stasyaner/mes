import { getNextToken, assertTokenType } from "./tokenizer.js";

let lookaheadToken = null;
function lookahead() {
    lookaheadToken = getNextToken();
}

export function parse() {
    lookahead();
    return program();
}

export function program() {
    return {
        type: 'File',
        content: statementList(),
    };
}

function statementList() {
    const list = [statement()];

    // readTokenAndLookahead('EOF');

    while(lookaheadToken.type !== 'EOF') {
        list.push(statement());
    }

    return list;
}

function statement() {
    return expressionStatement();
}

function expressionStatement() {
    const value = expression();

    // readTokenAndLookahead(';', 'Linebreak', 'EOF');
    readTokenAndLookahead(';', 'EOF');

    return {
        type: 'ExpressionStatement',
        value,
    };
}

function expression() {
    switch(lookaheadToken.type) {
        case 'JSXOpening':
        case 'JSXSelfClosing':
            return jsxExpression();
        default:
            return relationalExpression();
    }
}

function relationalExpression() {
    return binaryExpressionWrapper(literal, 'Relational');
}

function binaryExpressionWrapper(leftExpression, operatorToken) {
    let left = leftExpression();

    while(lookaheadToken.type === operatorToken) {
        const { value: operator } = readTokenAndLookahead(operatorToken);
        const right = literal();

        left = {
            type: 'BinaryExpression',
            left,
            right,
            operator,
        };
    }

    return left;
}

function jsxExpression() {
    const openingElement = jsxOpeningElement();
    let content = null;
    let closingElement = null;
    if (!openingElement.selfClosing) {
        content = jsxContent();
        closingElement = jsxClosingElement();
    }
    console.log('openingElement', openingElement);

    return {
        type: 'JSXExpression',
        openingElement,
        closingElement,
        value: content,
    };
}

function jsxOpeningElement() {
    const token = readTokenAndLookahead('JSXOpening', 'JSXSelfClosing');
    return {
        type: 'JSXOpeningElement',
        value: token.value,
        selfClosing: token.type === 'JSXSelfClosing',
    };
}

function jsxContent() {
    const token = readTokenAndLookahead('JSXText');
    return {
        type: 'JSXText',
        value: token,
    };
}

function jsxClosingElement() {
    const token = readTokenAndLookahead('JSXClosing');
    return {
        type: 'JSXClosingElement',
        value: token,
    }
}

function numericLiteral() {
    const token = readTokenAndLookahead('Number');
    return {
        type: 'NumericLiteral',
        value: Number(token.value),
    };
}

function stringLiteral() {
    const token = readTokenAndLookahead('String');
    return {
        type: 'StringLiteral',
        value: token.value,
    };
}

function literal() {
    switch(lookaheadToken.type) {
        case 'Number':
            return numericLiteral();
        case 'String':
            return stringLiteral();
    }
}

function readTokenAndLookahead(...acceptableTokenTypes) {
    const token = lookaheadToken;

    assertTokenType(token, acceptableTokenTypes);

    lookaheadToken = getNextToken();

    return token;
}
