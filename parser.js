import { getNextToken, assertTokenAndReadValue } from "./tokenizer.js";

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
        // if(assertTokenAndReadValue(lookahead, 'EOF')) break;
        try {
            assertTokenAndReadValue(lookahead, 'EOF');
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
    // assertTokenAndReadValue(nextLookahead, ';', 'Linebreak', 'EOF');
    assertTokenAndReadValue(nextLookahead, ';', 'EOF');

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
        case 'JSXSelfClosing':
            return jsxExpression(lookahead);
    }
}

function jsxExpression(lookahead) {
    const openingLookahead = lookahead;
    const openingElement = jsxOpeningElement(openingLookahead);
    let content = null;
    let closingElement = null;
    if (!openingElement.selfClosing) {
        content = jsxContent(getNextToken());
        closingElement = jsxClosingElement(getNextToken());
    }
    console.log('openingElement', openingElement);

    return {
        type: 'JSXExpression',
        openingElement,
        closingElement,
        value: content,
    };
}

function jsxOpeningElement(lookahead) {
    const token = assertTokenAndReadValue(lookahead, 'JSXOpening', 'JSXSelfClosing');
    return {
        type: 'JSXOpeningElement',
        value: token,
        selfClosing: lookahead.type === 'JSXSelfClosing',
    };
}

function jsxContent(lookahead) {
    const token = assertTokenAndReadValue(lookahead, 'JSXText');
    return {
        type: 'JSXText',
        value: token,
    };
}

function jsxClosingElement(lookahead) {
    const token = assertTokenAndReadValue(lookahead, 'JSXClosing');
    return {
        type: 'JSXClosingElement',
        value: token,
    }
}

function numericLiteral(lookahead) {
    const token = assertTokenAndReadValue(lookahead, 'Number');
    return {
        type: 'NumericLiteral',
        value: Number(token),
    };
}

function stringLiteral(lookahead) {
    const token = assertTokenAndReadValue(lookahead, 'String');
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
